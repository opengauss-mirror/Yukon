/*
 *
 * UGGeoHash.h
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

#ifndef UGGEOHASH_H
#define UGGEOHASH_H

#include "Stream/YkDefines.h"


namespace Yk
{

	//UGGeoHashKey 设计

	//	|			|			|
	//	|	01		|	11		|
	//	|___________|___________|
	//	|			|			|
	//	|	00		|	10		|

	/* 总共64位
	   前两位空出
	   3~58 存储空间编码值，59-64 存储level
	   第0层表示全范围，编码 0000...0000 (查询全部数据用 条件 >= 0 < 0011111...111)
	   第1层四分		编码 0000...0001    0011.....0001 
	   第2层在四分		编码 000000...0010  001111...0010
	   ...
	   第28层...	    编码
	*/

	#define PERCENTAGE 100
	#define MAXHASHLEVEL 28
	/**
	 * 用于查询中权重的计算
	 * 第零位不用.
	 * level1 - level 5 需要多次分割，因此权重较小
	 */
	const double WEIGHTS[] = { 0, 0.5, 0.65, 0.75, 0.85, 1.0, 1.5, 8, 10, 12, 16, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };

	typedef struct QueryParameter
	{
		YkLong lGeoKey;
		YkInt nLevel;
		//索引bounds
		YkRect2D rcGeoKey;
		//当前编码格子被查询范围完全包含
		YkDouble dPercentage;
		//与当前编码相交的Querybounds
		YkRect2D rcQueryBounds;
	} QueryParameter;

	class YKEXPORT UGGeoHash
	{
	public:
		UGGeoHash();
		virtual ~UGGeoHash();

		//计算覆盖indexbounds的最少的格子数，可以是多个level
		//核心宗旨，尽量少的格子数，数据范围和格网范围覆盖度最大。
		//数据和格网范围覆盖大于50%，记录格网ID 。
		//当key值大于一定数值，例如50个格网，允许数据与格网范围覆盖<50%情况出现
		//nUpperLimitLevel 上限默认到30层
		//nMaxReturnKeyCount 默认20个
		//另：面和线直接参与计算，面考虑用凸壳；线可以采样
		//另：有子对象具体比较远，可以分开计算
		static YkInt ComputerGeoHash(YkRect2D IndexBounds, YkArray<YkLong> &AryKeys, YkRect2D GeoBounds,
									 YkInt nUpperLimitLevel = 25, YkInt nMaxReturnKeyCount = 20);

		//采用最小相交法，快速计算一~四个相交
		static YkInt ComputerGeoHash2(YkRect2D IndexBounds, YkArray<YkLong> &AryKeys, YkRect2D GeoBounds,
									  YkInt nUpperLimitLevel = 25, YkInt nMaxReturnKeyCount = 20);

		static YkInt ComputerPointGeoHash(YkRect2D IndexBounds, YkLong &GeoHashKey, YkPoint2D pnt, YkInt nUpperLimitLevel = 25);

		//返回索引相交的条件
		//根据查询范围和格网的匹配，匹配的level以下，用大于小于查询，匹配level以上，用key=格网id处理
		//最理想情况，一组key=查询（level以上全部的key），一组大于小于查询
		//最差情况，四组key=查询，四组大于小于查询
		//另：查询如果所有对象编码都在某个level一下，= 条件可以省略
		//二维
		static YkString GetFilter2(YkRect2D IndexBounds, YkRect2D GeoBounds, YkInt nUpperLimitLevel = 25);

		//一维
		static YkString GetFilter1(YkRect2D IndexBounds, YkRect2D GeoBounds, YkInt nUpperLimitLevel = 25, YkInt nKeySizeLimit = 10);

		static YkRect2D GetBoundsByKey(YkRect2D IndexBounds, YkLong nKey);

	public:
		static YkBool IsIntersect(YkRect2D &rc1, YkRect2D &rc2);

	private:
		//返回相交的自格网
		static YkBool GetIntersectHash(QueryParameter &QueryFetch, YkArray<QueryParameter> &aryRest);
		static YkBool GetIntersectHash2(QueryParameter &QueryFetch, YkArray<QueryParameter> &aryRest);
		static YkBool Query(QueryParameter QueryFetch, YkArray<QueryParameter> &aryEquelRest, YkArray<QueryParameter> &aryBetweenRest);
		static YkInt GetMinPercentagePos(YkArray<QueryParameter> &aryRest);
		static YkInt FindLowLevelAndLowPercentage(YkArray<QueryParameter> &aryRest);
		static YkString GetBetweenFilter2(YkLong nGeoHashKey);
		static YkString GetBetweenFilter1(YkLong nGeoHashKey);

		// public:
		// 	static UGbool test_ComputerGeoHash();
		// 	static UGbool test_GetFilter();
		// 	static UGbool test_GetBoundsByKey();
		// 	static UGbool test_ComputerPointGeoHash();
		// 	static UGbool test_ComputerPointGeoHash1();
		// 	static UGbool test_ComputerGeoHash2();
	};
}

#endif
