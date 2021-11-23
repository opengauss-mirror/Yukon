/*
 *
 * YkDefs.h
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

#if !defined(AFX_YKDEFS_H__EB930378_2B6A_4DDE_891C_41996E06B5EC__INCLUDED_)
#define AFX_YKDEFS_H__EB930378_2B6A_4DDE_891C_41996E06B5EC__INCLUDED_

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>

#include <string>
#include <map>
#include <algorithm>

#include <vector>
#include <list>
#include <iostream>
#include <functional>
#include <set>

#ifdef WIN32
	#pragma warning(disable: 4251)
	#pragma warning(disable: 4275)
	#pragma warning(disable: 4786)
#endif

#ifndef WIN32
	#include <link.h>  // for dlopen() & dlsym()          

// 头文件包含
// 定义宏，使得可以处理大文件（>4GB）
	#undef _FILE_OFFSET_BITS
	#define _FILE_OFFSET_BITS 64
	#include <unistd.h>
	#include <dirent.h>	
	#include <fcntl.h>

	#include <dirent.h> // for opendir(), readdir(), closedir() 
	#include <sys/types.h> // for lstat
	#include <sys/stat.h> // for lstat
	#include <sys/time.h>
	#include <assert.h>
	#include <stddef.h>

	#include <pthread.h>
	#include <semaphore.h>

	#include <unistd.h> // for getwcd()
	#include <dlfcn.h> // for dlopen() & dlsym()
	#include <cstring>
	#include <time.h>
	#include <fstream>
	#include "Base/prefix.h"
#endif

#include "Base/YkExports.h"
#include "Base/YkColorDef.h"

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef NULL
	#define NULL 0
#endif


#ifdef _UGUNICODE
#define _U(x)  L ## x
#else
#define _U(x)	x
#endif
#define YK_DLL_VERSION		_U("")		//! \brief  dll版本号
		      
//! \brief  极小值定义
#define EP 1e-10
#define NEP -1e-10
//!  定义各种类型的极值
#define YK_DBLMAX 1.7976931348623158e+308 /* max value */
#define YK_DBLMIN 2.2250738585072014e-308 /* min positive value */
#define YK_FLTMAX 3.402823466e+38F  /* max value */
#define YK_FLTMIN 1.175494351e-38F  /* min positive value */

// For Windows
#if defined(_DEBUG)
#if !defined(DEBUG)
	#define DEBUG
#endif
#endif

#ifdef _NDEBUG
	#ifndef NDEBUG
	#define NDEBUG
#endif
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "(" __STR1__(__LINE__)") : "

#define CALLBACK    __stdcall
#define YKCALLBACK CALLBACK

#ifdef WIN32
#define YKSTDCALL __stdcall
#else
#define YKSTDCALL
#endif

//! \brief 因为不同平台SYSTEM_WCHAR_SIZE 不同，通过变量确认，
//默认宽度为 2
#if !defined(SYSTEM_WCHAR_SIZE)
#define SYSTEM_WCHAR_SIZE 2
#endif


#ifdef WIN32
#define ISPATHSEP(c) ((c)==_U('/') || (c)==_U('\\'))
#else
#define ISPATHSEP(c) ((c)==_U('/'))
#endif

namespace Yk {

typedef unsigned char          YkUchar;
typedef YkUchar			   YkByte;
typedef YkUchar			   YkBool;
typedef unsigned short         YkUshort;
typedef short                  YkShort;
typedef unsigned int           YkUint;
typedef wchar_t		       YkWchar;
typedef char				   YkAchar;
typedef unsigned int		   YkWchar32;
typedef int                    YkInt;
typedef float                  YkFloat;
typedef double                 YkDouble;

#ifdef _UGUNICODE
typedef YkWchar YkChar;
#else
typedef YkAchar YkChar;
#endif

#if defined(_MSC_VER)
	typedef unsigned __int64       YkUlong;
	typedef __int64                YkLong;	

#elif defined(__GNUG__) || defined(__GNUC__)

