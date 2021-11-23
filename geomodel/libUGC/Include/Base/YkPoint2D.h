/*
 *
 * YkPoint2D.h
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

#if !defined(AFX_YKPOINT2D_H__8AEE7D11_E2F3_447D_AE4B_CC31AA76F2E0__INCLUDED_)
#define AFX_YKPOINT2D_H__8AEE7D11_E2F3_447D_AE4B_CC31AA76F2E0__INCLUDED_

#pragma once

#include "Base/YkSize2D.h"
#include "Base/YkPoint.h"
#include "Base/YkArray.h"

namespace Yk {
//!  \brief 二维坐标点，YkDouble型，一般用来描述地理坐标的点
class BASE_API YkPoint2D  
{
public:
	//! \brief 点的横坐标
	YkDouble x;
	//! \brief 点的纵坐标
	YkDouble y;
public:
		
    //! \brief  	缺省构造函数
	YkPoint2D();
		
	//! \brief  	默认析造函数
	~YkPoint2D();
		
	//! \brief 		带参构造函数	
	//! \param 		dX			横坐标
	//! \param 		dY			纵坐标
   	YkPoint2D(YkDouble dX,YkDouble dY);
		
	//! \brief 		拷贝构造函数	
	//! \param 		sz			
	YkPoint2D(const YkSize2D& sz);
		
	//! \brief 		拷贝构造函数	
	//! \param 		sz			
	YkPoint2D(const YkPoint& pnt);
		
	//! \brief 		绕点旋转
	//! \param 		pntAnchor	中心点
	//! \param 		dRadian		旋转角度(弧度)	
	void Rotate(const YkPoint2D& pntAnchor, YkDouble dRadian);
       
	//! \brief 		绕点旋转
	//! \param 		pntAnchor	中心点
	//! \param 		dCos		余弦值
	//! \param 		dSin		正弦值
	void Rotate(const YkPoint2D& pntAnchor, YkDouble dCos, YkDouble dSin);
	
	//! \brief 		点偏移
	//! \return 	void
	//! \param 		szValue		偏移量 
	void Offset(const YkSize2D &szValue);

	//! \brief 		点偏移
	//! \param 		nOffsetX	横坐标偏移
	//! \param 		nOffsetY	纵坐标偏移
	void Offset(YkDouble dOffsetX, YkDouble dOffsetY);
	    
	//! \brief 		判断和当前点是否相等
	//! \param 		pntAnchor	要判断的点
	//! \param 		dTolerance	判断容限
	YkBool EQ(const YkPoint2D& pntAnchor, YkDouble dTolerance)const;
		
	//! \brief 		重载 == 
	//! \return 	返回是否相等
	friend BASE_API YkBool operator==(const YkPoint2D& pntStart,const YkPoint2D& pntEnd);
	
	//! \brief 		重载 !=		
	//! \return 	返回是否不相等
	friend BASE_API YkBool operator!=(const YkPoint2D& pntStart,const YkPoint2D& pntEnd);			
		
	//! \brief 		重载 = 	
	YkPoint2D& operator=(const YkSize2D& sz);
	
	//! \brief 		重载 = 	
	YkPoint2D& operator=(const YkPoint2D& pnt2);
		
	//! \brief 		重载 = 
	YkPoint2D& operator=(const YkPoint& pnt);
		
	//! \brief 		重载 += 
	YkPoint2D& operator+=(const YkPoint2D& pnt);
		
	//! \brief 		重载 += 
	YkPoint2D& operator+=(const YkSize2D& sz);
		
	//! \brief 		重载 -= 
	YkPoint2D& operator-=(const YkPoint2D& pnt);
        
	//! \brief 		重载 -= 
	YkPoint2D& operator-=(const YkSize2D& sz);	
		
	//! \brief 		重载 - 负号 
	YkPoint2D operator-() const;		
		
    //! \brief 		重载 + 
	friend BASE_API YkPoint2D operator+(const YkPoint2D& pntStart,const YkPoint2D& pntEnd);
		
	//! \brief 		重载 + 
	friend BASE_API YkPoint2D operator+(const YkPoint2D& pntStart,const YkSize2D& pntEnd);
		
    //! \brief 		重载 + 
	friend BASE_API YkPoint2D operator+(const YkSize2D& sz,const YkPoint2D& pnt);	
		
	//! \brief 		重载 - 
	friend BASE_API YkPoint2D operator-(const YkPoint2D& pntStart,const YkPoint2D& pntEnd);
		
    //! \brief 		重载 - 
	friend BASE_API YkPoint2D operator-(const YkPoint2D& pntStart,const YkSize2D& sz);
		
	//! \brief 		重载 - 
	friend BASE_API YkPoint2D operator-(const YkSize2D& sz,const YkPoint2D& pnt);	
				
	//! \brief 		重载 < 
	friend BASE_API YkBool operator<(const YkPoint2D& pntStart,const YkPoint2D& pntEnd);
		
};
	
typedef YkArray<YkPoint2D> YkPoint2Ds;
	
}

#endif

