/*
 *
 * YkMathEngine.cpp
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

#include "Base3D/YkMathEngine.h"
#include "Base3D/YkVector3d.h"
#include "Base3D/YkBoundingBox.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkVector4d.h"
#include "Base3D/YkTextureData.h"

namespace Yk {
SceneType YkMathEngine::m_msSceneType = ST_EARTH_SPHERICAL;

YkMathEngine::YkMathEngine()
{
}

YkMathEngine::~YkMathEngine()
{

}

YkVector3d YkMathEngine::SphericalToCartesian(YkDouble dLongitude, YkDouble dLatitude,YkDouble dRadius)
{
	return SphericalToCartesian(YkMathEngine::m_msSceneType, dLongitude, dLatitude, dRadius);
}

YkVector3d YkMathEngine::SphericalToCartesian(SceneType sceneType, YkDouble dLongitude, YkDouble dLatitude,YkDouble dRadius)
{
	switch (sceneType)
	{
	case ST_EARTH_SPHERICAL:
		{
			YkDouble dRadCosLat = dRadius * (YkDouble)cos(dLatitude);
			return YkVector3d((YkDouble)(dRadCosLat*sin(dLongitude)),
				(YkDouble)(dRadius*sin(dLatitude)),
				(YkDouble)(dRadCosLat*cos(dLongitude)));
		}
		break;
	case ST_NONEARTH:
		{
			return YkVector3d(dLongitude,dLatitude,dRadius); 
		}
		break;
	default:
		return YkVector3d(dLongitude,dLatitude,dRadius); 
	}
}

YkVector3d YkMathEngine::CartesianToSphericalD(YkDouble x, YkDouble y, YkDouble z)
{	
	return CartesianToSphericalD(x, y, z, YkMathEngine::m_msSceneType);
}
YkVector3d YkMathEngine::CartesianToSphericalD(YkDouble x, YkDouble y, YkDouble z, SceneType sceneType)
{
	switch (sceneType)
	{
	case ST_EARTH_SPHERICAL:
		{
			YkDouble rho = sqrt(x * x + y * y + z * z);
			if (YKIS0(rho))
			{
				return YkVector3d::ZERO;
			}
			YkDouble longitude = atan2(x, z);
			YkDouble latitude = asin(y / rho);

			return YkVector3d(longitude,latitude,rho);
		}
		break;
	case ST_NONEARTH:
		{
			return YkVector3d(x,y,z); 
		}
		break;
	default:
		return YkVector3d(x,y,z); 
	}
}

YkVector3d YkMathEngine::SphericalToGlobalCartesian(YkDouble dLongitude, YkDouble dLatitude, YkDouble dHigh, YkDouble dRadius/* = GLOBAL_RADIUS*/)
{
	return SphericalToCartesian(ST_EARTH_SPHERICAL, dLongitude * DTOR, dLatitude * DTOR, dHigh + dRadius);
}


