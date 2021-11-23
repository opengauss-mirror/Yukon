/*
 *
 * YkByteArray.h
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

#if !defined(AFX_YKBYTEARRAY_H__ABD143F6_543A_49A1_8D21_E3E5AD753F3E__INCLUDED_)
#define AFX_YKBYTEARRAY_H__ABD143F6_543A_49A1_8D21_E3E5AD753F3E__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"

namespace Yk {

class STREAM_API YkByteArray 
{
public:
	//! \brief 默认构造函数
	YkByteArray();
	//! \brief 默认析构函数
	~YkByteArray();
public:
	
	//! \brief 在 YkByteArray 最后添加一个对象
	//! \param nValue 要添加的对象
	//! \return 返回Position
	//! \remarks 如果原来 Position在其他位置，会自动移到最后的位置。
	YkSizeT Add(YkByte nValue);
	inline YkSizeT Add(YkAchar chValue)
	{
		return Add(YkByte(chValue));
	}
	YkSizeT Add(YkShort nValue);
	inline YkSizeT Add(YkUshort nValue)
	{
		return Add(YkShort(nValue));
	}
	#if _MSC_VER > 1200
	inline YkSizeT Add(YkWchar chvalue)
	{
		return Add(YkShort(chvalue));
	}
	#endif
	YkSizeT Add(YkInt nValue);
	inline YkSizeT Add(YkUint nValue)
	{
		return Add(YkInt(nValue));
	}
	YkSizeT Add(YkLong nValue);
	inline YkSizeT Add(YkUlong nValue)
	{
		return Add(YkLong(nValue));
	}
	YkSizeT Add(YkFloat dValue);
	YkSizeT Add(YkDouble dValue);
	inline YkSizeT Add(const YkString& strValue)
	{
		YKASSERT(strValue.Cstr()!=NULL);
#ifdef _UGUNICODE
		if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
		{
			YkInt nLength=0;
			nLength=strValue.GetLength();
			YKASSERT(nLength>0);
			YkInt nByteLength = nLength * sizeof(YkChar);
			Add(nByteLength);
			return Add((YkUshort*)strValue.Cstr(),nLength);
		}
		else
		{
			YkMBString strMB;
			strMB.SetCharset(m_eCharset);
			strValue.ToMBString(strMB);
			YkInt nLength=0;
			nLength=strMB.GetLength();
			YKASSERT(nLength>0);
			Add(nLength);
			return Add(strMB.Cstr(),nLength);
		}
#else
		Add(strValue.GetLength());
		return Add(strValue.Cstr(),strValue.GetLength());
#endif
	}

	inline YkSizeT Add(const YkPoint& pntValue)
	{
		return Add((YkInt *)&pntValue,2);
	}

	inline YkSizeT Add(const YkPoint2D& pnt2Value)
	{
		return Add((YkDouble *)&pnt2Value,2);
	}

	inline YkSizeT Add(const YkPoint3D& pnt3Value)
	{
		return Add((YkDouble *)&pnt3Value,3);	
	}

	inline YkSizeT Add(const YkRect& rcValue)
	{
		return Add((YkInt *)&rcValue,4);
	}

	inline YkSizeT Add(const YkRect2D& rc2Value)
	{
		return Add((YkDouble *)&rc2Value,4);
	}

	inline YkSizeT Add(const YkSize& szValue)
	{
		return Add((YkInt *)&szValue,2);
	}

	inline YkSizeT Add(const YkSize2D& sz2Value)
	{
		return Add((YkDouble *)&sz2Value,2);
	}
	YkSizeT Add(const YkByte* pBytes, YkSizeT nCount);
	inline YkSizeT Add(const YkAchar* pChars, YkSizeT nCount)
	{
		return Add((YkByte*)pChars,nCount);
	}
	YkSizeT Add(const YkShort *pShorts, YkSizeT nCount);
	inline YkSizeT Add(const YkUshort *pUshorts, YkSizeT nCount)
	{
		return Add((YkShort *)pUshorts,nCount);
	}
	
	inline YkSizeT Add(const YkWchar *pValues, YkSizeT nCount)
	{
		return Add((YkShort *)pValues, nCount);
	}
	
	YkSizeT Add(const YkInt *pInts, YkSizeT nCount);
	inline YkSizeT Add(const YkUint *pUints, YkSizeT nCount)
	{
		return Add((YkInt*)pUints,nCount);
	}

	YkSizeT Add(const YkLong *pLongs, YkSizeT nCount);
	inline YkSizeT Add(const YkUlong *pUlongs, YkSizeT nCount)
	{
		return Add((YkLong*)pUlongs,nCount);
	}
	YkSizeT Add(const YkFloat* pFloats, YkSizeT nCount);
	YkSizeT Add(const YkDouble* pDoubles, YkSizeT nCount);
	YkSizeT Add(const YkPoint* pPoints, YkSizeT nCount);
	YkSizeT Add(const YkPoint2D* pPoint2Ds, YkSizeT nCount);
	YkSizeT Add(const YkPoint3D* pPoint3Ds, YkSizeT nCount); 	
	
	
	
	//! \brief 在Position 位置设置一个对象
	//! \param nValue 要添加的对象
	//! \return 返回Position
	YkSizeT Set(YkByte nValue);
	inline YkSizeT Set(YkAchar chValue)
	{
		return Set(YkByte(chValue));
	}
	YkSizeT Set(YkShort nValue);
	inline YkSizeT Set(YkUshort nValue)
	{
		return Set(YkShort(nValue));
	}
	#if _MSC_VER > 1200
	inline YkSizeT Set(YkWchar nWChar)
	{
		return Set(YkShort(nWChar));
	}
	#endif
	YkSizeT Set(YkInt nValue);
	inline YkSizeT Set(YkUint nValue)
	{
		return Set(YkInt(nValue));
	}
	YkSizeT Set(YkLong nValue);
	inline YkSizeT Set(YkUlong nValue)
	{
		return Set(YkLong(nValue));
	}
	YkSizeT Set(YkFloat dValue);
	YkSizeT Set(YkDouble dValue);
	
	inline YkSizeT Set(const YkString& strValue)
	{
		YKASSERT(strValue.Cstr()!=NULL);
#ifdef _UGUNICODE
		if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
		{
			YkInt nLength=0;
			nLength=strValue.GetLength();
			YKASSERT(nLength>0);
			YkInt nByteLength = nLength * sizeof(YkChar);
			Add(nByteLength);
			return Set((YkUshort*)strValue.Cstr(),nLength);
		}
		else
		{
			YkMBString strMB;
			strMB.SetCharset(m_eCharset);
			strValue.ToMBString(strMB);
			YkInt nLength=0;
			nLength=strMB.GetLength();
			YKASSERT(nLength>0);
			Set(nLength);
			return Set(strMB.Cstr(),nLength);
		}
#else
		YkInt nLength=0;
		nLength=strValue.GetLength();
		YKASSERT(nLength>0);

		Set(nLength);
		return Set(strValue.Cstr(),nLength);
#endif
	}

	inline YkSizeT Set(const YkPoint& pntValue)
	{
		return Set((YkInt*)&pntValue,2);
	}

	inline YkSizeT Set(const YkPoint2D& pnt2Value)
	{
		return Set((YkDouble*)&pnt2Value,2);
	}

	inline YkSizeT Set(const YkPoint3D& pnt3Value)
	{
		return Set((YkDouble*)&pnt3Value,3);
	}

	inline YkSizeT Set(const YkRect& rcValue)
	{
		return Set((YkInt*)&rcValue,4);
	}

	inline YkSizeT Set(const YkRect2D& rc2Value)
	{
		return Set((YkDouble *)&rc2Value,4);
	}

	inline YkSizeT Set(const YkSize& szValue)
	{
		return Set((YkInt*)&szValue,2);
	}

	inline YkSizeT Set(const YkSize2D& sz2Value)
	{
		return Set((YkDouble*)&sz2Value,2);
	}

	
	YkSizeT Set(const YkByte* pBytes, YkSizeT nCount);
    inline YkSizeT Set(const YkAchar* pChars, YkSizeT nCount)
	{
		return Set((YkByte*)pChars,nCount);
	}
	YkSizeT Set(const YkShort *pShorts, YkSizeT nCount);
	inline YkSizeT Set(const YkUshort *pUshorts, YkSizeT nCount)
	{
		return Set((YkShort*)pUshorts,nCount);
	}
	
	inline YkSizeT Set(const YkWchar *pValues, YkSizeT nCount)
	{
		return Set((YkShort*)pValues, nCount);
	}
	
	YkSizeT Set(const YkInt *pInts, YkSizeT nCount);
	inline YkSizeT Set(const YkUint *pUints, YkSizeT nCount)
	{
		return Set((YkInt *)pUints,nCount);
	}
	YkSizeT Set(const YkLong *pLongs, YkSizeT nCount);
	inline YkSizeT Set(const YkUlong *pUlongs, YkSizeT nCount)
	{
		return Set((const YkLong *)pUlongs,nCount);
	}
	YkSizeT Set(const YkFloat* pFloats, YkSizeT nCount);
	YkSizeT Set(const YkDouble* pDoubles, YkSizeT nCount);
	YkSizeT Set(const YkPoint* pPoints, YkSizeT nCount);
	YkSizeT Set(const YkPoint2D* pPoint2Ds, YkSizeT nCount);
	YkSizeT Set(const YkPoint3D* pPoint3Ds, YkSizeT nCount); 	
    
	
	
	//! \brief 在Position 位置插入一个对象，同时原来Position的内容后移，
	//!        有效对象的Size会自动变大
	//! \param nValue 要添加的对象
	//! \return 返回Position
	//! \remarks Position会自动移到插入对象的后面，
	//! \attention 这里面会申请并移动内存，如果失败，可能崩溃，
	//!            无法确定指针的位置，注意要提前保留指针位置
	YkSizeT Insert(YkByte nValue);
	inline YkSizeT Insert(YkAchar chValue)
	{
		return Insert(YkByte(chValue));
	}
	YkSizeT Insert(YkShort nValue);
	inline YkSizeT Insert(YkUshort nValue)
	{
		return Insert(YkShort(nValue));
	}
	
	inline YkSizeT Insert(YkWchar nValue)
	{
		return Insert(YkShort(nValue));
	}
	
	YkSizeT Insert(YkInt nValue);
	inline YkSizeT Insert(YkUint nValue)
	{
		return Insert(YkInt(nValue));
	}
	YkSizeT Insert(YkLong nValue);
	inline YkSizeT Insert(YkUlong nValue)
	{
		return Insert(YkLong(nValue));
	}
	YkSizeT Insert(YkFloat dValue);
	YkSizeT Insert(YkDouble dValue);
	
	inline YkSizeT Insert(const YkString& strValue)
	{
#ifdef _UGUNICODE
		if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
		{
			YkInt nLength=0;
			nLength=strValue.GetLength();
			YkInt nByteLength = nLength * sizeof(YkChar);
			Insert(nByteLength);
			return Insert((YkUshort*)strValue.Cstr(),nLength);
		}
		else
		{
			YkMBString strMB;
			strMB.SetCapacity(m_eCharset);
			strValue.ToMBString(strMB);
			YkInt len = strMB.GetLength();
			Insert(len);
			return Insert(strMB.Cstr(),len);
		}
#else
		YkInt nLength=0;
		nLength=strValue.GetLength();
		Insert(nLength);
		return Insert((const YkByte *)strValue.Cstr(),nLength);
#endif
	}

	inline YkSizeT Insert(const YkPoint& pntValue)
	{
		return Insert((YkInt *)&pntValue,2);
	}

	inline YkSizeT Insert(const YkPoint2D& pnt2Value)
	{
		return Insert((YkDouble *)&pnt2Value,2);
	}

	inline YkSizeT Insert(const YkPoint3D& pnt3Value)
	{
		return Insert((YkDouble *)&pnt3Value,3);
	}

	inline YkSizeT Insert(const YkRect& rcValue)
	{
		return Insert((YkInt*)&rcValue,4);	
	}

	inline YkSizeT Insert(const YkRect2D& rc2Value)
	{
		return Insert((YkDouble *)&rc2Value,4);
	}
	inline YkSizeT Insert(const YkSize& szValue)
	{
		return Insert((YkInt *)&szValue,2);	
	}

	inline YkSizeT Insert(const YkSize2D& sz2Value)
	{
		return Insert((YkDouble *)&sz2Value,2);
	}

	YkSizeT Insert(const YkByte* pBytes, YkSizeT nCount);
    inline YkSizeT Insert(const YkAchar* pChars, YkSizeT nCount)
	{
		return 	Insert((YkByte *)pChars,nCount);
	}

	YkSizeT Insert(const YkShort *pShorts, YkSizeT nCount);
	inline YkSizeT Insert(const YkUshort *pUshorts, YkSizeT nCount)
	{
		return Insert((YkShort*)pUshorts,nCount);
	}

	inline YkSizeT Insert(const YkWchar* pValues, YkSizeT nCount)
	{
		return Insert((YkShort*)pValues, nCount);
	}

	YkSizeT Insert(const YkInt *pInts, YkSizeT nCount);
	inline YkSizeT Insert(const YkUint *pUints, YkSizeT nCount)
	{
		return Insert((YkInt*)pUints,nCount);
	}	

	YkSizeT Insert(const YkLong *pLongs, YkSizeT nCount);
	inline YkSizeT Insert(const YkUlong *pUlongs, YkSizeT nCount)
	{
		return Insert((YkLong*)pUlongs,nCount);
	}
	YkSizeT Insert(const YkFloat* pFloats, YkSizeT nCount);
	YkSizeT Insert(const YkDouble* pDoubles, YkSizeT nCount);
	YkSizeT Insert(const YkPoint* pPoints, YkSizeT nCount);
	YkSizeT Insert(const YkPoint2D* pPoint2Ds, YkSizeT nCount);
	YkSizeT Insert(const YkPoint3D* pPoint3Ds, YkSizeT nCount); 	
	
	
	
	//! \brief 在Position 位置获取一个YkByte对象
	//! \param nValue 获取的YkByte对象
	//! \return 返回Position
	//! \remarks Position会自动移到获取数值的后面。
	//! \attention 如果失败,无法确定指针的位置，注意要提前保留指针位置
	YkSizeT Get(YkByte& nValue);
	YkSizeT Get(YkAchar& chValue);
	YkSizeT Get(YkShort& nValue);

	inline YkSizeT Get(YkWchar& nValue)
	{
		return Get((YkShort&)nValue);
	}

	YkSizeT Get(YkUshort& nValue);
	YkSizeT Get(YkInt& nValue);
	YkSizeT Get(YkUint& nValue);
	YkSizeT Get(YkLong& nValue);
	YkSizeT Get(YkUlong& nValue);
	YkSizeT Get(YkFloat& dValue);
	YkSizeT Get(YkDouble& dValue);
	YkSizeT Get(YkString& strValue);

	inline YkSizeT Get(YkPoint& pntValue)
	{
		return Get((YkInt *)&pntValue,2);
	}

	inline YkSizeT Get(YkPoint2D& pnt2Value)
	{
		return Get((YkDouble *)&pnt2Value,2);
	}

	inline YkSizeT Get(YkPoint3D& pnt3Value)
	{
		return Get((YkDouble *)&pnt3Value,3);
	}
	inline YkSizeT Get(YkRect& rcValue)
	{
		return Get((YkInt *)&rcValue,4);	
	}
	inline YkSizeT Get(YkRect2D& rc2Value)
	{
		return Get((YkDouble *)&rc2Value,4);
	}
	inline YkSizeT Get(YkSize& szValue)
	{
		return Get((YkInt *)&szValue,2);
	}
	inline YkSizeT Get(YkSize2D& sz2Value)
	{
		return Get((YkDouble *)&sz2Value,2);
	}

	YkSizeT Get(YkByte* pBytes, YkSizeT nCount);
    inline YkSizeT Get(YkAchar* pChars, YkSizeT nCount)
	{
		return Get((YkByte *)pChars,nCount);
	}
	YkSizeT Get(YkShort *pShorts, YkSizeT nCount);
	inline YkSizeT Get(YkUshort *pUshorts, YkSizeT nCount)
	{
		return Get((YkShort *)pUshorts,nCount);
	}

	inline YkSizeT Get(YkWchar *pValues, YkSizeT nCount)
	{
		return Get((YkShort *)pValues, nCount);
	}

	YkSizeT Get(YkInt *pInts, YkSizeT nCount);
	inline YkSizeT Get(YkUint *pUints, YkSizeT nCount)
	{
		return Get((YkInt *)pUints,nCount);
	}

	YkSizeT Get(YkLong *pLongs, YkSizeT nCount);
	inline YkSizeT Get(YkUlong *pUlongs, YkSizeT nCount)
	{
		return Get((YkLong*)pUlongs,nCount);
	}

	YkSizeT Get(YkFloat* pFloats, YkSizeT nCount);
	YkSizeT Get(YkDouble* pDoubles, YkSizeT nCount);
	YkSizeT Get(YkPoint* pPoints, YkSizeT nCount);
	YkSizeT Get(YkPoint2D* pPoint2Ds, YkSizeT nCount);
	YkSizeT Get(YkPoint3D* pPoint3Ds, YkSizeT nCount); 	
   

	//! \brief 在Position 位置设置一个对象
	//! \param nValue 要添加的对象
	//! \return 返回Position
	//! \remarks Position会自动移到设置数值的后面。
	//!          如果原来Position后有内容，会被覆盖。如果空间不够会自动增长，
	//!          如果设置自动增长量,Capacity 按照自动增长量增长，
	//!          如果没有设置自动增长量，Capacity会以*2方式相应的大小，
	//!          建议大家设置自动增长量
	//! \attention 如果字节流长度不够，这里面会申请内存，如果失败，可能崩溃，
	//!            无法确定指针的位置，注意要提前保留指针位置

	inline YkByteArray& operator<<(YkByte nValue)
	{
		Set(nValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkAchar chValue)
	{
		Set(chValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkShort nValue)
	{
		Set(nValue);
		return *this;
	}

	inline YkByteArray& operator<<(YkWchar nValue)
	{
		Set(nValue);
		return *this;
	}

	inline YkByteArray& operator<<(YkUshort nValue)
	{
		Set(nValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkUint nValue)
	{
		Set(nValue);
		return *this;
	}

	inline YkByteArray& operator<<(YkInt nValue)
	{
		Set(nValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkLong nValue)
	{
		Set(nValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkUlong nValue)
	{
		Set(nValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkFloat dValue)
	{
		Set(dValue);
		return *this;
	}
	inline YkByteArray& operator<<(YkDouble dValue)
	{
		Set(dValue);
		return *this;
	}
	inline YkByteArray& operator<<(const YkString &strValue)
	{
		Set(strValue);	
		return *this;
	}
	inline YkByteArray& operator<<(const YkPoint& pntValue)
	{
		Set(pntValue);
		return *this;
	}
	inline YkByteArray& operator<<(const YkPoint2D& pnt2Value)
	{
		Set(pnt2Value);
		return *this;
	}
	inline YkByteArray& operator<<(const YkPoint3D& pnt3Value)
	{
		Set(pnt3Value);
		return *this;
	}
	inline YkByteArray& operator<<(const YkRect& rcValue)
	{
		Set(rcValue);
		return *this;
	}
	inline YkByteArray& operator<<(const YkRect2D& rc2Value)
	{
		Set(rc2Value);
		return *this;
	}
	inline YkByteArray& operator<<(const YkSize &szValue)
	{
		Set(szValue);
		return *this;
	}
	inline YkByteArray& operator<<(const YkSize2D& sz2Value)
	{
		Set(sz2Value);
		return *this;
	}

	inline YkByteArray& operator>>(YkByte&  nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkAchar& chValue)
	{
		Get(chValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkShort& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkWchar& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkUshort& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkUint& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkInt& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkLong& nValue)
	{
		Get(nValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkUlong& nvalue)
	{
		Get(nvalue);
		return *this;
	}

	inline YkByteArray& operator>>(YkFloat& dValue)
	{
		Get(dValue);
		return *this;
	}

	inline YkByteArray& operator>>(YkDouble& dValude)
	{
		Get(dValude);
		return *this;
	}

	inline YkByteArray& operator>>(YkString &strValue)
	{
		Get(strValue);
		return *this;
	}
	inline YkByteArray& operator>>(YkPoint& pntValue)
	{
		Get(pntValue);
		return *this;
	}
	inline YkByteArray& operator>>(YkPoint2D& pnt2Value)
	{
		Get(pnt2Value);
		return *this;
	}
	inline YkByteArray& operator>>(YkPoint3D& pnt3Value)
	{
		Get(pnt3Value);
		return *this;
	}
	inline YkByteArray& operator>>(YkRect& rcValue)
	{
		Get(rcValue);
		return *this;
	}
	inline YkByteArray& operator>>(YkRect2D& rc2Value)
	{
		Get(rc2Value);
		return *this;
	}
	inline YkByteArray& operator>>(YkSize &szValue)
	{
		Get(szValue);
		return *this;
	}
	inline YkByteArray& operator>>(YkSize2D& sz2Value)
	{
		Get(sz2Value);
		return *this;
	}

	//! \brief 获取当前Position
	//! \return 返回Position
	inline YkSizeT GetPosition() const
	{
		return m_nPos;
	}
	
	//! \brief 设置 Position
	//! \param nPos 设置的位置
	//! \return 返回实际设置的Position
	//! \remarks 如果 nPos<0 Position=0 如果 nPos>字节流大小，Position 会在最后的位置
	inline YkSizeT SetPosition(YkSizeT nPos)
	{
		m_nPos=nPos;
		if(m_nPos>m_array.GetSize())
			m_nPos=m_array.GetSize();
		if(m_nPos<0)
			m_nPos=0;
		return m_nPos;

	}
	
	//! \brief 拷贝YkByteArray
	//! \param aryByte 被拷贝的 YkByteArray
	//! \remarks 拷贝后的Position和aryByte的Position位置相同
	inline void Copy(const YkByteArray& aryByte)
	{
		m_array.Copy(aryByte.m_array);
		m_nPos=aryByte.GetPosition();
		m_bSwap = aryByte.SwapBytes();
	}
	
	//! \brief 从 Position 开始 移除nCount个元素,同时有效对象的Size变小
	//! \param nCount 要移除的元素个数
	//! \return 返回实际移除元素的个数
	//! \remarks 指定位置必须在字节流范围之内，如果指定的个数过大,会移除Position后的所有有效对象
	inline YkSizeT Remove(YkSizeT nCount)
	{
		return m_array.RemoveAt(m_nPos,nCount);
	}
	
	//! \brief 移除所有元素
	//! \remarks Position 置零
	inline void RemoveAll()
	{
		m_nPos=0;
		m_array.RemoveAll();
	}
	
	//! \brief 调整YkByteArray的有效数据的内存空间为 nSize,
	//! \remarks 如果原有有效数据的空间不够, 内存空间会增加, 如果指定自动增长量，
	//!          同时按照自动增长量的大小来增长Capacity 
	//!          如果原有空间比指定的大, 不会释放空间,
	//!          但 在指定的空间之后的元素就无效了
	//!          如果Position大于nSize ,Position为nSize;
	//! \param nSize 指定的新的元素个数
    inline void SetSize(YkSizeT nSize)
	{
		if(m_nPos>nSize)
			m_nPos=nSize;
		m_array.SetSize(nSize);
	}

	//! \brief 得到有用的元素的个数
	//! \return 有用的元素的个数
	inline YkSizeT GetSize() const
	{
		return m_array.GetSize();
	}
	
	//! \brief 设置自动增长量
	//! \param nGrowSize 自动增长量
	//! \remarks 如果不调用此函数，默认的Capacity自动增长量是乘以2，调用此函数，如果
	//           add的时候，超过数组的容量，会按照自动增长量的大小增加
	inline void SetGrowSize(YkInt nGrowSize = 10)
	{
		m_array.SetGrowSize(nGrowSize);
	}
	
	//! \brief 获取自动增长量
	//! \return 获取的自动增长量
	inline YkSizeT GetGrowSize() const
	{
		return m_array.GetGrowSize();
	}

	//! \brief 确保YkByteArray有足够的内存空间存储 指定的元素个数
	//! \param nCapacity 指定可容纳的元素个数,必须大于等于0
	//! \remarks 如果原有空间不够, 内存空间会增加
	//!          如果原有空间比指定的大, 也不会释放空间，Capacity的大小也不会有所变化。
	//!          如果 当前Position 大于有用元素的个数，Position在有用元素的最后。
	inline void SetCapacity(YkSizeT nCapacity)
	{
		m_array.SetCapacity(nCapacity);
	}

	
	//! \brief 得到目前YkArray中所有空间(包括已经使用的和保留的)所能容纳的元素的个数
	//! \return 返回数组所有空间可容纳的元素个数
	inline YkSizeT GetCapacity() const
	{
		return m_array.GetCapacity();
	}
	
	//! \brief 得到数组内部指针(地址连续)
	//! \return 返回数组内部指针(地址连续)
	inline const YkByte* GetData() const
	{
		return m_array.GetData();
	};

	inline void SetSwapBytes(YkBool s) {m_bSwap = s; }

	//! \brief 得到是否需要进行字节顺序的交换。
	//! \remarks 数据采用LittleEndian方式存储，在BigEndian的CPU中需要进行字节顺序交换。
	inline YkBool SwapBytes() const
	{
		return m_bSwap;
	}

	void SetCharset(YkString::Charset eCharset)
	{
		m_eCharset = eCharset;
	}
	YkString::Charset GetCharset()
	{
		return m_eCharset;
	}
private:
	YkArray<YkByte> m_array;
	YkSizeT m_nPos;

	YkBool m_bSwap;

	// 考虑到Stream中要操作字符串，就需要考虑字符串的编码问题，增加该变量来控制对Stream
	YkString::Charset m_eCharset;

};

}

#endif

