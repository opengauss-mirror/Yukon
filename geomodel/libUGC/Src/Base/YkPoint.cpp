/*
 *
 * YkPoint.cpp
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

#include "Base/YkPoint.h"

namespace Yk {

YkPoint::YkPoint() :x(0),y(0)
{

}	

YkPoint::~YkPoint()
{

}

YkPoint::YkPoint(YkInt nX,YkInt nY):x(nX),y(nY) 
{

}

YkPoint::YkPoint(const YkSize& sz):x(sz.cx),y(sz.cy) 
{
	
}

YkBool operator==(const YkPoint& pntStart,const YkPoint& pntEnd)
{
	return ((pntStart.x==pntEnd.x) && (pntStart.y==pntEnd.y)); 
}

YkBool operator!=(const YkPoint& pntStart,const YkPoint& pntEnd)
{
	return pntStart.x!=pntEnd.x || pntStart.y!=pntEnd.y;
}

YkPoint& YkPoint::operator=(const YkSize& sz)
{ 
	x=sz.cx; 
	y=sz.cy; 
	return *this; 
}

YkPoint& YkPoint::operator=(const YkPoint& pnt)
{
	x=pnt.x; 
	y=pnt.y; 
	return *this; 
}

void YkPoint::Offset(const YkSize &szValue)
{
	x += szValue.cx;
	y += szValue.cy;
}

void YkPoint::Offset(YkInt nOffsetX, YkInt nOffsetY) 
{
	x += nOffsetX;
	y += nOffsetY;
}

void YkPoint::Rotate(const YkPoint &pntAnchor, double dCos, double dSin)
{
    double xx = x - pntAnchor.x;
    double yy = y - pntAnchor.y;
    x = YKROUND(xx * dCos - yy * dSin)  + pntAnchor.x;
    y = YKROUND(xx * dSin + yy * dCos)  + pntAnchor.y;
}

void YkPoint::Rotate(const YkPoint &pntAnchor, double dRadian)
{
	double dCos = cos(dRadian), dSin = sin(dRadian);
    double xx = x - pntAnchor.x;
    double yy = y - pntAnchor.y;
    
	x = YKROUND(xx * dCos - yy * dSin)  + pntAnchor.x;
    y = YKROUND(xx * dSin + yy * dCos)  + pntAnchor.y;
}

YkPoint& YkPoint::operator+=(const YkPoint& pnt)
{
	x+=pnt.x; 
	y+=pnt.y; 
	return *this; 
}

YkPoint& YkPoint::operator+=(const YkSize& sz)
{
	x+=sz.cx; 
	y+=sz.cy; 
	return *this; 
}

YkPoint& YkPoint::operator-=(const YkPoint& pnt)
{
	x-=pnt.x;
	y-=pnt.y; 
	return *this; 
}

YkPoint& YkPoint::operator-=(const YkSize& sz)
{
	x-=sz.cx; 
	y-=sz.cy; 
	return *this; 
}

YkPoint YkPoint::operator-() const
{
	return YkPoint(-x,-y); 
}

bool YkPoint::operator<(const YkPoint& pt) const
{
	return x<pt.x || (x==pt.x && y<pt.y);
}

YkPoint operator+(const YkPoint& pntStart,const YkPoint& pntEnd)
{
	return YkPoint(pntStart.x+pntEnd.x,pntStart.y+pntEnd.y); 
}

YkPoint operator+(const YkPoint& pnt,const YkSize& sz)
{
	return YkPoint(pnt.x+sz.cx,pnt.y+sz.cy); 
}

YkPoint operator+(const YkSize& sz,const YkPoint& pnt)
{
	return YkPoint(sz.cx+pnt.x,sz.cy+pnt.y); 
}

YkPoint operator-(const YkPoint& pntStart,const YkPoint& pntEnd)
{
	return YkPoint(pntStart.x-pntEnd.x,pntStart.y-pntEnd.y); 
}

YkPoint operator-(const YkPoint& pnt,const YkSize& sz)
{
	return YkPoint(pnt.x-sz.cx,pnt.y-sz.cy); 
}

YkPoint operator-(const YkSize& sz,const YkPoint& pnt)
{
	return YkPoint(sz.cx-pnt.x,sz.cy-pnt.y); 
}


}
