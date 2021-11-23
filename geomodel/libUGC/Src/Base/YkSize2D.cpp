/*
 *
 * YkSize2D.cpp
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

#include "Base/YkSize2D.h"

namespace Yk {

YkSize2D::YkSize2D():cx(0.0),cy(0.0)
{
}

YkSize2D::~YkSize2D()
{
}

YkSize2D::YkSize2D(YkDouble dX,YkDouble dY):cx(dX),cy(dY)
{

}

YkBool operator==(const YkSize2D& szSrc,const YkSize2D& sztag)
{
	return YKEQUAL(szSrc.cx,sztag.cx) && YKEQUAL(szSrc.cy,sztag.cy);
}

YkBool operator!=(const YkSize2D& szSrc,const YkSize2D& sztag)
{
	return !YKEQUAL(szSrc.cx,sztag.cx) || !YKEQUAL(szSrc.cy,sztag.cy);
}

YkSize2D& YkSize2D::operator+=(const YkSize2D& sz)
{
	cx+=sz.cx;
	cy+=sz.cy;
	return *this;
}

YkSize2D& YkSize2D::operator-=(const YkSize2D& sz)
{
	cx-=sz.cx;
	cy-=sz.cy;
	return *this;
}

YkSize2D YkSize2D::operator-() const
{
	return YkSize2D(-cx,-cy);
}

YkSize2D operator+(const YkSize2D& szSrc,const YkSize2D& sztag)
{
	return YkSize2D(szSrc.cx+sztag.cx,szSrc.cy+sztag.cy);
}

YkSize2D operator-(const YkSize2D& szSrc,const YkSize2D& sztag)
{
	return YkSize2D(szSrc.cx-sztag.cx,szSrc.cy-sztag.cy);
}

}

