/*
 *
 * YkByteArray.cpp
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

#include "Stream/YkByteArray.h"

namespace Yk {

YkByteArray::YkByteArray()
{
	m_bSwap=YK_ISBIGENDIAN;
	m_nPos=0;
	m_eCharset=YkString::Default;
}

YkByteArray::~YkByteArray()
{

}

YkSizeT YkByteArray::Add(YkByte nValue)
{
	m_array.Add(nValue);
    m_nPos=m_array.GetSize();
	return m_nPos;
}

YkSizeT YkByteArray::Add(YkShort nValue)
{
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		m_array.Add(pValue[1]);
		m_array.Add(pValue[0]);
	}
	else
	{
		m_array.Add(pValue[0]);
		m_array.Add(pValue[1]);
	}
	m_nPos=m_array.GetSize();
	return m_nPos;
}

YkSizeT YkByteArray::Add(YkInt nValue)
{

	YkSizeT nTypeSize=sizeof(YkInt);
	m_nPos=m_array.GetSize();
	m_array.SetSize(m_array.GetSize()+nTypeSize);
	return Set(nValue);
}

YkSizeT YkByteArray::Add(YkLong nValue)
{
	YkSizeT nTypeSize=sizeof(YkLong);
	m_nPos=m_array.GetSize();
	m_array.SetSize(m_array.GetSize()+nTypeSize);
	return Set(nValue);
}

YkSizeT YkByteArray::Add(YkFloat dValue)
{

	YkSizeT nTypeSize=sizeof(YkFloat);
	m_nPos=m_array.GetSize();
	m_array.SetSize(m_array.GetSize()+nTypeSize);
	return Set(dValue);
}


YkSizeT YkByteArray::Add(YkDouble dValue)
{
	YkSizeT nTypeSize=sizeof(YkDouble);
	m_nPos=m_array.GetSize();
	m_array.SetSize(m_array.GetSize()+nTypeSize);
	return Set(dValue);
}



YkSizeT YkByteArray::Set(YkByte nValue)
{
	YkSizeT nTypeSize=sizeof(YkByte);
	YkByte* pData = ((YkByte*)GetData());
	YkSizeT nSize=m_array.GetSize();
	if((m_nPos+nTypeSize)<=nSize)
	{
		pData[m_nPos]=nValue;
	}
	else
	{
		m_array.Add(nValue);
	}
	
	
    m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Set(YkShort nValue)
{
	YkSizeT nTypeSize=sizeof(YkShort);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if((m_nPos+nTypeSize)<=nSize)
	{
		if(m_bSwap)
		{
			pData[1]=pValue[0];
			pData[0]=pValue[1];
		}
		else
		{
			memcpy(pData,&nValue,sizeof(YkShort));
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			pData[1]=pValue[0];
			pData[0]=pValue[1];
		}
		else
		{
			memcpy(pData,&nValue,sizeof(YkShort));
		}
	}
	m_nPos += nTypeSize;
	return m_nPos;	
}



YkSizeT YkByteArray::Set(YkInt nValue)
{
	YkSizeT nTypeSize=sizeof(YkInt);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if((m_nPos+nTypeSize)<=nSize)
	{
		if(m_bSwap)
		{
			pData[3]=pValue[0];
			pData[2]=pValue[1];
			pData[1]=pValue[2];
			pData[0]=pValue[3];
		}
		else
		{
			memcpy(pData,&nValue,sizeof(YkInt));
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			pData[3]=pValue[0];
			pData[2]=pValue[1];
			pData[1]=pValue[2];
			pData[0]=pValue[3];
		}
		else
		{
			memcpy(pData,&nValue,sizeof(YkInt));
		}

	}
	m_nPos += nTypeSize;
	return m_nPos;
}



YkSizeT YkByteArray::Set(YkLong nValue)
{
	YkSizeT nTypeSize=sizeof(YkLong);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if((m_nPos+nTypeSize)<=nSize)
	{
		if(m_bSwap)
		{
			pData[7]=pValue[0];
			pData[6]=pValue[1];
			pData[5]=pValue[2];
			pData[4]=pValue[3];
			pData[3]=pValue[4];
			pData[2]=pValue[5];
			pData[1]=pValue[6];
			pData[0]=pValue[7];
		}
	    else
		{
			memcpy(pData,&nValue,sizeof(YkLong));
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			pData[7]=pValue[0];
			pData[6]=pValue[1];
			pData[5]=pValue[2];
			pData[4]=pValue[3];
			pData[3]=pValue[4];
			pData[2]=pValue[5];
			pData[1]=pValue[6];
			pData[0]=pValue[7];
		}
		else
		{
			memcpy(pData,&nValue,sizeof(YkLong));
		}
	}

	m_nPos += nTypeSize;
	return m_nPos;
}



YkSizeT YkByteArray::Set(YkFloat dValue)
{
	YkSizeT nTypeSize=sizeof(YkFloat);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&dValue;
	if((m_nPos+nTypeSize)<=nSize)
	{
		if(m_bSwap)
		{
			pData[3]=pValue[0];
			pData[2]=pValue[1];
			pData[1]=pValue[2];
			pData[0]=pValue[3];
		}
		else
		{
			memcpy(pData,&dValue,sizeof(YkFloat));
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			pData[3]=pValue[0];
			pData[2]=pValue[1];
			pData[1]=pValue[2];
			pData[0]=pValue[3];
		}
		else
		{
			memcpy(pData,&dValue,sizeof(YkFloat));
		}
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Set(YkDouble dValue)
{
	YkSizeT nTypeSize=sizeof(YkDouble);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&dValue;
	if((m_nPos+nTypeSize)<=nSize)
	{
		if(m_bSwap)
		{
			pData[7]=pValue[0];
			pData[6]=pValue[1];
			pData[5]=pValue[2];
			pData[4]=pValue[3];
			pData[3]=pValue[4];
			pData[2]=pValue[5];
			pData[1]=pValue[6];
			pData[0]=pValue[7];
		}
		else
		{
			memcpy(pData,&dValue,sizeof(YkDouble));
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			pData[7]=pValue[0];
			pData[6]=pValue[1];
			pData[5]=pValue[2];
			pData[4]=pValue[3];
			pData[3]=pValue[4];
			pData[2]=pValue[5];
			pData[1]=pValue[6];
			pData[0]=pValue[7];
		}
		else
		{
			memcpy(pData,&dValue,sizeof(YkDouble));
		}
	}

	m_nPos += nTypeSize;
	return m_nPos;
}



YkSizeT YkByteArray::Set(const YkByte* pBytes, YkSizeT nCount)
{
	YKASSERT(pBytes != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkByte);
	YkSizeT nSize=m_array.GetSize();
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	if((m_nPos+nCount)<=nSize)
	{
		memcpy(pData,pBytes,nCount);
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		memcpy(pData,pBytes,nCount);
	}
	
    m_nPos=m_nPos+nCount;
	return m_nPos;
}


YkSizeT YkByteArray::Set(const YkShort *pShorts, YkSizeT nCount)
{
	YKASSERT(pShorts != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkShort);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pShorts;
	if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[1]=pValue[0];
				pData[0]=pValue[1];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[1]=pValue[0];
				pData[0]=pValue[1];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Set(const YkInt *pInts, YkSizeT nCount)
{

	YKASSERT(pInts != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkInt);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pInts;
	if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[3]=pValue[0];
				pData[2]=pValue[1];
				pData[1]=pValue[2];
				pData[0]=pValue[3];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[3]=pValue[0];
				pData[2]=pValue[1];
				pData[1]=pValue[2];
				pData[0]=pValue[3];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;	
}


	
YkSizeT YkByteArray::Set(const YkLong *pLongs, YkSizeT nCount)
{
	YKASSERT(pLongs != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkLong);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pLongs;
    if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[7]=pValue[0];
				pData[6]=pValue[1];
				pData[5]=pValue[2];
				pData[4]=pValue[3];
				pData[3]=pValue[4];
				pData[2]=pValue[5];
				pData[1]=pValue[6];
				pData[0]=pValue[7];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[7]=pValue[0];
				pData[6]=pValue[1];
				pData[5]=pValue[2];
				pData[4]=pValue[3];
				pData[3]=pValue[4];
				pData[2]=pValue[5];
				pData[1]=pValue[6];
				pData[0]=pValue[7];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Set(const YkFloat* pFloats, YkSizeT nCount)
{
	YKASSERT(pFloats != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkFloat);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pFloats;
	if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[3]=pValue[0];
				pData[2]=pValue[1];
				pData[1]=pValue[2];
				pData[0]=pValue[3];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[3]=pValue[0];
				pData[2]=pValue[1];
				pData[1]=pValue[2];
				pData[0]=pValue[3];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Set(const YkDouble* pDoubles, YkSizeT nCount)
{
	YKASSERT(pDoubles != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkDouble);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pDoubles;
    if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[7]=pValue[0];
				pData[6]=pValue[1];
				pData[5]=pValue[2];
				pData[4]=pValue[3];
				pData[3]=pValue[4];
				pData[2]=pValue[5];
				pData[1]=pValue[6];
				pData[0]=pValue[7];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				pData[7]=pValue[0];
				pData[6]=pValue[1];
				pData[5]=pValue[2];
				pData[4]=pValue[3];
				pData[3]=pValue[4];
				pData[2]=pValue[5];
				pData[1]=pValue[6];
				pData[0]=pValue[7];
				pData=pData+nTypeSize;
				pValue=pValue+nTypeSize;
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
		}		
	}
	
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Set(const YkPoint* pPoints, YkSizeT nCount)
{
	YKASSERT(pPoints != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoints;
    if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkInt*)pPoints,nCount*2);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkInt*)pPoints,nCount*2);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}		
	}
	
	return m_nPos;
}

YkSizeT YkByteArray::Set(const YkPoint2D* pPoint2Ds, YkSizeT nCount)
{
	YKASSERT(pPoint2Ds != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint2D);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoint2Ds;
    if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkDouble *)pPoint2Ds,nCount*2);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkDouble *)pPoint2Ds,nCount*2);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}		
	}	
	return m_nPos;	
}
YkSizeT YkByteArray::Set(const YkPoint3D* pPoint3Ds, YkSizeT nCount)
{
	YKASSERT(pPoint3Ds != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint3D);
	YkSizeT i=0;
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoint3Ds;
    if((m_nPos+nTypeSize*nCount)<=m_array.GetSize())
	{
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkDouble *)pPoint3Ds,nCount*3);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}
	}
	else
	{
		m_array.SetSize(m_nPos+nCount*nTypeSize);
		pData = ((YkByte*)GetData())+m_nPos;
		if(m_bSwap)
		{
			for(i=0;i<nCount;i++)
			{
				Set((YkDouble *)pPoint3Ds,nCount*3);
			}
		}
		else
		{
			memcpy(pData,pValue,nCount*nTypeSize);
			m_nPos += nTypeSize*nCount;
		}		
	}	
	return m_nPos;
}


YkSizeT YkByteArray::Insert(YkByte nValue)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YkSizeT nTypeSize=sizeof(YkByte);
	m_array.InsertAt(m_nPos,nValue);
	m_nPos=m_nPos+nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Insert(YkShort nValue)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YkSizeT nTypeSize=sizeof(YkShort);
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
		YkByte*	pData = ((YkByte*)GetData())+m_nPos;
		YkByte btTemp;
		btTemp=pData[0];
		pData[0]=pData[1];
		pData[1]=btTemp;
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	}
	m_nPos += nTypeSize;
	return m_nPos;
}



YkSizeT YkByteArray::Insert(YkInt nValue)
{
	YkSizeT nTypeSize=sizeof(YkInt);
	YKASSERT(m_nPos<=m_array.GetSize());
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	    YkByte*	pData = ((YkByte*)GetData())+m_nPos;
		YkByte btTemp;
        btTemp=pData[0];
        pData[0]=pData[3];
		pData[3]=btTemp;
		btTemp=pData[1];
        pData[1]=pData[2];
		pData[2]=btTemp;
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

	
YkSizeT YkByteArray::Insert(YkLong nValue)
{
	YkSizeT nTypeSize=sizeof(YkLong);
	YKASSERT(m_nPos<=m_array.GetSize());
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
		YkByte*	pData = ((YkByte*)GetData())+m_nPos;
		YkByte btTemp;
        btTemp=pData[0];
        pData[0]=pData[7];
		pData[7]=btTemp;
		btTemp=pData[1];
        pData[1]=pData[6];
		pData[6]=btTemp;
		btTemp=pData[2];
        pData[2]=pData[5];
		pData[5]=btTemp;
		btTemp=pData[3];
        pData[3]=pData[4];
		pData[4]=btTemp;
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	}
	m_nPos += nTypeSize;
	return m_nPos;
}



YkSizeT YkByteArray::Insert(YkFloat dValue)
{
	YkSizeT nTypeSize=sizeof(YkFloat);
	YKASSERT(m_nPos<=m_array.GetSize());
	YkByte* pValue = (YkByte*)&dValue;
	if(m_bSwap)
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
		YkByte*	pData = ((YkByte*)GetData())+m_nPos;
		YkByte btTemp;
        btTemp=pData[0];
        pData[0]=pData[3];
		pData[3]=btTemp;
		btTemp=pData[1];
        pData[1]=pData[2];
		pData[2]=btTemp;
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Insert(YkDouble dValue)
{
	YkSizeT nTypeSize=sizeof(YkDouble);
	YKASSERT(m_nPos<=m_array.GetSize());
	YkByte* pValue = (YkByte*)&dValue;
	if(m_bSwap)
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
		YkByte*	pData = ((YkByte*)GetData())+m_nPos;
		YkByte btTemp;
        btTemp=pData[0];
        pData[0]=pData[7];
		pData[7]=btTemp;
		btTemp=pData[1];
        pData[1]=pData[6];
		pData[6]=btTemp;
		btTemp=pData[2];
        pData[2]=pData[5];
		pData[5]=btTemp;
		btTemp=pData[3];
        pData[3]=pData[4];
		pData[4]=btTemp;
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize);
	}
	m_nPos += nTypeSize;
	return m_nPos;
}


YkSizeT YkByteArray::Insert(const YkByte* pBytes, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pBytes != NULL);
	YKASSERT(nCount > 0);
	m_array.InsertAt(m_nPos,pBytes,nCount);
	m_nPos=m_nPos+nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Insert(const YkShort *pShorts, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pShorts != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkShort);
	YkSizeT i=0;

	YkByte* pValue = (YkByte*)pShorts;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++){
			m_array.InsertAt(m_nPos,pValue,nTypeSize);
			YkByte*	pData = ((YkByte*)GetData())+m_nPos;
			YkByte btTemp;
			btTemp=pData[0];
			pData[0]=pData[1];
			pData[1]=btTemp;
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}


YkSizeT YkByteArray::Insert(const YkInt *pInts, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pInts != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkInt);
	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pInts;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			m_array.InsertAt(m_nPos,pValue,nTypeSize);
			YkByte*	pData = ((YkByte*)GetData())+m_nPos;
			YkByte btTemp;
			btTemp=pData[0];
			pData[0]=pData[3];
			pData[3]=btTemp;
			btTemp=pData[1];
			pData[1]=pData[2];
	    	pData[2]=btTemp;
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}



YkSizeT YkByteArray::Insert(const YkLong *pLongs, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pLongs != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkLong);
	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pLongs;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			m_array.InsertAt(m_nPos,pValue,nTypeSize);
			YkByte*	pData = ((YkByte*)GetData())+m_nPos;
			YkByte btTemp;
			btTemp=pData[0];
			pData[0]=pData[7];
			pData[7]=btTemp;
			btTemp=pData[1];
			pData[1]=pData[6];
			pData[6]=btTemp;
			btTemp=pData[2];
			pData[2]=pData[5];
			pData[5]=btTemp;
			btTemp=pData[3];
			pData[3]=pData[4];
	    	pData[4]=btTemp;
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}


YkSizeT YkByteArray::Insert(const YkFloat* pFloats, YkSizeT nCount)
{
    YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pFloats != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkFloat);
	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pFloats;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			YkByte*	pData = ((YkByte*)GetData())+m_nPos;
			YkByte btTemp;
			btTemp=pData[0];
			pData[0]=pData[3];
			pData[3]=btTemp;
			btTemp=pData[1];
			pData[1]=pData[2];
	    	pData[2]=btTemp;
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Insert(const YkDouble* pDoubles, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pDoubles != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkDouble);
	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pDoubles;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			m_array.InsertAt(m_nPos,pValue,nTypeSize);
			YkByte*	pData = ((YkByte*)GetData())+m_nPos;
			YkByte btTemp;
			btTemp=pData[0];
			pData[0]=pData[7];
			pData[7]=btTemp;
			btTemp=pData[1];
			pData[1]=pData[6];
			pData[6]=btTemp;
			btTemp=pData[2];
			pData[2]=pData[5];
			pData[5]=btTemp;
			btTemp=pData[3];
			pData[3]=pData[4];
	    	pData[4]=btTemp;
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
	}
	m_nPos += nTypeSize*nCount;
	return m_nPos;
}

YkSizeT YkByteArray::Insert(const YkPoint* pPoints, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pPoints != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint);

	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)&pPoints[0];
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			Insert((YkInt *)pPoints,nCount*2);
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
		m_nPos += nTypeSize*nCount;
	}
	return m_nPos;
}

YkSizeT YkByteArray::Insert(const YkPoint2D* pPoint2Ds, YkSizeT nCount)
{
	YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pPoint2Ds != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint2D);

	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pPoint2Ds;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			Insert((YkDouble *)pPoint2Ds,nCount*2);
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
		m_nPos += nTypeSize*nCount;
	}
	return m_nPos;
}

YkSizeT YkByteArray::Insert(const YkPoint3D* pPoint3Ds, YkSizeT nCount)
{

    YKASSERT(m_nPos<=m_array.GetSize());
	YKASSERT(pPoint3Ds != NULL);
	YKASSERT(nCount > 0);
	YkSizeT nTypeSize=sizeof(YkPoint3D);
	
	YkSizeT i=0;
	
	YkByte* pValue = (YkByte*)pPoint3Ds;
	if(m_bSwap)
	{
		for(i=0;i<nCount;i++)
		{
			Insert((YkDouble *)pPoint3Ds,nCount*3);
		}
	}
	else
	{
		m_array.InsertAt(m_nPos,pValue,nTypeSize*nCount);
		m_nPos += nTypeSize*nCount;
	}
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkByte& nValue)
{
	YkSizeT nTypeSize=sizeof(YkByte);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData());
	nValue=pData[m_nPos];
    m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkAchar& chValue)
{
	YkSizeT nTypeSize=sizeof(YkAchar);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkAchar* pData = ((YkAchar*)GetData());
	chValue=pData[m_nPos];
    m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkShort& nValue)
{
	YkSizeT nTypeSize=sizeof(YkShort);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[1];
		pValue[1]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkShort));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkUshort& nValue)
{
	YkSizeT nTypeSize=sizeof(YkUshort);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[1];
		pValue[1]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkUshort));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkInt& nValue)
{
	YkSizeT nTypeSize=sizeof(YkInt);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[3];
		pValue[1]=pData[2];
		pValue[2]=pData[1];
		pValue[3]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkInt));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}
YkSizeT YkByteArray::Get(YkUint& nValue)
{
	YkSizeT nTypeSize=sizeof(YkUint);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[3];
		pValue[1]=pData[2];
		pValue[2]=pData[1];
		pValue[3]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkUint));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}	
YkSizeT YkByteArray::Get(YkLong& nValue)
{
	YkSizeT nTypeSize=sizeof(YkLong);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[7];
		pValue[1]=pData[6];
		pValue[2]=pData[5];
		pValue[3]=pData[4];
		pValue[4]=pData[3];
		pValue[5]=pData[2];
		pValue[6]=pData[1];
		pValue[7]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkLong));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}
YkSizeT YkByteArray::Get(YkUlong& nValue)
{
	YkSizeT nTypeSize=sizeof(YkUlong);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&nValue;
	if(m_bSwap)
	{
		pValue[0]=pData[7];
		pValue[1]=pData[6];
		pValue[2]=pData[5];
		pValue[3]=pData[4];
		pValue[4]=pData[3];
		pValue[5]=pData[2];
		pValue[6]=pData[1];
		pValue[7]=pData[0];
	}
	else
	{
		memcpy(&nValue,pData,sizeof(YkUlong));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}
YkSizeT YkByteArray::Get(YkFloat& dValue)
{
	YkSizeT nTypeSize=sizeof(YkFloat);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&dValue;
	if(m_bSwap)
	{
		pValue[0]=pData[3];
		pValue[1]=pData[2];
		pValue[2]=pData[1];
		pValue[3]=pData[0];
	}
	else
	{
		memcpy(&dValue,pData,sizeof(YkFloat));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}
YkSizeT YkByteArray::Get(YkDouble& dValue)
{
    YkSizeT nTypeSize=sizeof(YkDouble);
	YKASSERT((m_nPos+nTypeSize)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)&dValue;
	if(m_bSwap)
	{
		pValue[0]=pData[7];
		pValue[1]=pData[6];
		pValue[2]=pData[5];
		pValue[3]=pData[4];
		pValue[4]=pData[3];
		pValue[5]=pData[2];
		pValue[6]=pData[1];
		pValue[7]=pData[0];
	}
	else
	{
		memcpy(&dValue,pData,sizeof(YkDouble));
	}
	m_nPos += nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkString& strValue)
{
#ifdef _UGUNICODE
	if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
	{
		YkInt nLength=0;
		Get(nLength);
		YKASSERT(nLength>0);
		YKASSERT((nLength+m_nPos) <= m_array.GetSize());
		YkByte* pStr=&m_array.ElementAt(m_nPos);
		m_nPos=m_nPos+nLength;
		strValue.Empty();
		strValue.Append((const YkChar*)pStr,nLength/sizeof(YkChar));
		return  m_nPos;
	}
	else
	{
		YkInt nLength=0;
		Get(nLength);
		YKASSERT(nLength>0);
		YKASSERT((nLength+m_nPos) <= m_array.GetSize());
		YkByte* pStr=&m_array.ElementAt(m_nPos);
		m_nPos=m_nPos+nLength;
		strValue.FromMBString((YkAchar*)pStr, nLength, m_eCharset);
		return  m_nPos;
	}
#else
	YkInt nLength=0;
	Get(nLength);
	YKASSERT(nLength>0);
	YKASSERT((nLength+m_nPos) <= m_array.GetSize());
	YkByte* pStr=&m_array.ElementAt(m_nPos);
	m_nPos=m_nPos+nLength;
	strValue.Empty();
	strValue.Append((const YkWchar*)pStr,nLength);
	strValue.SetCharset(m_eCharset);
	return  m_nPos;
#endif
}

YkSizeT YkByteArray::Add(const YkByte* pBytes, YkSizeT nCount)
{
	m_nPos=m_array.GetSize();
	YKASSERT(pBytes != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount);
	return Set(pBytes,nCount);
}

YkSizeT YkByteArray::Add(const YkShort *pShorts, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkShort);
	m_nPos=m_array.GetSize();
	YKASSERT(pShorts != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pShorts,nCount);
}

YkSizeT YkByteArray::Add(const YkInt *pInts, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkInt);
	m_nPos=m_array.GetSize();
	YKASSERT(pInts != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pInts,nCount);
}

YkSizeT YkByteArray::Add(const YkLong *pLongs, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkLong);
	m_nPos=m_array.GetSize();
	YKASSERT(pLongs != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pLongs,nCount);
}

YkSizeT YkByteArray::Add(const YkFloat* pFloats, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkFloat);
	m_nPos=m_array.GetSize();
	YKASSERT(pFloats != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pFloats,nCount);
}

YkSizeT YkByteArray::Add(const YkDouble* pDoubles, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkDouble);
	m_nPos=m_array.GetSize();
	YKASSERT(pDoubles != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pDoubles,nCount);
}
YkSizeT YkByteArray::Add(const YkPoint* pPoints, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint);
	m_nPos=m_array.GetSize();
	YKASSERT(pPoints != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pPoints,nCount);
}
YkSizeT YkByteArray::Add(const YkPoint2D* pPoint2Ds, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint2D);
	m_nPos=m_array.GetSize();
	YKASSERT(pPoint2Ds != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pPoint2Ds,nCount);
}
YkSizeT YkByteArray::Add(const YkPoint3D* pPoint3Ds, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint3D);
	m_nPos=m_array.GetSize();
	YKASSERT(pPoint3Ds != NULL);
	YKASSERT(nCount > 0);
	m_array.SetSize(m_nPos+nCount*nTypeSize);
	return Set(pPoint3Ds,nCount);
}

YkSizeT YkByteArray::Get(YkByte* pBytes, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkAchar);
	YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	memcpy(pBytes,pData,nCount);
    m_nPos += nTypeSize*nCount;
	return m_nPos;
}


YkSizeT YkByteArray::Get(YkShort *pShorts, YkSizeT nCount)
{
   	YkSizeT nTypeSize=sizeof(YkShort);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pShorts;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			pValue[0]=pData[1];
			pValue[1]=pData[0];
			pValue+=nTypeSize;
			pData+=nTypeSize;
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
	}

	m_nPos=m_nPos+nCount*nTypeSize;
	return m_nPos;
}


YkSizeT YkByteArray::Get(YkInt *pInts, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkInt);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pInts;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			pValue[0]=pData[3];
			pValue[1]=pData[2];
			pValue[2]=pData[1];
			pValue[3]=pData[0];
			pValue+=nTypeSize;
			pData+=nTypeSize;
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
	}
	
	m_nPos=m_nPos+nCount*nTypeSize;
	return m_nPos;
}
	
YkSizeT YkByteArray::Get(YkLong *pLongs, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkLong);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pLongs;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			pValue[0]=pData[7];
			pValue[1]=pData[6];
			pValue[2]=pData[5];
			pValue[3]=pData[4];
			pValue[4]=pData[3];
			pValue[5]=pData[2];
			pValue[6]=pData[1];
			pValue[7]=pData[0];
			pValue+=nTypeSize;
			pData+=nTypeSize;
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
	}
	
	m_nPos=m_nPos+nCount*nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkFloat* pFloats, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkFloat);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pFloats;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			pValue[0]=pData[3];
			pValue[1]=pData[2];
			pValue[2]=pData[1];
			pValue[3]=pData[0];
			pValue+=nTypeSize;
			pData+=nTypeSize;
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
	}
	
	m_nPos=m_nPos+nCount*nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkDouble* pDoubles, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkDouble);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pDoubles;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			pValue[0]=pData[7];
			pValue[1]=pData[6];
			pValue[2]=pData[5];
			pValue[3]=pData[4];
			pValue[4]=pData[3];
			pValue[5]=pData[2];
			pValue[6]=pData[1];
			pValue[7]=pData[0];
			pValue+=nTypeSize;
			pData+=nTypeSize;
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
	}
	
	m_nPos=m_nPos+nCount*nTypeSize;
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkPoint* pPoints, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoints;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			Get((YkInt *)pPoints,nCount*2);
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
		m_nPos=m_nPos+nCount*nTypeSize;
	}
	
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkPoint2D* pPoint2Ds, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint2D);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoint2Ds;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			Get((YkDouble *)pPoint2Ds,nCount*2);
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
		m_nPos=m_nPos+nCount*nTypeSize;
	}
	return m_nPos;
}

YkSizeT YkByteArray::Get(YkPoint3D* pPoint3Ds, YkSizeT nCount)
{
	YkSizeT nTypeSize=sizeof(YkPoint3D);
    YKASSERT((m_nPos+nTypeSize*nCount)<=m_array.GetSize());
	YkByte* pData = ((YkByte*)GetData())+m_nPos;
	YkByte* pValue = (YkByte*)pPoint3Ds;
	if(m_bSwap)
	{
		for(YkSizeT i=0;i<nCount;i++)
		{
			Get((YkDouble *)pPoint3Ds,nCount*3);
		}
	}
	else
	{
		memcpy(pValue,pData,nCount*nTypeSize);
		m_nPos=m_nPos+nCount*nTypeSize;
	}
	return m_nPos;
} 

}

