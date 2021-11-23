/*
 *
 * YkTextCodec.cpp
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

#include "Toolkit/YkTextCodec.h"
#include "Toolkit/YkToolkit.h"
#include "iconv.h"

namespace Yk {

YkString YkTextCodec::GetCharsetName(YkString::Charset charset)
{

	return YkSystem::GetCharsetName(charset);
}

YkString::Charset YkTextCodec::GetCharset(const YkString &strCharsetName)
{
	return YkSystem::GetCharset(strCharsetName);	
}

YkString::Charset YkTextCodec::GetGlobalCharset()
{
	return g_charset;
}

YkString::Charset YkTextCodec::GetStrGlobalCharset(const YkString& strCharset)
{
	YkString charset = strCharset;
	charset.MakeUpper();
	//两个日本的
	if(-1!=charset.Find(_U("JA"))) {
		return YkString::ShiftJIS;
	}
	else if(-1!=charset.Find(_U("JA-JP"))) {
		return YkString::ShiftJIS;
	}
	// 几个简体中文的,统统用GB18030
	else if(-1!=charset.Find(_U("ZH-CN"))) {
		return YkString::GB18030;
	}
	else if(-1!=charset.Find(_U("ZH-HANS"))) {
		return YkString::GB18030;
	}
	// 几个繁体中文的, 统一认为是big5,在真正调用iconv时,采用_U("BIG5-HKSCS:2001")
	else if(-1!=charset.Find(_U("ZH-SG"))) {
		return YkString::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("ZH-TW"))) {
		return YkString::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("ZH-HANT"))) {
		return YkString::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("ZH-MO"))) {
		return YkString::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("ZH-HK"))) {
		return YkString::ChineseBIG5;
	}
	else if (-1!=charset.Find(_U("SV-"))) {
		return YkString::Windows1252;
	}
	else if (-1 != charset.Find(_U("AR-SA"))) {
		return YkString::Arabic;
	}
	else if (-1 != charset.Find(_U("ES-ES"))) {
		return YkString::Windows1252;
	}
	else {
		return YkString::Default;
	}
}

void YkTextCodec::SetGlobalCharset( const YkString& strCharset,YkBool bUseGlobal )
{
	g_charset = GetStrGlobalCharset(strCharset);
	g_bUseGlobal = bUseGlobal;
}

void YkTextCodec::SetGlobalCharset(YkString::Charset charset,YkBool bUseGlobal)
{
	g_charset = charset;
	g_bUseGlobal = bUseGlobal;
}

YkBool YkTextCodec::IsUseGlobalCharset()
{
	return g_bUseGlobal;
}

void YkTextCodec::SetUseGlobalCharset(YkBool bUseGlobal /*= TRUE*/ )
{
	g_bUseGlobal = bUseGlobal;
}

YkTextCodec::YkTextCodec()
{
	m_pHandle=(void*)-1;
	m_bCharsetEqual = FALSE;
}

YkTextCodec::YkTextCodec(const YkMBString& to, const YkMBString& from)
{
	m_bCharsetEqual = FALSE;
	m_pHandle=(void*)-1;
	Open(to,from);
}

YkTextCodec::YkTextCodec(YkString::Charset to, YkString::Charset from)
{	
	m_bCharsetEqual = FALSE;
	m_pHandle=(void*)-1;
	Open(to, from);
}

YkTextCodec::~YkTextCodec()
{
	Close();
}

YkBool YkTextCodec::Open(const YkMBString& to, const YkMBString& from)
{
	/********************************************/
	/*在韩语操作系统下，YkMBString的默认编码是ks_c_5601-1987，*/
	/*而libiconv库不支持这种编码的转换，改成韩语其他支持的编码*/

	YkMBString fromTemp = from;
	if (from.CompareNoCase("ks_c_5601-1987") == 0)
	{
		fromTemp = "EUC-KR";
	}

	Close();
	m_bCharsetEqual = to == from? TRUE:FALSE;
	m_pHandle=(void*)::iconv_open(to.Cstr(),fromTemp.Cstr());

	YkBool bResult = m_pHandle != NULL && (YkLong)m_pHandle != -1;
	return bResult;
}

