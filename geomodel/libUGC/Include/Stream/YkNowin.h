/*
 *
 * YkNowin.h
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

#ifndef AFX_YKNOWIN_H__CF39CFEE_3784_4470_8C7E_CD6EE3771EB1__INCLUDED_
#define AFX_YKNOWIN_H__CF39CFEE_3784_4470_8C7E_CD6EE3771EB1__INCLUDED_        

#pragma once

// 头文件包含
// 定义宏，使得可以处理大文件（>4GB）
#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64

#include <fcntl.h>
#include "Base/YkDefs.h"
#include "Stream/YkExports.h"

using namespace Yk;

#ifdef _UGUNICODE
	EXTERN_C STREAM_API YkInt Yk_open(const YkChar* pFileName, YkInt nOpenFlag, YkInt nPermissionMode = 0);
	#define Ykopen Yk_open	//打开文件
	EXTERN_C STREAM_API YkInt Yk_access(const YkChar *path, YkInt mode );
	#define Ykaccess Yk_access	//打开文件
	EXTERN_C STREAM_API YkInt Yk_remove(const YkChar *path);
	#define Ykremove Yk_remove	//打开文件
	EXTERN_C STREAM_API YkInt Yk_rename(const YkChar *oldname, const YkChar *newname );
	#define Ykrename Yk_rename	//打开文件
	EXTERN_C STREAM_API FILE* Yk_fopen(const YkChar *path, const YkChar *mode);
	#define Ykfopen Yk_fopen	//打开文件
	EXTERN_C STREAM_API FILE* Yk_fopen64(const YkChar *path, const YkChar *mode);
	#define Ykfopen64 Yk_fopen64	//打开文件
#else
	#define Ykopen	open	//打开文件
	#define Ykaccess access
	#define Ykremove remove
	#define Ykrename rename
	#define Ykfopen fopen
	#define Ykfopen64 fopen64
	#define Ykstrlen strlen
	#define Ykstrcpy strcpy
#endif


#define Ykwrite	write	//写文件
#define Ykread	read	//读文件
#define Ykseek	lseek	//定位文件
#define Ykclose	close	//关闭文件
#define Ykchsize ftruncate
#if defined(Yktell)
#undef Yktell
#endif
// 查询文件位置
#define Yktell(h) (YkLong)lseek(h, 0, SEEK_CUR)
#define Yksleep(s) sleep(s/1000)
#define Ykfwrite	fwrite
#define Ykfclose	fclose
#define Ykfread		fread
#define Ykfseek		fseeko64

//是否是binary文件
#define YkBINARY 0
//是否是read/write文件
#define YkREADWRITE 0666

// 是否是目录
#define YkDIRECTORY 0x4000


#ifndef O_BINARY
	#define O_BINARY 0
#endif
//路径sep
#define PATHSEP _U('/')
//路径sep
#define PATHSEPSTRING _U("/")
//路径listSep
#define PATHLISTSEP _U(':')
//路径listSep
#define PATHLISTSEPSTRING _U(":")
//是否是路径 sep
#define ISPATHSEP(c) ((c)==_U('/'))
#define EOLSTRING _U("\n")

#define YKCALLBACK  
#define YKSTDCALL

/*设置函数在生成符号表时为不可见,防止因为linux同名符号只加载一次，函数重名导致加载符号表出错。
如果回调函数不跨工程调用，加上此宏*/
#define SETVISIBILITYISHIDDEN __attribute__((visibility("hidden")))

namespace Yk {

// 变量类型定义
typedef unsigned long YkThreadID;
typedef void*		  YkHandle;

}

#endif

