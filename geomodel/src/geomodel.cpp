/*
 *
 * geomodel.c
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */

#include "postgres.h"
#include "fmgr.h"
#include "geomodel.h"
#include "gmserialized.h"
#include "geomodel_util.h"
#include "extension_dependency.h"
#include "Geometry3D/YkGeoModel.h"
#include "Geometry3D/YkWrapCGeoModel.h"
#include "Toolkit/YkLicenseRSA.h"
#include "../../postgis_config.h"
#include "lwgeom_sfcgal.h"
#include "../../sfcgal/lwgeom_sfcgal.h"
#include "../../libpgcommon/lwgeom_pg.h"
#include <stdio.h>
#include <string.h>

using namespace Yk;
#define YUKON_LIB_VERSION "1.0.1_lib"

PG_MODULE_MAGIC;

YkModelMaterialPtr m_pMaterail;
YkModelTexture *m_pTexture = NULL;

char buf[1024] = {0};

PG_FUNCTION_INFO_V1(geomodel_in);
PG_FUNCTION_INFO_V1(geomodel_out);
PG_FUNCTION_INFO_V1(geomodel_recv);
PG_FUNCTION_INFO_V1(geomodel_send);

PG_FUNCTION_INFO_V1(model_elem_in);
PG_FUNCTION_INFO_V1(model_elem_out);
PG_FUNCTION_INFO_V1(model_elem_recv);
PG_FUNCTION_INFO_V1(model_elem_send);

PG_FUNCTION_INFO_V1(yukon_lib_version);

// {{ make from tin 
PG_FUNCTION_INFO_V1(make_skeleton_from_tin);
PG_FUNCTION_INFO_V1(make_hash_from_string);
PG_FUNCTION_INFO_V1(make_default_material);
PG_FUNCTION_INFO_V1(make_geomodelshell);
// }}

