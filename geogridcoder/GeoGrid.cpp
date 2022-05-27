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
#include "geosot.h"
#include "GSGUtil.h"

PG_MODULE_MAGIC;

using namespace Yk;

PG_FUNCTION_INFO_V1(UgComputerGeoHash);
PG_FUNCTION_INFO_V1(UgBBoxGeoHash);
PG_FUNCTION_INFO_V1(UgGetFilter1);

PG_FUNCTION_INFO_V1(gsg_geosotgrid);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_z);
PG_FUNCTION_INFO_V1(gsg_as_altitude);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_from_text);
PG_FUNCTION_INFO_V1(gsg_geosotgrid3d_from_text);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_as_text);
PG_FUNCTION_INFO_V1(gsg_geosotgrid_z_as_text);
PG_FUNCTION_INFO_V1(gsg_geom_from_geosotgrid);
PG_FUNCTION_INFO_V1(gsg_geom_from_geosotgrid_array);
PG_FUNCTION_INFO_V1(gsg_has_z);
PG_FUNCTION_INFO_V1(gsg_get_level);
PG_FUNCTION_INFO_V1(gsg_degenerate);
PG_FUNCTION_INFO_V1(gsg_degenerate_array);

extern "C" Datum UgComputerGeoHash(PG_FUNCTION_ARGS);
extern "C" Datum UgBBoxGeoHash(PG_FUNCTION_ARGS);
extern "C" Datum UgGetFilter1(PG_FUNCTION_ARGS);

extern "C" Datum gsg_geosotgrid(PG_FUNCTION_ARGS);
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
extern "C" Datum gsg_degenerate(PG_FUNCTION_ARGS);
extern "C" Datum gsg_degenerate_array(PG_FUNCTION_ARGS);

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

