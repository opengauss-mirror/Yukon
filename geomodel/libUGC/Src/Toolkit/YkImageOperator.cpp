/*
 *
 * YkImageOperator.cpp
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

#include "Toolkit/YkImageOperator.h"
#include "Toolkit/YkMemImage.h"

using namespace Yk;

#ifdef __linux__
YkBool YkImageOperator::m_IsSupportDxt = TRUE;
#endif

YkUint YkImageOperator::Encode( const YkUint nPixSize, YkUint &nWidth, YkUint &nHeight,
							   YkByte* pBufferIn,YkByte **ppBufferOut, YkUint eCodecType ,YkBool bGeneMipmaps, YkUint eQualityLevel/* = 128 */)
{
	//mipmap级别，默认1
	YkByte *pMipmapBuffer =NULL;
	YkArray<YkUint> arrByteOffset;

	YkByte *pEncodeSoureBuffer = NULL;
	pEncodeSoureBuffer = pBufferIn;

	if (bGeneMipmaps)
	{		
		YkImageOperator::GetMipMapData(nWidth,nHeight,pEncodeSoureBuffer,&pMipmapBuffer,arrByteOffset);				
	}
	else
	{
		pMipmapBuffer = pEncodeSoureBuffer;
		arrByteOffset.Add(0);
	}

	YkUint nMipmapCount =arrByteOffset.GetSize();
	YkUint nCompressedSize =0;

	if (*ppBufferOut !=NULL)
	{
		delete [](*ppBufferOut);
		(*ppBufferOut) =NULL;
	}

	switch (eCodecType)
	{
	case YkDataCodec::encNONE:
		{
			nCompressedSize = nWidth * nHeight * nPixSize;
			*ppBufferOut =new YkUchar[nCompressedSize];
			memcpy((*ppBufferOut),pBufferIn,nCompressedSize);
		}
		break;
	case YkDataCodec::enrS3TCDXTN:
		{
			YkUint nW =nWidth;
			YkUint nH =nHeight;
			YkUint i;
			YkUint nDxtSize =YkImageOperator::GetCompressedTextureSize(nPixSize,nWidth,nHeight,YkDataCodec::enrS3TCDXTN,bGeneMipmaps);

			*ppBufferOut =new YkUchar[nDxtSize];
			YkUchar *pBufferPointer =*ppBufferOut;
			for (i =0;i<nMipmapCount;i++)
			{
				YkUint nOffsetSrc =arrByteOffset.GetAt(i);
				YkMemImage::Encode(nPixSize,nW,nH,pMipmapBuffer + nOffsetSrc,pBufferPointer);			
				pBufferPointer +=YkImageOperator::GetCompressedTextureSize(nPixSize,nW,nH,YkDataCodec::enrS3TCDXTN);

				nW /=2;
				nH /=2;

				if (nW == 0) nW = 1;
				if (nH == 0) nH = 1;			
			}

			nCompressedSize =nDxtSize;
		}
		break;
	}
	//delete the temp buffer
	if (pMipmapBuffer != NULL && pMipmapBuffer != pEncodeSoureBuffer)
	{
		delete []pMipmapBuffer;
		pMipmapBuffer = NULL;
	}

	if (pEncodeSoureBuffer != pBufferIn)
	{
		delete[] pEncodeSoureBuffer;
		pEncodeSoureBuffer = NULL;
	}

	return nCompressedSize;
}

YkUint YkImageOperator::Decode(const YkUint nPixSize, YkUint nWidth, YkUint nHeight,
							 YkByte **ppBufferOut,const YkByte *pBufferIn,YkUint eCodecType,YkBool bGeneMipmaps,YkUint nTexSize)
{
	//若外部有数据，先清空
	if ((*ppBufferOut) != NULL)
	{
		delete [](*ppBufferOut);
		*ppBufferOut =NULL;
	}

	//返回的解压大小
	YkUint nSizeOut =0;

	switch (eCodecType)
	{
	case 14://YkDataCodec::enrS3TCDXTN:
		{
			nSizeOut =nWidth*nHeight*4;
			(*ppBufferOut) =new YkUchar[nSizeOut];
			YkMemImage::Decode(nPixSize,nWidth,nHeight,*ppBufferOut,pBufferIn);

			if (bGeneMipmaps)
			{
				YkByte *pGenMipMap =NULL;
				YkArray<YkUint> arrOffset;
				nSizeOut =YkImageOperator::GetMipMapData(nWidth,nHeight,*ppBufferOut,&pGenMipMap,arrOffset);
				delete [](*ppBufferOut);

				*ppBufferOut =new YkByte[nSizeOut];
				memcpy(*ppBufferOut,pGenMipMap,nSizeOut);

				delete[]pGenMipMap;
			}
		}
		break;
	}

	return nSizeOut;
}

