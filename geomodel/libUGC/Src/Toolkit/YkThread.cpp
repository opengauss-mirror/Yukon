/*
 *
 * YkThread.cpp
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

#include "Toolkit/YkThread.h"
#include "Toolkit/YkToolkit.h"
#ifdef WIN32
#include <Windows.h>
#endif

namespace Yk {

	YkThreadManager::YkThreadManager():m_mutex(TRUE)
	{

	}
	YkThreadManager::~YkThreadManager()
	{
		YkInt i=0;
		YkThreadEx* pThread = NULL;
		for(i=0;i<(YkInt)m_arrayThread.GetSize();i++)
		{
			pThread = m_arrayThread[i];
			if(pThread != NULL)
			{
				delete pThread;
			}
		}
		m_arrayThread.RemoveAll();
	}

	//获取工厂类的唯一实例
	YkThreadManager& YkThreadManager::GetInstance()
	{
		static YkThreadManager g_ThreadManager;
		return g_ThreadManager;
	}
	//!  \brief 创建一个线程，根据线程名字，线程函数，线程函数参数，是否托管
	//!  isManaged 主要线程如果设置为托管，则在线程结束的时候自动调用YkThreadFactory 类删除自己
	YkThreadEx* YkThreadManager::CreateThread(const YkString& strName,ThreadFunCallBack pFun,void* pParameter,YkBool isManaged/* =FALSE*/)
	{
		if(strName.IsEmpty() || pFun == NULL)
		{
			return NULL;
		}
		YkMutexLock mutex(m_mutex);
		mutex.lock();
		YkInt i = 0;
		for( i= 0;i<(YkInt)m_arrayThread.GetSize();i++)
		{	

			if(m_arrayThread[i] != NULL && m_arrayThread[i]->m_strName.CompareNoCase(strName) == 0 )
			{
				return NULL;
			}
		}
		
		YkThreadEx* pThread = NULL;
		pThread = new YkThreadEx();
		pThread->m_strName = strName;
		pThread->m_pFun = pFun;
		pThread->m_pParameter = pParameter;
		pThread->m_bManaged = isManaged;
		m_arrayThread.Add(pThread);
		return pThread;
	}
	//!  \brief 删除一个线程
	void YkThreadManager::DestoryThread(const YkString& strName)
	{
		YkInt i=0;
		YkThreadEx* pThread = NULL;
		YkMutexLock mutex(m_mutex);
		mutex.lock();
		for(i=0;i<(YkInt)m_arrayThread.GetSize();i++)
		{
			pThread = m_arrayThread[i];
			if(pThread != NULL && pThread->m_strName.CompareNoCase(strName) == 0)
			{
				if(pThread->IsAlive())
				{
#if defined(WIN32)
					TerminateThread(pThread->m_pHandle,NULL);
#endif

					pThread->Join();
				}
				delete pThread;
				m_arrayThread.RemoveAt(i);
				break;
			}
		}
	}
	void YkThreadManager::DestoryManagedThread(const YkString& strName)
	{
		YkInt i=0;
		YkThreadEx* pThread = NULL;
		YkMutexLock mutex(m_mutex);
		mutex.lock();
		for(i=0;i<(YkInt)m_arrayThread.GetSize();i++)
		{
			pThread = m_arrayThread[i];
			if(pThread != NULL && pThread->m_bManaged && pThread->m_strName.CompareNoCase(strName) == 0)
			{
				m_arrayThread.RemoveAt(i);
				delete pThread;
				break;
			}
		}
	}
	//!  \brief 获得当前机器的最优线程个数
	YkUint YkThreadManager::GetOptimalCount() const
	{
		return YkToolkit::GetCPUCount()*2;
	}
	//!  \brief 获取当前有效的线程个数
	YkInt YkThreadManager::GetActiveCount()
	{
		return (YkInt)m_arrayThread.GetSize();
	}
	//!  \brief 获取线程
	YkThreadEx* YkThreadManager::GetThread(const YkString& strName)
	{
		YkInt i=0;
		YkThreadEx* pThread = NULL;
		YkMutexLock mutex(m_mutex);
		mutex.lock();
		for(i=0;i<(YkInt)m_arrayThread.GetSize();i++)
		{
			pThread = m_arrayThread[i];
			if(pThread != NULL && pThread->m_strName.CompareNoCase(strName) == 0)
			{
				break;
			}
		}
		return pThread;
	}
	//!  \brief 取消托管
	YkBool YkThreadManager::CancleManaged(const YkString& strName) 
	{
		YkInt i=0;
		YkThreadEx* pThread = NULL;
		YkMutexLock mutex(m_mutex);
		mutex.lock();
		for(i=0;i<(YkInt)m_arrayThread.GetSize();i++)
		{
			pThread = m_arrayThread[i];
			if(pThread != NULL && pThread->m_strName.CompareNoCase(strName) == 0)
			{
				pThread->m_bManaged = FALSE;
				return TRUE;
			}
		}
		return FALSE;
	}
	//!  \brief 线程睡眠
	void YkThreadEx::Sleep(YkUlong nMillsecond)
	{
#if defined(WIN32)
		::Sleep(nMillsecond);
#else
		usleep(nMillsecond*1000);
#endif
	}

	//效果和yield保持一致
	YkBool YkThreadEx::SwitchToThread()
	{
#if defined(WIN32)
		return ::SwitchToThread();
#else
		return 0 == sched_yield();
#endif
	}

	void YkThreadEx::Init()
	{
		m_pFun = NULL;
		m_nPriority = PRIORITY_DEFAULT;
		m_pHandle = NULL;
		m_pParameter = NULL;
		m_nID = 0;
		m_bManaged = FALSE;
		m_nStackSize = 0;
		m_nPolicy = POLICY_DEFAULT;
		m_tSatus = THREAD_NEW;
	}

	Yk::YkString YkThreadEx::GetName()
	{
		return m_strName;
	}

}
