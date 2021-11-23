/*
 *
 * YkCompress.cpp
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

#ifdef WIN32 
#include <Windows.h>
#endif

#include "Toolkit/YkCompress.h"
#include "zlib.h"
#include "Stream/YkFile.h"

namespace Yk {

/*===========================================================================
根据C++编码规范,目的参数在前,源参数在后,故而对Sm30中参数的位置和名称做了相应的改动
解压缩数据参数说明
void *pvDestBuffer: 目标内存区域 ,dwDestLen: 目标内存区域大小
const void *pvSrcBuffer: 源数据; YkUint dwSrcLen:源数据长度

目标内存区域需要事先开辟，至少要有 dwSrcLen * 101% + 12,进行解压缩后，
内存的大小会改变，dwDestLen传出改变后的大小;
pvSrcBuffer和pvDestBuffer不能相同
Decompress returns Z_OK if success, Z_MEM_ERROR if there was not
enough memory, Z_BUF_ERROR if there was not enough room in the output
buffer, or Z_DATA_ERROR if the input data was corrupted.
===========================================================================*/
YkBool YkCompress::UnZip(YkByte *pvDestBuffer, YkUint &dwDestLen, 
	const YkByte *pvSrcBuffer, YkUint dwSrcLen)
{
	unsigned long ulDestLen = dwDestLen;
	YkBool bResult = FALSE;
	YkInt nResult = uncompress(pvDestBuffer, &ulDestLen, pvSrcBuffer, dwSrcLen);
	dwDestLen = (YkUint)ulDestLen;

	// 防错处理
	if (nResult == Z_OK)
	{
		bResult = TRUE;
	}
	else if (nResult == Z_MEM_ERROR)
	{
	}
	else if (nResult == Z_BUF_ERROR)
	{
	}
	else if (nResult == Z_STREAM_ERROR)
	{
	}
	else if (nResult == Z_DATA_ERROR)
	{
	}
	else 
	{ // 不可识别的错误，
		YKASSERT(FALSE); 
	}

	return bResult;
}

//1. Zip 压缩 与 UnZip解压缩
/*===========================================================================
根据C++编码规范,目的参数在前,源参数在后,故而对Sm30中参数的位置和名称做了相应的改动
压缩数据参数说明
void *pvDestBuffer: 目标内存区域 ,dwDestLen: 目标内存区域大小
const void *pvSrcBuffer: 源数据; YkUint dwSrcLen:源数据长度

目标内存区域需要事先开辟，至少要有 dwSrcLen * 1% + 12,	进行压缩后，
内存的大小会改变，dwDestLen 传出改变后的大小;
pvSrcBuffer和pvDestBuffer不能相同
参数YkInt nLevel 可以取1到9之间的整数.
nLevel越小，压缩过程中用的内存最少，但是压缩速度慢，压缩率低;
nLevel越大，压缩过程中用的内存最多，但是压缩速度快，压缩率高;

compress returns Z_OK if success, Z_MEM_ERROR if there was not enough
memory, Z_BUF_ERROR if there was not enough room in the output buffer,
Z_STREAM_ERROR if the level parameter is invalid.
compress2 (dest, dwDesLen, source, dwDesLen, level)
===========================================================================*/
YkBool YkCompress::Zip(YkByte *pvDestBuffer, YkUint &dwDestLen, 
	const YkByte *pvSrcBuffer, YkUint dwSrcLen, YkInt nLevel)
{
	unsigned long ulDestLen = dwDestLen;
	YkBool bResult = FALSE;
	YkInt nResult = compress2(pvDestBuffer, &ulDestLen, pvSrcBuffer, dwSrcLen, nLevel);
	dwDestLen = (YkUint)ulDestLen;

	// 防错处理
	if (nResult == Z_OK)
	{
		bResult = TRUE;	
	}
	else if (nResult == Z_MEM_ERROR)
	{
	}
	else if (nResult == Z_BUF_ERROR)
	{
	}
	else if (nResult == Z_STREAM_ERROR)
	{
	}
	else
	{ // 不可识别的错误，
		YKASSERT(FALSE); 
	}

	return bResult;
}
Yk::YkInt YkCompress::CompressBound(YkInt nSrcSize)
{
	if (nSrcSize>0)
	{
		return compressBound(nSrcSize);
	}
	return 0;
}

}
