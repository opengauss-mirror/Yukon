/*
 *
 * YkTextureUnitState.cpp
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

#include "Base3D/YkTextureUnitState.h"
#include "Toolkit/YkMarkup.h"

namespace Yk 
{
YkTextureUnitState::YkTextureUnitState()
	: m_unTextureCoordSetIndex(0)
	, m_dUScale(1)
	, m_dVScale(1)
	, m_TexModMatrix(YkMatrix4d::IDENTITY)
	, m_MinFilter(FO_LINEAR)
	, m_MaxFilter(FO_LINEAR)
	, m_MipFilter(FO_POINT)
	, m_bEnvironmentMapEnabled(false)
{
	m_AddressMode.u = TAM_WRAP;
	m_AddressMode.v = TAM_WRAP;
	m_AddressMode.w = TAM_WRAP;
}

YkTextureUnitState::YkTextureUnitState(const YkTextureUnitState& oth)
	: m_strName(oth.m_strName)
	, m_strTextureNameAlias(oth.m_strTextureNameAlias)
	, m_strTextureName(oth.m_strTextureName)
	, m_unTextureCoordSetIndex(oth.m_unTextureCoordSetIndex)
	, m_dUScale(oth.m_dUScale)
	, m_dVScale(oth.m_dVScale)
	, m_TexModMatrix(oth.m_TexModMatrix)
	, m_MinFilter(oth.m_MinFilter)
	, m_MaxFilter(oth.m_MaxFilter)
	, m_MipFilter(oth.m_MipFilter)
	, m_bEnvironmentMapEnabled(oth.m_bEnvironmentMapEnabled)
{
	m_AddressMode.u = oth.m_AddressMode.u;
	m_AddressMode.v = oth.m_AddressMode.v;
	m_AddressMode.w = oth.m_AddressMode.w;
}

YkTextureUnitState::~YkTextureUnitState()
{
}

void YkTextureUnitState::Save(YkStream& fStream)
{
	fStream<<m_strName;
	fStream<<m_strTextureNameAlias;
	fStream<<m_strTextureName;
	fStream<<m_strCubicTextureName;
	fStream<<m_unTextureCoordSetIndex;
	fStream<<m_AddressMode.u;
	fStream<<m_AddressMode.v;
	fStream<<m_AddressMode.w;
	fStream<<m_MinFilter;
	fStream<<m_MaxFilter;
	fStream<<m_MipFilter;
	fStream<<m_dUScale;
	fStream<<m_dVScale;
	fStream<<m_bEnvironmentMapEnabled;
	fStream<<m_EnvironmentMap;
	fStream.Save(m_TexModMatrix.GetPointer(), 16);
}

YkBool YkTextureUnitState::Load(YkStream& fStream)
{
	fStream>>m_strName;
	fStream>>m_strTextureNameAlias;
	fStream>>m_strTextureName;
	fStream>>m_strCubicTextureName;
	fStream>>m_unTextureCoordSetIndex;
	fStream>>(YkInt&)m_AddressMode.u;
	fStream>>(YkInt&)m_AddressMode.v;
	fStream>>(YkInt&)m_AddressMode.w;
	fStream>>(YkInt&)m_MinFilter;
	fStream>>(YkInt&)m_MaxFilter;
	fStream>>(YkInt&)m_MipFilter;
	fStream>>m_dUScale;
	fStream>>m_dVScale;
	fStream>>m_bEnvironmentMapEnabled;
	fStream>>(YkInt&)m_EnvironmentMap;
	fStream.Load(m_TexModMatrix.GetPointer(), 16);

	return TRUE;
}

YkInt YkTextureUnitState::GetDataSize() const
{
	YkInt nSize = 0;
	nSize += m_strName.GetLength() + sizeof(YkInt);
	nSize += m_strTextureNameAlias.GetLength() + sizeof(YkInt);
	nSize += m_strTextureName.GetLength() + sizeof(YkInt);
	nSize += m_strCubicTextureName.GetLength() + sizeof(YkInt);
	nSize += sizeof(m_unTextureCoordSetIndex);
	nSize += sizeof(YkInt) * 6;
	nSize += sizeof(m_dUScale);
	nSize += sizeof(m_dVScale);
	nSize += sizeof(m_bEnvironmentMapEnabled);
	nSize += sizeof(YkInt);
	nSize += sizeof(YkDouble) * 16;

	return nSize;
}
}
