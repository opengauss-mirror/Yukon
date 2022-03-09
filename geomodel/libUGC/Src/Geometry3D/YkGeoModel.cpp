/*
 *
 * YkGeoModel.cpp
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
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

#include "Base3D/YkMathEngine.h"
#include "Base3D/YkModelNodeTools.h"
#include "Geometry3D/YkGeoModel.h"

namespace Yk
{
#define Yk_GEOMODEL_VERSION	800

YkGeoModel::YkGeoModel()
{
	m_nVersion = Yk_GEOMODEL_VERSION;
	m_pModelNode = new YkModelNode();
	m_bNodeOwned = TRUE;
	m_bSpherePlaced = TRUE;
	m_BoundingBox.SetNULL();
	m_bPJInsertPntOnly = TRUE;
	m_strModelFilePath = _U("");
}

YkGeoModel::YkGeoModel(YkModelNode* pNode, YkBool bDelegate)
{
	m_nVersion = Yk_GEOMODEL_VERSION;
	YKASSERT(pNode != NULL);
	m_pModelNode = pNode;
	m_bNodeOwned = bDelegate;
	m_bSpherePlaced = TRUE;
	m_BoundingBox.SetNULL();
	m_bPJInsertPntOnly = TRUE;
	m_strModelFilePath = _U("");
}

YkGeoModel::~YkGeoModel()
{
	Clear();
}

void YkGeoModel::Clear()
{
	if(m_bNodeOwned && m_pModelNode != NULL)
	{
		delete m_pModelNode;
		m_pModelNode = NULL;
	}
	m_BoundingBox.SetNULL();
	m_bPJInsertPntOnly = TRUE;
}

YkPoint2D YkGeoModel::GetInnerPoint() const
{
	YkRect2D rcBounds = GetBounds();
	return rcBounds.CenterPoint();
}

YkPoint3D YkGeoModel::GetInnerPoint3D() const
{
	YkBoundingBox bbox = GetGeoBoundingBox();
	YkVector3d v = bbox.GetCenter();
	return YkPoint3D(v.x, v.y, v.z);
}

YkPoint3D YkGeoModel::GetPosition() const
{
	return YkPoint3D(m_pntPos.x, m_pntPos.y, m_pntPos.z);
}

YkGeometry::Type YkGeoModel::GetType() const
{
	return GeoModelPro/*(YkGeometry::Type) 1218*/;
}

YkInt YkGeoModel::GetDimension() const
{
	return 3;
}

YkBool YkGeoModel::IsValid() const
{
	return m_pModelNode != NULL;
}

YkBool YkGeoModel::MakeWithGeometry(const YkGeometry* pGeometry)
{
	YKASSERT(pGeometry);
	if(!MakeShell(pGeometry))
	{
		return FALSE;
	}

	m_pModelNode->Clear();

	YkGeoModel *pGeoModel = (YkGeoModel*)pGeometry;
	YkModelNode *pNode = pGeoModel->GetModelNode();
	*m_pModelNode = *pNode;
	m_bNodeOwned = TRUE;

	return CopyAllBaseData(*(YkGeometry*)pGeoModel);
}

YkBool YkGeoModel::MakeShell(const YkGeometry* pGeometry)
{
	YKASSERT(pGeometry);
	if(pGeometry == NULL || pGeometry->GetType() != YkGeometry::GeoModelPro)
	{
		return FALSE;
	}

	YkGeoModel *pGeoModel = (YkGeoModel*)pGeometry;
	m_nVersion = pGeoModel->GetVersion();
	m_pntPos = pGeoModel->GetPosition();
	m_pntRotate = pGeoModel->GetRotation();
	m_pntScale = pGeoModel->GetScale();
	m_BoundingBox = pGeoModel->GetBoundingBox();
	m_rcBounds = pGeoModel->GetBounds();
	m_nID = pGeometry->GetID();
	m_bSpherePlaced = pGeoModel->GetSpherePlaced();
	m_strModelFilePath = pGeoModel->GetFilePathName();

	return TRUE;
}

