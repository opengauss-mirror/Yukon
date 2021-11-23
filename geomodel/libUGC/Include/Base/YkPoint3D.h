/*
 *
 * YkPoint3D.h
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

#if !defined(AFX_YKPOINT3D_H__4F7FE3AF_CB0E_4831_98B1_1512914807A3__INCLUDED_)
#define AFX_YKPOINT3D_H__4F7FE3AF_CB0E_4831_98B1_1512914807A3__INCLUDED_

#pragma once

#include "Base/YkArray.h"

namespace Yk {
//!  \brief 三维坐标点，YkDouble型，一般用来描述地理坐标的三维点。
class BASE_API YkPoint3D  
{
public:
    //! \brief 点的x轴坐标
	YkDouble x;
	//! \brief 点的y轴坐标
	YkDouble y;
	//! \brief 点的z轴坐标
	YkDouble z;
public:
	//! \brief  	默认构造函数
	YkPoint3D();
	//! \brief  	缺省析构函数
	~YkPoint3D();

	//! \brief  带参数构造函数
	YkPoint3D(YkDouble xx,YkDouble yy,YkDouble zz);	

public:	
	//! \brief 		重载 = 
	YkPoint3D& operator=(const YkPoint3D& s);
	//! \brief 		重载 += 
	YkPoint3D& operator+=(const YkPoint3D& p);
	//! \brief 		重载 -= 
	YkPoint3D& operator-=(const YkPoint3D& p);
	//! \brief 		重载 *= 
	YkPoint3D& operator*=(YkDouble c);
	//! \brief 		重载 /= 
	YkPoint3D& operator/=(YkDouble c);
	//! \brief 		重载 - 负号
	YkPoint3D operator-() const;
	//! \brief 		重载 == 
	friend BASE_API YkBool operator==(const YkPoint3D& p,const YkPoint3D& q);
	//! \brief 		重载 !=
	friend BASE_API YkBool operator!=(const YkPoint3D& p,const YkPoint3D& q);			
		
};
	
typedef YkArray<YkPoint3D> YkPoint3Ds;
}

#endif

