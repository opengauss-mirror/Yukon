/*
 *
 * util.cpp
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
#include "util.h"
#include "json-c/json.h"
#include "json-c/json_object.h"
#include "json-c/json_types.h"

static double
toDegrees(double angrad)
{
	return angrad * 180.0 / PI;
}

static double
toRadians(double angdeg)
{
	return angdeg / 180.0 * PI;
}

double
tile2lat(int y, int z)
{
	double n = PI - (2.0 * PI * y) / pow(2.0, z);
	return toDegrees(atan(sinh(n)));
}

double
tile2lon(int x, int z)
{
	return x / pow(2.0, z) * 360.0 - 180;
}

BoundingBox
tile2boundingBox(int x, int y, int z)
{
	BoundingBox box;
	box.ymin = tile2lat(y, z);
	box.ymax = tile2lat(y + 1, z);
	box.xmin = tile2lon(x, z);
	box.ymax = tile2lon(x + 1, z);
	return box;
}

Tile
getTileNumber(double lat, double lon, int z)
{
	int xtile = (int)floor((lon + 180) / 360 * (1 << z));
	int ytile = (int)floor((1 - log(tan(toRadians(lat)) + 1 / cos(toRadians(lat))) / PI) / 2 * (1 << z));
	if (xtile < 0)
		xtile = 0;
	if (xtile >= (1 << z))
		xtile = ((1 << z) - 1);
	if (ytile < 0)
		ytile = 0;
	if (ytile >= (1 << z))
		ytile = ((1 << z) - 1);
	Tile tile;
	tile.x = xtile;
	tile.y = ytile;
	tile.z = z;
	return tile;
}


/**
 * @brief 将一个 LevelConfig 对象转换为一个 json 字符串
 * 
 * @return std::string 
 */
std::string
LevelConfig::to_string()
{
#if 0
		std::ostringstream output;
		output << std::setprecision(15);
		output << "level:" + std::to_string(level) + "\n";
		output << "bbox:" + bbox + "\n";
		output << "gridrule:" + gridrule + "\n";
		output << "extent:" + std::to_string(extent) + "\n";
		output << "buffer:" + std::to_string(buffer) + "\n";
		output << "srid:" + std::to_string(srid) + "\n";
		output << "filter:" + filter + "\n";
		output << "attribute:";
		for (size_t i = 0; i < attribute.size(); i++)
		{
			output << attribute[i] + " ";
		}
		output << "\n";
		output << "buildrule:" + buildrule + "\n";
		output << "tolerance:" << tolerance << "\n";
		output << "preserveCollapsed:" + preserveCollapsed + "\n";
		output << "resolution:" << resolution << "\n";
		return output.str();
#endif
	json_object *root;
	const char *jsonString;
	std::string res;

	// Create the JSON object
	root = json_object_new_object();
	json_object_object_add(root, "level", json_object_new_int(level));
	json_object_object_add(root, "resolution", json_object_new_double(resolution));
	json_object_object_add(root, "filter", json_object_new_string(filter.c_str()));

	json_object *attriarray = json_object_new_array();
	for (auto a : attribute)
	{
		json_object_array_add(attriarray, json_object_new_string(a.c_str()));
	}
	json_object_object_add(root, "attribute", attriarray);

	// Convert the JSON object to a string
	jsonString = json_object_to_json_string(root);
	res = std::string(jsonString);
	json_object_put(root);
	return res;
}

/**
 * @brief 解析具体的配置规则
 *
 * @param config 配置规则
 * @param rules 解析后的结果
 * @param index 配置规则数组的下标
 * @param errmsg 错误信息
 * @return int
 *		   0 - 解析正常
 * 		   -1 - 解析错误
 */
