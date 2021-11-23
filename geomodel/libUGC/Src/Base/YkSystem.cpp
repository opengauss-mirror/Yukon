/*
 *
 * YkSystem.cpp
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

#include "Base/YkSystem.h"
#include "Base/YkPlatform.h"
#ifdef _UGUNICODE
#ifdef _UGQSTRING
#include "QtCore/qchar.h"
#else
#include "unicode/uchar.h"
#endif
#endif


#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <dlfcn.h> // for dlopen() & dlsym()
#endif

namespace Yk{

YkBool YkSystem::ms_bNeedResetCurrentDirectory = FALSE;

YkBool YkSystem::IsSpaceChar(YkAchar ch)
{
	switch(ch) {
	case ' ':
	case '\t':
	case '\r':
	case '\n':
			return true;
	default:
		return false;
	}
}

YkBool YkSystem::IsSpaceChar(YkWchar wch)
{
	switch(wch) {
	case L' ':
	case L'\t':
	case L'\r':
	case L'\n':
		return true;
	default:
		return false;
	}
}

//! \brief 判断字符是否属于 数字字符, 从 '0' 到 '9'
YkBool YkSystem::IsDigit(YkAchar ch)
{
	return (ch>='0' && ch<='9');
}
#ifdef _UGUNICODE
Yk::YkBool YkSystem::IsDigit( YkWchar32 ch )
{
#ifdef _UGQSTRING
	return QChar(ch).isDigit();
#else
	return u_isdigit((UChar32)ch);
#endif
}
#endif
//! \brief 判断字符是否属于 十六进制字符, 包括从 '0' 到 '9', 'A'到'F', 'a'到'f'
YkBool YkSystem::IsHexDigit(YkAchar ch)
{
	return IsDigit(ch) || (ch>='A'&&ch<='F') || (ch>='a'&&ch<='f');
}

#ifdef _UGUNICODE
Yk::YkBool YkSystem::IsHexDigit( YkWchar32 ch )
{
#ifdef _UGQSTRING
	return IsDigit(ch) || (ch>=L'A'&&ch<=L'F') || (ch>=L'a'&&ch<=L'f');
#else
	return u_isxdigit((UChar32)ch);
#endif
}
#endif
//! \brief 判断字符是否属于 大写字符, 从 'A' 到 'Z'
YkBool YkSystem::IsBigLetter(YkAchar ch)
{
	return (ch>='A' && ch<='Z');
}

#ifdef _UGUNICODE
Yk::YkBool YkSystem::IsBigLetter( YkWchar32 ch )
{
#ifdef _UGQSTRING
	return QChar(ch).isUpper();
#else
	return u_isupper((UChar32)ch);
#endif
}
#endif
//! \brief 判断字符是否属于 小写字符, 从 'a' 到 'z'
YkBool YkSystem::IsLittleLetter(YkAchar ch)
{
	return (ch>='a' && ch<='z');
}

#ifdef _UGUNICODE
Yk::YkBool YkSystem::IsLittleLetter( YkWchar32 ch )
{
#ifdef _UGQSTRING
	return QChar(ch).isLower();
#else
	return u_islower((UChar32)ch);
#endif
}
#endif
//! \brief 判断该字节是否是lead byte，即该字节和后续字节联合在一起构成一个双字节的字符（如中文、日文等）
YkBool YkSystem::IsLeadByte(YkAchar ch)
{
	return ch & 0x80;
}

//! \brief 判断一个UCS2的字符是否是英文字符
//! \return 如果是中文或日文等字符，返回false；是英文字符，返回true
YkBool YkSystem::IsAscii(YkUshort ch)
{
	return ch < 0xff;
}

YkString YkSystem::GetDir(const YkString& strFile)
{
	YkInt n=0, i=0;	
	if(!strFile.IsEmpty())
	{
		i=0;
#ifdef WIN32
		if(isalpha((YkUchar)strFile[0]) && strFile[1]==':') 
		{
			i=2;
			n=i;
		}
#endif
		
		YkInt nLen = strFile.GetLength();
		while(i < nLen)
		{
			if(ISPATHSEP(strFile[i])) 
			{
				n=i;
			}
			i++;
		}
		if (n != 0 || ISPATHSEP(strFile[0]))
		{
			// 剔除没有路径信息的情况
			return YkString(strFile.Cstr(),n+1);
		}
    }
	return _U("");
}

YkString YkSystem::GetExtName(const YkString& strFile)
{
	YkInt f=0,e=0,i=0,n=0;
	if(!strFile.IsEmpty())
	{
		n=0;
#ifdef WIN32
		if(isalpha((YkUchar)strFile[0]) && strFile[1]==':')
		{
			n=2;
		}
#endif
		f=n;
		YkInt nLen = strFile.GetLength();
		while(n < nLen)
		{
			if(ISPATHSEP(strFile[n]))
			{
				f=n+1;
			}
			n++;
		}

		if (f >= nLen)
		{
			return _U("");
		}

		if(strFile[f]==_U('.'))
		{
			f++;     // Leading '.'
		}
		e=i=n;
		while(f<i)
		{
			if(strFile[--i]==_U('.'))
			{ 
				e=i; 
				break; 
			}
		}
		return YkString(strFile.Cstr()+e,n-e);
    }
	return _U("");
}

YkString  YkSystem::GetCharsetName(YKCharset::Charset charset)
{
	switch (charset)
	{	
	case YKCharset::MAC:			
		return _U("MACINTOSH");
	case YKCharset::ShiftJIS:
		return _U("CP932");
	case YKCharset::Hangeul:
		return _U("CP949");
	case YKCharset::Johab:
		return _U("JOHAB");
	case YKCharset::GB18030:
		return _U("GB18030");
	case YKCharset::ChineseBIG5:
		return _U("BIG5-HKSCS");
	case YKCharset::Greek:
		return _U("Greek");
	case YKCharset::Turkish:
		return _U("CP1254");
	case YKCharset::Vietnamese:			
		return _U("CP1258");
	case YKCharset::Hebrew:
		return _U("Hebrew");
	case YKCharset::Arabic:
		return _U("CP1256");
	case YKCharset::Baltic:
		return _U("CP1257");
	case YKCharset::Russian:
		return _U("CP1251");
	case YKCharset::Thai:
		return _U("CP874");
	case YKCharset::EastEurope:
		return _U("CP1250");
	case YKCharset::OEM:
		return _U("MACINTOSH");
	case YKCharset::UTF8:
		return _U("UTF-8");
	case YKCharset::UCS2LE:
		return _U("UTF-16LE");
	case YKCharset::UCS2BE:
		return _U("UTF-16BE");
	case YKCharset::UCS4LE:
		return _U("UTF-32LE");
	case YKCharset::UCS4BE:
		return _U("UTF-32BE");
	case YKCharset::Windows1252:
		return _U("windows-1252");
	case YKCharset::xIA5:
		return _U("x-IA5");
	case YKCharset::xIA5German:
		return _U("x-IA5-German");
	case YKCharset::xIA5Swedish:
		return _U("x-IA5-Swedish");
	case YKCharset::xIA5Norwegian:
		return _U("x-IA5-Norwegian");
	case YKCharset::UTF7:
		return _U("utf-7");
	case YKCharset::Korean:
		return _U("ks_c_5601-1987");
	case YKCharset::Unicode:
		return _U("utf-16");
	case YKCharset::Cyrillic:
		return _U("windows-1251");
	case YKCharset::Symbol:
		return _U("symbol");
	case YKCharset::ISO2022JP2:
		return _U("ISO-2022-JP-2");
	default:
		YKCharset::Charset nCharset = GetCurCharset();
		if (nCharset != YKCharset::Default)
		{
			return GetCharsetName(nCharset);
		}
		return _U("GB18030");
	}
}

Yk::YkString YkSystem::GetCharsetNameICU( YKCharset::Charset charset )
{
	switch (charset)
	{	
	case YKCharset::MAC:			
		return _U("MACINTOSH");
	case YKCharset::ShiftJIS:
		return _U("CP932");
	case YKCharset::Hangeul:
		return _U("CP949");
	case YKCharset::Johab:
		return _U("JOHAB");
	case YKCharset::GB18030:
		return _U("GB18030");
	case YKCharset::ChineseBIG5:
		return _U("BIG5-HKSCS");
	case YKCharset::Greek:
		return _U("Greek");
	case YKCharset::Turkish:
		return _U("CP1254");
	case YKCharset::Vietnamese:			
		return _U("CP1258");
	case YKCharset::Hebrew:
		return _U("Hebrew");
	case YKCharset::Arabic:
		return _U("CP1256");
	case YKCharset::Baltic:
		return _U("CP1257");
	case YKCharset::Russian:
		return _U("CP1251");
	case YKCharset::Thai:
		return _U("CP874");
	case YKCharset::EastEurope:
		return _U("CP1250");
	case YKCharset::OEM:
		return _U("MACINTOSH");
	case YKCharset::UTF8:
		return _U("UTF-8");
	case YKCharset::UCS2LE:
		return _U("UTF-16LE");
	case YKCharset::UCS2BE:
		return _U("UTF-16BE");
	case YKCharset::UCS4LE:
		return _U("UTF-32LE");
	case YKCharset::UCS4BE:
		return _U("UTF-32BE");
	case YKCharset::Windows1252:
	case YKCharset::xIA5:
	case YKCharset::xIA5German:
	case YKCharset::xIA5Swedish:
	case YKCharset::xIA5Norwegian:
		return _U("windows-1252");
	case YKCharset::UTF7:
		return _U("utf-7");
	case YKCharset::Korean:
		return _U("ks_c_5601-1987");
	case YKCharset::Unicode:
		return _U("utf-16");
	case YKCharset::Cyrillic:
		return _U("windows-1251");
	case YKCharset::Symbol:
		return _U("symbol");
	case YKCharset::ISO2022JP2:
		return _U("ISO-2022-JP-2");
	default:
		YKCharset::Charset nCharset = GetCurCharset();
		if (nCharset != YKCharset::Default)
		{
			return GetCharsetName(nCharset);
		}
		return _U("GB18030");
	}
}

YKCharset::Charset YkSystem::GetCharset(const YkString &strCharsetName)
{
	YkString charset = strCharsetName;
	charset.MakeUpper();
	if(-1!=charset.Find(_U("MACINTOSH"))) {
		return YKCharset::MAC;
	}
	else if(-1!=charset.Find(_U("SHIFT_JIS"))) {
		return YKCharset::ShiftJIS;
	}
	else if(-1!=charset.Find(_U("CP949"))) {
		return YKCharset::Hangeul;
	}
	else if(-1!=charset.Find(_U("JOHAB"))) {
		return YKCharset::Johab;
	}
	else if(-1!=charset.Find(_U("GB18030"))) {
		return YKCharset::GB18030;
	}
	else if(-1!=charset.Find(_U("GB2312"))) {
		return YKCharset::GB18030;
	}
	else if(-1!=charset.Find(_U("GBK"))) {
		return YKCharset::GB18030;
	}
	else if(charset==_U("ZH")) {
		return YKCharset::GB18030;
	}
	else if(-1!=charset.Find(_U("BIG5"))) {
		return YKCharset::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("BIG5-HKSCS"))) {
		return YKCharset::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("BIG5-HKSCS:2001"))) {
		return YKCharset::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("BIG5-HKSCS:1999"))) {
		return YKCharset::ChineseBIG5;
	}
	else if(-1!=charset.Find(_U("GREEK"))) {
		return YKCharset::Greek;
	}
	else if(-1!=charset.Find(_U("CP1254"))) {
		return YKCharset::Turkish;
	}
	else if(-1!=charset.Find(_U("CP1258"))) {
		return YKCharset::Vietnamese;
	}
	else if(-1!=charset.Find(_U("HEBREW"))) {
		return YKCharset::Hebrew;
	}
	else if(-1!=charset.Find(_U("CP1256"))) {
		return YKCharset::Arabic;
	}
	else if(-1!=charset.Find(_U("CP1257"))) {
		return YKCharset::Baltic;
	}
	else if(-1!=charset.Find(_U("CP1251"))) {
		return YKCharset::Russian;
	}
	else if(-1!=charset.Find(_U("CP874"))) {
		return YKCharset::Thai;
	}
	else if(-1!=charset.Find(_U("CP1250"))) {
		return YKCharset::EastEurope;
	}
	else if(-1!=charset.Find(_U("UTF-8"))) {
		return YKCharset::UTF8;
	}
	else if(-1!=charset.Find(_U("UCS-2LE"))) {
		return YKCharset::UCS2LE;
	}
	else if(-1!=charset.Find(_U("UCS-2BE"))) {
		return YKCharset::UCS2BE;
	}
	else if(-1!=charset.Find(_U("UCS-4LE"))) {
		return YKCharset::UCS4LE;
	}
	else if(-1!=charset.Find(_U("UCS-4BE"))) {
		return YKCharset::UCS4BE;
	}
	else if(-1!=charset.Find(_U("UTF-32BE"))) {
		return YKCharset::UCS4BE;
	}
	else if(-1!=charset.Find(_U("UTF-32LE"))) {
		return YKCharset::UCS4LE;
	}
	else if(-1!=charset.Find(_U("UTF-7"))) {
		return YKCharset::UTF7;
	}
	else if(-1!=charset.Find(_U("X-IA5"))) {
		return YKCharset::xIA5;
	}
	else if(-1!=charset.Find(_U("X-IA5-GERMAN"))) {
		return YKCharset::xIA5German;
	}
	else if(-1!=charset.Find(_U("X-IA5-SWEDISH"))) {
		return YKCharset::xIA5Swedish;
	}
	else if(-1!=charset.Find(_U("X-IA5-NORWEGIAN"))) {
		return YKCharset::xIA5Norwegian;
	}
	else if(-1!=charset.Find(_U("KS_C_5601-1987"))) {
		return YKCharset::Korean;
	}
	else if(-1!=charset.Find(_U("UTF-16LE"))) {
		return YKCharset::UCS2LE;
	}
	else if(-1!=charset.Find(_U("UTF-16BE"))) {
		return YKCharset::UCS2BE;
	}
	else if(-1!=charset.Find(_U("UTF-16"))) {
		return YKCharset::Unicode;
	}
	else if(-1!=charset.Find(_U("WINDOWS-1251"))) {
		return YKCharset::Cyrillic;
	}
	else if(-1!=charset.Find(_U("WINDOWS-1252"))) {
		return YKCharset::Windows1252;
	}
	else if(-1!=charset.Find(_U("MACINTOSH"))) {
		return YKCharset::OEM;
	}
	else if(-1!=charset.Find(_U("ANSI"))) {
		return YKCharset::Default;
	}
	else if(-1!=charset.Find(_U("SYMBOL"))) {
		return YKCharset::Symbol;
	}
	else if(-1!=charset.Find(_U("ISO-2022-JP-2"))) {
		return YKCharset::ISO2022JP2;
	}
	else {
		return YKCharset::Default;
	}
}

YKCharset::Charset  YkSystem::GetCurCharset()
{
#ifdef WIN32
	YKCharset::Charset nCharset = YKCharset::Default;
	YkInt nCodePage = GetACP();
	switch(nCodePage)
	{
	case 874:
		nCharset = YKCharset::Thai ;
		break;
	case 932:
		nCharset = YKCharset::ShiftJIS;
		break;
	case 936:
	case 54936:
		nCharset = YKCharset::GB18030;
		break;
	case 949:
		nCharset = YKCharset::Korean;
		break;
	case 950:
		nCharset = YKCharset::ChineseBIG5;
		break;
	case 1200:
		nCharset = YKCharset::Unicode;
		break;
	case 1201:
		nCharset = YKCharset::UCS2BE;
		break;
	case 1250:
		nCharset = YKCharset::EastEurope;
		break;
	case 1251:
		nCharset = YKCharset::Cyrillic;
		break;
	case 1252:
		nCharset = YKCharset::Windows1252;
		break;
	case 1253:
		nCharset = YKCharset::Greek;
		break;
	case 1254:
		nCharset = YKCharset::Turkish;
		break;
	case 1255:
		nCharset = YKCharset::Hebrew;
		break;
	case 1256:
		nCharset = YKCharset::Arabic;
		break;
	case 1257:
		nCharset = YKCharset::Baltic;
		break;
	case 1361:
		nCharset = YKCharset::Johab;
		break;
	case 20105:
		nCharset = YKCharset::xIA5;
		break;
	case 20106:
		nCharset = YKCharset::xIA5German;
		break;
	case 20107:
		nCharset = YKCharset::xIA5Swedish;
		break;
	case 20108:
		nCharset = YKCharset::xIA5Norwegian;
		break;
	case 65001:
		nCharset = YKCharset::UTF8;
		break;
	case 65000:
		nCharset = YKCharset::UTF7;
		break;
	case 65005:
		nCharset = YKCharset::UCS4LE;
		break;
	case 65006:
		nCharset = YKCharset::UCS4BE;
		break;
	}
	return nCharset;

#else
	YKCharset::Charset nCharset = YKCharset::Default;

	const YkAchar *p = getenv("SUPERMAP_CHARSET");
	if(!p)
	{
		p = getenv("LANG");
	}
	if(!p)
	{
		p = getenv("LC_ALL");
	}
	if(!p)
	{
		p = "zh_CN.GB18030";
	}
#ifdef _UGUNICODE
	YkUnicodeString strCharset;
	strCharset.FromUTF8(p,strlen(p));
	nCharset = GetCharset(strCharset);
#else
	YkString strCharset = p;
	nCharset = GetCharset(strCharset);
#endif
	return nCharset;
#endif

}

#ifdef _UGUNICODE
Yk::YkInt YkSystem::GetModulePath( const YkChar* pModuleTitle, YkChar* pModulePath, YkInt nLength )
{
	if(pModuleTitle == NULL || pModulePath == NULL || nLength <= 0)
	{
		return -1;
	}
	pModulePath[0] = _U('\0');
#ifdef WIN32
	YkChar pModuleFullName[_MAX_PATH];
	YKstrcpy(pModuleFullName, pModuleTitle);
	YKstrcat(pModuleFullName, YK_DLL_VERSION);
#ifdef _DEBUG
	YKstrcat(pModuleFullName, _U("d"));
#endif
	YKstrcat(pModuleFullName, _U(".dll"));
	GetModuleFileName(GetModuleHandle(pModuleFullName), pModulePath, nLength);
#else
	YkAchar pModuleFullName[_MAX_PATH];

	YkAchar *path=NULL,*p=NULL;
	YkInt i = 0;

	path = getenv("LD_LIBRARY_PATH");
	p = path;
	YkBool bFind = FALSE;
	while(*p != 0)
	{
		if(*p == ':')
		{
			memcpy(pModuleFullName, p-i, i);
		}
		if(pModuleFullName[i-1] != '/')
		{
			pModuleFullName[i] = '/';
			pModuleFullName[i+1] = '\0';
		}
		else
		{
			pModuleFullName[i] = '\0';
		}
#if defined(DEBUG)
		strcat(pModuleFullName, "libBased.so");
#else
		strcat(pModuleFullName, "libBase.so");
#endif
		if(access(pModuleFullName, F_OK) == 0)
		{
			bFind = TRUE;
			break;
		}
		i++;
	}
#ifdef _UGUNICODE
	if(bFind)
	{
		YkUnicodeString::AcharToWchar(pModuleFullName, pModulePath, nLength);
	}
#endif

#endif
	for(YkInt j = YKstrlen(pModulePath) - 1; j>=0; j--)
	{
		if(ISPATHSEP(pModulePath[j]))
		{
			pModulePath[j+1] = _U('\0');
			break;
		}
	}
	return YKstrlen(pModulePath);	
}
#endif

#ifdef WIN32
void* YkSystem::LoadDll(const YkString& strLibName)
{
	if (ms_bNeedResetCurrentDirectory)
	{
		YkChar chOriginallyPath[_MAX_PATH];
		::GetCurrentDirectory(_MAX_PATH, chOriginallyPath);

		YkString strLibDir = GetDir(strLibName);
		// 	load到dll之前，设置进程的工作目录路径为dll所在路径
		::SetCurrentDirectory(strLibDir);

		//不要弹出错误对话框，比如 Load Oracle的SDX时，如果没有找到oci.dll就不要弹出错误对话框了
		::SetErrorMode(SEM_FAILCRITICALERRORS);

		void* hModule = NULL;
		hModule = ::LoadLibrary(strLibName.Cstr());	

		// load dll之后再把进程原来的工作目录路径设置回去
		::SetCurrentDirectory(chOriginallyPath);
		return hModule;
	}
	else
	{
		//不要弹出错误对话框，比如 Load Oracle的SDX时，如果没有找到oci.dll就不要弹出错误对话框了
		::SetErrorMode(SEM_FAILCRITICALERRORS);

		void* hModule = NULL;
		hModule = ::LoadLibrary(strLibName.Cstr());	

		return hModule;
	}
}

void* YkSystem::GetProcAddress(void* hModule, const YkString& strProName)
{	
	void* hFunction = NULL;
#ifdef _UGUNICODE
	YkMBString strMB;
	hFunction = ::GetProcAddress((HMODULE)hModule, strProName.ToMBString(strMB).Cstr());
#else
	hFunction = ::GetProcAddress((HMODULE)hModule, strProName.Cstr());	
#endif
	return hFunction;
}

YkBool YkSystem::FreeDll(void* hModule)
{
	YkBool bResult = false;
	
	bResult = (::FreeLibrary((HMODULE)hModule)==0? false:true);	
	return bResult;
}

// 得到主程序所在的路径(不等于当前路径)
YkString YkSystem::GetAppPath()
{
	YkChar chPath[_MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), chPath, _MAX_PATH);
	return YkSystem::GetDir(chPath);
}

// 得到模块所在的路径(如果传入参数为NULL，就是主程序所在的路径)
YkString YkSystem::GetModulePath(const YkString& strModultTitle)
{
	if (strModultTitle.IsEmpty()) 
	{
		return GetAppPath();
	}

	YkString str = strModultTitle;
	
	str +=  YK_DLL_VERSION ;
#ifdef _DEBUG
	str += _U("d");
#endif
	str += _U(".dll");

	YkChar chPath[_MAX_PATH];
	GetModuleFileName(GetModuleHandle(str.Cstr()), chPath, _MAX_PATH);
	return YkSystem::GetDir(chPath);
}

YkString YkSystem::GetModulePathWithExt(const YkString& strModultTitle)
{
	if (strModultTitle.IsEmpty()) 
	{
		return GetAppPath();
	}

	YkChar chPath[_MAX_PATH];
	GetModuleFileName(GetModuleHandle(strModultTitle.Cstr()), chPath, _MAX_PATH);
	return YkSystem::GetDir(chPath);
}

YkBool YkSystem::FindFileExtPaths(const YkString& strPath, const YkString& strExtName, 
								  YkArray<YkString>& strFileNames, YkBool bAddPath)
{
	strFileNames.RemoveAll();

	YkString strPathTmp = strPath;
	if(!strPathTmp.Right(1).CompareNoCase(_U("\\")))
	{	
		strPathTmp = strPathTmp.Left(strPathTmp.GetLength()-1);
	}
	if(strPathTmp.Right(1).CompareNoCase(_U("/"))) 
	{
		strPathTmp += _U("/");
	}
	
	YkString strExtNameTmp = strExtName;
	
	WIN32_FIND_DATA FindFileData;
	YkString strFindFile = strPathTmp + _U("*") + strExtNameTmp;
	HANDLE hFind = FindFirstFile(strFindFile, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{ 
		YkBool bFindNext = false;
		do 
		{
			YkString strFileName = FindFileData.cFileName;
			if (bAddPath)
			{
				strFileName = strPath + strFileName;
			}
			strFileNames.Add(strFileName);
			bFindNext = (FindNextFile(hFind, &FindFileData)==0? false:true);
		} while(bFindNext);
	}

	FindClose(hFind);

	return strFileNames.GetSize()>0;
}

#else

void* YkSystem::LoadDll(const YkString& strLibNameIn)
{
#ifdef _UGUNICODE
	YkMBString strLibName;
	strLibNameIn.ToMBString(strLibName);
#else
	const YkString& strLibName = strLibNameIn;
#endif
	void* hModule = NULL;
	YkInt nFlag = RTLD_LAZY;
	hModule = dlopen(strLibName.Cstr(), nFlag);
	return hModule;
}

void* YkSystem::GetProcAddress(void* hModule, const YkString& strProNameIn)
{
#ifdef _UGUNICODE
	YkMBString strProName;
	strProNameIn.ToMBString(strProName);
#else
	const YkString& strProName = strProNameIn;
#endif
	void* hFunction = NULL;
	hFunction = dlsym(hModule, strProName.Cstr());
	return hFunction;
}

YkBool YkSystem::FreeDll(void* hModule)
{
	YkBool bResult = false;
	bResult = dlclose(hModule);
	return bResult;
}

// 得到主程序所在的路径(不等于当前路径)
YkString YkSystem::GetAppPath()
{
#ifdef _UGUNICODE
	YkString strAppPath;
	const YkAchar *pValue = SELFPATH;
	strAppPath.FromMBString(pValue, strlen(pValue)); 
#else
	YkString strAppPath = SELFPATH;
#endif
	YkInt nPos = strAppPath.ReverseFind(_U('/'));
	if (nPos != -1) {
		strAppPath = strAppPath.Left(nPos+1);
	}
	return strAppPath;
}

YkString YkSystem::GetModulePath(const YkString& strModultTitle)
{
	if (strModultTitle.IsEmpty()) 
	{
		return GetAppPath();
	}

	YkString str = strModultTitle;
	YkMBString strAppPath = SELFPATH; 
	if(strAppPath.IsEmpty() || strAppPath == "")
	{
		YkAchar *path=NULL,*p=NULL;
		YkInt i = 0;
		
		i = 0;
		path = getenv("LD_LIBRARY_PATH");
		
		YkMBString strPath(path);
		YkArray<YkMBString> strArrayPath;
		strPath.Split(strArrayPath,":");
		YkInt nCount = strArrayPath.GetSize();
		YkMBString strBuffer;
		for(i=0;i<nCount;i++)
		{
			strBuffer=strArrayPath.GetAt(i);
			strBuffer.TrimRight('/');
			strBuffer += "/";
#if defined(DEBUG)
			strBuffer += "libSuBased.so";
#else	
			strBuffer += "libSuBase.so";
#endif
			if (access(strBuffer.Cstr(),F_OK) == 0)
			{
				break;
			}
		}
		if (i>=nCount)
		{
			return _U("");
		}
#ifdef _UGUNICODE
		YkUnicodeString strUBuffer;
		strUBuffer.FromMBString(strBuffer.Cstr(), strBuffer.GetLength());
		return YkSystem::GetDir(strUBuffer);
#else
		return YkSystem::GetDir(strBuffer);
#endif
	}
	int nPos = strAppPath.ReverseFind('/');
	if (nPos != -1) 
	{
		strAppPath = strAppPath.Left(nPos+1);
	}
#ifdef _UGUNICODE
	YkUnicodeString strUAppPath;
	strUAppPath.FromMBString(strAppPath.Cstr(), strAppPath.GetLength());
	return strUAppPath;
#else
	return strAppPath;
#endif
}

YkString YkSystem::GetModulePathWithExt(const YkString& strModultTitle)
{
	return GetAppPath();
}

YkBool YkSystem::FindFileExtPaths(const YkString& strPath, const YkString& strExtName, 
								  YkArray<YkString>& strFileNames, YkBool bAddPath)
{
	strFileNames.RemoveAll();

	YkString strPathTmp = strPath;
	if(strPathTmp.Right(1) == _U("\\") )
	{	
		strPathTmp = strPathTmp.Left(strPathTmp.GetLength()-1);
	}
	if(strPathTmp.Right(1) != _U("/")) 
	{
		strPathTmp += _U("/");
	}
	
	YkString strExtNameTmp = strExtName;	
	strExtNameTmp.TrimLeft(_U('.'));
	
	struct dirent *pItem = NULL;
#ifdef _UGUNICODE
	YkMBString strLocalPath;
	strPathTmp.ToMBString(strLocalPath);
	DIR *pDir = opendir(strLocalPath.Cstr());
	YkUnicodeString strUName;
#else
	DIR *pDir = opendir(strPathTmp.Cstr());
#endif
	
	if (pDir) 
	{
		while ( (pItem=readdir(pDir)) != NULL )
		{
#ifdef _UGUNICODE
			strUName.FromMBString(pItem->d_name, strlen(pItem->d_name));
			YkString strCurExtName = YkSystem::GetExtName(strUName);
#else
			YkString strCurExtName = YkSystem::GetExtName(pItem->d_name);
#endif
			strCurExtName.TrimLeft(_U('.'));
			if (strCurExtName.CompareNoCase(strExtNameTmp) == 0)
			{
#ifdef _UGUNICODE
				YkString strName = strUName;
#else
				YkString strName = pItem->d_name;
#endif
				if (bAddPath)
				{
					strName = strPath+strName;
				}
				strFileNames.Add(strName);
			}
		}
		closedir(pDir);
	}

	return strFileNames.GetSize()>0;
}

#endif

}
 
