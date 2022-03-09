/*
 *
 * YkPoint.h
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

#if !defined(AFX_YKPOINT_H__2388B98A_6C57_4E01_B775_57C22DD6CEC6__INCLUDED_)
#define AFX_YKPOINT_H__2388B98A_6C57_4E01_B775_57C22DD6CEC6__INCLUDED_

#pragma once

#include "Base/YkSize.h"
#include "Base/YkArray.h"

namespace Yk {

class YkSize;
//!  \brief 二维坐标点，YkInt型，常用来表达屏幕坐标点或设备坐标点
class BASE_API YkPoint  
{
public:
	//! \brief 点的横坐标
	YkInt x;
	//! \brief 点的纵坐标
	YkInt y;
public:

	//! \brief 		缺省构造函数
	YkPoint();
	
	//! \brief 		默认析构函数
	~YkPoint();

	//! \brief 		带参构造函数	
	//! \param 		nX			横坐标
	//! \param 		nY			纵坐标		
	YkPoint(YkInt nX,YkInt nY);	

	//! \brief 		拷贝构造函数	
	//! \param 		sz					
	YkPoint(const YkSize& sz);

	//! \brief 		绕点旋转
	//! \param 		pntAnchor	中心点
	//! \param 		dRadian		顺时针旋转角度(弧度)	
	void Rotate(const YkPoint& pntAnchor, double dRadian);

	//! \brief 		绕点旋转
	//! \param 		pntAnchor	中心点
	//! \param 		dCos		余弦值
	//! \param 		dSin		正弦值	
	void Rotate(const YkPoint& pntAnchor, double dCos, double dSin);

	//! \brief 		点偏移
	//! \param 		szValue		偏移量	
	void Offset(const YkSize &szValue);

	//! \brief 		点偏移
	//! \param 		nOffsetX	横坐标偏移
	//! \param 		nOffsetY	纵坐标偏移	
	void Offset(YkInt nOffsetX, YkInt nOffsetY);

	//! \brief 		重载 == 
	//! \return 	返回pntStart与pntEnd的比较结果，相等返回true，不等返回false。
	friend BASE_API YkBool operator==(const YkPoint& pntStart,const YkPoint& pntEnd);

	//! \brief 		重载 != 
	//! \return 	返回pntStart与pntEnd的比较结果，不等返回true，相等返回false。	
	friend BASE_API YkBool operator!=(const YkPoint& pntStart,const YkPoint& pntEnd);			

	//! \brief 		重载 = 	
	YkPoint& operator=(const YkSize& sz);
	
	//! \brief 		重载 = 	
	YkPoint& operator=(const YkPoint& pnt);

	//! \brief 		重载 += 
	YkPoint& operator+=(const YkPoint& pnt);

	//! \brief 		重载 += 	
	YkPoint& operator+=(const YkSize& sz);

	//! \brief 		重载 -= 	
	YkPoint& operator-=(const YkPoint& pnt);

	//! \brief 		重载 -= 
	YkPoint& operator-=(const YkSize& sz);	

	//! \brief 		重载 -(取相反数) 
	//! \return 	YkPoint
	//! \remarks 	
	YkPoint operator-() const;		

	//! \brief 重载 <
	bool operator<(const YkPoint& pt) const;

	//! \brief 		重载 +
	friend BASE_API YkPoint operator+(const YkPoint& pntStart,const YkPoint& pntEnd);

	//! \brief 		重载 +	
	friend BASE_API YkPoint operator+(const YkPoint& pnt,const YkSize& sz);
	
	//! \brief 		重载 +	
	friend BASE_API YkPoint operator+(const YkSize& sz,const YkPoint& pnt);	
	
	//! \brief 		重载 -	
	friend BASE_API YkPoint operator-(const YkPoint& pntStart,const YkPoint& pntEnd);

	//! \brief 		重载 -
	friend BASE_API YkPoint operator-(const YkPoint& pnt,const YkSize& sz);

	//! \brief 		重载 -
	friend BASE_API YkPoint operator-(const YkSize& sz,const YkPoint& pnt);
	
	
};

typedef YkArray<YkPoint> YkPoints;
	
}

#endif