YkInt YkGeoModel::GetDataSize() const
{
	YkInt nSize = 0;
	nSize += sizeof(m_nVersion);
	nSize += sizeof(m_pntPos.x) * 3;
	nSize += sizeof(m_pntScale.x) * 3;
	nSize += sizeof(m_pntRotate.x) * 3;
	nSize += sizeof(YkDouble) * 3 * 2;

	//最后存储 模型
	if(m_pModelNode != NULL)
	{
		nSize += m_pModelNode->GetShellDataSize();
	}

	return 0;
}


void YkGeoModel::SetModelNode(YkModelNode *pGroup, YkBool bOwned)
{
	Clear();

	m_bNodeOwned = bOwned;
	m_pModelNode = pGroup;
	m_BoundingBox.SetNULL();
	SetBoundsDirty();
}

void YkGeoModel::SetScale(YkDouble fScaleX,YkDouble fScaleY,YkDouble fScaleZ)
{
	if(YKIS0(fScaleX) || YKIS0(fScaleY) || YKIS0(fScaleZ))
	{
		return;
	}

	m_pntScale.x=fScaleX;
	m_pntScale.y=fScaleY;
	m_pntScale.z=fScaleZ;

	SetBoundsDirty(TRUE);
	m_BoundingBox.SetNULL();
}

void YkGeoModel::SetScale(YkDouble fScaleXYZ)
{
	if(YKIS0(fScaleXYZ))
	{
		return;
	}

	m_pntScale.x=fScaleXYZ;
	m_pntScale.y=fScaleXYZ;
	m_pntScale.z=fScaleXYZ;

	SetBoundsDirty(TRUE);
	m_BoundingBox.SetNULL();
}

void YkGeoModel::SetRotation(YkDouble fRotateX,YkDouble fRotateY,YkDouble fRotateZ)
{
	m_pntRotate.x=fRotateX;
	m_pntRotate.y=fRotateY;
	m_pntRotate.z=fRotateZ;

	SetBoundsDirty(TRUE);
	m_BoundingBox.SetNULL();
}

YkModelNode *YkGeoModel::GetModelNode() const
{
	return m_pModelNode;
}

YkBool YkGeoModel::SaveGeoData(YkStream& stream, YkDataCodec::CodecType eCodecType,YkBool bSDBPlus)  const
{
	if (!IsValid())
	{
		return FALSE;
	}
	
 	TransformPntPosition();

	stream << m_nVersion;
	stream << m_bSpherePlaced;

	//定位点
	stream << m_pntPos.x;
	stream << m_pntPos.y;
	stream << m_pntPos.z;

	//缩放
	stream << m_pntScale.x;
	stream << m_pntScale.y;
	stream << m_pntScale.z;

	//旋转
	stream << m_pntRotate.x;
	stream << m_pntRotate.y;
	stream << m_pntRotate.z;

	//包围盒 存储之前需要算一下
	//在 TransformPntPosition 方法中已经重新计算过
	YkVector3d vecMax = m_BoundingBox.GetMax();
	YkVector3d vecMin = m_BoundingBox.GetMin();
	stream<<vecMax.x;//YkDouble
	stream<<vecMax.y;
	stream<<vecMax.z;

	stream<<vecMin.x;
	stream<<vecMin.y;
	stream<<vecMin.z;

	//最后存储 模型
	if(m_pModelNode != NULL)
	{
		m_pModelNode->SaveShells(stream);
	}

	return TRUE;
}

YkBool YkGeoModel::LoadGeoData( YkStream& stream , YkDataCodec::CodecType eCodecType,YkBool bSDBPlus)
{
	stream >> m_nVersion;
	if(m_nVersion != Yk_GEOMODEL_VERSION)
	{
		return FALSE;
	}

	stream >> m_bSpherePlaced;

	//定位点
	stream >> m_pntPos.x;
	stream >> m_pntPos.y;
	stream >> m_pntPos.z;

	//缩放
	stream >> m_pntScale.x;
	stream >> m_pntScale.y;
	stream >> m_pntScale.z;

	//旋转
	stream >> m_pntRotate.x;
	stream >> m_pntRotate.y;
	stream >> m_pntRotate.z;

	//包围盒
	YkVector3d vecMax;
	YkVector3d vecMin;
	stream >> vecMax.x;//YkDouble
	stream >> vecMax.y;
	stream >> vecMax.z;
	stream >> vecMin.x;
	stream >> vecMin.y;
	stream >> vecMin.z;
	m_BoundingBox.SetMax(vecMax);
	m_BoundingBox.SetMin(vecMin);

	if(m_bNodeOwned && m_pModelNode == NULL)
	{
		m_pModelNode = new YkModelNode();
	}
	m_pModelNode->LoadShells(stream);

	SetBoundsDirty();

	return TRUE;
}

