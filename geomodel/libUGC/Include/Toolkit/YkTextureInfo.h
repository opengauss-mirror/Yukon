/*
 *
 * YkTextureInfo.h
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

#ifndef AFX_YKTEXTUREINFO_H__3ABF090C_7A5D_4B37_9336_66EC397A4088__INCLUDED_
#define  AFX_YKTEXTUREINFO_H__3ABF090C_7A5D_4B37_9336_66EC397A4088__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include "Base/YkString.h"
#include "Stream/YkStream.h"

#define MACHINE_TAG (_U("TexImage@")) 

namespace Yk
{
	class TOOLKIT_API YkTextureInfo
	{
	public:
		YkTextureInfo();
		~YkTextureInfo();
		const YkTextureInfo& operator=(const YkTextureInfo& Info);
		YkTextureInfo(const YkTextureInfo& Info);
		// 接管了外边给的内存
		YkTextureInfo(Yk::PixelFormat enFormat,YkUint Width,YkUint Height,void* data,YkUint nSize, const YkString& path = _U(""));

		//! \brief 从流文件中加载
		//! \param fStream 文件流[in]。
		void Load(YkStream& fStream);

		//! \brief 存入流文件中
		//! \param fStream 文件流[in]。
		void Save(YkStream& fStream);

		//! \brief 获取数据存储时的大小
		//! \return 返回存储大小
		//! \remark 暂未考虑编码情况，与Save()一起维护
		//! \attention 字符串的长度要+4
		YkInt GetDataSize();
	public:
		YkBool			m_bLoadImage;

		// 纹理图片的像素格式
		Yk::PixelFormat		m_enFormat;

		// 纹理图片的宽度
		YkUint		m_nWidth;

		// 纹理图片的高度
		YkUint		m_nHeight;

		// 纹理图片流数据
		YkByte*		m_pBuffer;

		// 纹理图片流数据的长度,带压缩时很重要
		YkUint		m_nSize;

		// 纹理图片的索引路径
		YkString		m_strPath;

		YkString		m_strName;

		// 纹理图片数据中一共有几层mipmap
		YkUint        m_nMipLev;

		// 纹理图片的压缩类型
		YkInt			m_nCompress;
	};
}
#endif

