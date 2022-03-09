/*
 *
 * YkMatrix4d.cpp
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

#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkVector3d.h"
#include "Toolkit/YkMarkup.h"

namespace Yk {

const YkMatrix4d YkMatrix4d::IDENTITY(
								  1, 0, 0, 0,
								  0, 1, 0, 0,
								  0, 0, 1, 0,
								  0, 0, 0, 1);

YkMatrix4d::YkMatrix4d()
{
	*this = YkMatrix4d::IDENTITY;
}
YkMatrix4d::YkMatrix4d(
        YkDouble m00, YkDouble m01, YkDouble m02, YkDouble m03,
        YkDouble m10, YkDouble m11, YkDouble m12, YkDouble m13,
        YkDouble m20, YkDouble m21, YkDouble m22, YkDouble m23,
        YkDouble m30, YkDouble m31, YkDouble m32, YkDouble m33)	
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

YkMatrix4d::~YkMatrix4d()
{

}
YkMatrix4d YkMatrix4d::operator*(YkDouble scalar)
{
    return YkMatrix4d(
        scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
        scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
        scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
        scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
}

//! \brief 重载*，完成矩阵与矩阵相乘。
//! \param &m2 乘数矩阵[in]。
//! \return 运算完成后矩阵。
YkMatrix4d YkMatrix4d::operator * ( const YkMatrix4d &m2 ) const
{
    return Concatenate( m2 );
}
//! \brief 重载=，完成矩阵的赋值
//! \brief &m2,赋值矩阵
YkMatrix4d& YkMatrix4d::operator =(const YkMatrix4d &m2)
{
	memcpy(this->m, m2.m, 16 * sizeof(YkDouble));
	return *this;
}
//! \brief 重载*，矩阵与三维向量相乘。
//! \param &v 三维向量[in]。
//! \return 三维向量。
//! \remarks 从矩阵中取出最上面3X3矩阵与三维向量进行相乘。
YkVector3d YkMatrix4d::operator * ( const YkVector3d &v ) const
{
	YkVector3d r;		
	YkDouble fInvW = (YkDouble)(1.0 / ( m[3][0] + m[3][1] + m[3][2] + m[3][3] ));
	
	r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
	r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
	r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;
	
	return r;
}
	
//! \brief 矩阵相乘。
//! \param &m2 另外一个4X4矩阵[in]。
//! \return 完成运算后4X4矩阵。
YkMatrix4d YkMatrix4d::Concatenate(const YkMatrix4d &multiplyM2) const
{
	YkMatrix4d tempMatrix;
	tempMatrix.m[0][0] = m[0][0] * multiplyM2.m[0][0] + m[0][1] * multiplyM2.m[1][0] + m[0][2] * multiplyM2.m[2][0] + m[0][3] * multiplyM2.m[3][0];
	tempMatrix.m[0][1] = m[0][0] * multiplyM2.m[0][1] + m[0][1] * multiplyM2.m[1][1] + m[0][2] * multiplyM2.m[2][1] + m[0][3] * multiplyM2.m[3][1];
	tempMatrix.m[0][2] = m[0][0] * multiplyM2.m[0][2] + m[0][1] * multiplyM2.m[1][2] + m[0][2] * multiplyM2.m[2][2] + m[0][3] * multiplyM2.m[3][2];
	tempMatrix.m[0][3] = m[0][0] * multiplyM2.m[0][3] + m[0][1] * multiplyM2.m[1][3] + m[0][2] * multiplyM2.m[2][3] + m[0][3] * multiplyM2.m[3][3];

	tempMatrix.m[1][0] = m[1][0] * multiplyM2.m[0][0] + m[1][1] * multiplyM2.m[1][0] + m[1][2] * multiplyM2.m[2][0] + m[1][3] * multiplyM2.m[3][0];
	tempMatrix.m[1][1] = m[1][0] * multiplyM2.m[0][1] + m[1][1] * multiplyM2.m[1][1] + m[1][2] * multiplyM2.m[2][1] + m[1][3] * multiplyM2.m[3][1];
	tempMatrix.m[1][2] = m[1][0] * multiplyM2.m[0][2] + m[1][1] * multiplyM2.m[1][2] + m[1][2] * multiplyM2.m[2][2] + m[1][3] * multiplyM2.m[3][2];
	tempMatrix.m[1][3] = m[1][0] * multiplyM2.m[0][3] + m[1][1] * multiplyM2.m[1][3] + m[1][2] * multiplyM2.m[2][3] + m[1][3] * multiplyM2.m[3][3];

	tempMatrix.m[2][0] = m[2][0] * multiplyM2.m[0][0] + m[2][1] * multiplyM2.m[1][0] + m[2][2] * multiplyM2.m[2][0] + m[2][3] * multiplyM2.m[3][0];
	tempMatrix.m[2][1] = m[2][0] * multiplyM2.m[0][1] + m[2][1] * multiplyM2.m[1][1] + m[2][2] * multiplyM2.m[2][1] + m[2][3] * multiplyM2.m[3][1];
	tempMatrix.m[2][2] = m[2][0] * multiplyM2.m[0][2] + m[2][1] * multiplyM2.m[1][2] + m[2][2] * multiplyM2.m[2][2] + m[2][3] * multiplyM2.m[3][2];
	tempMatrix.m[2][3] = m[2][0] * multiplyM2.m[0][3] + m[2][1] * multiplyM2.m[1][3] + m[2][2] * multiplyM2.m[2][3] + m[2][3] * multiplyM2.m[3][3];

	tempMatrix.m[3][0] = m[3][0] * multiplyM2.m[0][0] + m[3][1] * multiplyM2.m[1][0] + m[3][2] * multiplyM2.m[2][0] + m[3][3] * multiplyM2.m[3][0];
	tempMatrix.m[3][1] = m[3][0] * multiplyM2.m[0][1] + m[3][1] * multiplyM2.m[1][1] + m[3][2] * multiplyM2.m[2][1] + m[3][3] * multiplyM2.m[3][1];
	tempMatrix.m[3][2] = m[3][0] * multiplyM2.m[0][2] + m[3][1] * multiplyM2.m[1][2] + m[3][2] * multiplyM2.m[2][2] + m[3][3] * multiplyM2.m[3][2];
	tempMatrix.m[3][3] = m[3][0] * multiplyM2.m[0][3] + m[3][1] * multiplyM2.m[1][3] + m[3][2] * multiplyM2.m[2][3] + m[3][3] * multiplyM2.m[3][3];

	return tempMatrix;
}

//! \brief 重载[]，取出对应行。
//! \param iRow 行号，有效范围为零到三[in]。
//! \return 矩阵指定一行数据，返回行数据指针。
YkDouble* YkMatrix4d::operator[] (YkUint iRow)
{
    YKASSERT( iRow < 4 );
    return (YkDouble*)m[iRow];
}


//! \brief 判断两个矩阵是否相等。
//! \param &m2 另外一个矩阵[in]。
//! \return 若相等返回为TRUE，若不相等则返回为FALSE。
YkBool YkMatrix4d::operator == (const YkMatrix4d &m2) const
{
		return
		(
		YKIS0(m[0][0]-m2.m[0][0])&& 
		YKIS0(m[0][1]-m2.m[0][1])&& 
		YKIS0(m[0][2]-m2.m[0][2])&& 
		YKIS0(m[0][3]-m2.m[0][3])&& 
		YKIS0(m[1][0]-m2.m[1][0])&& 
		YKIS0(m[1][1]-m2.m[1][1])&& 
		YKIS0(m[1][2]-m2.m[1][2])&& 
		YKIS0(m[1][3]-m2.m[1][3])&& 
		YKIS0(m[2][0]-m2.m[2][0])&& 
		YKIS0(m[2][1]-m2.m[2][1])&& 
		YKIS0(m[2][2]-m2.m[2][2])&& 
		YKIS0(m[2][3]-m2.m[2][3])&& 
		YKIS0(m[3][0]-m2.m[3][0])&& 
		YKIS0(m[3][1]-m2.m[3][1])&& 
		YKIS0(m[3][2]-m2.m[3][2])&& 
		YKIS0(m[3][3]-m2.m[3][3])
		);
}

//! \brief 判断两个矩阵是否不相等。
//! \param &m2 另外一个矩阵[in]。
//! \return 若不相等返回为TRUE，否则返回FALSE。
YkBool YkMatrix4d::operator != (const YkMatrix4d &m2) const
{
	return
		!(
		YKIS0(m[0][0]-m2.m[0][0])&& 
		YKIS0(m[0][1]-m2.m[0][1])&& 
		YKIS0(m[0][2]-m2.m[0][2])&& 
		YKIS0(m[0][3]-m2.m[0][3])&& 
		YKIS0(m[1][0]-m2.m[1][0])&& 
		YKIS0(m[1][1]-m2.m[1][1])&& 
		YKIS0(m[1][2]-m2.m[1][2])&& 
		YKIS0(m[1][3]-m2.m[1][3])&& 
		YKIS0(m[2][0]-m2.m[2][0])&& 
		YKIS0(m[2][1]-m2.m[2][1])&& 
		YKIS0(m[2][2]-m2.m[2][2])&& 
		YKIS0(m[2][3]-m2.m[2][3])&& 
		YKIS0(m[3][0]-m2.m[3][0])&& 
		YKIS0(m[3][1]-m2.m[3][1])&& 
		YKIS0(m[3][2]-m2.m[3][2])&& 
		YKIS0(m[3][3]-m2.m[3][3])
		); 
}

//! \brief 取出只读矩阵行数据。
//! \param iRow 行号，有效范围为零到三[in]。
//! \return 行数据。
const YkDouble *const YkMatrix4d::operator [] ( YkUint iRow ) const
{
    YKASSERT( iRow < 4 );
    return m[iRow];
}

YkDouble YkMatrix4d::operator () (YkUint iRow, YkUint iCol)
{
	YKASSERT( iRow < 4 || iCol < 4);
	return m[iRow][iCol];
}

YkDouble YkMatrix4d::MINOR(const YkMatrix4d& m, const YkInt r0, const YkInt r1, const YkInt r2, 
		const YkInt c0, const YkInt c1, const YkInt c2) const
{
	return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
		m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
		m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
}


YkMatrix4d YkMatrix4d::Translation(YkDouble x, YkDouble y, YkDouble z)
{
	 YkMatrix4d solution
     (
     1, 0, 0, 0,
     0, 1, 0, 0,
     0, 0, 1, 0,
     x, y, z, 1
     );

	 return solution;
}

YkMatrix4d YkMatrix4d::Scaling(YkDouble x, YkDouble y, YkDouble z)
{
	YkMatrix4d solution
	(
		x,0,0,0,
		0,y,0,0,
		0,0,z,0,
		0,0,0,1
	);

	return solution;
}

YkMatrix4d YkMatrix4d::RotationXYZ(YkDouble rotationX, YkDouble rotationY, YkDouble rotationZ)
{
	return YkMatrix4d::RotationX(rotationX)*YkMatrix4d::RotationY(rotationY)*RotationZ(rotationZ);
}
YkMatrix4d YkMatrix4d::RotationX(YkDouble angle)
{
	YkDouble dSin = (YkDouble)sin(angle);
	YkDouble dCos = (YkDouble)cos(angle);

	YkMatrix4d solution
	(
	1,0,0,0,
	0, dCos, dSin, 0,
	0, -dSin, dCos, 0,
	0, 0, 0, 1 
	);
	return solution;
}
YkMatrix4d YkMatrix4d::RotationY(YkDouble angle)
{
	YkDouble dSin = (YkDouble)sin(angle);
	YkDouble dCos = (YkDouble)cos(angle);

	YkMatrix4d solution
	(
	   dCos,0, -dSin, 0,
	   0,1,0,0,
	   dSin,0,dCos, 0,
	   0, 0, 0, 1 
	);
	return solution;
}
YkMatrix4d YkMatrix4d::RotationZ(YkDouble angle)
{
	YkDouble dSin = (YkDouble)sin(angle);
	YkDouble dCos = (YkDouble)cos(angle);

	YkMatrix4d solution
	(
	   dCos, dSin, 0, 0 ,
	   -dSin, dCos, 0, 0,
	   0, 0, 1, 0 ,
	   0, 0, 0, 1 
	);
	return solution;
}
YkMatrix4d YkMatrix4d::Adjoint() const
{
	//计算矩阵的伴随矩阵
	return YkMatrix4d( 
		//计算4x4中任意3x3子矩阵行列式	
		MINOR(*this, 1, 2, 3, 1, 2, 3),-MINOR(*this, 0, 2, 3, 1, 2, 3),MINOR(*this, 0, 1, 3, 1, 2, 3),-MINOR(*this, 0, 1, 2, 1, 2, 3),
		//计算4x4中任意3x3子矩阵行列式	
		-MINOR(*this, 1, 2, 3, 0, 2, 3),MINOR(*this, 0, 2, 3, 0, 2, 3),-MINOR(*this, 0, 1, 3, 0, 2, 3),MINOR(*this, 0, 1, 2, 0, 2, 3),
		//计算4x4中任意3x3子矩阵行列式		
		MINOR(*this, 1, 2, 3, 0, 1, 3),-MINOR(*this, 0, 2, 3, 0, 1, 3),MINOR(*this, 0, 1, 3, 0, 1, 3),-MINOR(*this, 0, 1, 2, 0, 1, 3),
		//计算4x4中任意3x3子矩阵行列式		
		-MINOR(*this, 1, 2, 3, 0, 1, 2),MINOR(*this, 0, 2, 3, 0, 1, 2),-MINOR(*this, 0, 1, 3, 0, 1, 2),MINOR(*this, 0, 1, 2, 0, 1, 2));
}
YkDouble YkMatrix4d::Determinant() const
{
	return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) - m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) + m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) - m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}
YkMatrix4d YkMatrix4d::Invert() const
{	
	return Adjoint() * (1.0f / Determinant());
}

YkDouble* YkMatrix4d::GetPointer()
{
	return (YkDouble*)&m;
}

}
