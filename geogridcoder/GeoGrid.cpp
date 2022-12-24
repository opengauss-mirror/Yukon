/*
 *
 * GeoGrid.cpp
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

#include "GeoGrid.h"
#include <math.h>
#include <algorithm>
#include "geosot.h"
#include "rasterize_grid.h"
#include "GSGUtil.h"

PG_MODULE_MAGIC;

using namespace Yk;
static pqsigfunc coreIntHandler = 0;

extern ArrayType* array_grid2d_unique(ArrayType* r);
extern ArrayType* array_grid3d_unique(ArrayType* r);

PG_FUNCTION_INFO_V1(UgComputerGeoHash);
PG_FUNCTION_INFO_V1(UgBBoxGeoHash);
PG_FUNCTION_INFO_V1(UgGetFilter1);

PG_FUNCTION_INFO_V1(gsg_geosotgrid);
PG_FUNCTION_INFO_V1(gsg_geosotgridagg);
PG_FUNCTION_INFO_V1(gsg_mingeosotgrid);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_z);
PG_FUNCTION_INFO_V1(gsg_as_altitude);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_from_text);
PG_FUNCTION_INFO_V1(gsg_geosotgrid3d_from_text);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_as_text);
PG_FUNCTION_INFO_V1(gsg_geom_from_geosotgrid);
PG_FUNCTION_INFO_V1(gsg_geom_from_geosotgrid_array);
PG_FUNCTION_INFO_V1(gsg_has_z);
PG_FUNCTION_INFO_V1(gsg_get_level);
PG_FUNCTION_INFO_V1(gsg_get_level_extremum);
PG_FUNCTION_INFO_V1(gsg_aggregate);
PG_FUNCTION_INFO_V1(gsg_aggregate_array);

extern "C" Datum UgComputerGeoHash(PG_FUNCTION_ARGS);
extern "C" Datum UgBBoxGeoHash(PG_FUNCTION_ARGS);
extern "C" Datum UgGetFilter1(PG_FUNCTION_ARGS);

extern "C" Datum gsg_geosotgrid(PG_FUNCTION_ARGS);
extern "C" Datum gsg_mingeosotgrid(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgridagg(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgrid_z(PG_FUNCTION_ARGS);
extern "C" Datum gsg_as_altitude(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgrid_from_text(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgrid3d_from_text(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgrid_as_text(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geosotgrid_z_as_text(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geom_from_geosotgrid(PG_FUNCTION_ARGS);
extern "C" Datum gsg_geom_from_geosotgrid_array(PG_FUNCTION_ARGS);
extern "C" Datum gsg_has_z(PG_FUNCTION_ARGS);
extern "C" Datum gsg_get_level(PG_FUNCTION_ARGS);
extern "C" Datum gsg_get_level_extremum(PG_FUNCTION_ARGS);
extern "C" Datum gsg_aggregate(PG_FUNCTION_ARGS);
extern "C" Datum gsg_aggregate_array(PG_FUNCTION_ARGS);
static void handleInterrupt(int sig);
void _PG_init(void);
void _PG_fini(void);

/**
 * @brief 计算 geometry 的 GEOHASH 编码
 * 输入 3 个参数，boundary,geometry,level
 * @return int8 类型
 */
Datum UgComputerGeoHash(PG_FUNCTION_ARGS)
{

    GBOX *box = NULL;
    RawHashData rawdata;
    GSERIALIZED *gser = NULL;
    LWGEOM *geom = NULL;
    rt_pgraster *pgrast = NULL;
    lwvarlena_t *wkb;
    size_t wkb_len = 0;
    uint8_t variant = WKB_SFSQL;
    YkInt level = 0;

    if (PG_ARGISNULL(0))
    {
        PG_RETURN_NULL();
    }
    box = (GBOX *)PG_GETARG_POINTER(0);

    if (PG_ARGISNULL(1))
    {
        PG_RETURN_NULL();
    }
    gser = PG_GETARG_GSERIALIZED_P(1);
    geom = lwgeom_from_gserialized(gser);

    if (PG_ARGISNULL(2))
    {
        PG_RETURN_NULL();
    }
    level = PG_GETARG_INT32(2);

    // 将 lwgeom 对象转化为 wkb 对象
    wkb = lwgeom_to_wkb_varlena(geom, variant);
	wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
    lwgeom_free(geom);

    // 获取 srid
    //char *srs = NULL;
    //srid = gserialized_get_srid(gser);

    //栅格化
	HashRasterize((unsigned char *)wkb->data, wkb_len, nullptr,
				  YkRect2D(box->xmin, box->ymax, box->xmax, box->ymin),
				  level, rawdata);
	delete wkb;
	wkb = nullptr;

    vector<uint64_t> vt;

    // 在这里要根据 rawdata 中的偏移量重新计算坐标
    for (int i = 0; i < rawdata.height; i++)
    {
        for (int j = 0; j < rawdata.width; j++)
        {
            if (rawdata.val[i * rawdata.width + j] == 1)
            {
                uint64_t hash = HashCode(j + rawdata.offsetx, i + rawdata.offsety, level);
                vt.push_back(hash);
            }
        }
    }

    Datum *elements = (Datum *)palloc(vt.size() * 8);
    ArrayType *array;
    int i;

    for (i = 0; i < vt.size(); i++)
        elements[i] = Int64GetDatum(vt[i]);

    array = construct_array(elements, vt.size(), INT8OID, 8, true, 'd');
    pfree(elements);
    PG_RETURN_POINTER(array);
}

/**
 * @brief 计算查询框的 hash 编码
 * 
 * @return Datum 
 */
Datum UgBBoxGeoHash(PG_FUNCTION_ARGS)
{
    GBOX *querybox = nullptr;
    GBOX *bbox = nullptr;
    GBOX _innerbox;
    uint8_t level = 0;
    double _scale[2] = {0};
    int _dim[2] = {0};
    int offsetx = 0;
    int offsety = 0;

    //获取边界框
    if (PG_ARGISNULL(0))
    {
        PG_RETURN_NULL();
    }
    bbox = (GBOX *)PG_GETARG_POINTER(0);

    //获取查询框
    if (PG_ARGISNULL(1))
    {
        PG_RETURN_NULL();
    }
    querybox = (GBOX *)PG_GETARG_POINTER(1);

    //获取查询等级
    if (PG_ARGISNULL(2))
    {
        PG_RETURN_NULL();
    }
    level = PG_GETARG_INT32(2);

    _dim[0] = (std::pow(2, level));
    _dim[1] = (std::pow(2, level));

    _scale[0] = (bbox->xmax - bbox->xmin) / _dim[0];
    _scale[1] = (bbox->ymax - bbox->ymin) / _dim[1];

    //计算 box 对应的栅格
    _innerbox.xmin = floor((querybox->xmin - bbox->xmin) / _scale[0]) * _scale[0] + bbox->xmin;
    _innerbox.ymin = floor((querybox->ymin - bbox->ymin) / _scale[1]) * _scale[1] + bbox->ymin;
    _innerbox.xmax = ceil(((querybox->xmax - bbox->xmin) + _scale[0]) / _scale[0]) * _scale[0] + bbox->xmin;
    _innerbox.ymax = ceil(((querybox->ymax - bbox->ymin) + _scale[1]) / _scale[1]) * _scale[1] + bbox->ymin;

    offsetx = floor(_innerbox.xmin / _scale[0] - bbox->xmin / _scale[0] + 0.1);
    offsety = floor(_innerbox.ymin / _scale[1] - bbox->ymin / _scale[1] + 0.1);

    /* 重新计算 width 和 height */
    _dim[0] = ceil((_innerbox.xmax - _innerbox.xmin) / _scale[0]);
    _dim[1] = ceil((_innerbox.ymax - _innerbox.ymin) / _scale[1]);

    Datum *elements = (Datum *)palloc0(_dim[0] * _dim[1] * 8);
    ArrayType *array;

    for (int i = 0; i < _dim[1]; i++)
    {
        for (int j = 0; j < _dim[0]; j++)
        {
            uint64_t hash = HashCode(j + offsetx, i + offsety, level);
            elements[i * _dim[0] + j] = Int64GetDatum(hash);
        }
    }

    array = construct_array(elements, _dim[1] * _dim[0], INT8OID, 8, true, 'd');
    pfree(elements);

    PG_RETURN_POINTER(array);
}

