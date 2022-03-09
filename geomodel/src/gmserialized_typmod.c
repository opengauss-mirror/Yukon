/*
 *
 * gmserialized_typmpd.c
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
#include "../../libpgcommon/lwgeom_pg.h"
#include "../../liblwgeom/lwgeom_log.h"
#include "Geometry3D/YkWrapCGeoModel.h"

#include <stdio.h>

PG_FUNCTION_INFO_V1(geomodel_typmod_in);
PG_FUNCTION_INFO_V1(geomodel_typmod_out);

PG_FUNCTION_INFO_V1(model_elem_typmod_in);
PG_FUNCTION_INFO_V1(model_elem_typmod_out);

PG_FUNCTION_INFO_V1(geomodel_typmod_srid);
PG_FUNCTION_INFO_V1(geomodel_typmod_dims);
PG_FUNCTION_INFO_V1(geomodel_typmod_type);

PG_FUNCTION_INFO_V1(geomodel_get_srid);
PG_FUNCTION_INFO_V1(geomodel_set_srid);

PG_FUNCTION_INFO_V1(geomodel_geomodeltype);
PG_FUNCTION_INFO_V1(model_elem_enforce_typmod);

extern "C" Datum geomodel_typmod_in(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_typmod_out(PG_FUNCTION_ARGS);

extern "C" Datum model_elem_typmod_in(PG_FUNCTION_ARGS);
extern "C" Datum model_elem_typmod_out(PG_FUNCTION_ARGS);

extern "C" Datum geomodel_typmod_srid(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_typmod_dims(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_typmod_type(PG_FUNCTION_ARGS);

extern "C" Datum geomodel_get_srid(PG_FUNCTION_ARGS);
extern "C" Datum geomodel_set_srid(PG_FUNCTION_ARGS);

extern "C" Datum geomodel_geomodeltype(PG_FUNCTION_ARGS);
extern "C" Datum model_elem_enforce_typmod(PG_FUNCTION_ARGS);

uint32 gmserialized_typmod_in(ArrayType *arr);
void geomodel_setSRID(GSERIALIZED *s, int32_t srid);
int geomodel_getSRID(GSERIALIZED *s);

/**
 * @brief model_elem 修改器输入函数
 * @note 支持的类型为：mesh surface any
 * @return Datum 
 */
Datum model_elem_typmod_in(PG_FUNCTION_ARGS)
{
    ArrayType *arr = (ArrayType *)DatumGetPointer(PG_GETARG_DATUM(0));
    uint32 typmod = gmserialized_typmod_in(arr);
    int srid = TYPMOD_GET_SRID(typmod);
    /* Check the SRID is legal (geographic coordinates) */
    //srid_is_latlong(fcinfo, srid);

    PG_RETURN_INT32(typmod);
}

/**
 * @brief model_elem 修改器输出函数
 * @note 支持的类型为： skeleten material texture
 * @return Datum 
 */
Datum model_elem_typmod_out(PG_FUNCTION_ARGS)
{
    char *s = (char *)palloc(64);
    char *str = s;
    uint32 typmod = PG_GETARG_INT32(0);
    uint32 srid = TYPMOD_GET_SRID(typmod);
    uint32 type = TYPMOD_GET_TYPE(typmod);
    uint32 hasz = TYPMOD_GET_Z(typmod);
    uint32 hasm = TYPMOD_GET_M(typmod);

    /* 如果没有 SRID ，类型和维度，则直接返回空字符串 */
    if (!(srid || type || hasz || hasm))
    {
        *str = '\0';
        PG_RETURN_CSTRING(str);
    }

    str += sprintf(str, "(");

    if (type)
        str += sprintf(str, "%s", geomodel_type_name(type));
    else if ((!type) && (srid || hasz || hasm))
        str += sprintf(str, "model_elem");

    //if (hasz)
    //  str += sprintf(str, "%s", "Z");

    //if (hasm)
    //  str += sprintf(str, "%s", "M");

    if (srid)
        str += sprintf(str, ",");

    if (srid)
        str += sprintf(str, "%d", srid);

    str += sprintf(str, ")");

    PG_RETURN_CSTRING(s);
}

/**
 * @brief geomodel 类型,修改器输入函数
 * 
 * @return Datum 
 */
Datum geomodel_typmod_in(PG_FUNCTION_ARGS)
{
    ArrayType *arr = (ArrayType *)DatumGetPointer(PG_GETARG_DATUM(0));
    uint32 typmod = gmserialized_typmod_in(arr);
    int srid = TYPMOD_GET_SRID(typmod);
    /* Check the SRID is legal (geographic coordinates) */
    //srid_is_latlong(fcinfo, srid);

    PG_RETURN_INT32(typmod);
}

/**
 * @brief geomodel 类型,修改器输出函数
 * 
 * @return  cstring
 */