static int
parseConfigDetial(json_object *config, std::vector<LevelConfig> &rules, int index, std::string &errmsg)
{
	LevelConfig &elem = rules.at(index);

	// level
	json_object *jo_level = json_object_object_get(config, "level");
	if (!jo_level || json_object_get_type(jo_level) != json_type_int)
	{
		errmsg = std::string("config must be contain level field and it's type is integer value");
		return -1;
	}

	elem.level = json_object_get_uint64(jo_level);

	// resolution

	json_object *jo_resolution = json_object_object_get(config, "resolution");

	if (jo_resolution && json_object_get_type(jo_resolution) == json_type_double)
	{
		elem.resolution = json_object_get_double(jo_resolution);
	}

	json_object *jo_bbox = json_object_object_get(config, "bbox");

	if (jo_bbox && json_object_get_type(jo_bbox) == json_type_string)
	{
		elem.bbox = std::string(json_object_get_string(jo_bbox));
	}

	json_object *jo_gridrule = json_object_object_get(config, "gridrule");

	if (jo_gridrule && json_object_get_type(jo_gridrule) == json_type_string)
	{
		elem.gridrule = std::string(json_object_get_string(jo_gridrule));
	}

	json_object *jo_extent = json_object_object_get(config, "extent");

	if (jo_extent && json_object_get_type(jo_extent) == json_type_int)
	{
		elem.extent = json_object_get_uint64(jo_extent);
	}

	json_object *jo_buffer = json_object_object_get(config, "buffer");

	if (jo_buffer && json_object_get_type(jo_buffer) == json_type_int)
	{
		elem.buffer = json_object_get_uint64(jo_buffer);
	}

	json_object *jo_srid = json_object_object_get(config, "srid");

	if (jo_srid && json_object_get_type(jo_srid) == json_type_int)
	{
		elem.srid = json_object_get_uint64(jo_srid);
	}

	json_object *jo_filter = json_object_object_get(config, "filter");

	if (jo_filter && json_object_get_type(jo_filter) == json_type_string)
	{
		elem.filter = std::string(json_object_get_string(jo_filter));
	}

	json_object *jo_attribute = json_object_object_get(config, "attribute");

	if (jo_attribute && json_object_get_type(jo_attribute) == json_type_array)
	{
		int n = json_object_array_length(jo_attribute);
		for (int i = 0; i < n; i++)
		{
			json_object *jo_str = json_object_array_get_idx(jo_attribute, i);
			if (json_object_get_type(jo_str) == json_type_string)
			{
				elem.attribute.push_back(std::string(json_object_get_string(jo_str)));
			}
		}
	}

	// json_object *jo_simplify = json_object_object_get(config, "simplify");

	// if (jo_simplify)
	// {

	json_object *jo_buildrule = json_object_object_get(config, "buildrule");
	if (jo_buildrule && json_object_get_type(jo_buildrule) == json_type_string)
	{
		elem.buildrule = json_object_get_string(jo_buildrule);
	}

	json_object *jo_tolerance = json_object_object_get(config, "tolerance");
	if (jo_tolerance && json_object_get_type(jo_tolerance) == json_type_double)
	{
		elem.tolerance = json_object_get_double(jo_tolerance);
	}

	json_object *jo_preserveCollapsed = json_object_object_get(config, "preserveCollapsed");
	if (jo_preserveCollapsed && json_object_get_type(jo_preserveCollapsed) == json_type_boolean)
	{
		elem.preserveCollapsed =
		    json_object_get_boolean(jo_preserveCollapsed) ? std::string("true") : std::string("false");
	}
	// }

	// 检查等级超过 19 级且未提供 resolution 参数则报错
	if (elem.level > MAX_LEVEL && elem.resolution == 0)
	{
		errmsg = std::string("the level has exceeded .provide the resolution parameter");
		return -1;
	}

	return 0;
}

/**
 * @brief  解析配置文件
 *
 * @param config 配置字符串
 * @return std::vector<LevelConfig>
 */
