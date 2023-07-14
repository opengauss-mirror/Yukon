/*
 *
 * pyramid.cpp
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
#include <string>
#include <iomanip>
#include <vector>
#include <chrono>
#include <queue>

using namespace std::chrono;

#include "postgres.h"
#include "fmgr.h"
#include "utils/palloc.h"
#include "lib/stringinfo.h"	 /* For binary input */
#include "utils/array.h"	 /* for ArrayType */
#include "utils/builtins.h"	 /* for pg_atoi */
#include "catalog/pg_type.h" /* for all oid */
#include "executor/spi.h"	 /* for spi */

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(yukon_pyramid_version);
extern "C" Datum yukon_pyramid_version(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(buildpyramid);
extern "C" Datum buildpyramid(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(buildTile);
extern "C" Datum buildTile(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(updatePyramid);
extern "C" Datum updatePyramid(PG_FUNCTION_ARGS);

#define YUKON_PYRAMID_VERSION "yukon_pyramid 1.0.0"
#define QUERYSIZE 4096

// 这里我们直接将定义包含在这里，不用包含 liblwgeom.h 头文件
typedef struct
{
	uint16_t flags;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double zmin;
	double zmax;
	double mmin;
	double mmax;
} GBOX;

static bool
isTouch(GBOX *gbox, Tile t)
{
	if ((gbox->xmax > t.minx && t.maxx > gbox->xmin) && (gbox->ymax > t.miny && t.maxy > gbox->ymin))
	{
		return true;
	}
	return false;
}

/**
 * 从 'BOX(115.375 39.416670000654165,117.49999999999999 41.08333000065866)' 字符串中找到 x,y 的最大最小值
 */
static void
getextent(const char *str, double extent[])
{
	Assert(str);
	Assert(xmin);
	Assert(ymin);
	Assert(xmax);
	Assert(ymax);

	char *temp = static_cast<char *>(palloc0(strlen(str) - 1));
	memcpy(temp, str + 4, strlen(str) - 5);
	char *pos[4] = {0};
	pos[0] = temp;
	for (int i = 0, j = 1; temp[i] != 0; i++)
	{
		if (temp[i] == ' ' || temp[i] == ',')
		{
			temp[i] = 0;
			pos[j++] = temp + i + 1;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		extent[i] = atof(pos[i]);
	}

	pfree(temp);
}

Datum yukon_pyramid_version(PG_FUNCTION_ARGS)
{
	char src[100] = {0};
	const char *ver = YUKON_PYRAMID_VERSION;
	const char *compileinfo = COMPILEINFO;
	strcat(src, ver);
	strcat(src, compileinfo);
	text *result = cstring_to_text(src);
	PG_RETURN_TEXT_P(result);
}

Datum
// ST_BuildPyramid(tablename text, columnname text, config text)
buildpyramid(PG_FUNCTION_ARGS)
{
	int ret = 0; // SPI_execute 返回的结果
	int tuplecount = 0;
	int srid = 0;
	double dbbox[4] = {0};
	std::vector<std::string> table_field;
	std::ostringstream sql_buffer;
	sql_buffer << std::setprecision(15);
	std::string geomtype;
	// 1. 参数校验

	if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2))
	{
		PG_RETURN_BOOL(false);
	}
	const char *schemaname = text_to_cstring(PG_GETARG_TEXT_P(0));
	const char *tablename = text_to_cstring(PG_GETARG_TEXT_P(1));
	const char *columname = text_to_cstring(PG_GETARG_TEXT_P(2));
	const char *pszconfig = text_to_cstring(PG_GETARG_TEXT_P(3));

	// 添加表名长度限制，如果太长，则限制生成

	if ((strlen(tablename) + strlen(columname) > 248))
	{
		elog(ERROR, "%s:%s", __FUNCTION__, "table's name or column'name is to long");
		PG_RETURN_BOOL(false);
	}

	// 解析配置
	std::string errmsg;
	std::vector<LevelConfig> configres = parseConfig(pszconfig, errmsg);

	// 如果 configres 为空，则报错
	if (configres.size() == 0)
	{
		// 如果有具体的错误信息，则显示具体的错误信息，否则提示 json 格式错误
		if (errmsg.empty())
		{
			elog(ERROR, "%s:Json format is invalid,%s", __FUNCTION__, pszconfig);
		}
		else
		{
			elog(ERROR, "%s:%s", __FUNCTION__, errmsg.c_str());
		}
	}

	if (SPI_OK_CONNECT != SPI_connect())
	{
		elog(NOTICE, "%s: could not connect to SPI manager", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 检查图层是否存在
	char query[QUERYSIZE] = {0};
	snprintf(
		query,
		QUERYSIZE,
		"select count(*) from geometry_columns where f_table_name=\'%s\' and f_geometry_column=\'%s\' and f_table_schema=\'%s\' ",
		tablename,
		columname,
		schemaname);

	ret = SPI_execute(query, true, 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		tuplecount = atoi(tupleval);
		pfree(tupleval);
		if (tuplecount == 0)
		{
			elog(ERROR, "%s:%s is not a valid geometry table.", __FUNCTION__, tablename);
		}
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 检查 geometry 类型
	bzero(query, QUERYSIZE);
	snprintf(query,
			 QUERYSIZE,
			 "SELECT GeometryType(\"%s\") FROM \"%s\".\"%s\" LIMIT 1",
			 columname,
			 schemaname,
			 tablename);

	ret = SPI_execute(query, true, 0);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		if (SPI_processed > 0)
		{
			HeapTuple tuple = tuptable->vals[0];
			char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
			geomtype = std::string(tupleval);
			pfree(tupleval);
		}
		else
		{
			elog(ERROR, "%s: can not get the geometry type", __FUNCTION__);
		}
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 获取 SRID
	snprintf(
		query,
		QUERYSIZE,
		"select srid from geometry_columns where f_table_name=\'%s\' and f_geometry_column=\'%s\' and f_table_schema=\'%s\'",
		tablename,
		columname,
		schemaname);

	ret = SPI_execute(query, true, 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		srid = atoi(tupleval);
		pfree(tupleval);
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	/**
	 * 检查划分的规则是否合法，包括划分等级，划分方法，要添加的属性字段，
	 * filter 暂时不做检查，由用户检查
	 */

	bzero(query, QUERYSIZE);
	snprintf(
		query,
		QUERYSIZE,
		"SELECT A.attname FROM	pg_class AS C,	pg_attribute AS A WHERE	C.relname = \'%s\' AND C.relnamespace = (select oid from pg_namespace where nspname = \'%s\' ) AND A.attrelid = C.oid ;",
		tablename,
		schemaname);

	ret = SPI_execute(query, true, 0);

	// 获取该表的所有字段
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;

		for (size_t i = 0; i < SPI_processed; i++)
		{
			HeapTuple tuple = tuptable->vals[i];
			char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
			table_field.push_back(std::string(tupleval));
			pfree(tupleval);
		}
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 检查配置是否正确
	for (size_t i = 0; i < configres.size(); i++)
	{
		auto e = configres.at(i);
		// gridrule check
		if (e.gridrule != "quad" && e.gridrule != "geosot")
		{
			elog(ERROR,
				 "%s: gridrule must be quad or geosot \nconfig:%s",
				 __FUNCTION__,
				 e.to_string().c_str());
			SPI_finish();
			PG_RETURN_BOOL(false);
		}

		// level check
		if (e.level < 0 || e.level > MAX_LEVEL)
		{
			elog(ERROR, "%s: level must be between 0 - 16\nconfig:%s", __FUNCTION__, e.to_string().c_str());
			SPI_finish();
			PG_RETURN_BOOL(false);
		}

		// attribute check
		for (auto s : e.attribute)
		{
			auto it = find(table_field.begin(), table_field.end(), s);

			if (it == table_field.end())
			{
				elog(ERROR,
					 "%s:%s is invalid\nconfig:%s",
					 __FUNCTION__,
					 s.c_str(),
					 e.to_string().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
	}

	// MULTIPOLYGON 或 LINESTRING 数据
	if (geomtype == "MULTIPOLYGON" || geomtype == "POLYGON" || geomtype == "LINESTRING" ||
		geomtype == "MULTILINESTRING")
	{
		/**
		 * 创建存储金字塔的表，这里将每个简化的单独形成一张表
		 */
		for (auto e : configres)
		{
			sql_buffer.str("");
			sql_buffer << "create table \"";
			sql_buffer << schemaname;
			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_" << e.level << "\""
					   << " as select ";

			// attribute
			for (auto attr : e.attribute)
			{
				sql_buffer << "\"" << attr << "\" ,";
			}

			if (e.resolution == 0.0 && srid == 4326)
			{

				e.resolution = e.tolerance = rateconfigs[e.level + 1].resolution;
			}
			else if (e.resolution == 0.0 && srid == 3857)
			{
				e.resolution = e.tolerance = rateconfigs[e.level + 1].distance;
			}
			else if (e.resolution == 0.0)
			{
				elog(ERROR, "%s:Only supports 4326 and 3857 coordinate systems.", __FUNCTION__);
			}

			// 如果 level 大于 10，则使用简化算法，否则使用 snaptogrid 算法
			if (e.level > 10)
			{
				sql_buffer << "st_simplify(\"" << columname << "\"," << e.tolerance << ","
						   << e.preserveCollapsed << ") as \"" << columname << "\"";
			}
			else
			{
				sql_buffer << "st_snaptogrid(\"" << columname << "\"," << e.tolerance << ") as \""
						   << columname << "\"";
			}

			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\"";

			// filter
			if (e.filter != "null")
			{
				sql_buffer << " where " << e.filter;
			}

			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());

			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 删除 EMPTY 数据

			sql_buffer.str("");
			sql_buffer << "DELETE FROM \"" << schemaname << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_" << e.level
					   << "\" WHERE ST_ISEMPTY(\"" << columname << "\")";
			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());

			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 存储元信息
			sql_buffer.str("");
			sql_buffer
				<< "insert into pyramid_columns (f_schema_name,  f_table_name, f_geometry_column, f_pyramid_table_name, resolution, config) values(";
			sql_buffer << "\'" << schemaname << "\'"
					   << ","
					   << "\'" << tablename << "\'"
					   << ","
					   << "\'" << columname << "\'"
					   << ","
					   << "\'"
					   << "pyd_" << tablename << "_" << columname << "_" << e.level << "\'"
					   << "," << e.tolerance << ","
					   << "\'" << e.to_string() << "\'"
					   << ")";
			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());
			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
	}
	// POINT 类型数据
	else if (geomtype == "POINT" || geomtype == "MULTIPOINT")
	{
		// 获取边界范围

		memset(query, 0, QUERYSIZE);
		snprintf(query,
				 QUERYSIZE,
				 "SELECT ST_EstimatedExtent(\'%s\',\'%s\',\'%s\')",
				 schemaname,
				 tablename,
				 columname);
		ret = SPI_exec(query, 1);

		if (ret > 0 && SPI_tuptable != NULL)
		{
			SPITupleTable *tuptable = SPI_tuptable;
			TupleDesc tupdesc = tuptable->tupdesc;
			HeapTuple tuple = tuptable->vals[0];
			char *bbox = SPI_getvalue(tuple, tupdesc, 1);
			if (bbox)
			{
				getextent(bbox, dbbox);
				pfree(bbox);
			}
			else
			{
				elog(ERROR, "%s:you need to analyze the table %s first.", __FUNCTION__, tablename);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
		else
		{
			elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
			SPI_finish();
			PG_RETURN_BOOL(false);
		}

		// 生成点抽稀表
		for (auto e : configres)
		{

			// 计算格子总数
			if (e.resolution == 0.0 && srid == 4326)
			{

				e.resolution = e.tolerance = rateconfigs[e.level + 1].resolution;
			}
			else if (e.resolution == 0.0 && srid == 3857)
			{
				e.resolution = e.tolerance = rateconfigs[e.level + 1].distance;
			}
			else if (e.resolution == 0.0)
			{
				elog(ERROR, "%s:Only supports 4326 and 3857 coordinate systems.", __FUNCTION__);
			}
			int xbuckets = (int(dbbox[2]) - int(dbbox[0])) / e.resolution;
			int ybuckets = (int(dbbox[3]) - int(dbbox[1])) / e.resolution;

			// 这里我们只生成一个临时表
			sql_buffer.str("");
			sql_buffer << "create table \"";
			sql_buffer << schemaname;
			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\""
					   << " as select  \"" << columname << "\" from (";
			sql_buffer << "SELECT WIDTH_BUCKET(st_x("
					   << "\"" << columname << "\""
					   << "), " << int(dbbox[0]) << "," << int(dbbox[2]) + 1 << "," << xbuckets
					   << ") grid_x,"
					   << "WIDTH_BUCKET(st_y("
					   << "\"" << columname << "\""
					   << ")," << int(dbbox[1]) << ", " << int(dbbox[3]) + 1 << ", " << ybuckets
					   << ")   grid_y,"
					   << "ST_GeometryN(ST_collect("
					   << "\"" << columname << "\""
					   << "),1) \"" << columname << "\"";

			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\"";

			// filter
			if (e.filter != "null")
			{
				sql_buffer << " where " << e.filter;
			}

			sql_buffer << " GROUP BY grid_x,grid_y) as temp";

			// 执行 SQL
			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());
			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
			// 这里我们将附带的信息加入到概化表中

			sql_buffer.str("");
			sql_buffer << "create table \"";
			sql_buffer << schemaname;
			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_" << e.level << "\""
					   << " as select  ";

			// attribute
			for (auto attr : e.attribute)
			{
				sql_buffer << "a.\"" << attr << "\" ,";
			}
			// geometry
			sql_buffer << "a.\"" << columname << "\"";
			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\" as a,"
					   << "\"" << schemaname;

			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\" as b";

			sql_buffer << " where a."
					   << "\"" << columname << "\""
					   << "=b."
					   << "\"" << columname << "\"";

			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());
			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 删除临时表
			sql_buffer.str("");
			sql_buffer << "DROP TABLE IF EXISTS \"" << schemaname << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\"";
			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());
			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 存储元信息
			sql_buffer.str("");
			sql_buffer
				<< "insert into pyramid_columns (f_schema_name, f_table_name, f_geometry_column, f_pyramid_table_name, resolution, config) values(";
			sql_buffer << "\'" << schemaname << "\'"
					   << ","
					   << "\'" << tablename << "\'"
					   << ","
					   << "\'" << columname << "\'"
					   << ","
					   << "\'"
					   << "pyd_" << tablename << "_" << columname << "_" << e.level << "\'"
					   << "," << e.resolution << ","
					   << "\'" << e.to_string() << "\'"
					   << ")";

			bzero(query, QUERYSIZE);
			snprintf(query, QUERYSIZE, "%s", sql_buffer.str().c_str());
			ret = SPI_exec(query, 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
	}
	else
	{
		elog(ERROR, "%s: Not Supported Geometry Type:%s", __FUNCTION__, geomtype.c_str());
	}

	SPI_finish();
	PG_RETURN_BOOL(true);
}

Datum buildTile(PG_FUNCTION_ARGS)
{
	int ret = 0;
	int srid = 0;
	char query[QUERYSIZE] = {0};
	if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2))
	{
		PG_RETURN_BOOL(false);
	}

	const char *schema = text_to_cstring(PG_GETARG_TEXT_P(0));
	const char *table = text_to_cstring(PG_GETARG_TEXT_P(1));
	const char *column = text_to_cstring(PG_GETARG_TEXT_P(2));
	unsigned int max_level = PG_GETARG_INT32(3);

	if (SPI_OK_CONNECT != SPI_connect())
	{
		elog(NOTICE, "%s: could not connect to SPI manager", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 检查是否包含 geometry 数据
	snprintf(
		query,
		QUERYSIZE,
		"select count(*) from geometry_columns where f_table_name=\'%s\' and f_geometry_column=\'%s\' and f_table_schema=\'%s\'",
		table,
		column,
		schema);

	ret = SPI_execute(query, true, 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		int tuplecount = atoi(tupleval);
		pfree(tupleval);
		if (tuplecount == 0)
		{
			elog(ERROR, "%s:%s is not a valid geometry table.", __FUNCTION__, table);
		}
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 获取 SRID
	snprintf(
		query,
		QUERYSIZE,
		"select srid from geometry_columns where f_table_name=\'%s\' and f_geometry_column=\'%s\' and f_table_schema=\'%s\'",
		table,
		column,
		schema);

	ret = SPI_execute(query, true, 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		srid = atoi(tupleval);
		pfree(tupleval);
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 创建矢量瓦片表
	snprintf(query,
			 QUERYSIZE,
			 "CREATE TABLE IF NOT EXISTS \"%s\".\"tile_%s_%s\" (id bigint,mvt bytea);",
			 schema,
			 table,
			 column);
	ret = SPI_exec(query, 1);

	if (ret != SPI_OK_UTILITY)
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 构建稀疏矩阵

	std::queue<Tile> qtiles;
	std::vector<Tile> vtiles;
	vtiles.reserve(20000);

	unsigned int point_thredhold = 10000;
	double minx, miny, maxx, maxy, centerx, centery;
	std::ostringstream sql_buffer;
	std::ostringstream _extent;
	sql_buffer << std::setprecision(15);
	_extent << std::setprecision(15);

	if (srid == 4326)
	{
		minx = -180.0000000000000000;
		miny = -270.0000000000000000;
		maxx = 180.0000000000000000;
		maxy = 90.0000000000000000;
	}
	else if (srid == 3857)
	{
		minx = -20037508.3427870012819767;
		miny = -20037508.3427809961140156;
		maxx = 20037508.3427809998393059;
		maxy = 20037508.3427870012819767;
	}
	else
	{
		elog(ERROR, "%s:Only supports 4326 and 3857 coordinate systems.", __FUNCTION__);
		PG_RETURN_BOOL(false);
	}
#ifdef DEBUG
	auto indexstart = system_clock::now();
#endif
	// 添加初始 Tile
	qtiles.push({0, 0, 0, minx, miny, maxx, maxy, INT_MAX});

	while (!qtiles.empty())
	{
		auto t = qtiles.front();

		// 这里加入等级限制
		if (t.z >= max_level)
		{
			break;
		}

		// 检查当前点数
		if (t.pts > point_thredhold)
		{
			// 如果大于当前设定的容限值，则将其四分后，加入到队列，等待后续处理
			// 1
			centerx = (t.minx + t.maxx) / 2;
			centery = (t.miny + t.maxy) / 2;
			Tile t1{t.x * 2, t.y * 2, t.z + 1, t.minx, centery, centerx, t.maxy};
			_extent.str("");
			sql_buffer.str("");
			_extent << "'BOX(" << t1.minx << " " << t1.miny << "," << t1.maxx << " " << t1.maxy
					<< ")'::BOX2D";
			sql_buffer << "with a as (select (st_dumppoints("
					   << "\"" << column
					   << "\""
						  ")).geom as geom from "
					   << "\"" << schema << "\""
					   << "."
					   << "\"" << table << "\""
					   << " where "
					   << "\"" << column << "\""
					   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
					   << _extent.str();

			ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
				unsigned int tuplecount = atoi(tupleval);
				pfree(tupleval);
				t1.pts = tuplecount;
				if (tuplecount > point_thredhold)
				{
					// 如果点数大于阈值,则加入队列等待处理
					qtiles.push(t1);
				}
				else
				{
					// 否则直接加入到最后的结果集中
					vtiles.push_back(t1);
				}
			}
			else
			{
				elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 2
			Tile t2{t.x * 2 + 1, t.y * 2, t.z + 1, centerx, centery, t.maxx, t.maxy};
			_extent.str("");
			sql_buffer.str("");
			_extent << "'BOX(" << t2.minx << " " << t2.miny << "," << t2.maxx << " " << t2.maxy
					<< ")'::BOX2D";
			sql_buffer << "with a as (select (st_dumppoints("
					   << "\"" << column
					   << "\""
						  ")).geom as geom from "
					   << "\"" << schema << "\""
					   << "."
					   << "\"" << table << "\""
					   << " where "
					   << "\"" << column << "\""
					   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
					   << _extent.str();
			ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
				unsigned tuplecount = atoi(tupleval);
				pfree(tupleval);
				t2.pts = tuplecount;
				if (tuplecount > point_thredhold)
				{
					// 如果点数大于阈值,则加入队列等待处理
					qtiles.push(t2);
				}
				else
				{
					// 否则直接加入到最后的结果集中
					vtiles.push_back(t2);
				}
			}
			else
			{
				elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
			// 3
			Tile t3{t.x * 2, t.y * 2 + 1, t.z + 1, t.minx, t.miny, centerx, centery};
			_extent.str("");
			sql_buffer.str("");
			_extent << "'BOX(" << t3.minx << " " << t3.miny << "," << t3.maxx << " " << t3.maxy
					<< ")'::BOX2D";
			sql_buffer << "with a as (select (st_dumppoints("
					   << "\"" << column
					   << "\""
						  ")).geom as geom from "
					   << "\"" << schema << "\""
					   << "."
					   << "\"" << table << "\""
					   << " where "
					   << "\"" << column << "\""
					   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
					   << _extent.str();
			ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
				unsigned int tuplecount = atoi(tupleval);
				pfree(tupleval);
				t3.pts = tuplecount;
				if (tuplecount > point_thredhold)
				{
					// 如果点数大于阈值,则加入队列等待处理
					qtiles.push(t3);
				}
				else
				{
					// 否则直接加入到最后的结果集中
					vtiles.push_back(t3);
				}
			}
			else
			{
				elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
			// 4
			Tile t4{t.x * 2 + 1, t.y * 2 + 1, t.z + 1, centerx, t.miny, t.maxx, centery};
			_extent.str("");
			sql_buffer.str("");
			_extent << "'BOX(" << t4.minx << " " << t4.miny << "," << t4.maxx << " " << t4.maxy
					<< ")'::BOX2D";
			sql_buffer << "with a as (select (st_dumppoints("
					   << "\"" << column
					   << "\""
						  ")).geom as geom from "
					   << "\"" << schema << "\""
					   << "."
					   << "\"" << table << "\""
					   << " where "
					   << "\"" << column << "\""
					   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
					   << _extent.str();

			ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
				unsigned int tuplecount = atoi(tupleval);
				pfree(tupleval);
				t4.pts = tuplecount;
				if (tuplecount > point_thredhold)
				{
					// 如果点数大于阈值,则加入队列等待处理
					qtiles.push(t4);
				}
				else
				{
					// 否则直接加入到最后的结果集中
					vtiles.push_back(t4);
				}
			}
			else
			{
				elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}

		vtiles.push_back(t);
		qtiles.pop();
	}

	// 将剩余的没有划分的加入到最终结果
	while (!qtiles.empty())
	{
		vtiles.push_back(qtiles.front());
		qtiles.pop();
	}
#ifdef DEBUG
	auto indexend = system_clock::now();
	auto indexdiff = std::chrono::duration_cast<std::chrono::milliseconds>(indexend - indexstart);
	sql_buffer.str("");
	sql_buffer << "build index use:" << indexdiff.count() << "ms";
	elog(NOTICE, "%s", sql_buffer.str().c_str());

	sql_buffer.str("");
	sql_buffer << "DROP TABLE IF EXISTS indextable";
	SPI_exec(sql_buffer.str().c_str(), 0);
	sql_buffer.str("");
	sql_buffer << "CREATE TABLE indextable(geom geometry,pcount int,level int, row int,col int)";
	SPI_exec(sql_buffer.str().c_str(), 0);

	for (auto t : vtiles)
	{
		sql_buffer.str("");
		sql_buffer << "insert into indextable(geom, pcount, level,row,col) values (ST_MakeEnvelope( " << t.minx
				   << "," << t.miny << "," << t.maxx << "," << t.maxy << "," << srid << ")," << t.pts << ","
				   << t.z << "," << t.y << "," << t.x << ");";
		SPI_execute(sql_buffer.str().c_str(), false, 0);
	}

#endif
	// 到这里我们就可以准备生成矢量瓦片了
	std::vector<SimpleStatus> simple_table_status;
	simple_table_status.resize(20, SimpleStatus::UNKNOWN);
	std::string tablename;

	for (auto t : vtiles)
	{
		if (t.pts == 0)
		{
			continue;
		}
		// 检查简化表是否存在
		// 状态未知
		if (simple_table_status.at(t.z) == SimpleStatus::UNKNOWN)
		{
			// 如果以前没有检查到存在简化表先判断一次
			tablename = std::string("pyd_") + table + std::string("_") + column + std::string("_") +
						std::to_string(t.z);
			memset(query, 0, QUERYSIZE);
			snprintf(
				query,
				QUERYSIZE,
				"SELECT count(*) from pyramid_columns where f_schema_name= \'%s\' and f_pyramid_table_name = \'%s\';",
				schema,
				tablename.c_str());
			ret = SPI_exec(query, 1);
			// 这里我们重复使用 tablename 变量作为了全路径
			tablename = "\"" + std::string(schema) + "\".\"" + std::string("pyd_") + table +
						std::string("_") + column + std::string("_") + std::to_string(t.z) + "\"";

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *count = SPI_getvalue(tuple, tupdesc, 1);
				if (atoi(count) == 0)
				{
					// 简化表不存在
					tablename = "\"" + std::string(schema) + "\".\"" + std::string(table) + "\"";
					simple_table_status.at(t.z) = SimpleStatus::NOTEXIST;
				}
				else
				{
					// 简化表存在
					tablename = "\"" + std::string(schema) + "\".\"" + std::string("pyd_") + table +
								std::string("_") + column + std::string("_") + std::to_string(t.z) +
								"\"";
					simple_table_status.at(t.z) = SimpleStatus::EXIST;
				}
				pfree(count);
			}
			else
			{
				elog(ERROR, "%s: SPI_execute error!SQL:%s", __FUNCTION__, query);
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
		else if (simple_table_status.at(t.z) == SimpleStatus::NOTEXIST)
		{
			// 不存在
			tablename = "\"" + std::string(schema) + "\".\"" + std::string(table) + "\"";
		}

		long long id = (((long long)t.z) << 50) + (((long long)t.x) << 25) + t.y;

#ifdef DEBUG
		auto tilestart = system_clock::now();
#endif
		// 这里我们可以开始生成矢量金字塔了
		snprintf(query,
				 QUERYSIZE,
				 "WITH mvtgeom AS(SELECT ST_AsMVTGeom(\"%s\", ST_MakeEnvelope(%f,%f,%f,%f,%d)) AS geom FROM %s "
				 " WHERE \"%s\" && ST_MakeEnvelope(%f,%f,%f,%f,%d))"
				 " insert into \"%s\".\"tile_%s_%s\"(id,mvt)(select %lld,ST_AsMVT(mvtgeom.*) from mvtgeom)",
				 column,
				 t.minx,
				 t.miny,
				 t.maxx,
				 t.maxy,
				 srid,
				 tablename.c_str(),
				 column,
				 t.minx,
				 t.miny,
				 t.maxx,
				 t.maxy,
				 srid,
				 schema,
				 table,
				 column,
				 id);

		SPI_exec(query, 1);
#ifdef DEBUG
		// elog(NOTICE, "%s:mvt sql:%s", __FUNCTION__, query);
		auto tileend = system_clock::now();
		auto tilediff = std::chrono::duration_cast<std::chrono::milliseconds>(tileend - tilestart);
		sql_buffer.str("");
		sql_buffer << "build " << t.z << "/" << t.x << "/" << t.y << " use " << tilediff.count() << " ms";
		elog(NOTICE, "%s", sql_buffer.str().c_str());
#endif
	}

	SPI_finish();
	PG_RETURN_BOOL(true);
}

/**
 * @brief 用来更新一个 geometry 数据变化的概化表和矢量金字塔
 *
 * @return Datum
 */
Datum updatePyramid(PG_FUNCTION_ARGS)
{
	// 1. 开始检查参数

	if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2) || PG_ARGISNULL(3))
	{
		elog(NOTICE, "%s:%d:paramater can not be null", __FUNCTION__, __LINE__);
		PG_RETURN_BOOL(false);
	}

	std::string schemaname = std::string(text_to_cstring(PG_GETARG_TEXT_P(0)));
	std::string tablename = std::string(text_to_cstring(PG_GETARG_TEXT_P(1)));
	std::string columname = std::string(text_to_cstring(PG_GETARG_TEXT_P(2)));
	unsigned int max_level = PG_GETARG_INT32(4);
	if (max_level < 0 || max_level > MAX_LEVEL)
	{
		elog(ERROR, "exceed the max level:%d", MAX_LEVEL);
		PG_RETURN_BOOL(false);
	}
	int origin_srid = 0;
	GBOX *box;
	int ret = 0;
	box = (GBOX *)PG_GETARG_POINTER(3);
	if (box->xmax - box->xmin <= 0 || box->ymax - box->ymin <= 0)
	{
		elog(ERROR, "%s: Geometric bounds are too small", __FUNCTION__);
		PG_RETURN_BOOL(false);
	}

	// 连接 SPI
	if (SPI_OK_CONNECT != SPI_connect())
	{
		elog(ERROR, "%s: could not connect to SPI manager", __FUNCTION__);

		PG_RETURN_BOOL(false);
	}

	std::ostringstream sql_buffer;
	sql_buffer << std::setprecision(16);

	// 1.1 获取 geometry 类型
	std::string geomtype;
	sql_buffer.str("");
	sql_buffer << "SELECT GeometryType(\"" << columname << "\") FROM "
			   << "\"" << schemaname << "\"."
			   << "\"" << tablename << "\""
			   << " LIMIT 1";

	ret = SPI_exec(sql_buffer.str().c_str(), 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		if (SPI_processed > 0)
		{
			HeapTuple tuple = tuptable->vals[0];
			char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
			geomtype = std::string(tupleval);
			pfree(tupleval);
		}
		else
		{
			elog(ERROR, "%s: can not get the geometry type", __FUNCTION__);
			SPI_finish();
			PG_RETURN_BOOL(false);
		}
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 获取 srid
	sql_buffer.str("");
	sql_buffer << "select srid from geometry_columns where f_table_name="
			   << "\'" << tablename << "\'"
			   << " and f_geometry_column= "
			   << "\'" << columname << "\'"
			   << " and f_table_schema= "
			   << "\'" << schemaname << "\'";

	ret = SPI_exec(sql_buffer.str().c_str(), 1);

	// 获取执行结果
	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		origin_srid = atoi(tupleval);
		pfree(tupleval);
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 1.3 根据获取到的概化的表，然后我们来先删除原来的数据
	sql_buffer.str("");
	sql_buffer << " select f_pyramid_table_name,config from pyramid_columns where f_schema_name= "
			   << "\'" << schemaname << "\'"
			   << " and f_table_name = "
			   << "\'" << tablename << "\'"
			   << " and f_geometry_column = "
			   << "\'" << columname << "\'";
#ifdef DEBUG
	elog(NOTICE, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
#endif
	ret = SPI_exec(sql_buffer.str().c_str(), 0);

	if (ret < 0)
	{
		elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	SPITupleTable *tuptable = SPI_tuptable;
	TupleDesc tupdesc = tuptable->tupdesc;
	size_t recordcount = SPI_processed;
	for (size_t i = 0; i < recordcount; i++)
	{
		HeapTuple tuple = tuptable->vals[i];
		char *val1 = SPI_getvalue(tuple, tupdesc, 1);
		std::string pydtable = std::string(val1);
		char *val2 = SPI_getvalue(tuple, tupdesc, 2);
		std::string config = std::string(val2);
		pfree(val1);
		pfree(val2);

		// 这里我们开始删除与边框相交的数据
		sql_buffer.str("");
		sql_buffer << "DELETE FROM "
				   << "\"" << schemaname << "\"."
				   << "\"" << pydtable << "\" WHERE \"" << columname << "\" && ST_MakeEnvelope(" << box->xmin
				   << "," << box->ymin << "," << box->xmax << "," << box->ymax << "," << origin_srid << ")";

		int ret = SPI_exec(sql_buffer.str().c_str(), 0);

		if (ret < 0)
		{
			elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
			SPI_finish();
			PG_RETURN_BOOL(false);
		}
		LevelConfig e;
		parseConfig(config.c_str(), e);

		// 1.3 这里将新数据插入到概化表中
		if (geomtype == "MULTIPOLYGON" || geomtype == "POLYGON" || geomtype == "LINESTRING" ||
			geomtype == "MULTILINESTRING")
		{
			sql_buffer.str("");
			sql_buffer << "INSERT INTO "
					   << "\"" << schemaname << "\"."
					   << "\"" << pydtable << "\"(";
			for (auto attr : e.attribute)
			{
				sql_buffer << "\"" << attr << "\""
						   << ",";
			}
			sql_buffer << "\"" << columname << "\")";
			sql_buffer << " SELECT ";
			for (auto attr : e.attribute)
			{
				sql_buffer << "\"" << attr << "\""
						   << ",";
			}

			if (e.level > 10)
			{
				sql_buffer << "st_simplify(\"" << columname << "\"," << e.resolution << ","
						   << e.preserveCollapsed << ") as \"" << columname << "\"";
			}
			else
			{
				sql_buffer << "st_snaptogrid(\"" << columname << "\"," << e.resolution << ") as \""
						   << columname << "\"";
			}

			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\"";

			// filter
			if (e.filter != "null")
			{
				sql_buffer << " where " << e.filter;
				sql_buffer << " and "
						   << "\"" << columname << "\""
						   << " && ST_MakeEnvelope(" << box->xmin << "," << box->ymin << ","
						   << box->xmax << "," << box->ymax << "," << origin_srid << ")";
			}
			else
			{
				sql_buffer << " where "
						   << "\"" << columname << "\""
						   << " && ST_MakeEnvelope(" << box->xmin << "," << box->ymin << ","
						   << box->xmax << "," << box->ymax << "," << origin_srid << ")";
			}

			ret = SPI_exec(sql_buffer.str().c_str(), 1);
			if (ret < 0)
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			sql_buffer.str("");
			sql_buffer << "DELETE FROM \"" << schemaname << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_" << e.level
					   << "\" WHERE ST_ISEMPTY(\"" << columname << "\")";
			ret = SPI_exec(sql_buffer.str().c_str(), 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
		else if (geomtype == "POINT" || geomtype == "MULTIPOINT")
		{
			double dbbox[4] = {0};

			sql_buffer.str("");
			sql_buffer << "SELECT ST_EstimatedExtent("
					   << " \'" << schemaname << "\'"
					   << ",\'" << tablename << "\'"
					   << ",\'" << columname << "\')";
			ret = SPI_exec(sql_buffer.str().c_str(), 0);

			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *bbox = SPI_getvalue(tuple, tupdesc, 1);
				if (bbox)
				{
					getextent(bbox, dbbox);
					pfree(bbox);
				}
				else
				{
					elog(ERROR, "%s:you need to analyze the table %s first.", __FUNCTION__, tablename.c_str());
					SPI_finish();
					PG_RETURN_BOOL(false);
				}
			}
			else
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			int xbuckets = (int(dbbox[2]) - int(dbbox[0])) / e.resolution;
			int ybuckets = (int(dbbox[3]) - int(dbbox[1])) / e.resolution;

			// 这里我们只生成一个临时表
			sql_buffer.str("");
			sql_buffer << "create table \"";
			sql_buffer << schemaname;
			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\""
					   << " as select  \"" << columname << "\" from (";
			sql_buffer << "SELECT WIDTH_BUCKET(st_x("
					   << "\"" << columname << "\""
					   << "), " << int(dbbox[0]) << "," << int(dbbox[2]) + 1 << "," << xbuckets
					   << ") grid_x,"
					   << "WIDTH_BUCKET(st_y("
					   << "\"" << columname << "\""
					   << ")," << int(dbbox[1]) << ", " << int(dbbox[3]) + 1 << ", " << ybuckets
					   << ")   grid_y,"
					   << "ST_GeometryN(ST_collect("
					   << "\"" << columname << "\""
					   << "),1) \"" << columname << "\"";

			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\"";

			// filter
			if (e.filter != "null")
			{
				sql_buffer << " where " << e.filter;
				sql_buffer << " and "
						   << "\"" << columname << "\""
						   << " && ST_MakeEnvelope(" << box->xmin << "," << box->ymin << ","
						   << box->xmax << "," << box->ymax << "," << origin_srid << ")";
			}
			else
			{
				sql_buffer << " where "
						   << "\"" << columname << "\""
						   << " && ST_MakeEnvelope(" << box->xmin << "," << box->ymin << ","
						   << box->xmax << "," << box->ymax << "," << origin_srid << ")";
			}

			sql_buffer << " GROUP BY grid_x,grid_y) as temp";

			ret = SPI_exec(sql_buffer.str().c_str(), 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 这里我们将附带的信息加入到概化表中

			sql_buffer.str("");
			sql_buffer << "INSERT INTO "
					   << "\"" << schemaname << "\"."
					   << "\"" << pydtable << "\"(";
			for (auto attr : e.attribute)
			{
				sql_buffer << "\"" << attr << "\""
						   << ",";
			}
			sql_buffer << "\"" << columname << "\")";
			sql_buffer << " SELECT  ";

			// attribute
			for (auto attr : e.attribute)
			{
				sql_buffer << "a.\"" << attr << "\" ,";
			}
			// geometry
			sql_buffer << "a.\"" << columname << "\"";
			// from
			sql_buffer << " from "
					   << "\"" << schemaname << "\""
					   << "."
					   << "\"" << tablename << "\" as a,"
					   << "\"" << schemaname;

			sql_buffer << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\" as b";

			sql_buffer << " where a."
					   << "\"" << columname << "\""
					   << "=b."
					   << "\"" << columname << "\"";

			ret = SPI_exec(sql_buffer.str().c_str(), 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}

			// 删除临时表
			sql_buffer.str("");
			sql_buffer << "DROP TABLE IF EXISTS \"" << schemaname << "\""
					   << ".\"pyd_" << tablename << "_" << columname << "_temp_" << e.level << "\"";
			ret = SPI_exec(sql_buffer.str().c_str(), 1);

			if (ret <= 0)
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}

#ifdef DEBUG
		elog(NOTICE, "%s", sql_buffer.str().c_str());
#endif
	}

	// 2.1 这里我们开始更新 tile 表
	// 检查是否存在一个瓦片的表
	sql_buffer.str("");
	sql_buffer << "SELECT count(*) from pg_tables where schemaname = "
			   << "\'" << schemaname << "\'"
			   << " and "
			   << " tablename= "
			   << "\'"
			   << "tile_" << tablename << "_" << columname << "\'";
	ret = SPI_exec(sql_buffer.str().c_str(), 0);

	if (ret > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		TupleDesc tupdesc = tuptable->tupdesc;
		HeapTuple tuple = tuptable->vals[0];
		char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
		int count = atoi(tupleval);

		if (count == 0)
		{
			elog(NOTICE, "does not have a tile table");
			SPI_finish();
			PG_RETURN_BOOL(true);
		}

		pfree(tupleval);
	}
	else
	{
		elog(ERROR, "%s: SPI_execute error!", __FUNCTION__);
		SPI_finish();
		PG_RETURN_BOOL(false);
	}

	// 如果存在一个表的话，我们需要重新构建矢量金字塔
	// 构建稀疏矩阵

	std::queue<Tile> qtiles;
	std::vector<Tile> vtiles;
	vtiles.reserve(20000);

	unsigned int point_thredhold = 10000;
	double minx, miny, maxx, maxy, centerx, centery;
	std::ostringstream _extent;
	_extent << std::setprecision(15);

	if (origin_srid == 4326)
	{
		minx = -180.0000000000000000;
		miny = -270.0000000000000000;
		maxx = 180.0000000000000000;
		maxy = 90.0000000000000000;
	}
	else if (origin_srid == 3857)
	{
		minx = -20037508.3427870012819767;
		miny = -20037508.3427809961140156;
		maxx = 20037508.3427809998393059;
		maxy = 20037508.3427870012819767;
	}
	else
	{
		elog(ERROR, "%s:Only supports 4326 and 3857 coordinate systems.", __FUNCTION__);
		PG_RETURN_BOOL(false);
	}
#ifdef DEBUG
	auto indexstart = system_clock::now();
#endif
	// 添加初始 Tile
	qtiles.push({0, 0, 0, minx, miny, maxx, maxy, INT_MAX});

	while (!qtiles.empty())
	{
		auto t = qtiles.front();

		// 这里加入等级限制
		if (t.z >= max_level)
		{
			break;
		}

		// 检查当前点数
		if (t.pts > point_thredhold)
		{
			// 如果大于当前设定的容限值，则将其四分后，加入到队列，等待后续处理
			// 1
			centerx = (t.minx + t.maxx) / 2;
			centery = (t.miny + t.maxy) / 2;
			Tile t1{t.x * 2, t.y * 2, t.z + 1, t.minx, centery, centerx, t.maxy};

			if (isTouch(box, t1))
			{
				_extent.str("");
				sql_buffer.str("");
				_extent << "'BOX(" << t1.minx << " " << t1.miny << "," << t1.maxx << " " << t1.maxy
						<< ")'::BOX2D";
				sql_buffer << "with a as (select (st_dumppoints("
						   << "\"" << columname
						   << "\""
							  ")).geom as geom from "
						   << "\"" << schemaname << "\""
						   << "."
						   << "\"" << tablename << "\""
						   << " where "
						   << "\"" << columname << "\""
						   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
						   << _extent.str();

				ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

				// 获取执行结果
				if (ret > 0 && SPI_tuptable != NULL)
				{
					SPITupleTable *tuptable = SPI_tuptable;
					TupleDesc tupdesc = tuptable->tupdesc;
					HeapTuple tuple = tuptable->vals[0];
					char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
					unsigned int tuplecount = atoi(tupleval);
					pfree(tupleval);
					t1.pts = tuplecount;
					if (tuplecount > point_thredhold)
					{
						// 如果点数大于阈值,则加入队列等待处理
						qtiles.push(t1);
					}
					else
					{
						// 否则直接加入到最后的结果集中
						vtiles.push_back(t1);
					}
				}
				else
				{
					elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
					SPI_finish();
					PG_RETURN_BOOL(false);
				}
			}

			// 2
			Tile t2{t.x * 2 + 1, t.y * 2, t.z + 1, centerx, centery, t.maxx, t.maxy};

			if (isTouch(box, t2))
			{
				_extent.str("");
				sql_buffer.str("");
				_extent << "'BOX(" << t2.minx << " " << t2.miny << "," << t2.maxx << " " << t2.maxy
						<< ")'::BOX2D";
				sql_buffer << "with a as (select (st_dumppoints("
						   << "\"" << columname
						   << "\""
							  ")).geom as geom from "
						   << "\"" << schemaname << "\""
						   << "."
						   << "\"" << tablename << "\""
						   << " where "
						   << "\"" << columname << "\""
						   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
						   << _extent.str();
				ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

				// 获取执行结果
				if (ret > 0 && SPI_tuptable != NULL)
				{
					SPITupleTable *tuptable = SPI_tuptable;
					TupleDesc tupdesc = tuptable->tupdesc;
					HeapTuple tuple = tuptable->vals[0];
					char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
					unsigned tuplecount = atoi(tupleval);
					pfree(tupleval);
					t2.pts = tuplecount;
					if (tuplecount > point_thredhold)
					{
						// 如果点数大于阈值,则加入队列等待处理
						qtiles.push(t2);
					}
					else
					{
						// 否则直接加入到最后的结果集中
						vtiles.push_back(t2);
					}
				}
				else
				{
					elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
					SPI_finish();
					PG_RETURN_BOOL(false);
				}
			}

			// 3
			Tile t3{t.x * 2, t.y * 2 + 1, t.z + 1, t.minx, t.miny, centerx, centery};

			if (isTouch(box, t3))
			{
				_extent.str("");
				sql_buffer.str("");
				_extent << "'BOX(" << t3.minx << " " << t3.miny << "," << t3.maxx << " " << t3.maxy
						<< ")'::BOX2D";
				sql_buffer << "with a as (select (st_dumppoints("
						   << "\"" << columname
						   << "\""
							  ")).geom as geom from "
						   << "\"" << schemaname << "\""
						   << "."
						   << "\"" << tablename << "\""
						   << " where "
						   << "\"" << columname << "\""
						   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
						   << _extent.str();
				ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

				// 获取执行结果
				if (ret > 0 && SPI_tuptable != NULL)
				{
					SPITupleTable *tuptable = SPI_tuptable;
					TupleDesc tupdesc = tuptable->tupdesc;
					HeapTuple tuple = tuptable->vals[0];
					char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
					unsigned int tuplecount = atoi(tupleval);
					pfree(tupleval);
					t3.pts = tuplecount;
					if (tuplecount > point_thredhold)
					{
						// 如果点数大于阈值,则加入队列等待处理
						qtiles.push(t3);
					}
					else
					{
						// 否则直接加入到最后的结果集中
						vtiles.push_back(t3);
					}
				}
				else
				{
					elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
					SPI_finish();
					PG_RETURN_BOOL(false);
				}
			}

			// 4
			Tile t4{t.x * 2 + 1, t.y * 2 + 1, t.z + 1, centerx, t.miny, t.maxx, centery};
			if (isTouch(box, t4))
			{
				_extent.str("");
				sql_buffer.str("");
				_extent << "'BOX(" << t4.minx << " " << t4.miny << "," << t4.maxx << " " << t4.maxy
						<< ")'::BOX2D";
				sql_buffer << "with a as (select (st_dumppoints("
						   << "\"" << columname
						   << "\""
							  ")).geom as geom from "
						   << "\"" << schemaname << "\""
						   << "."
						   << "\"" << tablename << "\""
						   << " where "
						   << "\"" << columname << "\""
						   << " && " << _extent.str() << ") select count(*) from a where a.geom && "
						   << _extent.str();

				ret = SPI_execute(sql_buffer.str().c_str(), true, 1);

				// 获取执行结果
				if (ret > 0 && SPI_tuptable != NULL)
				{
					SPITupleTable *tuptable = SPI_tuptable;
					TupleDesc tupdesc = tuptable->tupdesc;
					HeapTuple tuple = tuptable->vals[0];
					char *tupleval = SPI_getvalue(tuple, tupdesc, 1);
					unsigned int tuplecount = atoi(tupleval);
					pfree(tupleval);
					t4.pts = tuplecount;
					if (tuplecount > point_thredhold)
					{
						// 如果点数大于阈值,则加入队列等待处理
						qtiles.push(t4);
					}
					else
					{
						// 否则直接加入到最后的结果集中
						vtiles.push_back(t4);
					}
				}
				else
				{
					elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
					SPI_finish();
					PG_RETURN_BOOL(false);
				}
			}
		}

		vtiles.push_back(t);
		qtiles.pop();
	}

	// 将剩余的没有划分的加入到最终结果
	while (!qtiles.empty())
	{
		vtiles.push_back(qtiles.front());
		qtiles.pop();
	}
#ifdef DEBUG
	auto indexend = system_clock::now();
	auto indexdiff = std::chrono::duration_cast<std::chrono::milliseconds>(indexend - indexstart);
	sql_buffer.str("");
	sql_buffer << "build index use:" << indexdiff.count() << "ms";
	elog(NOTICE, "%s", sql_buffer.str().c_str());

	sql_buffer.str("");
	sql_buffer << "DROP TABLE IF EXISTS indextable";
	SPI_exec(sql_buffer.str().c_str(), 0);
	sql_buffer.str("");
	sql_buffer << "CREATE TABLE indextable(geom geometry,pcount int,level int, row int,col int)";
	SPI_exec(sql_buffer.str().c_str(), 0);

	for (auto t : vtiles)
	{
		sql_buffer.str("");
		sql_buffer << "insert into indextable(geom, pcount, level,row,col) values (ST_MakeEnvelope( " << t.minx
				   << "," << t.miny << "," << t.maxx << "," << t.maxy << "," << origin_srid << ")," << t.pts
				   << "," << t.z << "," << t.y << "," << t.x << ");";
		SPI_execute(sql_buffer.str().c_str(), false, 0);
	}

#endif

	// 到这里我们已经找到所有受到影响的瓦片，我们先将原来瓦片删除，然后插入新的瓦片
	// 到这里我们就可以准备生成矢量瓦片了
	std::vector<SimpleStatus> simple_table_status;
	simple_table_status.resize(20, SimpleStatus::UNKNOWN);
	std::string simptablename;

	for (auto t : vtiles)
	{
		if (t.pts == 0)
		{
			continue;
		}
		// 检查简化表是否存在
		// 状态未知
		if (simple_table_status.at(t.z) == SimpleStatus::UNKNOWN)
		{
			// 如果以前没有检查到存在简化表先判断一次
			simptablename = std::string("pyd_") + tablename + std::string("_") + columname +
							std::string("_") + std::to_string(t.z);
			sql_buffer.str("");
			sql_buffer << " SELECT count(*) from pyramid_columns where f_schema_name=  "
					   << "\'" << schemaname << "\'"
					   << " and f_pyramid_table_name = "
					   << "\'" << simptablename << "\'";
			ret = SPI_exec(sql_buffer.str().c_str(), 1);
			// 这里我们重复使用 tablename 变量作为了全路径
			simptablename = "\"" + std::string(schemaname) + "\".\"" + std::string("pyd_") + tablename +
							std::string("_") + columname + std::string("_") + std::to_string(t.z) + "\"";

			// 获取执行结果
			if (ret > 0 && SPI_tuptable != NULL)
			{
				SPITupleTable *tuptable = SPI_tuptable;
				TupleDesc tupdesc = tuptable->tupdesc;
				HeapTuple tuple = tuptable->vals[0];
				char *count = SPI_getvalue(tuple, tupdesc, 1);
				if (atoi(count) == 0)
				{
					// 简化表不存在
					simptablename =
						"\"" + std::string(schemaname) + "\".\"" + std::string(tablename) + "\"";
					simple_table_status.at(t.z) = SimpleStatus::NOTEXIST;
				}
				else
				{
					// 简化表存在
					simptablename = "\"" + std::string(schemaname) + "\".\"" + std::string("pyd_") +
									tablename + std::string("_") + columname + std::string("_") +
									std::to_string(t.z) + "\"";
					simple_table_status.at(t.z) = SimpleStatus::EXIST;
				}
				pfree(count);
			}
			else
			{
				elog(ERROR, "%s:%d:sql:%s", __FUNCTION__, __LINE__, sql_buffer.str().c_str());
				SPI_finish();
				PG_RETURN_BOOL(false);
			}
		}
		else if (simple_table_status.at(t.z) == SimpleStatus::NOTEXIST)
		{
			// 不存在
			simptablename = "\"" + std::string(schemaname) + "\".\"" + std::string(tablename) + "\"";
		}

		long long id = (((long long)t.z) << 50) + (((long long)t.x) << 25) + t.y;

		// 删除原来的旧数据
		sql_buffer.str("");
		sql_buffer << "DELETE FROM "
				   << "\"" << schemaname << "\"."
				   << "\""
				   << "tile_" << tablename << "_" << columname << "\" WHERE id= " << id;
		SPI_exec(sql_buffer.str().c_str(), 0);

#ifdef DEBUG
		auto tilestart = system_clock::now();
#endif
		// 这里我们可以开始生成矢量金字塔了
		char query[QUERYSIZE];
		snprintf(query,
				 QUERYSIZE,
				 "WITH mvtgeom AS(SELECT ST_AsMVTGeom(\"%s\", ST_MakeEnvelope(%f,%f,%f,%f,%d)) AS geom FROM %s "
				 " WHERE \"%s\" && ST_MakeEnvelope(%f,%f,%f,%f,%d))"
				 " insert into \"%s\".\"tile_%s_%s\"(id,mvt)(select %lld,ST_AsMVT(mvtgeom.*) from mvtgeom)",
				 columname.c_str(),
				 t.minx,
				 t.miny,
				 t.maxx,
				 t.maxy,
				 origin_srid,
				 tablename.c_str(),
				 columname.c_str(),
				 t.minx,
				 t.miny,
				 t.maxx,
				 t.maxy,
				 origin_srid,
				 schemaname.c_str(),
				 tablename.c_str(),
				 columname.c_str(),
				 id);

		SPI_exec(query, 1);
#ifdef DEBUG
		// elog(NOTICE, "%s:mvt sql:%s", __FUNCTION__, query);
		auto tileend = system_clock::now();
		auto tilediff = std::chrono::duration_cast<std::chrono::milliseconds>(tileend - tilestart);
		sql_buffer.str("");
		sql_buffer << "build " << t.z << "/" << t.x << "/" << t.y << " use " << tilediff.count() << " ms";
		elog(NOTICE, "%s", sql_buffer.str().c_str());
#endif
	}
	SPI_finish();
	PG_RETURN_BOOL(true);
}