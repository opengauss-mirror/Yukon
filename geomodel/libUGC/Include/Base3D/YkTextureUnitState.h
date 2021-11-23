/*
 *
 * YkTextureUnitState.h
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

#if !defined(AFX_YKTEXTUREUNITSTATE_H__8CD48F14_6B32_4535_AAFC_1AE07A7A4CF6__INCLUDED_)
#define AFX_YKTEXTUREUNITSTATE_H__8CD48F14_6B32_4535_AAFC_1AE07A7A4CF6__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Toolkit/YkColorValue3DEx.h"
#include "Base3D/YkMatrix4d.h"
#include "Stream/YkStream.h"

namespace Yk {
class YkFrustum;

class BASE3D_API YkTextureUnitState  
{
public:
    enum EnvMapType
    {
        ENV_PLANAR,
       
        ENV_CURVED,
        
        ENV_REFLECTION,
       
        ENV_NORMAL,
    };

    enum TextureAddressingMode
    {
        TAM_WRAP,
       
        TAM_MIRROR,
       
        TAM_CLAMP,
       
        TAM_BORDER,
    };

	struct UVWAddressingMode
	{
		TextureAddressingMode u, v, w;
	};

public:
	YkTextureUnitState();

	YkTextureUnitState(const YkTextureUnitState& oth);

	virtual ~YkTextureUnitState();
	
	//! \brief 从流文件中加载
	//! \param fStream 文件流[in]。
	YkBool Load(YkStream& fStream);

	//! \brief 存入流文件中
	//! \param fStream 文件流[in]。
	void Save(YkStream& fStream);

	//! \brief 获取数据存储时的大小
	//! \return 返回存储大小
	//! \remark 暂未考虑编码情况，与Save()一起维护
	//! \attention 字符串的长度要+4
	YkInt GetDataSize() const;
	
public:
	// optional name for the TUS
	YkString m_strName;

	// optional alias for texture frames
	YkString m_strTextureNameAlias;

	// 纹理单元使用的纹理名称
	YkString m_strTextureName;

	// 使用的立方体纹理名称
	YkString m_strCubicTextureName;

	// The index of the texture coordinate set to use.
	YkUint m_unTextureCoordSetIndex;

	// 设置纹理坐标分配模式
	UVWAddressingMode m_AddressMode;

	//! \brief 缩小时的滤波类型
	FilterOptions m_MinFilter;

	//! \brief 放大时的滤波类型
	FilterOptions m_MaxFilter;

	//! \brief Mipmap时滤波类型
	FilterOptions m_MipFilter;



	/* 采用的环境映射类型。环境映射使得物体看起来具有反射效果*/
	EnvMapType m_EnvironmentMap;
	// 是否启用环境映射
	YkBool m_bEnvironmentMapEnabled;

	//! \brief 纹理V,U的缩放
	YkDouble m_dUScale, m_dVScale;
	


	//! \brief 纹理矩阵
	mutable YkMatrix4d m_TexModMatrix;
	//////////////////////////////////////////////////////////////////////////
};
}
#endif

