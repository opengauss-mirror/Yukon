/*
 *
 * geomgrid_ops.cpp
 *
 * Copyright (C) 2021-2024 SuperMap Software Co., Ltd.
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
#include "access/gist.h"
#include "GSGUtil.h"
//#include "../include/extension_dependency.h"
#include <algorithm>
#include <string.h>
#include "geosot.h"
#include "utils/array.h"
#include "lib/stringinfo.h"
#include "utils/lsyscache.h"
#include "../libpgcommon/lwgeom_pg.h"

#define GIN_SEARCH_MODE_DEFAULT 0
#define GIN_SEARCH_MODE_INCLUDE_EMPTY 1
#define GIN_SEARCH_MODE_ALL 2
#define GIN_SEARCH_MODE_EVERYTHING 3 /* for internal use only */

struct geosotgrid
{
	int size;
	int level;
	ulong data;
};

#define RTEqualStrategyNumber			18	/* for = */
#define RTNotEqualStrategyNumber		19	/* for != */
#define RTSpanOverlapStrategyNumber		30  /* for @@ */
#define ARR_NDIM(a) ((a)->ndim)
#define ARR_DIMS(a) ((int *)(((char *)(a)) + sizeof(ArrayType)))
#define ARR_HASNULL(a) ((a)->dataoffset != 0)
#define TYPEALIGN(ALIGNVAL, LEN) (((uintptr_t)(LEN) + ((ALIGNVAL)-1)) & ~((uintptr_t)((ALIGNVAL)-1)))
#define MAXALIGN(LEN) TYPEALIGN(MAXIMUM_ALIGNOF, (LEN))
#define ARR_OVERHEAD_NONULLS(ndims) MAXALIGN(sizeof(ArrayType) + 2 * sizeof(int) * (ndims))
#define ARR_DATA_OFFSET(a) (ARR_HASNULL(a) ? (a)->dataoffset : ARR_OVERHEAD_NONULLS(ARR_NDIM(a)))
#define ARR_DATA_PTR(a) (((char *)(a)) + ARR_DATA_OFFSET(a))
#define ARRNELEMS(x) ArrayGetNItems(ARR_NDIM(x), ARR_DIMS(x))

#define CHECKARRVALID(x)                                                                                 \
  do                                                                                                     \
  {                                                                                                      \
    if (ARR_HASNULL(x) && array_contains_nulls(x))                                                       \
      ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("array must not contain nulls"))); \
  } while (0)

PG_FUNCTION_INFO_V1(geosotgrid_in);
PG_FUNCTION_INFO_V1(geosotgrid_out);
PG_FUNCTION_INFO_V1(geosotgrid_recv);
PG_FUNCTION_INFO_V1(geosotgrid_send);

PG_FUNCTION_INFO_V1(grid_lt);
PG_FUNCTION_INFO_V1(grid_le);
PG_FUNCTION_INFO_V1(grid_eq);
PG_FUNCTION_INFO_V1(grid_gt);
PG_FUNCTION_INFO_V1(grid_ge);
PG_FUNCTION_INFO_V1(grid_cmp);

PG_FUNCTION_INFO_V1(gridarray_cmp);
PG_FUNCTION_INFO_V1(gridarray_overlap);
PG_FUNCTION_INFO_V1(gridarray_spanoverlap);
PG_FUNCTION_INFO_V1(gridarray_contains);
PG_FUNCTION_INFO_V1(gridarray_contained);
PG_FUNCTION_INFO_V1(gridarray_extractvalue);
PG_FUNCTION_INFO_V1(gridarray_extractquery);
PG_FUNCTION_INFO_V1(gridarray_consistent);
PG_FUNCTION_INFO_V1(gridarray_comparepartial);

extern "C" Datum geosotgrid_in(PG_FUNCTION_ARGS);
extern "C" Datum geosotgrid_out(PG_FUNCTION_ARGS);
extern "C" Datum geosotgrid_recv(PG_FUNCTION_ARGS);
extern "C" Datum geosotgrid_send(PG_FUNCTION_ARGS);

