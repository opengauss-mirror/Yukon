/*
 *
 * YkModelElementPool.h
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

#if !defined(AFX_YKMODELELEMENTPOOL_H__388037F0_B0C7_4907_A4AF_690BEC569C58__INCLUDED_)
#define AFX_YKMODELELEMENTPOOL_H__388037F0_B0C7_4907_A4AF_690BEC569C58__INCLUDED_

#pragma once

#include "Base3D/YkModelNodeElements.h"
#include "Base3D/YkSharedPtr.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkTextureData.h"
#include "Base3D/YkMaterial3D.h"
#include "Toolkit/YkHashCode.h"

namespace Yk {
//! \brief 发现hash重复时使用的后缀；为提取原始名字
#define MODELENTITYHASHRETAG	_U("_[R.H]_") 

//! \brief 编辑模式
enum ElementEditModel
{
	EEM_Add		= 1,
	EEM_Update	= 2,
	EEM_Delete	= 3
};

template<class TElement, class TElementPtr>
class BASE3D_API YkModelElementMap : public std::map<YkString, TElementPtr>
{
public:
	YkModelElementMap()
	{

	}

	~YkModelElementMap()
	{
		ReleaseElements();
	}

	YkModelElementMap& operator = (YkModelElementMap& other)
	{
		typename std::map<YkString, TElementPtr>::iterator it;
		for (it=other.begin(); it != other.end(); it++)
		{
			if(!it->second.IsNull())
			{
				TElement* pElement = new TElement(*it->second.Get());
				(*this)[it->first] = TElementPtr(pElement);
			}
			else
			{
				(*this)[it->first] = TElementPtr();
			}		
		}
		return *this;
	}

	void ReleaseElements()
	{
		this->clear();
	}

	TElementPtr CreateElement(const YkString strName)
	{
		if(!GetElement(strName).IsNull())
		{
			YKASSERT(FALSE);
			return TElementPtr();
		}
		TElement* pElement = new TElement(strName);
		(*this)[pElement->m_strName] = TElementPtr(pElement);
		return (*this)[pElement->m_strName];
	}
	
	TElementPtr GetElement(const YkString strName, const YkBool bTagName=TRUE)
	{
		if(bTagName)
		{
			typename std::map<YkString, TElementPtr>::iterator it = \
				this->find(strName);
			if(it != this->end())
			{
				return it->second;
			}
		}
		else
		{
			typename std::map<YkString, TElementPtr>::iterator it = \
				this->begin();
			for (; it != this->end(); it++)
			{
				if(!it->second.IsNull() && 
					it->second->m_strName.CompareNoCase(strName) == 0)
				{
					return it->second;
				}
			}
		}
		return TElementPtr();
	}

	TElementPtr AddElement(TElement* pElement)
	{
		if(pElement == NULL)
		{
			YKASSERT(FALSE);
			return TElementPtr();
		}
		YkString strName = pElement->GetName();
		if(this->find(strName) != this->end())
		{
			YKASSERT(FALSE);
			return TElementPtr();
		}	
		(*this)[strName] = TElementPtr(pElement);
		return (*this)[strName];
	}

	//添加引用
	YkBool AddElement(TElementPtr pElement)
	{
		if(pElement.IsNull())
		{
			YKASSERT(FALSE);
			return FALSE;
		}
		YkString strName = pElement->GetName();
		(*this)[strName] = pElement;
		return TRUE;
	}

	//模型Load时使用
	TElementPtr SetElement(TElementPtr pElement)
	{
		if(pElement.IsNull())
		{
			YKASSERT(FALSE);
			return TElementPtr();
		}
		YkString strName = pElement->GetName();
		if(this->find(strName) == this->end())
		{
			YKASSERT(FALSE);
			return TElementPtr();
		}	
		(*this)[strName] = pElement;
		return (*this)[strName];
	}

	//! \brief 名字是否已经存在
	YkBool IsExisted(const YkString strName)
	{
		return this->find(strName) != this->end();
	}


	//! \brief 添加空对象
	void AddElementShell(const YkString strName)
	{
		if(!GetElement(strName).IsNull())
		{
			return;
		}
		(*this)[strName] = TElementPtr();
	}

	void LoadElements(YkModelElementMap& mapPack)
	{
		YkString strName;
		typename std::map<YkString, TElementPtr>::iterator it = this->begin();
		for (; it != this->end(); it++)
		{
			strName = it->first;
			TElementPtr pElementSrc = mapPack.GetElement(strName);
			if(it->second.IsNull() && !pElementSrc.IsNull())
			{
				(*this)[strName] = pElementSrc;
			}
		}
	}




	std::vector<YkString> GetElementNames()
	{
		std::vector<YkString> vecResult;
		typename std::map<YkString, TElementPtr>::iterator it;
		for (it=this->begin(); it != this->end(); it++)
		{
			vecResult.push_back(it->first);
		}
		return vecResult;
	}
};

typedef YkModelElementMap<YkModelSkeleton, YkModelSkeletonPtr> YkModelSkeletonMap;
typedef YkModelElementMap<YkModelMaterial, YkModelMaterialPtr> YkModelMaterialMap;
typedef YkModelElementMap<YkModelTexture, YkModelTexturePtr> YkModelTextureMap;


template<class TElement, class TElementPtr>
class BASE3D_API YkModelElementPool
{
public:
	YkModelElementPool(/*YkBool bHash64 = FALSE*/)
	{

	}

	~YkModelElementPool()
	{
		ReleaseElements();
	}

