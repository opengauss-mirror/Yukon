/*
 *
 * YkTime.cpp
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

#include "Base/YkTime.h"
#include "Base/YkPlatform.h"

namespace Yk {

YkTimeSpan::YkTimeSpan()
{
	m_timeSpan = 0;
}

YkTimeSpan::YkTimeSpan(double dTime)
{
	m_timeSpan = dTime;
}

YkTimeSpan::YkTimeSpan(YkInt nDays, YkInt nHours, YkInt nMins, YkInt nSecs)
{
	m_timeSpan = nDays + ((double)nHours)/24 + ((double)nMins)/(24*60) + ((double)nSecs)/(24*60*60);
}

YkTimeSpan::YkTimeSpan(const YkTimeSpan& tmsSrc)
{
	m_timeSpan = tmsSrc.m_timeSpan;
}

const YkTimeSpan& YkTimeSpan::operator=(const YkTimeSpan& tmsSrc)
{
	if(this == &tmsSrc)
		return *this;
	m_timeSpan = tmsSrc.m_timeSpan;
	return *this;
}

YkInt YkTimeSpan::GetDays() const
{
	return (YkInt)m_timeSpan;
}

#define YK_DATETIME_HALFSECOND (1.0 / (2.0 * (60.0 * 60.0 * 24.0)))

YkInt YkTimeSpan::GetTotalHours() const
{
	int nReturns = (int)(m_timeSpan * 24 + YK_DATETIME_HALFSECOND);
	return nReturns;
}

YkInt YkTimeSpan::GetHours() const
{
	double dblTemp=0;
	// Truncate days and scale up
	dblTemp = modf(m_timeSpan, &dblTemp);
	YkInt lReturns = (YkInt)((dblTemp + YK_DATETIME_HALFSECOND) * 24);
	if (lReturns >= 24)
	{
		lReturns -= 24;
	}
	return lReturns;
}

YkInt YkTimeSpan::GetTotalMinutes() const
{
	YkInt lReturns = (YkInt)(m_timeSpan * 24 * 60 + YK_DATETIME_HALFSECOND);
	return lReturns;
}

YkInt YkTimeSpan::GetMinutes() const
{
	double dblTemp=0;
	// Truncate hours and scale up
	dblTemp = modf(m_timeSpan * 24, &dblTemp);
	YkInt lReturns = (YkInt) ((dblTemp + YK_DATETIME_HALFSECOND) * 60);
	if (lReturns >= 60)
	{
		lReturns -= 60;
	}
	return lReturns;
}

YkInt YkTimeSpan::GetTotalSeconds() const
{
	YkInt lReturns = (YkInt)(m_timeSpan * 24 * 60 * 60 + YK_DATETIME_HALFSECOND);
	return lReturns;
}

YkInt YkTimeSpan::GetSeconds() const
{
	double dblTemp=0;
	// Truncate minutes and scale up
	dblTemp = modf(m_timeSpan * 24 * 60, &dblTemp);
	YkInt lReturns = (YkInt) ((dblTemp + YK_DATETIME_HALFSECOND) * 60);
	if (lReturns >= 60)
	{
		lReturns -= 60;
	}
	return lReturns;
}

double YkTimeSpan::GetTimeSpan() const
{
	return m_timeSpan;
}

YkTimeSpan YkTimeSpan::operator-(const YkTimeSpan &tms) const
{
	return YkTimeSpan(m_timeSpan - tms.m_timeSpan);
}

YkTimeSpan YkTimeSpan::operator+(const YkTimeSpan &tms) const
{
	 return YkTimeSpan(m_timeSpan + tms.m_timeSpan);
}

const YkTimeSpan& YkTimeSpan::operator+=(const YkTimeSpan &tms)
{
	m_timeSpan += tms.m_timeSpan; 
	return *this;
}

const YkTimeSpan& YkTimeSpan::operator-=(const YkTimeSpan &tms)
{
	m_timeSpan -= tms.m_timeSpan; 
	return *this;
}

YkBool YkTimeSpan::operator==(const YkTimeSpan &tms) const
{
	return YKEQUAL(m_timeSpan,tms.m_timeSpan);
}

YkBool YkTimeSpan::operator!=(const YkTimeSpan &tms) const
{
	return !YKEQUAL(m_timeSpan,tms.m_timeSpan);
}

YkBool YkTimeSpan::operator<(const YkTimeSpan &tms) const
{
	return m_timeSpan < tms.m_timeSpan;
}

YkBool YkTimeSpan::operator>(const YkTimeSpan &tms) const
{
	return m_timeSpan > tms.m_timeSpan;
}

YkBool YkTimeSpan::operator<=(const YkTimeSpan &tms) const
{
	return ((m_timeSpan < tms.m_timeSpan)||YKEQUAL(m_timeSpan,tms.m_timeSpan));
}

YkBool YkTimeSpan::operator>=(const YkTimeSpan &tms) const
{
	return ((m_timeSpan > tms.m_timeSpan)||YKEQUAL(m_timeSpan,tms.m_timeSpan));
}


YkTime::YkTime()
{	
	m_time = 0;
}

YkTime::YkTime(const YkTime& tmSrc)
{
	m_time = tmSrc.m_time;
}

//! \brief 通过time_t进行构造
YkTime::YkTime(time_t t)
{
	m_time = 0;
	*this = t;
}

YkTime YkTime::GetCurTime()
{
	return YkTime(::time(NULL));
	
}

YkTime YkTime::ConvertToRegionalTime(YkTime t,YkInt bias)
{
	struct tm timeDest;
	//获取系统日常时间
	t.GetDateTime(timeDest);	
	timeDest.tm_year = timeDest.tm_year - 1900;
	timeDest.tm_mon = timeDest.tm_mon - 1;

	time_t utcTime = ::mktime(&timeDest) + bias * 60;
	return YkTime(utcTime);
}

YkLong YkTime::ConvertToLongTime(YkTime t)
{
	struct tm timeDest;
	//获取系统日常时间
	t.GetDateTime(timeDest);
	timeDest.tm_year = timeDest.tm_year - 1900;
	timeDest.tm_mon = timeDest.tm_mon - 1;

	time_t utcTime = ::mktime(&timeDest);
	return (YkLong)utcTime;
}

YkString YkTime::ConvertToUGTimeFormat(YkString timeFormat)
{
	YkString newTimeFormat = timeFormat;
	newTimeFormat.TrimLeft();

	newTimeFormat.Replace(_U("YYYY"), _U("%Y"));
	newTimeFormat.Replace(_U("yyyy"), _U("%Y"));
	newTimeFormat.Replace(_U("YY"), _U("%y"));
	newTimeFormat.Replace(_U("yy"), _U("%y"));

	newTimeFormat.Replace(_U("MM"), _U("%m"));
	newTimeFormat.Replace(_U("DD"), _U("%d"));

	newTimeFormat.Replace(_U("hh"), _U("%H"));
	newTimeFormat.Replace(_U("mm"), _U("%M"));
	newTimeFormat.Replace(_U("ss"), _U("%S"));

	return newTimeFormat;
}

//! \brief 直接通过YkTime内部的double数值进行构造
YkTime::YkTime(double dValue)
{
	m_time = dValue;
}

YkTime::YkTime(YkInt nYear, YkInt nMonth, YkInt nDay)
{
	m_time = 0;
	SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
}

YkTime::YkTime(YkInt nYear, YkInt nMonth, YkInt nDay, 
			   YkInt nHour, YkInt nMin, YkInt nSec)
{
	m_time = 0;
	SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
}

const YkTime& YkTime::operator=(const YkTime& tmSrc)
{
	if(this == &tmSrc)
		return *this;
	m_time = tmSrc.m_time; 
	return *this;
}

const YkTime& YkTime::operator=(time_t t)
{
	// Convert time_t to struct tm
	tm *ptm = localtime(&t);

	if (ptm != NULL)
	{
		SetDateTime(ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	}
	return *this;
}

double YkTime::GetTime() const
{
	return m_time;
}

YkInt YkTime::GetYear() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_year;
}

YkInt YkTime::GetMonth() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_mon;
}

YkInt YkTime::GetDay() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_mday;
}

YkInt YkTime::GetHour() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_hour;
}

YkInt YkTime::GetMinute() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_min;
}

YkInt YkTime::GetSecond() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_sec;
}

YkInt YkTime::GetDayOfWeek() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_wday;
}

YkInt YkTime::GetDayOfYear() const
{
	struct tm tmDest;
	GetDateTime(tmDest);
	return tmDest.tm_yday;
}

YkTimeSpan YkTime::operator-(const YkTime &tmTime) const
{
	return GetDoubleValue() - tmTime.GetDoubleValue();
}

YkTime YkTime::operator-(const YkTimeSpan &tms) const
{
	YkTime tmResult;
	tmResult.FromDoubleValue(GetDoubleValue() - tms.m_timeSpan);
	return tmResult;
}

YkTime YkTime::operator+(const YkTimeSpan &tms) const
{
	YkTime tmResult;
	tmResult.FromDoubleValue(GetDoubleValue() + tms.m_timeSpan);
	return tmResult;
}

const YkTime& YkTime::operator+=(const YkTimeSpan &tms)
{
	 *this = this->operator+(tms);
	 return *this;
}

const YkTime& YkTime::operator-=(const YkTimeSpan &tms)
{
	*this = *this - tms;
	return *this;
}

YkBool YkTime::operator==(const YkTime &tmTime) const
{
	return YKEQUAL(m_time,tmTime.m_time);
}

YkBool YkTime::operator!=(const YkTime &tmTime) const
{
		return !YKEQUAL(m_time,tmTime.m_time);
}

YkBool YkTime::operator< (const YkTime &tmTime) const
{
	return GetDoubleValue() < tmTime.GetDoubleValue();
}

YkBool YkTime::operator> (const YkTime &tmTime) const
{
	return GetDoubleValue() > tmTime.GetDoubleValue();
}

YkBool YkTime::operator<=(const YkTime &tmTime) const
{
	 return (GetDoubleValue() < tmTime.GetDoubleValue())||YKEQUAL(GetDoubleValue() ,tmTime.GetDoubleValue());
}

YkBool YkTime::operator>=(const YkTime &tmTime) const
{
	  return (GetDoubleValue() > tmTime.GetDoubleValue())||YKEQUAL(GetDoubleValue() ,tmTime.GetDoubleValue());
}

YkDouble YkTime::GetDoubleValue()  const
{
	// No problem if positive
	if (m_time >= 0)
		return m_time;

	// If negative, must convert since negative dates not continuous
	// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
	double temp = ceil(m_time);
	return temp - (m_time - temp);
}

void YkTime::FromDoubleValue(YkDouble dValue)
{
	// No problem if positive
	if (dValue >= 0)
	{
		m_time = dValue;
		return ;
	}

	// If negative, must convert since negative dates not continuous
	// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
	double temp = floor(dValue); // dbl is now whole part
	//return temp + (temp - dbl);
	m_time = temp + (temp - dValue);
}

int g_nMonthDays[13] =
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

void YkTime::SetDateTime(YkInt nYear, YkInt nMonth, YkInt nDay, 
						 YkInt nHour, YkInt nMin, YkInt nSec)
{
	// Validate year and month (ignore day of week and milliseconds)
	if (nYear > 9999 || nMonth < 1 || nMonth > 12)
		return ;

	//  Check for leap year and set the number of days in the month
	bool bLeapYear = ((nYear & 3) == 0) &&
		((nYear % 100) != 0 || (nYear % 400) == 0);

	int nDaysInMonth =
		g_nMonthDays[nMonth] - g_nMonthDays[nMonth-1] +
		((bLeapYear && nDay == 29 && nMonth == 2) ? 1 : 0);

	// Finish validating the date
	if (nDay < 1 || nDay > nDaysInMonth ||
		nHour > 23 || nMin > 59 ||
		nSec > 59)
	{
		return ;
	}

	// Cache the date in days and time in fractional days
	long nDate=0;	//	WhiteBox_Ignore
	double dblTime=0;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate = nYear*365L + nYear/4 - nYear/100 + nYear/400 +
		g_nMonthDays[nMonth-1] + nDay;

	//  If leap year and it's before March, subtract 1:
	if (nMonth <= 2 && bLeapYear)
		--nDate;

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((long)nHour * 3600L) +  // hrs in seconds	//	WhiteBox_Ignore
		((long)nMin * 60L) +  // mins in seconds			//	WhiteBox_Ignore
		((long)nSec)) / 86400.0;							//	WhiteBox_Ignore

	m_time = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

	return ;
}

#define DATA_MIN                (-657434L)  
#define DATA_MAX               2958465L    
#define SECOND_HALF             (1.0/172800.0)

void YkTime::GetDateTime(struct tm& destTime) const
{
	if (m_time > DATA_MAX || m_time < DATA_MIN) 
	{
		destTime.tm_year = 1899;
		destTime.tm_mon = 12;
		destTime.tm_mday = 30;
		destTime.tm_hour = 0;
		destTime.tm_min = 0;
		destTime.tm_sec = 0;
		destTime.tm_isdst = 0;
		destTime.tm_wday = 0;
		destTime.tm_yday = 0;
		return ;
	}
	YkLong lDaysAbs=0;    //Absolute Days
	YkLong lSecWithinDay=0;       
	YkLong lMntWithinDay=0;     
	YkLong l4oo_Years=0;        
	YkLong l4oo_Century=0;       
	YkLong l4_Years=0;           
	YkLong l4_Day=0;             		
	YkLong l4_Yr=0;              
	YkBool bHop4 = TRUE;    
	YkDouble date = m_time; 

	date += ((m_time > 0.0) ? SECOND_HALF : -SECOND_HALF);

	lDaysAbs = (YkLong)date + 693959L; 

	date = fabs(date);
	lSecWithinDay = (YkLong)((date - floor(date)) * 86400.);

	destTime.tm_wday = (YkInt)((lDaysAbs - 1) % 7L) + 1;

	l4oo_Years = (YkLong)(lDaysAbs / 146097L);

	lDaysAbs %= 146097L;

	l4oo_Century = (YkLong)((lDaysAbs - 1) / 36524L);

	if (l4oo_Century != 0)
	{
		lDaysAbs = (lDaysAbs - 1) % 36524L;

		l4_Years = (YkLong)((lDaysAbs + 1) / 1461L);

		if (l4_Years != 0)
			l4_Day = (YkLong)((lDaysAbs + 1) % 1461L);
		else
		{
			bHop4 = FALSE;
			l4_Day = (YkLong)lDaysAbs;
		}
	}
	else
	{
		l4_Years = (YkLong)(lDaysAbs / 1461L);
		l4_Day = (YkLong)(lDaysAbs % 1461L);
	}

	if (!bHop4)
	{
		l4_Yr = l4_Day / 365;
		l4_Day %= 365;
	}
	else
	{
		l4_Yr = (l4_Day - 1) / 365;
		if (l4_Yr != 0)
		{
			l4_Day = (l4_Day - 1) % 365;
		}
	}

	destTime.tm_yday = (YkInt)l4_Day + 1;
	destTime.tm_year = (YkInt)(l4oo_Years * 400 + l4oo_Century * 100 + l4_Years * 4 + l4_Yr);

	if (l4_Yr == 0 && bHop4)
	{
		if (l4_Day == 59)
		{
			destTime.tm_mon = 2;
			destTime.tm_mday = 29;
			goto DoDataTime;
		}

		if (l4_Day >= 60)
		{
			l4_Day--;
		}
	}

	l4_Day++;

	for (destTime.tm_mon = (YkInt)((l4_Day >> 5) + 1);
		l4_Day > g_nMonthDays[destTime.tm_mon]; destTime.tm_mon++);

	destTime.tm_mday = (YkInt)(l4_Day - g_nMonthDays[destTime.tm_mon-1]);

DoDataTime:
	if (lSecWithinDay != 0)
	{
		destTime.tm_sec = (YkInt)lSecWithinDay % 60L;
		lMntWithinDay = lSecWithinDay / 60L;
		destTime.tm_min = (YkInt)lMntWithinDay % 60;
		destTime.tm_hour = (YkInt)lMntWithinDay / 60;
	}
	else
	{
		destTime.tm_hour = destTime.tm_min = destTime.tm_sec = 0;
	}

	return ;
}

//! \brief 得到标准的tm结构
void YkTime::GetStandardTm(struct tm& tmDest) const
{
	GetDateTime(tmDest);
	// Convert afx internal tm to format expected by runtimes (_tcsftime, etc)
	tmDest.tm_year -= 1900;  // year is based on 1900
	tmDest.tm_mon -= 1;      // month of year is 0-based
	tmDest.tm_wday -= 1;     // day of week is 0-based
	tmDest.tm_yday -= 1;     // day of year is 0-based
}
#define maxTimeBufferSize       128
YkString YkTime::Format(const YkChar *pFormat) const
{
	YkChar szBuffer[maxTimeBufferSize];	
	struct tm tmDest;
	GetStandardTm(tmDest);

	bool bDealYear = false;
	if (tmDest.tm_year < 0)
	{
		tmDest.tm_year = -tmDest.tm_year;// 得到正数
		bDealYear = true;
	}
	
	if (!YKstrftime(szBuffer, maxTimeBufferSize, pFormat, &tmDest))
	{
		szBuffer[0] = _U('\0');
	}

	YkString strResult(szBuffer);

	// 如果之前处理过,这里要还原
	if (bDealYear)
	{
		YkString strOld, strNew;
		strOld.Format(_U("%04d"), tmDest.tm_year+1900);
		strNew.Format(_U("%04d"), -tmDest.tm_year+1900);
		strResult.Replace(strOld.Cstr(), strNew.Cstr());
	}
	return strResult;
}

YkBool YkTime::ParseDateTime(YkString strDateTime, YkString strFormat)
{
	strFormat.TrimLeft();
	strFormat.TrimRight();
	strDateTime.TrimLeft();
	strDateTime.TrimRight();
	if( strFormat.GetLength() <= 0 )
	{
		return FALSE;
	}

	YkChar *pCh = (YkChar *)strFormat.Cstr();
	YkInt nPos = 0, nPosDateTime = 0;
	struct tm atm;
	atm.tm_year = 1899;
	atm.tm_mon = 12;
	atm.tm_mday = 30;
	atm.tm_hour = 0;
	atm.tm_min = 0;
	atm.tm_sec = 0;
	YkInt nLen = 0;
	while( nPos < strFormat.GetLength())
	{
		switch(*pCh)
		{
		case _U('%'):
			break;
		case _U('Y'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_year = strDateTime.Mid(nPosDateTime,nLen).ToInt();
					nPosDateTime += nLen;
				}
				break;
			}
		case _U('y'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_year = strDateTime.Mid(nPosDateTime,nLen).ToInt();
					nPosDateTime += nLen;
				}
				break;
			}
		case _U('m'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_mon = strDateTime.Mid(nPosDateTime,nLen).ToInt(); // - 1;
					nPosDateTime += nLen;
				}


				break;
			}
		case _U('d'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_mday = strDateTime.Mid(nPosDateTime,nLen).ToInt(); // - 1;
					nPosDateTime += nLen;
				}
				
				
				break;
			}
		case _U('X'):
			{
				atm.tm_hour = strDateTime.Mid(nPosDateTime,2).ToInt();
				nPosDateTime += 3;
				atm.tm_min = strDateTime.Mid(nPosDateTime,2).ToInt();
				nPosDateTime += 3;
				atm.tm_sec = strDateTime.Mid(nPosDateTime,2).ToInt();
				nPosDateTime += 2;
				break;
			}
		case _U('B'):
			{
				YkInt nCount = 0;
				YkChar *p = (YkChar *)(strDateTime.Cstr());
				if(nPosDateTime < strDateTime.GetLength())
				{
					p += nPosDateTime;
					while ( (*p >= _U('A') && *p <= _U('Z')) || 
						(*p >= _U('a') && *p <= _U('z')) )
					{
						nCount ++;	
						p++;
					}
					
					YkString strTemp = strDateTime.Mid(nPosDateTime,nCount);
					
					if (strTemp.CompareNoCase(_U("January")) == 0)
					{
						atm.tm_mon = 0;
					}
					else if (strTemp.CompareNoCase(_U("February")) == 0)
					{
						atm.tm_mon = 1;
					}
					else if (strTemp.CompareNoCase(_U("March")) == 0)
					{
						atm.tm_mon = 2;
					}
					else if (strTemp.CompareNoCase(_U("April")) == 0)
					{
						atm.tm_mon = 3;
					}
					else if (strTemp.CompareNoCase(_U("May")) == 0)
					{
						atm.tm_mon = 4;
					}
					else if (strTemp.CompareNoCase(_U("June")) == 0)
					{
						atm.tm_mon = 5;
					}
					else if (strTemp.CompareNoCase(_U("July")) == 0)
					{
						atm.tm_mon = 6;
					}
					else if (strTemp.CompareNoCase(_U("August")) == 0)
					{
						atm.tm_mon = 7;
					}
					else if (strTemp.CompareNoCase(_U("September")) == 0)
					{
						atm.tm_mon = 8;
					}
					else if (strTemp.CompareNoCase(_U("October")) == 0)
					{
						atm.tm_mon = 9;
					}
					else if (strTemp.CompareNoCase(_U("November")) == 0)
					{
						atm.tm_mon = 10;
					}
					else if (strTemp.CompareNoCase(_U("December")) == 0)
					{
						atm.tm_mon = 11;
					}
					else
					{
						return FALSE;
					}

					atm.tm_mon ++;
					nPosDateTime += nCount;

				}

				break;
			}
		case _U('b'):
			{
				YkString strTemp = strDateTime.Mid(nPosDateTime,3);
				
				if (strTemp.CompareNoCase(_U("Jan")) == 0)
				{
					atm.tm_mon = 0;
				}
				else if (strTemp.CompareNoCase(_U("Feb")) == 0)
				{
					atm.tm_mon = 1;
				}
				else if (strTemp.CompareNoCase(_U("Mar")) == 0)
				{
					atm.tm_mon = 2;
				}
				else if (strTemp.CompareNoCase(_U("Apr")) == 0)
				{
					atm.tm_mon = 3;
				}
				else if (strTemp.CompareNoCase(_U("May")) == 0)
				{
					atm.tm_mon = 4;
				}
				else if (strTemp.CompareNoCase(_U("Jun")) == 0)
				{
					atm.tm_mon = 5;
				}
				else if (strTemp.CompareNoCase(_U("Jul")) == 0)
				{
					atm.tm_mon = 6;
				}
				else if (strTemp.CompareNoCase(_U("Aug")) == 0)
				{
					atm.tm_mon = 7;
				}
				else if (strTemp.CompareNoCase(_U("Sep")) == 0)
				{
					atm.tm_mon = 8;
				}
				else if (strTemp.CompareNoCase(_U("Oct")) == 0)
				{
					atm.tm_mon = 9;
				}
				else if (strTemp.CompareNoCase(_U("Nov")) == 0)
				{
					atm.tm_mon = 10;
				}
				else if (strTemp.CompareNoCase(_U("Dec")) == 0)
				{
					atm.tm_mon = 11;
				}
				else
				{
					return FALSE;
				}
				nPosDateTime += 3;
				break;
			}
		case _U('H'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_hour = strDateTime.Mid(nPosDateTime,nLen).ToInt();
					nPosDateTime += nLen;
				}
				break;
			}
		case _U('M'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_min = strDateTime.Mid(nPosDateTime,nLen).ToInt();
					nPosDateTime += nLen;
				}
				break;
			}
		case _U('S'):
			{
				nLen = GetNumberPos(strDateTime,nPosDateTime);
				if(nLen > 0)
				{
					atm.tm_sec = strDateTime.Mid(nPosDateTime,nLen).ToInt();
					nPosDateTime += nLen;
				}
				break;
			}
		case _U('-'):
		case _U(' '):
		case _U(':'):
		case _U(','):
			nPosDateTime ++;
			break;
		default:
			return FALSE;
		}
		pCh ++;
		nPos ++;
	}
	
	SetDateTime(atm.tm_year, atm.tm_mon, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);
	return TRUE;
}



YkUint YkTime::GetMillisecond()
{
#ifdef WIN32	
	
	return ::GetTickCount();
#else
	timeval tv;
	YkTime time = YkTime::GetCurTime();
	YkInt nHour = time.GetHour();
	YkInt nMinute = time.GetMinute();
	YkInt nSecond = time.GetSecond();
	gettimeofday(&tv, NULL);
	YkInt nResult = ((nHour*60+nMinute)*60+nSecond)*1000+(YkInt)(tv.tv_usec/1000);
	return nResult;
#endif
}

YkInt YkTime::GetNumberPos( const YkString& strDateTime , YkInt nCurrentPos )
{
	YkInt nPos = 0;
	while( nCurrentPos<strDateTime.GetLength() && _U('0')<=strDateTime.GetAt(nCurrentPos) && strDateTime.GetAt(nCurrentPos)<=_U('9') )
	{
		nPos++;
		nCurrentPos++;
	}
	return nPos;
}
}