	typedef unsigned long long int YkUlong;	//	WhiteBox_Ignore
	typedef long long int          YkLong;	//	WhiteBox_Ignore
	
#endif

typedef YkUint                 YkColor;
typedef size_t                   YkSizeT;


#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#ifndef __FILEW__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __FILEW__ WIDEN(__FILE__)
#endif

//! \brief 坐标与距离单位定义 - 标准方式
//! \brief 毫米
#define AU_MILIMETER	10
//! \brief 厘米			
#define AU_CENTIMETER	100
//! \brief 分米			
#define AU_DECIMETER	1000
//! \brief 米		
#define AU_METER		10000
//! \brief 公里		
#define AU_KILOMETER	10000000
//! \brief 英里	
#define AU_MILE			16090000
//! \brief 英寸??	
#define AU_INCH			254	
//! \brief Yard码		
#define AU_YARD			9144
//! \brief Feet英尺		 
#define AU_FOOT			3048
//! \brief Yard码		
#define	AU_YARD_SEARS   9144		 

//! \remarks 角度单位以 (10e8)分之一弧度为 单位1为避免和长度单位混淆，都加上10e10
//   因此在计算时，应减去10e10，然后再计算
#define AU_DIFF              1000000000
#define AU_RADIAN            (100000000 + 1000000000)	//  Radian                                 
#define AU_DEGREE			 (1745329   + 1000000000)	//  Degree                                 
#define AU_MINUTE            (29089     + 1000000000)	//  Arc-minute                             
#define AU_SECOND            (485       + 1000000000)	//  Arc-second                             
#define AU_GRAD              (1570796   + 1000000000)	//  Grad (angle subtended by 1/400 circle) 
#define AU_MICRORADIAN       (100       + 1000000000)	//  Microradian ( 1e-6 radian )            
#define AU_MINUTE_CENTESIMAL (15708     + 1000000000)	//  Centesimal minute (1/100th Gon (Grad)) 
#define AU_SECOND_CENTESIMAL (157       + 1000000000)	//  Centesimal second(1/10000th Gon (Grad))
#define AU_MIL_6400          (98175     + 1000000000)	//  Mil (angle subtended by 1/6400 circle) 


//! \brief Abolute value
#define YKABS(val) (((val)>=0)?(val):-(val))

//是否为极值
#define YKEXTREFLT(x) (YKABS(YKABS(x)-YK_FLTMAX)<=0.000000001e+38F)
#define YKEXTREDBL(x) (YKABS(YKABS(x)-YK_DBLMAX)<=0.0000000000000001e+308)

//! \brief Return the maximum of a or b
#define YKMAX(a,b) (((a)>(b))?(a):(b))

//! \brief Return the minimum of a or b
#define YKMIN(a,b) (((a)>(b))?(b):(a))


//! \brief Swap a pair of numbers
#define YKSWAP(a,b,t) ((t)=(a),(a)=(b),(b)=(t))

//! \brief Clamp value x to range [lo..hi]
#define YKCLAMP(lo,x,hi) ((x)<(lo)?(lo):((x)>(hi)?(hi):(x)))

#define YKROUND(x) (((x)>0)?YkInt((x)+0.5):(YkInt((x)-0.5)))
//YKQuickRound速度快但是精度范围可能存在多1的问题,处理颜色时不能用这个定义为YKROUND2
#define YKROUND2(x) Yk::YKQuickRound(x)

//! \brief 两个数是否相等的判断
#define YKEQUAL(x,y) Yk::ykequal(x,y)
#define YKEQ(x,y,d) Yk::ykequal(x,y,d)

//! \brief 一个数是否为0 (或者0附近的极小数) 的判断
//! \brief 重要提示: 两个数是否相等的比较 请勿使用 YkIS0(a-b) , 而应该直接使用YkEQUAL(a,b)
#define  YKIS0(x)	(((x)<EP) && ((x)>NEP))

#ifdef DEBUG
	#define YKASSERT(exp) ((exp)?((void)0):(void)Yk::ykassert(#exp,__FILE__,__LINE__))
#else
	#define YKASSERT(exp) ((exp)?((void)0):((void)0))
#endif

/*
   Avoid "unused parameter" warnings
*/
#define YKUNUSED(x) (void)x;

#define YKIsLeadByte(ch) (ch & 0x80)

extern BASE_API void ykassert(const char* pExpression,const char* pFilename,unsigned int nLineno);

extern BASE_API YkBool ykbigendian();

extern BASE_API YkBool ykequal(YkDouble dX, YkDouble dY);

extern BASE_API YkBool ykequal(YkDouble dX, YkDouble dY, YkDouble dTolerance);

//! \brief 颜色相关
extern BASE_API YkColor YKRGB(YkByte chR,YkByte chG,YkByte chB);

extern BASE_API YkColor YKRGBA(YkUchar r,YkUchar g,YkUchar b,YkUchar a);

extern BASE_API YkUchar YKREDVAL(YkColor clr);

extern BASE_API YkUchar YKGREENVAL(YkColor clr);

extern BASE_API YkUchar YKBLUEVAL(YkColor clr);

extern BASE_API YkUchar YKALPHAVAL(YkColor clr);

extern BASE_API YkInt YKQuickRound(YkDouble dX);

#define YK_ISBIGENDIAN ykbigendian()

template<int> struct YkPtrType;
template<>    struct YkPtrType<4> { typedef YkUint Unsigned; typedef YkInt Signed; };
template<>    struct YkPtrType<8> { typedef YkUlong Unsigned; typedef YkLong Signed; };

}

#endif