const YkBoundingBox& YkGeoModel::GetBoundingBox() const
{
	if(m_BoundingBox.IsNULL())
	{
		YkBoundingBox bbox = ComputeBoundingBox();
		((YkGeometry*)this)->SetBoundingBox(bbox);
	}
	
	return m_BoundingBox;
}

void YkGeoModel::SetBoundingBox(const YkBoundingBox& box)	
{

	m_BoundingBox = box;
	SetBoundsDirty();
}

void YkGeoModel::SetSpherePlaced(YkBool bSpherePlaced)
{
	if(!IsBoundsDirty())
	{
		SetBoundsDirty( bSpherePlaced != m_bSpherePlaced);
	}

	m_bSpherePlaced = bSpherePlaced;
}

YkBool YkGeoModel::GetSpherePlaced() const
{
	return m_bSpherePlaced;
}

void YkGeoModel::ReMultiplyMatrix(const YkPoint3D& pntUserDefined, const YkMatrix4d& mTrans)
{
	// 第一步：转成世界坐标的变换矩阵
	const YkMatrix4d& mLocal = GetLocalMatrix();

	// 第二步：转成指定插入点的变换矩阵
	YkMatrix4d mRefer;
	if (m_bSpherePlaced)
	{
		YkMatrix4d mFvo = YkMatrix4d::RotationXYZ(-pntUserDefined.y * DTOR, pntUserDefined.x * DTOR, 0.0);
		YkVector3d vCar = YkMathEngine::SphericalToGlobalCartesian(pntUserDefined.x, pntUserDefined.y, pntUserDefined.z);
		YkMatrix4d mOff = YkMatrix4d::Translation(vCar.x, vCar.y, vCar.z);
		mRefer = (mFvo * mOff).Invert();
	}
	else
	{
		YkMatrix4d mOff = YkMatrix4d::Translation(pntUserDefined.x, pntUserDefined.y, pntUserDefined.z);
		mRefer = mOff.Invert();
	}

	// 第三步：进行矩阵变换
	YkMatrix4d mTransform = mLocal * mRefer * mTrans;
	m_pModelNode->ReMultiplyMatrix(mTransform);

	// 第四步：修改模型插入点信息
	m_pntPos = pntUserDefined;
	m_pntRotate = YkPoint3D(0.0, 0.0, 0.0);
	m_pntScale = YkPoint3D(1.0, 1.0, 1.0);
}

YkMatrix4d YkGeoModel::GetLocalMatrix() const
{
	return YkModelNodeTools::GetLocalMatrix(m_pntPos, m_pntScale, m_pntRotate, m_bSpherePlaced);
}

