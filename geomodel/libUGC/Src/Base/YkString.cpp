/*
 *
 * YkString.cpp
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

#include <new>
#include <string>
#include "Base/YkString.h"
#include "Base/YkSystem.h"
#include "Base/YkPlatform.h"
#if defined _UGUNICODE && !defined _UGQSTRING
#pragma warning (disable: 4512)
#include "unicode/unistr.h"
#include "unicode/numfmt.h"
#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#pragma warning (default: 4512)
using namespace U_ICU_NAMESPACE;
#endif

using namespace std;

namespace Yk {



YkMBString::YkMBString()
{
	m_nCharset = YkMBString::Default;
}
    
YkMBString::YkMBString(const YkMBString& str)
{
	m_nCharset = str.GetCharset();
	m_string.assign(str.Cstr());
}

YkMBString::YkMBString(const std::string& str)
{
	m_nCharset = YkMBString::Default;
	m_string.assign(str);
}

YkMBString::YkMBString(const YkAchar* pStr)
{
	m_nCharset = YkMBString::Default;
	if(pStr)
	{
		m_string.assign(pStr);
	}
}

YkMBString::YkMBString(const YkAchar* pStr, YkInt nSize)
{
	YKASSERT(nSize >= 0);
	m_nCharset = YkMBString::Default;
	if (pStr)
	{
		m_string.assign(pStr, nSize);
	}
}

YkMBString::YkMBString(YkAchar ch, YkInt nRepeat)
{
	m_nCharset = YkMBString::Default;
	if(nRepeat>0)
	{
		m_string.assign(nRepeat,ch);
	}
}

YkInt YkMBString::GetLength() const
{
	return (YkInt)m_string.size();
}

//! 设置字符串长度
//! \remark 如果长度变小,会自动截断; 如果长度变大,会填充0x00, 但Length会变长
void YkMBString::SetLength(YkInt nLength)
{
	m_string.resize(nLength);
}

YkMBString& YkMBString::MakeUpper()
{
	std::string::iterator  it;
	for(it = m_string.begin(); it != m_string.end(); ++it)
	{
		if(isascii(*it) != 0)
		{
			*it = (char)toupper(*it);
		}
		else if (YkSystem::IsLeadByte(*it))
		{
			++ it;
			if (it == m_string.end())
			{
				break;
			}
		}
	}
	return *this;
}

YkMBString& YkMBString::MakeLower()
{
	std::string::iterator  it;
	for(it = m_string.begin(); it != m_string.end(); ++it)
	{
		if(isascii(*it))
		{
			*it = (char)tolower(*it);
		}
		else if (YkSystem::IsLeadByte(*it))
		{
			++ it;
			if (it == m_string.end())
			{
				break;
			}
		}
	}
	return *this;
	
}

YkMBString& YkMBString::MakeReverse() 
{
	YkAchar chFirst, chSecond;
	std::string::iterator  it;
	for(it = m_string.begin(); it != m_string.end(); ++it)
	{
		if(YkSystem::IsLeadByte(*it))
		{
			++it;
			if (it != m_string.end()) 
			{
				chSecond = *it;
				-- it;
				chFirst = *it;
				*it = chSecond;
				++ it;
				*it = chFirst;
			}
			else
			{
				break;
			}
		}
	}

	std::reverse(m_string.begin(), m_string.end());
	return *this;
}

YkMBString YkMBString::Left(YkInt nCount) const
{
	if(nCount<=0 || m_string == "") 
	{
		return YkMBString("");
	}
	return m_string.substr(0,nCount);
}

YkMBString YkMBString::Right(YkInt nCount) const
{
	if (nCount<=0 || m_string == "") 
	{
		return YkMBString("");
	}
	YkMBString strTmp;
	if (nCount>GetLength()) 
	{
		strTmp = m_string.substr(0);
	}
	else 
	{
		strTmp = m_string.substr(GetLength()-nCount);
	}
	return strTmp;
}

YkMBString YkMBString::Mid(YkInt nFirst) const
{	
	if (nFirst >= GetLength() || m_string == "")
	{
		return YkMBString("");
	}

	YkMBString strTmp;
	if (nFirst <= 0) 
	{
		strTmp = m_string.substr(0);
	} 
	else 
	{
		strTmp = m_string.substr(nFirst);
	}
	return strTmp;		
}

YkMBString YkMBString::Mid(YkInt nFirst,YkInt nCount) const
{
	if (nCount <= 0 || m_string == "")
	{
		return YkMBString("");
	}
	if (nFirst >= GetLength()) 
	{
		return YkMBString("");
	}

	YkMBString strTmp;
	if (nFirst <= 0) 
	{
		strTmp = m_string.substr(0,nCount);
	} 
	else 
	{
		strTmp = m_string.substr(nFirst,nCount);
	}
	return strTmp;				
}


const YkAchar * YkMBString::Cstr() const
{
	return m_string.c_str();
}

const std::string& YkMBString::Str() const
{
	return m_string;
}

YkAchar YkMBString::GetAt(YkInt nIndex) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return m_string.at(nIndex);
}

void YkMBString::SetAt(YkInt nIndex, YkAchar ch)
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	m_string.at(nIndex)=ch;
}

YkAchar YkMBString::operator[](YkInt nIndex) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return m_string.at(nIndex);
}

YkAchar& YkMBString::operator[](YkInt nIndex)
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return m_string.at(nIndex);
}

//! 确保有指定字节个数的内存空间
//! \remark 如果原有空间不够, 内存空间会增加
//! \remark 如果原有空间比指定的大, 也不会释放空间
void YkMBString::SetCapacity(YkInt nCapacity)
{
	m_string.reserve(nCapacity);
}

//! 得到目前所有空间(包括已经使用的和保留的)所占用的字节个数
YkInt YkMBString::GetCapacity() const
{
	return (YkInt)m_string.capacity();
}

YkInt YkMBString::Delete(YkInt nIndex,YkInt nCount)
{
	if(nIndex<0) 
		nIndex=0;
	if(nCount>0 && nIndex<GetLength()) 
	{
		m_string.erase(nIndex,nCount);
	}
	return GetLength();
}

YkInt YkMBString::Insert(YkInt nIndex,const YkAchar * pStr)
{	
	YkInt nLength=GetLength();
	nIndex=YKCLAMP(0,nIndex,nLength);
	m_string.insert(nIndex,pStr);
	return GetLength();
}

YkInt YkMBString::Insert(YkInt nIndex,YkAchar ch,YkInt nCount)
{
	if(nCount>0) {
		YkInt nLength=GetLength();
		nIndex=YKCLAMP(0,nIndex,nLength);
		m_string.insert(nIndex,nCount,ch);
	}
	return GetLength();
}

YkInt YkMBString::Remove(YkAchar ch)
{
	YkInt nCount=0;
	YkInt nPos=0;
	while(nPos<GetLength())
	{
		if(m_string.at(nPos)==ch) {
			m_string.erase(nPos,1);
			nCount++;
		} else {
			nPos++;
		}
	}
	return nCount;
}

YkMBString& YkMBString::TrimLeft()
{
	YkUint pos = (YkInt)m_string.find_first_not_of("\t\r\n ");
	m_string.erase(0, pos);
		
	return *this;
}

YkMBString& YkMBString::TrimLeft(YkAchar ch)
{
	std::string::size_type pos =  m_string.find_first_not_of(ch);
	if(pos>0 && pos < m_string.size())
	{ 
		m_string.erase(m_string.begin(),m_string.begin()+pos);		
	}
	return *this;
}

YkMBString& YkMBString::TrimLeft(const YkAchar* pStr)
{
	if(pStr) 
	{
		std::string::iterator it=m_string.begin();
		for(;it!=m_string.end();++it)
		{
			if(strchr(pStr, *it) == NULL)
				break;		
		}
		m_string.erase(m_string.begin(),it);
	}
	return *this;
}

YkMBString& YkMBString::TrimRight()
{
	YkUint pos = (YkUint)m_string.find_last_not_of("\t\r\n ");
	m_string.erase(pos+1, m_string.size());
	
	return *this;
}

YkMBString& YkMBString::TrimRight(YkAchar ch)
{
	std::string::size_type pos = m_string.find_last_not_of(ch);
	++pos;
	if(pos<(YkUint)(GetLength()))
	{		
		m_string.erase(m_string.begin()+pos,m_string.end());
	}
	return *this;
}

YkMBString& YkMBString::TrimRight(const YkAchar* pStr)
{
	if(pStr) 
	{
		YkInt pos=GetLength();
		for(;pos>0;pos--)
		{
			if(strchr(pStr, m_string.at(pos-1)) == NULL)
				break;
		}
		if(pos<GetLength())
		{		
			m_string.erase(m_string.begin()+pos,m_string.end());
		}
	}
	return *this;
}

YkBool YkMBString::IsEmpty() const
{
	return m_string.empty();
}

void YkMBString::Empty()
{
	m_string.erase(m_string.begin(),m_string.end());
}

YkInt YkMBString::Compare(const YkAchar* pStr) const
{
	YKASSERT(pStr);
	return m_string.compare(pStr);	
}

YkInt YkMBString::CompareNoCase(const YkAchar* pStr) const
{
	YKASSERT(pStr);
	YkMBString str(pStr);	
	return this->CompareNoCase(str);
}

YkInt YkMBString::CompareNoCase(const YkMBString& str) const
{
	YkInt nCountA = 0,nCountB = 0;
	nCountA = this->GetLength();
	nCountB = str.GetLength();
	if(nCountA == nCountB)
	{
		YkInt i=0;
		YkAchar charA,charB;
		for(i=0;i<nCountA;i++)
		{
			charA = this->GetAt(i);
			charB = str.GetAt(i);
			if(isascii(charA) && isascii(charB))
			{
				if(tolower(charA) == tolower(charB))
				{
					continue;
				}
			}
			else if (YKIsLeadByte(charA) && YKIsLeadByte(charB))
			{
				if(charB == charA)
				{
					continue;
				}
			}
			return charA-charB;
		}
		return 0;
	}
	
	return nCountA-nCountB;
}

YkInt YkMBString::ReverseFind(YkAchar ch,YkInt nStart) const
{
	return (YkInt)m_string.rfind(ch,nStart);
}

YkInt YkMBString::ReverseFind(const YkAchar* pStrSub,YkInt nStart) const
{
	return (YkInt)m_string.rfind(pStrSub,nStart);
}

YkInt YkMBString::Find(YkAchar ch,YkInt nStart) const
{
	return (YkInt)m_string.find(ch,nStart);
}

YkInt YkMBString::FindOneOf(const YkAchar* pStrCharSet) const
{
	YKASSERT(pStrCharSet);
	return (YkInt)m_string.find_first_of(pStrCharSet);
}

YkInt YkMBString::Find(const YkAchar* pStrSub, YkInt nStart) const
{
	YKASSERT(pStrSub);
	return (YkInt)m_string.find(pStrSub,nStart);
}

YkMBString YkMBString::SpanIncluding(const YkAchar* pStrCharSet) const
{
	YKASSERT(pStrCharSet);
	YkInt nPos = (YkInt)m_string.find_first_not_of(pStrCharSet);
	if(nPos == -1) 
	{ 
		return *this;
	} 
	else
	{ 
		return Left(nPos);
	}
}

YkMBString YkMBString::SpanExcluding(const YkAchar* pStrCharSet) const
{
	YKASSERT(pStrCharSet);
	YkInt nPos = (YkInt)m_string.find_first_of(pStrCharSet);
	if(nPos == -1) 
	{ 
		return *this;
	} 
	else	
	{ 
		return Left(nPos);
	}
}

YkInt YkMBString::Replace(YkAchar chOld,YkAchar chNew)
{
	YkInt nCount=0;
	if(chOld != chNew) 
	{
		YkInt nLength=GetLength();	
		for(YkInt i=0;i<nLength;i++)
		{
			if (m_string.at(i) == chOld && (i > 0 && !YKIsLeadByte(m_string.at(i-1)))) 
			{
				m_string.at(i) = chNew;
				nCount++;
			}
		}		
	}
	return nCount;
}

YkInt YkMBString::Replace(const YkAchar* pStrOld,const YkAchar* pStrNew)
{
	YkInt nCount=0;
	if(pStrOld && pStrNew) 
	{	
		YkInt nSourceLen = (YkInt)strlen(pStrOld);
		if(nSourceLen == 0)
		{
			return 0;
		}
		YkInt nReplacementLen = (YkInt)strlen(pStrNew);	
		YkInt nIndex = (YkInt)m_string.find(pStrOld,0);
		while (nIndex > -1)
		{
			m_string.replace(nIndex,nSourceLen,pStrNew);		
			nCount++;
			nIndex = (YkInt)m_string.find(pStrOld,nIndex+nReplacementLen);
		}
	}
	return nCount;
}


void YkMBString::Format(const YkAchar* fmt,...)
{
	YkInt len=0;
	if(fmt && *fmt)
	{
		YkUint n=(YkInt)strlen(fmt);
		n+=128;
x:  m_string.resize(n);
		va_list args;	
		va_start(args,fmt);

		#pragma warning(disable:4996)
		
		len=vsnprintf(&(*m_string.begin()),n+1,fmt,args);

		#pragma warning(default:4996)  

		va_end(args);

		if(len<0)
		{ 
			n<<=1; 
			goto x; 
		}
		if(n<(YkUint)len)
		{
			n=len; 
			goto x; 
		}
	}
	m_string.resize(len);
}
 

YkMBString YkMBString::From(YkLong lVal, YkUint base)
{
	YkBool bNegative = FALSE;
	if (base == 10 && lVal < 0)
	{
 		bNegative = TRUE;
 		lVal = -lVal;
	}
	YkMBString strResult = From((YkUlong)lVal, base);
	if (bNegative)
	{
 		strResult = "-" + strResult;
	}
	return strResult;
}

YkMBString YkMBString::From(YkUlong lVal, YkUint base)
{
	YkMBString strResult;
	strResult.SetCapacity(20);
	do {
		YkUint nNum = 0;
		nNum = (YkUint)(lVal % base);
		lVal /= base;
		if (nNum > 9)
		{ // 十六进制
			strResult += YkAchar(nNum-10+'A');
		}
		else 
		{ // 十进制
			strResult += YkAchar(nNum+'0');
		}
	} while(lVal > 0);
	// 最后反转结果
	strResult.MakeReverse();
	if (base == 16)
	{
		strResult = "0X" + strResult;
	}
	return strResult;
}
 
YkMBString YkMBString::From(YkInt nVal, YkUint base)
{
	return From((YkLong)nVal, base);
}

YkMBString YkMBString::From(YkUint nVal, YkUint base)
{
	return From((YkUlong)nVal, base);
}

Yk::YkMBString YkMBString::From( unsigned long nVal, YkUint base/*=10*/ )	WhiteBox_Ignore
{
	return From((YkUlong)nVal, base);
}

