/*
 *
 * YkPoint3D.cpp
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

#include "Base/YkPoint3D.h"

namespace  Yk{

YkPoint3D::YkPoint3D():x(0),y(0),z(0)
{

}

YkPoint3D::~YkPoint3D()
{

}

YkPoint3D::YkPoint3D(YkDouble xx,YkDouble yy,YkDouble zz):x(xx),y(yy),z(zz)
{

}
YkBool operator==(const YkPoint3D& p,const YkPoint3D& q)
{
	return YKEQUAL(p.x, q.x) && YKEQUAL(p.y, q.y) && YKEQUAL(p.z, q.z);
}

YkBool operator!=(const YkPoint3D& p,const YkPoint3D& q)
{
	return !YKEQUAL(p.x, q.x) || !YKEQUAL(p.y, q.y) || !YKEQUAL(p.z, q.z);
}

YkPoint3D& YkPoint3D::operator=(const YkPoint3D& s)
{
	if(this == &s)
		return *this;
	x=s.x;
	y=s.y;
	z=s.z; 
	return *this;
}

YkPoint3D& YkPoint3D::operator+=(const YkPoint3D& p)
{
	x+=p.x;
	y+=p.y;
	z+=p.z;
	return *this;
}

YkPoint3D& YkPoint3D::operator-=(const YkPoint3D& p)
{
	x-=p.x;
	y-=p.y;
	z-=p.z;
	return *this;
}

YkPoint3D& YkPoint3D::operator*=(YkDouble c)
{
	x*=c;
	y*=c;
	z*=c;
	return *this;
}

YkPoint3D& YkPoint3D::operator/=(YkDouble c)
{
	if (!YKIS0(c))
	{
		x/=c;
		y/=c;
		z/=c;
	}
	
	return *this;
}

YkPoint3D YkPoint3D::operator-() const
{
	return YkPoint3D(-x,-y,-z);
}

}
