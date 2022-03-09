/*
 *
 * YkQString.cpp
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
#if defined _UGUNICODE && defined _UGQSTRING
#include "QtCore/qvector.h"
#include "QtCore/qendian.h"
#include "QtCore/qtextcodec.h"
#include "QtCore/qstringlist.h"
enum { Endian = 0, Data = 1 };
#endif
#include "Base/YkString.h"
#include "Base/YkSystem.h"
#include "Base/YkPlatform.h"

using namespace std;

namespace Yk {

#if defined _UGUNICODE && defined _UGQSTRING
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
	m_qString = new QString();
	InitCharset();
}

YkUnicodeString::YkUnicodeString( const YkUnicodeString& str )
{
	m_qString = new QString(str.m_qString->data(), str.m_qString->length());
	//拷贝构造的时候，就把原来的字符编码也拷贝上
	SetCharset(str.GetCharset());
}

YkUnicodeString::YkUnicodeString( const YkWchar* pStr )
{
	m_qString = new QString((const QChar*)pStr); WhiteBox_Ignore
	InitCharset();
}

YkUnicodeString::YkUnicodeString( const YkWchar* pStr, YkInt nSize )
{
	m_qString = new QString((const QChar*)pStr, nSize); WhiteBox_Ignore
	InitCharset();
}

YkUnicodeString::YkUnicodeString( YkWchar ch, YkInt nRepeat/*=1*/ )
{
	m_qString = new QString(nRepeat, QChar(ch));
	InitCharset();
}

YkUnicodeString::~YkUnicodeString()
{
	if (m_qString!=NULL)
	{
		delete m_qString;
		m_qString = NULL;
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
	m_qString->append(QString((const QChar*)pStr, nSize));
}

YkUnicodeString::operator YKPCWSTR() const
{
	return (const YkWchar*)m_qString->constData();
}

const YkWchar * YkUnicodeString::Cstr() const
{
	return (const YkWchar*)m_qString->constData();
}

YkWchar YkUnicodeString::GetAt( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return (YkWchar)m_qString->at(nIndex).unicode();
}

Yk::YkWchar32 YkUnicodeString::GetWchar32At( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	// TODO
	return 0;
}

void YkUnicodeString::SetAt( YkInt nIndex, YkWchar ch )
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	(*((YkWchar*)m_qString->data()+nIndex)) = ch;
}

YkWchar YkUnicodeString::operator[]( YkInt nIndex ) const
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return (YkWchar)m_qString->at(nIndex).unicode();
}

YkWchar& YkUnicodeString::operator[]( YkInt nIndex )
{
	YKASSERT(nIndex>=0 && nIndex<GetLength());
	return *((YkWchar*)m_qString->data()+nIndex);
}

void YkUnicodeString::SetCapacity( YkInt nCapacity )
{
	if(nCapacity <= m_qString->capacity())
	{
		return;
	}

	m_qString->reserve(nCapacity);
}

Yk::YkInt YkUnicodeString::GetCapacity() const
{
	return m_qString->capacity();
}

Yk::YkInt YkUnicodeString::GetLength() const
{
	return m_qString->length();
}
YkInt YkUnicodeString::GetWchar32Length() const
{
	return 0;
}

void YkUnicodeString::SetLength( YkInt nLength )
{
	if(nLength <0)
	{
		return ;
	}
	m_qString->resize(nLength);
}

YkUnicodeString& YkUnicodeString::MakeUpper()
{
	QString strUpper = m_qString->toUpper();
	m_qString->setUnicode(strUpper.constData(), strUpper.length());
	return *this;
}

YkUnicodeString& YkUnicodeString::MakeLower()
{
	QString strLower = m_qString->toLower();
	m_qString->setUnicode(strLower.constData(), strLower.length());
	return *this;
}

YkUnicodeString& YkUnicodeString::MakeReverse()
{
	// TODO
	return *this;
}	