/**
 * @brief   第一个参数整体编码的边界 BOX2D 类型
 *          第二个参数查询范围的边界 BOX2D 类型
 *          第三个参数 level 等级
 * 
 * @return Datum text
 */
Datum UgGetFilter1(PG_FUNCTION_ARGS)
{
    text *type_text;

    GBOX *indexbox = (GBOX *)PG_GETARG_POINTER(0);
    GBOX *boundbox = (GBOX *)PG_GETARG_POINTER(1);
    YkInt level = PG_GETARG_INT32(2);
    YkInt keysizelimit = PG_GETARG_INT32(3);
    YkString res = UGGeoHash::GetFilter1(YkRect2D(indexbox->xmin, indexbox->ymax, indexbox->xmax, indexbox->ymin),
                                         YkRect2D(boundbox->xmin, boundbox->ymax, boundbox->xmax, boundbox->ymin),
                                         level, keysizelimit);

	const char *str = res.Cstr();
	size_t len = strlen(str);
	type_text = (text *)palloc(len + VARHDRSZ); 
	SET_VARSIZE(type_text, len + VARHDRSZ);
	memcpy(VARDATA(type_text), str, len);

    PG_RETURN_TEXT_P(type_text);
}

/*----------------------------------------geosotgrid function----------------------------------------*/
/**
 * Return geosotgrids of the provided geometry
 * @param grid2d_array : grid results
 * @param grid2d_array_cache : grid cache
 * @param geom : src geometry
 * @param level_min ：min level
 * @param level_max ：max level
 */
void geosotgridagg(vector<GridPos> &grid2d_array, vector<GridPos> &grid2d_array_cache, lwvarlena_t *wkb, GEOSGeometry *g1, int &level_min, int &level_max)
{
	//新等级下的像素
	double pixel_size = GetPixSize(level_min);
	level_min++;

	LWPOLY *poly;
	GBOX gbox;
	POINT4D pt;
	POINTARRAY *pa;
	POINTARRAY **point_array;
	vector<GridPos>::iterator grid2d_it;
	vector<GridPos> grid2d_array1, grid2d_array2;

	//检查新等级下每一个面，是否在原几何图形内，不在的继续提升等级再次栅格化
	for (grid2d_it = grid2d_array_cache.begin(); grid2d_it != grid2d_array_cache.end();)
	{
		grid2d_it->level = level_min - 1;

		double xmin = grid2d_it->x;
		double xmax = grid2d_it->x + pixel_size;
		double ymin = grid2d_it->y;
		double ymax = grid2d_it->y + pixel_size;
		
		point_array = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *));
		pa = ptarray_construct_empty(0, 0, 5);
		point_array[0] = pa;

		if (10 <= grid2d_it->level && grid2d_it->level <= 12)
		{
			int32_t lng_l = int32_t(xmin) / 1;
			int32_t lng_r = int32_t(xmax) / 1;
			int32_t lat_d = int32_t(ymin) / 1;
			int32_t lat_t = int32_t(ymax) / 1;

			if (lng_l < lng_r && xmin < lng_r - pixel_size * 0.1)
				xmax = int32_t(xmax);

			if (lat_d < lat_t && ymin < lat_t - pixel_size * 0.1)
				ymax = int32_t(ymax);
		}
		else if (16 <= grid2d_it->level && grid2d_it->level <= 18)
		{
			int32_t lng_l = int32_t(xmin * 60) / 1;
			int32_t lng_r = int32_t(xmax * 60) / 1;
			int32_t lat_d = int32_t(ymin * 60) / 1;
			int32_t lat_t = int32_t(ymax * 60) / 1;

			if (lng_l < lng_r && xmin < lng_r / 60.0 - pixel_size * 0.1)
				xmax = int32_t(xmax * 60) / 60.0;

			if (lat_d < lat_t && ymin < lat_t / 60.0 - pixel_size * 0.1)
				ymax = int32_t(ymax * 60) / 60.0;
		}

		pt.x = xmin;
		pt.y = ymin;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = xmin;
		pt.y = ymax;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = xmax;
		pt.y = ymax;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = xmax;
		pt.y = ymin;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = xmin;
		pt.y = ymin;
		ptarray_append_point(pa, &pt, LW_TRUE);

		poly = lwpoly_construct(4490, nullptr, 1, point_array);
		GEOSGeometry *g2 = LWGEOM2GEOS((LWGEOM *)poly, 0);
		int rescot = GEOSContains(g1, g2);
		GEOSGeom_destroy(g2);
		// 小块被原几何包含，直接加入返回值
		if (1 == rescot)
		{
			grid2d_array.push_back(*grid2d_it);
			grid2d_it = grid2d_array_cache.erase(grid2d_it);
			lwpoly_free(poly);
			continue;
		}
		// 小块不被原几何包含，但相交，继续栅格化
		else if (0 == rescot)
		{
			// 移除不完全在原几何内的网格，重新栅格化成新网格再补充进去，直到等级等于初始等级
			if (level_min <= level_max)
			{
				size_t wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
				lwgeom_calculate_gbox(lwpoly_as_lwgeom(poly), &gbox);
				GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level_min, 3, grid2d_array1, &gbox);
				//将重新栅格化的小块暂时保存在2里
				for (GridPos &pos : grid2d_array1)
				{
					pos.level = level_min;
					grid2d_array2.push_back(pos);
				}
				grid2d_array1.clear();
			}
			else
			{
				grid2d_array.push_back(*grid2d_it);
			}
			//移除该小块
			grid2d_it = grid2d_array_cache.erase(grid2d_it);
		}
		else
		{
			lwpgerror("%s: %s", "GEOS_ERROR", lwgeom_geos_errmsg);
		}
		lwpoly_free(poly);
	}
	std::sort(grid2d_array2.begin(), grid2d_array2.end());
	grid2d_array2.erase(unique(grid2d_array2.begin(), grid2d_array2.end()), grid2d_array2.end());
	grid2d_array_cache = grid2d_array2;
}