extern "C" Datum grid_lt(PG_FUNCTION_ARGS);
extern "C" Datum grid_le(PG_FUNCTION_ARGS);
extern "C" Datum grid_eq(PG_FUNCTION_ARGS);
extern "C" Datum grid_gt(PG_FUNCTION_ARGS);
extern "C" Datum grid_ge(PG_FUNCTION_ARGS);
extern "C" Datum grid_cmp(PG_FUNCTION_ARGS);

extern "C" Datum gridarray_cmp(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_overlap(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_spanoverlap(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_contains(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_contained(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_extractvalue(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_extractquery(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_consistent(PG_FUNCTION_ARGS);
extern "C" Datum gridarray_comparepartial(PG_FUNCTION_ARGS);

Datum geosotgrid_in(PG_FUNCTION_ARGS)
{
	char *input = PG_GETARG_CSTRING(0);
	int len = strlen(input);
	if (len != 24 && len != 32)
		lwpgerror("invalid geosotgrid");

	char *data = (char *)palloc(len / 2 + 4);
	memset(data, 0, len / 2 + 4);
	for (int i = 0, j = (len / 2 - 1); i < len; i += 2, j--)
	{
		data[j + 4] = Char2Hex((uint8_t *)(input + i));
	}
	SET_VARSIZE(data, len / 2 + 4);
	PG_RETURN_POINTER(data);
}

Datum geosotgrid_out(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_GETARG_VARLENA_P(0);
	// 这里的数据没有包含 size 字段，只有 data
	char *buf_data = VARDATA(buf);
	uint16_t flag = POINTERGETUINT16(buf_data);
	int data_size = 0 == flag ? 24 : 32;
	uint8_t dst[2] = {0};
	// 申请18字节，存放字符 18位16进制数
	char *result = (char *)palloc(data_size + 1);
	memset(result, 0, data_size + 1);
	// 依次将字符转换成字符串
	for (int i = (data_size / 2 - 1), j = 0; i >= 0; i--, j++)
	{
		Hex2Char((uint8_t)buf_data[i], dst);
		result[j * 2 + 1] = dst[0];
		result[j * 2] = dst[1];
	}
	PG_RETURN_CSTRING(result);
}

Datum geosotgrid_recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
	char *data = (char *)palloc(buf->len + 4);
	memcpy(data + 4, buf->data, buf->len);
	SET_VARSIZE(data, buf->len + 4);
	buf->cursor = buf->len;
	PG_RETURN_POINTER(data);
}

Datum geosotgrid_send(PG_FUNCTION_ARGS)
{
	varlena *buf = PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	// 这里的 buf_size 是包含 size 字段的
	uint32_t buf_size = VARSIZE(buf);
	// 这里的数据没有包含 size 字段，只有 srid flags data（里边包含 bbox 数据） 数据
	char *buf_data = VARDATA(buf);
	// 这里分配大小时要包含 size 的大小，4个字节
	char *result = (char *)palloc(buf_size);
	memcpy(result + 4, buf_data, buf_size - 4);
	// 设置大小时，要包含 size 的大小
	SET_VARSIZE(result, buf_size + 4);
	PG_RETURN_POINTER(result);
}

Datum grid_lt(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	bool ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		ret = (grid_l->data < grid_r->data);
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = (isbig < 0);
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_BOOL(ret);
}

Datum grid_le(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	bool ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		ret = (grid_l->data <= grid_r->data);
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = (isbig <= 0);
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_BOOL(ret);
}

Datum grid_eq(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	bool ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		ret = (grid_l->data == grid_r->data);
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = (isbig == 0);
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_BOOL(ret);
}

Datum grid_gt(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	bool ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		ret = (grid_l->data > grid_r->data);
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = (isbig > 0);
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_BOOL(ret);
}

Datum grid_ge(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	bool ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		ret = (grid_l->data >= grid_r->data);
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = (isbig >= 0);
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_BOOL(ret);
}

Datum grid_cmp(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_GETARG_VARLENA_P(0);
	varlena *buf_r = PG_GETARG_VARLENA_P(1);
	int size_l = VARSIZE(buf_l);
	int size_r = VARSIZE(buf_r);
	int ret = 0;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		uint64_t pl = grid_l->data;
		uint64_t pr = grid_r->data;
		ret = pl > pr ? 1 : -1;
		if (pl == pr)
			ret = 0;
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = isbig > 0 ? 1 : -1;
		if (0 == isbig)
			ret = 0;
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_INT32(ret);
}

bool array_grid_overlap(ArrayType *a, ArrayType *b)
{
	int na, nb;
	int i = 0;
	int j = 0;
	char *da, *db;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARR_DATA_PTR(a);
	db = ARR_DATA_PTR(b);
	uint16_t flag_l = POINTERGETUINT16(da + 4);
	uint16_t flag_r = POINTERGETUINT16(da + 4);
	
	int size = 0;
	if (flag_l == flag_r && flag_r == 0)
	{
		size = 7;
		GEOSOTGRID *grid_a = PointerGetGEOSOTGrid(da);
		GEOSOTGRID *grid_b = PointerGetGEOSOTGrid(db);
		while (i < na && j < nb)
		{
			if (grid_a[i].data < grid_b[j].data)
				i++;
			else if (grid_a[i].data == grid_b[j].data)
				return true;
			else
				j++;
		}
	}
	else
	{
		size = 11;
		GEOSOTGRID3D *grid_a = PointerGetGEOSOTGrid3D(da);
		GEOSOTGRID3D *grid_b = PointerGetGEOSOTGrid3D(db);
		while (i < na && j < nb)
		{
			if (memcmp_reverse(grid_a[i].data, grid_b[j].data, size) < 0)
				i++;
			else if (0 == memcmp_reverse(grid_a[i].data, grid_b[j].data, size))
				return true;
			else
				j++;
		}
	}

	return false;
}

bool array_grid_spanoverlap(ArrayType *a, ArrayType *b)
{
	int na, nb;
	int i = 0;
	int j = 0;
	char *da, *db;

	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARR_DATA_PTR(a);
	db = ARR_DATA_PTR(b);
	uint16_t flag_l = POINTERGETUINT16(da + 4);
	uint16_t flag_r = POINTERGETUINT16(da + 4);
	
	int size = 0;
	int level_a, level_b, level;
	if (flag_l == flag_r && flag_r == 0)
	{
		uint64_t data_a, data_b;
		GEOSOTGRID *grid_a = PointerGetGEOSOTGrid(da);
		GEOSOTGRID *grid_b = PointerGetGEOSOTGrid(db);
		while (i < na && j < nb)
		{
			level_a = grid_a[i].level;
			level_b = grid_b[j].level;
			level = level_a > level_b ? level_b : level_a;
			data_a = grid_a[i].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
			data_b = grid_b[j].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
			if (data_a < data_b)
				i++;
			else if (data_a == data_b)
				return true;
			else
				j++;
		}
	}
	else
	{
		size = 11;
		double height;
		unsigned char *data_a;
		unsigned char *data_b;
		uint x, y, z;
		bitset<96> a, b, c, grid_code;
		GEOSOTGRID3D *grid_a = PointerGetGEOSOTGrid3D(da);
		GEOSOTGRID3D *grid_b = PointerGetGEOSOTGrid3D(db);
		while (i < na && j < nb)
		{
			level_a = grid_a[i].level;
			level_b = grid_b[j].level;
			level = level_a > level_b ? level_b : level_a;
			data_a = grid_a[i].data;
			data_b = grid_b[j].data;

			a = POINTERGETUINT32(data_a + 8);
			b = POINTERGETUINT32(data_a + 4);
			c = POINTERGETUINT32(data_a);
			grid_code = (a << 64) | (b << 32) | c;
			UnMagicBitset(grid_code, x, y, z);
			x = x >> (32 - level) << (32 - level);
			y = y >> (32 - level) << (32 - level);
			height = IntToAltitude(z, level_a);
			z = AltitudeToInt(height, level);
			string str_data_a = MagicBitset(x, y, z).to_string();

			a = POINTERGETUINT32(data_b + 8);
			b = POINTERGETUINT32(data_b + 4);
			c = POINTERGETUINT32(data_b);
			grid_code = (a << 64) | (b << 32) | c;
			UnMagicBitset(grid_code, x, y, z);
			x = x >> (32 - level) << (32 - level);
			y = y >> (32 - level) << (32 - level);
			height = IntToAltitude(z, level_b);
			z = AltitudeToInt(height, level);
			string str_data_b = MagicBitset(x, y, z).to_string();

			if (memcmp_reverse((unsigned char*)(str_data_a.c_str()),(unsigned char*)str_data_b.c_str(), size) < 0)
				i++;
			else if (0 == memcmp_reverse((unsigned char*)str_data_a.c_str(), (unsigned char*)str_data_b.c_str(), size))
				return true;
			else
				j++;
		}
	}

	return false;
}

bool array_grid_contains(ArrayType *a, ArrayType *b)
{
	int na, nb;
	int i, j, n;
	char *da, *db;
	i = j = n = 0;
	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARR_DATA_PTR(a);
	db = ARR_DATA_PTR(b);
	uint16_t flag_l = POINTERGETUINT16(da + 4);
	uint16_t flag_r = POINTERGETUINT16(da + 4);
	
	int size = 0;
	uint64_t data_a, data_b;
	int level_a, level_b, level;
	if (flag_l == flag_r && flag_r == 0)
	{
		GEOSOTGRID *grid_a = PointerGetGEOSOTGrid(da);
		GEOSOTGRID *grid_b = PointerGetGEOSOTGrid(db);
		while (i < na && j < nb)
		{
			level_a = grid_a[i].level;
			level_b = grid_b[j].level;
			if (level_a != level_b)
			{
				if (level_a < level_b)
				{
					level = level_a;
					data_a = grid_a[i].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
					data_b = grid_b[j].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
					if (data_a < data_b)
						i++;
					else if (data_a == data_b)
					{
						n++;
						j++;
					}
					else
						break;
				}
				else if (level_a > level_b)
				{
					level = level_b;
					data_a = grid_a[i].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
					data_b = grid_b[j].data & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
					if (data_a > data_b)
						j++;
					else
						i++;
				}
			}
			else
			{
				data_a = grid_a[i].data;
				data_b = grid_b[j].data;
				if (data_a < data_b)
					i++;
				else if (data_a == data_b)
				{
					n++;
					i++;
					j++;
				}
				else
					break; /* db[j] is not in da */
			}
		}
	}
	else
	{
		size = 11;
		GEOSOTGRID3D *grid_a = PointerGetGEOSOTGrid3D(da);
		GEOSOTGRID3D *grid_b = PointerGetGEOSOTGrid3D(db);
		while (i < na && j < nb)
		{
			if (memcmp_reverse(grid_a[i].data, grid_b[j].data, size) < 0)
				i++;
			else if (0 == memcmp_reverse(grid_a[i].data, grid_b[j].data, size))
			{
				n++;
				i++;
				j++;
			}
			else
				break; /* db[j] is not in da */
		}
	}

	return (n == nb) ? true : false;
}

// 数组去重，必须是已经排序之后的数组
ArrayType* array_grid2d_unique(ArrayType* r)
{
	GEOSOTGRID *tmp = nullptr;
	GEOSOTGRID *data = nullptr;
	GEOSOTGRID *dr = nullptr;
	int num = ARRNELEMS(r);
	if (num < 2)
		return r;
	data = tmp = dr = (GEOSOTGRID *)ARR_DATA_PTR(r);
	while (tmp - data < num)
	{
		if (tmp->data != dr->data)
			(++dr)->data = (tmp++)->data;
		else
			tmp++;
	}

	int num_new = dr + 1 - (GEOSOTGRID *)ARR_DATA_PTR(r);
	int nbytes = ARR_DATA_OFFSET(r) + sizeof(GEOSOTGRID) * num_new;
	if (num == num_new)
		return r;

	r = (ArrayType *)repalloc(r, nbytes);
	SET_VARSIZE(r, nbytes);
	ARR_DIMS(r)[0] = num_new; // 手动修改数组大小
	return r;
}

ArrayType* array_grid3d_unique(ArrayType* r)
{
	GEOSOTGRID3D *tmp = nullptr;
	GEOSOTGRID3D *data = nullptr;
	GEOSOTGRID3D *dr = nullptr;
	int num = ARRNELEMS(r);
	if (num < 2)
		return r;
	data = tmp = dr = PointerGetGEOSOTGrid3D(ARR_DATA_PTR(r));
	while (tmp - data < num)
	{
		if (memcmp_reverse(tmp->data, dr->data, 11) != 0)
			memcpy((tmp++)->data, (++dr)->data, 12);
		else
			tmp++;
	}

	int num_new = dr + 1 - (GEOSOTGRID3D *)ARR_DATA_PTR(r);
	int nbytes = ARR_DATA_OFFSET(r) + GEOSOTGRID3DSIZE * num_new;
	if (num == num_new)
		return r;

	r = (ArrayType *)repalloc(r, nbytes);
	SET_VARSIZE(r, nbytes);
	ARR_DIMS(r)[0] = num_new;
	return r;
}

Datum gridarray_cmp(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	int size_l = VARSIZE(buf_l);
	varlena *buf_r = PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	int size_r = VARSIZE(buf_r);
	int ret = false;
	if (size_l == size_r && size_l == GEOSOTGRIDSIZE)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(buf_l);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		int level_l = grid_l->level;
		int level_r = grid_r->level;
		uint64_t pl = grid_l->data;
		uint64_t pr = grid_r->data;
		ret = pl > pr ? 1 : -1;
		if (pl == pr)
		{
			if (level_l == level_r)
			{
				ret = 0;
			}
			else
			{
				ret = level_l < level_r ? 1 : -1;
			}
		}
	}
	else if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(buf_l);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(buf_r);
		int level_l = grid_l->level;
		int level_r = grid_r->level;
		if (level_l != level_r)
		{
			lwpgerror("Grid3d at different levels cannot used indexes");
		}
		int isbig = memcmp_reverse(grid_l->data, grid_r->data, 11);
		ret = isbig > 0 ? 1 : -1;
		if (0 == isbig)
			ret = 0;
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_INT32(ret);
}

Datum gridarray_overlap(PG_FUNCTION_ARGS)
{
	ArrayType *a = PG_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType *b = PG_GETARG_ARRAYTYPE_P_COPY(1);

	if (nullptr == a || nullptr == b)
		PG_RETURN_BOOL(false);

	if (0 == ARRNELEMS(a) || 0 == ARRNELEMS(b))
		PG_RETURN_BOOL(false);

	char *pl = ARR_DATA_PTR(a);
	char *pr = ARR_DATA_PTR(b);
	uint16_t flag_l = *(reinterpret_cast<uint16_t *>(pl + 4));
	uint16_t flag_r = *(reinterpret_cast<uint16_t *>(pr + 4));
	if (flag_l == flag_r && flag_r == 0)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(pl);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
	}
	else if (flag_l != flag_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(pl);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
	}

	bool result = array_grid_overlap(a, b);
	pfree(a);
	pfree(b);

	PG_RETURN_BOOL(result);
}

Datum gridarray_spanoverlap(PG_FUNCTION_ARGS)
{
	ArrayType *a = PG_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType *b = PG_GETARG_ARRAYTYPE_P_COPY(1);

	if (nullptr == a || nullptr == b)
		PG_RETURN_BOOL(false);

	if (0 == ARRNELEMS(a) || 0 == ARRNELEMS(b))
		PG_RETURN_BOOL(false);

	char *pl = ARR_DATA_PTR(a);
	char *pr = ARR_DATA_PTR(b);
	uint16_t flag_l = *(reinterpret_cast<uint16_t *>(pl + 4));
	uint16_t flag_r = *(reinterpret_cast<uint16_t *>(pr + 4));
	if (flag_l == flag_r && flag_r == 0)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(pl);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
	}
	else if (flag_l != flag_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(pl);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
	}

	bool result = array_grid_spanoverlap(a, b);
	pfree(a);
	pfree(b);

	PG_RETURN_BOOL(result);
}

Datum gridarray_contains(PG_FUNCTION_ARGS)
{
	ArrayType *a = PG_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType *b = PG_GETARG_ARRAYTYPE_P_COPY(1);

	if (nullptr == a || nullptr == b)
		PG_RETURN_BOOL(false);

	if (0 == ARRNELEMS(a) || 0 == ARRNELEMS(b))
		PG_RETURN_BOOL(false);

	char *pl = ARR_DATA_PTR(a);
	char *pr = ARR_DATA_PTR(b);
	uint16_t flag_l = *(reinterpret_cast<uint16_t *>(pl + 4));
	uint16_t flag_r = *(reinterpret_cast<uint16_t *>(pr + 4));
	if (flag_l == flag_r && flag_r == 0)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(pl);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
		a = array_grid2d_unique(a);
		b = array_grid2d_unique(b);
	}
	else if (flag_l != flag_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(pl);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
		a = array_grid3d_unique(a);
		b = array_grid3d_unique(b);
	}

	bool res = array_grid_contains(a, b);
	pfree(a);
	pfree(b);
	PG_RETURN_BOOL(res);
}

Datum gridarray_contained(PG_FUNCTION_ARGS)
{
	ArrayType *a = PG_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType *b = PG_GETARG_ARRAYTYPE_P_COPY(1);

	if (nullptr == a || nullptr == b)
		PG_RETURN_BOOL(false);

	if (0 == ARRNELEMS(a) || 0 == ARRNELEMS(b))
		PG_RETURN_BOOL(false);

	char *pl = ARR_DATA_PTR(a);
	char *pr = ARR_DATA_PTR(b);
	uint16_t flag_l = *(reinterpret_cast<uint16_t *>(pl + 4));
	uint16_t flag_r = *(reinterpret_cast<uint16_t *>(pr + 4));
	if (flag_l == flag_r && flag_r == 0)
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(pl);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
		a = array_grid2d_unique(a);
		b = array_grid2d_unique(b);
	}
	else if (flag_l != flag_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID3D *grid_l = PointerGetGEOSOTGrid3D(pl);
		GEOSOTGRID3D *grid_r = PointerGetGEOSOTGrid3D(pr);
		std::sort(grid_l, grid_l + ARRNELEMS(a));
		std::sort(grid_r, grid_r + ARRNELEMS(b));
		a = array_grid3d_unique(a);
		b = array_grid3d_unique(b);
	}
	bool res = array_grid_contains(b, a);
	pfree(a);
	pfree(b);
	PG_RETURN_BOOL(res);
}

/****************************************GIN索引函数****************************************/

Datum gridarray_extractvalue(PG_FUNCTION_ARGS)
{
	ArrayType *array = PG_GETARG_ARRAYTYPE_P_COPY(0);
	int32 *nkeys 	 = (int32 *)PG_GETARG_POINTER(1);
	bool **nullFlags = (bool **)PG_GETARG_POINTER(2);

	if (array == NULL || nkeys == NULL || nullFlags == NULL)
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			 errmsg("Invalid arguments for function gridarray_extractvalue")));

	int16_t elmlen;
	bool elmbyval = false;
	char elmalign;
	Datum *elems = nullptr;
	bool *nulls = nullptr;
	int nelems;

	get_typlenbyvalalign(ARR_ELEMTYPE(array), &elmlen, &elmbyval, &elmalign);
	deconstruct_array(array, ARR_ELEMTYPE(array), elmlen, elmbyval, elmalign, &elems, &nulls, &nelems);

	*nkeys = nelems;
	*nullFlags = nulls;
	/* we should not free array, elems[i] points into it */
	PG_RETURN_POINTER(elems);
}