YkInt YkMBString::ToInt(YkUint base) const
{
	return (YkInt)strtol(Cstr(),NULL,(YkInt)base);
}

YkUint YkMBString::ToUInt(YkUint base) const
{
	return (YkUint)strtoul(Cstr(),NULL,(YkInt)base);
}

YkLong YkMBString::ToLong(YkUint base) const
{
	YkMBString strTemp = *this;
	strTemp.TrimLeft();
	if (strTemp.GetLength() == 0)
	{
 		return 0;
	}
	YkLong lVal = 0;
	if (base == 10 && strTemp.GetAt(0) == '-')
	{
 		strTemp.TrimLeft('-');
 		YkUlong ulVal = strTemp.ToULong(base);
 		lVal = YkLong(ulVal * -1);
	}
	else
	{
 		lVal = (YkLong)strTemp.ToULong(base);
	}
	return lVal;
}

YkUlong YkMBString::ToULong(YkUint base) const
{
	YkMBString strTemp = *this;
	strTemp.Remove(',');
	if (base == 16 && strTemp.Left(2).CompareNoCase("0X") == 0)
	{
		strTemp = strTemp.Right(strTemp.GetLength()-2);
	}
	YkUlong ulVal = 0;
	YkInt nLen = strTemp.GetLength();
	if (base == 10)
	{
		for (int i=0; i<nLen; i++)
		{
			YkAchar ch = strTemp.GetAt(i);
			if (YkSystem::IsDigit(ch))
			{
				ulVal = ulVal*10 + (ch-'0');
			}
			else
			{
				break;
			}
		}
	}
	else if (base == 16) 
	{
		for (int i=0; i<nLen; i++)
		{
			YkAchar ch = strTemp.GetAt(i);
			if (YkSystem::IsHexDigit(ch))
			{
				ulVal = ulVal*16;
				if (YkSystem::IsDigit(ch)) 
				{
					ulVal += ch - '0';
				}
				else if (YkSystem::IsBigLetter(ch))
				{
					ulVal += ch - 'A' + 10;
				}
				else if (YkSystem::IsLittleLetter(ch))
				{
					ulVal += ch - 'a' + 10;
				}
			}
			else
			{
				break;
			}
		}
	}
	return ulVal;
}

