/*
 *
 * YkRect.h
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

#if !defined(AFX_YKRECT_H__389F8413_2C28_48F2_A87A_658DA28B5361__INCLUDED_)
#define AFX_YKRECT_H__389F8413_2C28_48F2_A87A_658DA28B5361__INCLUDED_

#pragma once

#include "YkPoint.h"

namespace Yk {
//!  \brief 矩形对象类，存储了矩形的左上角点、右下角点的(x,y)坐标，坐标值为YkInt型。
class BASE_API YkRect  
{
public:
	//! \brief 矩形左上角点的x轴坐标
	YkInt left;
	//! \brief 矩形左上角点的y轴坐标
	YkInt top;
	//! \brief 矩形右下角点的x轴坐标
	YkInt right;
	//! \brief 矩形右下角点的y轴坐标
	YkInt bottom;		

public:
	//! \brief 		缺省构造函数
	YkRect();	

	//! \brief 		缺省解析函数
	~YkRect();	

	//! \brief 		构造函数一个二维矩形对象
	//! \param 		nLeft	矩形左上角点的x轴坐标
	//! \param 		nTop	矩形左上角点的y轴坐标
	//! \param 		nRight	矩形右下角点的x轴坐标
	//! \param 		nBottom	矩形右下角点的y轴坐标
	//! \remarks 	请确保nLeft<nRight,nTop<nBottom
	YkRect(YkInt nLeft,YkInt nTop,YkInt nRight,YkInt nBottom);
	
	//! \brief 		带参构造函数
	//! \param 		pnt	左上角坐标
	//! \param 		sz	矩形的大小	
	YkRect(const YkPoint& pnt,const YkSize& sz);

	//! \brief 		带参构造函数
	//! \param 		pntTL	左上角坐标点
	//! \param 		pntBR   右下角坐标点	
	YkRect(const YkPoint& pntTL,const YkPoint& pntBR);	
 	
	//! \brief 		重载 =
    YkRect& operator=(const YkRect& rc);

	//! \brief 		重载 ==
	friend BASE_API YkBool operator==(const YkRect& rcStart,const YkRect& rcEnd);

	//! \brief 		重载 !=
	friend BASE_API YkBool operator!=(const YkRect& rcStart,const YkRect& rcEnd);
	
	//! \brief 		设置矩形参数
	//! \param 		nLeft	矩形左上角点的x轴坐标
	//! \param 		nTop	矩形左上角点的y轴坐标
	//! \param 		nRight	矩形右下角点的x轴坐标
	//! \param 		nBottom	矩形右下角点的y轴坐标
	void SetRect(YkInt nLeft,YkInt nTop,YkInt nRight,YkInt nBottom);

	//! \brief 		设置矩形参数
	//! \param 		pntTL	左上角坐标
	//! \param 		pntBR	右下角坐标	
	void SetRect(const YkPoint& pntTL,const YkPoint& pntBR);

	//! \brief 		将矩形置为空
	//! \remarks 	左上角、右下角点坐标都设为0
	void SetEmpty();

	//! \brief 		判断矩形是否为空或是否合法
	//! \return 	矩形为空返回true，矩形不为空返回false
	//! \remarks 	如果不符合left<right,top<bottom规则，返回true，认为矩形为空。
	YkBool IsEmpty() const;

	//! \brief 		矩形是否为未初始化
	//! \return 	如果符合left==right==top==bottom==0规则，则返回true，否则，返回false
	YkBool IsNull() const;

	//! \brief 		矩形的宽度
	//! \return 	返回right-left的值(整型)
	YkInt Width() const;

	//! \brief 		矩形的高度
	//! \return 	返回top-bottom的值(整型)
	YkInt  Height() const;

	//! \brief 		获取矩形的左上角点
	YkPoint& TopLeft();

	//! \brief 		获取矩形的左上角点
	//! \remarks 	如果矩形对象为const类型，则将调用本接口
	const YkPoint& TopLeft() const;	

	//! \brief 		获取矩形右下角坐标点
	YkPoint& BottomRight();		

	//! \brief 		获取矩形右下角坐标点
	//! \remarks 	如果矩形对象为const类型，则将调用本接口
	const YkPoint& BottomRight() const;
	
	//! \brief 		获取矩形的中心点
	YkPoint CenterPoint() const;
	
	//! \brief 		获取矩形的大小
	YkSize Size() const;

	//! \brief 		交换left与right的值
	void SwapLeftRight();

	//! \brief 		交换top与bottom的值	
	void SwapTopBottom();	

	//! \brief 		判断点是否在矩形中
	//! \param 		pnt	坐标点
	//! \return		若点在矩形中或在矩形边界上，则返回true，否则返回false。
	YkBool PtInRect(const YkPoint& pnt) const;

	//! \brief 		判断当前矩形是否包含矩形rc
	//! \param 		rc 矩形
	//! \return		若当前矩形包含矩形rc，则返回ture，否则，返回false。
	//! \image html Contains.png
	YkBool Contains(const YkRect& rc) const;

	//! \brief 		判断当前矩形是否被矩形rc包含
	//! \param 		rc	矩形
	//! \return		若当前矩形被矩形rc包含，则返回ture，否则，返回false。
	//! \image html within.png
	YkBool Withins(const YkRect& rc) const;

	//! \brief 		判断当前矩形是否与矩形rc相交
	//! \param 		rc	矩形
	//! \returen	相交返回true，否则，返回false。
	//! \image html Intersect2.png
	YkBool IsIntersect(const YkRect& rc) const;

	//! \brief 		规范矩形，确保left<=right,top<=bottom	
	void Normalize();
	
	//! \brief 		对矩形进行偏移
	//! \param 		nX	横坐标偏移量
	//! \param 		nY	纵坐标偏移量	
	void Offset(YkInt nX,YkInt nY);
	
	//! \brief 		对矩形进行偏移
	//! \param 		sz	横纵坐标偏移量	
	void Offset(const YkSize& sz);
	
	//! \brief 		对矩形进行偏移
	//! \param 		pnt	横纵坐标偏移量	
	void Offset(const YkPoint& p);

	//! \brief 		对矩形进行放大
	//! \param 		nMargin	横纵放大比例 	
	void Inflate(YkInt nMargin);

	//! \brief 		对矩形进行放大
	//! \param 		nHormargin	横坐标放大比例
	//! \param 		nVermargin	总坐标放大比例	
	void Inflate(YkInt nHorMargin,YkInt nVerMargin);

	//! \brief 		对矩形进行放大
	//! \param 		nLeftMargin	左上角点沿x轴向左的偏移量
	//! \param 		nTopMargin	左上角点沿y轴向上的偏移量
	//! \param 		nRightMargin    右下角点沿x轴向右的偏移量
	//! \param 		nBottomMargin	右下角点沿y轴向下的偏移量
	void Inflate(YkInt nLeftMargin,YkInt nTopMargin,YkInt nRightMargin,YkInt nBottomMargin);
	
	//! \brief 		对矩形进行缩小
	//! \param 		nMargin	横纵缩小比例	
	void Deflate(YkInt nMargin);

	//! \brief 		对矩形进行缩小
	//! \param 		nHormargin	横坐标缩小比例
	//! \param 		nVermargin	总坐标缩小比例	
	void Deflate(YkInt nHorMargin,YkInt nVerMargin);

	//! \brief 		对矩形进行缩小
	//! \param 		nLeftMargin	左上角点沿x轴向右的偏移量
	//! \param 		nTopMargin	左上角点沿y轴向下的偏移量
	//! \param 		nRightMargin 右下角点沿x轴向左的偏移量	
	//! \param 		nBottomMargin 右下角点沿y轴向上的偏移量	
	void Deflate(YkInt nLeftMargin,YkInt nTopMargin,YkInt nRightMargin,YkInt nBottomMargin);

	//! \brief 		矩形合并
	//! \param 		pnt	合并的点
	//! \image html union.png
	void Union(const YkPoint& pnt);

	//! \brief 		矩形合并
	//! \param 		rc 用来与原矩形合并的矩形rc	
	//! \image html union2.png
	void Union(const YkRect& rc);	

	//! \brief 		矩形求交
	//! \param 		rc 用来与原矩形求交的矩形rc
	//! \image html Intersect2.png
	void Intersection(const YkRect& rc);

};

}

#endif

