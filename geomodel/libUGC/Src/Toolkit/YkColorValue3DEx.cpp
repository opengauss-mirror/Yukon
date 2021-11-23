/*
 *
 * YkColorValue3DEx.cpp
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

#include "Toolkit/YkColorValue3DEx.h"

namespace  Yk 
{
class YkColorValue3D;

#define YkGetAValue(rgb)      ((YkByte)((rgb)>>24))
#define YkGetRValue(rgb)      ((YkByte)(rgb))
#define YkGetGValue(rgb)      ((YkByte)(((YkUshort)(rgb)) >> 8))
#define YkGetBValue(rgb)      ((YkByte)((rgb)>>16))

YkColorValue3D YkColorValue3D::Black = YkColorValue3D(0.0,0.0,0.0);
YkColorValue3D YkColorValue3D::White = YkColorValue3D(1.0,1.0,1.0);
YkColorValue3D YkColorValue3D::Red = YkColorValue3D(1.0,0.0,0.0);
YkColorValue3D YkColorValue3D::Green = YkColorValue3D(0.0,1.0,0.0);
YkColorValue3D YkColorValue3D::Blue = YkColorValue3D(0.0,0.0,1.0);	
YkColorValue3D YkColorValue3D::Yellow = YkColorValue3D(1.0,1.0,0.0);	

RGBA YkColorValue3D::GetAsLongRGBA(void) const
{
	YkUchar val8;
	YkUint val32 = 0;

	// Red
	val8 = (YkUchar)(r * 255);
	val32 = val8 << 24;

	// Green
	val8 = (YkUchar)(g * 255);
	val32 += val8 << 16;

	// Blue
	val8 = (YkUchar)(b * 255);
	val32 += val8 << 8;

	// Alpha
	val8 = (YkUchar)(a * 255);
	val32 += val8;

	return val32;
}

ARGB YkColorValue3D::GetAsLongARGB(void) const
{
	YkUchar val8;
	YkUint val32 = 0;

	// Alpha
	val8 = (YkUchar)(a * 255);
	val32 = val8 << 24;

	// Red
	val8 = (YkUchar)(r * 255);
	val32 += val8 << 16;

	// Green
	val8 = (YkUchar)(g * 255);
	val32 += val8 << 8;

	// Blue
	val8 = (YkUchar)(b * 255);
	val32 += val8;

	return val32;
}

ABGR YkColorValue3D::GetAsLongABGR(void) const
{
	YkUchar val8;
	YkUint val32 = 0;

	// Alpha
	val8 = (YkUchar)(a * 255);
	val32 = val8 << 24;

	// Blue
	val8 = (YkUchar)(b * 255);
	val32 += val8 << 16;

	// Green
	val8 = (YkUchar)(g * 255);
	val32 += val8 << 8;

	// Red
	val8 = (YkUchar)(r * 255);
	val32 += val8;

	return val32;
}
ABGR YkColorValue3D::GetAsCOLORREF(void)const
{
	YkUchar val8;
	YkUint val32 = 0;

	// Blue
	val8 = (YkUchar)(b * 255);
	val32 += val8 << 16;

	// Green
	val8 = (YkUchar)(g * 255);
	val32 += val8 << 8;

	// Red
	val8 = (YkUchar)(r * 255);
	val32 += val8;

	return val32;
}

YkBool YkColorValue3D::operator==(const YkColorValue3D& rhs) const
{
	return (YKEQUAL(r,rhs.r) &&
	 YKEQUAL(g,rhs.g) &&
	 YKEQUAL(b,rhs.b) &&
	 YKEQUAL(a,rhs.a));
}

YkBool YkColorValue3D::operator!=(const YkColorValue3D& rhs) const
{
	return !(*this == rhs);
}

YkColorValue3D::YkColorValue3D(YkUint color)
{
	r = YkDouble(YkGetRValue(color))/YkDouble(255);
	g = YkDouble(YkGetGValue(color))/YkDouble(255);
	b = YkDouble(YkGetBValue(color))/YkDouble(255);
	a = YkDouble(YkGetAValue(color))/YkDouble(255);
}

void YkColorValue3D::SetValue(YkInt nRed,YkInt nGreen,YkInt nBlue,YkInt nAlpha)
{
	r=nRed/255.0;
	g=nGreen/255.0;
	b=nBlue/255.0;
	a=nAlpha/255.0;
}
void YkColorValue3D::SetValue(YkDouble dRed,YkDouble dGreen,YkDouble dBlue,YkDouble dAlpha)
{
	r=dRed;
	g=dGreen;
	b=dBlue;
	a=dAlpha;

}

YkColorValue3D YkColorValue3D::GetReverseColor() const 
{
	return YkColorValue3D((1.0f-r), (1.0f-g), (1.0f-b), a);
}

void YkColorValue3D::GetRGBA(YkInt& nRed,YkInt& nGreen,YkInt& nBlue,YkInt& nAlpha) const
{
	nRed=(YkInt)ROUNDLONG(255.0*r);
	nGreen=(YkInt)ROUNDLONG(255.0*g);
	nBlue=(YkInt)ROUNDLONG(255.0*b);
	nAlpha=(YkInt)ROUNDLONG(255.0*a);
}
void YkColorValue3D::FromAsABGR(YkInt color)
{	
	r = ((YkByte)color)/255.0;
	g = ((YkByte)(color>>8))/255.0;
	b = ((YkByte)(color>>16))/255.0;
	a = ((YkByte)(color>>24))/255.0;
}
void YkColorValue3D::SetValue(YkColor color)
{
	r=YKREDVAL(color)/255.0;
	g=YKGREENVAL(color)/255.0;
	b=YKBLUEVAL(color)/255.0;
	a=YKALPHAVAL(color)/255.0;
}
YkColor YkColorValue3D::GetValue() const
{
	return YKRGBA(YkUchar(r*255),YkUchar(g*255),YkUchar(b*255),YkUchar(a*255));
}
YkColor YkColorValue3D::ABGRToColor(YkInt color)
{   
    return (((YkByte)color)<<24) + ((YkByte)(color>>8)<<16) + ((YkByte)(color>>16)<<8) + ((YkByte)(color>>24));
}

}