YkFloat YkMBString::ToFloat() const
{
	return (YkFloat)strtod(Cstr(),NULL);
}

YkDouble YkMBString::ToDouble() const
{
	return strtod(Cstr(),NULL);
}

YkInt YkMBString::Split(YkMBStringArray &strDest,const YkMBString& strDelims, YkBool bKeepEmptyString) const
{
	strDest.SetSize(0);
	const char * pdelims = strDelims.Cstr();

	if (bKeepEmptyString)
	{
		int nLimIndex = 0;
		int nLimLen = strDelims.GetLength();
		int nContentIndex = 0;
		YkBool bContinue = TRUE;
		while (bContinue)
		{
			nLimIndex = Find(pdelims, nContentIndex);
			if (nLimIndex == -1)
			{
				bContinue = FALSE;
			}
			else
			{
				strDest.Add(Mid(nContentIndex, nLimIndex-nContentIndex));
				nContentIndex += (nLimIndex-nContentIndex) + nLimLen;
			}
		}
		// 最后一个分隔符 后的内容加入
		strDest.Add(Mid(nContentIndex));
		return (YkInt)strDest.GetSize();
	}
	else
	{
		YkInt n=0;	// i 需要的字符索引, j 分隔符所在的索引

		std::string::size_type  i = 0;
		std::string::size_type  j = 0;

		while(j!=std::string::npos)
		{
			i=m_string.find_first_not_of(strDelims.Cstr(),j);
			if(i==std::string::npos)
			{
				break;
			}
			j=m_string.find_first_of(strDelims.Cstr(),i);
			strDest.Add(m_string.substr(i,j-i));
			n++;
		}
		return n;
	}
}

const YkMBString& YkMBString::operator=(const YkMBString& str)
{
	if( &str == this)
		return *this;
	m_nCharset = str.GetCharset();
	m_string.assign(str.Cstr());
	return *this;
}

const YkMBString& YkMBString::operator=(const std::string& str)		
{
	m_nCharset = YkMBString::Default;
	m_string.assign(str);
	return *this;
}

const YkMBString& YkMBString::operator=(const YkAchar* pStr)
{
	m_nCharset = YkMBString::Default;
	if(pStr)
	{
		if(m_string.c_str() == pStr)
		{
			return *this;
		}
		m_string.assign(pStr);
	}
	return *this;
}

YkMBString YkMBString::operator+(const YkAchar ch) const
{
	YkMBString strResult(*this);
	strResult.m_string += ch;
	return strResult;
}

YkMBString YkMBString::operator+(const YkAchar* pStr) const
{
	YkMBString strResult(*this);
	if (pStr != NULL)
	{
		strResult.m_string += pStr;
	}
	return strResult;
}

YkMBString YkMBString::operator+(const YkMBString& str) const
{
	YkMBString strResult(*this);
	strResult.m_string += str.Cstr();
	return strResult;
}

const YkMBString& YkMBString::operator+=(const YkMBString& str)
{
	m_string += str.Cstr();
	return *this;
}

const YkMBString& YkMBString::operator+=(const YkAchar ch)
{
	m_string += ch;
	return *this;
}

const YkMBString& YkMBString::operator+=(const YkAchar* pStr)
{
	m_string += pStr;
	return *this;
}

bool YkMBString::operator==(const YkAchar* pStr) const
{
	return m_string.compare(pStr) == 0;
}

bool YkMBString::operator==(const YkMBString& str) const
{
	return m_string.compare(str.m_string) == 0;
}

bool YkMBString::operator!=(const YkAchar* pStr) const
{
	return !(m_string == pStr);
}

bool YkMBString::operator!=(const YkMBString& str) const
{
	return !(m_string == str.m_string);
}

bool YkMBString::operator<(const YkMBString& str) const
{
	return m_string < str.m_string;
}

BASE_API YkMBString operator+(YkAchar ch, const YkMBString &str)
{
	YkMBString strResult(ch);
	strResult += str;
	return strResult;
}

BASE_API YkMBString operator+(const YkAchar* pStr, const YkMBString &str)
{
	YkMBString strResult(pStr);
	strResult += str;
	return strResult;
}

void YkMBString::SetCharset(YkInt nCharset)
{
	m_nCharset =(Yk::YkMBString::Charset)nCharset;
}

YkMBString::Charset YkMBString::GetCharset() const
{
	return m_nCharset;
}

void YkMBString::Append(const YkAchar* pStr, YkInt nSize)
{
	m_string.append(pStr, nSize);
}

YkMBString::operator YKPCTSTR() const
{
	return m_string.c_str();
}

YkMBString::operator string() const
{
	return m_string.c_str();
}

YkMBString& YkMBString::TrimRightString( const YkMBString &str)
{
	int nSubLength = str.GetLength();
	if(nSubLength > 0)
	{
		YkInt nMainLength = (YkInt)m_string.length();
		while(nMainLength>=nSubLength && m_string.compare(nMainLength - nSubLength, nSubLength, str.Cstr()) == 0)
		{
			m_string.erase(m_string.begin() + nMainLength - nSubLength, m_string.end());
			nMainLength = (YkInt)m_string.length();
		}
	}
	return *this;	
}

YkMBString& YkMBString::TrimLeftString(const YkMBString &str)
{
	int nSubLength = str.GetLength();
	if(nSubLength > 0)
	{
		while(m_string.compare(0, nSubLength, str.Cstr()) == 0)
		{
			m_string.erase(m_string.begin(), m_string.begin() + nSubLength);
		}
	}
	return *this;
}
void YkMBString::Replace(YkInt startIndex, YkInt length, const YkAchar* pStrNew)
{
	m_string.replace(startIndex,length,pStrNew);
}

std::string&  YkMBString::ToStd(std::string& str) const
{
	str = m_string;
	return str;
}
YkMBString&  YkMBString::FromStd(const std::string& str)
{

	if(&str == &m_string)
	{
		return *this;
	}
	else
	{
		m_string = str;
		return *this;
	}
}

#if defined _UGUNICODE && !defined _UGQSTRING
const YkWchar g_wchTerminal=0x0000;

#define IsHighSurrogates(x) ((x)>0xD7FF && (x)<0xDB80 )
#define IsHighPrivateUseSurrogates(x) ((x)>0xDB7F && (x)<0xDC00 )
#define IsLowSurrogates(x) ((x)>0xDBFF && (x)<0xE000 )
#define IsSurrogates(x) ((x)>0xD7FF && (x)<0xE000 )
#define IsHexLittleLetter(x) ((x)>='a' && (x)<='f')
#define IsHexBigLetter(x) ((x)>='A' && (x)<='F')
#define IsDigit(x) ((x)>='0' && (x)<='9')
#define IsHexDigit(x) IsDigit(x)||IsHexBigLetter(x)||IsHexLittleLetter(x)

YkUnicodeString::YkUnicodeString()
{
	m_pString = new ((char*)&m_innerUnicodeString.pVftable) UnicodeString();
	InitCharset();
}

YkUnicodeString::YkUnicodeString( const YkUnicodeString& str )
{
	m_pString = new ((char*)&m_innerUnicodeString.pVftable) UnicodeString(str.m_pString->getTerminatedBuffer());
	SetCharset(str.GetCharset());
}

YkUnicodeString::YkUnicodeString( const YkWchar* pStr )
{
	m_pString = new ((char*)&m_innerUnicodeString.pVftable) UnicodeString((UChar*)pStr);

	InitCharset();
}

YkUnicodeString::YkUnicodeString( const YkWchar* pStr, YkInt nSize )
{
	m_pString = new ((char*)&m_innerUnicodeString.pVftable) UnicodeString((UChar*)pStr, nSize);
	InitCharset();
}

