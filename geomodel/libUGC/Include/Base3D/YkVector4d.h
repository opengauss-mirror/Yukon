/*
 *
 * YkVector4d.h
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

#if !defined(AFX_YKVECTOR4R_H__92CEC4D7_6A04_475B_A0BB_2C457812FD95__INCLUDED_)
#define AFX_YKVECTOR4R_H__92CEC4D7_6A04_475B_A0BB_2C457812FD95__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Base3D/YkVector3d.h"

namespace Yk {
//! \brief 三维4阶向量类。
class BASE3D_API YkVector4d  
{
public:
#pragma warning(disable: 4201)   //使用了非标准扩展 : 无名称的结构/联合
	//! \brief 公用体，实现[]操作
    union 
	{
        struct 
		{
			//! \brief 三维4阶向量类成员。
            YkDouble x, y, z, w;
        };
        YkDouble val[4];
    };
#pragma warning(default: 4201)

public:
	//! \brief 构造函数
    YkVector4d();
	//! \brief 构造函数，由四个值构建
    YkVector4d( const YkDouble fX, const YkDouble fY, const YkDouble fZ, const YkDouble fW );
    //! \brief 构造函数,由数组构建
	YkVector4d( const YkDouble afCoordinate[4] );
    //! \brief 构造函数,由数组构建
	YkVector4d( const int afCoordinate[4] );
    //! \brief 构造函数,由数组构建
	YkVector4d( YkDouble* const r );
	//! \brief 构造函数,每个分量赋同样的值scaler
    YkVector4d( const YkDouble scaler );
	//! \brief 构造函数，由3×3的矩阵构造4×4的矩阵
    YkVector4d(const YkVector3d& rhs);
	//! \brief 拷贝构造函数
    YkVector4d( const YkVector4d& rkVector );
	//! \brief 重载的取值运算符，取索引i处的分量
	YkDouble operator [] ( const size_t i ) const;
	//! \brief 重载的取值运算符，取索引i处的分量的引用
	YkDouble& operator [] ( const size_t i );
	//! \brief 重载的赋值运算符，取索引i处的分量的引用
    YkVector4d& operator = ( const YkVector4d& rkVector );
	//! \brief 重载的赋值运算符，每个分量赋予同样的值fScalar
	YkVector4d& operator = ( const YkDouble fScalar);
	//! \brief 重载的等值判读运算符，判断两个向量是否相等
    bool operator == ( const YkVector4d& rkVector ) const;
	//! \brief 重载的不等值判读运算符，判断两个向量是否不相等
    bool operator != ( const YkVector4d& rkVector ) const;
	//! \brief 赋值函数，由3×3的矩阵构造4×4的矩阵
    YkVector4d& operator = (const YkVector3d& rhs);
	//! \brief 重载的加法运算符，求两个向量之和
    YkVector4d operator + ( const YkVector4d& rkVector ) const;
	//! \brief 重载的减法运算符，求两个向量之差
    YkVector4d operator - ( const YkVector4d& rkVector ) const;
	//! \brief 重载的乘法运算符，求乘以一个数的结果
    YkVector4d operator * ( const YkDouble fScalar ) const;
	//! \brief 重载的乘法运算符，求乘以一个向量的结果
    YkVector4d operator * ( const YkVector4d& rhs) const;
	//! \brief 重载的乘法运算符，求乘以一个矩阵的结果
	YkVector4d operator * (const YkMatrix4d& m);
	//! \brief 重载的除法运算符，求除以一个数的结果
    YkVector4d operator / ( const YkDouble fScalar ) const;
	//! \brief 重载的除法运算符，求除以一个向量的结果
    YkVector4d operator / ( const YkVector4d& rhs) const;
	//! \brief 重载的正算符，返回改向量
    const YkVector4d& operator + () const;
	//! \brief 重载的取反运算符，对向量求反
	//! \remarks 对向量求反就是对每个分量求反
    YkVector4d operator - () const;
	//! \brief 重载的乘法运算符，求向量乘以一个数的结果
    friend YkVector4d operator * ( const YkDouble fScalar, const YkVector4d& rkVector );
    //! \brief 重载的除法运算符，求向量除以一个数的结果
	friend YkVector4d operator / ( const YkDouble fScalar, const YkVector4d& rkVector );
	//! \brief 重载的加法运算符，求向量和一个数之和
	//! \remarks 对向量每个分量加上一个值
    friend YkVector4d operator + (const YkVector4d& lhs, const YkDouble rhs);
	//! \brief 重载的加法运算符，求向量和一个数之和
	//! \remarks 对向量每个分量加上一个值
    friend YkVector4d operator + (const YkDouble lhs, const YkVector4d& rhs);
	//! \brief 重载的减法运算符，求向量和一个数之差
	//! \remarks 对向量每个分量减去一个值
    friend YkVector4d operator - (const YkVector4d& lhs, YkDouble rhs);  
	//! \brief 重载的减法运算符，求向量和一个数之差
	//! \remarks 对向量每个分量减去一个值
	friend YkVector4d operator - (const YkDouble lhs, const YkVector4d& rhs);

	//! \brief 重载的+=运算符，求该向量+=一个向量的结果
    YkVector4d& operator += ( const YkVector4d& rkVector );

	//! \brief 重载的-=运算符，求该向量-=一个向量的结果
    YkVector4d& operator -= ( const YkVector4d& rkVector );

	//! \brief 重载的*=运算符，求该向量*=一个数的结果
    YkVector4d& operator *= ( const YkDouble fScalar );

	//! \brief 重载的+=运算符，求该向量+=一个数的结果
    YkVector4d& operator += ( const YkDouble fScalar );
	
	//! \brief 重载的-=运算符，求该向量-=一个数的结果
    YkVector4d& operator -= ( const YkDouble fScalar );

	//! \brief 重载的*=运算符，求该向量*=一个向量的结果
    YkVector4d& operator *= ( const YkVector4d& rkVector );

	//! \brief 重载的/=运算符，求该向量/=一个数的结果
    YkVector4d& operator /= ( const YkDouble fScalar );

	//! \brief 重载的/=运算符，求该向量/=一个向量的结果
    YkVector4d& operator /= ( const YkVector4d& rkVector );

	//! \brief 零向量 ( 0, 0, 0, 0 )
    static const YkVector4d ZERO;

};
}
#endif

