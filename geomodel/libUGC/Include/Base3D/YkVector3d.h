/*
 *
 * YkVector3d.h
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

#if !defined(AFX_YKVECTOR3D_H__07E8BCDA_D3A9_431B_A917_E7A1D7351969__INCLUDED_)
#define AFX_YKVECTOR3D_H__07E8BCDA_D3A9_431B_A917_E7A1D7351969__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"

namespace Yk {
//! \brief 三维向量类。
class BASE3D_API YkVector3d  
{
public:
	//! \brief 构造函数
	YkVector3d();

	//! \brief 构造函数
	YkVector3d(YkDouble fX, YkDouble fY, YkDouble fZ);

	//! \brief 析构函数
	virtual ~YkVector3d();	

	//! \brief 进行规格化
	//YkVector3d& Normalize();

	//! \brief 赋值函数
	YkVector3d& operator = (const YkVector3d& vec);

	//! \brief 重载的等值判读运算符，判断两个向量是否相等
	YkBool operator == (const YkVector3d& vec) const;
	
	//! \brief 重载的不等值判读运算符，判断两个向量是否不相等
	YkBool operator != (const YkVector3d& vec) const;

	//! \brief 重载的大于判读运算符，判断该向量否大于一个向量
	//! \remarks 如果x，y，z都大于一个向量的x，y，z分量，则返回TRUE。
	YkBool operator > (const YkVector3d& vec) const;

	//! \brief 重载的小于判读运算符，判断该向量否小于一个向量
	//! \remarks 如果x，y，z都小于一个向量的x，y，z分量，则返回TRUE。
	YkBool operator < (const YkVector3d& vec)const;	
	
	//! \brief 重载的加法运算符，求两个向量之和
	YkVector3d operator + (const YkVector3d& vec) const;

	//! \brief 重载的减法运算符，求两个向量之差
	YkVector3d operator - (const YkVector3d& vec) const;
	
	//! \brief 重载的乘法运算符，求乘以一个数的结果
	YkVector3d operator * (const YkDouble fScale) const;

	//! \brief 重载的乘法运算符，一个数乘以向量的结果
	friend BASE3D_API YkVector3d operator * ( const YkDouble fScalar, const YkVector3d& rkVector );

	//! \brief 重载的除法运算符，求除以一个数的结果
	YkVector3d operator / (const YkDouble fScale) const;

	//! \brief 重载的取值运算符，取索引i处的分量
	YkDouble operator [] (const unsigned i) const;

	//! \brief 重载的取值运算符，取索引i处的分量的引用
	YkDouble& operator [] (const unsigned i);
	
	//! \brief 重载的取反运算符，对向量求反
	//! \remarks 对向量求反就是对每个分量求反
	YkVector3d operator - () const;

	//! \brief 重载的+=运算符，求该向量+=一个向量的结果
	YkVector3d& operator += (const YkVector3d& vec);
	
	//! \brief 重载的-=运算符，求该向量-=一个向量的结果
	YkVector3d& operator -= (const YkVector3d& vec);
	
	//! \brief 重载的*=运算符，求该向量*=一个数的结果
	YkVector3d& operator *= (const YkDouble fScale);
	
	//! \brief 重载的/=运算符，求该向量/=一个数的结果
	YkVector3d& operator /= (const YkDouble fScale);

	//! \brief 取两个向量每个分类的最小值形成一个新的向量
	void MakeFloor( const YkVector3d& cmp );

	//! \brief 取两个向量每个分类的最大值形成一个新的向量
	void MakeCeil( const YkVector3d& cmp );
	
	//! \brief 求向量的长度
	YkDouble Length() const;



	//! \brief 该乘法支持D3D矩阵运算
	//! \brief 本系统可以直接使用
	YkVector3d MultiplyMatrix(const YkMatrix4d& m) const;

	//! \brief 求向量乘以一个4*4的矩阵
	//! \param vecPos 向量[in][out]。
	//! \param m 4×4矩阵[in]。
	static void MultiplyMatrix(YkVector3d& vecPos, YkMatrix4d& m);	

	//! \brief 零向量
	static const YkVector3d ZERO;
	//! \brief x单位向量(1,0,0)
	static const YkVector3d UNIT_X;
	//! \brief y单位向量(0,1,0)
    static const YkVector3d UNIT_Y;
	//! \brief z单位向量(0,0,1)
    static const YkVector3d UNIT_Z;
	//! \brief -x单位向量(-1,0,0)
    static const YkVector3d NEGATIVE_UNIT_X;
	//! \brief -y单位向量(0,-1,0)
    static const YkVector3d NEGATIVE_UNIT_Y;
	//! \brief -z单位向量(0,0,-1)
    static const YkVector3d NEGATIVE_UNIT_Z;
	//! \brief 全单位向量(1,1,1)
    static const YkVector3d UNIT_SCALE;
public:
	//! \brief 向量的三个分量
	YkDouble x, y, z;
};
}
#endif

