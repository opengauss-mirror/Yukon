/*
 *
 * util.h
 *
 * Copyright (C) 2023 SuperMap Software Co., Ltd.
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

#ifndef PYRAMID_UTIL_H__
#define PYRAMID_UTIL_H__

#define PI 3.14159265358979323846
#define MAX_LEVEL 16

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
struct BoundingBox {
	double xmin;
	double ymin;
	double xmax;
	double ymax;
};

struct Tile {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	double minx;
	double miny;
	double maxx;
	double maxy;
	unsigned int pts;
};

struct RateConfig {
	int _level;
	std::string _scale;
	double resolution;
	double distance;
};

enum class SimpleStatus : char
{
	UNKNOWN,
	EXIST,
	NOTEXIST
};

struct LevelConfig {
	// 划分等级
	int level = -1;
	// 存放与之相交对象的矢量金字塔，如果 bbox 为 null，默认是整个图层的范围
	std::string bbox = "null";
	// 瓦片划分规则，默认为 'quad' ，可选 geosot
	std::string gridrule = "quad";
	// 矢量瓦片大小，默认为 1024
	int extent = 1024;
	// 矢量瓦片缓存大小
	int buffer = 8;
	// 矢量金字塔输出坐标，默认 3857,可选 4326
	int srid = 3857;
	// 过滤条件 where 语句,默认为 null
	std::string filter = "null";
	// 添加到矢量瓦片中的其他信息
	std::vector<std::string> attribute;
	// st_simplicify 参数
	double tolerance = 0.0;
	std::string preserveCollapsed = "false";
	std::string buildrule = "simplify";

	// resolution 分辨率
	double resolution = 0.0;

	std::string to_string();
};

double tile2lon(int x, int z);
double tile2lat(int y, int z);
Tile getTileNumber(double lat, double lon, int z);
BoundingBox tile2boundingBox(int x, int y, int z);
std::string json_version();

std::vector<LevelConfig> parseConfig(const char *config, std::string &errmsg);
int parseConfig(const char *config, LevelConfig &levelconfig);
extern RateConfig rateconfigs[];
#endif