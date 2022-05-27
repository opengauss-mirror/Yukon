/*
 *
 * GSGUtil.cpp
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

#include "GSGUtil.h"

/**
 * @brief 将合法的小写字母转换成大写帘字母
 */
static char char_toupper(int in)
{
    if (in < 0 || in > 127)
        return '.';

    if (in >= 'a' && in <= 'z')
    {
        return in - ('a' - 'A');
    }

    return in;
}


/**
 * @brief 将 cstring 类型转换未 textp 类型
 * @note 在 sql 定义的函数中，不支持 cstring 类型
 * @param cstring 
 * @return text* 
 */

/**
 * @brief 将 hex 转换成可显示的 char
 * 
 * @param c 
 * @return unsigned 
 */
static unsigned char _Hex2Char(unsigned char c)
{
    if (c == 0)
    {
        return '0';
    }
    else if (c < 0xa)
    {
        return c + 0x30;
    }
    else
    {
        return c + 0x37;
    }
}

/**
 * @brief 
 * 
 * @param src 要转换的 hex
 * @param dst 转换后的字符
 */
void Hex2Char(unsigned char src, unsigned char *dst)
{
    dst[1] = _Hex2Char(src >> 4);
    dst[0] = _Hex2Char(src & 0x0f);
}

/**
 * @brief 将字符转换成 Hex '01'->1 'fa'->fa
 * 
 * @param c 
 * @return unsigned 
 */
unsigned char _Char2Hex(const unsigned char c)
{
    unsigned char t = char_toupper(c);
    if (t == '0')
    {
        return 0;
    }
    else if (t >= 'A' && t <= 'F')
    {
        return t - 0x37;
    }
    else
    {
        return t - 0x30;
    }
}

unsigned char Char2Hex(const unsigned char *c)
{
    return (_Char2Hex(c[0]) << 4) + _Char2Hex(c[1]);
}