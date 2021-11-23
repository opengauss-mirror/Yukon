/*
 *
 * geomodel_util.c
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


#include "geomodel_util.h"

static char *GeomodelTypeName[] =
    {
    "Unknown",
     "MESH",
     "SURFACE",
     "ANYTYPE"};

/**
 * @brief 定义 geomodel 所支持的类型
 * 
 */
struct GeomodelType_struct
{
    char *typename_C;
    int type;
};

/**
 * @brief 定义 geomodel 所支持的类型数组，用于判断函数 geomodel_typmpod_in 所传入的
 *        类型是否支持，
 * 
 */
struct GeomodelType_struct geomodel_struct_array[] =
    {
        {"MESH", MESHTYPE},
        {"SURFACE", SURFACETYPE},
        {"ANYTYPE", ANYTYPE}};

#define GEOMODELTYPE_STRUCT_ARRAY_LEN (sizeof geomodel_struct_array / sizeof(struct GeomodelType_struct))

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
 * @brief 根据 @type 查找是否具有该类型
 */
int geomodel_type_from_string(const char *str, uint8_t *type)
{
    char *tmpstr;
    int tmpstartpos, tmpendpos;
    int i;

    assert(str);
    assert(type);

    *type = 0;

    /* 删除首部和尾部的空格 */
    tmpstartpos = 0;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ')
        {
            tmpstartpos = i;
            break;
        }
    }

    tmpendpos = strlen(str) - 1;
    for (i = strlen(str) - 1; i >= 0; i--)
    {
        if (str[i] != ' ')
        {
            tmpendpos = i;
            break;
        }
    }

    /* 将字符串拷贝出来，并转换未大写 */
    tmpstr = palloc(tmpendpos - tmpstartpos + 2);
    for (i = tmpstartpos; i <= tmpendpos; i++)
        tmpstr[i - tmpstartpos] = char_toupper(str[i]);

    /* 添加结束符 */
    tmpstr[i - tmpstartpos] = '\0';

    /* 检查类型 */
    for (i = 0; i < GEOMODELTYPE_STRUCT_ARRAY_LEN; i++)
    {
        if (!strcmp(tmpstr, geomodel_struct_array[i].typename_C))
        {
            *type = geomodel_struct_array[i].type;
            pfree(tmpstr);
            return YK_SUCCESS;
        }
    }

    pfree(tmpstr);

    return Yk_FAILURE;
}

/**
 * @brief 根据 @type 返回具体的类型
 */
const char *geomodel_type_name(uint8_t type)
{
    if (type > 15)
    {
        /* assert(0); */
        return "Invalid type";
    }
    return GeomodelTypeName[(int)type];
}

/**
 * @brief 将 cstring 类型转换未 textp 类型
 * @note 在 sql 定义的函数中，不支持 cstring 类型
 * @param cstring 
 * @return text* 
 */
text *cstring2text(const char *cstring)
{
    text *output;
    size_t sz;

    /* 判断是否为空 */
    if (!cstring)
        return NULL;

    sz = strlen(cstring);
    output = palloc(sz + VARHDRSZ);
    if (!output)
        return NULL;
    SET_VARSIZE(output, sz + VARHDRSZ);
    if (sz)
        memcpy(VARDATA(output), cstring, sz);
    return output;
}

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
