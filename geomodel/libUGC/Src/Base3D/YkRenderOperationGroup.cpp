/*
 *
 * YkRenderOperationGroup.cpp
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

#include "Base3D/YkRenderOperationGroup.h"
#include "Toolkit/YkHashCode.h"
#include "Base3D/YkVertexDataPackageTemplate.h"

namespace Yk
{
	YkVertexDeclaration::YkVertexDeclaration() :
		m_nVertexDimension(3),
		m_bHasNormal(FALSE),
		m_bHasSecondColor(FALSE),
		m_bHasColor(FALSE)
	{
	}

	YkVertexDeclaration::YkVertexDeclaration(const YkVertexDeclaration& other)
	{
		m_nVertexDimension = other.m_nVertexDimension;
		m_arrTexDimensions = other.m_arrTexDimensions;
		m_bHasNormal = other.m_bHasNormal;
		m_bHasColor = other.m_bHasColor;
		m_bHasSecondColor = other.m_bHasSecondColor;
	}

	YkVertexDeclaration::YkVertexDeclaration(YkVertexDataPackage* pVertexDataPackage)
	{
		MakeWith(pVertexDataPackage);
	}

	void YkVertexDeclaration::MakeWith(YkVertexDataPackage* pVertexDataPackage)
	{
		if (pVertexDataPackage == NULL)
		{
			return;
		}
		m_nVertexDimension = pVertexDataPackage->m_nVertexDimension;
		m_bHasNormal = (pVertexDataPackage->m_pNormals != NULL && pVertexDataPackage->m_nNormalCount > 0);
		m_bHasColor = (pVertexDataPackage->m_pVertexColor != NULL && pVertexDataPackage->m_nVertexColorCount > 0);
		m_bHasSecondColor = (pVertexDataPackage->m_pSecondVertexColor != NULL && pVertexDataPackage->m_nSecondVertexColorCount > 0);
		
		for (YkInt i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			if (pVertexDataPackage->m_pTexCoords[i] == NULL)
			{
				break;
			}
			m_arrTexDimensions.Add(pVertexDataPackage->m_nTexDimensions[i]);
		}
	}

	YkBool YkVertexDeclaration::operator== (const YkVertexDeclaration& other) const
	{
		if (m_nVertexDimension != other.m_nVertexDimension || 
			m_bHasNormal != other.m_bHasNormal ||
			m_bHasColor != other.m_bHasColor ||
			m_bHasSecondColor != other.m_bHasSecondColor)
		{
			return FALSE;
		}
		
		if (m_arrTexDimensions.GetSize() != other.m_arrTexDimensions.GetSize())
		{
			YkInt nTexCoordSize = m_arrTexDimensions.GetSize();
			for (YkInt i = 0; i < nTexCoordSize; i++)
			{
				if (m_arrTexDimensions[i] != other.m_arrTexDimensions[i])
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	YkBool YkVertexDeclaration::operator!= (const YkVertexDeclaration& other) const
	{
		return !(*this == other);
	}
	//-----------------------------------------------------------------------
	YkIndexPackage::YkIndexPackage()
		: m_nIndexesCount(0)
		, m_pIndexes(NULL)
		, m_enIndexType(IT_16BIT)
		, m_bUseIndex(TRUE)
		, m_OperationType(OT_TRIANGLE_LIST)
	{
	}

	YkIndexPackage::YkIndexPackage(const YkIndexPackage& other)
		: m_nIndexesCount(0)
		, m_pIndexes(NULL)
		, m_enIndexType(IT_16BIT)
		, m_bUseIndex(TRUE)
		, m_OperationType(OT_TRIANGLE_LIST)
	{
		*this = other;
	}

	YkIndexPackage& YkIndexPackage::operator =(const YkIndexPackage& other)
	{
		m_enIndexType = other.m_enIndexType;
		m_bUseIndex = other.m_bUseIndex;
		m_OperationType = other.m_OperationType;

		if(other.m_pIndexes != NULL && other.m_nIndexesCount > 0)
		{
			SetIndexNum(other.m_nIndexesCount);
			YkInt nValue = 1;
			if (m_enIndexType ==  IT_32BIT || m_enIndexType ==  IT_32BIT_2)
			{
				nValue = 2;
			}
			memcpy(m_pIndexes, other.m_pIndexes, m_nIndexesCount * nValue * sizeof(YkUshort));
		}
		else
		{
			if(m_pIndexes != NULL)
			{
				delete m_pIndexes;
				m_pIndexes = NULL;
			}
			m_nIndexesCount = 0;
		}

		m_strPassName.RemoveAll();
		for(YkInt i = 0; i < other.m_strPassName.GetSize(); i++)
		{
			if(other.m_strPassName[i].IsEmpty())
			{
				break;
			}
			m_strPassName.Add(other.m_strPassName[i]);
		}

		return *this;
	}

	YkIndexPackage::~YkIndexPackage()
	{
		if (m_pIndexes != NULL)
		{
			delete [] m_pIndexes;
			m_pIndexes = NULL;
		}
		m_nIndexesCount = 0;
	}

	//-----------------------------------------------------------------------
	void YkIndexPackage::SetIndexNum(YkInt nIndexNum)
	{
		m_nIndexesCount = nIndexNum;
		if(m_pIndexes != NULL)
		{
			delete[] m_pIndexes;
			m_pIndexes =NULL;
		}
		if (m_enIndexType ==  IT_16BIT ||
			m_enIndexType ==  IT_16BIT_2)
		{
			m_pIndexes = new YkUshort[nIndexNum];
			memset(m_pIndexes, 0, sizeof(YkUshort)*nIndexNum);
		}
		else
		{
			m_pIndexes = (YkUshort*)new YkUint[nIndexNum];
			memset(m_pIndexes, 0, sizeof(YkUint)*nIndexNum);
		}
	}

	YkInt YkIndexPackage::GetDataSize()
	{
		YkInt nSize = sizeof(m_nIndexesCount);
		nSize += sizeof(m_enIndexType);
		nSize += sizeof(m_bUseIndex);
		nSize += sizeof(m_OperationType);

		if(m_nIndexesCount > 0 && m_pIndexes != NULL)
		{
			if(m_enIndexType == IT_32BIT || m_enIndexType == IT_32BIT_2)
			{
				nSize += sizeof(YkUint) * m_nIndexesCount;
			}
			else
			{
				nSize += sizeof(YkUshort) * m_nIndexesCount;
			}			
		}

		YkInt i = 0;
		nSize += sizeof(YkInt);
		for(i = 0; i < m_strPassName.GetSize(); i++)
		{
			YkString strName = m_strPassName[i];
			nSize += strName.GetLength() + sizeof(YkInt);
		}
		return nSize;
	}

	void YkIndexPackage::Save(YkStream& stream,YkBool bAlign)
	{
		if (bAlign == false)
		{
			stream<<m_nIndexesCount;
			stream<<m_enIndexType;
			stream<<m_bUseIndex;
			stream<<m_OperationType;

			if(m_nIndexesCount > 0 && m_pIndexes != NULL)
			{
				if(m_enIndexType == IT_32BIT || m_enIndexType == IT_32BIT_2)
				{
					stream.Save(m_pIndexes, m_nIndexesCount * 2);
				}
				else
				{
					stream.Save(m_pIndexes, m_nIndexesCount);
				}
			}

			YkInt i = 0;
			YkInt nCount = m_strPassName.GetSize();
			stream<<nCount;

			for(i = 0; i < nCount; i++)
			{
				YkString strName = m_strPassName[i];
				stream<<strName;
			}
		}
		else
		{
			YkUchar nByte = 0;

			stream<<m_nIndexesCount;

			stream<<(YkByte)m_enIndexType;
			stream<<m_bUseIndex;
			stream<<(YkByte)m_OperationType;
			stream<<nByte;

			if(m_nIndexesCount > 0 && m_pIndexes != NULL)
			{
				if(m_enIndexType == IT_32BIT || m_enIndexType == IT_32BIT_2)
				{
					stream.Save(m_pIndexes, m_nIndexesCount * 2);
				}
				else
				{
					stream.Save(m_pIndexes, m_nIndexesCount);
					if(m_nIndexesCount%2 != 0)
					{
						stream<<nByte;
						stream<<nByte;
					}
				}
			}

			YkInt i = 0;
			YkInt nCount = m_strPassName.GetSize();
			stream<<nCount;

			for(i = 0; i < nCount; i++)
			{
				YkString strName = m_strPassName[i];
				YkUint nMHashCode = YkHashCode::StringToHashCode(strName);
				stream<<nMHashCode;
			}
		}		
	}



	YkBool YkIndexPackage::Load(YkStream& stream,YkBool bAlign)
	{
		if(bAlign == false)
		{
			YkInt nCount = 0;
			stream>>nCount;
			stream>>(YkInt&)m_enIndexType;
			stream>>m_bUseIndex;
			stream>>(YkInt&)m_OperationType;

			if(nCount > 0)
			{
				m_nIndexesCount = nCount;
				if(m_enIndexType == IT_32BIT || m_enIndexType == IT_32BIT_2)
				{
					m_pIndexes = new(std::nothrow) YkUshort[m_nIndexesCount * 2];
					if(m_pIndexes == NULL)
					{
						return FALSE;
					}
					stream.Load(m_pIndexes, m_nIndexesCount * 2);
				}
				else
				{
					m_pIndexes = new(std::nothrow) YkUshort[m_nIndexesCount];
					if(m_pIndexes == NULL)
					{
						return FALSE;
					}
					stream.Load(m_pIndexes, m_nIndexesCount);
				}
			}

			YkInt i = 0;
			stream>>nCount;

			for(i = 0; i < nCount; i++)
			{
				YkString strName;
				stream>>strName;
				m_strPassName.Add(strName);
			}
		}
		else
		{
			YkUchar nByte = 0;

			YkInt nCount = 0;
			stream>>nCount;
			stream>>(YkByte&)m_enIndexType;
			stream>>m_bUseIndex;
			stream>>(YkByte&)m_OperationType;
			stream>>nByte;

			if(nCount > 0)
			{
				m_nIndexesCount = nCount;
				if(m_enIndexType == IT_32BIT || m_enIndexType == IT_32BIT_2)
				{
					m_pIndexes = new(std::nothrow) YkUshort[m_nIndexesCount * 2];
					if(m_pIndexes == NULL)
					{
						return FALSE;
					}
					stream.Load(m_pIndexes, m_nIndexesCount * 2);
				}
				else
				{
					m_pIndexes = new(std::nothrow) YkUshort[m_nIndexesCount];
					if(m_pIndexes == NULL)
					{
						return FALSE;
					}
					stream.Load(m_pIndexes, m_nIndexesCount);
					if(m_nIndexesCount%2 != 0)
					{
						stream>>nByte;
						stream>>nByte;
					}
				}
			}

			YkInt i = 0;
			stream>>nCount;

			for(i = 0; i < nCount; i++)
			{
				YkUint nMHashCode;
				stream>>nMHashCode;
				YkString strName;
				strName.Format(_U("%u"),nMHashCode);
				m_strPassName.Add(strName);
			}
		}
		

		return TRUE;
	}


	YkBool YkIndexPackage::HasAtt()
	{
		return m_enIndexType==IT_16BIT_2 || m_enIndexType==IT_32BIT_2;
	}

	
}