Datum gsg_geosotgridagg(PG_FUNCTION_ARGS)
{
	int32_t data_size;
	uint32_t grid_num;
	size_t grid2d_num;
	size_t wkb_len;
	lwvarlena_t *wkb;
	vector<GridPos> grid2d_array;
	uint8_t variant = WKB_SFSQL;

	int level_max = PG_GETARG_INT32(1);
	int level_min = PG_GETARG_INT32(2);
	int min_level = level_max;
	if (level_max < 1 || level_max > 32 || level_min < 1 || level_min > 32 || level_min >= level_max)
		lwpgerror("The level must be between 1-32 and Maximum level must be greater than minimum level");

	GSERIALIZED *gser = PG_GETARG_GSERIALIZED_P(0);
	LWGEOM *geom = lwgeom_from_gserialized(gser);
	int type = gserialized_get_type(gser);
	int srid = gserialized_get_srid(gser);
	if (srid != 4490)
		lwpgerror("srid must be 4490");

	// 将 lwgeom 对象转化为 wkb 对象
	wkb = lwgeom_to_wkb_varlena(geom, variant);
	wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
	// 针对面做特殊处理
	if (type == POLYGONTYPE || type == MULTIPOLYGONTYPE)
	{
		double area = lwgeom_area(geom);
		// 根据等级预估网格数
		uint64_t num = 0;
		if (12 < level_max && level_max < 19)
			num = 15 * pow(2, level_max - 4);
		else if (18 < level_max)
			num = 900 * pow(2, level_max - 10);
		else
			num = pow(2, level_max);

		vector<GridPos> grid2d_array_cache;
		//超出一定范围，降低编码等级，计算合适范围的精度等级
		int level_new = 0;
		if (area * pow(num / 512.0, 2) >= 4)
		{
			for (int i = level_max, j = 1; i >= 0; i--, j *= 2)
			{
				if (pow(ceil(num / 512.0 / j), 2) * area < 4)
				{
					level_new = i;
					break;
				}
			}
			level_min = level_new > level_min ? level_new : level_min;
			GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level_min, type, grid2d_array_cache);
			initGEOS(lwpgnotice, lwgeom_geos_error);
			GEOSGeometry *g1 = LWGEOM2GEOS(geom, 0);
			//如果有降低编码的情况，循环细分网格，当编码等级到指定等级后，不再继续细分网格
			while (level_max + 1 - level_min)
			{
				geosotgridagg(grid2d_array, grid2d_array_cache, wkb, g1, level_min, level_max);

				if (grid2d_array.size() != 0 && min_level == level_max)
				{
					min_level = level_min - 1;
				}
			}
			GEOSGeom_destroy(g1);
		}
		else
		{
			GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level_max, type, grid2d_array);
			for (GridPos &pos : grid2d_array)
			{
				pos.level = level_max;
			}
		}
	}
	else if (type == POINTTYPE)
	{
		POINT3DZ point3d;
		GridPos pos;
		getPoint3dz_p(((LWPOINT *)geom)->point, 0, &point3d);
		pos.x = point3d.x;
		pos.y = point3d.y;
		pos.level = level_max;
		grid2d_array.push_back(pos);
	}
	else
	{
		GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level_max, type, grid2d_array);
		for (GridPos &pos : grid2d_array)
		{
			pos.level = level_max;
		}
	}
	lwfree(wkb);
	wkb = nullptr;
	lwgeom_free(geom);

	grid2d_num = grid2d_array.size();
	grid_num = grid2d_num;
	Datum *result_array_data = (Datum *)palloc(grid_num * sizeof(Datum));
	data_size = 16;
	for (size_t i = 0; i < grid2d_num; i++)
	{
		uint64_t grid_code = GetCode(grid2d_array.at(i).x, grid2d_array.at(i).y, grid2d_array.at(i).level);
		GEOSOTGRID *val = (GEOSOTGRID *)palloc0(GEOSOTGRIDSIZE);
		SET_VARSIZE(val, GEOSOTGRIDSIZE);
		val->flag = 0;
		val->level = grid2d_array.at(i).level;
		val->level_min = min_level;
		val->data = grid_code;

		result_array_data[i] = Datum(val);
	}

	Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
	Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geosotgrid"), ObjectIdGetDatum(namespaceId));

	int16 elmlen;
	bool elmbyval;
	char elmalign;
	get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
	ArrayType *result = (ArrayType *)construct_array(result_array_data, grid_num, typ_ID, data_size, elmbyval, elmalign);

	PG_RETURN_POINTER(result);
}

