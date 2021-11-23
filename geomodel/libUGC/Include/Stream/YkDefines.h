/*
 *
 * YkDefines.h
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

#ifndef AFX_YKDEFINES_H__DEF00942_80A8_48FC_8054_E78BD1370E3F__INCLUDED_
#define AFX_YKDEFINES_H__DEF00942_80A8_48FC_8054_E78BD1370E3F__INCLUDED_

#pragma once

#include "Base/YkArray.h"
#include "Base/YkList.h"
#include "Base/YkDict.h"
#include "Base/YkRect2D.h"
#include "Base/YkRect.h"
#include "Base/YkSize2D.h"
#include "Base/YkPoint3D.h"
#include "Base/YkSystem.h"
#include "Base/YkTime.h"
#include <climits>
#include <memory>
#include "Stream/YkExports.h"

using namespace Yk;

#define WhiteBox_Ignore

#define YK_MMPERINCH 25.4    //毫米每英寸


//! Pi
#ifndef PI
	#define PI      3.1415926535897932384626433833
#endif

#define TILE_GROUP_SIZE 128
//! Euler constant
#define EULER   2.7182818284590452353602874713

//! Multiplier for degrees to radians
#define DTOR    0.0174532925199432957692369077

//! Multiplier for radians to degrees
#define RTOD    57.295779513082320876798154814

#define UNIT_UNKNOWN	-1   // 未知的单位
#define UNIT_LINEAR		10  // 长度单位
#define UNIT_ANGULAR    20  // 角度单位  

//{{坐标与距离单位定义 - 缩略方式
#define AU_MM	AU_MILIMETER			// mm毫米
#define AU_CM	AU_CENTIMETER			// 厘米
#define AU_DM	AU_DECIMETER			// 分米
#define AU_M	AU_METER				// 米
#define AU_KM	AU_KILOMETER			// 公里
#define AU_MI	AU_MILE					// 英里
#define AU_IN	AU_INCH					// 英寸(好象是这么多)
#define AU_YD	AU_YARD					// Yard码
#define AU_FT	AU_FOOT					// Feet英尺
#define AU_RD	0						// Radian弧度(5.0及其以前版本使用的角度单位，发现是零就当作角度计算了degree)
//}}坐标与距离单位定义


//{{坐标与距离单位定义 - 缩略方式 - MIF字符串
#define AUS_MM	(_U("mm"))	//mm毫米
#define AUS_CM	(_U("cm"))	//厘米
#define AUS_DM	(_U("dm"))	//分米
#define AUS_M	(_U("m"))		//米
#define AUS_KM	(_U("km"))	//公里
#define AUS_MI	(_U("mi"))	//英里
#define AUS_IN	(_U("in"))	//英寸
#define AUS_YD	(_U("yd"))	//Yard码
#define AUS_FT	(_U("ft"))	//Feet英尺
#define AUS_RD  (_U("dg"))    //Radian弧度

// 角度名称的字符串是按照 Arc/info PE取得 
#define AUS_RDN  (_U("Radian"))   
#define AUS_DG	 (_U("Degree"))	
#define AUS_MN	 (_U("Minute"))
#define AUS_SC   (_U("Second"))
#define AUS_GR   (_U("Grad"))          
//#define AUS_GN   ("Gon")           
#define AUS_MR	 (_U("Microradian"))
#define AUS_MC   (_U("Minute_Centesimal"))
#define AUS_SCT   (_U("Second_Centesimal"))
#define AUS_ML6000  (_U("Mil_6400"))		      

//}}坐标与距离单位定义

//{{坐标与距离单位定义 - 标准方式 - MIF字符串
#define AUS_MILIMETER	(_U("mm"))	//mm毫米
#define AUS_CENTIMETER	(_U("cm"))	//厘米
#define AUS_DECIMETER	(_U("dm"))	//分米
#define AUS_METER		(_U("m"))		//米
#define AUS_KILOMETER	(_U("km"))	//公里
#define AUS_MILE		(_U("mi"))	//英里
#define AUS_INCH		(_U("in"))	//英寸
#define AUS_YARD		(_U("yd"))	//Yard码
#define AUS_FOOT		(_U("ft"))	//Feet英尺
#define AUS_DEGREE		(_U("dg"))	//Degree度


#define AUS_RADIAN          (_U("Radian")) 
#define AUS_DEGREE_NEW      (_U("Degree"))      
#define AUS_MINUTE			(_U("Minute")) 	    
#define AUS_SECOND			(_U("Second"))    
#define AUS_GRAD			(_U("Grad"))      
//#define AUS_GON			("Gon")               
#define AUS_MICRORADIAN		        (_U("Microradian"))    
#define AUS_MINUTE_CENTESIMAL		(_U("Minute_Centesimal"))
#define AUS_SECOND_CENTESIMAL		(_U("Second_Centesimal"))
#define AUS_MIL_6400				(_U("Mil_6400"))		      

//}}坐标与距离单位定义
//{{/* Device Parameters for GetDeviceCaps() */
#define Yk_HORZSIZE      4		/* Horizontal size in millimeters           */
#define Yk_VERTSIZE      6     /* Vertical size in millimeters             */
#define Yk_HORZRES       8     /* Horizontal width in pixels               */
#define Yk_VERTRES       10    /* Vertical height in pixels                */
//}}/* Device Parameters for GetDeviceCaps() */