YkUint YkMathEngine::NextP2(YkUint a)
{
	YkUint rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

//对齐处理
YkBool YkMathEngine::ClipImageData(YkInt components, YkInt width, YkInt height,
									  const YkUchar *datain, const YkInt ndatainsize,YkUchar *dataout)
{
	// 目前只有RGB格式需要对数据进行对其处理
	if (components != 3)
	{
		return FALSE;
	}

	if(ndatainsize<0 || ndatainsize<=components * width * height)
	{
		return FALSE;
	}

	// 计算紧凑数据的宽度，获得宽度差
	YkInt i, j;
	YkInt widthData = WIDTHBYTES(width * IPF_RGB_BITS);
	YkInt widthOffset = widthData - width * 3;		
	if( widthOffset == 0 )
	{
		return FALSE;
	}

	for (i = 0; i < height; i++) 
	{
		YkInt nLineWidth = i * width * 3;
		YkInt nLineWidthOld = i * widthData;
		for (j = 0; j < width; j++) 
		{
			dataout[nLineWidth + 3 * j] = datain[nLineWidthOld + 3 * j];
			dataout[nLineWidth + 3 * j + 1] = datain[nLineWidthOld + 3 * j + 1];
			dataout[nLineWidth + 3 * j + 2] = datain[nLineWidthOld + 3 * j + 2];
		}
	}

	return TRUE;
}

void YkMathEngine::ScaleImageInternal(YkInt components, YkInt widthin, YkInt heightin,
						   const YkUchar *datain,
						   YkInt widthout, YkInt heightout,
						   YkUchar *dataout,const YkInt nsizedatain)
{
	YkFloat x, lowx, highx, convx, halfconvx;
	YkFloat y, lowy, highy, convy, halfconvy;
	YkFloat xpercent,ypercent;
	YkFloat percent;
	YkFloat totals[4];
	YkFloat area;
	YkInt i,j,k,yint,xint,xindex,yindex;
	YkInt temp;
	YkUchar* pData = (YkUchar*)datain;
	YkBool bDirty = FALSE;	

	if (widthin == widthout*2 && heightin == heightout*2) {
		HalveImage(components, widthin, heightin, pData, dataout);
		return;
	}

 	if(components == 3)
 	{
 		pData = new YkUchar[widthin * heightin * 3];
 		bDirty = TRUE;
 		if( !ClipImageData(components, widthin, heightin, datain, nsizedatain, pData) )
 		{
				delete[] pData;
				bDirty = FALSE;
 			pData = (YkUchar*)datain;
 		}
 	}

	convy = (YkFloat) heightin/heightout;
	convx = (YkFloat) widthin/widthout;
	halfconvx = convx/2;
	halfconvy = convy/2;
	for (i = 0; i < heightout; i++) {
		y = convy * (i+0.5);
		if (heightin > heightout) {
			highy = y + halfconvy;
			lowy = y - halfconvy;
		} else {
			highy = y + 0.5;
			lowy = y - 0.5;
		}
		for (j = 0; j < widthout; j++) {
			x = convx * (j+0.5);
			if (widthin > widthout) {
				highx = x + halfconvx;
				lowx = x - halfconvx;
			} else {
				highx = x + 0.5;
				lowx = x - 0.5;
			}

			/*
			** Ok, now apply box filter to box that goes from (lowx, lowy)
			** to (highx, highy) on input data into this pixel on output
			** data.
			*/
			totals[0] = totals[1] = totals[2] = totals[3] = 0.0;
			area = 0.0;

			y = lowy;
			yint = floor(y);
			while (y < highy) {
				yindex = (yint + heightin) % heightin;
				if (highy < yint+1) {
					ypercent = highy - y;
				} else {
					ypercent = yint+1 - y;
				}

				x = lowx;
				xint = floor(x);

				while (x < highx) {
					xindex = (xint + widthin) % widthin;
					if (highx < xint+1) {
						xpercent = highx - x;
					} else {
						xpercent = xint+1 - x;
					}

					percent = xpercent * ypercent;
					area += percent;
					temp = (xindex + (yindex * widthin)) * components;
					for (k = 0; k < components; k++) {
						totals[k] += pData[temp + k] * percent;
					}

					xint++;
					x = xint;
				}
				yint++;
				y = yint;
			}

			temp = (j + (i * widthout)) * components;
			for (k = 0; k < components; k++) {
				/* totals[] should be rounded in the case of enlarging an RGB
				* ramp when the type is 332 or 4444
				*/
				dataout[temp + k] = (totals[k]+0.5)/area;
			}
		}
	}

	if(bDirty)
	{
		delete[] pData;
	}
}

void YkMathEngine::HalveImage(YkInt components, YkUint width, YkUint height,
					   const YkUchar *datain, YkUchar *dataout)
{
	YkInt i, j, k;
	YkInt newwidth, newheight;
	YkInt delta;
	YkUchar *s;
	const YkUchar *t;

	newwidth = width / 2;
	newheight = height / 2;
	delta = width * components;
	s = dataout;
	t = datain;

	for (i = 0; i < newheight; i++) {
		for (j = 0; j < newwidth; j++) {
			for (k = 0; k < components; k++) {
				s[0] = (t[0] + t[components] + t[delta] +
					t[delta+components] + 2) / 4;
				s++; t++;
			}
			t += components;
		}
		t += delta;
	}
}

}
