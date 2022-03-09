/*
 *
 * YkTextCodec.h
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

#ifndef AFX_YKTEXTCODEC_H__F0186497_519B_4DE7_8D70_F923C62D9E74__INCLUDED_
#define AFX_YKTEXTCODEC_H__F0186497_519B_4DE7_8D70_F923C62D9E74__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"

namespace Yk {

//! \brief 当前的编码方式，由外部用户制定
static YkString::Charset g_charset;
static YkBool  g_bUseGlobal = FALSE;
//! \brief 字符集转换器, 内部采用iconv实现
class TOOLKIT_API YkTextCodec  
{
public:
	YkTextCodec();

	~YkTextCodec();

	YkTextCodec(const YkMBString& to, const YkMBString& from);

	YkTextCodec(YkString::Charset to, YkString::Charset from);

public:
	static YkString GetCharsetName(YkString::Charset charset);

	static YkString::Charset GetCharset(const YkString &strCharsetName);
#ifdef _UGUNICODE
	static YkString::Charset GetCharset(const YkMBString &strCharsetName)
	{
		return GetCharset(YkString().FromMBString(strCharsetName.Cstr(), strCharsetName.GetLength()));
	}
#endif

	//! 外部用户用的Charset
	static YkString::Charset GetGlobalCharset();
	static YkString::Charset GetStrGlobalCharset(const YkString& strCharset);

	static void SetGlobalCharset(const YkString& strCharset,YkBool bUseGlobal = TRUE);
	static void SetGlobalCharset(YkString::Charset charset,YkBool bUseGlobal = TRUE);
	static void SetUseGlobalCharset(YkBool bUseGlobal = TRUE);
	static YkBool IsUseGlobalCharset();
	

public:
	YkBool Open(const YkMBString& to, const YkMBString& from);

	YkBool Open(YkString::Charset to, YkString::Charset from);

	YkBool IsOpen();

	void Close();	

	//! 传入 pSource 和 nSize(按字节计)，
	//! 用target传出转换后的字符串
	//! remark 本函数适用于转换后的字符串属于MBCS（多字节编码）
	YkBool Convert(YkMBString& target,const YkMBString& source);

	YkBool Convert(YkMBString& target, const YkAchar* pSource, YkInt nSize);

	YkBool Convert(YkByte *pDest, YkInt nCapacity, YkInt &nDestSize, const YkByte* pSource, YkInt nSize);

private:
	void* m_pHandle;

	//加了一个环境变量用于控制相同编码转换。
	YkBool m_bCharsetEqual;
};

}

#endif