YkBoundingBox YkGeoModel::GetGeoBoundingBox() const
{
	YkBoundingBox bboxResult;
	YkBoundingBox bbox = GetBoundingBox();

	//插入点、旋转、缩放
	YkMatrix4d mat = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z) *
		YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR, m_pntRotate.y*DTOR, m_pntRotate.z*DTOR);

	bbox.Transform(mat);

	if(m_bSpherePlaced)
	{
		SceneType sceneTypeOrg = YkMathEngine::m_msSceneType;
		YkMathEngine::m_msSceneType = ST_EARTH_SPHERICAL;

		YkMatrix4d mRotate = YkMatrix4d::RotationXYZ(-m_pntPos.y*DTOR, m_pntPos.x*DTOR, 0);

		YkVector3d pnt_Car = YkMathEngine::SphericalToCartesian(m_pntPos.x*DTOR, 
			m_pntPos.y*DTOR, m_pntPos.z+GLOBAL_RADIUS);

		YkMatrix4d transMatrix = YkMatrix4d::Translation(pnt_Car.x, pnt_Car.y, pnt_Car.z);

		YkVector3d  v1(bbox.GetCorners()[0].x,bbox.GetCorners()[0].y,bbox.GetCorners()[0].z);
		YkVector3d  v2(bbox.GetCorners()[1].x,bbox.GetCorners()[1].y,bbox.GetCorners()[1].z);
		YkVector3d  v3(bbox.GetCorners()[2].x,bbox.GetCorners()[2].y,bbox.GetCorners()[2].z);
		YkVector3d  v4(bbox.GetCorners()[3].x,bbox.GetCorners()[3].y,bbox.GetCorners()[3].z);
		YkVector3d  v5(bbox.GetCorners()[4].x,bbox.GetCorners()[4].y,bbox.GetCorners()[4].z);
		YkVector3d  v6(bbox.GetCorners()[5].x,bbox.GetCorners()[5].y,bbox.GetCorners()[5].z);
		YkVector3d  v7(bbox.GetCorners()[6].x,bbox.GetCorners()[6].y,bbox.GetCorners()[6].z);
		YkVector3d  v8(bbox.GetCorners()[7].x,bbox.GetCorners()[7].y,bbox.GetCorners()[7].z);

		YkMatrix4d mat = mRotate * transMatrix;
		YkVector3d vM1 = v1.MultiplyMatrix(mat);
		YkVector3d vM2 = v2.MultiplyMatrix(mat);
		YkVector3d vM3 = v3.MultiplyMatrix(mat);
		YkVector3d vM4 = v4.MultiplyMatrix(mat);
		YkVector3d vM5 = v5.MultiplyMatrix(mat);
		YkVector3d vM6 = v6.MultiplyMatrix(mat);
		YkVector3d vM7 = v7.MultiplyMatrix(mat);
		YkVector3d vM8 = v8.MultiplyMatrix(mat);

		YkVector3d pnt1 = YkMathEngine::CartesianToSphericalD(vM1.x, vM1.y, vM1.z);
		YkVector3d pnt2 = YkMathEngine::CartesianToSphericalD(vM2.x, vM2.y, vM2.z);
		YkVector3d pnt3 = YkMathEngine::CartesianToSphericalD(vM3.x, vM3.y, vM3.z);
		YkVector3d pnt4 = YkMathEngine::CartesianToSphericalD(vM4.x, vM4.y, vM4.z);
		YkVector3d pnt5 = YkMathEngine::CartesianToSphericalD(vM5.x, vM5.y, vM5.z);
		YkVector3d pnt6 = YkMathEngine::CartesianToSphericalD(vM6.x, vM6.y, vM6.z);
		YkVector3d pnt7 = YkMathEngine::CartesianToSphericalD(vM7.x, vM7.y, vM7.z);
		YkVector3d pnt8 = YkMathEngine::CartesianToSphericalD(vM8.x, vM8.y, vM8.z);

		YkVector3d pntMax, pntMin;
		pntMax.x = YKMAX(YKMAX(YKMAX(pnt1.x, pnt2.x), YKMAX(pnt3.x, pnt4.x)),
			YKMAX(YKMAX(pnt5.x, pnt6.x), YKMAX(pnt7.x, pnt8.x))) * RTOD;
		pntMax.y =  YKMAX(YKMAX(YKMAX(pnt1.y, pnt2.y), YKMAX(pnt3.y, pnt4.y)),
			YKMAX(YKMAX(pnt5.y, pnt6.y), YKMAX(pnt7.y, pnt8.y))) * RTOD;
		pntMax.z = YKMAX(YKMAX(YKMAX(pnt1.z, pnt2.z), YKMAX(pnt3.z, pnt4.z)),
			YKMAX(YKMAX(pnt5.z, pnt6.z), YKMAX(pnt7.z, pnt8.z))) ;
		pntMin.x = YKMIN(YKMIN(YKMIN(pnt1.x, pnt2.x), YKMIN(pnt3.x, pnt4.x)),
			YKMIN(YKMIN(pnt5.x, pnt6.x), YKMIN(pnt7.x, pnt8.x))) * RTOD;
		pntMin.y = YKMIN(YKMIN(YKMIN(pnt1.y, pnt2.y), YKMIN(pnt3.y, pnt4.y)),
			YKMIN(YKMIN(pnt5.y, pnt6.y), YKMIN(pnt7.y, pnt8.y))) * RTOD;
		pntMin.z = YKMIN(YKMIN(YKMIN(pnt1.z, pnt2.z), YKMIN(pnt3.z, pnt4.z)),
			YKMIN(YKMIN(pnt5.z, pnt6.z), YKMIN(pnt7.z, pnt8.z)));

		bboxResult.SetMax(pntMax);
		bboxResult.SetMin(pntMin);

		YkMathEngine::m_msSceneType = sceneTypeOrg;
	}
	else
	{
		YkMatrix4d transMatrix = YkMatrix4d::Translation(m_pntPos.x, m_pntPos.y, m_pntPos.z);
		bbox.Transform(transMatrix);
		bboxResult = bbox;
	}
	return bboxResult;
}

