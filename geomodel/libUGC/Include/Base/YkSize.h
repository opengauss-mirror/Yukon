/*
 *
 * YkSize.h
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

#if !defined(AFX_YKSIZE_H__ED613AC5_ED5E_4B31_846E_3768AF9C4BA6__INCLUDED_)
#define AFX_YKSIZE_H__ED613AC5_ED5E_4B31_846E_3768AF9C4BA6__INCLUDED_

#pragma once

#include "Base/YkDefs.h"

namespace Yk {
//!  \brief 矩形大小类，存储了矩形的长度cx和宽度cy，为YkInt型。
class BASE_API YkSize  
{
public:
	//! \brief 		长度
	YkInt cx;
	//! \brief 		宽度
	YkInt cy;
public:
	
	//! \brief 		缺省构造函数
	YkSize();	

	//! \brief 		缺省析构函数
	~YkSize();	
	
	//! \brief 		带参构造函数
	//! \param 		nX		长度
	//! \param 		nY		宽度
	YkSize(YkInt nX,YkInt nY);
	
	//! \brief 		重载==
	friend BASE_API YkBool operator==(const YkSize& szSrc,const YkSize& szTag);
	
	//! \brief 		重载!=
	friend BASE_API YkBool operator!=(const YkSize& szSrc,const YkSize& szTag);	
	
	//! \brief 		重载 +=
	YkSize& operator+=(const YkSize& sz);
	
	//! \brief 		重载 -=
	YkSize& operator-=(const YkSize& sz);	
	
	//! \brief 		重载 -
	YkSize operator-() const;	
	
	//! \brief 		重载 +
	friend BASE_API YkSize operator+(const YkSize& szSrc,const YkSize& szTab);
	
	//! \brief 		重载 -
	friend BASE_API YkSize operator-(const YkSize& szSrc,const YkSize& szTab);
	
};
	
}

#endif