public:
	

	//! \brief 清空
	void Clear()
	{
		m_vecHash.clear();
		m_mapElementAtt.clear();
		m_mapBounds.clear();
		m_mapRename.clear();
		m_mapElement.clear();
		if(m_vecElementCreated.size() > 0 ||
			m_vecElementUpdated.size() > 0 ||
			m_vecElementDeleted.size() > 0)
		{
			YKASSERT(FALSE);
		}
		m_vecElementCreated.clear();
		m_vecElementUpdated.clear();
		m_vecElementDeleted.clear();		
	}


	//! \brief 从内存获取实体
	TElementPtr GetLoadedElement(const YkString strName)
	{
		typename std::map<YkString, TElementPtr>::iterator it = m_mapElement.find(strName);
		if(it == m_mapElement.end())
		{
			return TElementPtr();
		}
		return it->second;
	}

	//! \brief 从内存获取实体
	std::map<YkString, TElementPtr>& GetLoadedElement()
	{
		return m_mapElement;
	}


	//! \brief 清理内存：清理掉UseCount=1(仅自己使用)的对象	
	void ReleaseElements()
	{
#ifdef _DEBUG
		typename std::map<YkString, TElementPtr>::iterator it;
		for(it=m_mapElement.begin(); it != m_mapElement.end(); it++)
		{
			YKASSERT(it->second.UseCount() == 1);
		}
#endif
		m_mapElement.clear();
	}


	std::vector<YkString> GetAtt(YkString strName)
	{
		std::map<YkString, std::vector<YkString> >::iterator it =
			m_mapElementAtt.find(strName);
		if (it == m_mapElementAtt.end())
		{
			YKASSERT(FALSE);
			return std::vector<YkString>();
		}
		return it->second;
	}




private:
	//! \brief 池中已存在的实体Hash:包含未加载的和内存的
	std::vector<YkLong> m_vecHash;

	//! \brief 池中已存在的实体名及其属性的对应关系：包含未加载的和内存的
	//! \brief 对于骨架==>骨架名：材质名
	//! \brief 对于材质==>材质名：纹理名
	//! \brief 对于纹理，这个应该为空
	std::map<YkString, std::vector<YkString> > m_mapElementAtt;

	//! \brief 骨架的包围盒
	std::map<YkString, std::vector<YkVector3d> > m_mapBounds;
	
	//! \brief 内部重命名前后对照表
	std::map<YkString, YkString> m_mapRename;

	//! \brief 内存中的实体
	std::map<YkString, TElementPtr> m_mapElement;

	//! \brief 需要存储（即新Create出来的）实体的HashCodes
	std::vector<YkString> m_vecElementCreated;

	//! \brief 更新过的（修改的）实体名
	std::vector<YkString> m_vecElementUpdated;

	//! \brief 删除的实体名
	std::vector<YkString> m_vecElementDeleted;
};

typedef YkModelElementPool<YkModelSkeleton, YkModelSkeletonPtr> YkModelSkeletonPool;
typedef YkModelElementPool<YkModelMaterial, YkModelMaterialPtr> YkModelMaterialPool;
typedef YkModelElementPool<YkModelTexture, YkModelTexturePtr> YkModelTexturePool;

}

#endif
