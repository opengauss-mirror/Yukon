﻿/*
 *
 * YkArray.h
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

#if !defined(AFX_YKARRAY_H__1058166B_E59F_4206_869F_109ECDC3EDE2__INCLUDED_)
#define AFX_YKARRAY_H__1058166B_E59F_4206_869F_109ECDC3EDE2__INCLUDED_

#pragma once

#include "Base/YkDefs.h"
#include <vector>
#include <algorithm>

namespace Yk {

//! \brief 数组类
//内部采用STL的vector实现
template<typename T>
class YKEXPORT YkArray
{
public:
	//! \brief 默认构造函数，构造一个长度为零的数组
	YkArray() 
	{
		m_nGrowSize=0;
	}	

	//! \brief 构造函数，构造长度为nSize的数组
	//! \param nSize 数组长度，必须大于等于0
	//explicit用来放置隐形的从整型转化为数组类型
	explicit YkArray(YkSizeT nSize) 
	{
		YKASSERT(nSize>=0);
		m_nGrowSize=0;   
		m_array.resize(nSize);
	}

	//! \brief 构造长度为nSize的数组,数组元素的值为elem中的枚举值
	//! \param nSize 数组长度, 必须大于等于0
	//! \param elem 数组元素的默认值
	YkArray(YkSizeT nSize, const T& elem)
	{
		YKASSERT(nSize>=0);
		m_nGrowSize=0;  
		m_array.resize(nSize,elem);
	}

	YkArray(const std::vector<T> &vect) : m_array(vect.size())
	{
		std::copy(vect.begin(), vect.end(), m_array.begin());
	}

public:
	//! \brief 获取数组的长度
	//! \return 返回数组的长度，返回值类型为整型。
	YkSizeT GetCapacity() const
	{
		return (YkSizeT)m_array.capacity();
	}

	//! \brief 设置数组的长度
	//! \param nCapacity 数组的长度,必须大于等于0
	//! \remarks 如果原有空间不够, 内存空间会增加, 但增加的空间不会调用TYPE的构造函数。
	//! 如果原有空间比指定的大, 也不会释放空间。
	void SetCapacity(YkSizeT nCapacity)
	{
		YKASSERT(nCapacity >= 0);
		m_array.reserve(nCapacity);
	}

	//! \brief 在数组末尾增加一个元素，且数组长度增加1.
	//! \param newElement 新增元素
	//! \return 返回新增元素的索引值
	//! \remarks 若添加元素后，超过数组的容量，则数组长度会自动按照自动增长量来增长。
	YkSizeT Add(const T& newElement)
	{    
		if((m_array.size()>=m_array.capacity())&&(m_nGrowSize>0)) 
			m_array.reserve(m_array.capacity()+m_nGrowSize);      
		m_array.push_back(newElement);
		return (YkSizeT)m_array.size()-1;
	}

	//! \brief 设置自动增长量
	//! \param nGrowSize 自动增长量
	//! \remarks 设置数组自动增长量后，若数据添加元素后，长度超过数组的容量，会按照自动增长量增加数组容量
	void SetGrowSize(YkSizeT nGrowSize=10)
	{
		m_nGrowSize=nGrowSize;
	}

	//! \brief 获取自动增长量
	//! \return 返回数组的自动增长量，返回值类型为整型。
    YkSizeT GetGrowSize() const
	{   
		return m_nGrowSize;
	}

	//! \brief 在数组末尾追加元素
	//! \param pData 要追加的元素
	//! \param nSize 要追加的元素个数
	//! \return 返回加入元素的起始索引
	YkSizeT Append(const T* pData, YkSizeT nSize)
	{
		YkSizeT nOldSize=(YkSizeT)m_array.size();
		m_array.insert(m_array.end(),pData,pData+nSize);
		return nOldSize;
	}

	//! \brief 在数组末尾追加数组。
	//! \brief src 要追加的数组
	//! \return 返回追加数组的起始索引，返回值类型为整型。
	YkSizeT Append(const YkArray<T>& src)
	{
		YkSizeT nOldSize=(YkSizeT)m_array.size();
		m_array.insert(m_array.end(),src.m_array.begin(),src.m_array.end());
		return nOldSize;
	}

	//! \brief 拷贝数组
	//! \param src 传入的数组
	//! \remarks 复制一个数组的元素到另一个数组中。若原有数组中有值，则会被覆盖。	
	void Copy(const YkArray<T>& src)
	{
		if(this == &src)
			return ;

		*this=src;
	}

	//! \brief 得到数组内部指针(地址连续)
	//! \return 返回数组内部指针(地址连续)
	const T* GetData() const
	{
		if (GetSize() > 0)
		{
			return &(*m_array.begin());
		}
		else 
		{
			return NULL;
		}
	}

	//! \brief 得到数组内部指针(地址连续)
	//! \return 返回数组内部指针(地址连续)
	T* GetData()
	{
		if (GetSize() > 0)
		{
			return &(*m_array.begin());
		}
		else 
		{
			return NULL;
		}
	}

	//! \brief 获取最大的可用下标值 
	//! \return 返回最大可用下标值
	//! \remarks 如果数组为空, 则返回 -1
	YkSizeT GetUpperBound() const
	{
		return (YkSizeT)m_array.size()-1;
	}
		
	//! \brief 获取数组内的元素个数
	YkSizeT GetSize() const
	{
		return (YkSizeT)m_array.size();
	}

	//! \brief 判断是否为空
	//! \return 为空返回true;不为空返回false
	YkBool IsEmpty() const
	{
		return m_array.empty();	
	}

	//! \brief 释放多余的内存空间
	//! \attention 内部采用拷贝创建一个相同的数组,然后交换的方式实现的,
	//! 因此会破坏之前的内存, 导致之前GetData的指针变为非法, 故而不推荐使用.
	void FreeExtra()
	{
		if(m_array.capacity()>m_array.size()) {
			std::vector<T> vctTemp(m_array.begin(),m_array.end());
			m_array.swap(vctTemp);
		}
	}

	//! \brief 得到数组内指定位置元素的引用
	//! \param nIndex 指定的位置
	//! \return 返回该位置元素的引用
	T& ElementAt(YkSizeT nIndex)
	{
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		return m_array.at(nIndex);
	}

	//! \brief 得到数组中指定位置元素的const引用
	//! \param nIndex 指定的位置
	//! \return 返回该位置元素的const引用
	const T& ElementAt(YkSizeT nIndex) const
	{
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		return m_array.at(nIndex);
	}

	//! \brief 得到数组内指定位置元素的引用
	//! \param nIndex 指定的位置
	//! \return 返回该位置元素的引用
	T& operator[](YkSizeT nIndex)
	{
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		return m_array.at(nIndex);
	}

	//! \brief 得到数组中指定位置元素的const引用
	//! \param nIndex 指定的位置
	//! \return 返回该位置元素的const引用
	const T& operator[](YkSizeT nIndex) const
	{
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		return m_array.at(nIndex);
	}
	
	//! \brief 得到指定位置元素值
	//! \param nIndex 指定的位置/索引值
	//! \return 返回该位置元素的拷贝
	T GetAt(YkSizeT nIndex) const
	{				
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		return m_array.at(nIndex);
	}
	
	//! \brief 在指定位置设置元素值(替换原有元素)
	//! \remarks 指定位置必须在数组长度范围之内
	//! \param nIndex 指定的位置 
	//! \param newElement 要设置的元素
	void SetAt(YkSizeT nIndex, const T& newElement)
	{			
		YKASSERT(nIndex>=0 && nIndex<(YkSizeT)m_array.size());
		m_array.at(nIndex) = newElement;
	}
	
	//! \brief 在指定位置设置元素(替换原有元素)
	//! \remarks 如果指定位置大于数组长度,数组会自动增长
	//! \param nIndex 指定的位置 
	//! \param newElement 要设置的元素
	void SetAtGrow(YkSizeT nIndex,const T& newElement)
	{
		YKASSERT(nIndex >= 0);		
		if(nIndex>=(YkSizeT)m_array.size())
		{
			m_array.resize(nIndex+1);
		}
		m_array.at(nIndex) = newElement;
	}
	
	//! \brief 设置数组内可容纳的元素数量
	//! \param nNewSize 指定的新的元素个数
	//! \remarks 如果原有空间不够, 内存空间会增加, 
	//! 增加的空间会调用TYPE的默认构造函数(或等于newElement)。
	//! 如果原有空间比指定的大, 不会释放空间,但在指定的空间之后的元素就无效了。
	//! 如果要强制释放内存,请调用FreeExtra(),释放多余的内存空间。
	void SetSize(YkSizeT nNewSize)
	{
		YKASSERT(nNewSize>=0);
		if((nNewSize>(YkSizeT)m_array.capacity()) && (m_nGrowSize>0)) 
			m_array.reserve(m_array.capacity()+(nNewSize/m_nGrowSize+1)*m_nGrowSize);// 确保能够保证按照自动增长量增长
		m_array.resize(nNewSize);
	}	
	
	//! \brief 移除所有元素,同时释放内存空间
	void RemoveAll()
	{
		m_array.clear(); 		
		FreeExtra();
	}

	//! \brief 移除数组中指定位置,指定个数的元素
	//! \remarks 指定位置必须在数组范围内
	//! \param nIndex 要移除的元素位置
	//! \param nCount 要移除的元素个数,默认为1
	//! \return 返回移除的元素个数
	YkSizeT RemoveAt(YkSizeT nIndex,YkSizeT nCount=1)
	{
		YKASSERT(nIndex>=0);
		nCount = YKMAX(nCount,0);
		if(nCount == 0)
		{
			return 0;
		}

		YkSizeT nSize=(YkSizeT)m_array.size();
		if(nIndex >= nSize)
		{
			return 0;
		}

		YkSizeT nEnd=nIndex+nCount;
		if(nEnd>nSize)
		{
			nEnd=nSize;
		}
		m_array.erase(m_array.begin()+nIndex,m_array.begin()+nEnd);
		return nEnd - nIndex;
	}
	
	//! \brief 在数组中指定位置之前，插入指定个数的元素
	//! \param nIndex 指定的位置
	//! \param newElement 指定的元素
	//! \param nCount 要插入的元素个数，默认为1
	//! \remarks 如果指定位置不在数组范围内，会自动调整到[0,size]
	void InsertAt(YkSizeT nIndex,const T& newElement,YkSizeT nCount=1)
	{				
		YkSizeT nSize=(YkSizeT)m_array.size();
		nIndex=YKCLAMP(0,nIndex,nSize);
		if(nCount>0) 
		{
			m_array.insert(m_array.begin()+nIndex,nCount,newElement);
		}				
	}

	//! \brief 在数组中的指定位置之前，插入指定个数的元素
	//! \param nIndex 指定的位置
	//! \param pNewElement 指定的元素指针
	//! \param nCount 要插入的元素个数
	//! \remarks 如果指定位置不在数组范围内，会自动调整到[0,size]
	void InsertAt(YkSizeT nIndex,const T* pNewElement,YkSizeT nCount)
	{				
		YkSizeT nSize=(YkSizeT)m_array.size();
		nIndex=YKCLAMP(0,nIndex,nSize);
		if(nCount>0 && pNewElement) 
		{
			m_array.insert(m_array.begin()+nIndex,pNewElement,pNewElement+nCount);
		}				
	}
	
	//! \brief 在指定位置，插入一个新的数组
	//! \param nIndex 指定的位置
	//! \param newArray 要插入的数组
	//! \remarks 如果指定位置不在数组范围内，会自动调整到[0,size]
	void InsertAt(YkSizeT nIndex, const YkArray<T>& newArray)
	{		
		YkSizeT nSize=(YkSizeT)m_array.size();
		nIndex=YKCLAMP(0,nIndex,nSize);
		m_array.insert(m_array.begin()+nIndex,newArray.m_array.begin(),newArray.m_array.end());
	}

	//! \brief 得到内部STL类型, 外部一般不要使用
	//! \return 返回内部STL类型
	std::vector<T>& Inner()
	{
		return m_array;
	}

private:
	//! \brief 内部采用STL的vector实现
	std::vector<T> m_array;
	//! \brief 数组的自动增长量
	YkSizeT m_nGrowSize;
};

}

#endif