Datum gsg_geosotgrid(PG_FUNCTION_ARGS)
{
	int z_num;
	size_t grid2d_num;
	uint32_t grid_num;
	size_t wkb_len;
	lwvarlena_t *wkb;
	
	Datum *result_array_data;
	vector<GridPos> grid2d_array;
	uint8_t variant = WKB_SFSQL;

    int level = PG_GETARG_INT32(1);
    if (level < 1 || level > 32)
        lwpgerror("The level must be between 1-32");

    GSERIALIZED *gser = PG_GETARG_GSERIALIZED_P(0);
    LWGEOM *geom = lwgeom_from_gserialized(gser);
    int type = gserialized_get_type(gser);
    int srid = gserialized_get_srid(gser);
    if (srid != 4490)
        lwpgerror("srid must be 4490");

	uint16_t has_z = gserialized_has_z(gser);
	int data_size = has_z == 0 ? GEOSOTGRIDSIZE : GEOSOTGRID3DSIZE;
	if (0 == has_z)
	{
		if (POINTTYPE == type)
		{
			POINT3DZ point3d;
			GridPos pos;
			getPoint3dz_p(((LWPOINT *)geom)->point, 0, &point3d);
			pos.x = point3d.x;
			pos.y = point3d.y;
			pos.level = level;
			grid2d_array.push_back(pos);
		}
		else
		{
			// 将 lwgeom 对象转化为 wkb 对象
			wkb = lwgeom_to_wkb_varlena(geom, variant);
			wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
			// 栅格化
			GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level, type, grid2d_array);
			lwfree(wkb);
			wkb = nullptr;
		}

		grid2d_num = grid2d_array.size();
		grid_num = grid2d_num;
		result_array_data = (Datum *)palloc(grid_num * sizeof(Datum));
		for (size_t i = 0; i < grid2d_num; i++)
		{
			uint64_t grid_code = GetCode(grid2d_array.at(i).x, grid2d_array.at(i).y, level);
			GEOSOTGRID *val = (GEOSOTGRID *)palloc0(GEOSOTGRIDSIZE);
			SET_VARSIZE(val, GEOSOTGRIDSIZE);
			val->flag = has_z;
			val->level = level;
			val->level_min = level;
			val->data = grid_code;
			result_array_data[i] = Datum(val);
		}
	}
	else
	{
		GBOX gbox;
		lwgeom_calculate_gbox(geom, &gbox);
		BOX3D *box = box3d_from_gbox(&gbox);
		double z_min = Min(box->zmin, box->zmax);
		double z_max = Max(box->zmin, box->zmax);
		if (POLYHEDRALSURFACETYPE == type)
		{
			double x_min = Min(box->xmin, box->xmax);
			double x_max = Max(box->xmin, box->xmax);
			double y_min = Min(box->ymin, box->ymax);
			double y_max = Max(box->ymin, box->ymax);
			LWGEOM *geom2d = lwpoly_as_lwgeom(lwpoly_construct_envelope(4490, x_min, y_min, x_max, y_max));
			wkb = lwgeom_to_wkb_varlena(geom2d, variant);
			lwgeom_free(geom2d);
		}
		else
			wkb = lwgeom_to_wkb_varlena(geom, variant);

		wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
		GridRasterize((unsigned char *)wkb->data, wkb_len, nullptr, level, type, grid2d_array);
		lwfree(wkb);
		wkb = nullptr;

		int z_begin = AltitudeToInt(z_min, level);
		int z_end 	= AltitudeToInt(z_max, level);
		z_num 		= z_end - z_begin + 1;
		grid2d_num 	= grid2d_array.size();
		grid_num 	= z_num * grid2d_num;

		result_array_data = (Datum *)palloc(grid_num * sizeof(Datum));

		bitset<96> grid_code;
		uint32_t k = 0;
		for (; z_num > 0; z_num--)
		{
			for (GridPos pos : grid2d_array)
			{
				grid_code = GetCode(pos.x, pos.y, z_begin, level);

				GEOSOTGRID3D *val = (GEOSOTGRID3D *)palloc0(GEOSOTGRID3DSIZE);
				SET_VARSIZE(val, GEOSOTGRID3DSIZE);
				val->flag = has_z;
				val->level = level;
				string str_grid = grid_code.to_string();
				string str_a = string(str_grid.begin(), str_grid.begin() + 32);
				string str_b = string(str_grid.begin() + 32, str_grid.begin() + 64);
				string str_c = string(str_grid.begin() + 64, str_grid.end());

				uint32_t la = stoll(str_a, nullptr, 2);
				uint32_t lb = stoll(str_b, nullptr, 2);
				uint32_t lc = stoll(str_c, nullptr, 2);

				memcpy(val->data, &lc, 4);
				memcpy(val->data + 4, &lb, 4);
				memcpy(val->data + 8, &la, 4);

				result_array_data[k++] = Datum(val);
			}
			z_begin++;
		}
	}
	lwgeom_free(geom);

    Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
    Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geosotgrid"), ObjectIdGetDatum(namespaceId));

    int16 elmlen;
    bool elmbyval;
    char elmalign;
    get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
    ArrayType *result = (ArrayType *)construct_array(result_array_data, grid_num, typ_ID, data_size, elmbyval, elmalign);

	PG_RETURN_POINTER(result);
}

Datum gsg_mingeosotgrid(PG_FUNCTION_ARGS)
{
	int level_ = 0;
	GSERIALIZED *gser = PG_GETARG_GSERIALIZED_P(0);
	LWGEOM *geom = lwgeom_from_gserialized(gser);
	int srid = gserialized_get_srid(gser);
	if (srid != 4490)
		lwpgerror("srid must be 4490");

	GBOX gbox;
	lwgeom_calculate_gbox(geom, &gbox);
	BOX3D *box = box3d_from_gbox(&gbox);
	double xmin = box->xmin;
	double xmax = box->xmax;
	double ymin = box->ymin;
	double ymax = box->ymax;
	uint64_t pt1 = GetCode(xmin, ymin, 32);
	uint64_t pt2 = GetCode(xmin, ymax, 32);
	uint64_t pt3 = GetCode(xmax, ymax, 32);
	uint64_t pt4 = GetCode(xmax, ymin, 32);
	for (int level = 32; level > 0; level--)
	{
		uint64_t ppt1 = pt1 & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		uint64_t ppt2 = pt2 & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		uint64_t ppt3 = pt3 & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		uint64_t ppt4 = pt4 & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		if (ppt1 == ppt2 && ppt2 == ppt3 && ppt3 == ppt4 && ppt4 == ppt1)
		{
			level_ = level;
			break;
		}
	}

	GEOSOTGRID *buf_data = (GEOSOTGRID *)palloc0(GEOSOTGRIDSIZE);
	SET_VARSIZE(buf_data, GEOSOTGRIDSIZE);
	buf_data->flag = 0;
	buf_data->level = level_;
	buf_data->level_min = level_;
	buf_data->data = pt1 & (0XFFFFFFFFFFFFFFFF << (64 - level_ * 2));

	lwgeom_free(geom);
	lwfree(box);
	PG_FREE_IF_COPY(gser, 0);
	PG_RETURN_POINTER(buf_data);
}

Datum gsg_geosotgrid_z(PG_FUNCTION_ARGS)
{
	double altitude = PG_GETARG_FLOAT8(0);
	int level = PG_GETARG_INT32(1);
	int code = AltitudeToInt(altitude, level);
	PG_RETURN_INT32(code);
}

Datum gsg_as_altitude(PG_FUNCTION_ARGS)
{
	int32_t code = PG_GETARG_INT32(0);
	int level = PG_GETARG_INT32(1);
	double altitude = IntToAltitude(code, level);
	PG_RETURN_FLOAT8(altitude);
}

Datum gsg_geosotgrid_from_text(PG_FUNCTION_ARGS)
{
	char *input = PG_GETARG_CSTRING(0);
	string str_in;
	str_in += input;

	int16_t level;
	uint64_t code;
	ToCode(str_in, level, code);

	GEOSOTGRID *buf_data = (GEOSOTGRID *)palloc0(GEOSOTGRIDSIZE);
	SET_VARSIZE(buf_data, GEOSOTGRIDSIZE);
	buf_data->flag = 0;
	buf_data->level = level;
	buf_data->level_min = level;
	buf_data->data = code;

	PG_RETURN_POINTER(buf_data);
}

Datum gsg_geosotgrid3d_from_text(PG_FUNCTION_ARGS)
{
	char *grid2d = PG_GETARG_CSTRING(0);
	char *grid_z = PG_GETARG_CSTRING(1);
	string str_grid2d;
	str_grid2d += grid2d;

	int16_t level;
	uint64_t code_2d;
	ToCode(str_grid2d, level, code_2d);
	uint32_t x, y;
	UnMagicBits(code_2d, x, y);

	int z = std::stoll(grid_z, nullptr, 2);
	bitset<96> code_3d =  MagicBitset(x, y, z);

	GEOSOTGRID3D *buf_data = (GEOSOTGRID3D *)palloc0(GEOSOTGRID3DSIZE);
	SET_VARSIZE(buf_data, GEOSOTGRID3DSIZE);
	buf_data->flag = 1;
	buf_data->level = level;
	string str_grid3d = code_3d.to_string();
	string a = string(str_grid3d.begin(), str_grid3d.begin() + 32);
	string b = string(str_grid3d.begin() + 32, str_grid3d.begin() + 64);
	string c = string(str_grid3d.begin() + 64, str_grid3d.end());

	int64_t la = stoll(a, nullptr, 2);
	int64_t lb = stoll(b, nullptr, 2);
	int64_t lc = stoll(c, nullptr, 2);

	memcpy(buf_data->data, &lc, 4);
	memcpy(buf_data->data + 4, &lb, 4);
	memcpy(buf_data->data + 8, &la, 4);

	PG_RETURN_POINTER(buf_data);
}