void YkGeoModel::GetAltitude( YkDouble& dbMaxZ, YkDouble& dbMinZ)
{
	//重新计算
	YkBoundingBox  bbox;
	SetBoundingBox(bbox);
	bbox = GetGeoBoundingBox();

	dbMaxZ = bbox.GetMax().z;
	dbMinZ = bbox.GetMin().z;
	if(m_bSpherePlaced)
	{	
		dbMaxZ -= GLOBAL_RADIUS;
		dbMinZ -= GLOBAL_RADIUS;
	}
}

YkRect2D YkGeoModel::ComputeBounds() const
{
	YkBoundingBox bbGeoBox = GetGeoBoundingBox();

	return YkRect2D(bbGeoBox.GetMin().x, bbGeoBox.GetMax().y, 
		bbGeoBox.GetMax().x, bbGeoBox.GetMin().y);
}

YkBoundingBox YkGeoModel::ComputeBoundingBox() const
{
	YkBoundingBox boundBox =m_pModelNode->ComputeBoudingBox();
	if (boundBox.IsNULL())
	{
		//没获取成功就用原来存储的
		return m_BoundingBox;
	}

	return boundBox;
}


//! \brief 从加载YkModelsElementsManager填充vector
YkBool YkGeoModel::LoadElements(YkModelEM* pEM, YkInt nLOD, YkModelElement::METype mtype)
{
	YKASSERT(m_pModelNode != NULL);
	return m_pModelNode->LoadElements(pEM, nLOD, mtype);
}