YkBool YkTextCodec::Open(YkString::Charset to,YkString::Charset from)
{	
	if (from == YkString::Unicode)
	{
		if(YK_ISBIGENDIAN)
			from = YkString::UCS2BE;
		else
			from = YkString::UCS2LE;
	}
	if(to == YkString::Unicode)
	{
		if(YK_ISBIGENDIAN)
			to = YkString::UCS2BE;
		else
			to = YkString::UCS2LE;
	}
#ifdef _UGUNICODE
	YkMBString strMBto, strMBfrom;
	return Open(GetCharsetName(to).ToMBString(strMBto), GetCharsetName(from).ToMBString(strMBfrom));
#else
	return Open(GetCharsetName(to), GetCharsetName(from));
#endif
}

YkBool YkTextCodec::Convert(YkMBString& target, const YkMBString& source)
{
	if(!IsOpen()) return FALSE;
	
	if(m_bCharsetEqual)
	{
		target = source;
		return TRUE;
	}

	size_t s_len = source.GetLength();	
	const char* s_ptr = source.Cstr();
	size_t max_o_len = s_len * 15;
	YkMBString strBuffer;
	strBuffer.SetLength((YkInt)max_o_len);

	size_t o_len = max_o_len;
	YkAchar* buf = (YkAchar*)strBuffer.Cstr();
	YkAchar* o_ptr = buf;
	const char* tempChar = (const char*)s_ptr;
#if defined (WIN32)
	::iconv(m_pHandle, (const char**)&tempChar, &s_len, &o_ptr, &o_len);
#else
	::iconv(m_pHandle, (char**)&tempChar, &s_len, &o_ptr, &o_len);
#endif
	if (s_len != 0) 
	{
		return FALSE;
	}
	if(o_len > max_o_len)
	{
		return FALSE;
	}

	target.SetLength(0);
	target.Append(buf, (YkInt)(max_o_len - o_len));
	return TRUE;
}
 
YkBool YkTextCodec::Convert(YkMBString & target, const YkAchar* pSource, YkInt nSize)
{
	if(!IsOpen()) return FALSE;
	if(m_bCharsetEqual)
	{
		target = pSource;
		return TRUE;
	}

	size_t s_len = nSize;
	const char* s_ptr = pSource;
	size_t max_o_len = s_len * 5;
	YkMBString strBuffer;
	strBuffer.SetLength((YkInt)max_o_len);

	size_t o_len = max_o_len;
	YkAchar* buf = (YkAchar*)strBuffer.Cstr();
	YkAchar* o_ptr = buf;
	const char* tempChar = (const char*)s_ptr;
#if defined (WIN32)
	::iconv(m_pHandle, (const char**)&tempChar, &s_len, &o_ptr, &o_len);	
#else
	::iconv(m_pHandle, (char**)&tempChar, &s_len, &o_ptr, &o_len);	
#endif
	if (s_len != 0) 
	{
		return FALSE;
	}

	if(o_len > max_o_len)
	{
		return FALSE;
	}

	target.SetLength(0);
	target.Append(buf, (YkInt)(max_o_len - o_len));
	return TRUE;
}

Yk::YkBool YkTextCodec::Convert( YkByte *pDest, YkInt nCapacity, YkInt &nDestSize, const YkByte* pSource, YkInt nSize )
{
	if(!IsOpen()) return FALSE;
	if(m_bCharsetEqual)
	{
		if(nCapacity > nSize)
		{
			memcpy(pDest, pSource, nSize);
			nDestSize = nSize;
		}
		return TRUE;
	}

	size_t s_len = nSize;
	const char* s_ptr = (YkAchar*)pSource;
	size_t max_o_len = s_len * 5;

	size_t o_len = max_o_len;
	YkAchar* buf = (YkAchar *)pDest;
	YkAchar* o_ptr = buf;
	const char* tempChar = (const char*)s_ptr;
#if defined (WIN32)
	::iconv(m_pHandle, (const char**)&tempChar, &s_len, &o_ptr, &o_len);	
#else
	::iconv(m_pHandle, (char**)&tempChar, &s_len, &o_ptr, &o_len);	
#endif
	if (s_len != 0) 
	{
		return FALSE;
	}

	if(o_len > max_o_len)
	{
		return FALSE;
	}

	nDestSize = (YkInt)(max_o_len - o_len);
	return TRUE;
}
YkBool YkTextCodec::IsOpen()
{
	return (m_pHandle != (void*)-1);
}

void YkTextCodec::Close()
{
	if(IsOpen())
	{
		::iconv_close((iconv_t)m_pHandle);
		m_pHandle = (void*)-1;
	}
	m_bCharsetEqual = FALSE;
}

}
