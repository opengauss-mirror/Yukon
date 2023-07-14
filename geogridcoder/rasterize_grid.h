/*
 *
 * rasterize_grid.h
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

#ifndef RASTERIZE_GRID_H
#define RASTERIZE_GRID_H

#include <bitset>
#include <vector>
#include <cmath>
#include "../raster/rt_core/librtcore.h"
#include "../libpgcommon/lwgeom_pg.h"
#include "../liblwgeom/liblwgeom.h"

#include <vector>
#include "geosot.h"
using std::vector;

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

//单个网格坐标
struct GridPos
{
    double x;
    double y;
    int level = 0;
    bool operator <(const GridPos &pos)
    {
        return GetCode(x, y, level) < GetCode(pos.x, pos.y, pos.level);
    }

    bool operator ==(const GridPos &pos)
    {
        return (x == pos.x && y == pos.y && level == pos.level);
    }
};

void GridRasterize(const uint8_t *wkb, uint32_t wkb_len, const char *srs, 
                   uint32_t level, uint32_t geom_type,vector<GridPos> &results, GBOX *src_box = nullptr);

void RawFromGdalDataSet(GDALDatasetH ds, RawGridData &data);

void RegularRasterize(GDALDriverH _drv, OGRGeometryH src_geom, OGREnvelope &src_env, rt_envelope subextent,
                      char **options, const int level, LevelFlag level_flag, uint32_t geom_type,
                      vector<GridPos> &grid_record, vector<GridPos> &grid_back);

#endif