/*
 *
 * YkDefs.cpp
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

#include "Base/YkDefs.h"
#include "Base/YkString.h"

#ifdef WIN32
	#include <windows.h>
#endif

namespace Yk {

void ykassert(const char* expression,const char* filename,unsigned int lineno)
{
	fprintf(stderr,"%s:%d: YKASSERT(%s) failed.\n",filename,lineno,expression);
#if defined(_M_IX86) 
	__asm{int 3}
#endif
	return;
}

YkBool ykequal(YkDouble dValue1, YkDouble dValue2)
{
	if (YKIS0(dValue1)) {
		return YKIS0(dValue2);
	} else if (YKIS0(dValue2)) {
		return YKIS0(dValue1);
	}
	double dMaxValue = YKMAX(YKABS(dValue1),YKABS(dValue2));
	double dAbsolute = dValue1 - dValue2;
	
	return ((dAbsolute>=(dMaxValue*NEP)) && (dAbsolute<=(dMaxValue*EP)));
}

YkBool ykequal(YkDouble dValue1, YkDouble dValue2, YkDouble dTolerance)
{
	if (dTolerance > 0)
	{
		YkDouble dTemp = dValue1-dValue2;
		return  ((dTemp>(-dTolerance)) && (dTemp<dTolerance));
	}
	return FALSE;
}

YkColor YKRGB(YkUchar r,YkUchar g,YkUchar b)
{
	YkColor nValue = 0;
	if(YK_ISBIGENDIAN)
	{
		nValue =  (((YkUint)(YkUchar)(r)<<24) | ((YkUint)(YkUchar)(g)<<16) | ((YkUint)(YkUchar)(b)<<8) | 0x000000ff);	
	}
	else
	{
		nValue =  (((YkUint)(YkUchar)(r)) | ((YkUint)(YkUchar)(g)<<8) | ((YkUint)(YkUchar)(b)<<16) | 0xff000000);
	}

	return nValue;
}

YkColor YKRGBA(YkUchar r,YkUchar g,YkUchar b,YkUchar a)
{
	YkColor nValue = 0;
	if(YK_ISBIGENDIAN)
	{
		nValue =  (((YkUint)(YkUchar)(r)<<24) | ((YkUint)(YkUchar)(g)<<16) | ((YkUint)(YkUchar)(b)<<8) | ((YkUint)(YkUchar)(a)));
	}
	else
	{
		nValue =  (((YkUint)(YkUchar)(r)) | ((YkUint)(YkUchar)(g)<<8) | ((YkUint)(YkUchar)(b)<<16) | ((YkUint)(YkUchar)(a)<<24));
	}
	return nValue;
}

YkUchar YKREDVAL(YkColor rgba)
{
	YkUchar nValue= 0;
	if(YK_ISBIGENDIAN)
	{
		nValue = ((YkUchar)(((rgba)>>24)&0xff));
	}
	else
	{
		nValue = ((YkUchar)((rgba)&0xff));
	}
	return nValue;
}

YkUchar YKGREENVAL(YkColor rgba)
{
	YkUchar nValue =0;
	if(YK_ISBIGENDIAN)
	{
		nValue = ((YkUchar)(((rgba)>>16)&0xff));
	}
	else
	{
		nValue = ((YkUchar)(((rgba)>>8)&0xff));
	}
	return nValue;
}

YkUchar YKBLUEVAL(YkColor rgba)
{
	YkUchar nValue =0;
	if(YK_ISBIGENDIAN)
	{
		nValue = ((YkUchar)(((rgba)>>8)&0xff));
		
	}
	else
	{
		nValue = ((YkUchar)(((rgba)>>16)&0xff));
		
	}
	return nValue;
}

YkUchar YKALPHAVAL(YkColor rgba)
{
	YkUchar nValue =0;
	if(YK_ISBIGENDIAN)
	{
		nValue = ((YkUchar)(((rgba))&0xff));
	}
	else
	{
		nValue = ((YkUchar)(((rgba)>>24)&0xff));
	}
	return nValue;
}

YkInt YKQuickRound(YkDouble dX)
{		
	if(YK_ISBIGENDIAN)
	{
		return (((dX)>0)?YkInt((dX)+0.5):(YkInt((dX)-0.5)));
	}
	
	YkFloat x = (YkFloat)dX;
	void * pTemp = &x;
	YkInt *pIntTemp = (YkInt*)pTemp;
	YkInt nReturn = 0;
	if (x < 0)
	{
		x = -x + 0.5f;		
		nReturn = ((*pIntTemp & 0x007FFFFF) | 0x00800000);	
		YkInt nOffset = (150 -  ((*pIntTemp & 0x7F800000) >> 23));
		if(nOffset < 0)
		{
			return (((dX)>0)?YkInt((dX)+0.5):(YkInt((dX)-0.5)));
		}
		nReturn >>= nOffset;
		nReturn = -nReturn;		
	}
	else
	{	
		x = x + 0.5f;			
		nReturn = ((*pIntTemp & 0x007FFFFF) | 0x00800000);	
		YkInt nOffset = (150 -  ((*pIntTemp & 0x7F800000) >> 23));
		if(nOffset < 0)
		{
			return (((dX)>0)?YkInt((dX)+0.5):(YkInt((dX)-0.5)));
		}
		nReturn >>= nOffset;
	}	
	return nReturn;
}

YkBool ykbigendian()
{
	static union
	{
		int nTest;
		char cTest[sizeof(int)];
	}un_bigendina = { 1 };

	return un_bigendina.cTest[0] != 1;
}

}



