/*
 *
 * YkCompress.h
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

#ifndef AFX_YKCOMPRESS_H__DED677A9_D2F3_4C55_A76B_765BF91BCAE4__INCLUDED_
#define AFX_YKCOMPRESS_H__DED677A9_D2F3_4C55_A76B_765BF91BCAE4__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"

namespace Yk {

//! \brief 压缩算法类。
//! \remarks 目前仅提供Zip的压缩和解压缩。
class TOOLKIT_API YkCompress  
{
public:
	//! \brief Zip 压缩函数
	/** \details 根据C++编码规范,目的参数在前,源参数在后,故而对Sm30中参数的位置和名称做了相应的改动
	压缩数据参数说明
	void *pvDestBuffer: 目标内存区域 ,dwDestLen: 目标内存区域大小
	const void *pvSrcBuffer: 源数据; YkUint dwSrcLen:源数据长度
	
	  目标内存区域需要事先开辟，至少要有 dwSrcLen * 1% + 12,	进行压缩后，
	  dwDestLen可以用compressBound函数进行计算；
	  内存的大小会改变，dwDestLen 传出改变后的大小;
	  pvSrcBuffer和pvDestBuffer不能相同
	  参数YkInt nLevel 可以取1到9之间的整数.
	  nLevel越小，压缩过程中用的内存最少，但是压缩速度慢，压缩率低;
	  nLevel越大，压缩过程中用的内存最多，但是压缩速度快，压缩率高;
	  
		compress returns Z_OK if success, Z_MEM_ERROR if there was not enough
		memory, Z_BUF_ERROR if there was not enough room in the output buffer,
		Z_STREAM_ERROR if the level parameter is invalid.
		compress2 (dest, dwDesLen, source, dwDesLen, level)
	*/
	static YkBool Zip(YkByte *pvDestBuffer, YkUint &dwDestLen, 
		const YkByte *pvSrcBuffer, YkUint dwSrcLen, YkInt nLevel=8);
	
	//! \brief UnZip解压缩函数
	/** \details 根据C++编码规范,目的参数在前,源参数在后,故而对Sm30中参数的位置和名称做了相应的改动
	解压缩数据参数说明
	void *pvDestBuffer: 目标内存区域 ,dwDestLen: 目标内存区域大小
	const void *pvSrcBuffer: 源数据; YkUint dwSrcLen:源数据长度
	
	  目标内存区域需要事先开辟，至少要有 dwSrcLen * 101% + 12,进行解压缩后，
	  内存的大小会改变，dwDestLen传出改变后的大小;
	  pvSrcBuffer和pvDestBuffer不能相同
	  Decompress returns Z_OK if success, Z_MEM_ERROR if there was not
	  enough memory, Z_BUF_ERROR if there was not enough room in the output
	  buffer, or Z_DATA_ERROR if the input data was corrupted.
	*/
	static YkBool UnZip(YkByte *pvDestBuffer, YkUint &dwDestLen, 
		const YkByte *pvSrcBuffer, YkUint dwSrcLen);

	//!  \brief 用于计算进行压缩时所需要开辟的目标存储区域的大小
	//!  \param nSrcSize 需要压缩的源buf的大小
	//!  \return 压缩后的存储区域所需开辟的存储空间的最大可能大小
	static YkInt CompressBound(YkInt nSrcSize);
	
public:

	YkCompress() {m_uzFile = NULL;};

	virtual ~YkCompress() {};

private:

	void* m_uzFile;
};

}

#endif


