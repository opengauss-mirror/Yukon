/*
 *
 * YkModelEM.cpp
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

#include "Base3D/YkModelEM.h"
#include "Toolkit/YkHashCode.h"
#include "Base3D/YkMathEngine.h"
#include "Toolkit/YkImageOperator.h"
#include "Toolkit/YkCompress.h"

namespace Yk {

YkModelEMapPack::YkModelEMapPack()
{
	m_pEM = NULL;
}

YkModelEMapPack::YkModelEMapPack(YkModelEM* pEM)
{
	m_pEM = pEM;
}

YkModelEMapPack::~YkModelEMapPack()
{
	Clear();
}

YkModelEMapPack& YkModelEMapPack::operator = (YkModelEMapPack& other)
{
	m_mapSkeleton = other.m_mapSkeleton;
	m_mapMaterial = other.m_mapMaterial;
	m_mapTexture = other.m_mapTexture;
	return *this;
}

YkModelSkeletonPtr YkModelEMapPack::CreateSkeleton(const YkString strName)
{
	return m_mapSkeleton.CreateElement(strName);
}

YkModelMaterialPtr YkModelEMapPack::CreateMaterial(const YkString strName)
{
	return m_mapMaterial.CreateElement(strName);
}

YkModelTexturePtr YkModelEMapPack::CreateTexture(const YkString strName)
{
	return m_mapTexture.CreateElement(strName);
}

YkBool YkModelEMapPack::AddSkeleton(YkModelSkeletonPtr pElement)
{
	return m_mapSkeleton.AddElement(pElement);
}

YkBool YkModelEMapPack::AddMaterial(YkModelMaterialPtr pElement)
{
	return m_mapMaterial.AddElement(pElement);
}

YkBool YkModelEMapPack::AddTexture(YkModelTexturePtr pElement)
{
	return m_mapTexture.AddElement(pElement);
}

YkModelSkeletonPtr YkModelEMapPack::AddSkeleton(YkModelSkeleton* pElement)
{
	return m_mapSkeleton.AddElement(pElement);
}

YkModelMaterialPtr YkModelEMapPack::AddMaterial(YkModelMaterial* pElement)
{
	return m_mapMaterial.AddElement(pElement);
}

YkModelTexturePtr YkModelEMapPack::AddTexture(YkModelTexture* pElement)
{
	return m_mapTexture.AddElement(pElement);
}

YkModelSkeletonPtr YkModelEMapPack::SetSkeleton(YkModelSkeletonPtr pElement)
{
	return m_mapSkeleton.SetElement(pElement);
}

YkModelMaterialPtr YkModelEMapPack::SetMaterial(YkModelMaterialPtr pElement)
{
	return m_mapMaterial.SetElement(pElement);
}

YkModelTexturePtr YkModelEMapPack::SetTexture(YkModelTexturePtr pElement)
{
	return m_mapTexture.SetElement(pElement);
}

YkModelSkeletonPtr YkModelEMapPack::GetSkeleton(const YkString strName,const YkBool bTagName)
{
	return m_mapSkeleton.GetElement(strName, bTagName);
}

YkModelMaterialPtr YkModelEMapPack::GetMaterial(const YkString strName,const YkBool bTagName)
{
	return m_mapMaterial.GetElement(strName, bTagName);
}
YkModelTexturePtr YkModelEMapPack::GetTexture(const YkString strName,const YkBool bTagName)
{
	return m_mapTexture.GetElement(strName, bTagName);
}

YkString YkModelEMapPack::GetUnoccupiedName(YkString strName)
{
	YkString strResult = strName;
	YkInt nRecur = 0;
	while(IsExisted(strResult))
	{
		strResult.Format(_U("%s_%d"), strName.Cstr(), nRecur++);
	}
	return strResult;
}

void YkModelEMapPack::GetAtt(const YkString strName, const YkModelElement::METype eType, \
			std::vector<YkString>& vecAtt)
{
	//直接从管理器获取
	if(m_pEM != NULL)
	{
		return m_pEM->GetAtt(strName, eType, vecAtt);
	}

	switch (eType)
	{
	case YkModelElement::etSkeleton:
		{
			YkModelSkeletonPtr pElement = GetSkeleton(strName);
			if(pElement.IsNull())
			{
				YKASSERT(FALSE);
			}
			vecAtt = pElement->GetAtt();
		}
		break;
	case YkModelElement::etMaterial:
		{
			YkModelMaterialPtr pElement = GetMaterial(strName);
			if(pElement.IsNull())
			{
				YKASSERT(FALSE);
			}
			vecAtt = pElement->GetAtt();
		}
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
}

void YkModelEMapPack::SynElement()
{
	//从骨架开始
	std::map<YkString, YkModelSkeletonPtr>::iterator it ;
	YkInt nConst = 100;
	std::map<YkString, YkInt> mapTag;
	for (it=m_mapSkeleton.begin(); \
		it != m_mapSkeleton.end(); it++)
	{
		mapTag[it->first] = nConst;
		std::vector<YkString> vecAtt = it->second->GetAtt();
		YKASSERT(vecAtt.size() == 1);		
		for (YkInt i = 0; i < vecAtt.size(); i++)
		{
			mapTag[vecAtt[i]] = nConst; 

			YkModelMaterialPtr pMaterial = GetMaterial(vecAtt[i]);
			if(!pMaterial.IsNull())
			{
				std::vector<YkString> vecAttMat = pMaterial->GetAtt();
				for (YkInt j=0; j<vecAttMat.size(); j++)
				{
					mapTag[vecAttMat[j]] = nConst; 
				}
			}
		}
	}

	//开始清理：骨架
	for (it=m_mapSkeleton.begin(); it != m_mapSkeleton.end();)
	{
		if(mapTag[it->first] != nConst)
		{
			m_mapSkeleton.erase(it++);
		}
		else
		{
			 ++it;
		}
	}
	//开始清理：材质
	std::map<YkString, YkModelMaterialPtr>::iterator itMat;
	for (itMat=m_mapMaterial.begin(); itMat != m_mapMaterial.end();)
	{
		if(mapTag[itMat->first] != nConst)
		{
			m_mapMaterial.erase(itMat ++);
		}
		else
		{
			++ itMat;
		}
	}
	//开始清理：纹理
	std::map<YkString, YkModelTexturePtr>::iterator itTex;
	for (itTex=m_mapTexture.begin(); itTex != m_mapTexture.end(); )
	{
		if(mapTag[itTex->first] != nConst)
		{
			m_mapTexture.erase(itTex ++);
		}
		else
		{
			++ itTex;
		}
	}
}

YkString YkModelEMapPack::RemoveSkeleton(const YkString strSkeleton)
{
	YkString strMaterial;
	std::map<YkString, YkModelSkeletonPtr>::iterator it =
		m_mapSkeleton.find(strSkeleton);
	if(it != m_mapSkeleton.end())
	{
		if(it->second.Get() != NULL)
		{
			strMaterial = it->second->m_strMaterialName;
		}
		m_mapSkeleton.erase(it);
	}
	return strMaterial;
}

void YkModelEMapPack::AddElementShell(const YkModelElement::METype eType, const YkString strName)
{
	switch (eType)
	{
	case YkModelElement::etSkeleton:
		{
			m_mapSkeleton.AddElementShell(strName);
		}
		break;
	case YkModelElement::etMaterial:
		{
			m_mapMaterial.AddElementShell(strName);
		}
		break;
	case YkModelElement::etTexture:
		{
			m_mapTexture.AddElementShell(strName);
		}
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
}

void YkModelEMapPack::LoadElements(YkModelEMapPack& mapPack)
{
	m_mapSkeleton.LoadElements(mapPack.m_mapSkeleton);
	m_mapMaterial.LoadElements(mapPack.m_mapMaterial);
	m_mapTexture.LoadElements(mapPack.m_mapTexture);
}

void YkModelEMapPack::Clear()
{
	m_mapSkeleton.ReleaseElements();
	m_mapMaterial.ReleaseElements();
	m_mapTexture.ReleaseElements();
}

YkBool YkModelEMapPack::IsExisted(const YkString strName)
{
	return (m_mapSkeleton.IsExisted(strName) || 
		m_mapMaterial.IsExisted(strName) ||
		m_mapTexture.IsExisted(strName));
}

std::vector<YkString> YkModelEMapPack::GetElementNames(YkModelElement::METype metype)
{
	std::vector<YkString> vecResult;
	switch (metype)
	{
	case YkModelElement::etSkeleton:
	{
		vecResult = m_mapSkeleton.GetElementNames();
	}
	break;
	case YkModelElement::etTexture:
	{
		vecResult = m_mapTexture.GetElementNames();
	}
	break;
	case YkModelElement::etMaterial:
	{
		vecResult = m_mapMaterial.GetElementNames();
	}
	break;
	case YkModelElement::etUnknown:
	{
		std::vector<YkString> vecTemp = GetElementNames(YkModelElement::etSkeleton);
		vecResult.insert(vecResult.end(), vecTemp.begin(), vecTemp.end());

		vecTemp = GetElementNames(YkModelElement::etTexture);
		vecResult.insert(vecResult.end(), vecTemp.begin(), vecTemp.end());

		vecTemp = GetElementNames(YkModelElement::etMaterial);
		vecResult.insert(vecResult.end(), vecTemp.begin(), vecTemp.end());
	}
	break;
	default:
		break;
	}
	return vecResult;
}

//////////////////////////////////////////////////////////////////////////

YkModelEM::YkModelEM(YkBool bHash64) : \
	m_SkeletonPool(), m_MaterialPool(), m_TexturePool()
{
	m_bInited = FALSE;
}

YkModelEM::~YkModelEM()
{

}

YkBool YkModelEM::HasInited()
{
	return m_bInited;
}


void YkModelEM::Clear()
{
	m_SkeletonPool.Clear();
	m_MaterialPool.Clear();
	m_TexturePool.Clear();

	m_bInited = FALSE;
}


YkModelSkeletonPtr YkModelEM::GetLoadedSkeleton(YkString strName)
{
	return m_SkeletonPool.GetLoadedElement(strName);
}

YkModelMaterialPtr YkModelEM::GetLoadedMaterial3D(YkString strName)
{
	return m_MaterialPool.GetLoadedElement(strName);
}

YkModelTexturePtr YkModelEM::GetLoadedTexture(YkString strName)
{
	return m_TexturePool.GetLoadedElement(strName);
}

std::map<YkString, YkModelSkeletonPtr>& YkModelEM::GetLoadedSkeletonRef()
{
	return m_SkeletonPool.GetLoadedElement();
}

std::map<YkString, YkModelMaterialPtr>& YkModelEM::GetLoadedMaterialRef()
{
	return m_MaterialPool.GetLoadedElement();
}

std::map<YkString, YkModelTexturePtr>& YkModelEM::GetLoadedTextureRef()
{
	return m_TexturePool.GetLoadedElement();
}



void YkModelEM::Release()
{
	m_SkeletonPool.ReleaseElements();
	m_MaterialPool.ReleaseElements();
	m_TexturePool.ReleaseElements();
}



void YkModelEM::GetAtt(const YkString strName, const YkModelElement::METype eType, \
			std::vector<YkString>& vecAtt)
{
	switch (eType)
	{
	case YkModelElement::etSkeleton:
		{
			vecAtt = m_SkeletonPool.GetAtt(strName);
		}
		break;
	case YkModelElement::etMaterial:
		{
			vecAtt = m_MaterialPool.GetAtt(strName);
		}
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
}





}