/*
 *
 * gmfuncs.c
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


#include "gmserialized.h"
PG_FUNCTION_INFO_V1(boundary);
PG_FUNCTION_INFO_V1(BOX3D_combine);

extern "C" Datum boundary(PG_FUNCTION_ARGS);
extern "C" Datum BOX3D_combine(PG_FUNCTION_ARGS);

/**
 * @brief 返回 geomodel 的 boundary
 * 
 * @return Datum(BOX3D)
 */
Datum boundary(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gms = NULL;
	BOX3D *result = malloc(sizeof(BOX3D));
	if (result == NULL)
	{
		PG_RETURN_NULL();
	}
	gms = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	gmserialized_get_bbox(gms, result);
	PG_RETURN_POINTER(result);
}

/**
 * @brief ST_CombineBBox c 函数
 * 
 * @return Datum 
 */
Datum BOX3D_combine(PG_FUNCTION_ARGS)
{

	BOX3D *box = (BOX3D *)PG_GETARG_POINTER(0);
	GSERIALIZED *gms = PG_ARGISNULL(1) ? NULL : (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	BOX3D *result = NULL;

	/* Can't do anything with null inputs */
	if ((box == NULL) && gms == NULL)
	{
		PG_RETURN_NULL();
	}

	/*if gms is null,return box */
	if (gms == NULL)
	{
		result = palloc(sizeof(BOX3D));
		memcpy(result, box, sizeof(BOX3D));
		PG_RETURN_POINTER(result);
	}

	BOX3D *gbox = palloc(sizeof(BOX3D));
	gmserialized_get_bbox(gms, gbox);
	//result = bbox3f_merge(box, &geoBox);

	/*if box is null,return gbox */
	if (box == NULL)
	{
		result = palloc(sizeof(BOX3D));
		memcpy(result, gbox, sizeof(BOX3D));
		PG_RETURN_POINTER(result);
	}

	result = palloc(sizeof(BOX3D));
	result->xmax = Max(box->xmax, gbox->xmax);
	result->ymax = Max(box->ymax, gbox->ymax);
	result->zmax = Max(box->zmax, gbox->zmax);
	result->xmin = Min(box->xmin, gbox->xmin);
	result->ymin = Min(box->ymin, gbox->ymin);
	result->zmin = Min(box->zmin, gbox->zmin);
	result->srid = 0;

	PG_RETURN_POINTER(result);
}
