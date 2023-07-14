/*
 *
 * GSGUtil.h
 *
 * Copyright (C) 2021-2023 SuperMap Software Co., Ltd.
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

#ifndef GSG_H_
#define GSG_H_

#include "postgres.h"
#include "fmgr.h"
#include <assert.h>

void Hex2Char(unsigned char src, unsigned char *dst);
unsigned char Char2Hex(const unsigned char *c);

#endif