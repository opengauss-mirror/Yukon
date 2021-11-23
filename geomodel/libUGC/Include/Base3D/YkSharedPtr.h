/*
 *
 * YkSharedPtr.h
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

#if !defined(AFX_YKSHAREDPTR_H__E5567E39_00CB_4E7D_9FF6_835268B2D58B__INCLUDED_)
#define AFX_YKSHAREDPTR_H__E5567E39_00CB_4E7D_9FF6_835268B2D58B__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"

namespace Yk {
//! \brief 智能指针模版类
template<class T> 
class YkSharedPtr 
{
public:
	//! \brief 构造函数
	YkSharedPtr() : m_pRep(0), m_pUseCount(0)
	{
	}
	
	//! \brief 模型构造函数
	template< class Y>
	explicit YkSharedPtr(Y* rep) : m_pRep(rep), m_pUseCount(new YkUint(1))
	{
	}

	//! \拷贝构造函数
	YkSharedPtr(const YkSharedPtr& r)
		: m_pRep(0), m_pUseCount(0)
	{
		m_pRep = r.m_pRep;
		m_pUseCount = r.m_pUseCount; 
		if(m_pUseCount)
		{
			++(*m_pUseCount); 
		}
	}
	//! \重载=
	YkSharedPtr& operator=(const YkSharedPtr& r) 
	{
		if (m_pRep == r.m_pRep)
			return *this;

		YkSharedPtr<T> tmp(r);
		Swap(tmp);
		return *this;
	}

	//! \brief 析构
	virtual ~YkSharedPtr() 
	{
		Release();
	}

	//! \brief 重载*
	inline T& operator*() const 
	{
		YKASSERT(m_pRep); 
		return *m_pRep;
	}
	//! \brief 重载->
	inline T* operator->() const
	{
		YKASSERT(m_pRep); 
		return m_pRep;
	}

	//! \brief 获取
	inline T* Get() const 
	{
		return m_pRep; 
	}


	//! \brief 引用的次数
	inline YkUint UseCount() const 
	{
		YKASSERT(m_pUseCount);
		return *m_pUseCount;
	}



	//! \brief 引用对象指针
	inline T* GetPointer() const 
	{ 
		return m_pRep; 
	}

	//! \brief 引用对象是否为空
	inline YkBool IsNull() const 
	{ 
		return m_pRep == 0;
	}


protected:
	//! \释放当前引用的对象
	inline void Release()
	{
		YkBool bDestroyThis = false;

		if (m_pUseCount)
		{
			if (--(*m_pUseCount) == 0) 
			{
				bDestroyThis = true;
			}
		}
		if (bDestroyThis)
		{
			Destroy();
		}
	}

	//! \从内存中删除当前对象
	virtual void Destroy()
	{
		if (m_pRep)
		{
			delete m_pRep;
			m_pRep = NULL;
		}
	
		if (m_pUseCount)
		{
			delete m_pUseCount;
			m_pUseCount = NULL;
		}		
	}

	virtual void Swap(YkSharedPtr<T> &other) 
	{
		std::swap(m_pRep, other.m_pRep);
		std::swap(m_pUseCount, other.m_pUseCount);
	}

protected:
	//! \brief 当引用的对象指针
	T* m_pRep;

	//! \brief 当前对象的引用计数指针
	YkUint* m_pUseCount;
};
}
#endif