std::vector<LevelConfig>
parseConfig(const char *config, std::string &errmsg)
{
	json_object *jo_root_obj = json_tokener_parse(config);
	std::vector<LevelConfig> vres;
	if (!jo_root_obj)
	{
		return vres;
	}

	// 检查最顶层是否为 level
	// json_object *jo_level_obj = json_object_object_get(jo_root, "level");

	// if (jo_level_obj == NULL)
	// {
	// 	fprintf(stderr, "you should contain a top object named level,for example:{\"level\":[]}\n");
	// 	json_object_put(jo_root);
	// 	return vres;
	// }

	// json_type type;

	// 配置 json 是一个数组
	// type = json_object_get_type(jo_root_obj);

	// if (type != json_type_array)
	// {
	// 	fprintf(stderr, "the json format should be:{\"level\":[]}\n");
	// 	json_object_put(jo_root);
	// 	return vres;
	// }

	// int itemsize = json_object_array_length(jo_level);

	// if (itemsize == 0)
	// {
	// 	fprintf(stderr, "the array can not be empty\n");
	// 	json_object_put(jo_root);
	// 	return vres;
	// }

	// // 划分等级个数
	// json_object *jo_levels_key = json_object_array_get_idx(jo_level, 0);

	// if (!jo_levels_key)
	// {
	// 	fprintf(stderr, "format parse error\n");
	// 	json_object_put(jo_root);
	// 	return vres;
	// }

	// 获取等级数组
	// json_object *jo_levels_objs = json_object_object_get(jo_root_obj, "levels");

	// jo_root_obj 不为空，并且是一个 array
	if (json_object_get_type(jo_root_obj) != json_type_array)
	{
		fprintf(stderr, "levels parse error!\n");
		json_object_put(jo_root_obj);
		return vres;
	}
	int levels_size = json_object_array_length(jo_root_obj);
	vres.resize(levels_size);

	// // 填充结果中的等级信息
	// for (int i = 0; i < levels_size; i++)
	// {
	// 	json_object *jo_level_num = json_object_array_get_idx(jo_levels, i);
	// 	if (json_object_get_type(jo_level_num) == json_type_int)
	// 	{
	// 		vres[i].level = json_object_get_uint64(jo_level_num);
	// 	}
	// 	else
	// 	{
	// 		// 等级数组中含有非 int 型数据，返回空
	// 		fprintf(stderr, "levels should be only contains int value\n");
	// 		vres.resize(0);
	// 		json_object_put(jo_root);
	// 		return vres;
	// 	}
	// }

	for (int i = 0; i < levels_size; i++)
	{
		json_object *jo_detail = json_object_array_get_idx(jo_root_obj, i);

		// 解析具体的规则
		if (parseConfigDetial(jo_detail, vres, i, errmsg) != 0)
		{
			vres.clear();
			break;
		}
	}

	json_object_put(jo_root_obj);
	return vres;
}

/**
 * @brief 这个函数将单个配置字符串解析为一个 LevelConfig 对象,
 *		  这个函数只用于更新时使用
 *
 * @param config
 * @return LevelConfig
 */
int
parseConfig(const char *config, LevelConfig &levelconfig)
{
	json_object *jo_root_obj = json_tokener_parse(config);

	if (jo_root_obj != nullptr)
	{
		// level
		json_object *jo_level = json_object_object_get(jo_root_obj, "level");
		if (!jo_level || json_object_get_type(jo_level) != json_type_int)
		{
			return -1;
		}

		levelconfig.level = json_object_get_uint64(jo_level);

		// resolution

		json_object *jo_resolution = json_object_object_get(jo_root_obj, "resolution");

		if (jo_resolution && json_object_get_type(jo_resolution) == json_type_double)
		{
			levelconfig.resolution = json_object_get_double(jo_resolution);
		}

		json_object *jo_attribute = json_object_object_get(jo_root_obj, "attribute");

		if (jo_attribute && json_object_get_type(jo_attribute) == json_type_array)
		{
			int n = json_object_array_length(jo_attribute);
			for (int i = 0; i < n; i++)
			{
				json_object *jo_str = json_object_array_get_idx(jo_attribute, i);
				if (json_object_get_type(jo_str) == json_type_string)
				{
					levelconfig.attribute.push_back(std::string(json_object_get_string(jo_str)));
				}
			}
		}
	}

	json_object_put(jo_root_obj);
	return 0;
}

RateConfig rateconfigs[] = {
    {0, "1:591658710.91", 1.406250000000, 78271.51696},  {1, "1:295829355.455", 0.703125000000, 39135.75848},
    {2, "1:147914677.727", 0.351562500000, 19567.87924}, {3, "1:73957338.864", 0.175781250000, 9783.939621},
    {4, "1:36978669.432", 0.087890625000, 4891.96981},   {5, "1:18489334.716", 0.043945312500, 2445.984905},
    {6, "1:9244667.358", 0.021972656250, 1222.992453},   {7, "1:4622333.679", 0.010986328125, 611.4962263},
    {8, "1:2311166.839", 0.005493164063, 305.7481131},   {9, "1:1155583.42", 0.002746582031, 152.8740566},
    {10, "1:577791.71", 0.001373291016, 76.43702829},    {11, "1:288895.855", 0.000686645508, 38.21851414},
    {12, "1:144447.927", 0.000343322754, 19.10925707},   {13, "1:72223.964", 0.000171661377, 9.554628536},
    {14, "1:36111.982", 0.000085830688, 4.777314268},    {15, "1:18055.991", 0.000042915344, 2.388657134},
    {16, "1:9027.995", 0.000021457672, 1.194328567},     {17, "1:4513.998", 0.000010728836, 0.597164283},
    {18, "1:2256.999", 0.000005364418, 0.298582142},     {19, "1:1128.499", 0.000002682209, 0.149291071}};