Datum geomodel_typmod_out(PG_FUNCTION_ARGS)
{
    char *s = (char *)palloc(64);
    char *str = s;
    uint32 typmod = PG_GETARG_INT32(0);
    uint32 srid = TYPMOD_GET_SRID(typmod);
    uint32 type = TYPMOD_GET_TYPE(typmod);
    uint32 hasz = TYPMOD_GET_Z(typmod);
    uint32 hasm = TYPMOD_GET_M(typmod);

    /* 如果没有 SRID ，类型和维度，则直接返回空字符串 */
    if (!(srid || type || hasz || hasm))
    {
        *str = '\0';
        PG_RETURN_CSTRING(str);
    }

    str += sprintf(str, "(");

    if (type)
        str += sprintf(str, "%s", geomodel_type_name(type));
    else if ((!type) && (srid || hasz || hasm))
        str += sprintf(str, "Geomodel");

    //if (hasz)
    //  str += sprintf(str, "%s", "Z");

    //if (hasm)
    //  str += sprintf(str, "%s", "M");

    if (srid)
        str += sprintf(str, ",");

    if (srid)
        str += sprintf(str, "%d", srid);

    str += sprintf(str, ")");

    PG_RETURN_CSTRING(s);
}

Datum geomodel_get_srid(PG_FUNCTION_ARGS)
{
    GSERIALIZED *gmd = PG_GETARG_GSERIALIZED_P(0);
    int srid = geomodel_getSRID(gmd);
    PG_FREE_IF_COPY(gmd, 0);
    PG_RETURN_INT32(srid);
}

Datum geomodel_set_srid(PG_FUNCTION_ARGS)
{
    GSERIALIZED *gmd = (GSERIALIZED *)PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
    int srid = PG_GETARG_INT32(1);
    geomodel_setSRID(gmd, srid);
    PG_RETURN_POINTER(gmd);
}
/**
 * @brief 序列化辅助函数
 * 
 * @param arr 
 * @return uint32 
 */
uint32 gmserialized_typmod_in(ArrayType *arr)
{
    uint32 typmod = 0;
    Datum *elem_values;
    int n = 0;
    int i = 0;

    if (ARR_ELEMTYPE(arr) != CSTRINGOID)
        ereport(ERROR,
                (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
                 errmsg("typmod array must be type cstring[]")));

    if (ARR_NDIM(arr) != 1)
        ereport(ERROR,
                (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
                 errmsg("typmod array must be one-dimensional")));

    if (ARR_HASNULL(arr))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("typmod array must not contain nulls")));

    /**
     * @brief 函数原型如下，具体的参数，可以通过 selece * from pg_type 查看
     * void deconstruct_array(ArrayType* array, Oid elmtype, int elmlen, 
     *                              bool elmbyval, char elmalign, Datum** elemsp,
     *                              bool** nullsp, int* nelemsp)
     * 
     */
    deconstruct_array(arr,
                      CSTRINGOID, -2, false, 'c',
                      &elem_values, NULL, &n);

    /* 先将 SRID 设置为默认值 */
    TYPMOD_SET_SRID(typmod, SRID_DEFAULT);

    for (i = 0; i < n; i++)
    {
        if (i == 0) /* TYPE */
        {
            char *s = DatumGetCString(elem_values[i]);
            uint8_t type = 0;

            if (geomodel_type_from_string(s, &type) == Yk_FAILURE)
            {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("Invalid geomodel type modifier: %s", s)));
            }
            else
            {
                TYPMOD_SET_TYPE(typmod, type);
                // 固定为 1
                TYPMOD_SET_Z(typmod);
                // 固定为 0
                TYPMOD_SET_M(typmod);
            }
        }
        if (i == 1) /* SRID */
        {
            int srid = pg_atoi(DatumGetCString(elem_values[i]),
                               sizeof(int32), '\0');
            //srid = clamp_srid(srid);
            if (srid != SRID_UNKNOWN)
            {
                TYPMOD_SET_SRID(typmod, srid);
            }
        }
    }

    pfree(elem_values);

    return typmod;
}

/**
 * @brief 辅助函数，用于 geomodel_columns 和 修改 srid 函数
 * 
 * @return int 
 */
Datum geomodel_typmod_srid(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    if (typmod < 0)
        PG_RETURN_INT32(0);
    PG_RETURN_INT32(TYPMOD_GET_SRID(typmod));
}

/**
 * @brief 辅助函数，用于 geomodel_columns 和 修改 srid 函数
 * 
 * @return int 
 */
Datum geomodel_typmod_dims(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    int32 dims = 2;
    /* 当 typmod 小于0 时，表示没有使用类型修改器，geomodel_columns 中显示为 0 */
    if (typmod < 0)
        PG_RETURN_INT32(0);
    if (TYPMOD_GET_Z(typmod))
        dims++;
    if (TYPMOD_GET_M(typmod))
        dims++;
    PG_RETURN_INT32(dims);
}

