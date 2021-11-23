/*
 *
 * YkRect.cpp
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

#include "Base/YkRect.h"

namespace Yk {

YkRect::YkRect()
:left(0),top(0),right(0),bottom(0)
{
}

YkRect::~YkRect()
{
}

YkRect::YkRect(YkInt nLeft,YkInt nTop,YkInt nRight,YkInt nBottom)
:left(nLeft),top(nTop),right(nRight),bottom(nBottom)
{

}

YkRect::YkRect(const YkPoint& pnt,const YkSize& sz)
:left(pnt.x),top(pnt.y),right(pnt.x+sz.cx),bottom(pnt.y+sz.cy)
{

}

YkRect::YkRect(const YkPoint& pntTL,const YkPoint& pntBR)
:left(pntTL.x),top(pntTL.y),right(pntBR.x),bottom(pntBR.y)
{

}

YkRect& YkRect::operator=(const YkRect& rc)
{
    if( this == &rc)
		return *this;
	left   =rc.left;
	top    =rc.top;
    right  =rc.right;
	bottom =rc.bottom;
    return *this;
}

YkBool operator==(const YkRect& rcStart,const YkRect& rcEnd)
{
	return rcStart.left==rcEnd.left && rcStart.top==rcEnd.top &&
		rcStart.right==rcEnd.right && rcStart.bottom==rcEnd.bottom;
}

YkBool operator!=(const YkRect& rcStart,const YkRect& rcEnd)
{
	return rcStart.left!=rcEnd.left || rcStart.top!=rcEnd.top ||
		rcStart.right!=rcEnd.right || rcStart.bottom!=rcEnd.bottom;
}

void YkRect::SetRect(YkInt nLeft,YkInt nTop,YkInt nRight,YkInt nBottom)
{
	left=nLeft;
	top=nTop;
	right=nRight;
	bottom=nBottom;
}

void YkRect::SetRect(const YkPoint& pntTL,const YkPoint& pntBR)
{
	SetRect(pntTL.x,pntTL.y,pntBR.x,pntBR.y);
}

void YkRect::SetEmpty()
{
	left=top=right=bottom=0;
}

YkBool YkRect::IsEmpty() const
{
	return left>=right || top>=bottom;
}

YkBool YkRect::IsNull() const
{
	return left==0 && top==0 
		&& right==0 && bottom==0;
}

YkInt YkRect::Width() const	
{
	return right-left;
}

YkInt YkRect::Height() const
{
	return bottom-top;
}

YkPoint& YkRect::TopLeft()
{
	return *((YkPoint*)this);
}

const YkPoint& YkRect::TopLeft() const
{
	return *((YkPoint*)this);
}

YkPoint& YkRect::BottomRight()
{
	return *((YkPoint*)this+1);
}

const YkPoint& YkRect::BottomRight() const
{
	return *((YkPoint*)this+1);
}

YkPoint YkRect::CenterPoint() const
{
	return YkPoint((left+right)/2,(top+bottom)/2);
}

YkSize YkRect::Size() const
{
	return YkSize(right-left,bottom-top);
}

void YkRect::SwapLeftRight()
{
	YkInt tmp=0;
	YKSWAP(left,right,tmp);
}

void YkRect::SwapTopBottom()
{
	YkInt tmp=0;
	YKSWAP(top,bottom,tmp);
}

YkBool YkRect::PtInRect(const YkPoint& pnt) const
{
	return (pnt.x>=left)	&& (pnt.x<=right)
		&& (pnt.y>=top)	&& (pnt.y<=bottom);
}

YkBool YkRect::Contains(const YkRect& rc) const
{
	return left<=rc.left && top<=rc.top &&
		right>=rc.right && bottom>=rc.bottom;
}

YkBool YkRect::Withins(const YkRect& rc) const
{
	return left>=rc.left && top>=rc.top &&
		right<=rc.right && bottom<=rc.bottom;
}

YkBool YkRect::IsIntersect(const YkRect& rc) const
{
	return right>=rc.left && left<=rc.right &&
		bottom>=rc.top && top<=rc.bottom;
}

void YkRect::Normalize()
{
	YkInt tmp=0;
	if(left>right) YKSWAP(left,right,tmp);
	if(top>bottom) YKSWAP(top,bottom,tmp);	
}

void YkRect::Offset(YkInt x,YkInt y)
{
	left    += x;
	top		+= y;
	right	+= x;
	bottom	+= y;
}

void YkRect::Offset(const YkSize& sz)
{
	Offset(sz.cx,sz.cy);
}

void YkRect::Offset(const YkPoint& pnt)
{
	Offset(pnt.x,pnt.y);
}

void YkRect::Inflate(YkInt nMargin)
{
	left	    -= nMargin;
	top		-= nMargin;
	right	+= nMargin;
	bottom	+= nMargin;
}

void YkRect::Inflate(YkInt nHorMargin,YkInt nVerMargin)
{
	left	    -= nHorMargin;
	top		-= nVerMargin;
	right	+= nHorMargin;
	bottom	+= nVerMargin;
}

void YkRect::Inflate(YkInt nLeftMargin,YkInt nTopMargin,YkInt nRightMargin,YkInt nBottomMargin)
{
	left	    -= nLeftMargin;
	top		-= nTopMargin;
	right	+= nRightMargin;
	bottom	+= nBottomMargin;
}

void YkRect::Deflate(YkInt nMargin)
{
	Inflate(-nMargin);
}

void YkRect::Deflate(YkInt nHorMargin,YkInt nVerMargin)
{
	Inflate(-nHorMargin,-nVerMargin);
}

void YkRect::Deflate(YkInt nLeftMargin,YkInt nTopMargin,YkInt nRightMargin,YkInt nBottomMargin)
{
	Inflate(-nLeftMargin,-nTopMargin,-nRightMargin,-nBottomMargin);
}

void YkRect::Union(const YkRect& rc)
{
	left	    = YKMIN(left,rc.left);
	top		= YKMIN(top,rc.top);
	right	= YKMAX(right,rc.right);	
	bottom	= YKMAX(bottom,rc.bottom);
}

void YkRect::Union(const YkPoint& pnt)
{
	left	    = YKMIN(left,pnt.x);
	top		= YKMIN(top,pnt.y);
	right	= YKMAX(right,pnt.x);
	bottom	= YKMAX(bottom,pnt.y);
}

void YkRect::Intersection(const YkRect& rc)
{
	left	    = YKMAX(left,rc.left);
	top		= YKMAX(top,rc.top);
	right	= YKMIN(right,rc.right);
	bottom	= YKMIN(bottom,rc.bottom);
}

}