Datum gsg_geosotgrid_as_text(PG_FUNCTION_ARGS)
{
	text *output = nullptr;
	size_t len;

	varlena *buf = PG_GETARG_VARLENA_P(0);
	int size = VARSIZE(buf);
	if (GEOSOTGRIDSIZE == size)
	{
		GEOSOTGRID *p_grid = PointerGetGEOSOTGrid(buf);
		uint64_t grid_code = p_grid->data;
		uint16_t level = p_grid->level;
		string str_grid = ToString(grid_code, level);
		const char *grid = str_grid.c_str();
		len = strlen(grid);
		output = (text *)palloc(len + 4);
		SET_VARSIZE(output, len + 4);
		memcpy(VARDATA(output), grid, len);
	}
	else if(GEOSOTGRID3DSIZE == size)
	{
		GEOSOTGRID3D *p_grid3d = PointerGetGEOSOTGrid3D(buf);
		uint16_t level = p_grid3d->level;
		unsigned char *data = p_grid3d->data;
		bitset<96> data_a, data_b, data_c, grid_code;

		data_a = POINTERGETUINT32(data + 8);
		data_b = POINTERGETUINT32(data + 4);
		data_c = POINTERGETUINT32(data);
		grid_code = (data_a << 64) | (data_b << 32) | data_c;

		uint32_t grid_x = MergeByBitset(grid_code);
		uint32_t grid_y = MergeByBitset(grid_code >> 1);
		uint64_t grid_xy = MagicBits(grid_x, grid_y);
		string str_grid_2d = ToString(grid_xy, level);
		const char *grid_2d = str_grid_2d.c_str();

		uint32_t val_z = MergeByBitset(grid_code >> 2);
		string str_grid_z = "";
		while (val_z)
		{
			if (val_z % 2 == 0)
				str_grid_z = '0' + str_grid_z;
			else
				str_grid_z = '1' + str_grid_z;
			val_z /= 2;
		}
		const char *grid_z = str_grid_z.c_str();
		size_t grid_z_len = strlen(grid_z);

		if (grid_z_len != 0)
		{
			str_grid_z = ", " + str_grid_z;
			grid_z = str_grid_z.c_str();
			grid_z_len += 2;
		}
		else
		{
			str_grid_z = ", 0" + str_grid_z;
			grid_z = str_grid_z.c_str();
			grid_z_len += 3;
		}

		size_t grid2d_len = strlen(grid_2d);
		len = grid2d_len + grid_z_len;
		output = (text *)palloc(len + 4);
		SET_VARSIZE(output, len + 4);
		memcpy(VARDATA(output), grid_2d, grid2d_len);
		memcpy(VARDATA(output) + grid2d_len, grid_z, grid_z_len);
	}

	PG_RETURN_TEXT_P(output);
}

Datum gsg_geom_from_geosotgrid(PG_FUNCTION_ARGS)
{
	GSERIALIZED *result = nullptr;
	uint x, y, z;
	double min_lng, min_lat, min_ele;
	double max_lng, max_lat, max_ele;

	varlena *buf = PG_GETARG_VARLENA_P(0);
	int size = VARSIZE(buf);
	if (GEOSOTGRIDSIZE == size)
	{
		GEOSOTGRID *p_grid = PointerGetGEOSOTGrid(buf);
		uint16_t level = p_grid->level;
		// 获取当前等级下的像素大小
		double pixel_size = GetPixSize(level);
		uint64_t grid_code = p_grid->data;
		// 分离出x、y的编码
		UnMagicBits(grid_code, x, y);
		min_lng = Code2Dec(x);
		min_lat = Code2Dec(y);
		// 以像素大小为边，返回矩形地理范围
		max_lng = min_lng + pixel_size;
		max_lat = min_lat + pixel_size;

		// 分级网格不规则划分时，靠度级边界线网格最大最小值必在度级虚线两侧
		// 如最小0.9 最大1.1，取前面的整数值，不一致则说明此处为不规则网格
		// 此时将最大值赋予整数值
		if (10 <= level && level <= 12)
		{
			int32_t lng_l = int32_t(min_lng) / 1;
			int32_t lng_r = int32_t(max_lng) / 1;
			int32_t lat_d = int32_t(min_lat) / 1;
			int32_t lat_t = int32_t(max_lat) / 1;

			if (lng_l < lng_r && min_lng < lng_r - pixel_size * 0.1)
				max_lng = int32_t(max_lng);

			if (lat_d < lat_t && min_lat < lat_t - pixel_size * 0.1)
				max_lat = int32_t(max_lat);
		}
		// 此处为秒级网格不规则划分，放大60倍，则可模拟上述效果
		else if (16 <= level && level <= 18)
		{
			int32_t lng_l = int32_t(min_lng * 60) / 1;
			int32_t lng_r = int32_t(max_lng * 60) / 1;
			int32_t lat_d = int32_t(min_lat * 60) / 1;
			int32_t lat_t = int32_t(max_lat * 60) / 1;

			if (lng_l < lng_r && min_lng < lng_r / 60.0 - pixel_size * 0.1)
				max_lng = int32_t(max_lng * 60) / 60.0;

			if (lat_d < lat_t && min_lat < lat_t / 60.0 - pixel_size * 0.1)
				max_lat = int32_t(max_lat * 60) / 60.0;
		}

		POINTARRAY **ppa = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *));
		POINTARRAY *pa = ptarray_construct_empty(0, 0, 5);
		ppa[0] = pa;

		POINT4D pt;
		pt.x = min_lng;
		pt.y = min_lat;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = min_lng;
		pt.y = max_lat;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = max_lng;
		pt.y = max_lat;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = max_lng;
		pt.y = min_lat;
		ptarray_append_point(pa, &pt, LW_TRUE);
		pt.x = min_lng;
		pt.y = min_lat;
		ptarray_append_point(pa, &pt, LW_TRUE);

		LWPOLY *poly = lwpoly_construct(4490, nullptr, 1, ppa);
		result = geometry_serialize(lwpoly_as_lwgeom(poly));
		gserialized_set_srid(result, 4490);
		lwpoly_free(poly);
	}
	else if(GEOSOTGRID3DSIZE == size)
	{
		GEOSOTGRID3D *p_grid3d = PointerGetGEOSOTGrid3D(buf);
		uint16_t level = p_grid3d->level;
		double pixel_size = GetPixSize(level);
		bitset<96> data_a, data_b, data_c, grid3d_code;
		data_a = POINTERGETUINT32(p_grid3d->data + 8);
		data_b = POINTERGETUINT32(p_grid3d->data + 4);
		data_c = POINTERGETUINT32(p_grid3d->data);
		grid3d_code = (data_a << 64) | (data_b << 32) | data_c;
		UnMagicBitset(grid3d_code, x, y, z);
		min_lng = Code2Dec(x);
		min_lat = Code2Dec(y);
		min_ele = IntToAltitude(z, level);
		max_ele = IntToAltitude(z + 1, level);
		max_lng = min_lng + pixel_size;
		max_lat = min_lat + pixel_size;
		if (10 <= level && level <= 12)
		{
			int32_t lng_l = int32_t(min_lng) / 1;
			int32_t lng_r = int32_t(max_lng) / 1;
			int32_t lat_d = int32_t(min_lat) / 1;
			int32_t lat_t = int32_t(max_lat) / 1;

			if (lng_l < lng_r && min_lng < lng_r - pixel_size * 0.1)
				max_lng = int32_t(max_lng);

			if (lat_d < lat_t && min_lat < lat_t - pixel_size * 0.1)
				max_lat = int32_t(max_lat);
		}
		else if (16 <= level && level <= 18)
		{
			int32_t lng_l = int32_t(min_lng * 60) / 1;
			int32_t lng_r = int32_t(max_lng * 60) / 1;
			int32_t lat_d = int32_t(min_lat * 60) / 1;
			int32_t lat_t = int32_t(max_lat * 60) / 1;

			if (lng_l < lng_r && min_lng < lng_r / 60.0 - pixel_size * 0.1)
				max_lng = int32_t(max_lng * 60) / 60.0;

			if (lat_d < lat_t && min_lat < lat_t / 60.0 - pixel_size * 0.1)
				max_lat = int32_t(max_lat * 60) / 60.0;
		}

		POINT4D points[8];
		static const int ngeoms = 6;
		LWGEOM **geoms = (LWGEOM **)lwalloc(sizeof(LWGEOM *) * ngeoms);
		LWGEOM *geom = NULL;

		/* Initialize the 8 vertices of the box */
		points[0] = (POINT4D){min_lng, min_lat, min_ele, 0.0};
		points[1] = (POINT4D){min_lng, max_lat, min_ele, 0.0};
		points[2] = (POINT4D){max_lng, max_lat, min_ele, 0.0};
		points[3] = (POINT4D){max_lng, min_lat, min_ele, 0.0};
		points[4] = (POINT4D){min_lng, min_lat, max_ele, 0.0};
		points[5] = (POINT4D){min_lng, max_lat, max_ele, 0.0};
		points[6] = (POINT4D){max_lng, max_lat, max_ele, 0.0};
		points[7] = (POINT4D){max_lng, min_lat, max_ele, 0.0};

		/* add bottom polygon */
		geoms[0] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[0], &points[1], &points[2], &points[3]));
		/* add top polygon */
		geoms[1] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[4], &points[7], &points[6], &points[5]));
		/* add left polygon */
		geoms[2] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[0], &points[4], &points[5], &points[1]));
		/* add right polygon */
		geoms[3] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[3], &points[2], &points[6], &points[7]));
		/* add front polygon */
		geoms[4] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[0], &points[3], &points[7], &points[4]));
		/* add back polygon */
		geoms[5] = lwpoly_as_lwgeom(
		    lwpoly_construct_rectangle(LW_TRUE, LW_FALSE, &points[1], &points[5], &points[6], &points[2]));

		geom = (LWGEOM *)lwcollection_construct(POLYHEDRALSURFACETYPE, SRID_UNKNOWN, NULL, ngeoms, geoms);

		FLAGS_SET_SOLID(geom->flags, 1);
		result = geometry_serialize(geom);
		lwcollection_free((LWCOLLECTION *)geom);
		gserialized_set_srid(result, 4490);
	}
	PG_FREE_IF_COPY(buf, 0);
	PG_RETURN_POINTER(result);
}

