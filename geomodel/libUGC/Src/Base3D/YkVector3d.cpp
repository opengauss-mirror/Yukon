/*
 *
 * YkVector3d.cpp
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

#include "Base3D/YkVector3d.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkVector4d.h"

namespace Yk 
{
const YkVector3d YkVector3d::ZERO(0,0,0);
const YkVector3d YkVector3d::UNIT_X( 1, 0, 0 );
const YkVector3d YkVector3d::UNIT_Y( 0, 1, 0 );
const YkVector3d YkVector3d::UNIT_Z( 0, 0, 1 );
const YkVector3d YkVector3d::NEGATIVE_UNIT_X( -1,  0,  0 );
const YkVector3d YkVector3d::NEGATIVE_UNIT_Y(  0, -1,  0 );
const YkVector3d YkVector3d::NEGATIVE_UNIT_Z(  0,  0, -1 );
const YkVector3d YkVector3d::UNIT_SCALE(1, 1, 1);
YkVector3d::YkVector3d()
{
	x=0;
	y=0;
	z=0;
}
YkVector3d::YkVector3d(YkDouble fX, YkDouble fY, YkDouble fZ)
	:x(fX), y(fY), z(fZ)
{
};
YkVector3d::~YkVector3d()
{
}
YkDouble YkVector3d::Length() const
{
	return sqrt(x*x+y*y+z*z);
}

void YkVector3d::MultiplyMatrix(YkVector3d& vecPos, YkMatrix4d& m)
{
	YkDouble w = 1;
	YkDouble m11 = 0, m12 = 0, m13 = 0, m14 = 0;
	
	m11 = vecPos.x * m[0][0] + vecPos.y * m[1][0] + vecPos.z * m[2][0] + w* m[3][0];
	m12 = vecPos.x * m[0][1] + vecPos.y * m[1][1] + vecPos.z * m[2][1] + w* m[3][1];
	m13 = vecPos.x * m[0][2] + vecPos.y * m[1][2] + vecPos.z * m[2][2] + w* m[3][2];
	m14 = vecPos.x * m[0][3] + vecPos.y * m[1][3] + vecPos.z * m[2][3] + w* m[3][3];

	vecPos.x = m11;
	vecPos.y = m12;
	vecPos.z = m13;
}
YkVector3d YkVector3d::MultiplyMatrix(const YkMatrix4d& m) const
{   
	YkDouble w = 1;
	YkDouble m11 = 0, m12 = 0, m13 = 0, m14 = 0;

	m11 = x * m[0][0] + y * m[1][0] + z * m[2][0] + w* m[3][0];
	m12 = x * m[0][1] + y * m[1][1] + z * m[2][1] + w* m[3][1];
	m13 = x * m[0][2] + y * m[1][2] + z * m[2][2] + w* m[3][2];
	m14 = x * m[0][3] + y * m[1][3] + z * m[2][3] + w* m[3][3];

	return YkVector3d(m11, m12, m13);
}

YkVector3d& YkVector3d::operator = (const YkVector3d& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return *this;
}
YkBool YkVector3d::operator == (const YkVector3d& vec) const
{
	return (YKEQUAL(x,vec.x)&&YKEQUAL(y,vec.y)&&YKEQUAL(z,vec.z));
}

YkBool YkVector3d::operator != (const YkVector3d& vec) const
{
	return (x != vec.x|| y != vec.y||z != vec.z);
}
YkBool YkVector3d::operator > (const YkVector3d& vec)const
{
	return (x > vec.x && y > vec.y && z > vec.z);	
}
YkBool YkVector3d::operator < (const YkVector3d& vec)const
{
	return (x < vec.x && y < vec.y && z < vec.z);
}
YkVector3d YkVector3d::operator + (const YkVector3d& vec) const
{
	YkVector3d TempVec;
	TempVec.x =x + vec.x;
	TempVec.y =y + vec.y;
	TempVec.z =z + vec.z;
	return TempVec;
	
}
YkVector3d YkVector3d::operator - (const YkVector3d& vec) const
{
	YkVector3d TempVec;
	TempVec.x =x - vec.x;
	TempVec.y =y - vec.y;
	TempVec.z =z - vec.z;
	return TempVec;
}
YkVector3d YkVector3d::operator * (const YkDouble fScale) const
{
	YkVector3d TempVec;
	TempVec.x =x * fScale;
	TempVec.y =y * fScale;
	TempVec.z =z * fScale;
	return TempVec;	
}
YkDouble YkVector3d::operator [] (const unsigned i) const
{
	YKASSERT(i < 3);
	return *(&x + i);
}
YkDouble& YkVector3d::operator [] (const unsigned i)
{
	YKASSERT(i < 3);
	return *(&x + i);
}

YkVector3d YkVector3d::operator / (const YkDouble fScale) const
{
	YkVector3d TempVec;
	if (!YKIS0(fScale))
	{
		TempVec.x =x / fScale;
		TempVec.y =y / fScale;
		TempVec.z =z / fScale;
		return TempVec;
		 
	}
	return *this;
	
}
YkVector3d YkVector3d::operator - () const
{
	YkVector3d TempVec;
	TempVec.x = -x;
	TempVec.y = -y;
	TempVec.z = -z;
	return TempVec;
}
YkVector3d& YkVector3d::operator += (const YkVector3d& vec) 
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}
YkVector3d& YkVector3d::operator -= (const YkVector3d& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}
YkVector3d& YkVector3d::operator *= (const YkDouble fScale)
{
	x *= fScale;
	y *= fScale;
	z *= fScale;
	return *this;
}


YkVector3d& YkVector3d::operator /= (const YkDouble fScale )
{
	if ( !YKIS0( fScale ) )
	{
		x /= fScale;
		y /= fScale;
		z /= fScale;
	}

	return *this;
}


void YkVector3d::MakeFloor( const YkVector3d& cmp )
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y;
    if( cmp.z < z ) z = cmp.z;
}
void YkVector3d::MakeCeil( const YkVector3d& cmp )
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y;
    if( cmp.z > z ) z = cmp.z;
}


YkVector3d operator* ( const YkDouble fScalar, const YkVector3d& rkVector )
{
	return YkVector3d(
		fScalar * rkVector.x,
		fScalar * rkVector.y,
		fScalar * rkVector.z);
}
}