#define Yk_CXFULLSCREEN         16
#define Yk_CYFULLSCREEN         17
// 使用VIRTUAL的宏获取多屏环境下的的虚拟屏幕长宽
// SM_CXVIRTUALSCREEN == 78 SM_CYVIRTUALSCREEN == 79 
// SM_CXSCREEN == 0  SM_CYSCREEN == 1
// 为了适应多屏环境，Yk_C*SCREEN采用VIRTUAL的值
#define Yk_CXSCREEN				78
#define Yk_CYSCREEN				79

#define AM_FIELD_NAME_HEADER_UPPER	(_U("SM"))
#define AM_FIELD_NAME_HEADER		(_U("sm"))

//! 通用的NoValue
#ifndef NOVALUE
	#define NOVALUE -9999
#endif

#define Yk_RAND_MAX RAND_MAX

#define LENGTH_MAX_PATH 2048
#define LENGTH_MAX_DRIVE 5
#define LENGTH_MAX_DIR   1024
#define LENGTH_MAX_TITLE 1024
#define LENGTH_MAX_EXT   8

// 无穷大数
#define  INFINITEBIG Yk::uginfinitebig()


// Checking printf and scanf format strings
#if defined(_CC_GNU_) || defined(__GNUG__) || defined(__GNUC__)
	#define Yk_PRINTF(fmt,arg) __attribute__((format(printf,fmt,arg)))
	#define Yk_SCANF(fmt,arg)  __attribute__((format(scanf,fmt,arg)))
#else
	#define Yk_PRINTF(fmt,arg)
	#define Yk_SCANF(fmt,arg)
#endif

//! \brief  高精度的极小值定义,很多地图的比例都小于1e-10，所以导致出现很多莫名的问题，故此处定义一个高精度
#define HEP 1e-20
#define NHEP -1e-20

#ifdef WIN32
#define YkFINITE(v) _finite(v)
#else
#define YkFINITE(v) finite(v)
#endif

