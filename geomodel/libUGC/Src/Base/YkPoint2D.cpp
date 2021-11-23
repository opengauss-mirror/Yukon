/*
 *
 * YkPoint2D.cpp
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

#include "Base/YkPoint2D.h"

namespace Yk {

YkPoint2D::YkPoint2D():x(0),y(0)
{
	
}

YkPoint2D::~YkPoint2D()
{
	
}

YkPoint2D::YkPoint2D(YkDouble dX,YkDouble dY):x(dX),y(dY)
{

}

YkPoint2D::YkPoint2D(const YkSize2D& sz):x(sz.cx),y(sz.cy)
{

}

YkPoint2D::YkPoint2D(const YkPoint& pnt):x(pnt.x),y(pnt.y)
{

}

YkBool operator==(const YkPoint2D& pntStart,const YkPoint2D& pntEnd)
{
	return (YKIS0(pntStart.x-pntEnd.x) && YKIS0(pntStart.y-pntEnd.y));
}

YkBool operator!=(const YkPoint2D& pntStart,const YkPoint2D& pntEnd)
{
	return ((!YKIS0(pntStart.x-pntEnd.x)) || (!YKIS0(pntStart.y-pntEnd.y)));
}

void YkPoint2D::Offset(const YkSize2D &szValue)
{
	x += szValue.cx;
	y += szValue.cy;
}

void YkPoint2D::Offset(YkDouble dOffsetX, YkDouble dOffsetY)
{
	x += dOffsetX;
	y += dOffsetY;
}

void YkPoint2D::Rotate(const YkPoint2D &pntAnchor, YkDouble dCos, YkDouble dSin)
{
    YkDouble xx = x - pntAnchor.x;
    YkDouble yy = y - pntAnchor.y;
    
	x = xx * dCos - yy * dSin + pntAnchor.x;
    y = xx * dSin + yy * dCos + pntAnchor.y;
}

void YkPoint2D::Rotate(const YkPoint2D &pntAnchor, YkDouble dRadian)
{
	YkDouble dCos = cos(dRadian), dSin = sin(dRadian);
    YkDouble xx = x - pntAnchor.x;
    YkDouble yy = y - pntAnchor.y;
    
	x = xx * dCos - yy * dSin + pntAnchor.x;
    y = xx * dSin + yy * dCos + pntAnchor.y;
}

YkBool YkPoint2D::EQ(const YkPoint2D &pntTest, YkDouble dTolerance)const
{
	if (!YKEQ(x,pntTest.x,dTolerance) || !YKEQ(y,pntTest.y,dTolerance))
	{
		return FALSE;
	}
	double dDistance2 = (x-pntTest.x)*(x-pntTest.x) + (y-pntTest.y)*(y-pntTest.y);
	double dTolerance2 = dTolerance*dTolerance;
	return dDistance2 <= dTolerance2;
}

YkPoint2D& YkPoint2D::operator=(const YkSize2D& sz)
{
	x=sz.cx;
	y=sz.cy;
	return *this;
}

YkPoint2D& YkPoint2D::operator=(const YkPoint2D& pnt2)
{
	if( this == &pnt2)
		return *this;
	x=pnt2.x;
	y=pnt2.y;
	return *this;
}

YkPoint2D& YkPoint2D::operator=(const YkPoint& pnt)
{
	x=pnt.x;
	y=pnt.y;
	return *this;
}

YkPoint2D& YkPoint2D::operator+=(const YkPoint2D& pnt)
{
	x+=pnt.x;
	y+=pnt.y;
	return *this;
}

YkPoint2D& YkPoint2D::operator+=(const YkSize2D& sz)
{
	x+=sz.cx;
	y+=sz.cy;
	return *this;
}

YkPoint2D& YkPoint2D::operator-=(const YkPoint2D& pnt)
{
	x-=pnt.x;
	y-=pnt.y;
	return *this;
}

YkPoint2D& YkPoint2D::operator-=(const YkSize2D& sz)
{
	x-=sz.cx;
	y-=sz.cy;
	return *this;
}

YkPoint2D YkPoint2D::operator-() const
{
	return YkPoint2D(-x,-y);
}

YkPoint2D operator+(const YkPoint2D& pntStart,const YkPoint2D& pntEnd)
{
	return YkPoint2D(pntStart.x+pntEnd.x,pntStart.y+pntEnd.y);
}

YkPoint2D operator+(const YkPoint2D& pnt,const YkSize2D& sz)
{
	return YkPoint2D(pnt.x+sz.cx,pnt.y+sz.cy);
}

YkPoint2D operator+(const YkSize2D& sz,const YkPoint2D& pnt)
{
	return YkPoint2D(sz.cx+pnt.x,sz.cy+pnt.y);
}

YkPoint2D operator-(const YkPoint2D& pntStart,const YkPoint2D& pntEnd)
{
	return YkPoint2D(pntStart.x-pntEnd.x,pntStart.y-pntEnd.y);
}

YkPoint2D operator-(const YkPoint2D& pnt,const YkSize2D& sz)
{
	return YkPoint2D(pnt.x-sz.cx,pnt.y-sz.cy);
}

YkPoint2D operator-(const YkSize2D& sz,const YkPoint2D& pnt)
{
	return YkPoint2D(sz.cx-pnt.x,sz.cy-pnt.y);
}

YkBool operator<(const YkPoint2D& pnt,const YkPoint2D& sz)
{
	if(pnt.x < sz.x - EP)
	{
		return true;
	}
	else if(pnt.x > sz.x + EP)
	{
		return false;
	}
	else 
	{
		if(pnt.y < sz.y - EP)
		{
			return true;
		}
	}
	return false;
}
}


