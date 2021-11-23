/*
 *
 * YkHashCode.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
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

#if !defined(AFX_YKHASHCODE_H__655A005A_77A3_426D_B890_151E99E6F0F3__INCLUDED_)
#define AFX_YKHASHCODE_H__655A005A_77A3_426D_B890_151E99E6F0F3__INCLUDED_

#pragma once

#include "Stream//YkDefines.h"

namespace Yk {

class TOOLKIT_API YkHashCode  
{
public:
	YkHashCode();

	~YkHashCode();
public:
	static YkInt ShortToHashCode(YkShort nValue);
	static YkInt StringToHashCode(const YkString& strValue);
	static YkInt BoolToHashCode(YkBool bValue);

	//! \brief 快速把字符串转化为HashCode
	//! \remarks 由于要和SFC保持兼容,StringToHashCode中进行了Unicode的转化,非常费时
	//! 其它不需要保持兼容的地方,可以用这个函数,速度快很多
	static YkInt FastStringToHashCode(const YkString& strValue);

	//! \brief 字符串转HashCode
	static YkLong StringToHashCode64(const YkString& strValue);
};
}
#endif