void YkGeoModel::TransformPntPosition() const
{
	//中心点地理坐标
	YkBoundingBox bGeoBox = GetGeoBoundingBox();
	YkVector3d vecCenter = bGeoBox.GetCenter();

	YkVector3d vecPnt(m_pntPos.x, m_pntPos.y, m_pntPos.z);
	YkMatrix4d mMatrix = YkMatrix4d::IDENTITY;
	if(m_bSpherePlaced)
	{
		vecCenter.z = GLOBAL_RADIUS + m_pntPos.z;//高度不变，相当于是取底面中心点

		SceneType sceneTypeOrg = YkMathEngine::m_msSceneType;
		YkMathEngine::m_msSceneType = ST_EARTH_SPHERICAL;

		YkMatrix4d matGeoScale = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z);
		YkMatrix4d matGeoRotate = YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR, 
			m_pntRotate.y*DTOR, m_pntRotate.z*DTOR);
		YkMatrix4d mPntRotate = YkMatrix4d::RotationXYZ(-m_pntPos.y*DTOR, m_pntPos.x*DTOR, 0);
		YkVector3d vecPntCartesian = YkMathEngine::SphericalToCartesian(m_pntPos.x*DTOR, 
			m_pntPos.y*DTOR, m_pntPos.z+GLOBAL_RADIUS);
		YkMatrix4d mPnt = matGeoScale * matGeoRotate * 
			mPntRotate * YkMatrix4d::Translation(vecPntCartesian.x, vecPntCartesian.y, vecPntCartesian.z);

		YkMatrix4d mCenterRotate = YkMatrix4d::RotationXYZ(-vecCenter.y*DTOR, vecCenter.x*DTOR, 0);
		YkVector3d vecCenterCartesian = YkMathEngine::SphericalToCartesian(vecCenter.x*DTOR, vecCenter.y*DTOR, vecCenter.z);
		YkMatrix4d mCenter = matGeoScale * matGeoRotate * 
			mCenterRotate * YkMatrix4d::Translation(vecCenterCartesian.x, vecCenterCartesian.y, vecCenterCartesian.z);
		
		mMatrix = mPnt * mCenter.Invert();

		vecCenter.z = vecCenter.z - GLOBAL_RADIUS;

		YkMathEngine::m_msSceneType = sceneTypeOrg;
	}
	else
	{
		vecCenter.z = m_pntPos.z;//高度不变，相当于是取底面中心点

		YkMatrix4d pntGeoScale = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z);
		YkMatrix4d pntGeoRotate = YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR,m_pntRotate.y*DTOR, m_pntRotate.z*DTOR);
		YkMatrix4d pntGeoTrans = YkMatrix4d::Translation(m_pntPos.x, m_pntPos.y, m_pntPos.z);
		YkMatrix4d mPnt = pntGeoScale * pntGeoRotate * pntGeoTrans;

		YkMatrix4d centerScale = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z);
		YkMatrix4d centerRotate = YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR,m_pntRotate.y*DTOR, m_pntRotate.z*DTOR);
		YkMatrix4d centerTrans = YkMatrix4d::Translation(vecCenter.x, vecCenter.y, vecCenter.z);

		mMatrix = mPnt * centerTrans.Invert() * centerRotate.Invert() * centerScale.Invert();
	}

	m_pModelNode->ReMultiplyMatrix(mMatrix);
	
	((YkGeometry*)this)->SetPosition(YkPoint3D(vecCenter.x, vecCenter.y, vecCenter.z));
	
	//重新计算一下BoundingBox
	YkBoundingBox bboxNew = ComputeBoundingBox();
	((YkGeometry*)this)->SetBoundingBox(bboxNew);
}

YkString YkGeoModel::GetFilePathName() const
{
	return m_strModelFilePath;
}

void YkGeoModel::SetFilePathName(YkString strFilePath)
{
	m_strModelFilePath = strFilePath;
}


void YkGeoModel::SetEM(YkModelEM* pEM)
{
	m_pEM = pEM;
}

YkGeoModel* YkGeoModel::Compose(YkArray<YkGeoModel*> arrGeoModel)
{
	if(arrGeoModel.GetSize() == 0)
	{
		return NULL;
	}

	//先判断坐标系是否统一
	YkBool bSpherePlaced = arrGeoModel[0]->GetSpherePlaced();
	for (YkInt i = 1; i < arrGeoModel.GetSize(); i++)
	{
		if(bSpherePlaced != arrGeoModel[i]->GetSpherePlaced())
		{
			return NULL;
		}
	}

	YkBool bIgnoreLOD = FALSE;
	YkInt nLOD = arrGeoModel[0]->GetModelNode()->GetPagedLODs().size();
	for (YkInt i = 1; i < arrGeoModel.GetSize(); i++)
	{
		if(nLOD != 
			arrGeoModel[i]->GetModelNode()->GetPagedLODs().size())
		{
			bIgnoreLOD = TRUE;
			break;
		}
	}

	//开始合并
	YkGeoModel* pGeoModelDes = new YkGeoModel();
	YkModelNode* pModelNodeDes = pGeoModelDes->GetModelNode();

	pGeoModelDes->MakeShell(arrGeoModel[0]);
	YkMatrix4d matDes = pGeoModelDes->GetLocalMatrix();
	YkMatrix4d matDesInvert = matDes.Invert();
	for (YkInt i = 0; i < arrGeoModel.GetSize(); i++)
	{
		YkGeoModel* pGeoModeSrc = arrGeoModel[i];
		YkMatrix4d matSrc = pGeoModeSrc->GetLocalMatrix();
		YkMatrix4d matSupplement = matSrc * matDesInvert;

		YkModelNode* pModelNodeSrc = pGeoModeSrc->GetModelNode();
		pModelNodeSrc->ReMultiplyMatrix(matSupplement);
		pModelNodeDes->AppendByPatch(pModelNodeSrc, bIgnoreLOD);
	}

	return pGeoModelDes;
}