YkUnicodeString::YkUnicodeString( YkWchar ch, YkInt nRepeat/*=1*/ )
{
	m_pString = new ((char*)&m_innerUnicodeString.pVftable) UnicodeString(nRepeat, ch, nRepeat);
	InitCharset();
}

YkUnicodeString::~YkUnicodeString()
{
	if (m_pString!=NULL)
	{
		m_pString->~UnicodeString();
		m_pString = NULL;
	}
}

YkInt YkUnicodeString::GetStrLength(const YkWchar* pStr) const 
{
	const YkWchar *eos = pStr;

	while( *eos++ ) ;

	return( (YkInt)(eos - pStr - 1) );
}

void YkUnicodeString::Append( const YkWchar* pStr, YkInt nSize )
{
	m_pString->append((UChar*)pStr, nSize);
}

YkUnicodeString::operator YKPCWSTR() const
{
	return (const YkWchar*)m_pString->getTerminatedBuffer();
}

const YkWchar * YkUnicodeString::Cstr() const
{
	return (const YkWchar*)m_pString->getTerminatedBuffer();
}

YkWchar YkUnicodeString::GetAt( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return m_pString->charAt(nIndex);
}

Yk::YkWchar32 YkUnicodeString::GetWchar32At( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return (YkWchar32)m_pString->char32At(nIndex);
}

void YkUnicodeString::SetAt( YkInt nIndex, YkWchar ch )
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	m_pString->setCharAt(nIndex, ch);
}

YkWchar YkUnicodeString::operator[]( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return m_pString->charAt(nIndex);
}

YkWchar& YkUnicodeString::operator[]( YkInt nIndex )
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return *((YkWchar*)m_pString->getBuffer()+nIndex);
}

void YkUnicodeString::SetCapacity( YkInt nCapacity )
{
	if(nCapacity <= m_pString->getCapacity())
	{
		return;
	}
	YkInt nLength = m_pString->length();
	m_pString->getBuffer(nCapacity);
	m_pString->releaseBuffer(nLength);
}

Yk::YkInt YkUnicodeString::GetCapacity() const
{
	return m_pString->getCapacity();
}

Yk::YkInt YkUnicodeString::GetLength() const
{
	return m_pString->length();
}
YkInt YkUnicodeString::GetWchar32Length() const
{
	return m_pString->countChar32();
}

void YkUnicodeString::SetLength( YkInt nLength )
{
	if(nLength <0)
	{
		return ;
	}
	m_pString->getBuffer(nLength);
	m_pString->releaseBuffer(nLength);
}

YkUnicodeString& YkUnicodeString::MakeUpper()
{
	m_pString->toUpper();
	return *this;
}

YkUnicodeString& YkUnicodeString::MakeLower()
{
	m_pString->toLower();
	return *this;
}

YkUnicodeString& YkUnicodeString::MakeReverse()
{
	YkWchar *pCharTemp = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkWchar chTemp;
	YkInt i =0 ;
	for(;i<nLength-1;++i)
	{
		if(IsHighSurrogates(pCharTemp[i]))
		{
			chTemp = pCharTemp[i];
			pCharTemp[i] = pCharTemp[i+1];
			pCharTemp[i+1]=chTemp;
			++i;
		}
	}
	m_pString->reverse();
	return *this;
}	

Yk::YkUnicodeString YkUnicodeString::Left( YkInt nCount ) const
{
	if(nCount >0 && nCount < this->GetLength() )
	{
		return YkUnicodeString((const YkWchar*)m_pString->getBuffer(), nCount);
	}
	else if(nCount >= this->GetLength())
	{
		return *this;
	}
	else
	{
		return YkUnicodeString(L"");
	}
}

Yk::YkUnicodeString YkUnicodeString::Right( YkInt nCount ) const
{
	if(nCount > this->m_pString->length())
	{
		nCount = this->m_pString->length();
	}
	if(nCount >-1)
	{
		return YkUnicodeString((const YkWchar*)(m_pString->getBuffer()+GetLength()-nCount), nCount);
	}
	else
	{
		return *this;
	}
}

Yk::YkUnicodeString YkUnicodeString::Mid( YkInt nFirst ) const
{
	if(nFirst >= this->m_pString->length())
	{
		return YkUnicodeString(L"");
	}
	if(nFirst <0)
	{
		nFirst = 0;
	}
	return YkUnicodeString((const YkWchar*)(m_pString->getBuffer()+nFirst), GetLength() - nFirst);
}

Yk::YkUnicodeString YkUnicodeString::Mid( YkInt nFirst,YkInt nCount ) const
{
	if(nFirst >= this->m_pString->length() || nCount<=0)
	{
		return YkUnicodeString(L"");
	}
	if(nFirst <0)
	{
		nFirst = 0;
	}
	if(nFirst + nCount > m_pString->length())
	{
		nCount = m_pString->length() - nFirst;
	}

	return YkUnicodeString((const YkWchar*)(m_pString->getBuffer()+nFirst), nCount);	
}

Yk::YkInt YkUnicodeString::Delete( YkInt nIndex,YkInt nCount/*=1*/ )
{
	UChar* pBuffer = (UChar*)((UnicodeString*)m_pString)->getBuffer();
	YkInt nLength = ((UnicodeString*)m_pString)->length();
	YkInt nLast = nIndex +nCount;
	if( nLast < nLength)
	{
		u_memmove(pBuffer+nIndex,pBuffer+nLast,nLength-nLast);
		this->SetLength(nLength-nCount);
	}
	else if(nLast >=nLength)
	{
		if(nIndex <nLength)
		{
			this->SetLength(nIndex);
		}
	}
	return m_pString->length();

}

Yk::YkInt YkUnicodeString::Insert( YkInt nIndex,const YkWchar * pStr )
{
 	YkInt nLength = GetStrLength(pStr);
	m_pString->insert(nIndex, (const UChar*)pStr, nLength);
	return m_pString->length();
}

Yk::YkInt YkUnicodeString::Insert( YkInt nIndex,YkWchar ch,YkInt nCount/*=1*/ )
{
	if(nCount == 1)
	{
		m_pString->insert(nIndex, (UChar)ch);
	}
	else if(nCount>1)
	{
		YkInt i;
		YkWchar *pStr = new YkWchar[nCount+1];
		for(i=0;i<nCount;i++)
		{
			pStr[i] = ch;
		}
		pStr[i]=g_wchTerminal;
		Insert(nIndex,pStr);
		delete [] pStr;
	}
	return m_pString->length();
}

YkInt YkUnicodeString::Remove(YkWchar ch)
{
	UChar* pBuffer = (UChar*)((UnicodeString*)m_pString)->getBuffer();
	YkInt nLength = ((UnicodeString*)m_pString)->length();
	YkInt nCount = 0;
	UChar* pBufferTerminal = pBuffer+ nLength-1;

	if( nLength >0 )
	{
		YkInt i = 0;
		for( ; i<nLength ; ++i )
		{
			if(*pBuffer == ch)
			{
				++nCount;
				u_memmove(pBuffer,pBuffer+1,pBufferTerminal-pBuffer);
				--pBufferTerminal;
			}
			else
			{
				++pBuffer;
			}

		}
		if(nCount >0)
		{
			this->SetLength(nLength-nCount);
		}

	}
	return nCount;
}