Datum gsg_geom_from_geosotgrid_array(PG_FUNCTION_ARGS)
{
	bool isnull;
	Datum value;

	ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
	int nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
	ArrayIterator iterator = array_create_iterator(array, 0);
	Datum *result_array_data = (Datum *)palloc(nelems * sizeof(Datum));

	int32_t i = 0;
	while (array_iterate(iterator, &value, &isnull))
	{
		if (isnull)
			continue;

		//获取当前等级下的像素大小
		varlena *buf = (varlena *)DatumGetPointer(value);
		int size = VARSIZE(buf);
		
		uint x, y, z;
		double min_lng, min_lat, min_ele;
		double max_lng, max_lat, max_ele;
		if (GEOSOTGRIDSIZE == size)
		{
			GEOSOTGRID *p_grid = PointerGetGEOSOTGrid(buf);
			uint16_t level = p_grid->level;
			double pixel_size = GetPixSize(level);
			uint64_t grid_code = p_grid->data;
			UnMagicBits(grid_code, x, y);
			//将x、y的编码转为十进制坐标
			min_lng = Code2Dec(x);
			min_lat = Code2Dec(y);

			//以像素大小为边，返回矩形地理范围
			max_lng = min_lng + pixel_size;
			max_lat = min_lat + pixel_size;

			if (10 <= level && level <= 12)
			{
				int32_t lng_l = int32_t(min_lng) / 1;
				int32_t lng_r = int32_t(max_lng) / 1;
				int32_t lat_d = int32_t(min_lat) / 1;
				int32_t lat_t = int32_t(max_lat) / 1;

				if (lng_l < lng_r && min_lng < lng_r - pixel_size * 0.1)
					max_lng = int32_t(max_lng);

				if (lat_d < lat_t && min_lat < lat_t - pixel_size * 0.1)
					max_lat = int32_t(max_lat);
			}
			else if (16 <= level && level <= 18)
			{
				int32_t lng_l = int32_t(min_lng * 60) / 1;
				int32_t lng_r = int32_t(max_lng * 60) / 1;
				int32_t lat_d = int32_t(min_lat * 60) / 1;
				int32_t lat_t = int32_t(max_lat * 60) / 1;

				if (lng_l < lng_r && min_lng < lng_r / 60.0 - pixel_size * 0.1)
					max_lng = int32_t(max_lng * 60) / 60.0;

				if (lat_d < lat_t && min_lat < lat_t / 60.0 - pixel_size * 0.1)
					max_lat = int32_t(max_lat * 60) / 60.0;
			}

			POINTARRAY **ppa = (POINTARRAY **)lwalloc(sizeof(POINTARRAY *));
			POINTARRAY *pa = ptarray_construct_empty(0, 0, 5);
			ppa[0] = pa;

			POINT4D pt;
			pt.x = min_lng;
			pt.y = min_lat;
			ptarray_append_point(pa, &pt, LW_TRUE);
			pt.x = min_lng;
			pt.y = max_lat;
			ptarray_append_point(pa, &pt, LW_TRUE);
			pt.x = max_lng;
			pt.y = max_lat;
			ptarray_append_point(pa, &pt, LW_TRUE);
			pt.x = max_lng;
			pt.y = min_lat;
			ptarray_append_point(pa, &pt, LW_TRUE);
			pt.x = min_lng;
			pt.y = min_lat;
			ptarray_append_point(pa, &pt, LW_TRUE);

			LWPOLY *poly = lwpoly_construct(4490, nullptr, 1, ppa);
			GSERIALIZED *gser;
			gser = geometry_serialize(lwpoly_as_lwgeom(poly));
			lwpoly_free(poly);
			gserialized_set_srid(gser, 4490);
			result_array_data[i++] = (Datum)gser;
		}
		else if (GEOSOTGRID3DSIZE == size)
		{
			GEOSOTGRID3D *p_grid3d = PointerGetGEOSOTGrid3D(buf);
			uint16_t level = p_grid3d->level;
			double pixel_size = GetPixSize(level);
			bitset<96> data_a, data_b, data_c, grid3d_code;
			data_a = POINTERGETUINT32(p_grid3d->data + 8);
			data_b = POINTERGETUINT32(p_grid3d->data + 4);
			data_c = POINTERGETUINT32(p_grid3d->data);
			grid3d_code = (data_a << 64) | (data_b << 32) | data_c;
			UnMagicBitset(grid3d_code, x, y, z);
			min_lng = Code2Dec(x);
			min_lat = Code2Dec(y);
			min_ele = IntToAltitude(z, level);
			max_ele = IntToAltitude(z + 1, level);
			max_lng = min_lng + pixel_size;
			max_lat = min_lat + pixel_size;
			if (10 <= level && level <= 12)
			{
				int32_t lng_l = int32_t(min_lng) / 1;
				int32_t lng_r = int32_t(max_lng) / 1;
				int32_t lat_d = int32_t(min_lat) / 1;
				int32_t lat_t = int32_t(max_lat) / 1;

				if (lng_l < lng_r && min_lng < lng_r - pixel_size * 0.1)
					max_lng = int32_t(max_lng);

				if (lat_d < lat_t && min_lat < lat_t - pixel_size * 0.1)
					max_lat = int32_t(max_lat);
			}
			else if (16 <= level && level <= 18)
			{
				int32_t lng_l = int32_t(min_lng * 60) / 1;
				int32_t lng_r = int32_t(max_lng * 60) / 1;
				int32_t lat_d = int32_t(min_lat * 60) / 1;
				int32_t lat_t = int32_t(max_lat * 60) / 1;

				if (lng_l < lng_r && min_lng < lng_r / 60.0 - pixel_size * 0.1)
					max_lng = int32_t(max_lng * 60) / 60.0;

				if (lat_d < lat_t && min_lat < lat_t / 60.0 - pixel_size * 0.1)
					max_lat = int32_t(max_lat * 60) / 60.0;
			}

			POINT4D points[8];
			static const int ngeoms = 6;
			LWGEOM **geoms = (LWGEOM **)lwalloc(sizeof(LWGEOM *) * ngeoms);
			LWGEOM *geom = NULL;

			/* Initialize the 8 vertices of the box */
			points[0] = (POINT4D){min_lng, min_lat, min_ele, 0.0};
			points[1] = (POINT4D){min_lng, max_lat, min_ele, 0.0};
			points[2] = (POINT4D){max_lng, max_lat, min_ele, 0.0};
			points[3] = (POINT4D){max_lng, min_lat, min_ele, 0.0};
			points[4] = (POINT4D){min_lng, min_lat, max_ele, 0.0};
			points[5] = (POINT4D){min_lng, max_lat, max_ele, 0.0};
			points[6] = (POINT4D){max_lng, max_lat, max_ele, 0.0};
			points[7] = (POINT4D){max_lng, min_lat, max_ele, 0.0};

			/* add bottom polygon */
			geoms[0] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[0], &points[1], &points[2], &points[3]));
			/* add top polygon */
			geoms[1] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[4], &points[7], &points[6], &points[5]));
			/* add left polygon */
			geoms[2] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[0], &points[4], &points[5], &points[1]));
			/* add right polygon */
			geoms[3] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[3], &points[2], &points[6], &points[7]));
			/* add front polygon */
			geoms[4] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[0], &points[3], &points[7], &points[4]));
			/* add back polygon */
			geoms[5] = lwpoly_as_lwgeom(lwpoly_construct_rectangle(
			    LW_TRUE, LW_FALSE, &points[1], &points[5], &points[6], &points[2]));

			geom = (LWGEOM *)lwcollection_construct(POLYHEDRALSURFACETYPE, SRID_UNKNOWN, NULL, ngeoms, geoms);

			FLAGS_SET_SOLID(geom->flags, 1);
			GSERIALIZED *gser = geometry_serialize(geom);
			lwcollection_free((LWCOLLECTION *)geom);
			gserialized_set_srid(gser, 4490);
			result_array_data[i++] = (Datum)gser;
		}
		else
			elog(ERROR, "Convert to geometry: unknown flag number");
	}
	array_free_iterator(iterator);

	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
	Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geometry"), ObjectIdGetDatum(namespaceId));

	get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
	ArrayType *result = construct_array(result_array_data, nelems, typ_ID, elmlen, elmbyval, elmalign);
	PG_RETURN_POINTER(result);
}

