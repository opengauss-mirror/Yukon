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

#ifndef AFX_YKPLATFORM_H__5C93C974_69BF_4477_8FCF_856BA63D224A__INCLUDED_
#define AFX_YKPLATFORM_H__5C93C974_69BF_4477_8FCF_856BA63D224A__INCLUDED_

#pragma once

#include "Stream/YkExports.h"
#include "Base/YkDefs.h"

using namespace Yk;

EXTERN_C STREAM_API YkChar* Yk_strcpy( YkChar *strDestination, const YkChar *strSource );
#define Ykstrcpy	Yk_strcpy	
EXTERN_C STREAM_API YkChar* Yk_strncpy( YkChar *strDestination, const YkChar *strSource, YkInt n);
#define Ykstrncpy	Yk_strncpy
EXTERN_C STREAM_API YkInt Yk_strlen(const YkChar *string);
#define Ykstrlen	Yk_strlen
EXTERN_C STREAM_API YkChar* Yk_strcat(YkChar  *dst, const YkChar *src);
#define Ykstrcat	Yk_strcat 
EXTERN_C STREAM_API YkInt Yk_strcmp(YkChar  *s1, const YkChar *s2);
#define Ykstrcmp	Yk_strcmp
EXTERN_C STREAM_API YkInt Yk_strncmp (const YkChar *ucs1, const YkChar *ucs2, YkInt n);
#define Ykstrncmp	Yk_strncmp
EXTERN_C STREAM_API YkInt Yk_strncasecmp (const YkChar *s1, const YkChar *s2, YkInt n);
#define Ykstrnicmp Yk_strncasecmp
EXTERN_C STREAM_API YkChar* Yk_strchr( const YkChar *string, YkChar c);
#define Ykstrchr	Yk_strchr
EXTERN_C STREAM_API YkChar* Yk_strstr (const YkChar* s, const YkChar *substring);
#define Ykstrstr	Yk_strstr
EXTERN_C STREAM_API YkInt Yk_strtol( const YkChar *nptr, YkChar **endptr, YkInt base );
#define Ykstrtol Yk_strtol


extern STREAM_API YkInt Yksprintf( YkChar *buffer, YkInt nMaxSize, const YkChar *format, ...);
extern STREAM_API YkInt Ykvfprintf(FILE* stream,const YkChar *format,va_list argptr);


#ifdef WIN32
	#include "YkWin32.h"
#endif

#ifndef WIN32
	#include "YkNowin.h"
#endif

#endif
