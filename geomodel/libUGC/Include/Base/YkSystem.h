/*
 *
 * YkSystem.h
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

#if !defined(AFX_YKSYSTEM_H__980E850B_7989_4E01_A079_7B18C9B132D5__INCLUDED_)
#define AFX_YKSYSTEM_H__980E850B_7989_4E01_A079_7B18C9B132D5__INCLUDED_

#pragma once

#include "Base/YkString.h"

namespace Yk{
//!  \brief 系统工具类
class BASE_API YkSystem  
{
private:
	YkSystem();
    ~YkSystem();

public:
	//! \brief 通过动态库名称，加载动态库
	//! \param strLibName 动态库名称
	static void* LoadDll(const YkString& strLibName);

	//! \brief 通过库的句柄和函数名字,得到函数指针
	//! \param hModule 动态库句柄
	//! \param strProName 函数名称
	static void* GetProcAddress(void* hModule, const YkString& strProName);

	//! \brief 释放动态库
	//! \param hModule 动态库句柄
	static YkBool FreeDll(void* hModule);

	//! \brief 判断字符是否属于空ASCII字符, 包括：空格,\\t\\r\\n等
	static YkBool IsSpaceChar(YkAchar ch);

	//! \brief 判断字符是否属于空ASCII字符, 包括：空格,\\t\\r\\n等
	static YkBool IsSpaceChar(YkWchar wch);

	//! \brief 判断字符是否属于数字字符, 从 '0' 到 '9'
	static YkBool IsDigit(YkAchar ch);

	//! \brief 判断字符是否属于十六进制字符, 包括从 '0' 到 '9', 'A'到'F', 'a'到'f'
	static YkBool IsHexDigit(YkAchar ch);

	//! \brief 判断字符是否属于大写字符, 从 'A' 到 'Z'
	static YkBool IsBigLetter(YkAchar ch);

	//! \brief 判断字符是否属于小写字符, 从 'a' 到 'z'
	static YkBool IsLittleLetter(YkAchar ch);
	
	//! \brief 判断该字节是否是lead byte，即该字节和后续字节联合在一起构成一个双字节的字符（如中文、日文等）
	static YkBool IsLeadByte(YkAchar ch);
	
	//! \brief 判断一个UCS2的字符是否是英文字符
	//! \return 如果是中文或日文等字符，返回false；是英文字符，返回true
	static YkBool IsAscii(YkUshort ch);

#ifdef _UGUNICODE
	//! \brief 判断字符是否属于数字字符, 从 '0' 到 '9'
	static YkBool IsDigit(YkWchar32 ch);

	//! \brief 判断字符是否属于十六进制字符, 包括从 '0' 到 '9', 'A'到'F', 'a'到'f'
	static YkBool IsHexDigit(YkWchar32 ch);

	//! \brief 判断字符是否属于大写字符, 从 'A' 到 'Z'
	static YkBool IsBigLetter(YkWchar32 ch);

	//! \brief 判断字符是否属于小写字符, 从 'a' 到 'z'
	static YkBool IsLittleLetter(YkWchar32 ch);
#endif

	//! \brief 获取程序路径
	static YkString GetAppPath();
	//! \brief 获取模块路径
	static YkString GetModulePath(const YkString& strModultTitle);

	//! \brief 获取模块路径（带后缀名）
	static YkString GetModulePathWithExt(const YkString& strModultTitle);

	//! \brief 获取目录
	static YkString GetDir(const YkString& strFile);
	//! \brief 获取文件后缀名
	static YkString GetExtName(const YkString& strFile);
	//! \brief 获取文件后缀名
	static YkBool FindFileExtPaths(const YkString& strPath, const YkString& strExtName, 
								  YkArray<YkString>& strFileNames, YkBool bAddPath);

	//! \brief 判断当前dll加载方式是否需要修改系统路径，默认不需要修改，web三维应用时需要特殊处理
	static YkBool ms_bNeedResetCurrentDirectory;
	
	//!  \brief 获取对应字符的String 
	static YkString GetCharsetName(YKCharset::Charset charset);
	//!  \brief 获取String的对应的字符串
	static YKCharset::Charset GetCharset(const YkString &strCharsetName);
	//! \brief 得到字符集的名称
	//! param charset[in] 字符集
	//! return ICU中使用的字符集的名称
	//! remark 这个方法是在YkUnicodeString进行编码转换的时候使用，仅限YkUnicodeString的FromMBString和ToMBString中调用！
	//! IA5编码部分采用windows-1252进行处理，参见ICU官网,Converter Explorer
	static YkString GetCharsetNameICU(YKCharset::Charset charset);
	static YKCharset::Charset GetCurCharset();

#ifdef _UGUNICODE
	static YkInt GetModulePath(const YkChar* pModuleTitle, YkChar* pModulePath, YkInt nLength);
#endif
};

}

#endif

