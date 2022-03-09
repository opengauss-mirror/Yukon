/*
 *
 * YkMatrix4d.h
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

#if !defined(AFX_YKMATRIX4D_H__67A327BB_7D71_4996_8DBC_4884AB212BCD__INCLUDED_)
#define AFX_YKMATRIX4D_H__67A327BB_7D71_4996_8DBC_4884AB212BCD__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"

namespace Yk {
//! \brief   4X4矩阵类
//! \remarks 维护4X4矩阵
class BASE3D_API YkMatrix4d  
{
public:
	//! \brief 缺省构造函数。
	YkMatrix4d();

	//! \brief 用16个浮点数构造矩阵类的构造函数。
	//! \param m00 [in]。
	//! \param m01 [in]。
	//! \param m02 [in]。
	//! \param m03 [in]。
	//! \param m10 [in]。
	//! \param m11 [in]。
	//! \param m12 [in]。
	//! \param m13 [in]。
	//! \param m20 [in]。
	//! \param m21 [in]。
	//! \param m22 [in]。
	//! \param m23 [in]。
	//! \param m30 [in]。
	//! \param m31 [in]。
	//! \param m32 [in]。
	//! \param m33 [in]。
	YkMatrix4d(
            YkDouble m00, YkDouble m01, YkDouble m02, YkDouble m03,
            YkDouble m10, YkDouble m11, YkDouble m12, YkDouble m13,
            YkDouble m20, YkDouble m21, YkDouble m22, YkDouble m23,
            YkDouble m30, YkDouble m31, YkDouble m32, YkDouble m33);

	//! \brief 析构函数。
	virtual ~YkMatrix4d();

	//! \brief 重载*，矩阵与数值相乘。
	//! \param scalar 数值[in]。
	//! \return 运算完成后4x4矩阵。
	YkMatrix4d operator*(YkDouble scalar);

	//! \brief 重载*，完成矩阵与矩阵相乘。
	//! \param &m2 乘数矩阵[in]。
	//! \return 运算完成后矩阵。
	YkMatrix4d operator * ( const YkMatrix4d &m2 ) const;

	//! \brief 重载=，完成矩阵的赋值
	//! \brief &m2,赋值矩阵	
	YkMatrix4d& operator = (const YkMatrix4d &m2);

	//! \brief 重载*，矩阵与三维向量相乘。
	//! \param &v 三维向量[in]。
	//! \return 三维向量。
	//! \remarks 从矩阵中取出最上面3X3矩阵与三维向量进行相乘。
	YkVector3d operator * ( const YkVector3d &v ) const;

	//! \brief 矩阵相乘。
	//! \param &m2 另外一个4X4矩阵[in]。
	//! \return 完成运算后4X4矩阵。
	YkMatrix4d Concatenate(const YkMatrix4d &m2) const;
    
	//! \brief 重载[]，取出对应行。
	//! \param iRow 行号，有效范围为零到三[in]。
	//! \return 矩阵指定一行数据，返回行数据指针。
	YkDouble* operator[] (YkUint iRow);

	//! \brief 判断两个矩阵是否相等。
	//! \param &m2 另外一个矩阵[in]。
	//! \return 若相等返回为TRUE，若不相等则返回为FALSE。
	YkBool operator == (const YkMatrix4d &m2) const;

	//! \brief 判断两个矩阵是否不相等。
	//! \param &m2 另外一个矩阵[in]。
	//! \return 若不相等返回为TRUE，否则返回FALSE。
	YkBool operator != (const YkMatrix4d &m2) const;
 
	//! \brief 取出只读矩阵行数据。
	//! \param iRow 行号，有效范围为零到三[in]。
	//! \return 行数据。
	const YkDouble *const operator [] ( YkUint iRow ) const;

    //! \brief 取出矩阵某行某列的值。
	//! \param iRow 行数[in]。
	//! \param iCol 列数[in]
	YkDouble operator () (YkUint iRow, YkUint iCol);

	//! \brief 矩阵平移。
	//! \param x X轴移动大小[in]。
	//! \param y Y轴移动大小[in]。
	//! \param z Z轴移动大小[in]。
	//! \return 平移后矩阵。
	static YkMatrix4d Translation(YkDouble x, YkDouble y, YkDouble z);

	//! \brief 构建一个缩放矩阵
	//! param x X轴缩放大小[in]。
	//! param y Y轴缩放大小[in]。
	//! param z Z轴缩放大小[in]。
	//! return 缩放矩阵。
	static YkMatrix4d Scaling(YkDouble x, YkDouble y, YkDouble z);

	//! \brief 旋转。
	//! \param rotationX 绕X轴旋转角度[in]。
	//! \param rotationY 绕Y轴旋转角度[in]。
	//! \param rotationZ 绕Z轴旋转角度[in]。
	//! \return 旋转后矩阵。
	static YkMatrix4d RotationXYZ(YkDouble rotationX, YkDouble rotationY, YkDouble rotationZ);
	
	//! \brief 绕X轴旋转。
	//! \param angle 绕X轴旋转角度[in]。
	//! \return 旋转后矩阵。		
	static YkMatrix4d RotationX(YkDouble angle);

	//! \brief 绕Y轴旋转。
	//! \param angle 绕Y轴旋转角度[in]。
	//! \return 旋转后矩阵。
	static YkMatrix4d RotationY(YkDouble angle);

	//! \brief 绕Z轴旋转。
	//! \param angle [in]。
	//! \return 旋转后矩阵。		
	static YkMatrix4d RotationZ(YkDouble angle);

	//! \brief 计算矩阵的伴随矩阵。
	//! \return 矩阵的伴随矩阵。
	//! \remarks 。	
	YkMatrix4d Adjoint() const;

	//! \brief 计算矩阵行列式值。
	//! \return 行列式值。。
	YkDouble Determinant() const;

	//! \brief 计算机矩阵逆矩阵。
	//! \return 矩阵逆矩阵。
	YkMatrix4d Invert() const;
		
	//! \brief 计算4x4中任意3x3子矩阵行列式。
	//! \param m 4x4矩阵[in]。
	//! \param r0 第一行行号[in]。
	//! \param r1 第二行行号[in]。
	//! \param r2 第三行行号[in]。
	//! \param c0 第一列列号[in]。
	//! \param c1 第二列列号[in]。
	//! \param c2 第三列列号[in]。
	YkDouble MINOR(const YkMatrix4d& m, const YkInt r0, const YkInt r1, const YkInt r2, 
		const YkInt c0, const YkInt c1, const YkInt c2) const;

	//! \brief 4X4 单位矩阵
	//! \remarks 对角线元素为1，其他为0
	static const YkMatrix4d IDENTITY;

	//! \brief 获取矩阵的指针
	YkDouble* GetPointer();

protected:
	//! \brief 矩阵中数据
	//! \remarks 矩阵中数据保存在该变量中。
	YkDouble m[4][4];
};

}
#endif

