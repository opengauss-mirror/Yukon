/*
 *
 * YkWin32.h
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

#ifndef AFX_YKWIN32_H__69258408_138D_402D_92D0_0051E78F6036__INCLUDED_
#define AFX_YKWIN32_H__69258408_138D_402D_92D0_0051E78F6036__INCLUDED_

#pragma once

// base的线程类会使用
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0600	// Change this to the appropriate value to target Windows 2000 or later.
#endif	


#pragma warning(disable: 4018)  // signed/unsigned mismatch
#pragma warning(disable: 4097)
#pragma warning(disable: 4100)
#pragma warning(disable: 4164)
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)	// security warning about POSIX functions
#pragma warning(disable: 4514)
#pragma warning(disable: 4663)

// 头文件包含 
#include <io.h>
#ifdef _UGUNICODE
	//打开文件
	#define Ykopen	_wopen
	#define Ykfopen_s _wfopen_s  
	// 字符串长度
	#define Ykaccess _waccess
	#define Ykremove _wremove
	#define Ykrename _wrename
	#define Ykfopen _wfopen

	#define Yk_findfirst _wfindfirst
	#define Yk_finddata_t _wfinddata_t
	#define Yk_findnext _wfindnext
	#define Yk_findclose _findclose
	#define Yk_stat	_wstat
	#define Yk_stati64 _wstati64 
	#define Yk_mkdir _wmkdir
	#define Yk_rmdir _wrmdir
	#define Yk_getcwd _wgetcwd
#else
	//打开文件
	#define Ykopen	_open
	#define Ykaccess _access
	#define Ykremove remove
	#define Ykrename rename
	#define Ykfopen fopen
	#define Ykfopen_s fopen_s  
	#define Yk_findfirst _findfirst
	#define Yk_finddata_t _finddata_t
	#define Yk_findnext _findnext
	#define Yk_findclose _findclose
	#define Yk_stat	_stat
	#define Yk_stati64 _stati64 
	#define Yk_mkdir _mkdir
	#define Yk_rmdir _rmdir
	#define Yk_getcwd _getcwd
#endif

#define Ykwrite	_write	// 写文件
#define Ykread	_read	// 读文件
#define Ykseek	_lseeki64	// 定位文件
#define Ykclose	_close	// 关闭文件
#define Ykchsize _chsize
#define Yktell _telli64	// 查询文件位置
#define Yksleep Sleep
#define Ykfwrite fwrite
#define Ykfclose	fclose

#define	Ykfseek _fseeki64

		
#define Ykfread fread


//是否是binary文件
#define YkBINARY O_BINARY
//是否是read/write文件
#define YkREADWRITE (_S_IREAD|_S_IWRITE)

// 是否是目录
#define YkDIRECTORY _S_IFDIR

//路径sep
#define PATHSEP _U('\\')
//路径sep
#define PATHSEPSTRING _U("\\")
//路径listSep
#define PATHLISTSEP _U(';')
//路径listSep
#define PATHLISTSEPSTRING _U(";")
//是否是路径 sep
#define ISPATHSEP(c) ((c)==_U('/') || (c)==_U('\\'))
//换行符号
#define EOLSTRING _U("\r\n")

#define YKSTDCALL __stdcall
/*设置函数在生成符号表时为不可见,防止因为linux同名符号只加载一次，函数重名导致加载符号表出错。
如果回调函数不跨工程调用，加上此宏*/
#define SETVISIBILITYISHIDDEN

namespace Yk {

// 变量定义
typedef void*         YkThreadID;
typedef unsigned int  YkHandle;

}

#endif

