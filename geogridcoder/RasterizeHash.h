/*
 *
 * rasterize_hash.h
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

#ifndef RASTERIZE_HASH_H
#define RASTERIZE_HASH_H

#include <vector>
#include <bitset>
#include "../geomodel/libUGC/Include/Stream/YkDefines.h"
#include "librtcore.h"
#include "liblwgeom.h"
#include "geos_c.h"
#include "lwgeom_geos.h"

struct RawHashData
{
    std::vector<uint8_t> val;
    /* extent 左下角为参照点 */
    size_t offsetx = 0;
    /* extenet 左下角为参考点 */
    size_t offsety = 0;
    ssize_t width = 0;
    ssize_t height = 0;
};

void 
HashRasterize(
    const unsigned char *wkb, uint32_t wkb_len,
    const char *srs,
    const Yk::YkRect2D global_extent,
    const unsigned int level,RawHashData &data);

void RawFromGdalDataset(GDALDatasetH ds,RawHashData &data);


uint64_t HashCode(unsigned int x,unsigned int y, unsigned int level);


#endif