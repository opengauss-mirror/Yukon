/*
 *
 * YkTime.h
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

#if !defined(AFX_YKTIME_H__F8775450_4889_4D7E_A042_7E61AF93876D__INCLUDED_)
#define AFX_YKTIME_H__F8775450_4889_4D7E_A042_7E61AF93876D__INCLUDED_

#pragma once

#ifdef WIN32
/* This needs to come before any includes for MSVC compiler */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "YkString.h"

namespace Yk {
//! \brief 时间间隔类
class BASE_API YkTimeSpan 
{
public:
	//! \brief 默认构造函数,时间间隔为0
	YkTimeSpan();
	//! \brief 指定时间间隔进行构造
	YkTimeSpan(YkInt nDays, YkInt nHours, YkInt nMins, YkInt nSecs);
	//! \brief 指定double时间进行构造
	YkTimeSpan(double dTime);
	
	//! \brief 拷贝构造函数
	YkTimeSpan(const YkTimeSpan& tmsSrc);
	//! \brief 拷贝赋值函数
	const YkTimeSpan& operator=(const YkTimeSpan& tmsSrc);
	
	//! \brief 获取日
	YkInt GetDays() const;
	//! \brief 获取小时,日转化为24小时
	YkInt GetTotalHours() const;
	//! \brief 获取小时,日不计算在内
	YkInt GetHours() const;
	//! \brief 获取分钟,日和时都转化为分
	YkInt GetTotalMinutes() const;
	//! \brief 获取分钟,日和时不计算在内
	YkInt GetMinutes() const;
	//! \brief 获取秒,日时分都转化为秒
	YkInt GetTotalSeconds() const;
	//! \brief 获取秒,日时分不计算在内
	YkInt GetSeconds() const;

	//! \brief 获取时间间隔
	double GetTimeSpan() const;
	
