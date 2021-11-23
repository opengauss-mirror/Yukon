/*
 *
 * YkMaterial3D.cpp
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

#include "Base3D/YkMaterial3D.h"
#include "Toolkit/YkMarkup.h"

#define Yk_MATERIAL_VERSION		1.2

namespace Yk {
//! \brief 构造函数。
YkMaterial3D::YkMaterial3D()
{
	m_strName = _U("");
	m_strGroupName = _U("");
	m_dVersion = Yk_MATERIAL_VERSION;
}

//! \brief 拷贝构造函数。
YkMaterial3D::YkMaterial3D(const YkMaterial3D& other)
{
	*this = other;
}

YkMaterial3D& YkMaterial3D::operator=(const YkMaterial3D& other)
{
	m_strName = other.m_strName;
	m_strGroupName = other.m_strGroupName;
	m_nType = other.m_nType;
	m_dVersion = other.m_dVersion;

	removeAllTechniques();
	YkInt i = 0;
	for(i = 0; i < other.mTechniques.size(); i++)
	{
		YkTechnique *pTech = new YkTechnique(*other.mTechniques.at(i));
		if(pTech != NULL)
		{
			mTechniques.push_back(pTech);
		}
	}
	return *this;
}

//! \brief 构造函数。
YkMaterial3D::~YkMaterial3D()
{
	removeAllTechniques();
}

//-----------------------------------------------------------------------
YkTechnique* YkMaterial3D::createTechnique(void)
{
	YkTechnique *t = new YkTechnique();
	mTechniques.push_back(t);
	return t;
}
//-----------------------------------------------------------------------
YkTechnique* YkMaterial3D::getTechnique(YkUshort index)
{
	return mTechniques[index];
}
//-----------------------------------------------------------------------	
YkUint YkMaterial3D::getNumTechniques(void) const
{
	return static_cast<YkUshort>(mTechniques.size());
}
//-----------------------------------------------------------------------
void YkMaterial3D::removeAllTechniques(void)
{
	Techniques::iterator i, iend;
	iend = mTechniques.end();
	for (i = mTechniques.begin(); i != iend; ++i)
	{
		delete (*i);
	}
	mTechniques.clear();
}

void YkMaterial3D::Save(YkStream& fStream)
{
	fStream<<m_dVersion;
	fStream<<m_strName;
	fStream<<m_strGroupName;
	fStream<<m_nType;

	YkInt i = 0;
	YkInt nSize = mTechniques.size();
	fStream<<nSize;		
	for(i = 0; i < nSize; i++)
	{
		YkTechnique *pTech = mTechniques.at(i);
		if(pTech != NULL)
		{
			pTech->Save(fStream, m_dVersion);
		}
	}
}

YkBool YkMaterial3D::Load(YkStream& fStream)
{
	fStream>>m_dVersion;
	fStream>>m_strName;

	//版本兼容
	if( !YKEQUAL(m_dVersion, 1.2) )
	{
		YkTechnique* pTech = createTechnique();
		YkPass* pPass = pTech->createPass();
		pPass->m_strName = m_strName;
		return TRUE;
	}

	fStream>>m_strGroupName;
	fStream>>(YkInt&)m_nType;

	YkInt i = 0;
	YkInt nSize = 0;
	fStream>>nSize;
	for(i = 0; i < nSize; i++)
	{
		YkTechnique *pTech = new(std::nothrow) YkTechnique;
		if(pTech == NULL)
		{
			return FALSE;
		}

		if(!pTech->Load(fStream, m_dVersion))
		{
			delete pTech;
			pTech = NULL;
			return FALSE;
		}
		mTechniques.push_back(pTech);
	}

	return TRUE;
}

}