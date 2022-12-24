/*
 *
 * geosot.h
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
#ifndef GEOSOT_H
#define GEOSOT_H

#include <iostream>
#include <math.h>
#include <bitset>

using std::pow;
using std::string;
using std::bitset;

#define POINTERGETUINT8(a)  (*reinterpret_cast<uint8_t *>(a))
#define POINTERGETUINT16(a) (*reinterpret_cast<uint16_t *>(a))
#define POINTERGETUINT32(a) (*reinterpret_cast<uint32_t *>(a))
#define POINTERGETUINT64(a) (*reinterpret_cast<uint64_t *>(a))

#define PointerGetGEOSOTGrid(a) (reinterpret_cast<GEOSOTGRID *>(a))
#define PointerGetGEOSOTGrid3D(a) (reinterpret_cast<GEOSOTGRID3D *>(a))
#define GEOSOTGRIDSIZE 16
#define GEOSOTGRID3DSIZE 20

#define EARTH_RADIUS 6378137
#define ONEPLUSTHLTA0 1.017453292519943295
static bitset<96> all("000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111");
static bitset<96> half("000000000000000000000000000000001111111111111111000000000000000000000000000000001111111111111111");
static bitset<96> quarter("000000000000000011111111000000000000000011111111000000000000000011111111000000000000000011111111");
static bitset<96> one_eighth ("000000001111000000001111000000001111000000001111000000001111000000001111000000001111000000001111");
static bitset<96> one_sixteenth("000011000011000011000011000011000011000011000011000011000011000011000011000011000011000011000011");
static bitset<96> one_thirty_two("001001001001001001001001001001001001001001001001001001001001001001001001001001001001001001001001");

/**
 *  根据十进制经纬度获取geomgrid值
 * @param x: 经度
 * @param y: 纬度
*/
uint64_t GetCode(double x, double y, int precision);

/**
 *  根据十进制经纬度获取geomgrid值
 * @param x: 经度
 * @param y: 纬度
*/
bitset<96> GetCode(double x, double y, uint32_t z, int precision);

/**
 *  将十进制经纬度值转为经纬度编码
 * @param dec: 经度或纬度编码
 * @param code: 十进制经纬度值
*/
uint32_t Dec2code(double dec, int precision);

/**
 *  将经纬度编码转位十进制经纬度值
 * @param x: 经度或纬度编码
 * @param dec: 十进制经纬度值
*/
double Code2Dec(uint32_t x);

/**
 *  高度转编码
 * @param height: 大地高度
 * @param level: 精度等级
*/
int32_t AltitudeToInt(double height, int level);

/**
 *  编码转高度
 * @param z: 高度编码
 * @param level: 高度等级
 * @param height: 高度
*/
double IntToAltitude(int32_t z, short level);

/**
 *  将geomgrid转为文本形式
 * @param code: geomgrid编码
 * @param level: 精度等级
*/
string ToString(uint64_t code, int level);

/**
 *  将文本形式的geomgrid转为编码形式
 * @param grid: geomgrid文本
 * @param level: 精度等级
*/
void ToCode(string grid, short &level, uint64_t &code);

/**
 *  字符转int
 * @param c: 字符
*/
char DeCodeChar(char c);

/**
 *  将经度编码和纬度编码转为geomgrid值
 * @param lng: 经度
 * @param lat: 纬度
*/
uint64_t MagicBits(uint32_t lng, uint32_t lat);

/**
 *  将经度编码和纬度编码转为geomgrid值
 * @param lng: 经度
 * @param lat: 纬度
 * @param hig: 高度
*/
bitset<96> MagicBitset(uint32_t lng, uint32_t lat, uint32_t hig);

/**
 *  根据geomgrid分离出经度和纬度的编码
 * @param lng: 经度
 * @param lat: 纬度
*/
void UnMagicBits(uint64_t m, uint32_t &lng, uint32_t &lat);

/**
 *  根据geomgrid分离出经度和纬度的编码
 * @param lng: 经度
 * @param lat: 纬度
 * @param hig: 高度
*/
void UnMagicBitset(bitset<96> m, uint32_t &lng, uint32_t &lat, uint32_t &hig);

/**
 *  将32位经纬度编码转为64位形式
 * @param a: 度分秒按位转换后的值
*/
uint64_t SplitByBits(uint32_t a);

/**
 *  将32位经纬度编码转为64位形式
 * @param a: 度分秒按位转换后的值
*/
bitset<96> SplitByBitset(uint32_t a);

/**
 *  从geomgrid值分离其中的单个32位编码
 * @param m: geomgrid值
*/
uint32_t MergeByBits(uint64_t m);

/**
 *  从geomgrid值分离其中的单个32位编码
 * @param m: geomgrid值
*/
uint32_t MergeByBitset(bitset<96> m);

/**
 *  四舍五入
 * @param x: 要操作的值
 * @param y: 保留的位数
*/
double Round(double x, int y);

/**
 *  比较内存里的值大小
 * @param a: 指向内存块的指针
 * @param b: 指向内存块的指针
 * @param n: 要被比较的字节数 - 1 
*/
int memcmp_reverse(uint8_t *a, uint8_t *b, int size);

/**
 *  根据等级获取像素大小，等级错误返回-1
 * @param level: 编码等级
*/
double GetPixSize(int level);

struct GEOSOTGRID
{
	uint32_t size;
	uint16_t flag;
	uint8_t level;
	uint8_t level_min = 0;
	uint64_t data;
	bool operator < (GEOSOTGRID &grid)
	{
		return data < grid.data ? true : false;
	}
};

struct GEOSOTGRID3D
{
	uint32_t size;
	uint16_t flag;
	uint16_t level;
	uint8_t data[12];
	bool operator < (GEOSOTGRID3D &grid)
	{
		return memcmp_reverse(data, grid.data, 11) < 0 ? true : false;
	}
};
#endif