void YkGeoModel::TranslatePosition(YkModelPagedPatch* pPatch, \
							  YkMatrix4d& matSrc, YkMatrix4d& matLocalBase)
{
	YkString strSkeletonOrg, strSkeletonDes;
	for (YkInt i=0; i<pPatch->GetGeodeCount(); i++)
	{
		YkModelGeode* pGeode = pPatch->GetGeode(i);
		YkMatrix4d matSupplement = matSrc * matLocalBase.Invert();
		YkMatrix4d matGeo = pGeode->GetPosition();
		matGeo = matGeo * matSupplement;
		pGeode->SetPosition(matGeo);
	}
}

//! \brief 模型对象平移，单位：米或弧度
YkBool YkGeoModel::Translate(YkDouble dbX, YkDouble dbY, \
								YkDouble dbZ, YkPoint3D pntBase)
{
	YkMatrix4d matLocalSrc = YkModelNodeTools::GetLocalMatrix(m_pntPos, m_pntScale, m_pntRotate, m_bSpherePlaced);
	
	//1、原始对象插入点改为统一点：pntBase；绝对位置不变
	YkMatrix4d matLocalBase = YkModelNodeTools::GetLocalMatrix(pntBase, m_pntScale, m_pntRotate, m_bSpherePlaced);
	//重新计算各顶点值
	//各层LOD
	YkInt nLOD = m_pModelNode->GetPagedLODs().size();
	for (YkInt i=0; i<nLOD; i++)
	{
		YkModelPagedLOD* pPagedLOD = m_pModelNode->GetPagedLOD(i);
		YkInt nPatch = pPagedLOD->GetPagedPatchCount();
		for (YkInt j=0; j<nPatch; j++)
		{
			YkModelPagedPatch *pPatch = pPagedLOD->GetPagedPatch(j);
			TranslatePosition(pPatch, matLocalSrc, matLocalBase);
		}
	}
	//精细层
	YkInt nPatch = m_pModelNode->GetDataPatcheCount();
	for (YkInt i=0; i<nPatch; i++)
	{
		YkModelPagedPatch* pPatch = m_pModelNode->GetDataPatche(i);
		TranslatePosition(pPatch, matLocalSrc, matLocalBase);
	}

	YkMatrix4d matTransZ = YkMatrix4d::Translation(0, 0, pntBase.z - m_pntPos.z);
	m_pModelNode->ReMultiplyMatrix(matTransZ);

	//2、改插入点咯	
	YkPoint3D pntDes(pntBase.x+dbX*RTOD, pntBase.y+dbY*RTOD, m_pntPos.z+dbZ);

	m_pntPos = pntDes;

	SetBoundsDirty();
	m_BoundingBox.SetNULL();
	ComputeBounds();

	return TRUE;
}

