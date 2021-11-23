/*
 *
 * YkVertexDataPackageTemplate.h
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

#ifndef AFX_YKVERTEXDATAPACKAGETEMPLATE_H__E9EEC12C_10E1_415E_B4F4_E99A84372E8F__INCLUDED_
#define AFX_YKVERTEXDATAPACKAGETEMPLATE_H__E9EEC12C_10E1_415E_B4F4_E99A84372E8F__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"

#if (__GNUC__==4)
 #include <typeinfo>
#endif

namespace Yk {

#define SIZE_PER_INSTANCE 17
#define SIZE_PER_DM_INSTANCE  29


enum VertexCompressOptions
{
	SVC_Vertex				=	1,				//顶点带压缩
	SVC_Normal				= 2,				//法线带压缩
	SVC_VertexColor		= 4,				//顶点颜色带压缩
	SVC_SecondColor		= 8,				//SecondColor带压缩
	SVC_TexutreCoord	= 16,				//纹理坐标带压缩
	SVC_TexutreCoordIsW = 32,  // 表示第一重纹理坐标存储顶点的W位
};

//! \brief 顶点数组
template<typename T, class VertexDeclaration>
class YkVertexDataPackageTemplate
{
	friend class YkVertexDeclaration;
public:
	//! \brief 数据绘制属性
	YkInt m_nVertexOptions;

	//! \brief 数据压缩标记
	YkInt m_nCompressOptions;

	//! \brief 顶点维数
	YkUshort m_nVertexDimension;
	//! \brief 法线维数
	YkUshort m_nNormalDimension;
	//! \brief 贴图坐标的维数
	YkUshort m_nTexDimensions[SMSCN_MAX_TEXTURE_COORD_SETS];

	//! \brief 顶点数组
	T* m_pVertices;
	//! \brief 顶点数目
	YkUint m_nVerticesCount;
	//! \brief 顶点数组偏移
	YkUshort m_nVertexStride;
	//! \brief 顶点各分量的最小值
	YkFloat m_minVerticesValue[4];
	//! \brief 顶点压缩归一化常量
	YkFloat m_fVertCompressConstant;

	//! \brief 向量数组
	YkFloat* m_pNormals;
	//! \brief 向量数目
	YkUint m_nNormalCount;
	//! \brief 向量数组偏移 
	YkUshort m_nNormalStride; 

	//! \brief 颜色数组
	YkUint *m_pVertexColor;
	//! \brief 颜色数组大小
	YkUint m_nVertexColorCount;	
	//! \brief 颜色数组偏移
	YkUshort m_nVertexColorStride;

	YkUint *m_pSecondVertexColor;
	YkUint m_nSecondVertexColorCount;	
	YkUshort m_nSecondVertexColorStride;

	//! \brief 顶点混合权重,目前当做BatchID使用，前端设置，并不存储到数据中
	YkFloat* m_pBlendWeights;
	YkUint m_nBlendWeightsCount;	
	YkUshort m_nBlendWeightsStride;

	//! \brief 顶点混合索引,目前当做BatchID使用，前端设置，并不存储到数据中
	YkFloat* m_pBlendIndices;
	YkUint m_nBlendIndicesCount;	
	YkUshort m_nBlendIndicesStride;

	//! \brief 贴图坐标数据
	YkFloat* m_pTexCoords[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 贴图坐标数目
	YkUint m_TexCoordCount[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 每组贴图坐标的Stride
	YkUshort m_TexCoordStride[SMSCN_MAX_TEXTURE_COORD_SETS];
	//! \brief 纹理坐标各分量的最小值
	YkFloat m_minTexCoordValue[SMSCN_MAX_TEXTURE_COORD_SETS][4];
	//! \brief 纹理坐标压缩归一化常量
	YkFloat m_texCoordCompressConstant[SMSCN_MAX_TEXTURE_COORD_SETS];


	YkVertexDataPackageTemplate()
	{
		Init();
	}

	YkVertexDataPackageTemplate(const YkVertexDataPackageTemplate& other)
	{
		Init();
		m_nCompressOptions = other.m_nCompressOptions;
		m_fVertCompressConstant = other.m_fVertCompressConstant;
		m_minVerticesValue[0] = other.m_minVerticesValue[0];
		m_minVerticesValue[1] = other.m_minVerticesValue[1];
		m_minVerticesValue[2] = other.m_minVerticesValue[2];
		m_minVerticesValue[3] = other.m_minVerticesValue[3];
		*this = other;
	}

	YkVertexDataPackageTemplate& operator = (const YkVertexDataPackageTemplate& other)
	{
		{
			if(other.m_pVertices != NULL && other.m_nVerticesCount > 0)
			{
				m_nVertexDimension = other.m_nVertexDimension;
				SetVertexNum(other.m_nVerticesCount, other.m_nVertexStride);
				if(m_nCompressOptions  & SVC_Vertex)
				{
					memcpy(m_pVertices, other.m_pVertices, m_nVerticesCount * m_nVertexDimension * sizeof(YkShort));
				}
				else
				{
					memcpy(m_pVertices, other.m_pVertices, m_nVerticesCount * m_nVertexDimension * sizeof(T));
				}
			}
			else
			{
				if (m_pVertices != NULL)
				{
					delete [] m_pVertices;
					m_pVertices = NULL;
				}
				m_nVerticesCount = 0;
			}

			if(other.m_pNormals != NULL && other.m_nNormalCount > 0)
			{
				m_nNormalDimension = other.m_nNormalDimension;
				SetNormalNum(other.m_nNormalCount, other.m_nNormalStride);
				memcpy(m_pNormals, other.m_pNormals, m_nNormalCount * m_nNormalDimension * sizeof(YkFloat));
			}
			else
			{
				if (m_pNormals != NULL)
				{
					delete [] m_pNormals;
					m_pNormals = NULL;
				}
				m_nNormalCount = 0;
			}

			if(other.m_pVertexColor != NULL && other.m_nVertexColorCount > 0)
			{
				SetColorNum(other.m_nVertexColorCount, other.m_nVertexColorStride);
				memcpy(m_pVertexColor, other.m_pVertexColor, m_nVertexColorCount * sizeof(YkUint));
			}
			else
			{
				if (m_pVertexColor != NULL)
				{
					delete [] m_pVertexColor;
					m_pVertexColor = NULL;
				}
				m_nVertexColorCount = 0;
			}

			if(other.m_pSecondVertexColor != NULL && other.m_nSecondVertexColorCount > 0)
			{
				SetSecondColorNum(other.m_nSecondVertexColorCount, other.m_nSecondVertexColorStride);
				memcpy(m_pSecondVertexColor, other.m_pSecondVertexColor, m_nSecondVertexColorCount * sizeof(YkUint));
			}
			else
			{
				if(m_pSecondVertexColor != NULL)
				{
					delete [] m_pSecondVertexColor;
					m_pSecondVertexColor = NULL;
				}
				m_nSecondVertexColorCount = 0;
			}

			if(other.m_pBlendWeights != NULL && other.m_nBlendWeightsCount > 0)
			{
				SetBlendWeightsNum(other.m_nBlendWeightsCount, other.m_nBlendWeightsStride);
				memcpy(m_pBlendWeights, other.m_pBlendWeights, m_nBlendWeightsCount * sizeof(YkFloat));
			}
			else
			{
				if(m_pBlendWeights != NULL)
				{
					delete [] m_pBlendWeights;
					m_pBlendWeights = NULL;
				}
				m_nBlendWeightsCount = 0;
			}

			if(other.m_pBlendIndices != NULL && other.m_nBlendIndicesCount > 0)
			{
				SetBlendIndicesNum(other.m_nBlendIndicesCount, other.m_nBlendIndicesStride);
				memcpy(m_pBlendIndices, other.m_pBlendIndices, m_nBlendIndicesCount * sizeof(YkFloat));
			}
			else
			{
				if(m_pBlendIndices != NULL)
				{
					delete [] m_pBlendIndices;
					m_pBlendIndices = NULL;
				}
				m_nBlendIndicesCount = 0;
			}

			for(YkInt i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
			{
				if(other.m_pTexCoords[i] == NULL || other.m_TexCoordCount[i] < 1)
				{
					if (m_pTexCoords[i] != NULL)
					{
						delete [] m_pTexCoords[i];
						m_pTexCoords[i] = NULL;
					}
					m_TexCoordCount[i] = 0;
					continue;
				}

				m_nTexDimensions[i] = other.m_nTexDimensions[i];
				SetTexCoordsNum(i, other.m_TexCoordCount[i], other.m_TexCoordStride[i]);
				memcpy(m_pTexCoords[i], other.m_pTexCoords[i], m_TexCoordCount[i] * m_nTexDimensions[i] * sizeof(YkFloat));
			}

			m_nVertexOptions = other.m_nVertexOptions;
			return *this;
		}
	}
		
	~YkVertexDataPackageTemplate()
	{
		Clear();
	}

	void Clear()
	{
		if (m_pVertices != NULL)
		{
			delete [] m_pVertices;
			m_pVertices = NULL;
		}
		m_nVerticesCount = 0;

		if (m_pNormals != NULL)
		{
			delete [] m_pNormals;
			m_pNormals = NULL;
		}
		m_nNormalCount = 0;

		if (m_pVertexColor != NULL)
		{
			delete [] m_pVertexColor;
			m_pVertexColor = NULL;
		}
		if(m_pSecondVertexColor != NULL)
		{
			delete [] m_pSecondVertexColor;
			m_pSecondVertexColor = NULL;
		}

		m_nVertexColorCount = 0;
		m_nSecondVertexColorCount = 0;

		if (m_pBlendIndices != NULL)
		{
			delete [] m_pBlendIndices;
			m_pBlendIndices = NULL;
		}
		m_nBlendIndicesCount = 0;
		if(m_pBlendWeights != NULL)
		{
			delete [] m_pBlendWeights;
			m_pBlendWeights = NULL;
		}
		m_nBlendWeightsCount = 0;

		YkUshort i = 0;
		for (i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			if (m_pTexCoords[i] != NULL)
			{
				delete [] m_pTexCoords[i];
				m_pTexCoords[i] = NULL;
			}
			m_TexCoordCount[i] = 0;
		}
	}


	void SetVertexNum(YkUint nVertexCount, YkUshort nVertexStride = 0)
	{
		m_nVerticesCount = nVertexCount;
		if(m_pVertices != NULL)
		{
			delete[] m_pVertices;
			m_pVertices =NULL;
		}
		m_pVertices = new T[nVertexCount * m_nVertexDimension];
		memset(m_pVertices, 0, sizeof(T) * nVertexCount * m_nVertexDimension);
		m_nVertexStride = nVertexStride;
	}
	void SetNormalNum(YkUint nNormalCount, YkUshort nNormalStride = 0)
	{
		m_nNormalCount = nNormalCount;
		if(m_pNormals != NULL)
		{
			delete[] m_pNormals;
			m_pNormals =NULL;
		}
		m_nNormalStride = nNormalStride;
		if (nNormalCount == 0)
		{
			return;
		}

		m_pNormals = new YkFloat[nNormalCount * m_nNormalDimension];
		memset(m_pNormals, 0, sizeof(YkFloat) * nNormalCount * m_nNormalDimension);
	}
	void SetColorNum(YkUint nColorCount, YkUshort nColorStride = 0)
	{
		m_nVertexColorCount = nColorCount;
		if(m_pVertexColor != NULL)
		{
			delete[] m_pVertexColor;
			m_pVertexColor =NULL;
		}
		m_pVertexColor = new YkUint[nColorCount];
		memset(m_pVertexColor, 0, sizeof(YkUint)*nColorCount);
		m_nVertexColorStride = nColorStride;
	}
	void SetSecondColorNum(YkUint nColorCount, YkUshort nColorStride = 0)
	{
		m_nSecondVertexColorCount = nColorCount;


		if(m_pSecondVertexColor != NULL)
		{
			delete[] m_pSecondVertexColor;
			m_pSecondVertexColor =NULL;
		}
		m_pSecondVertexColor = new YkUint[nColorCount];
		memset(m_pSecondVertexColor, 0, sizeof(YkUint)*nColorCount);

		m_nSecondVertexColorStride = nColorStride;
	}

	void SetBlendWeightsNum(YkUint nCount, YkUshort nStride = 0)
	{
		m_nBlendWeightsCount = nCount;
		if(m_pBlendWeights != NULL)
		{
			delete[] m_pBlendWeights;
			m_pBlendWeights = NULL;
		}
		m_pBlendWeights = new YkFloat[nCount];
		memset(m_pBlendWeights, 0, sizeof(YkFloat) * nCount);

		m_nBlendWeightsStride = nStride;
	}

	void SetBlendIndicesNum(YkUint nCount, YkUshort nStride = 0)
	{
		m_nBlendIndicesCount = nCount;
		if(m_pBlendIndices != NULL)
		{
			delete[] m_pBlendIndices;
			m_pBlendIndices = NULL;
		}
		m_pBlendIndices = new YkFloat[nCount];
		memset(m_pBlendIndices, 0, sizeof(YkFloat) * nCount);

		m_nBlendIndicesStride = nStride;
	}

	YkBool SetTexCoordsNum(YkUshort nTexUnit, YkUint nTexCoordCount, YkUshort nTexCoordStride = 0)
	{
		if (nTexUnit >= SMSCN_MAX_TEXTURE_COORD_SETS)
		{
			return FALSE;
		}
		m_TexCoordCount[nTexUnit] = nTexCoordCount;

		if(m_pTexCoords[nTexUnit] != NULL)
		{
			delete[] m_pTexCoords[nTexUnit];
			m_pTexCoords[nTexUnit] =NULL;
		}
		m_pTexCoords[nTexUnit] = new YkFloat[nTexCoordCount * m_nTexDimensions[nTexUnit]];
		memset(m_pTexCoords[nTexUnit], 0, sizeof(YkFloat)*nTexCoordCount * m_nTexDimensions[nTexUnit]);
		m_TexCoordStride[nTexUnit] = nTexCoordStride;
		return TRUE;
	}


	void Save(YkStream& stream,YkBool bAlign = false)
	{
		YkUshort nDimension = m_nVertexDimension;
		if(m_nVertexDimension != m_nNormalDimension)
		{
			nDimension = m_nVertexDimension | (m_nNormalDimension << 8);
		}

		if(bAlign == false)
		{
			stream<<m_nVertexOptions;
			stream<<nDimension;

			stream<<m_nVerticesCount;
			// 顶点
			if(m_nVerticesCount > 0 && m_pVertices != NULL)
			{
				stream<<m_nVertexStride;
				stream.Save(m_pVertices, m_nVerticesCount * m_nVertexDimension);
			}

			stream<<m_nNormalCount;
			//法向量
			if (m_nNormalCount > 0 && m_pNormals != NULL) 
			{
				stream<<m_nNormalStride;
				stream.Save(m_pNormals, m_nNormalCount * m_nNormalDimension);
			}

			stream<<m_nVertexColorCount;
			//顶点颜色
			if (m_nVertexColorCount > 0 && m_pVertexColor != NULL) 
			{
				stream<<m_nVertexColorStride;
				stream.Save(m_pVertexColor, m_nVertexColorCount);
			}


			YkInt nTexCount = 0, i = 0;
			for(i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
			{
				if(m_TexCoordCount[i] == 0)
				{
					break;
				}
				nTexCount++;
			}

			stream<<nTexCount;
			for(i = 0; i < nTexCount; i++)
			{
				stream<<m_nTexDimensions[i];
				stream<<m_TexCoordCount[i];
				stream<<m_TexCoordStride[i];
				stream.Save(m_pTexCoords[i], m_TexCoordCount[i] * m_nTexDimensions[i]);
			}
		}
		else
		{
			YkUchar nByte = 0;
			stream<<m_nVertexOptions;

			stream<<nDimension;
			stream<<nByte;
			stream<<nByte;

			stream<<m_nVerticesCount;
			// 顶点
			if(m_nVerticesCount > 0 && m_pVertices != NULL)
			{
				stream<<m_nVertexStride;
				stream<<nByte;
				stream<<nByte;

				stream.Save(m_pVertices, m_nVerticesCount * m_nVertexDimension);
			}

			stream<<m_nNormalCount;
			//法向量
			if (m_nNormalCount > 0 && m_pNormals != NULL) 
			{
				stream<<m_nNormalStride;
				stream<<nByte;
				stream<<nByte;

				stream.Save(m_pNormals, m_nNormalCount * m_nNormalDimension);
			}

			stream<<m_nVertexColorCount;
			//顶点颜色
			if (m_nVertexColorCount > 0 && m_pVertexColor != NULL) 
			{
				stream<<m_nVertexColorStride;
				stream<<nByte;
				stream<<nByte;

				for(int j=0;j<m_nVertexColorCount;j++)
				{
					YkUint nColor = m_pVertexColor[j];
					YkFloat fR = YKREDVAL(nColor)/255.0;
					YkFloat fG = YKGREENVAL(nColor)/255.0;
					YkFloat fB = YKBLUEVAL(nColor)/255.0;
					YkFloat fA = YKALPHAVAL(nColor)/255.0;

					stream<<fR;
					stream<<fG;
					stream<<fB;
					stream<<fA;
				}
			}

			stream<<m_nSecondVertexColorCount;
			//顶点颜色
			if (m_nSecondVertexColorCount > 0 && m_pSecondVertexColor != NULL) 
			{
				for(YkInt j=0;j<m_nSecondVertexColorCount;j++)
				{
					YkUint nColor = m_pSecondVertexColor[j];
					YkFloat fR = YKREDVAL(nColor)/255.0;
					YkFloat fG = YKGREENVAL(nColor)/255.0;
					YkFloat fB = YKBLUEVAL(nColor)/255.0;
					YkFloat fA = YKALPHAVAL(nColor)/255.0;

					stream<<fR;
					stream<<fG;
					stream<<fB;
					stream<<fA;				
				}
			}


			YkInt nTexCount = 0, i = 0;
			for(i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
			{
				if(m_TexCoordCount[i] == 0)
				{
					break;
				}
				nTexCount++;
			}

			stream<<nTexCount;
			for(i = 0; i < nTexCount; i++)
			{
				if(m_nTexDimensions[i] == SIZE_PER_INSTANCE)
				{
					YkUshort nDimension = SIZE_PER_INSTANCE + 3;
					stream<<m_TexCoordCount[i];
					stream<<nDimension;
					stream<<m_TexCoordStride[i];

					YkFloat* pValue = m_pTexCoords[i];
					YkUint* pUint = (YkUint*)pValue;

					for (YkInt kIndex =0;kIndex<m_TexCoordCount[i];kIndex++)
					{
						stream.Save(pValue+kIndex*SIZE_PER_INSTANCE,16);

						YkUint nSelectionID = pUint[SIZE_PER_INSTANCE * kIndex + 16];
						YkFloat fR = YKREDVAL(nSelectionID)/255.0;
						YkFloat fG = YKGREENVAL(nSelectionID)/255.0;
						YkFloat fB = YKBLUEVAL(nSelectionID)/255.0;
						YkFloat fA = YKALPHAVAL(nSelectionID)/255.0;
						stream<<fR;
						stream<<fG;
						stream<<fB;
						stream<<fA;				
					}

				}
				else if (m_nTexDimensions[i] == SIZE_PER_DM_INSTANCE)
				{
					YkUshort nDimension = SIZE_PER_DM_INSTANCE + 6;
					stream<<m_TexCoordCount[i];
					stream<<nDimension;
					stream<<m_TexCoordStride[i];

					YkFloat* pValue = m_pTexCoords[i];
					YkUint* pUint = (YkUint*)pValue;

					for (YkInt kIndex =0;kIndex<m_TexCoordCount[i];kIndex++)
					{
						stream.Save(pValue+kIndex*SIZE_PER_DM_INSTANCE,27);

						YkUint nSelectionID = pUint[SIZE_PER_DM_INSTANCE * kIndex + 27];
						YkFloat fR = YKREDVAL(nSelectionID)/255.0;
						YkFloat fG = YKGREENVAL(nSelectionID)/255.0;
						YkFloat fB = YKBLUEVAL(nSelectionID)/255.0;
						YkFloat fA = YKALPHAVAL(nSelectionID)/255.0;
						stream<<fR;
						stream<<fG;
						stream<<fB;
						stream<<fA;	

						nSelectionID = pUint[SIZE_PER_DM_INSTANCE * kIndex + 28];
						fR = YKREDVAL(nSelectionID)/255.0;
						fG = YKGREENVAL(nSelectionID)/255.0;
						fB = YKBLUEVAL(nSelectionID)/255.0;
						fA = YKALPHAVAL(nSelectionID)/255.0;
						stream<<fR;
						stream<<fG;
						stream<<fB;
						stream<<fA;	

					}
				}
				else 
				{
					stream<<m_TexCoordCount[i];
					stream<<m_nTexDimensions[i];
					stream<<m_TexCoordStride[i];
					stream.Save(m_pTexCoords[i], m_TexCoordCount[i] * m_nTexDimensions[i]);
				}
			}
		}
	}

	YkBool Load(YkStream& stream,YkBool bAlign = false)
	{
		if(bAlign == false)
		{
			stream>>m_nVertexOptions;
			stream>>m_nVertexDimension;

			if(m_nVertexDimension > 4)
			{
				m_nNormalDimension = (m_nVertexDimension >> 8);
				m_nVertexDimension = 0x0F & m_nVertexDimension;
			}
			else
			{
				m_nNormalDimension = m_nVertexDimension;
			}

			YkUint nVerticesCount = 0;
			stream>>nVerticesCount;
			// 顶点
			if(nVerticesCount > 0)
			{
				m_nVerticesCount = nVerticesCount;
				stream>>m_nVertexStride;
				m_pVertices = new(std::nothrow) T[m_nVerticesCount * m_nVertexDimension];
				if(m_pVertices == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pVertices, m_nVerticesCount * m_nVertexDimension);
			}

			YkUint nNormalCount = 0;
			stream>>nNormalCount;
			//法向量
			if (nNormalCount > 0) 
			{
				m_nNormalCount = nNormalCount;
				stream>>m_nNormalStride;
				m_pNormals = new(std::nothrow) YkFloat[m_nNormalCount * m_nNormalDimension];
				if(m_pNormals == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pNormals,m_nNormalCount * m_nNormalDimension);
			}

			YkUint nColorCount = 0;
			stream>>nColorCount;
			//顶点颜色
			if (nColorCount > 0) 
			{
				m_nVertexColorCount = nColorCount;
				stream>>m_nVertexColorStride;
				m_pVertexColor = new(std::nothrow) YkUint[m_nVertexColorCount];
				if(m_pVertexColor == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pVertexColor, m_nVertexColorCount);
			}

			YkInt nTexCount = 0, i = 0;
			stream>>nTexCount;
			for(i = 0; i < nTexCount; i++)
			{
				stream>>m_nTexDimensions[i];
				stream>>m_TexCoordCount[i];
				stream>>m_TexCoordStride[i];
				m_pTexCoords[i] = new(std::nothrow) YkFloat[m_TexCoordCount[i] * m_nTexDimensions[i]];
				if(m_pTexCoords[i] == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pTexCoords[i], m_TexCoordCount[i] * m_nTexDimensions[i]);
			}
		}
		else
		{
			YkUchar nByte = 0;
			stream>>m_nVertexOptions;

			stream>>m_nVertexDimension;
			stream>>nByte;
			stream>>nByte;

			if(m_nVertexDimension > 4)
			{
				m_nNormalDimension = (m_nVertexDimension >> 8);
				m_nVertexDimension = 0x0F & m_nVertexDimension;
			}
			else
			{
				m_nNormalDimension = m_nVertexDimension;
			}

			YkUint nVerticesCount = 0;
			stream>>nVerticesCount;
			// 顶点
			if(nVerticesCount > 0)
			{
				m_nVerticesCount = nVerticesCount;
				stream>>m_nVertexStride;
				stream>>nByte;
				stream>>nByte;

				m_pVertices = new(std::nothrow) T[m_nVerticesCount * m_nVertexDimension];
				if(m_pVertices == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pVertices, m_nVerticesCount * m_nVertexDimension);
			}

			YkUint nNormalCount = 0;
			stream>>nNormalCount;
			//法向量
			if (nNormalCount > 0) 
			{
				m_nNormalCount = nNormalCount;
				stream>>m_nNormalStride;
				stream>>nByte;
				stream>>nByte;

				m_pNormals = new(std::nothrow) YkFloat[m_nNormalCount * m_nNormalDimension];
				if(m_pNormals == NULL)
				{
					return FALSE;
				}
				stream.Load(m_pNormals,m_nNormalCount * m_nNormalDimension);
			}

			YkUint nColorCount = 0;
			stream>>nColorCount;
			//顶点颜色
			if (nColorCount > 0) 
			{
				m_nVertexColorCount = nColorCount;
				stream>>m_nVertexColorStride;
				stream>>nByte;
				stream>>nByte;

				m_pVertexColor = new(std::nothrow) YkUint[m_nVertexColorCount];
				if(m_pVertexColor == NULL)
				{
					return FALSE;
				}

				for(int j=0;j<m_nVertexColorCount;j++)
				{
					YkFloat fR,fG,fB,fA;

					stream>>fR;
					stream>>fG;
					stream>>fB;
					stream>>fA;

					m_pVertexColor[j] = YKRGBA(fR*255,fG*255,fB*255,fA*255);
				}
			}

			YkUint nSecondVertexColorCount = 0;
			stream>>nSecondVertexColorCount;
			//顶点颜色
			if (nSecondVertexColorCount > 0) 
			{
				m_nSecondVertexColorCount = nSecondVertexColorCount;

				m_pSecondVertexColor = new(std::nothrow) YkUint[m_nSecondVertexColorCount];
				if(m_pSecondVertexColor == NULL)
				{
					return FALSE;
				}

				for(int j=0;j<m_nSecondVertexColorCount;j++)
				{
					YkFloat fR,fG,fB,fA;

					stream>>fR;
					stream>>fG;
					stream>>fB;
					stream>>fA;

					m_pSecondVertexColor[j] = YKRGBA(fR*255,fG*255,fB*255,fA*255);
				}
			}

			YkInt nTexCount = 0, i = 0;
			stream>>nTexCount;
			for(i = 0; i < nTexCount; i++)
			{
				YkUshort nDimension;

				stream>>m_TexCoordCount[i];
				stream>>nDimension;
				stream>>m_TexCoordStride[i];

				if (nDimension == SIZE_PER_INSTANCE + 3)
				{
					m_nTexDimensions[i] = SIZE_PER_INSTANCE;

					m_pTexCoords[i] = new(std::nothrow) YkFloat[m_TexCoordCount[i] * m_nTexDimensions[i]];
					if(m_pTexCoords[i] == NULL)
					{
						return FALSE;
					}
					
					YkFloat* pValue = m_pTexCoords[i];
					YkUint* pUint = (YkUint*)pValue;
					YkFloat fR,fG,fB,fA;
					for (YkInt kIndex =0;kIndex<m_TexCoordCount[i];kIndex++)
					{
						stream.Load(pValue+kIndex*SIZE_PER_INSTANCE,16);

						stream>>fR;
						stream>>fG;
						stream>>fB;
						stream>>fA;		

						YkUint nSelectionID = YKRGBA(fR*255,fG*255,fB*255,fA*255);		
						pUint[SIZE_PER_INSTANCE * kIndex + 16] = nSelectionID;
					}
				}
				else if (nDimension == SIZE_PER_DM_INSTANCE + 6)
				{
					m_nTexDimensions[i] = SIZE_PER_DM_INSTANCE;

					m_pTexCoords[i] = new(std::nothrow) YkFloat[m_TexCoordCount[i] * m_nTexDimensions[i]];
					if(m_pTexCoords[i] == NULL)
					{
						return FALSE;
					}

					YkFloat* pValue = m_pTexCoords[i];
					YkUint* pUint = (YkUint*)pValue;
					YkFloat fR,fG,fB,fA;

					if(m_pTexCoords[i] == NULL)
					{
						return FALSE;
					}
					for (YkInt kIndex =0;kIndex<m_TexCoordCount[i];kIndex++)
					{
						stream.Load(pValue+kIndex*SIZE_PER_DM_INSTANCE,27);

						stream>>fR;
						stream>>fG;
						stream>>fB;
						stream>>fA;		

						YkUint nVertexColor = YKRGBA(fR*255,fG*255,fB*255,fA*255);		
						pUint[SIZE_PER_DM_INSTANCE * kIndex + 27] = nVertexColor;

						stream>>fR;
						stream>>fG;
						stream>>fB;
						stream>>fA;		

						YkUint nSelectionID = YKRGBA(fR*255,fG*255,fB*255,fA*255);		
						pUint[SIZE_PER_DM_INSTANCE * kIndex + 28] = nSelectionID;
					}
				}
				else
				{
					m_nTexDimensions[i] = nDimension;
					m_pTexCoords[i] = new(std::nothrow) YkFloat[m_TexCoordCount[i] * m_nTexDimensions[i]];
					if(m_pTexCoords[i] == NULL)
					{
						return FALSE;
					}
					stream.Load(m_pTexCoords[i], m_TexCoordCount[i] * m_nTexDimensions[i]);
				}
			}
		}

		return TRUE;
	}


private:
	void Init()
	{
		m_nCompressOptions = 0;
		m_nVertexDimension = 3;
		m_nNormalDimension = 3;
		m_pVertices = NULL;
		m_nVerticesCount = 0;
		m_nVertexStride = 0;
		m_fVertCompressConstant = 0.0;
		m_pNormals = NULL;
		m_nNormalCount = 0;
		m_nNormalStride = 0;
		m_pVertexColor = NULL;
		m_pSecondVertexColor = NULL;
		m_nVertexColorCount = 0;
		m_nSecondVertexColorCount = 0;
		m_nVertexColorStride = 0;
		m_nSecondVertexColorStride = 0;

		m_pBlendIndices = NULL;
		m_nBlendWeightsCount = 0;
		m_nBlendWeightsStride = 0;
		m_pBlendWeights = NULL;
		m_nBlendIndicesCount = 0;
		m_nBlendIndicesStride = 0;

		m_nVertexOptions = sizeof(T)==sizeof(YkDouble) ? VO_VERTEX_DOUBLE : 0;
		m_nVertexOptions = m_nVertexOptions | VO_NORMALS | VO_DIFFUSE_COLOURS | VO_TEXTURE_COORDS;

		YkUshort i = 0;
		for (i = 0; i < SMSCN_MAX_TEXTURE_COORD_SETS; i++)
		{
			m_pTexCoords[i] = NULL;
			m_TexCoordStride[i] = 0;
			m_TexCoordCount[i] = 0;
			m_nTexDimensions[i] = 2;
			m_texCoordCompressConstant[i] = -1.0;
			m_minTexCoordValue[i][0] = m_minTexCoordValue[i][1] = m_minTexCoordValue[i][2] = m_minTexCoordValue[i][3] = 0.0;
		}
	};
};

}

#endif
