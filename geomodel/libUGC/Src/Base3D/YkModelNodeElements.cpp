/*
 *
 * YkModelNodeElements.cpp
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

#include "Base3D/YkModelNodeElements.h"
#include "Toolkit/YkDataCodec.h"
#include "Base3D/YkMathEngine.h"
#include "Toolkit/YkImageOperator.h"

namespace Yk
{
YkModelSkeleton::YkModelSkeleton(): 
	m_matLocalView(YkMatrix4d::IDENTITY)
{	
	m_nRefCount = 0;
	m_BoundingBox.SetNULL();	
	m_pDataExactPack = NULL;
	m_pDataInExactPack = NULL;
	m_pDataRenderPack = NULL;
	m_bExactTag = FALSE;
	m_bRenderPackShared = TRUE;
}

YkModelSkeleton::YkModelSkeleton(YkString strName): 
	m_matLocalView(YkMatrix4d::IDENTITY)
{	
	m_strName = strName;
	m_nRefCount = 0;
	m_BoundingBox.SetNULL();
	m_pDataExactPack = NULL;
	m_pDataInExactPack = NULL;
	m_pDataRenderPack = NULL;
	m_bExactTag = FALSE;
	m_bRenderPackShared = TRUE;
}

YkModelSkeleton::YkModelSkeleton(const YkModelSkeleton& other) 
{
	m_pDataExactPack = NULL;
	m_pDataInExactPack = NULL;

	m_pDataRenderPack = NULL;
	m_bRenderPackShared = TRUE;

	m_bExactTag = FALSE;
	*this = other;
}

YkModelSkeleton& YkModelSkeleton::operator=(const YkModelSkeleton& other)
{
	Clear();

	m_strMaterialName = other.m_strMaterialName;
	m_strName = other.m_strName;
	m_BoundingBox = other.m_BoundingBox;
	m_matLocalView = other.m_matLocalView;
	m_nRefCount = other.m_nRefCount;

	if(m_bExactTag)
	{
		YKASSERT(other.m_pDataExactPack != NULL);
		m_pDataExactPack = new YkModelSkeletonExactPack();
		*m_pDataExactPack = *other.m_pDataExactPack;
	}
	else
	{
		YKASSERT(other.m_pDataInExactPack != NULL);
		m_pDataInExactPack = new YkModelSkeletonRenderPack();
		*m_pDataInExactPack = *other.m_pDataInExactPack;
	}
	return *this;
}

YkModelSkeleton::~YkModelSkeleton()
{
	Clear();
}

void YkModelSkeleton::Clear()
{
	ClearRenderPack();
	ClearExactPack();
	ClearInExactPack();

	m_BoundingBox.SetNULL();
	m_matLocalView = YkMatrix4d::IDENTITY;
	m_strMaterialName = _U("");
	m_bExactTag = FALSE;
}

std::vector<YkString> YkModelSkeleton::GetAtt()
{
	std::vector<YkString> vecAtt;
	vecAtt.push_back(m_strMaterialName);
	return vecAtt;
}

void YkModelSkeleton::ClearRenderPack()
{
	if(m_pDataRenderPack != NULL)
	{
		m_pDataRenderPack->Clear(!m_bRenderPackShared);
		
		delete m_pDataRenderPack;
		m_pDataRenderPack = NULL;
	}
}

void YkModelSkeleton::ClearExactPack()
{
	if(m_pDataExactPack != NULL)
	{
		delete m_pDataExactPack;
		m_pDataExactPack = NULL;
	}
}

void YkModelSkeleton::ClearInExactPack()
{
	if(m_pDataInExactPack != NULL)
	{
		delete m_pDataInExactPack;
		m_pDataInExactPack = NULL;
	}
}

YkBool YkModelSkeleton::Save(YkStream& stream)
{
	stream << m_strName;
	stream << m_strMaterialName;

	//m_BoundingBox
	GetBoundingBox();
	stream << m_BoundingBox.GetMax().x;
	stream << m_BoundingBox.GetMax().y;
	stream << m_BoundingBox.GetMax().z;
	stream << m_BoundingBox.GetMin().x;
	stream << m_BoundingBox.GetMin().y;
	stream << m_BoundingBox.GetMin().z;

	stream.Save(m_matLocalView.GetPointer(), 16);

	//外部设置
	if(m_bExactTag)
	{
		YKASSERT(m_pDataExactPack != NULL);
		m_pDataExactPack->Save(stream);
	}
	else
	{
		YKASSERT(m_pDataInExactPack != NULL);
		m_pDataInExactPack->Save(stream);
	}

	return TRUE;
}

YkBool YkModelSkeleton::Load(YkStream& stream)
{
	stream >> m_strName;
	stream >> m_strMaterialName;

	//m_BoundingBox
	YkVector3d vecMax;
	YkVector3d vecMin;
	stream >> vecMax.x;//YkDouble
	stream >> vecMax.y;
	stream >> vecMax.z;
	stream >> vecMin.x;
	stream >> vecMin.y;
	stream >> vecMin.z;
	m_BoundingBox.SetMax(vecMax);
	m_BoundingBox.SetMin(vecMin);

	stream.Load(m_matLocalView.GetPointer(), 16);

	YkUlong lpos = stream.GetPosition();
	YkInt nVertexOptions = 0;
	stream>>nVertexOptions;
	stream.SetPosition(lpos); //记得回去

	m_bExactTag = (nVertexOptions & VO_VERTEX_DOUBLE) == VO_VERTEX_DOUBLE;
	
	if(!m_bExactTag)
	{
		m_pDataInExactPack = new YkModelSkeletonRenderPack();
		m_pDataInExactPack->Load(stream);
	}
	else
	{
		m_pDataExactPack = new YkModelSkeletonExactPack();
		m_pDataExactPack->Load(stream);
	}
	return TRUE;
}

YkBool YkModelSkeleton::IsValid()
{
	if(!m_bExactTag)
	{
		YKASSERT(m_pDataInExactPack != NULL);
		m_pDataInExactPack->IsValid();
	}
	else
	{
		YKASSERT(m_pDataExactPack != NULL);
		m_pDataExactPack->IsValid();
	}
	return TRUE;
}

YkArray<YkString> YkModelSkeleton::GetPassNames()
{
	YkArray<YkString> arrResult;
	if(m_bExactTag)
	{
		YKASSERT(m_pDataExactPack != NULL);
		for (YkInt i=0; i<m_pDataExactPack->m_arrIndexPack.GetSize(); i++)
		{
			arrResult.Append(m_pDataExactPack->m_arrIndexPack[i]->m_strPassName);
		}		
	}
	else
	{
		YKASSERT(m_pDataInExactPack != NULL);
		for (YkInt i=0; i<m_pDataInExactPack->m_arrIndexPack.GetSize(); i++)
		{
			arrResult.Append(m_pDataInExactPack->m_arrIndexPack[i]->m_strPassName);
		}
	}
	return arrResult;
}

YkInt YkModelSkeleton::GetIndexPackageCount()
{
	return m_bExactTag ? m_pDataExactPack->m_arrIndexPack.GetSize() :
		m_pDataInExactPack->m_arrIndexPack.GetSize();
}

YkIndexPackage* YkModelSkeleton::GetIndexPackage(const YkInt i)
{
	if(i<0 || i >= GetIndexPackageCount())
	{
		return NULL;
	}

	return m_bExactTag ? m_pDataExactPack->m_arrIndexPack[i] :
		m_pDataInExactPack->m_arrIndexPack[i];
}

void YkModelSkeleton::SetExactDataPackRef(YkVertexDataPackageExact* pVertexPack, 
						 YkIndexPackage* pIndexPack)
{
	YkArray<YkIndexPackage*> arrIndexPack;
	arrIndexPack.Add(pIndexPack);
	SetExactDataPackRef(pVertexPack, arrIndexPack);
}

void YkModelSkeleton::SetExactDataPackRef(YkVertexDataPackageExact* pVertexPack, 
						 YkArray<YkIndexPackage*> &arrIndexPack)
{
	ClearRenderPack();
	m_BoundingBox.SetNULL();

	if(m_pDataExactPack == NULL)
	{
		m_pDataExactPack = new YkModelSkeletonExactPack();
	}
	m_pDataExactPack->SetDataPackRef(pVertexPack, arrIndexPack);

	SetExactDataTag(TRUE);
}

YkModelSkeletonExactPack* YkModelSkeleton::GetExactDataPackRef()
{
	return m_pDataExactPack;
}

YkModelSkeletonRenderPack* YkModelSkeleton::GetInExactDataPackRef()
{
	return m_pDataInExactPack;
}

void YkModelSkeleton::SetDataPackRef(YkVertexDataPackage* pVertexPack, YkArray<YkIndexPackage*> &arrIndexPack)
{
	ClearRenderPack();
	m_BoundingBox.SetNULL();

	if(m_pDataInExactPack == NULL)
	{
		m_pDataInExactPack = new YkModelSkeletonRenderPack();
	}
	m_pDataInExactPack->SetDataPackRef(pVertexPack, arrIndexPack);

	SetExactDataTag(FALSE);
}

void YkModelSkeleton::SetDataPackRef(YkVertexDataPackage* pVertexPack, YkIndexPackage* pIndexPack)
{
	YkArray<YkIndexPackage*> arrIndexPack;
	arrIndexPack.Add(pIndexPack);
	SetDataPackRef(pVertexPack, arrIndexPack);
}

void YkModelSkeleton::SetIndexPackage(YkArray<YkIndexPackage*> &arrIndexPack)
{
	if(m_bExactTag)
	{
		YKASSERT(m_pDataExactPack != NULL);
		m_pDataExactPack->SetIndexPackage(arrIndexPack);
	}
	else
	{
		YKASSERT(m_pDataInExactPack != NULL);
		m_pDataInExactPack->SetIndexPackage(arrIndexPack);
	}		
}

YkInt YkModelSkeleton::GetVertexCount()
{
	return m_bExactTag ? m_pDataExactPack->m_pVertexDataPack->m_nVerticesCount :
		m_pDataInExactPack->m_pVertexDataPack->m_nVerticesCount;
}



YkVector3d YkModelSkeleton::GetVertex(const YkInt i)
{
	return m_bExactTag ? m_pDataExactPack->GetVertex(i) :
		m_pDataInExactPack->GetVertex(i);
}

void YkModelSkeleton::SetMaterialName(YkString strMaterialName)
{
	m_strMaterialName = strMaterialName;

	if(m_pDataExactPack == NULL && m_pDataInExactPack == NULL)
	{
		return;
	}

	if(m_bExactTag)
	{
		for (YkInt i=0; i<m_pDataExactPack->m_arrIndexPack.GetSize(); i++)
		{
			m_pDataExactPack->m_arrIndexPack[i]->m_strPassName.RemoveAll();
			m_pDataExactPack->m_arrIndexPack[i]->m_strPassName.Add(m_strMaterialName);
		}
	}
	else
	{
		for (YkInt i=0; i<m_pDataInExactPack->m_arrIndexPack.GetSize(); i++)
		{
			m_pDataInExactPack->m_arrIndexPack[i]->m_strPassName.RemoveAll();
			m_pDataInExactPack->m_arrIndexPack[i]->m_strPassName.Add(m_strMaterialName);
		}
	}

	if(m_pDataRenderPack != NULL)
	{
		for (YkInt i=0; i<m_pDataRenderPack->m_arrIndexPack.GetSize(); i++)
		{
			m_pDataRenderPack->m_arrIndexPack[i]->m_strPassName.RemoveAll();
			m_pDataRenderPack->m_arrIndexPack[i]->m_strPassName.Add(m_strMaterialName);
		}
	}
}

void YkModelSkeleton::SetExactDataTag(YkBool bExactTag)
{
	m_bExactTag = bExactTag;
}

YkBool YkModelSkeleton::GetExactDataTag() const
{
	return m_bExactTag;
}

//////////////////////////////////////////////////////////////////////////

YkModelTexture::YkModelTexture()
{
	m_pTextureData = new YkTextureData();
	m_bMipmap = FALSE;
	m_nLevel = 0;
}

YkModelTexture::YkModelTexture(YkString strName)
{
	m_pTextureData = new YkTextureData();
	m_bMipmap = FALSE;
	m_nLevel = 0;
	m_strName = strName;
}

YkModelTexture::YkModelTexture(const YkModelTexture& other)
{
	*this = other;
}

YkModelTexture& YkModelTexture::operator = (const YkModelTexture& other)
{
	m_pTextureData = new YkTextureData(*(other.m_pTextureData));
	m_bMipmap = other.m_bMipmap;
	m_nLevel = other.m_nLevel;
	m_strName = other.m_strName;
	return *this;
}

YkModelTexture::~YkModelTexture()
{
	if(m_pTextureData != NULL)
	{
		delete m_pTextureData;
		m_pTextureData = NULL;
	}
}

YkBool YkModelTexture::Save(YkStream& stream)
{
	if(m_pTextureData == NULL)
	{
		return FALSE;
	}

	if (!m_bMipmap && m_pTextureData != NULL
		&& m_pTextureData->m_pBuffer != NULL)
	{
		UpdateTextureTier();
	}

	stream << m_strName;
	stream << m_bMipmap;
	stream << m_nLevel;
	m_pTextureData->Save(stream, 14);
	return TRUE;
}

YkBool YkModelTexture::Load(YkStream& stream)
{
	if(m_pTextureData == NULL)
	{
		return FALSE;
	}
	stream >> m_strName;
	stream >> m_bMipmap;
	stream >> m_nLevel;
	m_pTextureData->Load(stream);
	return TRUE;
}

YkInt YkModelTexture::ScaleTextureData(YkTextureData* pTextureData, const YkInt nMaxW, const YkInt nMaxH)
{
	YkUint nWidth = pTextureData->m_nWidth;
	YkUint nHeight = pTextureData->m_nHeight;
	YkUint nAdjustW = YkMathEngine::NextP2(pTextureData->m_nWidth);
	YkUint nAdjustH = YkMathEngine::NextP2(pTextureData->m_nHeight);
	nAdjustW = (nMaxW==0) ? nAdjustW : (YKMIN(nAdjustW, nMaxW));
	nAdjustH = (nMaxH==0) ? nAdjustH : (YKMIN(nAdjustH, nMaxH));

	Yk3DPixelFormat enFormat = pTextureData->m_enFormat;
	YkInt comp = 1;
	if (enFormat == PF_BYTE_RGB || enFormat == PF_BYTE_BGR)
	{
		comp = 3;
	}
	else if (enFormat == PF_BYTE_RGBA ||  enFormat == PF_BYTE_BGRA)
	{
		comp = 4;
	}

	//若纹理已压缩，则需解压
	YkUchar* pOutData= NULL;
	if (pTextureData->m_CompressType != YkDataCodec::encNONE)
	{
		YkInt nOutDataSize = YkImageOperator::Decode(comp,nAdjustW,nAdjustH,&pOutData,pTextureData->m_pBuffer,pTextureData->m_CompressType,FALSE,pTextureData->m_nSize);
		comp = 4;
		delete[] pTextureData->m_pBuffer;
		pTextureData->m_pBuffer = pOutData;
		pTextureData->m_nSize = nOutDataSize;
		pTextureData->m_CompressType = YkDataCodec::encNONE;
	}

	YkUchar* pSrc = pTextureData->m_pBuffer;
	if (nWidth != nAdjustW || nHeight != nAdjustH)
	{
		//缩放纹理图片为2的N次方
		YkUchar* pDest = new YkUchar[nAdjustW * nAdjustH * comp];
		YkMathEngine::ScaleImageInternal(comp, nWidth, nHeight, pSrc, nAdjustW, nAdjustH, pDest);
		delete[] pTextureData->m_pBuffer;
		pTextureData->m_nSize = nAdjustW * nAdjustH * comp;
		pTextureData->m_pBuffer = pDest;
	}
	
	pTextureData->m_nWidth = nAdjustW;
	pTextureData->m_nHeight = nAdjustH;
	return comp;
}

void YkModelTexture::UpdateTextureTier()
{
	if ((m_pTextureData == NULL) || (m_bMipmap && m_pTextureData->m_CompressType == YkDataCodec::enrS3TCDXTN))
	{
		return;
	}
	YkInt comp = ScaleTextureData(m_pTextureData);

	Yk3DPixelFormat enFormat = m_pTextureData->m_enFormat;
	YkUint nAdjustW = m_pTextureData->m_nWidth;
	YkUint nAdjustH = m_pTextureData->m_nHeight;
	YkUchar *pDest = m_pTextureData->m_pBuffer;

	//若原始数据没有压缩格式需要将像素格式排列成RGBA
	YKASSERT (m_pTextureData->m_CompressType == YkDataCodec::encNONE);

	YkInt i=0, j=0, k=0;
	YkUchar* pRGBAData = new YkUchar[nAdjustW*nAdjustH*4];
	if (comp == 3)
	{
		if(enFormat == PF_BYTE_BGR)
		{
			for (i=0;i<nAdjustH*nAdjustW;i++,j+=4,k+=3)
			{
				pRGBAData[j]   = pDest[k+2]; //r
				pRGBAData[j+1] = pDest[k+1]; //g
				pRGBAData[j+2] = pDest[k];	//b
				pRGBAData[j+3] = 255;
			}
		}
		else
		{
			for (i=0;i<nAdjustH*nAdjustW;i++,j+=4,k+=3)
			{
				pRGBAData[j]   = pDest[k]; //r
				pRGBAData[j+1] = pDest[k+1]; //g
				pRGBAData[j+2] = pDest[k+2];	//b
				pRGBAData[j+3] = 255;
			}
		}
	}
	else
	{
		if(enFormat == PF_BYTE_BGRA)
		{
			for (i=0;i<nAdjustH*nAdjustW;i++,j+=4,k+=4)
			{
				pRGBAData[j]   = pDest[k+2];	//r
				pRGBAData[j+1] = pDest[k+1];	//g
				pRGBAData[j+2] = pDest[k];	//b
				pRGBAData[j+3] = pDest[k+3]; //a
			}
		}
		else
		{
			for (i=0;i<nAdjustH*nAdjustW;i++,j+=4,k+=4)
			{
				pRGBAData[j]   = pDest[k];	//r
				pRGBAData[j+1] = pDest[k+1];	//g
				pRGBAData[j+2] = pDest[k+2];	//b
				pRGBAData[j+3] = pDest[k+3]; //a
			}
		}
	}
	delete m_pTextureData;
	m_pTextureData = NULL;

	//创建mipmap，压缩为DXT格式
	YkUchar* pOut  = NULL;
	YkTextureData* pTextureData = new YkTextureData;
	pTextureData->m_enFormat = PF_BYTE_RGBA;
	YkUint nCompressedTextureSize= YkImageOperator::Encode(4,nAdjustW,nAdjustH,pRGBAData,&pOut,YkDataCodec::enrS3TCDXTN,TRUE);
	pTextureData->m_nSize = nCompressedTextureSize;
	pTextureData->m_pBuffer = pOut;
	pTextureData->m_nWidth = nAdjustW;
	pTextureData->m_nHeight = nAdjustH;
	pTextureData->m_CompressType = YkDataCodec::enrS3TCDXTN;
	m_nLevel = YkImageOperator::GetMipMapLevel(nAdjustW,nAdjustH);

	m_pTextureData = pTextureData;
	m_bMipmap = TRUE;

	delete[] pRGBAData;
	pRGBAData = NULL;
}



//////////////////////////////////////////////////////////////////////////

YkModelMaterial::YkModelMaterial(YkString strName)
{
	m_strName = strName;
}

YkModelMaterial::YkModelMaterial(const YkModelMaterial & other)
	:YkMaterial3D(other),YkModelElement(other)
{
}

YkModelMaterial & YkModelMaterial::operator=(const YkModelMaterial & other)
{
	YkMaterial3D::operator=(other);
	YkModelElement::operator=(other);
	return *this;
}

YkModelMaterial::~YkModelMaterial()
{
}

std::vector<YkString> YkModelMaterial::GetAtt()
{
	std::vector<YkString> vecResult;
	GetTextureNames(vecResult);
	return vecResult;
}



void YkModelMaterial::GetTextureNames(std::vector<YkString> &vecNames)
{
	if(getNumTechniques() > 0)
	{
		YkTechnique* pTech = getTechnique(0);	
		if(pTech != NULL && pTech->getNumPasses()>0)
		{
			YkPass* pPass = pTech->getPass(0);
			if(pPass != NULL)
			{
				for(YkInt t = 0; t < pPass->GetTextureUnitStatesSize(); t++)
				{
					YkTextureUnitState* pTexUnit = pPass->GetTextureUnitState(t);
					if(pTexUnit == NULL)
					{
						continue;
					}

					YkString strTextureName = pTexUnit->m_strTextureName;
					vecNames.push_back(strTextureName);					
				}
			}
		}
	}
}

void YkModelMaterial::SetTextureName(std::vector<YkString>& vecTexNames)
{
	YkTechnique* pTech = NULL;
	if(getNumTechniques() == 0)
	{
		pTech = createTechnique();
	}
	else
	{
		pTech = getTechnique(0);
	}
	if(pTech->getNumPasses() == 0)
	{
		pTech->createPass();
	}
	YkPass* pPass = pTech->getPass(0);
	
	//不相等就重建
	if(vecTexNames.size() != pPass->GetTextureUnitStatesSize())
	{
		pPass->removeAllTextureUnitState();
	}

	while (pPass->GetTextureUnitStatesSize() < vecTexNames.size())
	{
		pPass->CreateTextureUnitState();
	}
	YKASSERT(pPass->GetTextureUnitStatesSize() == vecTexNames.size());

	for(YkInt t = 0; t < pPass->GetTextureUnitStatesSize(); t++)
	{
		YkTextureUnitState* pTexUnit = pPass->GetTextureUnitState(t);
		if(pTexUnit == NULL)
		{
			continue;
		}
		
		pTexUnit->m_strTextureName = vecTexNames[t];
	}
}

void YkModelMaterial::MakeDefault()
{
	YkTechnique* pTech = createTechnique();
	YkPass* pPass = pTech->createPass();
	pPass->m_strName = m_strName;
}

}