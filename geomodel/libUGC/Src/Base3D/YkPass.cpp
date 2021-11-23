/*
 *
 * YkPass.cpp
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

#include "Base3D/YkPass.h"
#include "Toolkit/YkMarkup.h"

namespace Yk
{
	YkPass::YkPass()
		: m_strName(_U(""))
		, m_enPolygonMode(PM_SOLID)
		, m_bPolygonModeOverrideable(TRUE)
		, m_CullMode(CULL_NONE)
		, m_fConstantPolygonOffset(0.0)
		, m_fSlopeScalePolygonOffset(0.0)
		, m_fDepthBiasPerIteration(0.0)
		, m_fAlphaReject(0.0)
		, m_AlphaRejectFunc(CMPF_ALWAYS_PASS)
		, m_bAlphaToCoverageEnabled(FALSE)
		, m_bReceiveShadow(FALSE)
		, m_MaterialColor(YkColorValue3D())
		, m_Ambient(YkColorValue3D(1.0, 1.0, 1.0, 1.0))
		, m_Diffuse(YkColorValue3D(1.0, 1.0, 1.0, 1.0))
		, m_Specular(YkColorValue3D(0.0, 0.0, 0.0, 0.0))
		, m_SelfIllumination(YkColorValue3D(0.0, 0.0, 0.0, 0.0))
		, m_Shininess(0)
		, m_bDepthCheck(TRUE)
		, m_bDepthWrite(TRUE)
		, m_DepthBufferFunc(CMPF_LESS_EQUAL)
		, m_bColorWrite(TRUE)
		//,m_fOpacity(1.0)
		, m_bBlendAlpha(TRUE)
		, m_dPointSize(1.0)
		, m_dPointMinSize(0.0)
		, m_dPointMaxSize(0.0)
		, m_bPointSpritesEnabled(FALSE)
		, m_bPointAttenuationEnabled(FALSE)
		, m_enPointSmoothHintMode(SHM_NONE)
		, m_enLineSmoothHintMode(SHM_NONE)
		, m_bMutiTexture(FALSE)
		, m_strVertexProgram(_U(""))
		, m_strFragmentProgram(_U(""))
		, m_strGeometryProgram(_U(""))
		, m_strShadowCasterVertexProgram(_U(""))
		, m_strShadowCasterFragmentProgram(_U(""))
		, m_strShadowReceiverVertexProgram(_U(""))
		, m_strShadowReceiverFragmentProgram(_U(""))
		, m_TransparentSorting(FALSE)
		, m_bTransparentSortingForced(FALSE)
		, m_bLightEnabled(TRUE)
		, m_nMaxSimultaneousLights(MAX_LIGHTS)
		, m_nStartLight(0)
		, m_bStencilCheck(FALSE)
	{
		for (YkInt i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			m_strTextures[i] = YkString(_U(""));
			m_TexAddressingMode[i] = YkTextureUnitState::TAM_CLAMP;
			m_nTextureIndex[i] = 0;
			m_nTextureZType[i] = 0;
			m_MinFilter[i] = FO_LINEAR;
			m_MagFilter[i] = FO_LINEAR;
			m_MipFilter[i] = FO_LINEAR;
		}

		m_PointAttenuationCoeffs[0] = 1.0;
		m_PointAttenuationCoeffs[1] = m_PointAttenuationCoeffs[2] = 0.0;
	}

	YkPass::YkPass(const YkPass& other)
		: m_strName(other.m_strName)
		, m_enPolygonMode(other.m_enPolygonMode)
		, m_bPolygonModeOverrideable(other.m_bPolygonModeOverrideable)
		, m_CullMode(other.m_CullMode)
		, m_fConstantPolygonOffset(other.m_fConstantPolygonOffset)
		, m_fSlopeScalePolygonOffset(other.m_fSlopeScalePolygonOffset)
		, m_fDepthBiasPerIteration(other.m_fDepthBiasPerIteration)
		, m_fAlphaReject(other.m_fAlphaReject)
		, m_AlphaRejectFunc(other.m_AlphaRejectFunc)
		, m_bAlphaToCoverageEnabled(other.m_bAlphaToCoverageEnabled)
		, m_bReceiveShadow(other.m_bReceiveShadow)
		, m_MaterialColor(other.m_MaterialColor)
		, m_Ambient(other.m_Ambient)
		, m_Diffuse(other.m_Diffuse)
		, m_Specular(other.m_Specular)
		, m_SelfIllumination(other.m_SelfIllumination)
		, m_Shininess(other.m_Shininess)
		, m_Tracking(other.m_Tracking)
		, m_bDepthCheck(other.m_bDepthCheck)
		, m_bDepthWrite(other.m_bDepthWrite)
		, m_DepthBufferFunc(other.m_DepthBufferFunc)
		, m_bColorWrite(other.m_bColorWrite)
		, m_bBlendAlpha(other.m_bBlendAlpha)
		, m_dPointSize(other.m_dPointSize)
		, m_dPointMinSize(other.m_dPointMinSize)
		, m_dPointMaxSize(other.m_dPointMaxSize)
		, m_bPointSpritesEnabled(other.m_bPointSpritesEnabled)
		, m_bPointAttenuationEnabled(other.m_bPointAttenuationEnabled)
		, m_enPointSmoothHintMode(other.m_enPointSmoothHintMode)
		, m_enLineSmoothHintMode(other.m_enLineSmoothHintMode)
		, m_bMutiTexture(other.m_bMutiTexture)
		, m_strVertexProgram(other.m_strVertexProgram)
		, m_strFragmentProgram(other.m_strFragmentProgram)
		, m_strGeometryProgram(other.m_strGeometryProgram)
		, m_strShadowCasterVertexProgram(other.m_strShadowCasterVertexProgram)
		, m_strShadowCasterFragmentProgram(other.m_strShadowCasterFragmentProgram)
		, m_strShadowReceiverVertexProgram(other.m_strShadowReceiverVertexProgram)
		, m_strShadowReceiverFragmentProgram(other.m_strShadowReceiverFragmentProgram)
		, m_TransparentSorting(other.m_TransparentSorting)
		, m_bTransparentSortingForced(other.m_bTransparentSortingForced)
		, m_bLightEnabled(other.m_bLightEnabled)
		, m_nMaxSimultaneousLights(other.m_nMaxSimultaneousLights)
		, m_nStartLight(other.m_nStartLight)
		, m_bStencilCheck(other.m_bStencilCheck)
	{
		memcpy(m_PointAttenuationCoeffs, other.m_PointAttenuationCoeffs, sizeof(YkDouble)*3);
		YkInt i = 0;
		for (i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			m_strTextures[i] = other.m_strTextures[i];
			m_matTexTransform[i] = other.m_matTexTransform[i];
			m_TexAddressingMode[i] = other.m_TexAddressingMode[i];
			m_nTextureIndex[i] = other.m_nTextureIndex[i];
			m_nTextureZType[i] = other.m_nTextureZType[i];
			m_MinFilter[i] = other.m_MinFilter[i];
			m_MagFilter[i] = other.m_MagFilter[i];
			m_MipFilter[i] = other.m_MipFilter[i];
		}

		TextureUnitStates::const_iterator iter, iend;

		iend = m_pTextureUnitStates.end();
		for (iter = m_pTextureUnitStates.begin(); iter != iend; ++iter)
		{
			delete *iter;
		}

		m_pTextureUnitStates.clear();

		// Copy texture units
		for(YkInt j =0; j < other.m_pTextureUnitStates.size(); j++)
		{
			YkTextureUnitState *pTextureUnitState = new YkTextureUnitState(*other.m_pTextureUnitStates.at(j));
			if(pTextureUnitState != NULL)
			{
				m_pTextureUnitStates.push_back(pTextureUnitState);
			}
		}

	}

	//-----------------------------------------------------------------------
	YkPass& YkPass::operator=(const YkPass& other)
	{
		m_strName = other.m_strName;
		m_enPolygonMode = other.m_enPolygonMode;
		m_bPolygonModeOverrideable = other.m_bPolygonModeOverrideable;
		m_CullMode = other.m_CullMode;
		m_fConstantPolygonOffset = other.m_fConstantPolygonOffset;
		m_fSlopeScalePolygonOffset = other.m_fSlopeScalePolygonOffset;
		m_fDepthBiasPerIteration = other.m_fDepthBiasPerIteration;
		m_fAlphaReject = other.m_fAlphaReject;
		m_AlphaRejectFunc = other.m_AlphaRejectFunc;
		m_bAlphaToCoverageEnabled = other.m_bAlphaToCoverageEnabled;
		m_bReceiveShadow = other.m_bReceiveShadow;
		m_MaterialColor = other.m_MaterialColor;
		m_Ambient = other.m_Ambient;
		m_Diffuse = other.m_Diffuse;
		m_Specular = other.m_Specular;
		m_SelfIllumination = other.m_SelfIllumination;
		m_Shininess = other.m_Shininess;
		m_Tracking = other.m_Tracking;
		m_bDepthCheck = other.m_bDepthCheck;
		m_bDepthWrite = other.m_bDepthWrite;
		m_DepthBufferFunc = other.m_DepthBufferFunc;
		m_bColorWrite = other.m_bColorWrite;
		m_bBlendAlpha = other.m_bBlendAlpha;
		m_dPointSize = other.m_dPointSize;
		m_dPointMinSize = other.m_dPointMinSize;
		m_dPointMaxSize = other.m_dPointMaxSize;
		m_bPointAttenuationEnabled = other.m_bPointAttenuationEnabled;
		memcpy(m_PointAttenuationCoeffs, other.m_PointAttenuationCoeffs, sizeof(YkDouble)*3);
		m_bPointSpritesEnabled = other.m_bPointSpritesEnabled;
		m_enPointSmoothHintMode = other.m_enPointSmoothHintMode;
		m_enLineSmoothHintMode = other.m_enLineSmoothHintMode;
		m_bMutiTexture = other.m_bMutiTexture;
		m_strVertexProgram = other.m_strVertexProgram;
		m_strFragmentProgram = other.m_strFragmentProgram;
		m_strGeometryProgram = other.m_strGeometryProgram;
		m_strShadowCasterVertexProgram = other.m_strShadowCasterVertexProgram;
		m_strShadowCasterFragmentProgram = other.m_strShadowCasterFragmentProgram;
		m_strShadowReceiverVertexProgram = other.m_strShadowReceiverVertexProgram;
		m_strShadowReceiverFragmentProgram = other.m_strShadowReceiverFragmentProgram;
		m_TransparentSorting = other.m_TransparentSorting;
		m_bTransparentSortingForced = other.m_bTransparentSortingForced;
		m_bLightEnabled = other.m_bLightEnabled;
		m_nMaxSimultaneousLights = other.m_nMaxSimultaneousLights;
		m_nStartLight = other.m_nStartLight;
		m_bStencilCheck = other.m_bStencilCheck;

		YkInt i;
		for (i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			m_strTextures[i] = other.m_strTextures[i];
			m_matTexTransform[i] = other.m_matTexTransform[i];
			m_TexAddressingMode[i] = other.m_TexAddressingMode[i];
			m_nTextureIndex[i] = other.m_nTextureIndex[i];
			m_nTextureZType[i] = other.m_nTextureZType[i];
			m_MinFilter[i] = other.m_MinFilter[i];
			m_MagFilter[i] = other.m_MagFilter[i];
			m_MipFilter[i] = other.m_MipFilter[i];
		}

		TextureUnitStates::const_iterator iter, iend;

		iend = m_pTextureUnitStates.end();
		for (iter = m_pTextureUnitStates.begin(); iter != iend; ++iter)
		{
			delete *iter;
		}

		m_pTextureUnitStates.clear();

		// Copy texture units
		for(YkInt j =0; j < other.m_pTextureUnitStates.size(); j++)
		{
			YkTextureUnitState *pTextureUnitState = new YkTextureUnitState(*other.m_pTextureUnitStates.at(j));
			if(pTextureUnitState != NULL)
			{
				m_pTextureUnitStates.push_back(pTextureUnitState);
			}
		}

		return *this;
	}

	YkPass::~YkPass()
	{
		TextureUnitStates::const_iterator i, iend;
		iend = m_pTextureUnitStates.end();
		for (i = m_pTextureUnitStates.begin(); i != iend; ++i)
		{
			delete *i;
		}
		m_pTextureUnitStates.clear();
	}
	
	//-----------------------------------------------------------------------
	YkTextureUnitState* YkPass::CreateTextureUnitState(void)
	{
		YkTextureUnitState *t = new YkTextureUnitState();
		m_pTextureUnitStates.push_back(t);
		// if texture unit state name is empty then give it a default name based on its index
		if (t->m_strName.IsEmpty())
		{
			// its the last entry in the container so its index is size - 1
			YkUint idx = m_pTextureUnitStates.size() - 1;
			YkString strName;
			strName.Format(_U("%d"),idx);
			t->m_strName = strName; 
		}
		return t;
	}

	YkTextureUnitState* YkPass::GetTextureUnitState(YkUshort index)
	{
		if ( index >= m_pTextureUnitStates.size())
		{
			return NULL;
		}
		return m_pTextureUnitStates[index];
	}

	void YkPass::removeAllTextureUnitState()
	{
		TextureUnitStates::iterator i, iend;
		iend = m_pTextureUnitStates.end();
		for (i = m_pTextureUnitStates.begin(); i != iend; ++i)
		{
			delete (*i);
		}
		m_pTextureUnitStates.clear();
	}

	YkUshort YkPass::GetTextureUnitStatesSize(void) const
	{
		return static_cast<YkUshort>(m_pTextureUnitStates.size());
	}
	
	void YkPass::Save(YkStream& fStream, YkDouble dVersion)
	{
		fStream<<m_strName;
		fStream<<m_enPolygonMode;
		fStream<<m_CullMode;
		fStream<<m_bLightEnabled;
		fStream<<m_nMaxSimultaneousLights;
		fStream<<m_nStartLight;
		fStream<<m_bPolygonModeOverrideable;
		fStream<<m_dPointSize;
		fStream<<m_dPointMinSize;
		fStream<<m_dPointMaxSize;
		fStream<<m_bPointSpritesEnabled;
		fStream<<m_bPointAttenuationEnabled;
		fStream.Save(m_PointAttenuationCoeffs, 3);	
		fStream<<m_enPointSmoothHintMode;
		fStream<<m_enLineSmoothHintMode;

		fStream<<m_Ambient.GetValue();
		fStream<<m_Diffuse.GetValue();
		fStream<<m_Specular.GetValue();
		fStream<<m_SelfIllumination.GetValue();
		fStream<<m_MaterialColor.GetValue();
		fStream<<m_Shininess;
		fStream<<m_Tracking;
		
		fStream<<m_bReceiveShadow;
		fStream<<m_bColorWrite;
		fStream<<m_fAlphaReject;
		fStream<<m_AlphaRejectFunc;
		fStream<<m_bAlphaToCoverageEnabled;
		fStream<<m_TransparentSorting;
		fStream<<m_bTransparentSortingForced;
		fStream<<m_bDepthCheck;
		fStream<<m_bDepthWrite;
		fStream<<m_DepthBufferFunc;
		fStream<<m_fConstantPolygonOffset;
		fStream<<m_fSlopeScalePolygonOffset;
		fStream<<m_fDepthBiasPerIteration;
		fStream<<m_bBlendAlpha;
		fStream<<m_strVertexProgram;
		fStream<<m_strFragmentProgram;
		fStream<<m_strGeometryProgram;
		fStream<<m_strShadowCasterVertexProgram;
		fStream<<m_strShadowReceiverVertexProgram;

		fStream<<m_strShadowReceiverFragmentProgram;
		
		YkInt i = 0;
		YkInt nSize = m_pTextureUnitStates.size();
		fStream<<nSize;
		for(i = 0; i < nSize; i++)
		{
			YkTextureUnitState *pTextureUnitState = m_pTextureUnitStates.at(i);
			if(pTextureUnitState != NULL)
			{
				pTextureUnitState->Save(fStream);
			}
		}

		if(YKEQUAL(dVersion, 1.2))
		{
			for(i = 0; i <  nSize; i++)
			{
				fStream<<m_nTextureZType[i];
			}
		}
	}

	YkBool YkPass::Load(YkStream& fStream, YkDouble dVersion)
	{
		fStream>>m_strName;
		fStream>>(YkInt&)m_enPolygonMode;
		fStream>>(YkInt&)m_CullMode;
		fStream>>m_bLightEnabled;
		fStream>>m_nMaxSimultaneousLights;
		fStream>>m_nStartLight;
		fStream>>m_bPolygonModeOverrideable;
		fStream>>m_dPointSize;
		fStream>>m_dPointMinSize;
		fStream>>m_dPointMaxSize;
		fStream>>m_bPointSpritesEnabled;
		fStream>>m_bPointAttenuationEnabled;
		fStream.Load(m_PointAttenuationCoeffs, 3);	
		fStream>>(YkInt&)m_enPointSmoothHintMode;
		fStream>>(YkInt&)m_enLineSmoothHintMode;

		YkColor nColor;
		fStream>>(YkUint&)nColor;
		m_Ambient.SetValue(nColor);
		fStream>>(YkUint&)nColor;
		m_Diffuse.SetValue(nColor);
		fStream>>(YkUint&)nColor;
		m_Specular.SetValue(nColor);
		fStream>>(YkUint&)nColor;
		m_SelfIllumination.SetValue(nColor);
		fStream>>(YkUint&)nColor;
		m_MaterialColor.SetValue(nColor);

		fStream>>m_Shininess;
		fStream>>m_Tracking;
		fStream>>m_bReceiveShadow;
		fStream>>m_bColorWrite;
		fStream>>m_fAlphaReject;
		fStream>>(YkInt&)m_AlphaRejectFunc;
		fStream>>m_bAlphaToCoverageEnabled;
		fStream>>m_TransparentSorting;
		fStream>>m_bTransparentSortingForced;
		fStream>>m_bDepthCheck;
		fStream>>m_bDepthWrite;
		fStream>>(YkInt&)m_DepthBufferFunc;
		fStream>>m_fConstantPolygonOffset;
		fStream>>m_fSlopeScalePolygonOffset;
		fStream>>m_fDepthBiasPerIteration;
		fStream>>m_bBlendAlpha;
		fStream>>m_strVertexProgram;
		fStream>>m_strFragmentProgram;
		fStream>>m_strGeometryProgram;
		fStream>>m_strShadowCasterVertexProgram;
		fStream>>m_strShadowReceiverVertexProgram;
		fStream>>m_strShadowReceiverFragmentProgram;
		if (m_strShadowReceiverFragmentProgram.CompareNoCase(_U(""))!=0)
		{

		}
		YkInt i = 0;
		YkInt nSize = 0;
		fStream>>nSize;
		for(i = 0; i < nSize; i++)
		{
			YkTextureUnitState *pTextureUnitState = new(std::nothrow) YkTextureUnitState;
			if(pTextureUnitState == NULL)
			{
				return FALSE;
			}

			if(!pTextureUnitState->Load(fStream))
			{
				delete pTextureUnitState;
				pTextureUnitState = NULL;
				return FALSE;
			}
			m_pTextureUnitStates.push_back(pTextureUnitState);
		}

		if(YKEQUAL(dVersion, 1.2))
		{
			for(i = 0; i <  nSize; i++)
			{
				fStream>>m_nTextureZType[i];
			}
		}

		return TRUE;
	}

	YkInt YkPass::GetDataSize() const
	{
		YkInt nSize = 0;
		nSize += m_strName.GetLength() + sizeof(YkInt);
		nSize += sizeof(YkInt);
		nSize += sizeof(YkInt);
		nSize += sizeof(m_bLightEnabled);
		nSize += sizeof(m_nMaxSimultaneousLights);
		nSize += sizeof(m_nStartLight);
		nSize += sizeof(m_bPolygonModeOverrideable);
		nSize += sizeof(m_dPointSize);
		nSize += sizeof(m_dPointMinSize);
		nSize += sizeof(m_dPointMaxSize);
		nSize += sizeof(m_bPointSpritesEnabled);
		nSize += sizeof(m_bPointAttenuationEnabled);
		nSize += sizeof(YkDouble) * 3;	
		nSize += sizeof(m_enPointSmoothHintMode);
		nSize += sizeof(m_enLineSmoothHintMode);
		nSize += sizeof(YkUint) * 5;
		nSize += sizeof(m_Shininess);
		nSize += sizeof(m_Tracking);
		nSize += sizeof(m_bReceiveShadow);
		nSize += sizeof(m_bColorWrite);
		nSize += sizeof(m_fAlphaReject);
		nSize += sizeof(YkInt);
		nSize += sizeof(m_bAlphaToCoverageEnabled);
		nSize += sizeof(m_TransparentSorting);
		nSize += sizeof(m_bTransparentSortingForced);
		nSize += sizeof(m_bDepthCheck);
		nSize += sizeof(m_bDepthWrite);
		nSize += sizeof(YkInt);
		nSize += sizeof(m_fConstantPolygonOffset);
		nSize += sizeof(m_fSlopeScalePolygonOffset);
		nSize += sizeof(m_fDepthBiasPerIteration);
		nSize += sizeof(m_bBlendAlpha);
		nSize += m_strVertexProgram.GetLength() + sizeof(YkInt);
		nSize += m_strFragmentProgram.GetLength() + sizeof(YkInt);
		nSize += m_strGeometryProgram.GetLength() + sizeof(YkInt);
		nSize += m_strShadowCasterVertexProgram.GetLength() + sizeof(YkInt);
		nSize += m_strShadowReceiverVertexProgram.GetLength() + sizeof(YkInt);
		nSize += m_strShadowReceiverFragmentProgram.GetLength() + sizeof(YkInt);
		nSize += sizeof(YkInt);

		YkInt i = 0;
		for(i = 0; i < m_pTextureUnitStates.size(); i++)
		{
			YkTextureUnitState *pTextureUnitState = m_pTextureUnitStates.at(i);
			if(pTextureUnitState != NULL)
			{
				nSize += pTextureUnitState->GetDataSize();
			}
		}

		nSize += sizeof(YkDouble) * m_pTextureUnitStates.size();
		return nSize;
	}

}