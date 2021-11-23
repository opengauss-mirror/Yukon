/*
 *
 * YkSize.cpp
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

#include "Base/YkSize.h"

namespace Yk {

YkSize::YkSize():cx(0),cy(0)
{	
}

YkSize::~YkSize()
{	
}

YkSize::YkSize(YkInt nX,YkInt nY):cx(nX),cy(nY)
{

}

YkBool operator==(const YkSize& szSrc,const YkSize& szTag)
{
	return szSrc.cx==szTag.cx && szSrc.cy==szTag.cy;
}

YkBool operator!=(const YkSize& szSrc,const YkSize& szTag)
{
	return szSrc.cx!=szTag.cx || szSrc.cy!=szTag.cy; 
}

YkSize& YkSize::operator+=(const YkSize& sz)
{
	cx+=sz.cx;
	cy+=sz.cy;
	return *this;
}

YkSize& YkSize::operator-=(const YkSize& sz)
{
	cx-=sz.cx; 
	cy-=sz.cy;
	return *this; 
}

YkSize YkSize::operator-() const
{
	return YkSize(-cx,-cy);
}

YkSize operator+(const YkSize& szSrc,const YkSize& szTag)
{
	return YkSize(szSrc.cx+szTag.cx,szSrc.cy+szTag.cy); 
}

YkSize operator-(const YkSize& szSrc,const YkSize& szTag)
{
	return YkSize(szSrc.cx-szTag.cx,szSrc.cy-szTag.cy);
}

}