Datum gsg_has_z(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	int size = VARSIZE(buf);
	if (GEOSOTGRIDSIZE == size)
		PG_RETURN_BOOL(false);
	else if (GEOSOTGRID3DSIZE == size)
		PG_RETURN_BOOL(true);
	else
		elog(ERROR, "unkown tpye");
}

Datum gsg_get_level(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	int16_t level = POINTERGETUINT8(buf->vl_dat + 2);
	PG_RETURN_INT16(level);
}

Datum gsg_get_level_extremum(PG_FUNCTION_ARGS)
{
	uint16_t level_max = 0;
	uint16_t level_min = 32;
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
	int nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
	if (nelems == 0)
		PG_RETURN_NULL();

	Datum *result_array_data = (Datum *)palloc(2 * sizeof(Datum));
	bool isnull;
	Datum value;
	ArrayIterator iterator = array_create_iterator(array, 0);
	while (array_iterate(iterator, &value, &isnull))
	{
		if (isnull)
			continue;
		uint16_t has_z = POINTERGETUINT16(DatumGetPointer(value) + 4);
		if (0 == has_z)
		{
			GEOSOTGRID *grid = PointerGetGEOSOTGrid(DatumGetPointer(value));
			level_max = grid->level > level_max ? grid->level : level_max;
			level_min = grid->level_min < level_min ? grid->level_min : level_min;
		}
		else
		{
			PG_RETURN_NULL();
		}
	}
	result_array_data[0] = (Datum)level_min;
	result_array_data[1] = (Datum)level_max;
	ArrayType *result = construct_array(result_array_data, 2, INT2OID, 2, true, 's');
	PG_RETURN_POINTER(result);
}