Datum gridarray_extractquery(PG_FUNCTION_ARGS)
{
	int32 *nentries = (int32 *)PG_GETARG_POINTER(1);
	StrategyNumber strategy = PG_GETARG_UINT16(2);
	bool **pmatch = (bool **)PG_GETARG_POINTER(3);
	Pointer **extra_data = (Pointer **)PG_GETARG_POINTER(4);
	int32 *searchMode = (int32 *)PG_GETARG_POINTER(6);
	Datum *res = nullptr;
	*nentries = 0;

	ArrayType *query = PG_GETARG_ARRAYTYPE_P(0);

	CHECKARRVALID(query);
	*nentries = ARRNELEMS(query);
	if (*nentries > 0)
	{
		char *data = ARR_DATA_PTR(query);
		uint16_t flag = *(uint16_t *)(data + 4);
		if (0 == flag)
		{
			res = (Datum *)palloc(sizeof(Datum) * (*nentries));
			GEOSOTGRID *arr = PointerGetGEOSOTGrid(ARR_DATA_PTR(query));
			GEOSOTGRID *grid_arr = (GEOSOTGRID *)palloc0(sizeof(GEOSOTGRID) * (*nentries));
			// 变长类型按引用传递，此处传地址，res[i]存每个geosotgrid对象地址
			for (int i = 0; i < *nentries; i++)
			{
				grid_arr[i].data = arr[i].data & (0XFFFFFFFFFFFFFFFF << (64 - arr[i].level_min * 2));;
				grid_arr[i].flag =  arr[i].flag;
				grid_arr[i].level = arr[i].level;
				grid_arr[i].level_min = arr[i].level_min;
				grid_arr[i].size = arr[i].size;
				res[i] = PointerGetDatum(&grid_arr[i]);
			}
		}
		else
		{
			GEOSOTGRID3D *arr = nullptr;
			res = (Datum *)palloc(sizeof(Datum) * (*nentries));
			arr = PointerGetGEOSOTGrid3D(ARR_DATA_PTR(query));
			for (int i = 0; i < *nentries; i++)
				res[i] = PointerGetDatum(&arr[i]);
		}
	}

	switch (strategy)
	{
	// case RTOverlapStrategyNumber:
	// 	*searchMode = GIN_SEARCH_MODE_DEFAULT;
	// 	break;
	case RTContainedByStrategyNumber:
		/* empty set is contained in everything */
		*searchMode = GIN_SEARCH_MODE_INCLUDE_EMPTY;
		break;
	case RTContainsStrategyNumber:
		if (*nentries > 0)
			*searchMode = GIN_SEARCH_MODE_DEFAULT;
		else /* everything contains the empty set */
			*searchMode = GIN_SEARCH_MODE_ALL;
		break;
	case RTEqualStrategyNumber:
	case RTNotEqualStrategyNumber:
		if (*nentries > 0)
			*searchMode = GIN_SEARCH_MODE_DEFAULT;
		else
			*searchMode = GIN_SEARCH_MODE_INCLUDE_EMPTY;
		break;
	case RTOverlapStrategyNumber:
		// case RTSpanOverlapStrategyNumber:
		if (*nentries > 0)
		{
			*searchMode = GIN_SEARCH_MODE_DEFAULT;
			bool *p = *pmatch = (bool *)palloc(sizeof(bool) * (*nentries));
			Pointer *extra = *extra_data = (Pointer *)palloc(sizeof(Pointer) * (*nentries));
			GEOSOTGRID *pmatch_data = (GEOSOTGRID *)palloc0(sizeof(GEOSOTGRID) * (*nentries));
			GEOSOTGRID *arr = PointerGetGEOSOTGrid(ARR_DATA_PTR(query));
			for (int i = 0; i < *nentries; i++)
			{
				pmatch_data[i].data = arr[i].data | (uint64_t(pow(2, 64 - arr[i].level * 2)) - 1);
				pmatch_data[i].flag = 0;
				pmatch_data[i].size = 64;
				pmatch_data[i].level = arr[i].level;
				p[i] = true;
				extra[i] = (Pointer)(pmatch_data + i);
			}
		}
		else /* everything contains the empty set */
			*searchMode = GIN_SEARCH_MODE_ALL;
		break;
	default:
		elog(ERROR, "gridarray_extractquery: unknown strategy number: %d", strategy);
	}

	PG_RETURN_POINTER(res);
}

