/*
 *
 * YkRect2D.h
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

#if !defined(AFX_YKRECT2D_H__A3169E94_5510_4729_B37C_17CD30FC987B__INCLUDED_)
#define AFX_YKRECT2D_H__A3169E94_5510_4729_B37C_17CD30FC987B__INCLUDED_

#pragma once

#include "YkPoint2D.h"

namespace Yk {
//!  \brief 矩形对象类，存储了矩形的左上角点、右下角点的(x,y)坐标，坐标值为YkDouble类型。
class BASE_API YkRect2D  
{
public:
	//! \brief 矩形左边界
	YkDouble left;
	//! \brief 矩形上边界
	YkDouble top;
	//! \brief 矩形右边界。left<right
	YkDouble right;
	//! \brief 矩形下边界。bottom < top
	YkDouble bottom;

public:
	//! \brief 构造函数一个二维矩形对象
	YkRect2D();

	//! \brief 拷贝构造函数
	YkRect2D(const YkRect2D& rc);

	//! \brief 		缺省析造函数
	~YkRect2D();
	
	//! \brief 		构造函数一个二维矩形对象
	//! \param 		nLeft	矩形左上角点的x轴坐标
	//! \param 		nTop	矩形左上角点的y轴坐标
	//! \param 		nRight	矩形右下角点的x轴坐标
	//! \param 		nBottom	矩形右下角点的y轴坐标
	//! \remarks 	请确保nLeft<nRight,nTop<nBottom
	YkRect2D(YkDouble dLeft,YkDouble dTop,YkDouble dRight,YkDouble dBottom);

	//! \brief 		构造函数一个二维矩形对象
	//! \param 		pnt	左下角坐标
	//! \param 		sz	矩形的大小
	YkRect2D(const YkPoint2D& pnt,const YkSize2D& sz);

	//! \brief 		构造函数一个二维矩形对象
	//! \param 		pntTL	左上角坐标
	//! \param 		pntBR   右下角坐标
	YkRect2D(const YkPoint2D& pntTL,const YkPoint2D& pntBR);	

	//! \brief 		重载 =
    YkRect2D& operator=(const YkRect2D& rc);

	//! \brief 		重载 ==
	friend BASE_API YkBool operator==(const YkRect2D& rcStart,const YkRect2D& rcEnd);

	//! \brief 		重载 !=
	friend BASE_API YkBool operator!=(const YkRect2D& rcStart,const YkRect2D& rcEnd);
	
	
	//! \brief 		设置矩形参数
	//! \param 		dLeft	矩形左上角点的x轴坐标
	//! \param 		dTop	矩形左上角点的y轴坐标
	//! \param 		dRight	矩形右下角点的x轴坐标
	//! \param 		dBottom	矩形右下角点的y轴坐标
	void SetRect(YkDouble dLeft,YkDouble dTop,YkDouble dRight,YkDouble dBottom);
	
	//! \brief 		设置矩形参数
	//! \param 		pntTL	左上角坐标
	//! \param 		pntBR	右下角坐标
	void SetRect(const YkPoint2D& pntTL,const YkPoint2D& pntBR);

	//! \brief 		将矩形置为空
	//! \remarks 	左上角、右下角点坐标都设为0
	void SetEmpty();

	//! \brief 		判断矩形是否为空或是否合法
	//! \return 	矩形为空返回true，矩形不为空返回false
	//! \remarks 	如果符合left=right,top=bottom规则，返回true，认为矩形为空。
	YkBool IsEmpty() const;

	//! \brief 		矩形是否为未初始化
	//! \return 	如果符合left==right==top==bottom==0规则，则返回true，否则，返回false
	YkBool IsNull() const;

	//! \brief 		矩形的宽度
	//! \return 	返回right-left的值(整型)
	YkDouble Width() const;

	//! \brief 		矩形的高度
	//! \return 	返回top-bottom的值(整型)
	YkDouble  Height() const;
	
	//! \brief 		获取矩形的左上角点
	YkPoint2D& TopLeft();

	//! \brief 		获取矩形的左上角点
	//! \remarks 	如果矩形对象为const类型，则将调用本接口
	const YkPoint2D& TopLeft() const;	

	
	//! \brief 		获取矩形右上角坐标点
	YkPoint2D TopRight() const;
	
	//! \brief 		获取矩形右下角坐标点
	YkPoint2D& BottomRight();	
	
	//! \brief 		获取矩形右下角坐标点
	//! \remarks 	如果矩形对象为const类型，则将调用本接口
	const YkPoint2D& BottomRight() const;

	//! \brief 		获取矩形左下角坐标点
	YkPoint2D BottomLeft() const;
	
	
	//! \brief 		获取矩形的中心点
	//! \remarks 	矩形对角线的交点
	YkPoint2D CenterPoint() const;

	//! \brief 		获取矩形的大小
	YkSize2D Size() const;

	
	//! \brief 		交换left与right的值
	void SwapLeftRight();

	//! \brief 		交换top与bottom的值	
	void SwapTopBottom();	
	
	//! \brief 		判断点是否在矩形中
	//! \param 		pnt	坐标点
	//! \return		若点在矩形中或在矩形边界上，则返回true，否则返回false。
	YkBool PtInRect(const YkPoint2D& pnt) const;

	//! \brief 		判断当前矩形是否包含矩形rc
	//! \param 		rc 矩形
	//! \return		若当前矩形包含矩形rc，则返回ture，否则，返回false。
	//! \image html Contains.png
	YkBool Contains(const YkRect2D& rc) const;

	//! \brief 		判断当前矩形是否被矩形rc包含
	//! \param 		rc	矩形
	//! \return		若当前矩形被矩形rc包含，则返回ture，否则，返回false。
	YkBool Withins(const YkRect2D& rc) const;

	
	//! \brief 		判断当前矩形是否与矩形rc相交
	//! \param 		rc	矩形
	//! \returen	相交返回true，否则，返回false。
	YkBool IsIntersect(const YkRect2D& rc) const;
	
	//! \brief 		规范矩形，确保left<=right,bottom<=top	
	void Normalize();
	
	//! \brief 		对矩形进行偏移
	//! \param 		dX	横坐标偏移量
	//! \param 		dY	纵坐标偏移量
	void Offset(YkDouble dX,YkDouble dY);

	//! \brief 		对矩形进行偏移
	//! \param 		sz	横纵坐标偏移量
	void Offset(const YkSize2D& sz);

	//! \brief 		对矩形进行偏移
	//! \param 		pnt	横纵坐标偏移量
	void Offset(const YkPoint2D& pnt);
	
	//! \brief 		对矩形进行放大
	//! \param 		nMargin	横纵放大比例 	
	void Inflate(YkDouble dMargin);

	//! \brief 		对矩形进行放大
	//! \param 		dHormargin	横坐标放大比例
	//! \param 		dVermargin	总坐标放大比例	
	void Inflate(YkDouble dHorMargin,YkDouble dVerMargin);
	
	//! \brief 		对矩形进行放大
	//! \param 		nLeftMargin	左上角点沿x轴向左的偏移量
	//! \param 		nTopMargin	左上角点沿y轴向上的偏移量
	//! \param 		nRightMargin    右下角点沿x轴向右的偏移量
	//! \param 		nBottomMargin	右下角点沿y轴向下的偏移量
	void Inflate(YkDouble nLeftMargin,YkDouble nTopMargin,YkDouble nRightMargin,YkDouble nBottomMargin);
	
	//! \brief 		对矩形进行缩小
	//! \param 		dMargin	横纵缩小比例	
	void Deflate(YkDouble dMargin);
	
	//! \brief 		对矩形进行缩小
	//! \param 		dHorMargin	横坐标缩小比例
	//! \param 		dVerMargin	总坐标缩小比例
	void Deflate(YkDouble dHorMargin,YkDouble dVerMargin);

	
	//! \brief 		对矩形进行缩小
	//! \param 		dLeftMargin	左上角点沿x轴向右的偏移量
	//! \param 		dTopMargin	左上角点沿y轴向下的偏移量
	//! \param 		dRightMargin 右下角点沿x轴向左的偏移量	
	//! \param 		dBottomMargin 右下角点沿y轴向上的偏移量	
	void Deflate(YkDouble dLeftMargin,YkDouble dTopMargin,YkDouble dRightMargin,YkDouble dBottomMargin);
	
	//! \brief 		矩形合并
	//! \param 		pnt	合并的点
	//! \image html union.png
	void Union(const YkPoint2D& pnt);
	void UnionPt(const YkPoint2D& pnt);
	
	//! \brief 		矩形合并
	//! \param 		rc 用来与原矩形合并的矩形rc
	//! \image html union2.png
	void Union(const YkRect2D& rc);	

	//! \brief 矩形合并
	//! \remark 检查参数和自身合法性
	//! \return 是否成功
	void UnionRect(const YkRect2D& rc);
	
	//! \brief 		矩形求交
	//! \param 		rc 用来与原矩形求交的矩形rc
	//! \param		当前矩形为与rc求交的结果矩形
	//! \image html Intersect2.png
	YkBool IntersectRect(const YkRect2D& rc);
	
	//! \brief 		矩形求交
	//! \param 		rc1 用来求交的矩形	
	//! \param 		rc2	用来求交的矩形
	//! \remarks	当前矩形为rc1，与rc2求交后的结构矩形。
	YkBool IntersectRect(const YkRect2D& rc1,const YkRect2D& rc2);
	
	//! \brief 		当前矩形与旋转之后的矩形求并
	//! \param 		pntOrg 中心点
	//! \param 		dbAngle 旋转角度
	void UnionRotate(const YkPoint2D &pntOrg, YkDouble dbAngle);

	void Rotate(const YkPoint2D &pntOrg, YkDouble dbAngle);
	
	//! \brief 判断YkRect2D对象是否有效
	YkBool IsValid() const;
	
	void InflateRect(YkDouble x, YkDouble y);
	
};

}

#endif