/*----------------------------------------geomgrid function----------------------------------------*/
Datum gsg_geosotgrid(PG_FUNCTION_ARGS)
{
    int32_t data_size;
	int32_t z_num;
	uint32_t grid_num;
	size_t grid2d_num;
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

    int has_z = gserialized_has_z(gser);
    if (0 == has_z)
    {
        // 将 lwgeom 对象转化为 wkb 对象
        wkb = lwgeom_to_wkb_varlena(geom, variant);
		wkb_len = LWSIZE_GET(wkb->size) - LWVARHDRSZ;
         //栅格化
        GridRasterize((uint8_t *)wkb->data, wkb_len, nullptr, level, type, grid2d_array);
		delete wkb;
		wkb = nullptr;

        grid2d_num = grid2d_array.size();
		grid_num = grid2d_num;
		result_array_data = (Datum *)palloc(grid_num * sizeof(Datum));
		data_size = 16;
		for (size_t i = 0; i < grid2d_num; i++)
		{
			uint64_t grid_code = GetCode(grid2d_array.at(i).x, grid2d_array.at(i).y, level);
			char *val = (char *)palloc(data_size);
			memset(val, 0, data_size);
			val[0] = data_size * 4; // 1 hex = 4 bit
			val[4] = has_z;
			val[6] = level;
			*reinterpret_cast<uint64_t *>(val + 8) = grid_code;

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
		GridRasterize((uint8_t *)wkb->data, wkb_len, nullptr, level, type, grid2d_array);
		delete wkb;
		wkb = nullptr;

		int z_begin = AltitudeToInt(z_min, level);
		int z_end 	= AltitudeToInt(z_max, level);
		z_num 		= z_end - z_begin + 1;
		grid2d_num 	= grid2d_array.size();
		grid_num 	= z_num * grid2d_num;

		result_array_data = (Datum *)palloc(grid_num * sizeof(Datum));

		bitset<96> grid_code;
		uint32_t k = 0;
		data_size = 20;
		for (; z_num > 0; z_num--)
		{
			for (GridPos pos : grid2d_array)
			{
				grid_code = GetCode(pos.x, pos.y, z_begin, level);
				char *val = (char *)palloc(data_size);
				memset(val, 0, data_size);
				val[0] = data_size * 4;
				val[4] = has_z;
				val[6] = level;
				string str_data = grid_code.to_string();
				string a = string(str_data.begin(), str_data.begin() + 32);
				string b = string(str_data.begin() + 32, str_data.begin() + 64);
				string c = string(str_data.begin() + 64, str_data.end());

				int64_t la = stoll(a, nullptr, 2);
				int64_t lb = stoll(b, nullptr, 2);
				int64_t lc = stoll(c, nullptr, 2);

				*(uint32_t *)(val + 8) = lc;
				*(uint32_t *)(val + 12) = lb;
				*(uint32_t *)(val + 16) = la;

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
	double altitude = 0;
	IntToAltitude(code, level, altitude);
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

	int data_size = 16;
	char *buf_data = (char *)palloc(data_size);
	memset(buf_data, 0, data_size);
	buf_data[0] = data_size * 4;
	buf_data[4] = 0;
	buf_data[6] = level;
	*reinterpret_cast<uint64_t *>(buf_data + 8) = code;

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

	int data_size = 20;
	char *buf_data = (char *)palloc(data_size);
	memset(buf_data, 0, data_size);
	buf_data[0] = data_size * 4;
	buf_data[4] = 1;
	buf_data[6] = level;
	string str_grid3d = code_3d.to_string();
	string a = string(str_grid3d.begin(), str_grid3d.begin() + 32);
	string b = string(str_grid3d.begin() + 32, str_grid3d.begin() + 64);
	string c = string(str_grid3d.begin() + 64, str_grid3d.end());

	int64_t la = stoll(a, nullptr, 2);
	int64_t lb = stoll(b, nullptr, 2);
	int64_t lc = stoll(c, nullptr, 2);

	*(uint32_t *)(buf_data + 8) = lc;
	*(uint32_t *)(buf_data + 12) = lb;
	*(uint32_t *)(buf_data + 16) = la;

	PG_RETURN_POINTER(buf_data);
}

Datum gsg_geosotgrid_as_text(PG_FUNCTION_ARGS)
{
	text *output;
	size_t len;

	varlena *buf = PG_GETARG_VARLENA_P(0);
	int16_t has_z = *reinterpret_cast<int16_t *>(buf->vl_dat);
	int16_t level = *reinterpret_cast<int *>(buf->vl_dat + 2);
	if (0 == has_z)
	{
		uint64_t grid_code = *reinterpret_cast<uint64_t *>(buf->vl_dat + 4);
		string str_grid = ToString(grid_code, level);
		const char *grid = str_grid.c_str();
		len = strlen(grid);
		output = (text *)palloc(len + 4);
		SET_VARSIZE(output, len + 4);
		memcpy(VARDATA(output), grid, len);
	}
	else
	{
		char *data = buf->vl_dat + 4;
		bitset<96> a, b, c, grid_code;

		a = *reinterpret_cast<uint32_t *>(data + 8);
		b = *reinterpret_cast<uint32_t *>(data + 4);
		c = *reinterpret_cast<uint32_t *>(data);
		grid_code = (a << 64) | (b << 32) | c;

		uint32_t x = MergeByBitset(grid_code);
		uint32_t y = MergeByBitset(grid_code >> 1);
		uint64_t xy = MagicBits(x, y);
		string str_grid_2d = ToString(xy, level);
		const char *grid_2d = str_grid_2d.c_str();

		uint32_t z = MergeByBitset(grid_code >> 2);
		string str_grid_z = "";
		while (z)
		{
			if (z % 2 == 0)
				str_grid_z = '0' + str_grid_z;
			else
				str_grid_z = '1' + str_grid_z;
			z /= 2;
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

Datum gsg_geosotgrid_z_as_text(PG_FUNCTION_ARGS)
{
	uint32_t code = PG_GETARG_INT32(0);
	string str_grid = "";
	while (code)
	{
		if (code % 2 == 0)
			str_grid = '0' + str_grid;
		else
			str_grid = '1' + str_grid;
		code /= 2;
	}
	const char *grid = str_grid.c_str();
	text *output;
	size_t len = strlen(grid);
	output = (text *)palloc(len + 4);
	SET_VARSIZE(output, len + 4);
	memcpy(VARDATA(output), grid, len);
	PG_RETURN_TEXT_P(output);
}

Datum gsg_geom_from_geosotgrid(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	uint16_t has_z = *reinterpret_cast<uint16_t *>(buf->vl_dat);
	uint16_t level = *reinterpret_cast<uint16_t *>(buf->vl_dat + 2);
	char *data = buf->vl_dat + 4;

	GSERIALIZED *result;
	// 分离出x、y的编码
	uint x, y, z;
	double min_lng, min_lat, min_ele;
	double max_lng, max_lat, max_ele;
	// 获取当前等级下的像素大小
	double pixel_size = 0;

	if (level <= 9)
		pixel_size = pow(2, 9 - level);
	else if (10 <= level && level <= 15)
		pixel_size = pow(2, 15 - level) / 60.0;
	else if (16 <= level && level <= 32)
		pixel_size = pow(2, 21 - level) / 3600.0;

	if (0 == has_z)
	{
		uint64_t grid_code = *reinterpret_cast<uint64_t *>(data);
		UnMagicBits(grid_code, x, y);
		Code2Dec(x, min_lng);
		Code2Dec(y, min_lat);
		// 以像素大小为边，返回矩形地理范围
		POINT4D *bottom_left = (POINT4D *)palloc(sizeof(POINT4D));
		POINT4D *top_left = (POINT4D *)palloc(sizeof(POINT4D));
		POINT4D *top_right = (POINT4D *)palloc(sizeof(POINT4D));
		POINT4D *bottom_right = (POINT4D *)palloc(sizeof(POINT4D));

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

			if (lng_l < lng_r)
				max_lng = int32_t(max_lng);

			if (lat_d < lat_t)
				max_lat = int32_t(max_lat);
		}
		// 此处为秒级网格不规则划分，放大60倍，则可模拟上述效果
		else if (16 <= level && level <= 18)
		{
			int32_t lng_l = int32_t(min_lng * 60) / 1;
			int32_t lng_r = int32_t(max_lng * 60) / 1;
			int32_t lat_d = int32_t(min_lat * 60) / 1;
			int32_t lat_t = int32_t(max_lat * 60) / 1;

			if (lng_l < lng_r)
				max_lng = int32_t(max_lng * 60) / 60.0;

			if (lat_d < lat_t)
				max_lat = int32_t(max_lat * 60) / 60.0;
		}

		bottom_left->x = min_lng;
		bottom_left->y = min_lat;

		top_left->x = min_lng;
		top_left->y = max_lat;

		top_right->x = max_lng;
		top_right->y = max_lat;

		bottom_right->x = max_lng;
		bottom_right->y = min_lat;

		POINTARRAY *point_array;
		point_array = ptarray_construct_empty(0, 0, 5);

		ptarray_append_point(point_array, bottom_left, LW_FALSE);
		ptarray_append_point(point_array, top_left, LW_FALSE);
		ptarray_append_point(point_array, top_right, LW_FALSE);
		ptarray_append_point(point_array, bottom_right, LW_FALSE);
		ptarray_append_point(point_array, bottom_left, LW_FALSE);

		LWPOLY *poly = lwpoly_construct(4490, nullptr, 1, &point_array);
		result = geometry_serialize(lwpoly_as_lwgeom(poly));
		gserialized_set_srid(result, 4490);
		// lwpoly_free(poly);
	}
	else
	{
		bitset<96> a, b, c, grid_code;

		a = *reinterpret_cast<uint32_t *>(data + 8);
		b = *reinterpret_cast<uint32_t *>(data + 4);
		c = *reinterpret_cast<uint32_t *>(data);
		grid_code = (a << 64) | (b << 32) | c;
		UnMagicBitset(grid_code, x, y, z);
		Code2Dec(x, min_lng);
		Code2Dec(y, min_lat);
		IntToAltitude(z, level, min_ele);
		IntToAltitude(z + 1, level, max_ele);
		max_lng = min_lng + pixel_size;
		max_lat = min_lat + pixel_size;
		if (10 <= level && level <= 12)
		{
			int32_t lng_l = int32_t(min_lng) / 1;
			int32_t lng_r = int32_t(max_lng) / 1;
			int32_t lat_d = int32_t(min_lat) / 1;
			int32_t lat_t = int32_t(max_lat) / 1;

			if (lng_l < lng_r)
				max_lng = int32_t(max_lng);

			if (lat_d < lat_t)
				max_lat = int32_t(max_lat);
		}
		else if (16 <= level && level <= 18)
		{
			int32_t lng_l = int32_t(min_lng * 60) / 1;
			int32_t lng_r = int32_t(max_lng * 60) / 1;
			int32_t lat_d = int32_t(min_lat * 60) / 1;
			int32_t lat_t = int32_t(max_lat * 60) / 1;

			if (lng_l < lng_r)
				max_lng = int32_t(max_lng * 60) / 60.0;

			if (lat_d < lat_t)
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

		uint16_t level = *reinterpret_cast<uint16_t *>(DatumGetPointer(value) + 6);
		//获取当前等级下的像素大小
		double pixel_size = 0;

		if (level <= 9)
			pixel_size = pow(2, 9 - level);
		else if (10 <= level && level <= 15)
			pixel_size = pow(2, 15 - level) / 60.0;
		else if (16 <= level && level <= 32)
			pixel_size = pow(2, 21 - level) / 3600.0;

		uint x, y, z;
		double min_lng, min_lat, min_ele;
		double max_lng, max_lat, max_ele;
		uint16_t has_z = *reinterpret_cast<uint16_t *>(DatumGetPointer(value) + 4);
		if (0 == has_z)
		{
			uint64_t grid_code = *reinterpret_cast<uint64_t *>(DatumGetPointer(value) + 8);
			UnMagicBits(grid_code, x, y);
			//将x、y的编码转为十进制坐标
			Code2Dec(x, min_lng);
			Code2Dec(y, min_lat);

			//以像素大小为边，返回矩形地理范围
			POINT4D *bottom_left = (POINT4D *)palloc(sizeof(POINT4D));
			POINT4D *top_left = (POINT4D *)palloc(sizeof(POINT4D));
			POINT4D *top_right = (POINT4D *)palloc(sizeof(POINT4D));
			POINT4D *bottom_right = (POINT4D *)palloc(sizeof(POINT4D));

			max_lng = min_lng + pixel_size;
			max_lat = min_lat + pixel_size;

			if (10 <= level && level <= 12)
			{
				int32_t lng_l = int32_t(min_lng) / 1;
				int32_t lng_r = int32_t(max_lng) / 1;
				int32_t lat_d = int32_t(min_lat) / 1;
				int32_t lat_t = int32_t(max_lat) / 1;

				if (lng_l < lng_r)
					max_lng = int32_t(max_lng);

				if (lat_d < lat_t)
					max_lat = int32_t(max_lat);
			}
			else if (16 <= level && level <= 18)
			{
				int32_t lng_l = int32_t(min_lng * 60) / 1;
				int32_t lng_r = int32_t(max_lng * 60) / 1;
				int32_t lat_d = int32_t(min_lat * 60) / 1;
				int32_t lat_t = int32_t(max_lat * 60) / 1;

				if (lng_l < lng_r)
					max_lng = int32_t(max_lng * 60) / 60.0;

				if (lat_d < lat_t)
					max_lat = int32_t(max_lat * 60) / 60.0;
			}

			bottom_left->x = min_lng;
			bottom_left->y = min_lat;

			top_left->x = min_lng;
			top_left->y = max_lat;

			top_right->x = max_lng;
			top_right->y = max_lat;

			bottom_right->x = max_lng;
			bottom_right->y = min_lat;

			POINTARRAY *point_array;
			point_array = ptarray_construct_empty(0, 0, 5);

			ptarray_append_point(point_array, bottom_left, LW_FALSE);
			ptarray_append_point(point_array, top_left, LW_FALSE);
			ptarray_append_point(point_array, top_right, LW_FALSE);
			ptarray_append_point(point_array, bottom_right, LW_FALSE);
			ptarray_append_point(point_array, bottom_left, LW_FALSE);

			LWPOLY *poly = lwpoly_construct(4490, nullptr, 1, &point_array);
			GSERIALIZED *gser;
			gser = geometry_serialize(lwpoly_as_lwgeom(poly));
			gserialized_set_srid(gser, 4490);
			result_array_data[i++] = (Datum)gser;
		}
		else
		{
			bitset<96> a, b, c, grid_code;
			a = *reinterpret_cast<uint32_t *>(DatumGetPointer(value) + 16);
			b = *reinterpret_cast<uint32_t *>(DatumGetPointer(value) + 12);
			c = *reinterpret_cast<uint32_t *>(DatumGetPointer(value) + 8);
			grid_code = (a << 64) | (b << 32) | c;
			UnMagicBitset(grid_code, x, y, z);
			Code2Dec(x, min_lng);
			Code2Dec(y, min_lat);
			IntToAltitude(z, level, min_ele);
			IntToAltitude(z + 1, level, max_ele);
			max_lng = min_lng + pixel_size;
			max_lat = min_lat + pixel_size;
			if (10 <= level && level <= 12)
			{
				int32_t lng_l = int32_t(min_lng) / 1;
				int32_t lng_r = int32_t(max_lng) / 1;
				int32_t lat_d = int32_t(min_lat) / 1;
				int32_t lat_t = int32_t(max_lat) / 1;

				if (lng_l < lng_r)
					max_lng = int32_t(max_lng);

				if (lat_d < lat_t)
					max_lat = int32_t(max_lat);
			}
			else if (16 <= level && level <= 18)
			{
				int32_t lng_l = int32_t(min_lng * 60) / 1;
				int32_t lng_r = int32_t(max_lng * 60) / 1;
				int32_t lat_d = int32_t(min_lat * 60) / 1;
				int32_t lat_t = int32_t(max_lat * 60) / 1;

				if (lng_l < lng_r)
					max_lng = int32_t(max_lng * 60) / 60.0;

				if (lat_d < lat_t)
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
	}
	array_free_iterator(iterator);

	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
	Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geosotgrid"), ObjectIdGetDatum(namespaceId));

	get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
	ArrayType *result = construct_array(result_array_data, nelems, typ_ID, elmlen, elmbyval, elmalign);
	PG_RETURN_POINTER(result);
}

Datum gsg_has_z(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	uint16_t has_z = *reinterpret_cast<uint16_t *>(buf->vl_dat);
	PG_RETURN_BOOL(has_z == 1);
}

Datum gsg_get_level(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	uint16_t level = *reinterpret_cast<uint16_t *>(buf->vl_dat + 2);
	PG_RETURN_INT16(level);
}

Datum gsg_degenerate(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	int level = PG_GETARG_INT32(1);
	uint16_t flag = *reinterpret_cast<uint16_t *>(buf->vl_dat);
	uint16_t grid_level = *reinterpret_cast<uint16_t *>(buf->vl_dat + 2);

	if (level > grid_level)
		lwpgerror("can't convert to higher precision");
	else if (level == grid_level)
		PG_RETURN_POINTER(buf);

	char *data = buf->vl_dat;
	int data_size = flag == 0 ? 16 : 20;
	char *buf_data = (char *)palloc(data_size);
	memset(buf_data, 0, data_size);
	buf_data[0] = data_size * 4;
	buf_data[4] = flag;
	buf_data[6] = level;
	if (!flag)
	{
		uint64_t code = *reinterpret_cast<uint64_t *>(data + 4);
		code = code & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		*reinterpret_cast<uint64_t *>(buf_data + 8) = code;
	}
	else
	{
		bitset<96> a, b, c, grid_code;
		a = *reinterpret_cast<uint32_t *>(data + 12);
		b = *reinterpret_cast<uint32_t *>(data + 8);
		c = *reinterpret_cast<uint32_t *>(data + 4);
		grid_code = (a << 64) | (b << 32) | c;

		uint x, y, z;
		UnMagicBitset(grid_code, x, y, z);
		x = x >> (32 - level) << (32 - level);
		y = y >> (32 - level) << (32 - level);
		double height;
		IntToAltitude(z, grid_level, height);
		z = AltitudeToInt(height, level);
		string str_data = MagicBitset(x, y, z).to_string();
		string str_a = string(str_data.begin(), str_data.begin() + 32);
		string str_b = string(str_data.begin() + 32, str_data.begin() + 64);
		string str_c = string(str_data.begin() + 64, str_data.end());

		int64_t la = stoll(str_a, nullptr, 2);
		int64_t lb = stoll(str_b, nullptr, 2);
		int64_t lc = stoll(str_c, nullptr, 2);

		*(uint32_t *)(buf_data + 8) = lc;
		*(uint32_t *)(buf_data + 12) = lb;
		*(uint32_t *)(buf_data + 16) = la;
	}

	PG_RETURN_POINTER(buf_data);
}

Datum gsg_degenerate_array(PG_FUNCTION_ARGS)
{
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
	int nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));

	if (nelems == 0)
		PG_RETURN_NULL();

	char *array_data = ARR_DATA_PTR(array);
	Datum *result_array_data = (Datum *)palloc(nelems * sizeof(Datum));
	GEOSOTGRID *grid = (GEOSOTGRID *)array_data;
	for (int i = 0; i < nelems; i++)
	{
		uint16_t flag = grid[i].flag;
		uint16_t level = grid[i].level;
		uint64_t code = grid[i].data;
		code = code & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		int data_size = flag == 0 ? 16 : 20;
		char *buf_data = (char *)palloc(data_size);
		memset(buf_data, 0, data_size);
		buf_data[0] = data_size * 4;
		buf_data[4] = 0;
		buf_data[6] = level;
		*reinterpret_cast<uint64_t *>(buf_data + 8) = code;

		result_array_data[i] = Datum(buf_data);
	}
	int16 elmlen;
	bool elmbyval;
	char elmalign;

	Oid namespaceId = GetSysCacheOid1(NAMESPACENAME, CStringGetDatum("public"));
	Oid typ_ID = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum("geosotgrid"), ObjectIdGetDatum(namespaceId));
	get_typlenbyvalalign(typ_ID, &elmlen, &elmbyval, &elmalign);
	ArrayType *result = construct_array(result_array_data, nelems, typ_ID, elmlen, elmbyval, elmalign);

	PG_RETURN_POINTER(result);
}