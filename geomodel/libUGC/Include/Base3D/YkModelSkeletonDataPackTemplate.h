/*
 *
 * YkModelSkeletonDataPackTemplate.h
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

#if !defined(AFX_YKMODELSKELETONDATAPACKTEMPLATE_H__F370B7CC_E94B_4522_927C_30735D9B5AAD__INCLUDED_)
#define AFX_YKMODELSKELETONDATAPACKTEMPLATE_H__F370B7CC_E94B_4522_927C_30735D9B5AAD__INCLUDED_

#pragma once

#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkTextureData.h"
#include "Base3D/YkMaterial3D.h"

namespace Yk {
template<class TVertexDataPack>
class BASE3D_API YkModelSkeletonDataPackTemplate
{
public:
	YkModelSkeletonDataPackTemplate()
	{
		m_pVertexDataPack = NULL;
	}

	YkModelSkeletonDataPackTemplate(TVertexDataPack* pVertexPack, 
		YkArray<YkIndexPackage*> arrIndexPack)
	{
		Set(pVertexPack, arrIndexPack);
	}

	YkModelSkeletonDataPackTemplate(TVertexDataPack* pVertexPack, 
		YkIndexPackage* pIndexPack)
	{
		m_pVertexDataPack = pVertexPack;
		m_arrIndexPack.Add(pIndexPack);
	}
	
	~YkModelSkeletonDataPackTemplate()
	{
		Clear(TRUE);
	}
	
	YkModelSkeletonDataPackTemplate& operator = (const YkModelSkeletonDataPackTemplate& other)
	{
		if(m_pVertexDataPack != NULL)
		{
			delete m_pVertexDataPack;
			m_pVertexDataPack = NULL;
		}

		if(other.m_pVertexDataPack != NULL)
		{
			m_pVertexDataPack = new TVertexDataPack(*other.m_pVertexDataPack);
		}

		YkInt i = 0;
		for(i = 0; i < m_arrIndexPack.GetSize(); i++)
		{
			delete m_arrIndexPack[i];
		}
		m_arrIndexPack.RemoveAll();

		for(i = 0; i < other.m_arrIndexPack.GetSize(); i++)
		{
			YkIndexPackage* pIndexPackage = other.m_arrIndexPack[i];
			if(pIndexPackage == NULL)
			{
				continue;
			}

			m_arrIndexPack.Add(new YkIndexPackage(*pIndexPackage));
		}
		return *this;
	}
	
	void Set(TVertexDataPack* pVertexPack, YkArray<YkIndexPackage*> arrIndexPack)
	{
		//重复Set的情况，外部负责处理指针内存
		m_pVertexDataPack = pVertexPack;		
		m_arrIndexPack.RemoveAll();
		m_arrIndexPack.Append(arrIndexPack);
	}

	void SetDataPackRef(TVertexDataPack* pVertexPack, \
		YkArray<YkIndexPackage*> arrIndexPack)
	{
		SetVertexPackage(pVertexPack);
		SetIndexPackage(arrIndexPack);
	}

	void SetVertexPackage(TVertexDataPack* pVertexPack)
	{
		if (m_pVertexDataPack != NULL && m_pVertexDataPack != pVertexPack)
		{
			delete m_pVertexDataPack;
			m_pVertexDataPack = NULL;
		}
		m_pVertexDataPack = pVertexPack;
	}

	void SetIndexPackage(YkArray<YkIndexPackage*>& arrIndexPack)
	{
		for (YkInt i=0; i<m_arrIndexPack.GetSize(); i++)
		{
			YkBool bFound = FALSE;
			for (YkInt j=0; j<arrIndexPack.GetSize(); j++)
			{
				if(m_arrIndexPack[i] == arrIndexPack[j])
				{
					bFound = TRUE;
					break;
				}
			}
			if(!bFound)
			{
				delete m_arrIndexPack[i];
				m_arrIndexPack[i] = NULL;
			}
		}
		m_arrIndexPack.RemoveAll();
		m_arrIndexPack.Append(arrIndexPack);
	}

	void Clear(YkBool bRelease)
	{
		if(bRelease)
		{
			if(m_pVertexDataPack != NULL)
			{
				delete m_pVertexDataPack;			
			}
			for(YkInt i = 0; i < m_arrIndexPack.GetSize(); i++)
			{
				delete m_arrIndexPack[i];
				m_arrIndexPack[i] = NULL;
			}		
		}

		m_pVertexDataPack = NULL;
		m_arrIndexPack.RemoveAll();
	}

	//! \brief 写入
	void Save(YkStream& stream)
	{
		if(m_pVertexDataPack == NULL)
		{
			YKASSERT(FALSE);
			return;
		}

		m_pVertexDataPack->Save(stream);

		YkUint nCount =  m_arrIndexPack.GetSize();
		stream << nCount;
		for (YkInt i=0; i< m_arrIndexPack.GetSize(); i++)
		{
			m_arrIndexPack[i]->Save(stream);
		}
	}

	//! \brief 加载
	void Load(YkStream& stream)
	{
		m_pVertexDataPack = new TVertexDataPack();
		m_pVertexDataPack->Load(stream);

		YkUint nCount = 0;
		stream >> nCount;
		m_arrIndexPack.SetSize(nCount);
		for (YkInt i=0; i< m_arrIndexPack.GetSize(); i++)
		{
			YkIndexPackage* pIndexPackage = new YkIndexPackage();
			pIndexPackage->Load(stream);
			m_arrIndexPack[i] = pIndexPackage;
		}
	}

	//! \brief 判断存储的数据是否有效
	YkBool IsValid()
	{
		if (m_pVertexDataPack == NULL || m_pVertexDataPack->m_pVertices == NULL)
		{
			return FALSE;
		}
		YkInt nIndexSize = m_arrIndexPack.GetSize();
		if (nIndexSize < 1)
		{
			return FALSE;
		}
		for (YkInt i = 0; i < nIndexSize; i++)
		{
			if (m_arrIndexPack[i] == NULL || m_arrIndexPack[i]->m_pIndexes == NULL)
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	//! \brief 获取第i个顶点
	YkVector3d GetVertex(const YkInt i)
	{
		YkInt nDimension = m_pVertexDataPack->m_nVertexDimension;
		YkVector3d vec;
		vec.x = m_pVertexDataPack->m_pVertices[i*nDimension+0];
		vec.y = m_pVertexDataPack->m_pVertices[i*nDimension+1];
		vec.z = m_pVertexDataPack->m_pVertices[i*nDimension+2];
		return vec;
	}
	//! \brief 设置第i个顶点值
	void SetVertex(const YkInt i, const YkVector3d& vec)
	{
		YkInt nDimension = m_pVertexDataPack->m_nVertexDimension;
		m_pVertexDataPack->m_pVertices[i*nDimension+0] = vec.x;
		m_pVertexDataPack->m_pVertices[i*nDimension+1] = vec.y;
		m_pVertexDataPack->m_pVertices[i*nDimension+2] = vec.z;
	}

	//! \brief nCount:坐标对个数
	//! \brief pTextureCoods内存仍是骨架管理
	void GetTextureCoords(YkInt nTexUnit, YkFloat*& pTextureCoods, YkInt& nCount, YkInt& nDim)
	{
		if(nTexUnit < 0 || nTexUnit > SMSCN_MAX_TEXTURE_COORD_SETS)
		{
			pTextureCoods = NULL;
			nCount = -1;
			nDim = -1;
			YKASSERT(FALSE);
			return;
		}

		nDim = m_pVertexDataPack->m_nTexDimensions[nTexUnit];
		nCount = m_pVertexDataPack->m_TexCoordCount[nTexUnit];
		pTextureCoods = m_pVertexDataPack->m_pTexCoords[nTexUnit];
	}

	//! \brief nCount:坐标对个数
	//! \brief pTextureCoods 托管给骨架对象，外部不能释放
	void SetTextureCoords(YkInt nTexUnit, YkFloat* pTextureCoods, YkInt nCount, YkInt nDim)
	{
		if(nTexUnit < 0 || nTexUnit > SMSCN_MAX_TEXTURE_COORD_SETS ||
			pTextureCoods == NULL)
		{
			pTextureCoods = NULL;
			nCount = -1;
			nDim = -1;
			YKASSERT(FALSE);
			return;
		}
		m_pVertexDataPack->m_nTexDimensions[nTexUnit] = nDim;
		m_pVertexDataPack->m_TexCoordCount[nTexUnit] = nCount;
		if(m_pVertexDataPack->m_pTexCoords[nTexUnit] != NULL)
		{
			delete[] m_pVertexDataPack->m_pTexCoords[nTexUnit];
		}
		m_pVertexDataPack->m_pTexCoords[nTexUnit] = pTextureCoods;
	}

	//! \brief 获取骨架法向量
	void GetNormalCoords(YkFloat*& pNormal, YkInt& nCount)
	{
		nCount = m_pVertexDataPack->m_nNormalCount;
		pNormal = m_pVertexDataPack->m_pNormals;
	}

	//! \brief nCount:坐标对个数
	//! \brief pNormal 托管给骨架对象，外部不能释放
	void SetNormalCoords(YkFloat* pNormal, YkInt nCount)
	{
		if(m_pVertexDataPack->m_pNormals != NULL)
		{
			delete[] m_pVertexDataPack->m_pNormals;
		}
		m_pVertexDataPack->m_pNormals = pNormal;
		m_pVertexDataPack->m_nNormalCount = nCount;
	}

public:
	//! \brief 顶点信息
	TVertexDataPack* m_pVertexDataPack;
	//! \brief 索引信息
	YkArray<YkIndexPackage*> m_arrIndexPack;
};

}

#endif