Yk::YkUnicodeString YkUnicodeString::Left( YkInt nCount ) const
{
	if(nCount >0 && nCount < this->GetLength() )
	{
		return YkUnicodeString((const YkWchar*)m_qString->constData(), nCount);
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
	if(nCount > this->m_qString->length())
	{
		nCount = this->m_qString->length();
	}
	if(nCount >-1)
	{
		return YkUnicodeString((const YkWchar*)(m_qString->constData() + GetLength()-nCount), nCount);
	}
	else
	{
		return *this;
	}
}

Yk::YkUnicodeString YkUnicodeString::Mid( YkInt nFirst ) const
{
	if(nFirst >= this->m_qString->length())
	{
		return YkUnicodeString(L"");
	}
	if(nFirst <0)
	{
		nFirst = 0;
	}
	return YkUnicodeString((const YkWchar*)(m_qString->constData()+nFirst), GetLength() - nFirst);
}

Yk::YkUnicodeString YkUnicodeString::Mid( YkInt nFirst,YkInt nCount ) const
{
	if(nFirst >= this->m_qString->length() || nCount<=0)
	{
		return YkUnicodeString(L"");
	}
	if(nFirst <0)
	{
		nFirst = 0;
	}
	if(nFirst + nCount > m_qString->length())
	{
		nCount = m_qString->length() - nFirst;
	}

	return YkUnicodeString((const YkWchar*)(m_qString->constData()+nFirst), nCount);	
}

Yk::YkInt YkUnicodeString::Delete( YkInt nIndex,YkInt nCount/*=1*/ )
{
	m_qString->remove(nIndex, nCount);
	return m_qString->length();
}

Yk::YkInt YkUnicodeString::Insert( YkInt nIndex,const YkWchar * pStr )
{
 	YkInt nLength = GetStrLength(pStr);
	m_qString->insert(nIndex, (const QChar*)pStr, nLength);
	return m_qString->length();
}

Yk::YkInt YkUnicodeString::Insert( YkInt nIndex,YkWchar ch,YkInt nCount/*=1*/ )
{
	if(nCount == 1)
	{
		m_qString->insert(nIndex, (QChar)ch);
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
	return m_qString->length();
}

YkInt YkUnicodeString::Remove(YkWchar ch)
{
	YkInt nOldLength = m_qString->length();
	m_qString->remove(QChar(ch));
	return nOldLength - m_qString->length();
}

YkUnicodeString& YkUnicodeString::TrimLeft(YkWchar ch)
{
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	m_qString->remove(0,nCount);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimLeft(const YkWchar* pStr)
{
	if(pStr == NULL)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	m_qString->remove(0,nCount);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRight(const YkWchar* pStr)
{
	if(pStr == NULL)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	m_qString->remove(nLength - nCount,nLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRight(YkWchar ch)
{
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	m_qString->remove(nLength - nCount,nLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimRightString(const YkUnicodeString &str)
{
	YkInt nLength = m_qString->length();
	YkInt nReplaceLength = str.GetLength();
	if(nReplaceLength >nLength || nReplaceLength == 0)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
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
	m_qString->remove(pBufferTemp+nReplaceLength-pBuffer,pBuffer+nLength-pBufferTemp+nReplaceLength);
	return *this;
}

YkUnicodeString& YkUnicodeString::TrimLeftString(const YkUnicodeString &str)
{
	YkInt nLength = m_qString->length();
	YkInt nReplaceLength = str.GetLength();
    if(nReplaceLength >nLength || nReplaceLength == 0)
	{
		return *this;
	}
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
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
	m_qString->remove(0,pBufferTemp-pBuffer);
	return *this;
}

YkBool YkUnicodeString::IsEmpty() const
{
	return m_qString->length() > 0 ? FALSE: TRUE;
}

void YkUnicodeString::Empty()
{
	SetLength(0);
}

YkInt YkUnicodeString::Compare(const YkWchar* pStr) const
{
	YkInt nLength = GetStrLength(pStr);
	if(m_qString->length() != nLength) { return 1; }
	return m_qString->compare(QString((const QChar*)pStr, nLength));
}

YkInt YkUnicodeString::CompareNoCase(const YkWchar* pStr) const
{
	YkInt nLength = GetStrLength(pStr);
	if(m_qString->length() != nLength) { return 1; }
	return m_qString->compare(QString((const QChar*)pStr, nLength), Qt::CaseInsensitive);
}

YkInt YkUnicodeString::CompareNoCase(const YkUnicodeString& str) const
{
	if(m_qString->length() != str.GetLength()) { return 1; }
	return m_qString->compare(str.m_qString, Qt::CaseInsensitive);
}

YkInt YkUnicodeString::Find(YkWchar ch,YkInt nStart/*=0*/) const
{
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkWchar *pBuffer = (YkWchar*)m_qString->data();
	YkInt nLength = m_qString->length();
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
	YkInt nOldLength = GetStrLength(pStrOld);
	YkInt nNewLength = GetStrLength(pStrNew);
	
	m_qString->replace((const QChar *)pStrOld, nOldLength, (const QChar *)pStrNew, nNewLength);
	return TRUE;
}

void YkUnicodeString::Format(const YkWchar* pStr,...)
{
	if(pStr && *pStr)
	{
		YkInt len=0;
		YkUint nCapacity=GetStrLength(pStr);
		nCapacity = nCapacity +512;
		
		SetLength(nCapacity);

		va_list args;	
		va_start(args,pStr);

		len=YK_vsnprintf((YkWchar*)m_qString->constData(),nCapacity,pStr,args);
        
		if(len > (YkInt)nCapacity)
		{
			SetLength(len);
			va_end(args);	
			va_start(args,pStr);
			len=YK_vsnprintf((YkWchar*)m_qString->constData(),len,pStr,args);
		}
		else
		{
			SetLength(len);
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
	if(pStr != NULL)
	{
		m_qString->setUnicode((const QChar*)pStr, GetStrLength(pStr));
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
	*m_qString += *str.m_qString;
	return *this;
}

const YkUnicodeString& YkUnicodeString::operator+=(const YkWchar ch)
{
	*m_qString+=(QChar)ch;
	return *this;
}

const YkUnicodeString& YkUnicodeString::operator+=(const YkWchar* pStr)
{
	this->Append(pStr, GetStrLength(pStr));
	return *this;
}

bool YkUnicodeString::operator==(const YkWchar* pStr) const
{
	return this->Compare(pStr) == 0;
}

bool YkUnicodeString::operator==(const YkUnicodeString& str) const
{
	return *m_qString == *str.m_qString;
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
#ifdef _MSC_VER
#pragma warning (disable: 4800)
#endif
	return *m_qString< *(str.m_qString);
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
	YkUnicodeString strResult;
	strResult.m_qString->setNum(lVal, base);
	return strResult;
}

YkUnicodeString YkUnicodeString::From(YkUlong lVal, YkUint base)
{
	YkUnicodeString strResult;
	strResult.m_qString->setNum(lVal, base);
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
	YkInt value;
	YKUNUSED(ToInt(value, base));
	return value;
}

YkBool YkUnicodeString::ToInt(YkInt& value, YkUint base) const
{
	bool bOK = true;
	if(m_qString->indexOf(QChar(',')) > -1)
	{
		QString tempStr(*m_qString);
		tempStr.remove(QChar(','));
		value = tempStr.toInt(&bOK, base);
	}
	else
	{
		value = m_qString->toInt(&bOK, base);
	}
	return bOK;
}

YkUint YkUnicodeString::ToUInt(YkUint base) const
{
	bool bOK = true;
	return m_qString->toUInt(&bOK, base);
}

YkBool  YkUnicodeString::ToUInt(YkUint& value, YkUint base) const
{
	bool bOK = true;
	value = m_qString->toUInt(&bOK, base);
	return bOK;
}

YkLong YkUnicodeString::ToLong(YkUint base) const
{
	bool bOK = true;
	return m_qString->toLongLong(&bOK, base);
}

YkBool YkUnicodeString::ToLong(YkLong& value, YkUint base) const
{
	bool bOK = true;
	value = m_qString->toLongLong(&bOK, base);
	return bOK;
}

YkUlong YkUnicodeString::ToULong(YkUint base) const
{
	bool bOK = true;
	return m_qString->toULongLong(&bOK, base);
}

YkBool YkUnicodeString::ToULong(YkUlong& value, YkUint base) const
{
	bool bOK = true;
	value = m_qString->toULongLong(&bOK, base);
	return bOK;
}

YkFloat YkUnicodeString::ToFloat() const
{
	return m_qString->toFloat();
}

YkBool YkUnicodeString::ToFloat(YkFloat& value) const
{
	bool bOK = true;
	value = m_qString->toFloat(&bOK);
	return bOK;
}

YkDouble YkUnicodeString::ToDouble() const
{
	return m_qString->toDouble();
}

YkBool YkUnicodeString::ToDouble(YkDouble& value) const
{
	bool bOK = true;
	value = m_qString->toDouble(&bOK);
	return bOK;
}

YkInt YkUnicodeString::Split(YkUnicodeStringArray &strDest,const YkWchar* pStrDelims, YkBool bKeepEmptyString) const
{
	strDest.SetSize(0);

	QString::SplitBehavior behavior = bKeepEmptyString ? QString::KeepEmptyParts : QString::SkipEmptyParts;
	QString strsep((const QChar*)pStrDelims);
	QStringList strList = m_qString->split(strsep, behavior, Qt::CaseSensitive);
	for(int i = 0; i < strList.size(); i++)
	{
		strDest.Add(YkUnicodeString((const YkWchar*)strList.at(i).data()));
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
 	m_qString->replace(startIndex,length,(const QChar*)pStrNew,GetStrLength(pStrNew));
}

enum DataEndianness
{
	DetectEndianness,
	BigEndianness,
	LittleEndianness
};

Yk::YkInt YkUnicodeString::WcharToAchar( const YkWchar* pWchar, YkAchar *pAchar, YkInt nAcharLength )
{
	memset(pAchar, 0, nAcharLength * sizeof(YkAchar));
	std::string dataAchar;
	YkUnicodeString(pWchar).ToStd(dataAchar, Default);
	memcpy(pAchar, dataAchar.c_str(), dataAchar.length());
	return dataAchar.length();
}

Yk::YkInt YkUnicodeString::AcharToWchar( const YkAchar* pAchar, YkWchar *pWchar, YkInt nWcharLength )
{
	memset(pWchar, 0, nWcharLength * sizeof(YkWchar));
	YkUnicodeString uString;
	std::string aString(pAchar);
	uString.FromStd(aString, Default);
	memcpy(pWchar, uString.Cstr(), uString.GetLength() * sizeof(YkWchar));
	return uString.GetLength();
}

Yk::YkInt YkUnicodeString::WcharToWchar32( const YkWchar* pWchar, YkWchar32* pWchar32, YkInt nWchar32Length )
{
	YkInt nWcharLength = u_str16len(pWchar);
	if(pWchar == NULL || pWchar32 == NULL || nWchar32Length < nWcharLength)
	{
		return 0;
	}

	memset(pWchar32, 0, nWchar32Length * sizeof(YkWchar32));
	QString strutf16((QChar*)pWchar);
	QVector<YkUint> vecutf32 = strutf16.toUcs4();
	YkInt cpysize = YKMIN(vecutf32.size(), nWchar32Length);
	memcpy(pWchar32, vecutf32.constData(), cpysize * sizeof(YkWchar32));
	return cpysize;
}

Yk::YkInt YkUnicodeString::Wchar32ToWchar( const YkWchar32* pWchar32, YkWchar* pWchar, YkInt nWcharLength )
{
	YkInt nWchar32Length = u_str32len(pWchar32);
	if(pWchar == NULL || pWchar32 == NULL || nWcharLength < nWchar32Length*2)
	{
		return 0;
	}

	memset(pWchar, 0, nWcharLength * sizeof(YkWchar));
	YkInt size = YKMIN(nWchar32Length, nWcharLength);
	QString strutf16 = QString::fromUcs4(pWchar32, size);
	memcpy(pWchar, strutf16.constData(), size * sizeof(YkWchar));
	return size;
}

YkUnicodeString& YkUnicodeString::FromUTF8( const YkAchar *pSrc, YkInt nSrcLength )
{
	QString newstring = QString::fromUtf8(pSrc, nSrcLength);
	m_qString->setUnicode(newstring.data(), newstring.length());
	return *this;
}

YkMBString& YkUnicodeString::ToUTF8( YkMBString& strResult ) const
{
	QByteArray utf8data = m_qString->toUtf8();
	strResult.SetLength(utf8data.size());
	strResult = utf8data.constData();
	return strResult;
}


std::string&  YkUnicodeString::ToStd(std::string& str,YKCharset::Charset charset/*= YkUnicodeString::UTF8*/) const
{
	YkString strTargetU = YkSystem::GetCharsetNameICU(charset);
	YkMBString strTarget;
	strTargetU.ToUTF8(strTarget);
	QTextCodec* pTextCodec = QTextCodec::codecForName(strTarget.Cstr());
	if(pTextCodec == NULL)
	{
		pTextCodec = QTextCodec::codecForLocale();
	}
	QByteArray dataTarget = pTextCodec->fromUnicode(*m_qString);
	str.resize(dataTarget.size());
	memcpy((char*)str.c_str(), dataTarget.data(), dataTarget.size());
	
	return str;
}

YkUnicodeString&  YkUnicodeString::FromMBString(const YkAchar *pSrc, YkInt nSrcLength,YKCharset::Charset charset/* = YKCharset::Default*/)
{
	YkString strSrcU = YkSystem::GetCharsetNameICU(charset);
	YkMBString strSrc;
	strSrcU.ToUTF8(strSrc);
	QTextCodec* pTextCodec = QTextCodec::codecForName(strSrc.Cstr());
	if(pTextCodec == NULL)
	{
		pTextCodec = QTextCodec::codecForLocale();
	}
	QString newstring = pTextCodec->toUnicode(pSrc);
	m_qString->setUnicode(newstring.data(), newstring.length());
	return *this;
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
	YkInt nLength(GetLength());
	YkInt bLength(str.GetLength());

	// If one string has null length, we return 0.
	if (nLength == 0 || bLength == 0)
	{
		return 0.0;
	}

	// Calculate max length range.
	YkInt maxRange(YKMAX(0, YKMAX(nLength, bLength) / 2 - 1));

	// Creates 2 vectors of integers.
	std::vector<YkBool> aMatch(nLength, false);
	std::vector<YkBool> bMatch(bLength, false);

	// Calculate matching characters.
	YkInt matchingCharacters(0);
	for (YkInt nIndex(0); nIndex < nLength; ++nIndex)
	{
		// Calculate window test limits (limit inferior to 0 and superior to bLength).
		YkInt minIndex(YKMAX(nIndex - maxRange, 0));
		YkInt maxIndex(YKMIN(nIndex + maxRange + 1, bLength));

		if (minIndex >= maxIndex)
		{
			// No more common character because we don't have characters in b to test with characters in a.
			break;
		}

		for (YkInt bIndex(minIndex); bIndex < maxIndex; ++bIndex)
		{
			if (!bMatch.at(bIndex) && this->GetAt(nIndex) == str.GetAt(bIndex))
			{
				// Found some new match.
				aMatch[nIndex] = true;
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

	for (YkInt nIndex(0), positionIndex(0); nIndex < nLength; nIndex++)
	{
		if (aMatch.at(nIndex))
		{
			aPosition[positionIndex] = nIndex;
			positionIndex++;
		}
	}

	for (YkInt nIndex(0), positionIndex(0); nIndex < bLength; nIndex++)
	{
		if (bMatch.at(nIndex))
		{
			bPosition[positionIndex] = nIndex;
			positionIndex++;
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
		JARO_WEIGHT_STRING_A * matchingCharacters / nLength +
		JARO_WEIGHT_STRING_B * matchingCharacters / bLength +
		JARO_WEIGHT_TRANSPOSITIONS * (matchingCharacters - transpositions / 2) / matchingCharacters
		);
}

YkDouble YkUnicodeString::CalculateDistance(const YkUnicodeString& str) const
{
	YkDouble distance(CalculateJaroDistance(str));

	YkInt aLength(GetLength());
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

#endif

}
