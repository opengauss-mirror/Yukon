/*
 *
 * YkPlatform.cpp
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

#include "Stream/YkPlatform.h"
#include "Base/YkPlatform.h"
#include "Base/YkString.h"
#if defined _UGUNICODE && !defined _UGQSTRING
#include "unicode/ustring.h"
#endif

using namespace Yk;

#ifdef _UGUNICODE
	YkChar* Yk_strcpy( YkChar *strDestination, const YkChar *strSource)
	{
#ifdef _UGQSTRING
		return (YkChar*)YKstrcpy((YkWchar*)strDestination, (YkWchar*)strSource);
#else
		return (YkChar*)u_strcpy((UChar*)strDestination, (UChar*)strSource);
#endif
	}

	extern STREAM_API  YkChar* Yk_strncpy( YkChar *strDestination, const YkChar *strSource, YkInt n )
	{
#ifdef _UGQSTRING
		return (YkChar*)YKstrncpy((YkWchar*)strDestination, (YkWchar*)strSource, n);
#else
		return (YkChar*)u_strncpy((UChar*)strDestination, (UChar*)strSource, n);
#endif
	}

	extern STREAM_API YkInt Yk_strlen( const YkChar *string )
	{
#ifdef _UGQSTRING
		return YKstrlen(string);
#else
		return u_strlen((UChar*)string);
#endif
	}

	extern STREAM_API  YkChar* Yk_strcat( YkChar *dst, const YkChar *src )
	{
#ifdef _UGQSTRING
		return (YkChar*)YKstrcat((YkWchar*)dst, (YkWchar*)src);
#else
		return (YkChar*)u_strcat((UChar*)dst, (UChar*)src);
#endif
	}

	extern STREAM_API  YkInt Yk_strcmp( YkChar *s1, const YkChar *s2 )
	{
#ifdef _UGQSTRING
		return YKstrcmp((YkWchar*)s1, (YkWchar*)s2);
#else
		return u_strcmp((UChar*)s1, (UChar*)s2);
#endif
	}

	extern STREAM_API  YkInt Yk_strncmp( const YkChar *ucs1, const YkChar *ucs2, YkInt n )
	{
#ifdef _UGQSTRING
		return YKstrncmp((YkWchar*)ucs1, (YkWchar*)ucs2, n);
#else
		return u_strncmp((UChar*)ucs1, (UChar*)ucs2, n);
#endif
	}

	extern STREAM_API  YkInt Yk_strncasecmp( const YkChar *s1, const YkChar *s2, YkInt n)
	{
#ifdef _UGQSTRING
		return YKstrncasecmp((YkWchar*)s1, (YkWchar*)s2, n);
#else
		return u_strncasecmp((UChar*)s1, (UChar*)s2, n, 0);
#endif
	}

	extern STREAM_API  YkChar* Yk_strchr( const YkChar *string, YkChar c )
	{
#ifdef _UGQSTRING
		return (YkChar*)YKstrchr((YkWchar*)string, c);
#else
		return (YkChar*)u_strchr((UChar*)string, c);
#endif
	}

	extern STREAM_API  YkChar* Yk_strstr( const YkChar* s, const YkChar *substring )
	{
#ifdef _UGQSTRING
		return (YkChar*)YKstrstr((YkWchar*)s, (YkWchar*)substring);
#else
		return (YkChar*)u_strstr((UChar*)s, (UChar*)substring);
#endif
	}


	extern STREAM_API  YkInt Yk_strtol( const YkChar *nptr, YkChar **endptr, YkInt base )
	{
		if(endptr == NULL ||*endptr == NULL)
		{
			YkUnicodeString str(nptr);
			return str.ToInt(base);
		}
		else
		{
			YkUnicodeString str(nptr, *endptr-nptr);
			return str.ToInt(base);
		}
	}
#endif

	extern STREAM_API  YkInt Yksprintf( YkChar *buffer, YkInt nMaxSize, const YkChar *format, ... )
	{
		va_list args;	
		va_start(args,format);
#ifdef _UGUNICODE
		YkInt nResult=YK_vsnprintf(buffer,nMaxSize,format,args);
#else
		YkInt nResult=vsnprintf(buffer,nMaxSize,format,args);
#endif
		va_end(args);	
		return nResult;
	}
	extern STREAM_API YkInt Ykvfprintf(FILE* stream,const YkChar *format,va_list argptr)
	{
#ifdef _UGUNICODE
#if SYSTEM_WCHAR_SIZE==2
		return vfwprintf(stream,format,argptr);
#else
		YkInt nFormatLen = YKstrlen(format);
		YkWchar32* pTempFormat = new YkWchar32[nFormatLen+1];
		wmemset((wchar_t*)pTempFormat, '\0', nFormatLen+1);
		YkString::WcharToWchar32(format, pTempFormat, nFormatLen+1);
		YkInt nCharaterCount = vfwprintf(stream,(wchar_t*)pTempFormat,argptr);
		delete[] pTempFormat;
		return nCharaterCount;
#endif
#else
		return vfprintf(stream,format,argptr);
#endif

	}
