/*
 *
 * YkDefines.cpp
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

#include "Stream/YkDefines.h"
#include "Base/YkPlatform.h"
#include "Stream/YkPlatform.h"

#ifdef WIN32
	#include <windows.h>
#endif

namespace Yk 
{

	YkChar PathSet[] = {_U('/'), _U('\\')};


void ugtrace(const YkChar* format,...){	
#ifndef WIN32
	va_list arguments;
	va_start(arguments,format);
	Ykvfprintf(stderr,format,arguments);
	va_end(arguments);
#else
	YkString msg;
	int n = (int)Ykstrlen(format)+128;
    va_list arguments;
    va_start(arguments,format);

x:	msg.SetLength(n);
#ifdef _UGUNICODE
	int len = YK_vsnprintf((YkChar*)msg.Cstr(), n, format, arguments); 
#else
	int len = vsnprintf((YkChar*)msg.Cstr(), n, format,arguments);
#endif
    if(len<0)
	{ 
		n<<=1; 
		goto x; 
	}
    if(n<len)
	{ 
		n=len; 
		goto x; 
	}
	msg.SetLength(len);

    va_end(arguments);
    OutputDebugString(msg.Cstr());
#endif
}

YkInt ugmalloc(void** ptr,YkSizeT size){	
	*ptr=NULL;
	if(size!=0){
		if((*ptr=malloc(size))==NULL) return FALSE;
	}
	return TRUE;
}

YkInt ugcalloc(void** ptr,YkSizeT size){
	*ptr=NULL;
	if(size!=0){
		if((*ptr=calloc(size,1))==NULL) return FALSE;
	}
	return TRUE;
}

YkInt ugresize(void** ptr,YkSizeT size){
	void *p=NULL;
	if(size!=0){
		if((p=realloc(*ptr,size))==NULL) return FALSE;
    }
	else{
		if(*ptr){ 
			free(*ptr);
			*ptr=NULL;
		}
    }
	*ptr=p;
	return TRUE;
  }

YkInt ugmemdup(void** ptr,const void* src,YkSizeT size){
	*ptr=NULL;
	if(size!=0 && src!=NULL){
		if((*ptr=malloc(size))==NULL) return FALSE;
		memcpy(*ptr,src,size);
	}
	return TRUE;
}

void ugfree(void** ptr){
	if(*ptr){
		free(*ptr);
		*ptr=NULL;
    }
}


YkBool ugequal(YkDouble dValue1, YkDouble dValue2)
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

YkBool ugequal(YkDouble dValue1, YkDouble dValue2, YkDouble dTolerance)
{
	if (dTolerance > 0)
	{
		YkDouble dTemp = dValue1-dValue2;
		return  ((dTemp>(-dTolerance)) && (dTemp<dTolerance));
	}
	return FALSE;
}



// 下面几个函数基于一个事实: double 数值采用的是 IEEE 的标准
// 如果某编译器不是该标准,请 重写下面的函数
YkDouble uginfinitebig()
{
	YkDouble dInfiniteBig = 0;
	YkByte* pc = (YkByte*)&dInfiniteBig;
	if(!YK_ISBIGENDIAN)
	{
		pc[7] = 0x7f;
		pc[6] = 0xf0;
	}
	else
	{
		pc[0] = 0x7f;
		pc[1] = 0xf0;
	}
	
	return dInfiniteBig;
}

YkColor YkWinColortoCurrentColor(YkColor rgba)
{
	YkByte red = ((YkByte)((rgba)&0xff));
	YkByte green = ((YkByte)(((rgba)>>8)&0xff));
	YkByte blue = ((YkByte)(((rgba)>>16)&0xff));
	YkByte alpha = ((YkByte)(((rgba)>>24)&0xff));
	YkColor nValue = YKRGBA(red,green,blue, alpha);
	return nValue;
}

YkColor YkCurrentColortoWinColor(YkColor rgba)
{
	YkByte red = YKREDVAL(rgba);
	YkByte green = YKGREENVAL(rgba);
	YkByte blue = YKBLUEVAL(rgba);
	YkByte alpha = YKALPHAVAL(rgba);
	YkColor nValue =  (((YkUint)(YkByte)(red)) | ((YkUint)(YkByte)(green)<<8)
		| ((YkUint)(YkByte)(blue)<<16) | ((YkUint)(YkByte)(alpha)<<24));
	return nValue;
}

YkColor YkCurrentColortoWinColorRGB(YkColor rgb)
{
	YkByte red = YKREDVAL(rgb);
	YkByte green = YKGREENVAL(rgb);
	YkByte blue = YKBLUEVAL(rgb);
	YkColor nValue =  (((YkUint)(YkByte)(red)) | ((YkUint)(YkByte)(green)<<8)
		| ((YkUint)(YkByte)(blue)<<16));
	return nValue;
}

YkBool YkISSAMERGB(YkColor color1, YkColor color2)
{
	if(YK_ISBIGENDIAN)
	{
		if((color1 >> 8) == (color2 >> 8))
		{
			return true;
		}
	}
	else
	{
		if((color1 << 8) == (color2 << 8))
		{
			return true;
		}
	}
	return false;
}

}