extern "C" Datum geomodel_in(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_out(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_recv(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_send(PG_FUNCTION_ARGS);

extern "C" Datum model_elem_in(PG_FUNCTION_ARGS);
extern "C" Datum model_elem_out(PG_FUNCTION_ARGS);
extern "C" Datum model_elem_send(PG_FUNCTION_ARGS);
extern "C" Datum model_elem_recv(PG_FUNCTION_ARGS);

extern "C" Datum yukon_lib_version(PG_FUNCTION_ARGS);

// {{ make from tin 
extern "C" Datum make_skeleton_from_tin(PG_FUNCTION_ARGS);
extern "C" Datum make_hash_from_string(PG_FUNCTION_ARGS);
extern "C" Datum make_default_material(PG_FUNCTION_ARGS);
extern "C" Datum make_geomodelshell(PG_FUNCTION_ARGS);
// }}

Datum geomodel_in(PG_FUNCTION_ARGS)
{
  char *input = PG_GETARG_CSTRING(0);
  int len = strlen(input);
  if (len % 2 != 0)
  {
    len = len - 1;
  }

  char *data = palloc(len / 2 + 4);
  SET_VARSIZE(data, len / 2 + 4);
  for (int i = 0, j = 0; i < len; i += 2, j++)
  {
    data[j + 4] = Char2Hex(input + i);
  }
  PG_RETURN_POINTER(data);
  //PG_RETURN_NULL();
}

Datum geomodel_out(PG_FUNCTION_ARGS)
{
  varlena *buf = PG_GETARG_VARLENA_P(0);
  uint32_t buf_size = VARSIZE(buf);
  unsigned char dst[2] = {0};

  char *buf_data = VARDATA(buf);
  //申请两倍空间，存放字符
  char *result = palloc((buf_size - 4) * 2 + 1);
  memset(result, 0, (buf_size - 4) * 2 + 1);
  //依次将字符转换成字符串
  for (int i = 0; i < buf_size - 4; i++)
  {
    Hex2Char((unsigned char)buf_data[i], dst);
    result[i * 2 + 1] = dst[0];
    result[i * 2] = dst[1];
  }
  PG_RETURN_CSTRING(result);
}

bool geomodel_isvalid(GSERIALIZED *gs)
{
  //Tobe written by UGC Function
  //TODO
  //gs->bbox;
  return true;
}

bool geomodel_reset_flags(GSERIALIZED *gs)
{
  FLAGS_SET_Z(gs->gflags, true);
  FLAGS_SET_M(gs->gflags, false);
  FLAGS_SET_BBOX(gs->gflags, true);
  FLAGS_SET_GEODETIC(gs->gflags, true);
  //TODO
  //geomodel 或者 element 是否要设置具体的子类型，目前还剩余两个 bit 可用
}

GSERIALIZED *wrap_header_geomodeldata(char *pData, int nLen, YkBoundingBox& bbox)
{
  int size = 0;
  // size,srid,flags
  size += 8;
  // bbox
  size += 24;
  // data
  size += nLen;
  GSERIALIZED * result = (GSERIALIZED *)palloc(size);
  memset(result, 0, size);

  // 设置大小
  SET_VARSIZE(result, size);  
  // 设置 flags
 // geomodel_reset_flags(result);
  FLAGS_SET_Z(result->gflags, true);
  FLAGS_SET_M(result->gflags, false);
  FLAGS_SET_BBOX(result->gflags, true);
  FLAGS_SET_GEODETIC(result->gflags, true);

  gmserialized_set_bbox(result, bbox);  
  // 复制数据
  memcpy(((char *)result) + 32, pData, nLen);

  return result;
}

Datum geomodel_recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  int len = buf->len;
  int size = 0;
  int32 geom_typmod = -1; 

  if ((PG_NARGS() > 2) && (!PG_ARGISNULL(2)))
  {
    geom_typmod = PG_GETARG_INT32(2);
  }
  
  // is buf->data valid ?
  YkGeoModel *pGeoModel = LoadShell(buf->data, len);
  if (pGeoModel == NULL)
  {
    elog(NOTICE, "Load shell data failed!");
    return false;
  }
  YkBoundingBox bbox = pGeoModel->GetGeoBoundingBox();
  delete pGeoModel;
  pGeoModel = NULL;

  GSERIALIZED *geom = wrap_header_geomodeldata(buf->data, len, bbox);

  buf->cursor = buf->len;
  PG_RETURN_POINTER(geom);
}

/**
 * @brief geomodel 二进制输出函数
 * 
 * @return Datum 
 */
Datum geomodel_send(PG_FUNCTION_ARGS)
{
  varlena *buf = PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
  // 这里的 buf_size 是包含 size 字段的
  uint32_t buf_size = VARSIZE(buf);
  // 这里的数据没有包含 size 字段，只有 srid flags data（里边包含 bbox 数据） 数据
  char *buf_data = VARDATA(buf);
  //这里分配大小时要包含 size 的大小，4个字节，
  char *result = palloc(buf_size - 28);
  //拷贝具体的数据，不包括 srid,flags data,同时也不包括 bbox 数据
  memcpy(result + 4, buf_data + 28, buf_size - 32);
  //设置大小时，要包含 size 的大小
  SET_VARSIZE(result, buf_size - 28);
  PG_RETURN_POINTER(result);
}

/////////////////////////////////////////////////////////////////////

/**
 * @brief 字符串形式数据存入 ,未实现
 * 
 * @return Datum 
 */
Datum model_elem_in(PG_FUNCTION_ARGS)
{
  char *input = PG_GETARG_CSTRING(0);
  int len = strlen(input);
  if (len % 2 != 0)
  {
    len = len - 1;
  }

  char *data = palloc(len / 2 + 4);
  SET_VARSIZE(data, len / 2 + 4);
  for (int i = 0, j = 0; i < len; i += 2, j++)
  {
    data[j + 4] = Char2Hex(input + i);
  }
  PG_RETURN_POINTER(data);
}

/**
 * @brief 字符串形式数据输出，
 * 
 * @return Datum 
 */
Datum model_elem_out(PG_FUNCTION_ARGS)
{
  varlena *buf = PG_GETARG_VARLENA_P(0);
  uint32_t buf_size = VARSIZE(buf);
  unsigned char dst[2] = {0};

  char *buf_data = VARDATA(buf);
  //申请两倍空间，存放字符
  char *result = palloc((buf_size - 4) * 2 + 1);
  memset(result, 0, (buf_size - 4) * 2 + 1);
  //依次将字符转换成字符串
  for (int i = 0; i < buf_size - 4; i++)
  {
    Hex2Char((unsigned char)buf_data[i], dst);
    result[i * 2 + 1] = dst[0];
    result[i * 2] = dst[1];
  }
  PG_RETURN_CSTRING(result);
}

/**
 * @brief 二进制数据存入，
 * 
 * @note StringInfo 类型是一个变长数据存储类型
 * @return Datum 
 */
Datum model_elem_recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  char *data = (char *)palloc(buf->len + 4);
  memcpy(data + 4, buf->data, buf->len);
  SET_VARSIZE(data, buf->len + 4);
  buf->cursor = buf->len;
  PG_RETURN_POINTER(data);
}

/**
 * @brief 二进制数据取出，
 * 
 * @note varlena 类型是一个变长数据存储类型
 * @return Datum 
 */
Datum model_elem_send(PG_FUNCTION_ARGS)
{
  varlena *result = PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
  PG_RETURN_CSTRING(result);
}

/**
 * @brief 获取 geomodel 的包围盒
 * 
 * @param gms GSERIALIZED 数据
 * @param bbox 返回的包围盒
 * @return true 
 * @return false 
 * @note gms 的 data 开始是包围盒数据
 */
bool gmserialized_get_bbox(GSERIALIZED *gms, BOX3D *bbox)
{
  if (gms == NULL || bbox == NULL)
  {
	  return false;
  }
  /*
   * 由于 GSERIALIZED 中索引使用的  BOX 为 float 类型，
   * 为了提高精度，这里我们直接从模型数据中读取 BOX 并返回 
   */
  uint32_t size = VARSIZE((varlena *)gms);

  /* 这里我们减去 size(4) + srid(3) + gflag(1) + box(6*4) 个字节大小 */
  size -= 32;

  YkGeoModel *pGeoModel = LoadShell((char *)gms + 32, size);
  if (pGeoModel == NULL)
  {
	  elog(NOTICE, "Load shell data failed!");
	  return false;
  }
  YkBoundingBox ykbbox = pGeoModel->GetGeoBoundingBox();
  delete pGeoModel;
  pGeoModel = NULL;

  YkVector3d vecMax = ykbbox.GetMax();
  YkVector3d vecMin = ykbbox.GetMin();

  bbox->xmin = vecMin.x;
  bbox->xmax = vecMax.x;
  bbox->ymin = vecMin.y;
  bbox->ymax = vecMax.y;
  bbox->zmin = vecMin.z;
  bbox->zmax = vecMax.z;
  bbox->srid = 0;

  return true;
}

/**
 * @brief 用来计算 geomodel 的 boundary box
 * 
 * @param gms 这里需要在函数体内转换成 UGGeoModelPro 类型才可以计算 
 * @param len 传入数据的长度
 * @return true 
 * @return false 
 */
bool gmserialized_set_bbox(GSERIALIZED *gms, YkBoundingBox &bbox)
{
  //设置包围盒
  YkVector3d vecMax = bbox.GetMax();
  YkVector3d vecMin = bbox.GetMin();
  (*((float *)(gms->data) + 0)) = (YkFloat)vecMin.x;
  (*((float *)(gms->data) + 1)) = (YkFloat)vecMax.x;
  (*((float *)(gms->data) + 2)) = (YkFloat)vecMin.y;
  (*((float *)(gms->data) + 3)) = (YkFloat)vecMax.y;
  (*((float *)(gms->data) + 4)) = (YkFloat)vecMin.z;
  (*((float *)(gms->data) + 5)) = (YkFloat)vecMax.z;
  return true;
}

Datum yukon_lib_version(PG_FUNCTION_ARGS)
{
  char *ver = YUKON_LIB_VERSION;
	text *result = cstring2text(ver);
	PG_RETURN_TEXT_P(result);
}


// 从TIN 构造 Skeleton，返回 Skeleton的序列化流
// ST_MakeSkeletonFromTIN(geometry, cstring)
Datum make_skeleton_from_tin(PG_FUNCTION_ARGS)
{
  GSERIALIZED *input = PG_GETARG_GSERIALIZED_P(0);
  srid_t srid = gserialized_get_srid(input);
  //补充 ==> 准入条件
//  if(srid != 3857)
//  {
//    return 0;
//  }

  //骨架名字
  char *inputname1 = PG_GETARG_CSTRING(1);
  YkString strSkeletonName;
  strSkeletonName = strSkeletonName.FromMBString(inputname1, strlen(inputname1));

   //材质名字
  char *inputname2 = PG_GETARG_CSTRING(2);
  YkString strMaterialName;
  strMaterialName = strMaterialName.FromMBString(inputname2, strlen(inputname2));

  LWGEOM *geom = lwgeom_from_gserialized(input);

  //构造 pSkeleton 对象
  YkModelSkeleton *pSkeleton = new YkModelSkeleton();
  YkVertexDataPackage *pVertexDataPack = new YkVertexDataPackage();
  YkIndexPackage *pIndexData = new YkIndexPackage();

  LWCOLLECTION *col = lwgeom_as_lwcollection(geom);
  LWCOLLECTION *g1 = lwgeom_as_lwcollection(col->geoms[0]);
  LWTIN *item = lwgeom_as_lwtin(g1->geoms[0]);

  //数据填充
  pVertexDataPack->SetVertexNum(item->ngeoms * 3);
  pVertexDataPack->m_nVertexDimension = 3;
  pIndexData->m_enIndexType = IT_16BIT;
  pIndexData->SetIndexNum(pVertexDataPack->m_nVerticesCount);

  for (int i = 0; i < item->ngeoms; i++)
  {
    LWTRIANGLE *trig = item->geoms[i];
    POINTARRAY *arr = trig->points;
    YKASSERT(arr->npoints > 3 );

    POINT3DZ *pot = (POINT3DZ *)(arr->serialized_pointlist);
    
    for (int j = 0; j <3; j++)
    {
      pVertexDataPack->m_pVertices[i*9+j*3] = pot[j].x;
      pVertexDataPack->m_pVertices[i*9+j*3 + 1] = pot[j].y;
      pVertexDataPack->m_pVertices[i*9+j*3 + 2] = pot[j].z;
    }
  }
  lwgeom_free(geom);

  for (int i = 0; i < pVertexDataPack->m_nVerticesCount; i++)
  {
    pIndexData->m_pIndexes[i] = i;
  }

  pSkeleton->SetDataPackRef(pVertexDataPack, pIndexData);
  pSkeleton->SetMaterialName(strMaterialName);  
  pSkeleton->m_strName = strSkeletonName;

  char *pData = 0;
  int nSize = 0;
  SaveElement(pSkeleton, pData, nSize);
  delete pSkeleton;

  char* output = (void *)palloc(nSize + 4);
  memcpy(output + 4, (char *)pData, nSize);
  SET_VARSIZE(output, nSize + 4);

  PG_RETURN_POINTER(output);
}

// ST_MakeHashID(cstring)
Datum make_default_material(PG_FUNCTION_ARGS)
{
 //材质名字
  char *inputname = PG_GETARG_CSTRING(0);
  YkString strMaterial;
  strMaterial = strMaterial.FromMBString(inputname, strlen(inputname));

  YkModelMaterial *pMaterial = new YkModelMaterial(strMaterial);
	pMaterial->MakeDefault();
	pMaterial->getTechnique(0)->getPass(0)->m_MaterialColor = YkColorValue3D(0.5, 0.5, 0.5, 0.5);

  char *pData = 0;
  int nSize = 0;
  SaveElement(pMaterial, pData, nSize);
  delete pMaterial;

  char* output = (void *)palloc(nSize + 4);
  memcpy(output + 4, (char *)pData, nSize);
  delete pData;

  SET_VARSIZE(output, nSize + 4);

  PG_RETURN_POINTER(output);
}

#if 0
// ST_Make_Geomodel(cstring, cstring)
Datum make_geomodelshell(PG_FUNCTION_ARGS)
{
    //骨架名字
  char *inputname1 = PG_GETARG_CSTRING(0);
  YkString strSkeleton;
  strSkeleton = strSkeleton.FromMBString(inputname1, strlen(inputname1));

 //材质名字
  char *inputname2 = PG_GETARG_CSTRING(1);
  YkString strMaterial;
  strMaterial = strMaterial.FromMBString(inputname2, strlen(inputname2));

  YkModelNode* pModelNode = new YkModelNode();
  YkGeoModel* pGeoModel = new YkGeoModel(pModelNode, TRUE);
	pGeoModel->SetSpherePlaced(FALSE);
	YkModelPagedPatch* pPatch = pModelNode->CreatePatch();
	YkModelGeode* pGeode = new YkModelGeode();
  pPatch->AddGeode(pGeode);
  pGeode->AddElementShell(YkModelElement::etSkeleton, strSkeleton);
  pGeode->AddElementShell(YkModelElement::etMaterial, strMaterial);
//  YkBoundingBox bbox = pGeoModel->GetGeoBoundingBox();
  YkVector3d vMin(11644831.000000000, 4243161.5000000000,0);
  YkVector3d vMax(11644995.000000000, 4243208.0000000000, 100);
  YkBoundingBox bbox(vMin, vMax);
  pModelNode->Set
  
  // 序列化
  char *pData = 0;
  int nSize = 0;
  SaveShell(pGeoModel, pData, nSize);
  delete pGeoModel;
  
  GSERIALIZED *output = wrap_header_geomodeldata(pData, nSize, bbox);

  delete pData;
  PG_RETURN_POINTER(output);
}
#endif

// ST_Make_Geomodel(model_elem)
Datum make_geomodelshell(PG_FUNCTION_ARGS)
{
  varlena *buf = PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
  // 这里的 buf_size 是包含 size 字段的
  uint32_t buf_size = VARSIZE(buf);
  // 这里的数据没有包含 size 字段
  char *buf_data = VARDATA(buf);
  
  YkModelElement* pElem = LoadElement(buf_data, buf_size);  
  if(pElem == nullptr || pElem->GetType() != YkModelElement::etSkeleton)
  {
    // log...
    PG_RETURN_POINTER(0);
  }
  YkModelSkeleton *pSkeleton = (YkModelSkeleton *)pElem;

  YkString strMaterial = pSkeleton->m_strMaterialName;

  YkModelNode* pModelNode = new YkModelNode();
  YkGeoModel* pGeoModel = new YkGeoModel(pModelNode, TRUE);
	pGeoModel->SetSpherePlaced(FALSE);
	YkModelPagedPatch* pPatch = pModelNode->CreatePatch();
	YkModelGeode* pGeode = new YkModelGeode();
  pPatch->AddGeode(pGeode);
  pGeode->AddElement(pSkeleton);
  pGeode->AddElementShell(YkModelElement::etMaterial, strMaterial);
  YkBoundingBox bbox = pGeoModel->GetBoundingBox();
  
  // 序列化
  char *pData = 0;
  int nSize = 0;
  SaveShell(pGeoModel, pData, nSize);
  delete pGeoModel;
  
  GSERIALIZED *output = wrap_header_geomodeldata(pData, nSize, bbox);

  delete pData;
  PG_RETURN_POINTER(output);
}

Datum make_hash_from_string(PG_FUNCTION_ARGS)
{
  if (PG_ARGISNULL(0))
  {
    PG_RETURN_NULL();
  }

  char *input = PG_GETARG_CSTRING(0);

  YkString strName;
  strName = strName.FromMBString(input, strlen(input));
  YkLong lHash = YkHashCode::StringToHashCode64(strName);

  PG_RETURN_INT64(lHash);
}
