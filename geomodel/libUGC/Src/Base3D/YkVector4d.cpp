/*
 *
 * YkVector4d.cpp
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

#include "Base3D/YkVector4d.h"
#include "Base3D/YkMatrix4d.h"

namespace Yk 
{
const YkVector4d YkVector4d::ZERO( 0, 0, 0, 0 );

YkVector4d::YkVector4d()
{
	*this = YkVector4d::ZERO;
}

YkVector4d::YkVector4d(const YkDouble fX, const YkDouble fY, const YkDouble fZ, const YkDouble fW )
    : x( fX ), y( fY ), z( fZ ), w( fW)
{
}

YkVector4d::YkVector4d( const YkDouble afCoordinate[4] )
    : x( afCoordinate[0] ),
      y( afCoordinate[1] ),
      z( afCoordinate[2] ),
      w( afCoordinate[3] )
{
}

YkVector4d::YkVector4d( const int afCoordinate[4] )
{
    x = (YkDouble)afCoordinate[0];
    y = (YkDouble)afCoordinate[1];
    z = (YkDouble)afCoordinate[2];
    w = (YkDouble)afCoordinate[3];
}

YkVector4d::YkVector4d( YkDouble* const r )
    : x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
{
}

YkVector4d::YkVector4d( const YkDouble scaler )
    : x( scaler )
    , y( scaler )
    , z( scaler )
    , w( scaler )
{
}

YkVector4d::YkVector4d(const YkVector3d& rhs)
    : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
{
}

YkVector4d::YkVector4d( const YkVector4d& rkVector )
    : x( rkVector.x ), y( rkVector.y ), z( rkVector.z ), w (rkVector.w)
{
}

YkDouble YkVector4d::operator [] ( const size_t i ) const
{
    return *(&x+i);
}

YkDouble& YkVector4d::operator [] ( const size_t i )
{
    return *(&x+i);
}

YkVector4d& YkVector4d::operator = ( const YkVector4d& rkVector )
{
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    w = rkVector.w;

    return *this;
}

YkVector4d& YkVector4d::operator = ( const YkDouble fScalar)
{
	x = fScalar;
	y = fScalar;
	z = fScalar;
	w = fScalar;
	return *this;
}

bool YkVector4d::operator == ( const YkVector4d& rkVector ) const
{
    return ( YKEQUAL(x,rkVector.x) &&
        YKEQUAL(y,rkVector.y) &&
        YKEQUAL(z,rkVector.z) &&
        YKEQUAL(w,rkVector.w) );
}

bool YkVector4d::operator != ( const YkVector4d& rkVector ) const
{
    return ( x != rkVector.x ||
        y != rkVector.y ||
        z != rkVector.z ||
        w != rkVector.w );
}

YkVector4d& YkVector4d::operator = (const YkVector3d& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = 1.0f;
    return *this;
}

YkVector4d YkVector4d::operator + ( const YkVector4d& rkVector ) const
{
    return YkVector4d(
        x + rkVector.x,
        y + rkVector.y,
        z + rkVector.z,
        w + rkVector.w);
}

YkVector4d YkVector4d::operator - ( const YkVector4d& rkVector ) const
{
    return YkVector4d(
        x - rkVector.x,
        y - rkVector.y,
        z - rkVector.z,
        w - rkVector.w);
}

YkVector4d YkVector4d::operator * ( const YkDouble fScalar ) const
{
    return YkVector4d(
        x * fScalar,
        y * fScalar,
        z * fScalar,
        w * fScalar);
}

YkVector4d YkVector4d::operator * ( const YkVector4d& rhs) const
{
    return YkVector4d(
        rhs.x * x,
        rhs.y * y,
        rhs.z * z,
        rhs.w * w);
}

YkVector4d YkVector4d::operator / ( const YkDouble fScalar ) const
{
	if (!YKIS0(fScalar))
	{
		YkDouble fInv = 1.0 / fScalar;

		return YkVector4d(
			x * fInv,
			y * fInv,
			z * fInv,
			w * fInv);
	}
	return *this;
   
}

YkVector4d YkVector4d::operator / ( const YkVector4d& rhs) const
{
	if (YKIS0(rhs.x) || YKIS0(rhs.y) || YKIS0(rhs.z) || YKIS0(rhs.w))
	{
		return *this;
	}
    return YkVector4d(
        x / rhs.x,
        y / rhs.y,
        z / rhs.z,
        w / rhs.w);
}

const YkVector4d& YkVector4d::operator + () const
{
    return *this;
}

YkVector4d YkVector4d::operator - () const
{
    return YkVector4d(-x, -y, -z, -w);
}

YkVector4d operator * ( const YkDouble fScalar, const YkVector4d& rkVector )
{

	
    return YkVector4d(
        fScalar * rkVector.x,
        fScalar * rkVector.y,
        fScalar * rkVector.z,
        fScalar * rkVector.w);
}

YkVector4d operator / ( const YkDouble fScalar, const YkVector4d& rkVector )
{
	if (YKIS0(rkVector.x) || YKIS0(rkVector.y) || YKIS0(rkVector.z) || YKIS0(rkVector.w))
	{
		return YkVector4d::ZERO;
	}

    return YkVector4d(
        fScalar / rkVector.x,
        fScalar / rkVector.y,
        fScalar / rkVector.z,
        fScalar / rkVector.w);
}

YkVector4d operator + (const YkVector4d& lhs, const YkDouble rhs)
{
    return YkVector4d(
        lhs.x + rhs,
        lhs.y + rhs,
        lhs.z + rhs,
        lhs.w + rhs);
}

YkVector4d operator + (const YkDouble lhs, const YkVector4d& rhs)
{
    return YkVector4d(
        lhs + rhs.x,
        lhs + rhs.y,
        lhs + rhs.z,
        lhs + rhs.w);
}

YkVector4d operator - (const YkVector4d& lhs, YkDouble rhs)
{
    return YkVector4d(
        lhs.x - rhs,
        lhs.y - rhs,
        lhs.z - rhs,
        lhs.w - rhs);
}

YkVector4d operator - (const YkDouble lhs, const YkVector4d& rhs)
{
    return YkVector4d(
        lhs - rhs.x,
        lhs - rhs.y,
        lhs - rhs.z,
        lhs - rhs.w);
}

YkVector4d& YkVector4d::operator += ( const YkVector4d& rkVector )
{
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;
    w += rkVector.w;

    return *this;
}

YkVector4d& YkVector4d::operator -= ( const YkVector4d& rkVector )
{
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;
    w -= rkVector.w;

    return *this;
}

YkVector4d& YkVector4d::operator *= ( const YkDouble fScalar )
{
    x *= fScalar;
    y *= fScalar;
    z *= fScalar;
    w *= fScalar;
    return *this;
}

YkVector4d& YkVector4d::operator += ( const YkDouble fScalar )
{
    x += fScalar;
    y += fScalar;
    z += fScalar;
    w += fScalar;
    return *this;
}

YkVector4d& YkVector4d::operator -= ( const YkDouble fScalar )
{
    x -= fScalar;
    y -= fScalar;
    z -= fScalar;
    w -= fScalar;
    return *this;
}

YkVector4d& YkVector4d::operator *= ( const YkVector4d& rkVector )
{
    x *= rkVector.x;
    y *= rkVector.y;
    z *= rkVector.z;
    w *= rkVector.w;

    return *this;
}

YkVector4d& YkVector4d::operator /= ( const YkDouble fScalar )
{
	if (YKIS0(fScalar))
	{
		return *this;
	}
    YkDouble fInv = 1.0 / fScalar;

    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;

    return *this;
}

YkVector4d& YkVector4d::operator /= ( const YkVector4d& rkVector )
{
	if (YKIS0(rkVector.x) || YKIS0(rkVector.y) || YKIS0(rkVector.z) || YKIS0(rkVector.w))
	{
		return *this;
	}
    x /= rkVector.x;
    y /= rkVector.y;
    z /= rkVector.z;
    w /= rkVector.w;

    return *this;
}


YkVector4d YkVector4d::operator * (const YkMatrix4d& m)
{
	YkDouble m11 = 0, m12 = 0, m13 = 0, m14 = 0;
	m11 = x * m[0][0] + y * m[1][0] + z * m[2][0] + w* m[3][0];
	m12 = x * m[0][1] + y * m[1][1] + z * m[2][1] + w* m[3][1];
	m13 = x * m[0][2] + y * m[1][2] + z * m[2][2] + w* m[3][2];
	m14 = x * m[0][3] + y * m[1][3] + z * m[2][3] + w* m[3][3];
	return YkVector4d(m11, m12, m13, m14);
}
}