void YkImageOperator::Scale( const YkUint components, YkUint widthin, YkUint heightin,YkByte *in, YkUint widthout, YkUint heightout, YkByte *out)
{
	YkMemImage::Scale(components,widthin,heightin,in,widthout,heightout,out);
}

YkUint YkImageOperator::GetMipMapData(YkUint nWidth,YkUint nHeight,const YkByte* pBufferIn, YkByte** ppBufferOut,YkArray<YkUint>& arrByteOffset)
{
	YkUint nW =nWidth,nH =nHeight;
	arrByteOffset.RemoveAll();

	//计算mipmap偏移量
	YkUint nMipMapBufferSize =0;
	while(true)
	{
		arrByteOffset.Add(nMipMapBufferSize);
		nMipMapBufferSize +=nW * nH *4;
		if (nW == 1 && nH == 1)
		{
			break;
		}

		nW  /= 2;
		nH /= 2;

		if (0 == nW) nW =1;
		if (0 == nH) nH =1;
	};

	//先清除外部数据
	if (*ppBufferOut)
	{
		delete [](*ppBufferOut);
		(*ppBufferOut) =NULL;
	}

	(*ppBufferOut) =new YkUchar[nMipMapBufferSize]; WhiteBox_Ignore
	memcpy((*ppBufferOut) ,pBufferIn ,nWidth*nHeight*4);

	YkUint i =0;
	YkUint nMipmapCount =arrByteOffset.GetSize();
	nW =nWidth;
	nH =nHeight;

	//生成mipmap
	for (i =0;i<nMipmapCount -1;i++)
	{
		YkUint nOffsetSrc =arrByteOffset.GetAt(i);
		YkUint nOffsetDst =arrByteOffset.GetAt(i+1);

		YkUint nHalfWidth = (nW / 2 == 0) ? 1 : nW / 2;
		YkUint nHalfHeight =(nH / 2 == 0) ? 1 : nH / 2;	

		YkImageOperator::Scale(4,nW,nH,(*ppBufferOut) + nOffsetSrc ,nHalfWidth, nHalfHeight, (*ppBufferOut) +nOffsetDst);	

		nW /=2;
		nH /=2;

		if (0 == nW) nW =1;
		if (0 == nH) nH =1;
	}

	return nMipMapBufferSize;
}

YkUint YkImageOperator::NextP2(YkUint a)
{
	YkUint rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

YkUint YkImageOperator::GetCompressedTextureSize(const YkUint nPixSize,YkUint nWidth,YkUint nHeight,YkUint eCodecType,YkBool bIsMipmaps)
{
	//包含mipmap大小	
	YkUint nBufferSize =0;

	do
	{
		switch (eCodecType)
		{
		case YkDataCodec::encNONE:
			{
				nBufferSize += nWidth * nHeight * nPixSize;
			}
			break;
		case YkDataCodec::enrS3TCDXTN:
			{
				if (3 == nPixSize)
				{
					nBufferSize += DXT1_ENCODE_SIZE(nWidth,nHeight);
				}
				else if(4 == nPixSize)
				{
					nBufferSize += DXT5_ENCODE_SIZE(nWidth,nHeight);
				}		
			}
			break;
		}

		if (nWidth == 1 && nHeight == 1)
		{
			break;
		}

		nWidth  /= 2;
		nHeight /= 2;

		if (0 == nWidth) nWidth =1;
		if (0 == nHeight) nHeight =1;

	}while(bIsMipmaps);

	return nBufferSize;

}

YkUint YkImageOperator::GetMipMapLevel(const YkUint nWidth,const YkUint nHeight)
{
	YkUint nMipMapLevel =0;

	YkUint nW =nWidth;
	YkUint nH =nHeight;

	while(true)
	{
		nMipMapLevel ++;

		if (nW == 1 && nH ==1)
		{
			break;
		}

		nW /=2;
		nH /=2;

		if (nW ==0) nW =1;
		if (nH ==0) nH =1;
	}

	return nMipMapLevel;
}

YkBool YkImageOperator::IsCompressedTextureType(YkDataCodec::CodecType eType)
{
	YkBool bResult = TRUE;
	switch (eType)
	{	
	case Yk::YkDataCodec::enrS3TCDXTN:
		bResult = TRUE;
		break;
	default:
		bResult = FALSE;
		break;
	}

	return bResult;
}