Datum gsg_aggregate(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	int size = VARSIZE(buf);
	int level = PG_GETARG_INT32(1);

	char *buf_data = (char *)palloc0(size);
	SET_VARSIZE(buf_data, size);
	if (GEOSOTGRIDSIZE == size)
	{
		GEOSOTGRID *p_grid = PointerGetGEOSOTGrid(buf);
		uint16_t grid_level = p_grid->level;
		if (level > grid_level)
		{
			lwpgerror("can't convert to higher precision");
		}
		else if (level == grid_level)
		{
			PG_RETURN_POINTER(p_grid);
		}
		else
		{
			GEOSOTGRID *res_grid = PointerGetGEOSOTGrid(buf_data);
			uint64_t code = p_grid->data;
			code = code & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
			res_grid->size = p_grid->size;
			res_grid->flag = p_grid->flag;
			res_grid->level = level;
			res_grid->level_min = p_grid->level_min < level ? p_grid->level_min : level;
			res_grid->data = code;
		}
	}
	else if (GEOSOTGRID3DSIZE == size)
	{
		GEOSOTGRID3D *p_grid3d = PointerGetGEOSOTGrid3D(buf);
		uint16_t grid3d_level = p_grid3d->level;
		if (level > grid3d_level)
		{
			lwpgerror("can't convert to higher precision");
		}
		else if (level == grid3d_level)
		{
			PG_RETURN_POINTER(p_grid3d);
		}
		else
		{
			GEOSOTGRID3D *res_grid3d = PointerGetGEOSOTGrid3D(buf_data);
			bitset<96> data_a, data_b, data_c, grid_code;
			data_a = POINTERGETUINT32(p_grid3d->data + 8);
			data_b = POINTERGETUINT32(p_grid3d->data + 4);
			data_c = POINTERGETUINT32(p_grid3d->data);
			grid_code = (data_a << 64) | (data_b << 32) | data_c;

			uint x, y, z;
			UnMagicBitset(grid_code, x, y, z);
			x = x >> (32 - level) << (32 - level);
			y = y >> (32 - level) << (32 - level);

			double height = IntToAltitude(z, grid3d_level);
			z = AltitudeToInt(height, level);
			string str_grid = MagicBitset(x, y, z).to_string();
			string str_a = string(str_grid.begin(), str_grid.begin() + 32);
			string str_b = string(str_grid.begin() + 32, str_grid.begin() + 64);
			string str_c = string(str_grid.begin() + 64, str_grid.end());

			int64_t la = stoll(str_a, nullptr, 2);
			int64_t lb = stoll(str_b, nullptr, 2);
			int64_t lc = stoll(str_c, nullptr, 2);
			res_grid3d->size = p_grid3d->size;
			res_grid3d->flag = p_grid3d->flag;
			res_grid3d->level = level;
			memcpy(res_grid3d->data, &lc, 4);
			memcpy(res_grid3d->data + 4, &lb, 4);
			memcpy(res_grid3d->data + 8, &la, 4);
		}
	}

	PG_RETURN_POINTER(buf_data);
}

Datum gsg_aggregate_array(PG_FUNCTION_ARGS)
{
	int data_size = 0;
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
	int level = PG_GETARG_INT32(1);
	int nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));

	if (nelems == 0)
		PG_RETURN_NULL();

	Datum *result_array_data = (Datum *)palloc(nelems * sizeof(Datum));

	bool isnull;
	int i = 0;
	Datum value;
	ArrayIterator iterator = array_create_iterator(array, 0);
	while (array_iterate(iterator, &value, &isnull))
	{
		if (isnull)
			continue;

		uint16_t has_z = POINTERGETUINT16(DatumGetPointer(value) + 4);
		if (0 == has_z)
		{
			GEOSOTGRID *grid = PointerGetGEOSOTGrid(DatumGetPointer(value));
			uint16_t grid_level = grid->level;
			uint64_t code = grid->data;
			if (level > grid_level)
			{
				lwpgerror("can't convert to higher precision");
			}
			else if (level == grid_level) {}
			else
			{
				code = code & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
			}

			data_size = 16;
			GEOSOTGRID *buf_data = (GEOSOTGRID *)palloc0(GEOSOTGRIDSIZE);
			SET_VARSIZE(buf_data, GEOSOTGRIDSIZE);
			buf_data->flag = has_z;
			buf_data->level = level;
			buf_data->level_min = grid->level_min < level ? grid->level_min : level;
			buf_data->data = code;

			result_array_data[i++] = Datum(buf_data);
		}
		else if (1 == has_z)
		{
			data_size = 20;
			GEOSOTGRID3D *buf_data = (GEOSOTGRID3D *)palloc0(GEOSOTGRID3DSIZE);
			SET_VARSIZE(buf_data, GEOSOTGRID3DSIZE);
			buf_data->flag = has_z;
			buf_data->level = level;
			GEOSOTGRID3D *grid3d = PointerGetGEOSOTGrid3D(DatumGetPointer(value));
			uint16_t grid_level = grid3d->level;
			if (level > grid_level)
			{
				lwpgerror("can't convert to higher precision");
			}
			else if (level == grid_level) 
			{
				memcpy(buf_data + 8, grid3d->data, 12);
			}
			else
			{
				bitset<96> a, b, c, grid_code;
				a = POINTERGETUINT32(grid3d->data + 8);
				b = POINTERGETUINT32(grid3d->data + 4);
				c = POINTERGETUINT32(grid3d->data);
				grid_code = (a << 64) | (b << 32) | c;

				uint x, y, z;
				UnMagicBitset(grid_code, x, y, z);
				x = x >> (32 - level) << (32 - level);
				y = y >> (32 - level) << (32 - level);
				double height = IntToAltitude(z, grid_level);
				z = AltitudeToInt(height, level);
				string str_grid = MagicBitset(x, y, z).to_string();
				string str_a = string(str_grid.begin(), str_grid.begin() + 32);
				string str_b = string(str_grid.begin() + 32, str_grid.begin() + 64);
				string str_c = string(str_grid.begin() + 64, str_grid.end());

				int64_t la = stoll(str_a, nullptr, 2);
				int64_t lb = stoll(str_b, nullptr, 2);
				int64_t lc = stoll(str_c, nullptr, 2);
				memcpy(buf_data->data, &lc, 4);
				memcpy(buf_data->data + 4, &lb, 4);
				memcpy(buf_data->data + 8, &la, 4);
			}
			result_array_data[i++] = Datum(buf_data);
		}
		else
			elog(ERROR, "unknown flag number: %d", has_z);
	}

	int16 elmlen;
	bool elmbyval;
	char elmalign;

	Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
	Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geosotgrid"), ObjectIdGetDatum(namespaceId));
	get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
	ArrayType *result = construct_array(result_array_data, nelems, typ_ID, data_size, elmbyval, elmalign);
	if (GEOSOTGRIDSIZE == data_size)
	{
		GEOSOTGRID *gridarray = PointerGetGEOSOTGrid(ARR_DATA_PTR(result));
		std::sort(gridarray, gridarray + nelems);
		result = array_grid2d_unique(result);
	}
	else if (GEOSOTGRID3DSIZE == data_size)
	{
		GEOSOTGRID3D *gridarray = PointerGetGEOSOTGrid3D(ARR_DATA_PTR(result));
		std::sort(gridarray, gridarray + nelems);
		result = array_grid3d_unique(result);
	}

	PG_RETURN_POINTER(result);
}

static void handleInterrupt(int sig)
{
	lwgeom_request_interrupt();

	if (coreIntHandler)
	{
		(*coreIntHandler)(sig);
	}
}

void _PG_init(void)
{
	coreIntHandler = pqsignal(18, handleInterrupt);
	pg_install_lwgeom_handlers();
}

void _PG_fini(void)
{
	elog(18, "Goodbye from Yukon GeosotGrid %s", POSTGIS_VERSION);
	pqsignal(2, coreIntHandler);
}