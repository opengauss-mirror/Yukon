/*
 *
 * YkRect2D.cpp
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

#include "Base/YkRect2D.h"
#include <float.h>

namespace Yk {

YkRect2D::YkRect2D()
{
	left =	top = right = bottom = 0;
}

YkRect2D::YkRect2D(const YkRect2D& rc)
{
	left=rc.left;
	bottom=rc.bottom;
	right=rc.right;
	top=rc.top;
}

YkRect2D::~YkRect2D()
{
}

YkRect2D::YkRect2D(YkDouble dLeft,YkDouble dTop,YkDouble dRight,YkDouble dBottom)
:left(dLeft),top(dTop),right(dRight),bottom(dBottom)
{

}

YkRect2D::YkRect2D(const YkPoint2D& pnt,const YkSize2D& sz)
:left(pnt.x),top(pnt.y+sz.cy),right(pnt.x+sz.cx),bottom(pnt.y)
{

}

YkRect2D::YkRect2D(const YkPoint2D& pntTL,const YkPoint2D& pntBR)
:left(pntTL.x),top(pntTL.y),right(pntBR.x),bottom(pntBR.y)
{

}

YkRect2D& YkRect2D::operator=(const YkRect2D& rc)
{
	if( this == &rc)
		return *this;
	left=rc.left;
	bottom=rc.bottom;
	right=rc.right;
	top=rc.top;
	return *this;
}

YkBool operator==(const YkRect2D& rcStart,const YkRect2D& rcEnd)
{
	return YKEQUAL(rcStart.left, rcEnd.left) && YKEQUAL(rcStart.right, rcEnd.right) 
		&& YKEQUAL(rcStart.bottom, rcEnd.bottom) && YKEQUAL(rcStart.top, rcEnd.top);
}

YkBool operator!=(const YkRect2D& rcStart,const YkRect2D& rcEnd)
{
	return !YKEQUAL(rcStart.left, rcEnd.left) || !YKEQUAL(rcStart.right, rcEnd.right) 
		|| !YKEQUAL(rcStart.bottom, rcEnd.bottom) || !YKEQUAL(rcStart.top, rcEnd.top);
}

void YkRect2D::SetRect(YkDouble dLeft,YkDouble dTop,YkDouble dRight,YkDouble dBottom)
{
	left=dLeft;
	top=dTop;
	right=dRight;
	bottom=dBottom;
}

void YkRect2D::SetRect(const YkPoint2D& pntTL,const YkPoint2D& pntBR)
{
	SetRect(pntTL.x,pntTL.y,pntBR.x,pntBR.y);
}

void YkRect2D::SetEmpty()
{
	left=top=right=bottom=0;
}

YkBool YkRect2D::IsEmpty() const
{
	if(!this->IsValid())
	{
		return TRUE;
	}
	return YKIS0(right - left) || YKIS0(bottom - top);
}

YkBool YkRect2D::IsNull() const
{
	if(!this->IsValid())
	{
		return TRUE;
	}
	return YKIS0(left) && YKIS0(right) 
		&& YKIS0(bottom) && YKIS0(top) ;
}

YkDouble YkRect2D::Width() const	
{
	return right-left;
}

YkDouble YkRect2D::Height() const
{
	return top-bottom;
}

YkPoint2D& YkRect2D::TopLeft()
{
	return *((YkPoint2D*)this);
}

const YkPoint2D& YkRect2D::TopLeft() const
{
	return *((YkPoint2D*)this);
}

YkPoint2D YkRect2D::TopRight() const
{
	return YkPoint2D(right, top);
}

YkPoint2D& YkRect2D::BottomRight()
{
	return *((YkPoint2D*)this+1);
}

const YkPoint2D& YkRect2D::BottomRight() const
{
	return *((YkPoint2D*)this+1);
}

YkPoint2D YkRect2D::BottomLeft() const
{
	return YkPoint2D(left, bottom);
}

YkPoint2D YkRect2D::CenterPoint() const
{
	return YkPoint2D((left+right)/2,(top+bottom)/2);
}

YkSize2D YkRect2D::Size() const
{
	return YkSize2D(right-left,top-bottom);
}

void YkRect2D::SwapLeftRight()
{
	YkDouble tmp=0;
	YKSWAP(left,right,tmp);
}

void YkRect2D::SwapTopBottom()
{
	YkDouble tmp=0;
	YKSWAP(top,bottom,tmp);
}

YkBool YkRect2D::PtInRect(const YkPoint2D& pnt) const
{
	return (pnt.x>left || YKIS0(pnt.x-left) )	&& (pnt.x<right|| YKIS0(pnt.x-right))
		&& (pnt.y<top|| YKIS0(pnt.y-top))	&& (pnt.y>bottom|| YKIS0(pnt.y-bottom));
}

YkBool YkRect2D::Contains(const YkRect2D& rc) const
{
	return (rc.left > left || YKIS0(rc.left-left))
		&& (rc.top < top || YKIS0(rc.top-top))
		&& (rc.right < right || YKIS0(rc.right -right))
		&& (rc.bottom > bottom || YKIS0(rc.bottom-bottom));
}

YkBool YkRect2D::Withins(const YkRect2D& rc) const
{
	return (rc.left < left || YKIS0(rc.left-left))
		&& (rc.top > top || YKIS0(rc.top-top))
		&& (rc.right > right || YKIS0(rc.right -right))
		&& (rc.bottom < bottom || YKIS0(rc.bottom-bottom));
}

YkBool YkRect2D::IsIntersect(const YkRect2D& rc) const
{
	if(!this->IsValid() || !rc.IsValid())
	{
		return false;
	}
	return (right>rc.left || YKEQUAL(right,rc.left)) 
		&& (left<rc.right || YKEQUAL(left,rc.right))
		&& (top>rc.bottom|| YKEQUAL(top,rc.bottom))
		&& (bottom<rc.top|| YKEQUAL(bottom,rc.top));
}

void YkRect2D::Normalize()
{
	if(!IsValid())
	{
		return;
	}
	YkDouble tmp=0;
	if(left>right) YKSWAP(left,right,tmp);
	if(bottom>top) YKSWAP(top,bottom,tmp);	
}

void YkRect2D::Offset(YkDouble dX,YkDouble dY)
{
	left+=dX;
	top+=dY;
	right+=dX;
	bottom+=dY;
}

void YkRect2D::Offset(const YkSize2D& sz)
{
	Offset(sz.cx,sz.cy);
}

void YkRect2D::Offset(const YkPoint2D& pnt)
{
	Offset(pnt.x,pnt.y);
}

void YkRect2D::Inflate(YkDouble dMargin)
{
	left-=dMargin;
	top+=dMargin;
	right+=dMargin;
	bottom-=dMargin;
}

void YkRect2D::Inflate(YkDouble dHorMargin,YkDouble dVerMargin)
{
	left-=dHorMargin;
	top+=dVerMargin;
	right+=dHorMargin;
	bottom-=dVerMargin;
}

void YkRect2D::Inflate(YkDouble nLeftMargin,YkDouble nTopMargin,YkDouble nRightMargin,YkDouble nBottomMargin)
{
	left-=nLeftMargin;
	top+=nTopMargin;
	right+=nRightMargin;
	bottom-=nBottomMargin;
}

void YkRect2D::Deflate(YkDouble dMargin)
{
	Inflate(-dMargin);
}

void YkRect2D::Deflate(YkDouble mHorMargin,YkDouble mVerMargin)
{
	Inflate(-mHorMargin,-mVerMargin);
}

void YkRect2D::Deflate(YkDouble dLeftMargin,YkDouble dTopMargin,YkDouble dRightMargin,YkDouble dBottomMargin)
{
	Inflate(-dLeftMargin,-dTopMargin,-dRightMargin,-dBottomMargin);
}

void YkRect2D::Union(const YkRect2D& rc)
{
	if(!((YkRect2D)rc).IsValid()) return;
	if(!IsValid())
	{
		*this = rc;
		return;
	}
	left=YKMIN(left,rc.left);
	top=YKMAX(top,rc.top);
	right=YKMAX(right,rc.right);	
	bottom=YKMIN(bottom,rc.bottom);
}

void YkRect2D::UnionRect(const YkRect2D& rc)
{
	if (rc.IsNull())
	{
		return;
	}
	
	if(IsNull())
	{
		*this = rc;
	}
	else
	{
		left = YKMIN(left, rc.left);
		top = YKMAX(top, rc.top);
		right = YKMAX(right, rc.right);	
		bottom = YKMIN(bottom, rc.bottom);
	}
}

void YkRect2D::Union(const YkPoint2D& pnt)
{
	left = YKMIN(left, pnt.x);
	top = YKMAX(top, pnt.y);
	right = YKMAX(right, pnt.x);
	bottom = YKMIN(bottom, pnt.y);
}

void YkRect2D::UnionPt(const YkPoint2D& pnt)
{
	if (IsNull())
	{
		right = left = pnt.x;
		top = bottom = pnt.y;
	}
	else
	{
		Union(pnt);
	}
}

void YkRect2D::UnionRotate(const YkPoint2D &pntOrg, YkDouble dbAngle)
{
	YkDouble dCosAngle = cos(dbAngle);
    YkDouble dSinAngle = sin(dbAngle);
	
	YkPoint2D pntResults[4];
	pntResults[0].x = left;    pntResults[0].y = top;
    pntResults[1].x = left;    pntResults[1].y = bottom;
    pntResults[2].x = right;   pntResults[2].y = bottom;
    pntResults[3].x = right;   pntResults[3].y = top;
	
	for (YkInt j=0; j<4; j++)
	{
		YkDouble xx = pntResults[j].x - pntOrg.x;
		YkDouble yy = pntResults[j].y - pntOrg.y;
		pntResults[j].x = xx * dCosAngle - yy * dSinAngle + pntOrg.x;
		pntResults[j].y = xx * dSinAngle + yy * dCosAngle + pntOrg.y;
	}
	
	left	= pntResults[0].x;
	right	= pntResults[0].x;
	top		= pntResults[0].y;
	bottom	= pntResults[0].y;
	
	for (YkInt i=1; i<4; i++)
	{
		if (pntResults[i].y > top)
		{
			top = pntResults[i].y;
		}
		else if (pntResults[i].y < bottom)
		{
			bottom = pntResults[i].y;
		}

		if (pntResults[i].x < left)
		{
			left = pntResults[i].x;
		}
		else if (pntResults[i].x > right)
		{
			right = pntResults[i].x;
		}
	}	
}

void YkRect2D::Rotate(const YkPoint2D &pntOrigin, YkDouble dAngle)
{
	if (!YKIS0(dAngle))
	{
		dAngle = -dAngle*3.14159265358979323846264338327950288/180.0;
		YkDouble dCos = cos(dAngle);
		YkDouble dSin = sin(dAngle);

		YkPoint2D pntLast = CenterPoint();
		YkDouble xx = pntLast.x - pntOrigin.x;
		YkDouble yy = pntLast.y - pntOrigin.y;
		pntLast.x = xx * dCos - yy * dSin + pntOrigin.x;
		pntLast.y = xx * dSin + yy * dCos + pntOrigin.y;

		YkDouble cx = pntLast.x - CenterPoint().x;
		YkDouble cy = pntLast.y - CenterPoint().y;
		Offset(cx,cy);
	}
}

YkBool YkRect2D::IntersectRect(const YkRect2D& rc)
{
	if (this->IsIntersect(rc))
	{
		left=YKMAX(left,rc.left);
		top=YKMIN(top,rc.top);
		right=YKMIN(right,rc.right);
		bottom=YKMAX(bottom,rc.bottom);
		
		return TRUE;
	}
	
	return FALSE;
}

YkBool YkRect2D::IntersectRect(const YkRect2D& rc1,const YkRect2D& rc2)
{
	if (rc1.IsIntersect(rc2))
	{
		left=YKMAX(rc1.left,rc2.left);
		top=YKMIN(rc1.top,rc2.top);
		right=YKMIN(rc1.right,rc2.right);
		bottom=YKMAX(rc1.bottom,rc2.bottom);
		
		return TRUE;
	}
	else
	{
		SetEmpty();
	}
	
	return FALSE;
}

YkBool YkRect2D::IsValid() const
{
#ifdef WIN32
	#define YKFINITE(v) _finite(v)
#else
	#define YKFINITE(v) finite(v)
#endif

 	if(YKFINITE(left)!=0 && YKFINITE(right)!=0
		&& YKFINITE(top)!=0 && YKFINITE(bottom)!=0)
	{
		return true;
	}
	return false;
}

void YkRect2D::InflateRect(YkDouble x, YkDouble y)
{
	if(x<0) x = -x;
	if(y<0) y = -y;

	left -= x;
	top += y;
	right += x;
	bottom -= y;
}
}

