/*
 *
 * YkSize2D.h
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

#if !defined(AFX_YKSIZE2D_H__C6D5EB89_6542_4FE3_9B39_C2B292F51D6D__INCLUDED_)
#define AFX_YKSIZE2D_H__C6D5EB89_6542_4FE3_9B39_C2B292F51D6D__INCLUDED_

#pragma once

#include "Base/YkDefs.h"

namespace Yk {
//!  \brief 二维矩形大小类。该类存储了矩形的长度cx和宽度cy，类型为YkDouble型。
class BASE_API YkSize2D  
{
public:
	//! \brief 		长度
	YkDouble cx;
	//! \brief 		宽度
	YkDouble cy;

public:
	//! \brief 		缺省构造函数
	YkSize2D();
	
	//! \brief 		缺省构造函数
	~YkSize2D();
	
	//! \brief 		带参构造函数
	//! \param 		cx		长度
	//! \param 		cy		宽度
	YkSize2D(YkDouble dX,YkDouble dY);
	
	//! \brief 		重载==
	friend BASE_API YkBool operator==(const YkSize2D& szSrc,const YkSize2D& sztag);
	
	//! \brief 		重载!=
	friend BASE_API YkBool operator!=(const YkSize2D& szSrc,const YkSize2D& szTag);	
	
	//! \brief 		重载 +=
	YkSize2D& operator+=(const YkSize2D& sz);
	
	//! \brief 		重载 -=
	YkSize2D& operator-=(const YkSize2D& sz);	
		
	//! \brief 		重载 -
	YkSize2D operator-() const;	
	
	//! \brief 		重载 +
	friend BASE_API YkSize2D operator+(const YkSize2D& szSrc,const YkSize2D& szTag);
	
	//! \brief 		重载 -
	friend BASE_API YkSize2D operator-(const YkSize2D& szSrc,const YkSize2D& szTag);	
};

}

#endif

