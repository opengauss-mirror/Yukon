/*
 *
 * geosot.cpp
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
#include "geosot.h"

uint64_t GetCode(double x, double y, int precision)
{
	uint32_t m_x;
	uint32_t m_y;
	Dec2code(x, m_x, precision);
	Dec2code(y, m_y, precision);

	return MagicBits(m_x, m_y);
}

bitset<96> GetCode(double x, double y, uint32_t z, int precision)
{
	uint32_t m_x;
	uint32_t m_y;
	Dec2code(x, m_x, precision);
	Dec2code(y, m_y, precision);

	return MagicBitset(m_x, m_y, z);
}

void Dec2code(double &dec, uint32_t &code, int precision)
{
	double val = dec < 0 ? -dec : dec;
	bool G = dec < 0 ? 1 : 0;
	uint32_t D = val;
	double dm = Round((val - D) * 60, 6);
	uint32_t M = dm;
	double seconds = Round((dm - M) * 60, 4);
	uint32_t S = seconds;
	double dotSeconds = (seconds - S) * 2048;
	uint32_t S11 = (uint32_t)round(dotSeconds);
	code = G << 31 | D << 23 | M << 17 | S << 11 | S11;
	code = code >> (32 - precision) << (32 - precision);
}

void Code2Dec(uint32_t x, double &dec)
{
	uint32_t G = x >> 31;          // 1b
	uint32_t D = (x >> 23) & 0xFF; // 8b
	uint32_t M = (x >> 17) & 0x3F; // 6b
	uint32_t S = (x >> 11) & 0x3F; // 6b
	uint32_t S11 = x & 0x7FF;      // 11b

	dec = D + M / 60.0 + (S + S11 / 2048.0) / 3600.0;
	dec = G == 1 ? -dec : dec;
}

int32_t AltitudeToInt(double height, int level)
{
	double pix_size = 0;
	double factor = 0;
	double result = 0;

	if (level >= 0 && level <= 9)
		pix_size = pow(2, 9 - level);
	
	else if (level >= 10 && level <= 15)
		pix_size = pow(2, 15 - level) / 60.0;
	
	else if (level >= 16 && level <= 32)
		pix_size = pow(2, 21 - level) / 3600.0;

	factor = 1.0 / pix_size;
	result = factor * log((EARTH_RADIUS + height) / EARTH_RADIUS) / log(ONEPLUSTHLTA0);
	int32_t code = floor(result);

	return code;
}

void IntToAltitude(int32_t z, short level, double &height)
{
	double factor = 0;
	double pix_size = 0;
	if (level >= 0 && level <= 9)
		pix_size = pow(2, 9 - level);

	else if (level >= 10 && level <= 15)
		pix_size = pow(2, 15 - level) / 60.0;

	else if (level >= 16 && level <= 32)
		pix_size = pow(2, 21 - level) / 3600.0;

	factor = 1.0 / pix_size;
	height = exp(z * log(ONEPLUSTHLTA0) / factor) * EARTH_RADIUS - EARTH_RADIUS;
}

string ToString(uint64_t code, int level)
{
	string strOut = "G";
	char buf[4];
	for (int i = 31; i > 31 - level; i--)
	{
		int v = (code >> i * 2) & 0x3;
		sprintf(buf, "%d", v);
		strOut += buf;
		if (i > 32 - level)
		{
			if (i == 23 || i == 17)
				strOut += "-";
			if (i == 11)
				strOut += ".";
		}
	}
	return strOut;
}

void ToCode(string grid, short &level, uint64_t &code)
{
	uint64_t id = 0L;
    int m_level = 0;
    for (auto c : grid)
    {
        if (isdigit(c))
            {
                char v = DeCodeChar(c);
                int shift = ((31 - m_level) * 2);
                id = id | ((uint64_t)v << shift);
                m_level++;
            }
    }
	level = m_level;
	code = id;
}

char DeCodeChar(char c)
{
        if (c == '1')
            return 1;
        if (c == '2')
            return 2;
        if (c == '3')
            return 3;
        return 0;
}

uint64_t MagicBits(uint32_t lng, uint32_t lat)
{
	return SplitByBits(lng) | (SplitByBits(lat) << 1);
}

bitset<96> MagicBitset(uint32_t lng, uint32_t lat, uint32_t hig)
{
	return SplitByBitset(lng) | (SplitByBitset(lat) << 1) | (SplitByBitset(hig) << 2);
}

void UnMagicBits(uint64_t m, uint32_t &lng, uint32_t &lat)
{
	lng = MergeByBits(m);
	lat = MergeByBits(m >> 1);
}

void UnMagicBitset(bitset<96> m, uint32_t &lng, uint32_t &lat, uint32_t &hig)
{
	lng = MergeByBitset(m);
	lat = MergeByBitset(m >> 1);
	hig = MergeByBitset(m >> 2);
}

uint64_t SplitByBits(uint32_t a)
{
	uint64_t x = a;
	x = (x | x << 32) & 0x00000000FFFFFFFF;
	x = (x | x << 16) & 0x0000FFFF0000FFFF;
	x = (x | x << 8) & 0x00FF00FF00FF00FF;
	x = (x | x << 4) & 0x0F0F0F0F0F0F0F0F;
	x = (x | x << 2) & 0x3333333333333333;
	x = (x | x << 1) & 0x5555555555555555;
	return x;
}

bitset<96> SplitByBitset(unsigned int i)
{
	bitset<96> x = i;
	x = (x | x << 32) & half;
	x = (x | x << 16) & quarter;
	x = (x | x << 8)  & one_eighth;
	x = (x | x << 4)  & one_sixteenth;
	x = (x | x << 2)  & one_thirty_two;
	return x;
}

uint32_t MergeByBits(uint64_t m)
{
	uint64_t x = m & 0x5555555555555555;
	x = (x ^ (x >> 1)) & 0x3333333333333333;
	x = (x ^ (x >> 2)) & 0x0F0F0F0F0F0F0F0F;
	x = (x ^ (x >> 4)) & 0x00FF00FF00FF00FF;
	x = (x ^ (x >> 8)) & 0x0000FFFF0000FFFF;
	x = (x ^ (x >> 16)) & 0x00000000FFFFFFFF;
	return (uint32_t)x;
}

uint32_t MergeByBitset(bitset<96> m)
{
	bitset<96> x = m & one_thirty_two;
	x = (x | (x >> 2)) & one_sixteenth;
	x = (x | (x >> 4)) & one_eighth;
	x = (x | (x >> 8)) & quarter;
	x = (x | (x >> 16)) & half;
	x = (x | (x >> 32)) & all;
	return (uint32_t)(x.to_ulong());
}

double Round(double x, int y)
{
	int64_t mul = pow(10, y);
	if (x >= 0)
		return double((int64_t)(x * mul + 0.5)) / mul;
	else
		return double((int64_t)(x * mul - 0.5)) / mul;
}

 int memcmp_reverse(const char *a, const char *b, int size)
 {
	 uint8_t *m_a = (uint8_t *)a;
	 uint8_t *m_b = (uint8_t *)b;
	 for (; size >= 0; size--)
	 {
		 if (*(m_a + size) != *(m_b + size))
			 return (*(m_a + size) > *(m_b + size)) ? 1 : -1;
	 }
	 return 0;
 }