Datum gridarray_consistent(PG_FUNCTION_ARGS)
{
	bool *check = (bool *)PG_GETARG_POINTER(0);
	StrategyNumber strategy = PG_GETARG_UINT16(1);
	int32 nkeys = PG_GETARG_INT32(3);
	/* Pointer	   *extra_data = (Pointer *) PG_GETARG_POINTER(4); */
	bool *recheck = (bool *)PG_GETARG_POINTER(5);
	bool res = false;
	int32 i;

	switch (strategy)
	{
	// case RTSpanOverlapStrategyNumber:
	case RTOverlapStrategyNumber:
		/* result is not lossy */
		*recheck = false;
		/* at least one element in check[] is true, so result = true */
		res = true;
		break;
	case RTContainedByStrategyNumber:
		/* we will need recheck */
		*recheck = true;
		/* at least one element in check[] is true, so result = true */
		res = true;
		break;
	case RTContainsStrategyNumber:
		/* result is not lossy */
		*recheck = false;
		/* Must have all elements in check[] true */
		res = true;
		for (i = 0; i < nkeys; i++)
		{
			if (!check[i])
			{
				res = false;
				break;
			}
		}
		break;
	case RTEqualStrategyNumber:
	case RTNotEqualStrategyNumber:
		*recheck = false;
		res = true;
		break;
	default:
		elog(ERROR, "gridarray_consistent: unknown strategy number: %d", strategy);
	}

	PG_RETURN_BOOL(res);
}

Datum gridarray_comparepartial(PG_FUNCTION_ARGS)
{
	varlena *buf_l = PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	varlena *buf_r = PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	GEOSOTGRID *query_high = (GEOSOTGRID *)PG_GETARG_POINTER(3);
	int size_l = VARSIZE(query_high);
	int size_r = VARSIZE(buf_r);

	int ret = 0;
	if (size_l != size_r)
	{
		lwpgerror("cannot compare two different types");
	}
	else
	{
		GEOSOTGRID *grid_l = PointerGetGEOSOTGrid(query_high);
		GEOSOTGRID *grid_r = PointerGetGEOSOTGrid(buf_r);
		int level_l = grid_l->level;
		int level_r = grid_r->level;
		int level = level_l > level_r ? level_r : level_l;
		uint64_t pl = grid_l->data;
		uint64_t pr = grid_r->data;
		pl = pl & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		pr = pr & (0XFFFFFFFFFFFFFFFF << (64 - level * 2));
		ret = pr < pl ? -1 : 1;
		if (pr == pl)
			ret = 0;
	}
	PG_FREE_IF_COPY(buf_l, 0);
	PG_FREE_IF_COPY(buf_r, 1);
	PG_RETURN_INT32(ret);
}