/**
 * @brief 辅助函数，用于 geomodel_columns 和 修改 srid 函数
 * 
 * @return cstring 
 */
Datum geomodel_typmod_type(PG_FUNCTION_ARGS)
{
    int32 typmod = PG_GETARG_INT32(0);
    int32 type = TYPMOD_GET_TYPE(typmod);
    char *s = (char *)palloc(64);
    char *ptr = s;
    text *stext;

    if (typmod < 0 || type == 0)
        ptr += sprintf(ptr, "ANYTYPE");
    else
        ptr += sprintf(ptr, "%s", geomodel_type_name(type));

    //if ( typmod >= 0 && TYPMOD_GET_Z(typmod) )
    //	ptr += sprintf(ptr, "%s", "Z");

    //if ( typmod >= 0 && TYPMOD_GET_M(typmod) )
    //	ptr += sprintf(ptr, "%s", "M");

    stext = cstring2text(s);
    pfree(s);
    PG_RETURN_TEXT_P(stext);
}

int geomodel_getSRID(GSERIALIZED *s)
{
    int32_t srid = 0;
    srid = srid | (s->srid[0] << 16);
    srid = srid | (s->srid[1] << 8);
    srid = srid | s->srid[2];
    /* Only the first 21 bits are set. Slide up and back to pull
	   the negative bits down, if we need them. */
    srid = (srid << 11) >> 11;

    /* 0 is our internal unknown value. We'll map back and forth here for now */
    if (srid == 0)
        return SRID_UNKNOWN;
    else
        return srid;
}

void geomodel_setSRID(GSERIALIZED *s, int32_t srid)
{
    srid = clamp_srid(srid);

    if (srid == SRID_UNKNOWN)
        srid = 0;

    s->srid[0] = (srid & 0x001F0000) >> 16;
    s->srid[1] = (srid & 0x0000FF00) >> 8;
    s->srid[2] = (srid & 0x000000FF);
}


Datum geomodel_geomodeltype(PG_FUNCTION_ARGS)
{
    text *type_text;
    char *ptr = (char *)palloc(20);

    varlena *buf = PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
    char *buf_data = VARDATA(buf);
    int len = VARSIZE(buf);

    YkModelElement *pElement = LoadElement(buf_data, len);
    if (pElement != NULL)
    {
        if (pElement->GetType() == etSkeleton)
        {
            YkModelSkeleton *pSkeleton = (YkModelSkeleton *)pElement;
            bool bExact = pSkeleton->GetExactDataTag(); //mesh(bExact=false)  surface(bExact=true)
            switch (bExact)
            {
            case 0: sprintf(ptr, "MESH");
            break;
            case 1: sprintf(ptr, "SURFACE");
            break;
            default: sprintf(ptr, "ANYTYPE");
            break;
            }
			type_text = cstring2text(ptr);
			PG_RETURN_TEXT_P(type_text);
        }
    }
    sprintf(ptr, "ANYTYPE");
    type_text = cstring2text(ptr);
    PG_RETURN_TEXT_P(type_text);
}

/**
 * @brief model_elem 类型隐式转换函数，
 * 
 * @return Datum 
 */
Datum model_elem_enforce_typmod(PG_FUNCTION_ARGS)
{

    varlena *buf = PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(0));
    char *buf_data = VARDATA(buf);
    int len = VARSIZE(buf);

    YkModelElement *pElement = LoadElement(buf_data, len);
    if (pElement == NULL)
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("the ELEMENT is not valid")));
        PG_RETURN_NULL();
    }

    if (pElement->GetType() == etSkeleton)
    {
        YkModelSkeleton *pSkeleton = (YkModelSkeleton *)pElement;
        bool bExact = pSkeleton->GetExactDataTag(); //mesh(1,bExact=false) or surface(2,bExact=true)

        int32 typmod = PG_GETARG_INT32(1);
        int32 typmod_srid = TYPMOD_GET_SRID(typmod);
        int32 typmod_type = TYPMOD_GET_TYPE(typmod);

        //如果 bExact 为真则为高精度数据，则不能存入 MESH 类型的数据库中
        //如果 bExact 为假，则为低精度数据，则不能存入 SURFACE 类型的数据库中
        if ((bExact && (typmod_type == MESHTYPE)) || (!bExact && (typmod_type == SURFACETYPE)))
        {
            ereport(ERROR, (
                               errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                               errmsg("SKELETON type (%s) does not match column type (%s)", geomodel_type_name(bExact ? SURFACETYPE : MESHTYPE), geomodel_type_name(typmod_type))));
            delete pElement;
            PG_RETURN_NULL();
        }
    }
    delete pElement;
    pElement = NULL;

    PG_RETURN_POINTER(buf);
}