/*
 *
 * YkList.h
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

#if !defined(AFX_YKLIST_H__E90FD10D_6701_47C1_8726_BD749DE72C09__INCLUDED_)
#define AFX_YKLIST_H__E90FD10D_6701_47C1_8726_BD749DE72C09__INCLUDED_

#pragma once

#include "Base/YkDefs.h"
#include <list>
#include <algorithm>

namespace Yk {

//! \brief 链表模板类
//接口参考MFC的CList,内部采用stl的list实现
template<typename T>
class YKEXPORT YkList
{
public:
	//typedef typename std::list<T>::iterator POSITION;
	//! \brief 用来指示元素在列表中位置的值
	typedef typename std::list<T>::iterator STDPosition;
	//! \brief 在YkList中定位用的类,不能直接构造,必须通过YkList来进行操作.
	struct POSITION
	{
		friend class YkList<T>;
	public:
		POSITION() {
			bIsBOF = false;
		}
	private:
		STDPosition it;
		// 是否已到链表尾
		// YkBool bIsEOF;
		//! \brief 是否已到链表头
		YkBool bIsBOF; 

		POSITION(STDPosition pos, YkBool bisBOF=false){
			this->it = pos;
			this->bIsBOF = bisBOF;
		}
	};

public:
	//! \brief 默认构造函数
	YkList()
	{
	}

	//! \brief 从 std::list 进行构造的函数
	YkList(const std::list<T> &lst) : m_list(lst.size())
	{
		std::copy(lst.begin(), lst.end(), m_list.begin());
	}
	
	//! \brief 得到元素个数
	YkInt GetCount() const
	{
		return (YkInt)m_list.size();
	}

	//! \brief 判断是否为空
	//! \return 为空返回true;不为空返回false
	YkBool IsEmpty() const
	{
		return m_list.empty();	
	}

	//! \brief 得到链表头的元素
	//! \return T指定了链表元素的类型，由调用者指定。
	T& GetHead()
	{
		YKASSERT(!IsEmpty());
		return m_list.front();
	}

	//! \brief 得到链表头的元素
	//! \return T指定了链表元素的类型，由调用者指定。
    const T& GetHead() const
	{
		YKASSERT(!IsEmpty());
		return m_list.front();
	}

	//! \brief 得到链表尾的元素
	//! \return T指定了链表元素的类型，由调用者指定。
    T& GetTail()
	{
		YKASSERT(!IsEmpty());
		return m_list.back();
	}

	//! \brief 得到链表尾的元素
	//! \return T指定了链表元素的类型，由调用者指定。
    const T& GetTail() const
	{
		YKASSERT(!IsEmpty());
		return m_list.back();
	}

	//! \brief 移除链表头的元素
	//! \return 返回被移除的元素。T指定了链表元素的类型，由调用者指定。
	T RemoveHead()
	{
		YKASSERT(!IsEmpty());
		T f=m_list.front();
		m_list.pop_front();
		return f;
	}

	//! \brief 移除尾元素
	//! \return 返回被移除的元素。T指定了链表元素的类型，由调用者指定。
    T RemoveTail()
	{
		YKASSERT(!IsEmpty());
		T t=m_list.back();
		m_list.pop_back();
		return t;
	}

	//! \brief 在链表头添加元素
	//! \param newElement 被添加的元素
	//! \return 返回添加元素在链表中的位置(Position)。
	POSITION AddHead(const T& newElement)
	{
		m_list.push_front(newElement);
		return m_list.begin();
	}

	//! \brief 在链表尾添加元素
	//! \param newElement 被添加的元素
	//! \return 返回添加元素在链表中的位置(Position)。
    POSITION AddTail(const T& newElement)
	{
		m_list.push_back(newElement);
		return --m_list.end();
	}

	//! \brief 在链表头添加一个链表
	//! \param newList 被添加的链表
	void AddHead(const YkList<T>& newList)
	{
		m_list.insert(m_list.begin(),newList.m_list.begin(),newList.m_list.end());
	}

	//! \brief 在链表尾部增加一个链表
	//! \param newList 要被添加的链表
    void AddTail(const YkList<T>& newList)
	{
		m_list.insert(m_list.end(),newList.m_list.begin(),newList.m_list.end());
	}
	
	//! \brief 移除链表中所有元素
	void RemoveAll()
	{
		m_list.clear();
	}

	//! \brief 得到链表头的位置
	//! \return 若返回为空，则说明链表为空
	POSITION GetHeadPosition() const
	{				
		return POSITION( (const_cast< std::list<T>& >(m_list)).begin());
	}

	//! \brief 得到链表尾的位置
	//! \return 若返回为空，则说明链表为空
    POSITION GetTailPosition() const
	{
		STDPosition itEnd = const_cast<std::list<T>&>(m_list).end();
		if (GetCount() == 0) {
			return POSITION(itEnd, true);
		}
		return POSITION(--itEnd);
	}

	//! \brief 判断是否已到链表尾
	//! \param pos 要判断的位置
	//! \return 已到链表尾返回true;否则返回false
	YkBool IsEOF(POSITION pos) const
	{
		return pos.it==const_cast<std::list<T>&>(m_list).end();
	}

	//! \brief 判断某位置是否已到链表头
	//! \param pos 要判断的位置
	//! \return 已到链表头返回true;否则返回false
	YkBool IsBOF(POSITION pos) const
	{
		return pos.bIsBOF 
			|| (++pos.it==const_cast<std::list<T>&>(m_list).begin());
	}
	
	//! \brief 获取链表中的下一个位置
	//! \param pos[in][out] 链表中的指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。
	//! \remarks pos对应的位置不能是链表头或者链表尾
	T& GetNext(POSITION& pos)
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		return *pos.it++;
	}

	//! \brief 获取链表中的下一个位置
	//! \param pos[in][out] 链表中的指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。
    const T& GetNext(POSITION& pos) const
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		return *pos.it++;
	}

	//! \brief 得到链表中的上一个位置
	//! \param pos[in][out] 链表中的指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。
	//! \remarks pos对应的位置不能是链表头或者链表尾
    T& GetPrev(POSITION& pos)
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		if (pos.it == m_list.begin()) 
		{
			pos.bIsBOF = true; 
			return *pos.it;
		}
		return *pos.it--;
	}

	//! \brief 得到链表中的上一个位置
	//! \param pos[in][out] 链表中的指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。
	//! \remarks pos对应的位置不能是链表头或者链表尾
    const T& GetPrev(POSITION& pos) const
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		if (pos.it == m_list.begin()) 
		{
			pos.bIsBOF = true; 
			return *pos.it;
		}
		return *pos.it--;
	}
	
	//! \brief 得到链表中指定位置的元素
	//! \param pos 指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。	
	T& GetAt(POSITION pos)
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		return *pos.it;
	}

	//! \brief 得到链表中指定位置的元素
	//! \param pos 指定位置
	//! \return 返回传入位置的元素的引用,T指定了链表元素的类型，由调用者指定。	
    const T& GetAt(POSITION pos) const
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		return *pos.it;
	}

	//! \brief 设置链表中指定位置的元素值
	//! \param pos 链表中的指定位置
	//! \param newElement 要设置的元素的新值
	void SetAt(POSITION pos, const T& newElement)
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		*pos.it=newElement;
	}

	//! \brief 移除链表中指定位置的元素
	//! \param pos 指定的位置
    void RemoveAt(POSITION& pos)
	{
		YKASSERT(!(IsEOF(pos) || IsBOF(pos)));
		pos.it = m_list.erase(pos.it);
	}
	
	//! \brief 在链表中指定位置之前插入一个元素
	//! \param pos 要插入的位置
	//! \param newElement 要插入的元素
	//! \return 返回被插入元素在链表中的位置
	POSITION InsertBefore(POSITION pos,const T& newElement)
	{
		YKASSERT(!IsBOF(pos));
		return m_list.insert(pos.it,newElement);
	}

	//! \brief 在链表中指定位置之后插入一个元素
	//! \param pos 要插入的位置
	//! \param newElement 要插入的元素
	//! \return 返回被插入元素在链表中的位置
    POSITION InsertAfter(POSITION pos,const T& newElement)
	{
		YKASSERT(!IsEOF(pos));
		return m_list.insert(++pos.it,newElement);
	}

	//! \brief 根据元素值查找元素所在位置（从链表头开始查找）
	//! \param searchValue 要查找的元素
	//! \return 返回第一个找到的元素的位置
	POSITION Find(const T& searchValue) const
	{
		return std::find(const_cast<std::list<T>&>(m_list).begin(),
			const_cast<std::list<T>&>(m_list).end(),searchValue);
	}

	//! \brief 根据元素值查找元素所在位置（从指定位置之后开始查找，指定位置不被查找）。
	//! \param searchValue 要查找的元素
	//! \param startAfter 指定查找的位置
	//! \return 返回第一个找到的元素的位置
    POSITION Find(const T& searchValue, POSITION pos) const
	{
		YKASSERT(!IsEOF(pos));
		return std::find(++pos.it,const_cast<std::list<T>&>(m_list).end(),searchValue);
	}
	
	//! \brief 得到指定索引的元素的位置
	//! \param nIndex 指定的查找开始的索引
	//! \return 返回找到的元素的位置; 如果整个链表都未找到,则返回尾位置
    POSITION FindIndex(YkInt nIndex) const
	{
		POSITION pos(const_cast<std::list<T>&>(m_list).begin());
		for(YkInt i=0;i<nIndex;i++)
		{
			if(++pos.it==const_cast<std::list<T>&>(m_list).end())
				break;
		}
		return pos;
	}

private:
	//! \brief 内部实现采用stl的list
	std::list<T> m_list;
};

}

#endif

