/*
 *
 * YkPass.h
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

#ifndef AFX_YKPASS_H__5ECC6D04_7C6B_4FAC_B644_C27843304715__INCLUDED_
#define AFX_YKPASS_H__5ECC6D04_7C6B_4FAC_B644_C27843304715__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Toolkit/YkColorValue3DEx.h"
#include "Base3D/YkTextureUnitState.h"
#include "Base3D/YkVector3d.h"
#include "Base3D/YkVector4d.h"
#include "Toolkit/YkMarkup.h"

namespace Yk {
enum YkMaterialParamType
{
	MAT_Unknown		  = 0,
	//! \brief 金属粗糙度模型
	MAT_PBRMetallicRough =	1,
	//! \brief 镜面光高光模型
	MAT_PBRSpecularGlossy = 2,
	//! \brief 普通材质模型
	MAT_Common			=3,
};

//! \brief 封装的用于保存RO的渲染状态的结构体
class BASE3D_API YkPass  
{	
public:
	YkPass();
	YkPass(const YkPass& other);
	YkPass & operator=(const YkPass& other);
	~YkPass();

	YkTextureUnitState* CreateTextureUnitState();

	//! \brief 从流文件中加载
	//! \param fStream 文件流[in]。
	YkBool Load(YkStream& fStream, YkDouble dVersion);

	//! \brief 存入流文件中
	//! \param fStream 文件流[in]。
	void Save(YkStream& fStream, YkDouble dVersion);

	//! \brief 获取数据存储时的大小
	//! \return 返回存储大小
	//! \remark 暂未考虑编码情况，与Save()一起维护
	//! \attention 字符串的长度要+4
	YkInt GetDataSize() const;

	//! \brief  名字
	YkString m_strName;

	//绘制模式，枚举值
	PolygonMode m_enPolygonMode;

	// 设置光照是否开启，默认开启 true
	YkBool m_bLightEnabled;

	// Max simultaneous lights
	YkUshort m_nMaxSimultaneousLights;

	/** Light index that this pass will start at in the light list.
	Normally the lights passed to a pass will start from the beginning
	of the light list for this object. This option allows you to make this
	pass start from a higher light index, for example if one of your earlier
	passes could deal with lights 0-3, and this pass dealt with lights 4+. 
	This option also has an interaction with pass iteration, in that
	if you choose to iterate this pass per light too, the iteration will
	only begin from light 4.*/
	YkUshort m_nStartLight;

	/** Sets whether this pass's chosen detail level can be
	overridden (downgraded) by the camera setting. 
	override true means that a lower camera detail will override this
	pass's detail level, false means it won't (default true).*/
	YkBool m_bPolygonModeOverrideable;

	//点尺寸
	YkFloat m_dPointSize;
	YkFloat m_dPointMinSize;
	YkFloat m_dPointMaxSize;

	/** Sets whether or not rendering points using OT_POINT_LIST will
	render point sprites (textured quads) or plain points (dots).
	@param enabled True enables point sprites, false returns to normal
	point rendering.*/
	YkBool m_bPointSpritesEnabled;

	YkBool m_bPointAttenuationEnabled;
	// constant, linear, quadratic coeffs
	YkDouble m_PointAttenuationCoeffs[3];
	//点的反走样模式
	SmoothHintMode m_enPointSmoothHintMode;
	//线的反走样模式
	SmoothHintMode m_enLineSmoothHintMode;
	
	//! \brief 贴图纹理数据 最多支持8组
	YkString m_strTextures[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 纹理变换矩阵
	YkMatrix4d m_matTexTransform[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 纹理的贴图模式
	YkTextureUnitState::TextureAddressingMode m_TexAddressingMode[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 标记纹理通道
	YkUint m_nTextureIndex[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 标记纹理Z通道
	YkInt m_nTextureZType[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief  是否多重纹理
	YkBool m_bMutiTexture;

	//! \brief 环境光
	YkColorValue3D m_Ambient;
	//! \brief 散射光
	YkColorValue3D m_Diffuse;
	//! \brief 反射光
	YkColorValue3D m_Specular;
	//! \brief 自发光
	YkColorValue3D m_SelfIllumination;
	//! \brief 发光，影响发射光点的大小
	YkFloat m_Shininess;
	//! \brief 顶点颜色跟踪
	YkUint m_Tracking;

	//材质颜色
	YkColorValue3D m_MaterialColor;
	//是否使用阴影
	YkBool m_bReceiveShadow;
	//颜色是否能够写入
	YkBool m_bColorWrite;
	//Alpha测试参考值
	YkFloat m_fAlphaReject;
	//Alpha测试方法
	CompareFunction m_AlphaRejectFunc;
	/** Sets whether to use alpha to coverage (A2C) when blending alpha rejected values. 
	Alpha to coverage performs multisampling on the edges of alpha-rejected
	textures to produce a smoother result. It is only supported when multisampling
	is already enabled on the render target, and when the hardware supports
	alpha to coverage (see RenderSystemCapabilities). */
	YkBool m_bAlphaToCoverageEnabled;
	//! \brief 透明物体深度排序
	YkBool m_TransparentSorting;
	/** Sets whether or not transparent sorting is forced.
	@param enabled
	If true depth sorting of this material will be depend only on the value of
	getTransparentSortingEnabled().
	@remarks
	By default even if transparent sorting is enabled, depth sorting will only be
	performed when the material is transparent and depth write/check are disabled.
	This function disables these extra conditions.*/
	YkBool m_bTransparentSortingForced;
	//是否进行深度测试
	YkBool m_bDepthCheck;
	//渲染时是否进行深度写入
	YkBool m_bDepthWrite;
	//深度测试方法
	CompareFunction m_DepthBufferFunc;
	//是否进行模板测试
	YkBool m_bStencilCheck;
	//! \brief多边形偏移量常量部分
	YkFloat m_fConstantPolygonOffset;
	//! \brief多边形偏移量深度坡度因子部分
	//! \brief最终偏移量 = maxSlope * m_fSlopeScalePolygonOffset + m_fConstantPolygonOffset
	YkFloat m_fSlopeScalePolygonOffset;
	/** Sets a factor which derives an additional depth bias from the number 
	of times a pass is iterated. The Final depth bias will be the constant depth bias as set through
	setDepthBias, plus this value times the iteration number. */
	YkFloat m_fDepthBiasPerIteration;
	//是否进行Alpha混合
	YkBool m_bBlendAlpha;
	//使用的顶点着色器的名字
	YkString m_strVertexProgram;
	//使用的片元着色器的名字
	YkString m_strFragmentProgram;
	//使用的几何着色器的名字
	YkString m_strGeometryProgram;
	//使用的阴影投射顶点着色器的名字
	YkString m_strShadowCasterVertexProgram;
	//使用的阴影投射片元着色器的名字
	YkString m_strShadowCasterFragmentProgram;
	//使用的阴影接收顶点着色器的名字
	YkString m_strShadowReceiverVertexProgram;
	//使用的阴影接收片元着色器的名字
	YkString m_strShadowReceiverFragmentProgram;
	
	//-----------------------------------------------------------
	// Texture Filtering
	FilterOptions m_MinFilter[SMSCN_MAX_TEXTURE_COORD_SETS];
	FilterOptions m_MagFilter[SMSCN_MAX_TEXTURE_COORD_SETS];
	FilterOptions m_MipFilter[SMSCN_MAX_TEXTURE_COORD_SETS];

protected:
	// 渲染引擎用的的裁剪模式
	CullingMode m_CullMode;

	/// Storage of texture unit states
	typedef std::vector<YkTextureUnitState*> TextureUnitStates;
	TextureUnitStates m_pTextureUnitStates;

public:
	//获取TextureUnitState
	YkTextureUnitState* GetTextureUnitState(YkUshort index);

	//获取TextureUnitState的数量
	YkUshort GetTextureUnitStatesSize(void) const;
	//移除所有的TextureUnitState
	void removeAllTextureUnitState(void);
};

}

#endif 


