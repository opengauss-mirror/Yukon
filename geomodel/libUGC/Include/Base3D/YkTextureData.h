/*
 *
 * YkTextureData.h
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

#if !defined(AFX_YKTEXTUREDATA_H__208EFE4E_3502_4F4D_9705_1198D58E48C1__INCLUDED_)
#define AFX_YKTEXTUREDATA_H__208EFE4E_3502_4F4D_9705_1198D58E48C1__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Toolkit/YkTextureInfo.h"
#include "Stream/YkStream.h"

namespace Yk {
//! \brief 纹理数据的封装类。
class BASE3D_API YkTextureData
{
public:
	//! \brief 构造函数
	YkTextureData();
	//! \brief 拷贝构造函数
	YkTextureData(const YkTextureData& other);
	//! \brief 拷贝构造函数
	YkTextureData(const YkTextureInfo& textureInfo);
	//! \brief 赋值函数
	YkTextureData& operator=(const YkTextureData& other);

	//! \brief 克隆自己的数据
	virtual YkTextureData* Clone();
	//! \brief 析构函数
	virtual ~YkTextureData();
	//! \brief 释放纹理数据
	void Release();

	//! \brief 从流文件中加载
	YkBool Load(YkStream& fStream ,YkUint eCodecType = 0);

	//! \brief 存入流文件中
	void Save(YkStream& fStream,YkUint eCodecType = 0);

private:
	//! \brief 存入流文件中
	//! \remarks 实现纹理的DDS编码及zip压缩
	YkBool BuildTextureTier(YkStream& fStream);

public:
	//! \brief 纹理数据内存
	YkUchar* m_pBuffer;
	//! \brief 纹理的高
	YkUint m_nHeight;
	//! \brief 纹理的宽
	YkUint m_nWidth;
	//! \brief 纹理的深度
	YkUint m_nDepth;
	//! \brief 纹理的像素格式，目前只支持RGBA
	Yk3DPixelFormat m_enFormat;

	// 压缩纹理类型
	YkUint m_CompressType;

	// 数据的内存大小
	YkUint m_nSize;

	//! \brief 原始压缩方式
	YkUint m_OriCompressType;
};

}

#endif

