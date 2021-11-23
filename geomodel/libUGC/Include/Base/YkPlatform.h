/*
 *
 * YkPlatform.h
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

#ifndef AFX_YKPLATFORM_H__948BECAF_793C_4C31_B20C_D659B42F2CB8__INCLUDED_
#define AFX_YKPLATFORM_H__948BECAF_793C_4C31_B20C_D659B42F2CB8__INCLUDED_

#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include "Base/YkDefs.h"

#ifdef _UGQSTRING
#ifdef WIN32
#define YKstrcpy	wcscpy
#define YKstrncpy	wcsncpy
#define YKstrlen(x)	wcslen((const YkWchar*)(x))
#define YKstrcat	wcscat 
#define YKstrcmp	wcscmp
#define YKstrncmp	wcsncmp
#define YKstrncasecmp wcsnicmp
#define YKstrchr	wcschr
#define YKstrstr	wcsstr
#else
#define YKstrcpy	yk_strcpy
#define YKstrncpy	yk_strncpy
#define YKstrlen(x)	yk_strlen((const YkWchar*)(x))
#define YKstrcat	yk_strcat 
#define YKstrcmp	yk_strcmp
#define YKstrncmp	yk_strncmp
#define YKstrncasecmp wcsncasecmp
#define YKstrchr	yk_strchr
#define YKstrstr	yk_strstr
#endif

#else
#define YKstrcpy	u_strcpy
#define YKstrncpy	u_strncpy
#define YKstrlen(x)	u_strlen((const UChar*)(x))
#define YKstrcat	u_strcat 
#define YKstrcmp	u_strcmp
#define YKstrncmp	u_strncmp
#define YKstrncasecmp	u_strncasecmp
#define YKstrchr	u_strchr
#define YKstrstr	u_strstr
#endif

using namespace Yk;
extern BASE_API YkWchar* yk_strcpy( YkWchar* wsDes, const YkWchar* wsSrc);
extern BASE_API YkWchar* yk_strncpy( YkWchar* wsDes, const YkWchar* wsSrc, YkInt count);
extern BASE_API YkInt yk_strlen(const YkWchar* pStr);
extern BASE_API YkInt yk_strcmp(const YkWchar* ws1, const YkWchar* ws2);
extern BASE_API YkInt yk_strncmp(const YkWchar* ws1, const YkWchar* ws2, YkInt count);
extern BASE_API YkWchar* yk_strcat( YkWchar* wsDes, const YkWchar* wsSrc);
extern BASE_API YkWchar* yk_strchr(const YkWchar* ws, YkWchar wc);
extern BASE_API YkWchar* yk_strstr(YkWchar* ws, const YkWchar* wsub);
extern size_t BASE_API YK_vsnprintf(YkWchar *buf, size_t size, const YkWchar *fmt, va_list args);

extern BASE_API size_t YKstrftime(YkChar* szBuffer, size_t maxsize, const YkChar* Format, const struct tm *timeptr);

#ifndef WIN32
#define _MAX_PATH 260
#endif

#endif

