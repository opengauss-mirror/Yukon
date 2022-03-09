/*
 *
 * YkToolkit.cpp
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

#include "Toolkit/YkToolkit.h"
#include "Toolkit/YkTextCodec.h"
#include "Stream/YkFile.h"
#include "Stream/YkFileStdio.h"
#include "Toolkit/YkHashCode.h"

#ifdef WIN32
#include <windows.h>
#endif

// 创建GUID
#if !defined(__linux__)
#include <objbase.h>
#else
#if (__GNUC__ <= 4 && __GNUC_MINOR__ < 9)
// @cemment: Linux编译不过可以注释掉(包括CreateGUID函数中的实现)
// 或者提供系统的预编译宏和uuid.h文件所在路径给作者，谢谢！！！
#include <uuid/uuid.h>
#endif
#endif

namespace Yk
{

YkMutex g_MutexToolkit;

void* YkToolkit::LoadDll(const YkString& strLibName)
{
	return YkSystem::LoadDll(strLibName);
}


YkBool YkToolkit::FreeDll(void* hModule)
{
	return YkSystem::FreeDll(hModule);
}

void* YkToolkit::GetProcAddress(void* hModule, const YkString& strProName)
{	
	return YkSystem::GetProcAddress(hModule,strProName);
}

YkString YkToolkit::GetXmlFileHeader(YkMBString::Charset charset/* = GetCurCharset()*/)
{
	YkString strXmlHeader = (_U("<?xml version=\"1.0\" encoding=\""));
	strXmlHeader += YkTextCodec::GetCharsetName(charset);	
	strXmlHeader += _U("\"?>");
	return strXmlHeader;
}

YkUint YkToolkit::GetCPUCount()
{
#if defined(WIN32)
	_SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
#elif defined(__linux__) 
// linux 
//more /proc/cpuinfo |grep processor|grep -c processor
//more /proc/cpuinfo |grep processor|sed '$!d'|awk '{printf $3}' +1
	YkString strTempFile = _U("/proc/cpuinfo");
	YkUint nCpuCount = 0;
	YkFileStdio fileTemp;
	YkString strInfo;
	if( fileTemp.Open(strTempFile.Cstr(), YkStreamLoad))
	{
		while (!fileTemp.IsEOF())
		{
			fileTemp.ReadLine(strInfo);
			if (strInfo.Find(_U("processor")) != -1)
			{
				++nCpuCount;
			}
		}
		fileTemp.Close();
	}
	return nCpuCount;
#endif
}

Yk::YkBool YkToolkit::IsUTF8(YkMBString &strSrc)
{
	YkInt i = 0;
	YkUint nBytes = 0;
	YkUchar chr = 0x0;
	YkBool bAllAscii = TRUE; // 如果全部都是ASCII, 说明不是UTF-8
	
	for(i = 0; i < strSrc.GetLength(); i ++)
	{
		chr = strSrc.GetAt(i);
		if( (chr & 0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii = FALSE;
		if(nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if(chr >= 0x80)
			{
				if(chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if(chr >= 0xF8)
					nBytes = 5;
				else if(chr >= 0xF0)
					nBytes = 4;
				else if(chr >= 0xE0)
					nBytes = 3;
				else if(chr >= 0xC0)
					nBytes = 2;
				else
				{
					return FALSE;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return FALSE;
			}
			nBytes--;
		}
	}
	if( nBytes > 0 ) //违返规则
	{
		return FALSE;
	}
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
	{
		return FALSE;
	}
	return TRUE;
}

YkString YkToolkit::CreateGuid()
{
	YkString strGUID = _U("");
#if !defined(__linux__)
	GUID guid;
	if (S_OK == CoCreateGuid(&guid))
	{
		strGUID.Format(_U("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
#else
#if (__GNUC__ <= 4 && __GNUC_MINOR__ < 9)
	uuid_t guid;
	uuid_generate(guid);
	strGUID.Format(_U("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
		guid[0], guid[1], guid[2], guid[3], 
		guid[4], guid[5], guid[6], guid[7], 
		guid[8], guid[9], guid[10], guid[11],
		guid[12], guid[13], guid[14], guid[15]);
#endif
#endif

	return strGUID;
}

}
