/*
 *
 * YkTextureData.cpp
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

#include "Base3D/YkTextureData.h"
#include "Base3D/YkMathEngine.h"
#include "Toolkit/YkCompress.h"
#include "Toolkit/YkToolkit.h"
#include "Toolkit/YkImageOperator.h"
#include "Toolkit/YkDataCodec.h"

namespace Yk
{
YkTextureData::YkTextureData()
{
	m_pBuffer=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_nDepth=1;
	m_enFormat=PF_UNKNOWN;
	m_nSize = 0;
	m_CompressType = 0;
	m_OriCompressType = YkDataCodec::encNONE;
}
YkTextureData::YkTextureData(const YkTextureData& other)
{
	m_nWidth=other.m_nWidth;
	m_nHeight=other.m_nHeight;
	m_nDepth=other.m_nDepth;
	YkUint nLen= 0;
	if (other.m_enFormat == PF_BYTE_RGB || other.m_enFormat == PF_BYTE_BGR)
	{
		nLen=m_nWidth*m_nHeight*3;
	}
	else if(other.m_enFormat == PF_L8)
	{
		nLen=m_nWidth*m_nHeight;
	}
	else if(other.m_enFormat == PF_BYTE_LA)
	{
		nLen = m_nWidth * m_nHeight * 2;
	}
	else
	{
		nLen=m_nWidth*m_nHeight*4;
	}

	m_pBuffer=NULL;
	m_enFormat=other.m_enFormat;
	m_nSize = other.m_nSize;
	m_CompressType = other.m_CompressType;
	m_OriCompressType = other.m_OriCompressType;

	if (nLen>0||m_nSize>0)
	{
		if (m_CompressType == 0)
		{
			m_pBuffer=new YkUchar[nLen];
			memcpy(m_pBuffer,other.m_pBuffer,nLen);	
		}
		else
		{
			m_pBuffer=new YkUchar[m_nSize];
			memcpy(m_pBuffer,other.m_pBuffer,m_nSize);	
		}
	}
	else
	{
		m_pBuffer=NULL;
	}
}
 YkTextureData* YkTextureData::Clone()
 {
	return new YkTextureData(*this);
 }
YkTextureData::YkTextureData(const YkTextureInfo& textureInfo)
{
	m_nWidth=textureInfo.m_nWidth;
	m_nHeight=textureInfo.m_nHeight;
	m_nDepth=1;

	if (textureInfo.m_enFormat == IPF_RGB)
	{
		m_enFormat = PF_BYTE_RGB;
	}
	else if (textureInfo.m_enFormat == IPF_RGBA)
	{
		m_enFormat = PF_BYTE_RGBA;
	}
	m_nSize = textureInfo.m_nSize;
	m_CompressType = textureInfo.m_nCompress;
	m_OriCompressType = textureInfo.m_nCompress;
	m_pBuffer=NULL;
	if (textureInfo.m_nSize>0)
	{
		m_pBuffer=new YkUchar[textureInfo.m_nSize];
		memcpy(m_pBuffer,textureInfo.m_pBuffer,textureInfo.m_nSize);
	}
}

YkTextureData& YkTextureData::operator=(const YkTextureData& other)
{
	if (this== &other)
	{
		return *this;
	}
	if (m_pBuffer!=NULL)
	{
		delete[] m_pBuffer;
		m_pBuffer=NULL;
	}
	m_nWidth=other.m_nWidth;
	m_nHeight=other.m_nHeight;
	m_nDepth=other.m_nDepth;
	YkUint nLen = 0;
	if (other.m_enFormat == PF_BYTE_RGB || other.m_enFormat == PF_BYTE_BGR)
	{
		nLen=m_nWidth*m_nHeight*m_nDepth*3;
	}
	else if(other.m_enFormat == PF_L8)
	{
		nLen=m_nWidth*m_nHeight*m_nDepth;
	}
	else if(other.m_enFormat == PF_BYTE_LA)
	{
		nLen = m_nWidth * m_nHeight * m_nDepth * 2;
	}
	else if(other.m_enFormat == PF_FLOAT32_RGBA)
	{
		nLen = m_nWidth * m_nHeight * m_nDepth * 4 * sizeof(YkFloat);
	}
	else
	{
		nLen=m_nWidth*m_nHeight*m_nDepth*4;
	}
	m_enFormat=other.m_enFormat;
	m_nSize = other.m_nSize;
	m_CompressType = other.m_CompressType;
	m_OriCompressType = other.m_OriCompressType;
	if (nLen>0)
	{
		m_pBuffer=new YkUchar[m_nSize];
		memcpy(m_pBuffer,other.m_pBuffer,m_nSize);
	}
	return *this;
}
YkTextureData::~YkTextureData()
{
	Release();
}
void YkTextureData::Release()
{
	if (m_pBuffer!=NULL) 
	{
		delete []m_pBuffer;
		m_pBuffer=NULL;
		m_nSize = 0;
	}
}

YkBool YkTextureData::Load(YkStream& fStream ,YkUint eCodecType)
{
	fStream>>m_CompressType;
	switch (m_CompressType)
	{
	case 0://encNONE
		{			
		}
		break;
	case 14://enrS3TCDXTN
		{
			fStream>>m_nWidth;
			fStream>>m_nHeight;
			YkInt nFormat;
			fStream>>nFormat;
			m_enFormat = (Yk3DPixelFormat)nFormat;
			fStream>>m_nSize;
			YkInt nZipSize = 0;
			fStream>>nZipSize;
			YkUchar* pOutZip = new(std::nothrow) YkUchar[nZipSize];
			if(pOutZip == NULL)
			{
				return FALSE;
			}

			fStream.Load(pOutZip,nZipSize);

			m_pBuffer = new(std::nothrow) YkUchar[m_nSize];
			if(m_pBuffer == NULL)
			{
				delete[] pOutZip;
				pOutZip = NULL;
				return FALSE;
			}

			if (m_pBuffer != NULL)
			{
				YkCompress::UnZip(m_pBuffer,m_nSize,pOutZip,nZipSize);
			}
			
			delete[] pOutZip;
			pOutZip = NULL;
		}
		break;
	case 16://enrGIF
		{
			fStream>>m_nSize;
			m_pBuffer = new(std::nothrow) YkUchar[m_nSize];
			if(m_pBuffer == NULL)
			{
				return FALSE;
			}

			fStream.Load(m_pBuffer,m_nSize);
		}
		break;
	}	

	return TRUE;
}

//! \brief 存入流文件中
void YkTextureData::Save(YkStream& fStream,YkUint eCodecType)
{
	fStream<<eCodecType;
	switch (eCodecType)
	{
	case 0://encNONE
		{	
		}
		break;
	case 14://enrS3TCDXTN
		{
			if (m_CompressType == eCodecType)
			{
				YkUint nCompressedTextureSize = m_nSize;
				YkUchar* pOutZip  = new YkUchar[m_nSize];

				YkCompress::Zip(pOutZip,nCompressedTextureSize,m_pBuffer,m_nSize);

				fStream<<m_nWidth;
				fStream<<m_nHeight;
				fStream<<m_enFormat;
				fStream<<m_nSize;
				fStream<<nCompressedTextureSize;
				fStream.Save(pOutZip,nCompressedTextureSize);

				delete[] pOutZip;
				pOutZip = NULL;

			}
			else if(m_CompressType == YkDataCodec::encNONE)
			{
				BuildTextureTier(fStream);
			}
		}
		break;
	case 16://enrGIF
		{
			fStream<<m_nSize;
			fStream.Save(m_pBuffer,m_nSize);
		}
		break;
	}	
}
YkBool YkTextureData::BuildTextureTier(YkStream& fStream)
{
	YkUint nWidth = m_nWidth;
	YkUint nHeight = m_nHeight;

	// 把图片缩放到最合适的范围内
	YkUint nAdjustW = YkMathEngine::NextP2(m_nWidth);
	YkUint nAdjustH = YkMathEngine::NextP2(m_nHeight);

	YkUint comp = 1;

	if (m_enFormat == PF_BYTE_RGB || m_enFormat == PF_BYTE_BGR)
	{
		comp = 3;
	}
	else if (m_enFormat == PF_BYTE_RGBA ||  m_enFormat == PF_BYTE_BGRA)
	{
		comp = 4;
	}

	YkUchar* pData    = new YkUchar[nAdjustW*nAdjustH*4];
	YkUchar* pTexture = new YkUchar[nAdjustW*nAdjustH*4];

	if (pData == NULL || pTexture == NULL)
	{
		delete []pData;
		delete []pTexture;
		return FALSE;
	}

	if (nWidth != nAdjustW || nHeight != nAdjustH)
	{
		YkImageOperator::Scale(comp,nWidth,nHeight,m_pBuffer,
			nAdjustW,nAdjustH,pTexture);
	}
	else
	{
		memcpy(pTexture,m_pBuffer,nWidth*nHeight*comp);
	}

	nWidth  = nAdjustW;
	nHeight = nAdjustH;

	// 调整像素格式排列成RGBA
	YkInt i=0;
	YkInt j=0;
	YkInt k=0;

	if (comp == 3)
	{
		for (i=0;i<nHeight*nWidth;i++,j+=4,k+=3)
		{
			pData[j]   = pTexture[k+2]; //r
			pData[j+1] = pTexture[k+1]; //g
			pData[j+2] = pTexture[k];	//b
			pData[j+3] = 255;
		}
	}
	else
	{
		for (i=0;i<nHeight*nWidth;i++,j+=4,k+=4)
		{
			pData[j]   = pTexture[k+2];	//r
			pData[j+1] = pTexture[k+1];	//g
			pData[j+2] = pTexture[k];	//b
			pData[j+3] = pTexture[k+3]; //a
		}
	}

	YkUint size = 0;
	YkUint twidth  = nWidth;
	YkUint theight = nHeight;

	YkUchar* pOut  = NULL;
	YkUint nCompressedTextureSize= YkImageOperator::Encode(comp,twidth,theight,pData,&pOut,YkDataCodec::enrS3TCDXTN,TRUE);

	YkUchar* pOutZip  = new YkUchar[nCompressedTextureSize];
	
	m_nSize = nCompressedTextureSize;
	YkCompress::Zip(pOutZip,nCompressedTextureSize,pOut,nCompressedTextureSize);

 	fStream<<nWidth;
 	fStream<<nHeight;
 	fStream<<m_enFormat;
	fStream<<m_nSize;
	fStream<<nCompressedTextureSize;
	fStream.Save(pOutZip,nCompressedTextureSize);

	delete[] pData;
	pData = NULL;

	delete[] pOut;
	pOut = NULL;

	delete[] pOutZip;
	pOutZip = NULL;

	delete[] pTexture;
	pTexture = NULL;

	return TRUE;
}

}