	//! \brief 重载操作符 -
	YkTimeSpan operator-(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 +
	YkTimeSpan operator+(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 +=
	const YkTimeSpan& operator+=(const YkTimeSpan &tms);
	//! \brief 重载操作符 -=
	const YkTimeSpan& operator-=(const YkTimeSpan &tms);
	//! \brief 重载操作符 ==
	YkBool operator==(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 !=
	YkBool operator!=(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 <
	YkBool operator<(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 >
	YkBool operator>(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 <=
	YkBool operator<=(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 >=
	YkBool operator>=(const YkTimeSpan &tms) const;
		
private:
	//! \brief 时间间隔（天数）
	double m_timeSpan; 
	//! \brief 时间类
	friend class YkTime;
};

//! \brief 时间类
// 参照COleDateTime实现
// implemented as a floating-point value, measuring days from midnight, 
// 30 December 1899. So, midnight, 31 December 1899 is represented by 1.0. 
// Similarly, 6 AM, 1 January 1900 is represented by 2.25, 
// and midnight, 29 December 1899 is – 1.0. 
// However, 6 AM, 29 December 1899 is – 1.25.
// Now, YkTime can handle dates from 1 January 100 – 31 December 9999
class BASE_API YkTime  
{
public:
	//! \brief 获取以毫秒为计数的时间; 用来精确测算性能等; 
	static YkUint GetMillisecond();
	
	//! \brief 默认构造函数
	YkTime();

	//! \brief 拷贝构造函数
	YkTime(const YkTime& tmSrc);

	//! \brief 拷贝构造函数
	YkTime(time_t t);

	//! \brief 拷贝构造函数
	YkTime(double dValue);
	
	//! \brief 年月日构造函数
	YkTime(YkInt nYear,YkInt nMonth,YkInt nDay);
	
	//! \brief 年月日时分秒构造函数
	YkTime(YkInt nYear, YkInt nMonth, YkInt nDay, YkInt nHour, YkInt nMin, YkInt nSec);
	//! \brief 重载操作符=
	const YkTime& operator=(const YkTime& tmSrc);
	//! \brief 重载操作符=
	const YkTime& operator=(time_t t);

	//! \brief 获取时间
	double GetTime() const;

	//! \brief 获取年(公元)。
	YkInt GetYear() const;	
	//! \brief 获取一年中月数(1~12)
	//! \remarks 1月=1,2月=2,...,12月=12.
	YkInt GetMonth() const;
	//! \brief 获取每月中的日数(1~31)
	//! \remarks 1号=1,2号=2,...,31号=31.
	YkInt GetDay() const;         
	//! \brief 获取一天中的小时数(0~23)
	//! \remarks 0点=0，1点=1，...23点=23.
	YkInt GetHour() const;
	//! \brief 获取一小时内的分钟数(0~59)
	YkInt GetMinute() const;
	//! \brief 获取一分钟中的秒数(从0~59)
	YkInt GetSecond() const;
	//! \brief 获取一个星期的天数
	//! \remarks 周日=1,周一=2，周二=3，周三=4，周四=5；周五=6，周六=7
	YkInt GetDayOfWeek() const; 
	//! \brief 获取每年的日数(1~366)
	//! \remarks 1月1日为1,1月2日为2,...,12月31日为365或366
	YkInt GetDayOfYear() const;   
	//! \brief 重载操作符 -
	YkTimeSpan operator-(const YkTime &tmTime) const;
	//! \brief 重载操作符 -
	YkTime operator-(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 +
	YkTime operator+(const YkTimeSpan &tms) const;
	//! \brief 重载操作符 +=
	const YkTime& operator+=(const YkTimeSpan &tms);
	//! \brief 重载操作符-=
	const YkTime& operator-=(const YkTimeSpan &tms);
	//! \brief 重载操作符 ==
	YkBool operator==(const YkTime &tmTime) const;
	//! \brief 重载操作符 !=
	YkBool operator!=(const YkTime &tmTime) const;
	//! \brief 重载操作符 <
	YkBool operator< (const YkTime &tmTime) const;
	//! \brief 重载操作符 >
	YkBool operator> (const YkTime &tmTime) const;
	//! \brief 重载操作符 <=
	YkBool operator<=(const YkTime &tmTime) const;
	//! \brief 重载操作符 >=
	YkBool operator>=(const YkTime &tmTime) const;

	//! \brief 传入一个格式化字符串,把YkTime的信息生成一个便于阅读的字符串
	//! \remarks 
	//! %d Day of month as decimal number (01 – 31);
	//! %H Hour in 24-hour format (00 – 23);
	//! %m Month as decimal number (01 – 12)
	//! %M Minute as decimal number (00 – 59)
	//! %S Second as decimal number (00 – 59)
	//! %y Year without century, as decimal number (00 – 99)
	//! %Y Year with century, as decimal number
	YkString Format(const YkChar *pFormat) const;

	//! \brief 按照指定格式生成日期和时间
	//! \param strDateTime 日期和时间值
	//! \param strFormat 日期和时间的格式
	//! \remarks 例如: strFormat为"%Y-%m-%d %H:%M:%S", strDateTime为"1487-05-29 14:25:59", 
	//! 最后获取的YkTime为: 1487年05月29日 14时25分59秒
	YkBool ParseDateTime(YkString strDateTime,YkString strFormat);
	
	//! \brief 获取当前时间
	static YkTime GetCurTime();

	//!\brief 计算与当地时刻相差一定时间的地方时
	//!\param t 基础时间
    //!\param bias,与基础时间t的偏差，单位是分钟
	//!\param return返回新的地方时
	static YkTime ConvertToRegionalTime(YkTime t, YkInt bias);

	//!\brief 转换为Long型的时间
	//!\param t 时间
	//!\return 返回Long型的时间
	static YkLong ConvertToLongTime(YkTime t);

	//!\brief 将普通的日期格式转换成UGTime接受的转换格式,暂时不支持星期，毫秒，英文格式
	//!\timeFormat C#/Java日期格式，如“YYYY-MM-DD HH:mm:ss”
	//!\ return "%Y-%m-%d %H:%M:%S"
	static YkString ConvertToUGTimeFormat(YkString timeFormat);

public: 
	//! \brief 把年月日时分秒转化为内部的double数值
	//! \param nYear 年
	//! \param nMonth 月
	//! \param nDay 日
	//! \param nHour 小时
	//! \param nMin 分钟
	//! \param nSec 秒
	void SetDateTime(YkInt nYear, YkInt nMonth, YkInt nDay, YkInt nHour, YkInt nMin, YkInt nSec);
	//! \brief 把double数值转化为tm结构
	void GetDateTime(struct tm& tmDest) const;

	//! \brief 获取处理后的double值
	//! \remarks 等同于MFC中的_AfxDoubleFromDate
	YkDouble GetDoubleValue() const;

private:
	//! \brief 获取标准的tm结构
	void GetStandardTm(struct tm& tmDest) const;
	
	//! \brief 从处理后的double值中得到时间
	//! \remarks 等同于MFC中的_AfxDateFromDouble
	void FromDoubleValue(YkDouble dValue);
	//! \brief 从当前位置计算第一个不是Number的长度
	YkInt GetNumberPos(const YkString& strDateTime , YkInt nCurrentPos);

	double m_time;
};

}

#endif

