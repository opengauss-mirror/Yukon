/*
 *
 *  geomodel_util.h
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


#ifndef GM_UTIL_H
#define GM_UTIL_H

#include "postgres.h"
#include "fmgr.h"
#include <assert.h>

/***************************************** definitions ****************************************/

#define MESHTYPE 1
#define SURFACETYPE 2
#define ANYTYPE 3

#define GMNUMTYPES 1

#define YK_TRUE 1
#define YK_FALSE 0
#define YK_UNKNOWN 2
#define Yk_FAILURE 0
#define YK_SUCCESS 1

/******************************************* macros ******************************************/

/****************************************** functions ****************************************/

int geomodel_type_from_string(const char *str, uint8_t *type);
const char *geomodel_type_name(uint8_t type);
text *cstring2text(const char *cstring);
void Hex2Char(unsigned char src, unsigned char *dst);
unsigned char Char2Hex(const unsigned char *c);

#endif
