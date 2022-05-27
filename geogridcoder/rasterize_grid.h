/*
 *
 * rasterize_grid.h
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

#ifndef RASTERIZE_GRID_H
#define RASTERIZE_GRID_H

#include <bitset>
#include <vector>
#include <cmath>
#include "../raster/rt_core/librtcore.h"
#include "../libpgcommon/lwgeom_pg.h"
#include "../liblwgeom/liblwgeom.h"

using std::vector;

//栅格化后的网格占用信息
struct RawGridData
{
    vector<uint8_t> val;

    size_t offsetx = 0; // extent 左下角为参照点
    size_t offsety = 0;
    ssize_t width = 0;
    ssize_t height = 0;
};

//全局地图框
struct GlobalBox
{
    const double up = 256 ;
    const double down = -256 ;
    const double left = -256 ;
    const double right = 256 ;
};

//等级标志：全局、分级、秒级
enum LevelFlag
{
    UNIFIED_ = 1,
    MINUTE_ = 2,
    SECOND_ = 3,
};

//单个网格左下角坐标
struct GridPos
{
    double x;
    double y;
};

void GridRasterize(const uint8_t *wkb, uint32_t wkb_len, const char *srs, 
                   const uint32_t level, uint32_t geom_type, vector<GridPos> &results);

void RawFromGdalDataSet(GDALDatasetH ds, RawGridData &data);

void RegularRasterize(GDALDriverH _drv, OGRGeometryH src_geom, OGREnvelope &src_env,
                      char **options, const int level, LevelFlag level_flag, uint32_t geom_type,
                      vector<GridPos> &grid_record, vector<GridPos> &grid_back);

#endif