YkUnicodeString& YkUnicodeString::TrimLeft(YkWchar ch)
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	YkBool bFind;
	for(i=0 ;i<nLength;++i)
	{
		bFind = FALSE;
		if(ch == pBuffer[i])
		{
			++nCount;
			bFind = TRUE;
		}
		if(!bFind)
		{
			break;
		}
	}
	m_pString->remove(0,nCount);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimLeft(const YkWchar* pStr)
{
	if(pStr == NULL)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	YkBool bFind;
	YkInt j;
	for(i=0 ;i<nLength;++i)
	{
		bFind = FALSE;
		j = 0;
		YkWchar wchTemp = pStr[j];
		while (wchTemp != g_wchTerminal)
		{
			if(wchTemp == pBuffer[i])
			{
				++nCount;
				bFind = TRUE;
				break;
			}
			wchTemp = pStr[++j];
		}

		if(!bFind)
		{
			break;
		}
	}
	m_pString->remove(0,nCount);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRight(const YkWchar* pStr)
{
	if(pStr == NULL)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	YkInt j;
	YkBool bFind;
	for(i=0 ;i<nLength;++i)
	{
		j = 0;
		bFind = FALSE;
		YkWchar wchTemp = pStr[j];
		while (wchTemp != g_wchTerminal)
		{
			if(wchTemp == pBuffer[nLength - i-1])
			{
				++nCount;
				bFind = TRUE;
				break;
			}
			wchTemp = pStr[++j];
		} 

		if(!bFind)
		{
			break;
		}
	}
	m_pString->remove(nLength - nCount,nLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRight(YkWchar ch)
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	YkBool bFind;
	for(i=nLength-1 ;i>=0;--i)
	{
		bFind = FALSE;
		if(ch == pBuffer[i])
		{
			++nCount;
			bFind = TRUE;
		}

		if(!bFind)
		{
			break;
		}
	}
	m_pString->remove(nLength - nCount,nLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRightString(const YkUnicodeString &str)
{
	YkInt nLength = m_pString->length();
	YkInt nReplaceLength = str.GetLength();
	if(nReplaceLength >nLength || nReplaceLength == 0)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkWchar *pReplaceBuffer = (YkWchar*)str.Cstr();
	YkWchar *pBufferTemp = pBuffer+nLength-nReplaceLength;
	
	while (memcmp(pBufferTemp,pReplaceBuffer,sizeof(YkWchar)*nReplaceLength) == 0)
	{
		pBufferTemp =pBufferTemp-nReplaceLength;
		if(pBufferTemp <pBuffer)
		{
			break;
		}
	}
	m_pString->remove(pBufferTemp+nReplaceLength-pBuffer,pBuffer+nLength-pBufferTemp+nReplaceLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimLeftString(const YkUnicodeString &str)
{
	YkInt nLength = m_pString->length();
	YkInt nReplaceLength = str.GetLength();
    if(nReplaceLength >nLength || nReplaceLength == 0)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkWchar *pReplaceBuffer = (YkWchar*)str.Cstr();
	YkWchar *pBufferTemp = pBuffer;

	while (memcmp(pBufferTemp,pReplaceBuffer,sizeof(YkWchar)*nReplaceLength) == 0)
	{
		pBufferTemp =pBufferTemp+ nReplaceLength;
		if(pBufferTemp>pBuffer+nLength-nReplaceLength)
		{
			break;
		}
	}
	m_pString->remove(0,pBufferTemp-pBuffer);
	return *this;
}

YkBool YkUnicodeString::IsEmpty() const
{
	return m_pString->length() > 0 ? FALSE: TRUE;
}

void YkUnicodeString::Empty()
{
	SetLength(0);
}

YkInt YkUnicodeString::Compare(const YkWchar* pStr) const
{
	YkInt nLength = GetStrLength(pStr);
	return m_pString->compare((const UChar*)pStr,nLength);
}

YkInt YkUnicodeString::CompareNoCase(const YkWchar* pStr) const
{
	YkInt nLength = GetStrLength(pStr);
	return m_pString->caseCompare((const UChar*)pStr,nLength,0);
}

YkInt YkUnicodeString::CompareNoCase(const YkUnicodeString& str) const
{
	return m_pString->caseCompare((const UChar*)str.Cstr(),str.GetLength(),0);
}

static const YkDouble JARO_WEIGHT_STRING_A(1.0 / 3.0);
static const YkDouble JARO_WEIGHT_STRING_B(1.0 / 3.0);
static const YkDouble JARO_WEIGHT_TRANSPOSITIONS(1.0 / 3.0);

static const YkInt JARO_WINKLER_PREFIX_SIZE(4);
static const YkDouble JARO_WINKLER_SCALING_FACTOR(0.1);
static const YkDouble JARO_WINKLER_BOOST_THRESHOLD(0.7);

YkDouble YkUnicodeString::CalculateJaroDistance(const YkUnicodeString& str) const
{
	// Register strings length.
	YkInt aLength(m_pString->length());
	YkInt bLength(str.GetLength());

	// If one string has null length, we return 0.
	if (aLength == 0 || bLength == 0)
	{
		return 0.0;
	}

	// Calculate max length range.
	YkInt maxRange(YKMAX(0, YKMAX(aLength, bLength) / 2 - 1));

	// Creates 2 vectors of integers.
	std::vector<YkBool> aMatch(aLength, false);
	std::vector<YkBool> bMatch(bLength, false);

	// Calculate matching characters.
	YkInt matchingCharacters(0);
	for (YkInt aIndex(0); aIndex < aLength; ++aIndex)
	{
		// Calculate window test limits (limit inferior to 0 and superior to bLength).
		YkInt minIndex(YKMAX(aIndex - maxRange, 0));
		YkInt maxIndex(YKMIN(aIndex + maxRange + 1, bLength));

		if (minIndex >= maxIndex)
		{
			// No more common character because we don't have characters in b to test with characters in a.
			break;
		}

		for (YkInt bIndex(minIndex); bIndex < maxIndex; ++bIndex)
		{
			if (!bMatch.at(bIndex) && this->GetAt(aIndex) == str.GetAt(bIndex))
			{
				// Found some new match.
				aMatch[aIndex] = true;
				bMatch[bIndex] = true;
				++matchingCharacters;
				break;
			}
		}
	}

	// If no matching characters, we return 0.
	if (matchingCharacters == 0)
	{
		return 0.0;
	}

	// Calculate character transpositions.
	std::vector<YkInt> aPosition(matchingCharacters, 0);
	std::vector<YkInt> bPosition(matchingCharacters, 0);

	for (YkInt aIndex(0), positionIndex(0); aIndex < aLength; ++aIndex)
	{
		if (aMatch.at(aIndex))
		{
			aPosition[positionIndex] = aIndex;
			++positionIndex;
		}
	}

	for (YkInt bIndex(0), positionIndex(0); bIndex < bLength; ++bIndex)
	{
		if (bMatch.at(bIndex))
		{
			bPosition[positionIndex] = bIndex;
			++positionIndex;
		}
	}

	// Counting half-transpositions.
	YkInt transpositions(0);
	for (YkInt index(0); index < matchingCharacters; ++index)
	{
		if (this->GetAt(aPosition.at(index)) != str.GetAt(bPosition.at(index)))
		{
			++transpositions;
		}
	}

	// Calculate Jaro distance.
	return (
		JARO_WEIGHT_STRING_A * matchingCharacters / aLength +
		JARO_WEIGHT_STRING_B * matchingCharacters / bLength +
		JARO_WEIGHT_TRANSPOSITIONS * (matchingCharacters - transpositions / 2) / matchingCharacters
		);
}

YkDouble YkUnicodeString::CalculateDistance(const YkUnicodeString& str) const
{
	YkDouble distance(CalculateJaroDistance(str));

	YkInt aLength(m_pString->length());
	YkInt bLength(str.GetLength());

	if (distance > JARO_WINKLER_BOOST_THRESHOLD)
	{
		// Calculate common string prefix.
		int commonPrefix(0);
		for (int index(0), indexEnd(YKMIN(YKMIN(aLength, bLength), JARO_WINKLER_PREFIX_SIZE)); index < indexEnd; ++index)
		{
			if (this->GetAt(index) == str.GetAt(index))
			{
				++commonPrefix;
			}
			else
			{
				break;
			}
		}

		// Calculate Jaro-Winkler distance.
		distance += JARO_WINKLER_SCALING_FACTOR * commonPrefix * (1.0 - distance);
	}

	return distance;
}

YkInt YkUnicodeString::Find(YkWchar ch,YkInt nStart/*=0*/) const
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt i=nStart;
	for(;i<nLength;++i)
	{
		if(pBuffer[i] == ch)
		{
			return i;
		}
	}
	return -1;
}

YkInt YkUnicodeString::Find(const YkWchar* pStrSub, YkInt nStart/*=0*/) const
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nSrcLength = GetStrLength(pStrSub);
	YkInt i=nStart;
	for(;i<nLength;++i)
	{
		if(nSrcLength+i > nLength)
		{
			return -1;
		}
		if(memcmp(pStrSub,pBuffer+i,sizeof(YkWchar)*nSrcLength) == 0)
		{
			return i;
		}
	}
	return -1;
}

YkInt YkUnicodeString::ReverseFind(YkWchar ch,YkInt nStart) const
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	if(nStart<0 || nStart>nLength)
	{
		nStart = nLength;
	}

	YkInt i=nStart -1 ;
	for(;i>=0;--i)
	{
		if(pBuffer[i] == ch)
		{
			return i;
		}
	}
	return -1;
}

YkInt YkUnicodeString::ReverseFind(const YkWchar* pStrSub,YkInt nStart) const
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nSrcLength = GetStrLength(pStrSub);
	if(nLength<nSrcLength)
	{
		return -1;
	}

	if(nStart<0 || nStart>(nLength-nSrcLength))
	{
		nStart = nLength-nSrcLength;
	}
	YkInt i=nStart;
	for(;i>=0;--i)
	{
		if(memcmp(pStrSub,pBuffer+i,sizeof(YkWchar)*nSrcLength) == 0)
		{
			return i;
		}
	}
	return -1;

}

YkInt YkUnicodeString::FindOneOf(const YkWchar* pStrCharSet) const
{
	if(pStrCharSet == NULL)
	{
		return -1;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkWchar wchTemp;
	YkInt i = 0;
	YkInt j;
	for(;i<nLength;++i)
	{
		j = 0;
		wchTemp = pStrCharSet[j];
		while (wchTemp !=g_wchTerminal)
		{
			if(wchTemp == pBuffer[i])
			{
				return i;
			}
			wchTemp = pStrCharSet[++j];
		}
	}
	return -1;
}

YkInt YkUnicodeString::FindNotOneOf(const YkWchar* pStrCharSet) const
{
	if(pStrCharSet == NULL)
	{
		return -1;
	}
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	YkInt j ;
	YkBool bFind;
	for(;i<nLength;++i)
	{
		bFind = FALSE;
		j = 0;
		YkWchar wchTemp = pStrCharSet[j];
		while (wchTemp != g_wchTerminal)
		{
			if(wchTemp == pBuffer[i])
			{
				++nCount;
				bFind = TRUE;
				break;
			}
			wchTemp = pStrCharSet[++j];
		} 

		if(!bFind)
		{
			return i;
		}
	}
	return i;
}

YkUnicodeString YkUnicodeString::SpanIncluding(const YkWchar* pStrCharSet) const
{
	YkInt nPos = FindNotOneOf(pStrCharSet);
	return Left(nPos);
}

YkUnicodeString YkUnicodeString::SpanExcluding(const YkWchar* pStrCharSet) const
{
	YkInt nPos = FindOneOf(pStrCharSet);
	if(nPos == -1) 
	{ 
		return *this;
	} 
	else
	{ 
		return Left(nPos);
	}
}

YkInt YkUnicodeString::Replace(YkWchar chOld,YkWchar chNew)
{
	YkWchar *pBuffer = (YkWchar*)m_pString->getBuffer();
	YkInt nLength = m_pString->length();
	YkInt nCount = 0;
	YkInt i = 0;
	for(;i<nLength;i++)
	{
		if(pBuffer[i] == chOld)
		{
			pBuffer[i] = chNew;
			++nCount;
		}
	}
	return nCount;
}

YkInt YkUnicodeString::Replace(const YkWchar* pStrOld,const YkWchar* pStrNew)
{
	YkInt nOldLength = u_strlen((const UChar*)pStrOld);
	YkInt nNewLength = u_strlen((const UChar*)pStrNew);
	YkInt nLength = this->GetLength();
	YkInt nTemp = nLength - nOldLength; //位置偏移
	if(nTemp <0 || nOldLength<=0)
	{
		return 0;
	}
	YkInt nCount = 0;
	YkWchar *pBuffer = (YkWchar *)m_pString->getBuffer();
	YkWchar *pBufferTemp = pBuffer;
	YkWchar *pBufferTerminal = pBuffer+ nLength-nOldLength;
	if(nOldLength > nNewLength)
	{

		while( (pBufferTemp = YK_memchr(pBufferTemp,*pStrOld,nTemp+1)) != NULL)
		{
			if(YK_memcmp(pBufferTemp,pStrOld,nOldLength) == 0)
			{
				nTemp = pBufferTerminal - pBufferTemp;
				u_memmove((UChar *)pBufferTemp+nNewLength,(UChar *)pBufferTemp+nOldLength,nTemp);	
				u_memmove((UChar *)pBufferTemp,(UChar *)pStrNew,nNewLength);
				++nCount;	
				pBufferTemp = pBufferTemp+ nNewLength;
				pBufferTerminal = pBufferTerminal+(nNewLength-nOldLength);
			}
			else
			{
				++pBufferTemp;
			}
			nTemp = pBufferTerminal - pBufferTemp;
			if(nTemp<0)
			{
				break;
			}
		}	
		this->SetLength(nLength+(nNewLength-nOldLength)*nCount);


	}
	else if(nOldLength <nNewLength)
	{
		//一开始先开辟临时内存，防止new耗费时间
		const YkInt nOrginalTemp = 100;
		YkInt nCountTemp = nOrginalTemp;
		YkInt arrPosTemp[nOrginalTemp];
		YkInt* arrPos = arrPosTemp;
		while( (pBufferTemp = YK_memchr(pBufferTemp,*pStrOld,nTemp+1)) != NULL)
		{
			if(YK_memcmp(pBufferTemp,pStrOld,nOldLength) == 0)
			{
				if(nCount == nCountTemp)
				{
					YkInt* pPosTemp = new YkInt[nCountTemp+nOrginalTemp];
					memcpy(pPosTemp,arrPos,nCountTemp*sizeof(YkInt));
					if(nCountTemp != nOrginalTemp)
					{
						delete[] arrPos;
					}
					arrPos = pPosTemp;
					nCountTemp = nCountTemp+nOrginalTemp;

				}
				arrPos[nCount] = pBufferTemp-pBuffer;
				++nCount;
				pBufferTemp = pBufferTemp+nOldLength;

			}
			else
			{
				++pBufferTemp;
			}
			nTemp = pBufferTerminal - pBufferTemp;
			if(nTemp<0)
			{
				break;
			}
		}	
		this->SetLength(nLength+ (nNewLength-nOldLength)*nCount);
		//重新获取内存指针
		pBuffer = (YkWchar *)m_pString->getBuffer();

		YkInt nLastPos = nLength;
		YkWchar *pForm;
		YkWchar *pTo;
		for(YkInt i = nCount-1;i>=0;--i)
		{
			nLastPos = (i == nCount-1)?nLength:arrPos[i+1];
			pForm = pBuffer+arrPos[i]+nOldLength;
			pTo = pBuffer+arrPos[i]+nOldLength+(i+1)*(nNewLength-nOldLength);
			u_memmove((UChar *)pTo,(UChar *)pForm,(nLastPos-arrPos[i]-nOldLength));
			pTo = pBuffer+arrPos[i]+i*(nNewLength-nOldLength);
			u_memmove((UChar *)pTo,(UChar *)pStrNew,nNewLength);
		}
		if(arrPos != arrPosTemp)
		{
			delete[] arrPos;
		}
	}
	else if(nOldLength == nNewLength)
	{
		while((pBufferTemp = YK_memchr(pBufferTemp,*pStrOld,nTemp+1)) != NULL)
		{
			if(YK_memcmp(pBufferTemp,pStrOld,nOldLength) == 0)
			{
				u_memmove((UChar *)pBufferTemp,(UChar *)pStrNew,nNewLength);
				++nCount;
				pBufferTemp = pBufferTemp +nNewLength;

			}
			else
			{
				++pBufferTemp;
			}
			nTemp = pBufferTerminal - pBufferTemp;
			if(nTemp<0)
			{
				break;
			}

		}
	}
	return nCount;
}

void YkUnicodeString::Format(const YkWchar* pStr,...)
{
	if(pStr && *pStr)
	{
		YkInt len=0;
		YkUint nCapacity=GetStrLength(pStr);
		nCapacity = nCapacity +512;
		
		m_pString->getBuffer(nCapacity);
		m_pString->releaseBuffer(nCapacity);

		va_list args;	
		va_start(args,pStr);

		len=YK_vsnprintf((YkWchar*)m_pString->getTerminatedBuffer(),nCapacity,pStr,args);
        
		if(len > (YkInt)nCapacity)
		{
			m_pString->getBuffer(len);
			m_pString->releaseBuffer(len);
			va_end(args);	
			va_start(args,pStr);
			len=YK_vsnprintf((YkWchar*)m_pString->getTerminatedBuffer(),len,pStr,args);
			

		}
		else
		{
			m_pString->getBuffer(len);
			m_pString->releaseBuffer(len);
		}
		va_end(args);	
	}
}

const YkUnicodeString& YkUnicodeString::operator=(const YkUnicodeString& str)
{
	if(this != &str)
	{
		*this = str.Cstr();
		//拷贝的时候把字符编码也拷贝过去
		this->SetCharset(str.GetCharset());
	}
	return *this;
}

const YkUnicodeString& YkUnicodeString::operator=(const YkWchar* pStr)
{
	InitCharset();
	if(pStr != NULL)
	{
		if(m_pString->getBuffer() != (const UChar*)pStr)
		{
	 		YkInt nLength = GetStrLength(pStr);
			UChar* pBuffer = m_pString->getBuffer(nLength);
            if(pBuffer)
            {
                u_memcpy(pBuffer, (const UChar*)pStr, nLength);
                m_pString->releaseBuffer(nLength);
            }
            else
            {
                printf("*** ugString buffer is null *** \n");
            }
		}
	}
	return *this;
}

YkUnicodeString YkUnicodeString::operator+(const YkWchar ch) const
{
	return YkUnicodeString(*this)+=ch;
}

YkUnicodeString YkUnicodeString::operator+(const YkWchar* pStr) const
{
	return YkUnicodeString(*this) += pStr;
}

YkUnicodeString YkUnicodeString::operator+(const YkUnicodeString& str) const
{
	return YkUnicodeString(*this) += str;
}

const YkUnicodeString& YkUnicodeString::operator+=(const YkUnicodeString& str)
{
	*m_pString += *str.m_pString;
	return *this;
}

const YkUnicodeString& YkUnicodeString::operator+=(const YkWchar ch)
{
	*m_pString+=(UChar)ch;
	return *this;
}

const YkUnicodeString& YkUnicodeString::operator+=(const YkWchar* pStr)
{
	m_pString->append((const UChar*)pStr, u_strlen((const UChar*)pStr));
	return *this;
}

bool YkUnicodeString::operator==(const YkWchar* pStr) const
{
	YkInt nLength = u_strlen((const UChar*)pStr);
	return m_pString->compare((UChar*)pStr,nLength) == 0;
}

bool YkUnicodeString::operator==(const YkUnicodeString& str) const
{
	return m_pString->compare(*(str.m_pString)) == 0;
}

BASE_API bool operator==( const YkWchar* pStr, const YkUnicodeString& str )
{
	return str == pStr;
}

bool YkUnicodeString::operator!=(const YkWchar* pStr) const
{
	return !(*this == pStr);
}

bool YkUnicodeString::operator!=(const YkUnicodeString& str) const
{
	return !(*this == str);
}

bool YkUnicodeString::operator<(const YkUnicodeString& str) const
{
#pragma warning (disable: 4800)
	return *m_pString< *(str.m_pString);
#pragma warning (disable: 4800)
}

BASE_API YkUnicodeString operator+(YkWchar ch, const YkUnicodeString &str)
{
	return YkUnicodeString(ch)+=str;
}

BASE_API YkUnicodeString operator+(const YkWchar* pStr, const YkUnicodeString &str)
{
	return YkUnicodeString(pStr)+=str;
}

YkUnicodeString YkUnicodeString::From(YkLong lVal, YkUint base)
{
	YkBool bNegative = FALSE; // 记录是否为负数
	if (base == 10 && lVal < 0)
	{
		bNegative = TRUE;
		lVal = -lVal;
	}
	YkUnicodeString strResult = From((YkUlong)lVal, base);
	if (bNegative)
	{
		strResult = YkUnicodeString(L"-" + strResult); // 十进制下的负数,前面要加-
	}
	return strResult;
}

YkUnicodeString YkUnicodeString::From(YkUlong lVal, YkUint base)
{
	YkUnicodeString strResult;
	do {
		YkUint nNum = 0;
		nNum = (YkUint)(lVal % base);
		lVal /= base;
		if (nNum > 9)
		{ // 十六进制
			strResult += YkWchar(nNum-10+L'A');
		}
		else 
		{ // 十进制
			strResult += YkWchar(nNum+L'0');
		}
	} while(lVal > 0);
	// 最后反转结果
	strResult.MakeReverse();
	if (base == 16)
	{
		strResult = L"0X" + strResult;
	}
	return strResult;
}

YkUnicodeString YkUnicodeString::From(YkInt nVal, YkUint base)
{
	return From((YkLong)nVal, base);
}

YkUnicodeString YkUnicodeString::From(YkUint nVal, YkUint base)
{
	return From((YkUlong)nVal, base);
}

YkUnicodeString YkUnicodeString::From( unsigned long nVal, YkUint base/*=10*/ )	WhiteBox_Ignore
{
	return From((YkUlong)nVal, base);
}

YkInt YkUnicodeString::ToInt(YkUint base) const
{
	return (YkInt)ToLong(base); 
}

YkUint YkUnicodeString::ToUInt(YkUint base) const
{
	return (YkUint)ToLong(base);
}

YkLong YkUnicodeString::ToLong(YkUint base) const
{
	if(this->GetLength() ==0)
	{
		return 0;
	}
	YkLong lVal = 0;
	if (base == 10 && this->GetAt(0) == L'-')
	{
		YkUlong ulVal = ToULong(base);
		lVal = YkLong(ulVal * -1);
	}
	else
	{
		lVal = (YkLong)ToULong(base);
	}
	return lVal;
}

YkUlong YkUnicodeString::ToULong(YkUint base) const
{
	YkUnicodeString strTemp = *this;
	strTemp.Remove(_U(','));
	strTemp.TrimLeft();
	strTemp.TrimRight();

	//记录那个pos开始
	YkInt nPos = 0;

	const YkWchar *pBuffer = strTemp.Cstr();
	YkInt nLen = strTemp.GetLength();


	YkUlong ulVal = 0;

	if (base == 10)
	{
		if(nLen >0)
		{
			if(pBuffer[0]== L'-')
			{
				nPos = 1;
			}
		}

		for (int i=nPos; i<nLen; i++)
		{
			YkWchar32 ch = pBuffer[i];
			if (IsDigit(ch))
			{
				ulVal = ulVal*10 + (ch-L'0');
			}
			else
			{
				break;
			}
		}
	}
	else if (base == 16) 
	{
		
		if(nLen >1)
		{
			if(pBuffer[0]== L'0' && (pBuffer[1] == L'x'||pBuffer[1] == L'X'))
			{
				nPos = 2;
			}
		}

		for (int i=nPos; i<nLen; i++)
		{
			YkWchar ch = pBuffer[i];
			if (IsHexDigit(ch))
			{
				ulVal = ulVal*16;
				if (IsDigit(ch)) 
				{
					ulVal += ch - L'0';
				}
				else if (IsHexBigLetter(ch))
				{
					ulVal += ch - 'A' + 10;
				}
				else if (IsHexLittleLetter(ch))
				{
					ulVal += ch - L'a' + 10;
				}
			}
			else
			{
				break;
			}
		}
	}
	return ulVal;
}

YkFloat YkUnicodeString::ToFloat() const
{
	return (YkFloat)ToDouble(); 
}

YkDouble YkUnicodeString::ToDouble() const
{
	YkUnicodeString strTemp = *this;
	strTemp.TrimLeft();
	strTemp.TrimRight();
	
	static NumberFormat *g_uNumberTransltor = NULL;
	if(g_uNumberTransltor == NULL)
	{
		UErrorCode err = U_ZERO_ERROR;
		g_uNumberTransltor = NumberFormat::createInstance(Locale("en_US"), err);
	}
	if(g_uNumberTransltor != NULL)
	{
		UErrorCode status = U_ZERO_ERROR;
		Formattable fmtTarget;
		g_uNumberTransltor->parse(*strTemp.m_pString, fmtTarget, status);
		if(U_FAILURE(status))
		{
			return 0.0;
		}
		switch(fmtTarget.getType())
		{
		case Formattable::kInt64:
			return (YkDouble)fmtTarget.getInt64();
		case Formattable::kLong:
			return (YkDouble)fmtTarget.getLong();
		default:
			return (YkDouble)fmtTarget.getDouble();
		}
	}
	return 0.0;
}

YkInt YkUnicodeString::Split(YkUnicodeStringArray &strDest,const YkWchar* pStrDelims, YkBool bKeepEmptyString) const
{
	strDest.SetSize(0);
	const YkWchar * pdelims = pStrDelims;

	int nLimIndex = 0;
	int nLimLen = GetStrLength(pStrDelims);
	int nContentIndex = 0;
	YkBool bContinue = TRUE;
	YkUnicodeString strSub;
	while (bContinue)
	{
		nLimIndex = Find(pdelims, nContentIndex);
		if (nLimIndex == -1)
		{
			bContinue = FALSE;
		}
		else
		{
			strSub = Mid(nContentIndex, nLimIndex-nContentIndex);
			if(bKeepEmptyString || strSub.GetLength() > 0)
			{
				strDest.Add(strSub);
			}
			nContentIndex += (nLimIndex-nContentIndex) + nLimLen;
		}
	}
	// 最后一个分隔符 后的内容加入
	strSub = Mid(nContentIndex, this->GetLength() - nContentIndex);
	if(bKeepEmptyString || strSub.GetLength() > 0)
	{
		strDest.Add(strSub);
	}
	return strDest.GetSize();
}

void YkUnicodeString::SetCharset(YkInt nCharset)
{
	m_nCharset =(Yk::YkMBString::Charset)nCharset;
}

YkMBString::Charset YkUnicodeString::GetCharset() const
{
	return m_nCharset;
}

void YkUnicodeString::Replace(YkInt startIndex, YkInt length, const YkWchar* pStrNew)
{
 	m_pString->replace(startIndex,length,(const UChar*)pStrNew,GetStrLength(pStrNew));
}

YkUnicodeString::operator UnicodeString() const
{
	return *m_pString;
}

Yk::YkInt YkUnicodeString::WcharToAchar( const YkWchar* pWchar, YkAchar *pAchar, YkInt nAcharLength )
{
	YkInt nWcharLength = u_strlen((const UChar*)pWchar);

	if(pWchar == NULL || pAchar == NULL || nAcharLength < nWcharLength * 3 + 1)
		return 0;
	u_austrcpy(pAchar, (const UChar*)pWchar);
	return strlen(pAchar);
}

Yk::YkInt YkUnicodeString::AcharToWchar( const YkAchar* pAchar, YkWchar *pWchar, YkInt nWcharLength )
{
	YkInt nAcharLength = strlen(pAchar);
	if(pAchar == NULL || pWchar == NULL || nWcharLength < nAcharLength * 3)
		return 0;
	u_uastrcpy((UChar*)pWchar, pAchar);
	return u_strlen((const UChar*)pWchar);
}

Yk::YkInt YkUnicodeString::WcharToWchar32( const YkWchar* pWchar, YkWchar32* pWchar32, YkInt nWchar32Length )
{
	YkInt nWcharLength = u_strlen((const UChar*)pWchar);
	if(pWchar == NULL || pWchar32 == NULL || nWchar32Length < nWcharLength)
	{
		return 0;
	}
	UErrorCode err = UErrorCode(0);
	YkInt nDestLength = 0;
	u_strToUTF32((UChar32*)pWchar32, nWchar32Length, &nDestLength,(const UChar*)pWchar, nWcharLength, &err);
	return nDestLength;
}

Yk::YkInt YkUnicodeString::Wchar32ToWchar( const YkWchar32* pWchar32, YkWchar* pWchar, YkInt nWcharLength )
{
	YkInt nWchar32Length = u_str32len(pWchar32);
	if(pWchar == NULL || pWchar32 == NULL || nWcharLength < nWchar32Length*2)
	{
		return 0;
	}
	UErrorCode err = UErrorCode(0);
	YkInt nDestLength = 0;
	u_strFromUTF32((UChar*)pWchar, nWcharLength, &nDestLength,(const UChar32 *)pWchar32, nWchar32Length, &err);
	return nDestLength;
}

YkUnicodeString& YkUnicodeString::FromUTF8( const YkAchar *pSrc, YkInt nSrcLength )
{
	YkWchar *pBuffer = (YkWchar *)m_pString->getBuffer(nSrcLength * 4);
	YkInt nDestLength = 0;
	UErrorCode err = UErrorCode(0);
	u_strFromUTF8((UChar*)pBuffer, nSrcLength * 4, &nDestLength, pSrc, nSrcLength, &err);
	m_pString->releaseBuffer(nDestLength);
	return *this;
}

YkMBString& YkUnicodeString::ToUTF8( YkMBString& strResult ) const
{
	strResult.SetLength(GetLength() * 4);
	YkInt nResultLength = 0;
	UErrorCode err = UErrorCode(0);
	u_strToUTF8((YkAchar*)strResult.Cstr(), strResult.GetCapacity(), &nResultLength, (const UChar*)Cstr(), GetLength(), &err);
	strResult.SetLength(nResultLength);
	return strResult;
}

std::string&  YkUnicodeString::ToStd(std::string& str,YKCharset::Charset charset/*= YkUnicodeString::UTF8*/) const
{
	YkString strTargetU = YkSystem::GetCharsetNameICU(charset);
	YkMBString strTarget;
	strTargetU.ToUTF8(strTarget);
	char *pSrcCharset =	NULL ;
	if(YK_ISBIGENDIAN)
	{
		pSrcCharset	= "UTF-16BE";
	}
	else
	{
		pSrcCharset = "UTF-16LE";

	}
	YkInt nBufferLength =m_pString->length()*4; 
	str.resize(nBufferLength);
	UErrorCode pErrorCode = UErrorCode(0);
	
	YkInt nLength  =ucnv_convert(strTarget.Cstr(),pSrcCharset,(char*)str.c_str(),nBufferLength,(const char*)m_pString->getBuffer(),m_pString->length()*2,&pErrorCode);

	if(nLength>nBufferLength)
	{
		str.resize(nLength);
		pErrorCode = UErrorCode(0);
		nLength  =ucnv_convert(strTarget.Cstr(),pSrcCharset,(char*)str.c_str(),nLength,(const char*)m_pString->getBuffer(),m_pString->length()*2,&pErrorCode);
	}
	else
	{
		str.resize(nLength);
	}
	return str;
}

YkUnicodeString&  YkUnicodeString::FromMBString(const YkAchar *pSrc, YkInt nSrcLength,YKCharset::Charset charset/* = YKCharset::Default*/)
{
	YkString strSrcU = YkSystem::GetCharsetNameICU(charset);
	YkMBString strSrc;
	strSrcU.ToUTF8(strSrc);
	char *pTarCharset =	NULL ;
	if(YK_ISBIGENDIAN)
	{
		pTarCharset	= "UTF-16BE";
	}
	else
	{
		pTarCharset = "UTF-16LE";

	}
	this->SetLength(nSrcLength);
	UErrorCode pErrorCode = UErrorCode(0);

	YkInt nLength  = ucnv_convert(pTarCharset,strSrc,(char*)m_pString->getBuffer(),nSrcLength*2,pSrc,nSrcLength,&pErrorCode);

	if(nLength>nSrcLength*2)
	{
		this->SetLength(nLength/2);
		pErrorCode = UErrorCode(0);
		nLength  =ucnv_convert(pTarCharset,strSrc,(char*)m_pString->getBuffer(),nSrcLength*2,pSrc,nSrcLength,&pErrorCode);
	}
	else
	{
		this->SetLength(nLength/2);
	}
	return *this;
}

#endif

EXTERN_C BASE_API void Unicode2MBString(const YkString& strUnicode, YkMBString& strMB)
{
#ifdef _UGUNICODE
	strUnicode.ToMBString(strMB);
#else
	strMB = strUnicode;
#endif
}

EXTERN_C BASE_API  void MBString2Unicode( const YkMBString& strMB, YkString& strUnicode )
{
#ifdef _UGUNICODE
	strUnicode.FromMBString(strMB.Cstr(), strlen(strMB.Cstr()));
#else
	strUnicode=strMB;
#endif	
}

}
