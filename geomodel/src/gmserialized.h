/*
 *
 * gmserialized.h
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
#include <stdio.h>
#include "../../liblwgeom/liblwgeom.h"
#include "Base3D/YkBoundingBox.h"
using namespace Yk;

/**
* Macros for manipulating the 'typemod' int. An int32_t used as follows:
* Plus/minus = Top bit.
* Spare bits = Next 2 bits.
* SRID = Next 21 bits.
* TYPE = Next 6 bits.
* ZM Flags = Z=1,M=0.
*/

#define TYPMOD_GET_SRID(typmod) ((((typmod)&0x0FFFFF00) - ((typmod)&0x10000000)) >> 8)
#define TYPMOD_SET_SRID(typmod, srid) ((typmod) = (((typmod)&0xE00000FF) | ((srid & 0x001FFFFF) << 8)))
#define TYPMOD_GET_TYPE(typmod) ((typmod & 0x000000FC) >> 2)
#define TYPMOD_SET_TYPE(typmod, type) ((typmod) = (typmod & 0xFFFFFF03) | ((type & 0x0000003F) << 2))
#define TYPMOD_GET_Z(typmod) (1)
#define TYPMOD_SET_Z(typmod) ((typmod) |= 1 << 1)
#define TYPMOD_GET_M(typmod) (0)
#define TYPMOD_SET_M(typmod) ((typmod) &= ~(1 << 0))
#define TYPMOD_GET_NDIMS(typmod) (2 + TYPMOD_GET_Z(typmod) + TYPMOD_GET_M(typmod))

#define SRID_MAXIMUM 999999
#define SRID_USER_MAXIMUM 998999
#define SRID_UNKNOWN 0
#define SRID_IS_UNKNOWN(x) ((int)x <= 0)
#define SRID_DEFAULT 4326

bool gmserialized_get_bbox(GSERIALIZED *gms, BOX3D *bbox);
bool gmserialized_set_bbox(GSERIALIZED *gms, YkBoundingBox &bbox);