namespace Yk {

#if defined(_MSC_VER)
	#define Yk_LONG
	#define Yk_ULONG_MAX		   _UI64_MAX
	#define Yk_LONG_MIN			   _I64_MIN	
	#define Yk_LONG_MAX			   _I64_MAX
#elif defined(__GNUG__) || defined(__GNUC__)
	#define Yk_LONG
	#define Yk_ULONG_MAX		   18446744073709551615ULL	
	#define Yk_LONG_MAX			   9223372036854775807LL
	#define Yk_LONG_MIN			   (-Yk_LONG_MAX - 1LL)	
#endif

#define YK_UINT_MAX		0xffffffffUL

// Integral types large enough to hold value of a pointer
#if defined(_MSC_VER) && defined(_WIN64)
	typedef __int64                Ykival;
	typedef unsigned __int64       Ykuval;
#else
	typedef long                   Ykival;		//	WhiteBox_Ignore
	typedef unsigned long          Ykuval;		//	WhiteBox_Ignore
#endif

//	专门用来存储指针类型,以解决32位和64位下指针类型长度不一样的问题
typedef Ykuval Ykptr;
typedef YkUint                 YkColor;
typedef YkUint				   YkPixel;


typedef struct YkRGB16  // 16位色彩
{
	YkUshort b : 5;		// 蓝，占5位
	YkUshort g : 6;		// 绿，占6位	
	YkUshort r : 5;		// 红，占5位

}YkRGB16, *LPYkRGB16;

typedef struct YkRGB24  //24位真彩
{
	YkByte blue;		//蓝，占8位
	YkByte green;		//绿，占8位
	YkByte red;			//红，占8位	
}YkRGB24, *LPYkRGB24;

typedef struct YkRGBA32  //32位真彩
{
	YkByte blue;		//蓝，占8位
	YkByte green;		//绿，占8位
	YkByte red;			//红，占8位	
	YkByte alpha;		//alpha，占8位	
}YkRGB32, *LPYkRGB32;

typedef YkArray<YkString> YkStrArray;
typedef YkArray<YkString> YkStrings; 

typedef YkArray<YkPoint2D> YkPoint2Ds;
typedef YkArray<YkPoint3D> YkPoint3Ds;
typedef YkArray<YkPoint> YkPoints;

#ifdef _UGUNICODE
#define  __YkFILE__ __FILEW__ 
#else
#define	 __YkFILE__ __FILE__ 
#endif

//! Return the minimum of x, y and z
#define YKMIN3(x,y,z) ((x)<(y)?YKMIN(x,z):YKMIN(y,z))

//! Return the maximum of x, y and z
#define YKMAX3(x,y,z) ((x)>(y)?YKMAX(x,z):YKMAX(y,z))

// ceil 数值
#define YKCEIL(x) ((YkInt)ceil(x))

// floor 数值
#define YKFLOOR(x) ((YkInt)floor(x))

//获取低位
#define YKLOWORD(l)           ((YkUshort)(l))

//获取高位
#define YKHIWORD(l)           ((YkUshort)(((YkUint)(l) >> 16) & 0xFFFF))

//四舍五入宏
#ifndef ROUNDLONG
	#define ROUNDLONG(x) ((x>0)?YkLong(x+0.5):(YkLong(x-0.5)))	
#endif

// 判断double是否是一个非法值(如：1.#QNAN0000000)  NAN：not a number
#define YKISNAN(x) !YKEQUAL(x,x)

// 与YkIS0(x)功能相同，但是精度高要一倍，判断容限更小
#define  YKHIS0(x)	(((x)<HEP) && ((x)>NHEP))

// 与YkIS0(x)功能相同，但是精度为13位,和iserver保持一致，原来15位还有问题
#define  YKMIS0(x)	(((x)<1e-13) && ((x)>-1e-13))
//! 在Yk中全面禁止使用_T宏的说明
//! 在MFC中,CString在定义MBCS宏时,内部使用的是char*,在定义UNICODE宏时,内部使用的是wchar*;
//! 因此,在给CString赋值时,字符串前都添加_T,以便代码可以在MBCS和UNICODE两种不同的编码方式编译下都可行.
//! SFC是基于MFC开发的,因此也继承了这种使用习惯, 由于Yk很多代码从SFC移植而来, 很多地方也都使用_T宏.
//! 但是, 这种做法在Yk中是不必要,甚至导致问题的. 原因如下: 1)YkString内部限定于char*,使用_T宏起不了作用,
//! 事实上,现在就是#define _U(str) str; 2)系统的很多地方都定义了_T宏,Yk内部再重复定义,会导致编译警告和混乱;
//! 3)在WinCE等系统中,由于其系统内的字符串固定为Unicode,即#define _U(str) L(str), Yk中再定义和使用_T,
//! 要么导致Yk内的字符串错误(前面加了不应该有的L),要么导致系统的字符串错误(_T被取消了)
//! 因此, 最好的做法是:1)在Yk中,不定义_T宏; 2)Yk中的字符串前不添加_T; 3)给YkStringW赋值时,添加L(YkStringW采用wstring实现)
//! 所以, 这里取消Yk中的_T宏(再使用会导致编译错误), 并把Yk中所有_T删除; 另外,请再编写或移植代码的时候注意删除_T


#ifndef NDEBUG
	#define YKTRACE(arguments) Yk::ugtrace arguments
#else
	#define YKTRACE(arguments) ((void)0)
#endif
//! \brief malloc 内存
#define YKMALLOC(ptr,type,no)     (Yk::ugmalloc((void **)(ptr),(YkSizeT)(sizeof(type)*(no))))	
//! \brief calloc 内存
#define YKCALLOC(ptr,type,no)     (Yk::ugcalloc((void **)(ptr),(YkSizeT)(sizeof(type)*(no))))	
//! \brief 重新设置指针内存大小
#define YKRESIZE(ptr,type,no)     (Yk::ugresize((void **)(ptr),(YkSizeT)(sizeof(type)*(no))))
//! \brief 内存拷贝
#define YKMEMDUP(ptr,src,type,no) (Yk::ugmemdup((void **)(ptr),(const void*)(src),(YkSizeT)(sizeof(type)*(no))))
//! \brief 释放空间
#define YKFREE(ptr)               (Yk::ugfree((void **)(ptr)))
//dpi(每英寸象素个数)
#define YKDPI	(Yk::ugdpi())

//! \brief trace内存
extern STREAM_API void ugtrace(const YkChar* format,...);

//! \brief malloc ptr内存
extern STREAM_API YkInt ugmalloc(void** ptr,YkSizeT size);

//! \brief calloc ptr内存
extern STREAM_API YkInt ugcalloc(void** ptr,YkSizeT size);

//! \brief 重新设置指针内存大小
extern STREAM_API YkInt ugresize(void** ptr,YkSizeT size);

//! \brief 内存拷贝
extern STREAM_API YkInt ugmemdup(void** ptr,const void* src,YkSizeT size);

//! \brief 释放空间
extern STREAM_API void ugfree(void** ptr);

//dpi(每英寸象素个数)
extern STREAM_API YkInt ugdpi();

//! \brief 获取当前的最大double数值
extern STREAM_API YkDouble uginfinitebig();

//! \brief linux和windoiws的路径分隔符 '/','\\',在移除分隔符的地方使用
extern STREAM_API YkChar PathSet[];

//! \brief 判断两个颜色是否一
extern STREAM_API YkBool YkISSAMERGB(YkColor color1, YkColor color2);

//! \brief windows上的color（存储标准）转换成当前平台的color，涉及到读取Color的地方都需要用它来转换
extern STREAM_API YkColor YkWinColortoCurrentColor(YkColor rgba);

//! \brief 当前平台的color转换成windows上的color(存储标准)，涉及到存储Color的地方都需要用它来转换
extern STREAM_API YkColor YkCurrentColortoWinColor(YkColor rgba);

//! \brief 当前平台的color转换成windows上的color(存储标准)，涉及到存储Color的地方都需要用它来转换
extern STREAM_API YkColor YkCurrentColortoWinColorRGB(YkColor rgb);

//! \brief 该函数能够将任何POD类型的值改为与之endian相反的对应值。该函数没有检查传入的类型是否为POD！
template<typename T>
inline T YkEndianConvert(const T& value) {
	size_t nSize = sizeof(value);
	T ret = 0;
	for(size_t i = 0; i < nSize; ++i) {
		ret = ret | (((value >> (i * CHAR_BIT)) & 0xff) << (CHAR_BIT * (nSize - 1 - i)));
	}
	return ret;
}

}

#define YKNOTHROW (nothrow)

#endif