YkBool YkGeoModel::Rotate(YkPoint3D rotatePoint,
							 YkDouble dXAngle, YkDouble dYAngle, YkDouble dZAngle,
							 YkPoint3D pntBase)
{
	YkMatrix4d mat = YkMatrix4d::IDENTITY;

	// 统一基准点
	YkMatrix4d matLocalSrc = YkModelNodeTools::GetLocalMatrix(m_pntPos, m_pntScale, m_pntRotate, m_bSpherePlaced); 
	mat = mat * matLocalSrc;

	YkMatrix4d matLocalBase = YkModelNodeTools::GetLocalMatrix(pntBase, m_pntScale, m_pntRotate, m_bSpherePlaced);
	mat = mat * matLocalBase.Invert();

	//----------------------------------------------------------
	// 旋转

	// ----- 求转换到绝对坐标的矩阵
	YkVector3d vecPosition;
	if(m_bSpherePlaced)
	{
		vecPosition.x = pntBase.x* DTOR * GLOBAL_RADIUS;
		vecPosition.y = pntBase.y* DTOR * GLOBAL_RADIUS;
		vecPosition.z = pntBase.z + GLOBAL_RADIUS;
	}
	else
	{
		vecPosition.x = pntBase.x;
		vecPosition.y = pntBase.y;
		vecPosition.z = pntBase.z;
	}
	YkMatrix4d transMatrix = YkMatrix4d::Translation(vecPosition.x,vecPosition.y,vecPosition.z);

	matLocalBase = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z) *
		YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR, m_pntRotate.y*DTOR, m_pntRotate.z*DTOR) *
		transMatrix;

	mat = mat * matLocalBase;

	//-----------------------
	YkVector3d rotatePos;
	if(m_bSpherePlaced)
	{
		rotatePos.x = rotatePoint.x* DTOR * GLOBAL_RADIUS;
		rotatePos.y = rotatePoint.y* DTOR * GLOBAL_RADIUS;
		rotatePos.z = rotatePoint.z + GLOBAL_RADIUS;
	}
	else
	{
		rotatePos.x = rotatePoint.x;
		rotatePos.y = rotatePoint.y;
		rotatePos.z = rotatePoint.z;
	}
	YkMatrix4d matTrans = YkMatrix4d::Translation(-rotatePos.x, -rotatePos.y, -rotatePos.z);
	mat = mat * matTrans;
	// 再旋转角度
	YkMatrix4d matRotate = YkMatrix4d::RotationXYZ(dXAngle * DTOR , dYAngle * DTOR, dZAngle * DTOR);
	mat = mat * matRotate;

	YkMatrix4d matBackTrans = YkMatrix4d::Translation(rotatePos.x, rotatePos.y, rotatePos.z);
	mat = mat * matBackTrans;
	//--------------------------

	vecPosition = vecPosition.MultiplyMatrix(matTrans);
	vecPosition = vecPosition.MultiplyMatrix(matRotate);
	vecPosition = vecPosition.MultiplyMatrix(matBackTrans);

	YkMatrix4d matPos = YkMatrix4d::Translation(-vecPosition.x, -vecPosition.y, -vecPosition.z);
	
	if(m_bSpherePlaced)
	{
		vecPosition.x = vecPosition.x / GLOBAL_RADIUS / DTOR;
		vecPosition.y = vecPosition.y / GLOBAL_RADIUS / DTOR;
		vecPosition.z -= GLOBAL_RADIUS;
	}

	YkMatrix4d matSR = YkMatrix4d::Scaling(m_pntScale.x, m_pntScale.y, m_pntScale.z) *
		YkMatrix4d::RotationXYZ(m_pntRotate.x*DTOR, m_pntRotate.y*DTOR, m_pntRotate.z*DTOR);

	matSR = matSR.Invert();

	matPos = matPos * matSR;

	mat = mat * matPos;
	
	YkMatrix4d identityMat = YkMatrix4d::IDENTITY;

	// 重新计算各顶点值
	// 各层LOD
	YkInt nLOD = m_pModelNode->GetPagedLODs().size();
	for (YkInt i=0; i<nLOD; i++)
	{
		YkModelPagedLOD* pPagedLOD = m_pModelNode->GetPagedLOD(i);
		YkInt nPatch = pPagedLOD->GetPagedPatchCount();
		for (YkInt j=0; j<nPatch; j++)
		{
			YkModelPagedPatch *pPatch = pPagedLOD->GetPagedPatch(j);
			TranslatePosition(pPatch, mat, identityMat);
		}
	}
	//精细层
	YkInt nPatch = m_pModelNode->GetDataPatcheCount();
	for (YkInt i=0; i<nPatch; i++)
	{
		YkModelPagedPatch* pPatch = m_pModelNode->GetDataPatche(i);
		TranslatePosition(pPatch, mat, identityMat);
	}
	
	YkPoint3D pntDes(vecPosition.x, vecPosition.y, vecPosition.z);
	
	m_pntPos = pntDes;
	
	SetBoundsDirty(TRUE);
	m_BoundingBox.SetNULL();
	ComputeBounds();
	
	return TRUE;
}

}
