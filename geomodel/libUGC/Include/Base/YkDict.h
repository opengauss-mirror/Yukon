/*
 *
 * YkDict.h
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

#if !defined(AFX_YKDICT_H__EA6AD17F_452A_4C4B_9FA0_F79418967EB8__INCLUDED_)
#define AFX_YKDICT_H__EA6AD17F_452A_4C4B_9FA0_F79418967EB8__INCLUDED_

#pragma once

#include "Base/YkDefs.h"

namespace Yk {

#define YkLess std::less
#define YkGreater std::greater

//! \brief 字典类。
//! 类似于MFC中的CMap,内部采用stl的map实现。
template<typename Key, typename T, class Compare = std::less<Key> >
class YKEXPORT YkDict 
{
public:
	//! \brief 用来指示元素在字典中位置的值
	typedef typename std::map<Key,T,Compare>::const_iterator POSITION;
	
public:
	//! \brief 默认构造函数。
	YkDict()	
	{

	}

	//! \brief 得到元素个数。
	//! \return 返回元素个数。
	YkInt GetCount() const	
	{
		return (YkInt)m_dict.size();
	}

	//! \brief 判断是否为空。
	//! \return 空返回true;非空返回false。
	YkBool IsEmpty() const	
	{
		return m_dict.empty();
	}

	//! \brief 从另一个字典拷贝。
	//! \param src 用于拷贝的字典[in]。
	void Copy(const YkDict<Key,T,Compare>& src)	
	{
		*this=src;
	}

	//! \brief 查找指定rkey的value元素。
	//! \param rkey 要查找的key[in]。
	//! \param rValue 得到的元素[out]。
	//! \return 查找成功返回true;不成功返回false。
	YkBool Lookup(const Key& rkey,T& rValue) const	
	{
		POSITION it=m_dict.find(rkey);		
		if(it!=m_dict.end())
		{
			rValue = it->second;
			return TRUE;
		}
		return FALSE;
	}

	//! \brief 通过rkey直接查找到元素的指针。
	//! \param rkey 要查找的key值[in]。
	//! \return 找到则返回指向UGDict内部的元素的指针, 不是拷贝;若没查到, 则返回空指针。
	const T* Lookup(const Key &rkey) const	
	{
		POSITION it=m_dict.find(rkey);		
		if(it!=m_dict.end())
		{
			return (const T*)(&it->second);
		}
		return NULL;
	}	

	//! \brief 给指定的rkey设置指定的元素。
	//! \param rkey 指定的键值[in]。
	//! \param newValue 指定的元素[in]。
	void SetAt(const Key& rkey,const T& newValue)	
	{
		operator[](rkey)=newValue;
	}

	//! \brief 通过指定的rkey得到对应元素的引用。
	//! \param rkey 指定的rkey值[in]。
	//! \return 返回对应元素的引用。
	const T& operator[](const Key& rkey) const	
	{
		return m_dict.operator [](rkey);
	}

	//! \brief 通过指定的rkey得到对应元素的引用。
	//! \param rkey 指定的key值[in]。
	//! \return 返回对应元素的引用。
	T& operator[](const Key& rkey) 
	{
		return m_dict.operator [](rkey);
	}

	//! \brief 移除rkey值对应的元素。
	//! \param rkey 要移除的key值[in]。
	//! \return 成功返回true;失败返回false。
	YkBool RemoveKey(const Key& rkey)	
	{
		return m_dict.erase(rkey)>0;
	}

	//! \brief 移除所有元素
	void RemoveAll()
	{
		m_dict.clear();
	}

	//! \brief 得到开始位置。
	//! \return 返回开始位置。
	//! \remarks 得到开始位置后,再通过GetNextAssoc函数就可以一个接着一个得到字典中所有的元素
	//! \attention 注意使用IsEOF函数判断不要超过字典的范围
	POSITION GetStartPosition() const	
	{
		return m_dict.begin();
	}

	//! \brief 判断传入的位置是否是字典尾。
	//! \param pos 要判断的位置[in]。
	//! \return 如果已到字典尾则发挥true;否则返回false。
	YkBool IsEOF(POSITION pos) const	
	{
		return pos==m_dict.end();
	}

	//! \brief 得到指定pos的rkey和元素,并把pos移动到下一个位置
	//! \param rNextPosition 指定的位置[in]。
	//! \param rKey 得到的key[out]。
	//! \param rValue 得到的元素[out]。
	void GetNextAssoc(POSITION& rNextPosition,Key& rKey,T& rValue) const	
	{
		YKASSERT(!IsEOF(rNextPosition));		
		rKey = rNextPosition->first;
		rValue = rNextPosition->second;
		++rNextPosition;
	}

private:
	std::map<Key,T,Compare> m_dict;
};

}

#endif

