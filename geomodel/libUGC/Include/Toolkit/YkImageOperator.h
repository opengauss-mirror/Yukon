/*
 *
 * YkImageOperator.h
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

#ifndef  AFX_YKIMAGEOPERATOR_H__3BAD902C_4420_4789_A261_A87DB79DA0A9__INCLUDED_
#define  AFX_YKIMAGEOPERATOR_H__3BAD902C_4420_4789_A261_A87DB79DA0A9__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include "Toolkit/YkTextureInfo.h"
#include "Toolkit/YkDataCodec.h"
#include <algorithm>

#define DXT1_ENCODE_SIZE(w,h) (((w-1)/4 + 1)*((h-1)/4 + 1)*8)
#define DXT3_ENCODE_SIZE(w,h) (((w-1)/4 + 1)*((h-1)/4 + 1)*16)
#define DXT5_ENCODE_SIZE DXT3_ENCODE_SIZE

#define PVRTC_RGBA2_ENCODE_SIZE(w,h) ((YkMAX((int)w,16) * YkMAX((int)h, 8) * 2 + 7)/8)

#define PVRTC_RGBA4_ENCODE_SIZE(w,h) ((YkMAX((int)w, 8) * YkMAX((int)h, 8) * 4 + 7)/8)
#define ETC1_RGB8_ENCODE_SIZE(w,h) ((w*h)>>1)
#define ETC1_RGBA8_ENCODE_SIZE ETC1_RGB8_ENCODE_SIZE

using namespace Yk;

class TOOLKIT_API YkImageOperator
{
public:
	// format 原图像的像素格式
	// width,height 原图像的宽度和高度
	// in 原图像的数据
	// comtype 图像压缩类型
	// out  压缩后的输出,内部分配空间
	// return 返回压缩后图像数据的大小
	static YkUint Encode( const YkUint pixsize, YkUint &nWidth, YkUint &nHeight,YkByte* pBufferIn,YkByte **ppBufferOut,YkUint eCodecType =YkDataCodec::enrS3TCDXTN,YkBool bGeneMipmaps =FALSE, YkUint eQualityLevel = 128);

	//// 解压缩dxtn
	static YkUint Decode( const YkUint pixsize, YkUint nWidth, YkUint nHeight,YkByte** ppBufferOut,const YkByte *pBufferIn ,YkUint eCodecType,YkBool bGeneMipmaps =FALSE, YkUint nTexSize = 0);

	// 按照指定的大小缩放图片
	static void Scale(const YkUint components, YkUint widthin, YkUint heightin,YkByte *pBufferIn,
					        YkUint widthout, YkUint heightout, YkByte *pBufferOut);

 	// 获取mipmap的数据
	static YkUint GetMipMapData(YkUint nWidth,YkUint nHeight,const YkByte* pBufferIn,YkByte** ppBufferOut,YkArray<YkUint>& arrByteOffset);

 	//! \brief  返回比a大的，并且是最接近a的2的次方的数。
 	static YkUint NextP2 (YkUint a);


	//! \brief 获取指定压缩格式的纹理压缩后的大小
	static YkUint GetCompressedTextureSize(const YkUint pixsize,YkUint nWidth,YkUint nHeight,YkUint eCodecType,YkBool bIsMipmaps =FALSE);

	//! \brief 根据宽高获取mipMap层级
	static YkUint GetMipMapLevel(const YkUint nWidth,const YkUint nHeight);

	//! \是否是压缩纹理
	static YkBool IsCompressedTextureType(YkDataCodec::CodecType eType);

private:

public:
#ifdef __linux__
		//! \brief 显卡是否支持DXT压缩
		static YkBool m_IsSupportDxt;
#endif
};
#endif

