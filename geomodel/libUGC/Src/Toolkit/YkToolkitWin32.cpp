/*
 *
 * YkToolkitWin32.cpp
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

// 在定义win32宏的情况下，才能使用本文件
#ifdef WIN32 

#include "Stream/YkWin32.h"
#include "Toolkit/YkThread.h"
#include "Toolkit/YkToolkit.h"
#include "Toolkit/YkTextCodec.h"
#include "Stream/YkFile.h"
#include <Windows.h>
#include <assert.h>
#include <process.h>
#include <direct.h>
#include <signal.h>

#include <psapi.h>
#pragma comment(lib,"Psapi.lib")

#define MAX_PRO_STRING_LENGTH 10240

namespace Yk {

YkMutex::YkMutex(YkBool)
{  
	YKASSERT(sizeof(dataArray)>=sizeof(CRITICAL_SECTION));

	// Win32平台 调用Win32函数进行功能实现
	InitializeCriticalSection((CRITICAL_SECTION*)dataArray);
	// 完成调用
}


void YkMutex::lock()
{
	// Win32平台 调用Win32函数进行功能实现
	EnterCriticalSection((CRITICAL_SECTION*)dataArray);
	// 完成调用
}


YkBool YkMutex::trylock()
{
#if(_WIN32_WINNT >= 0x0400)
	// Win32平台 调用Win32函数进行功能实现
	return TryEnterCriticalSection((CRITICAL_SECTION*)dataArray)!=0;
	// 完成调用
#else
	return FALSE;
#endif
}


void YkMutex::unlock()
{
	// Win32平台 调用Win32函数进行功能实现
	LeaveCriticalSection((CRITICAL_SECTION*)dataArray);
	// 完成调用
}


YkBool YkMutex::locked()
{
#if(_WIN32_WINNT >= 0x0400)
	// Win32平台 调用Win32函数进行功能实现
	if(TryEnterCriticalSection((CRITICAL_SECTION*)dataArray)==0)
	{
		return TRUE;
	}

	LeaveCriticalSection((CRITICAL_SECTION*)dataArray);
	// 完成调用
	return FALSE;
#else
 	return FALSE;
#endif
}


YkMutex::~YkMutex()
{
	// Win32平台 调用Win32函数进行功能实现
	DeleteCriticalSection((CRITICAL_SECTION*)dataArray);
	// 完成调用
}

struct Yk_RWLOCK 
{
	CRITICAL_SECTION mutex[1];
	CRITICAL_SECTION access[1];
	YkUlong            readers;
};


YkReadWriteLock::YkReadWriteLock()
{
	YKASSERT(sizeof(dataArray)>=sizeof(Yk_RWLOCK));

	// Win32平台 调用Win32函数进行功能实现
	InitializeCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
	InitializeCriticalSection(((Yk_RWLOCK*)dataArray)->access);

	((Yk_RWLOCK*)dataArray)->readers=0;
}



void YkReadWriteLock::readLock()
{
	// Win32平台 调用Win32函数进行功能实现
	EnterCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);

	if(++((Yk_RWLOCK*)dataArray)->readers == 1)
	{
		EnterCriticalSection(((Yk_RWLOCK*)dataArray)->access);
	}

	LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
}


bool YkReadWriteLock::tryReadLock()
{
#if(_WIN32_WINNT >= 0x0400)

	if(TryEnterCriticalSection(((Yk_RWLOCK*)dataArray)->mutex))
	{
		if(++((Yk_RWLOCK*)dataArray)->readers==1 && !TryEnterCriticalSection(((Yk_RWLOCK*)dataArray)->access))
		{
			--((Yk_RWLOCK*)dataArray)->readers;
			LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
			return FALSE;
		}

		LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
		return TRUE;
	}

#endif
	return FALSE;
}


void YkReadWriteLock::readUnlock()
{
	EnterCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
	
	if(--((Yk_RWLOCK*)dataArray)->readers==0)
	{
		LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->access);
	}

	LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
}


void YkReadWriteLock::writeLock()
{
	EnterCriticalSection(((Yk_RWLOCK*)dataArray)->access);
}


// Try to acquire write lock for read/write lock
bool YkReadWriteLock::tryWriteLock(){
#if(_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection(((Yk_RWLOCK*)dataArray)->access)!=0;
#else
  return false;
#endif
  }


void YkReadWriteLock::writeUnlock()
{
	LeaveCriticalSection(((Yk_RWLOCK*)dataArray)->access);
}


YkReadWriteLock::~YkReadWriteLock()
{
	DeleteCriticalSection(((Yk_RWLOCK*)dataArray)->mutex);
	DeleteCriticalSection(((Yk_RWLOCK*)dataArray)->access);
}


YkSemaphore::YkSemaphore(YkInt initial)
{
	dataValues[0]=(Ykuval)(CreateSemaphore(NULL,initial,0x7fffffff,NULL));
}


void YkSemaphore::Wait()
{
  WaitForSingleObject((HANDLE)dataValues[0],INFINITE);
}


YkBool YkSemaphore::TryWait()
{
	return WaitForSingleObject((HANDLE)dataValues[0],0)==WAIT_OBJECT_0;
}


void YkSemaphore::Post()
{
	ReleaseSemaphore((HANDLE)dataValues[0],1,NULL);
}


YkSemaphore::~YkSemaphore()
{
	CloseHandle((HANDLE)dataValues[0]);
}


/*******************************************************************************/



YkCondition::YkCondition()
{
	YKASSERT(sizeof(dataArray)>=sizeof(CRITICAL_SECTION)+sizeof(HANDLE)+sizeof(HANDLE)+sizeof(YkUint));
	dataArray[0]=(Ykuval)CreateEvent(NULL,0,0,NULL);                   
	dataArray[1]=(Ykuval)CreateEvent(NULL,1,0,NULL);                   
	dataArray[2]=0;                                                    
	InitializeCriticalSection((CRITICAL_SECTION*)&dataArray[3]);       
}


void YkCondition::signal()
{
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);

	YkBool isBlocked=(dataArray[2]>0);

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);

	if(isBlocked)
	{
		SetEvent((HANDLE)dataArray[0]);
	}
}


void YkCondition::broadcast()
{
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);

	YkBool isBlocked=(dataArray[2]>0);

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	
	if(isBlocked) 
	{
		SetEvent((HANDLE)dataArray[1]);
	}
}


void YkCondition::Wait(YkMutex& mtx)
{
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	dataArray[2]++;

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	mtx.unlock();

	YkUlong result = WaitForMultipleObjects(2,(HANDLE*)dataArray,0,INFINITE);
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	dataArray[2]--;

	YkBool isLast_waiter=(result==WAIT_OBJECT_0+1)&&(dataArray[2]==0);

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	
	if(isLast_waiter)
	{
		ResetEvent((HANDLE)dataArray[1]);                  
	}

	mtx.lock();
}


YkBool YkCondition::Wait(YkMutex& mtx,YkUint nsec)
{
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	dataArray[2]++;

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	mtx.unlock();

	nsec-=YkThread::time();

	YkUlong result = WaitForMultipleObjects(2,(HANDLE*)dataArray,0,(DWORD)(nsec/1000000));
	EnterCriticalSection((CRITICAL_SECTION*)&dataArray[3]);

	dataArray[2]--;
	YkBool isLast_waiter = (result == WAIT_OBJECT_0 + 1) && (dataArray[2] == 0);

	LeaveCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
	
	if(isLast_waiter) 
	{
		ResetEvent((HANDLE)dataArray[1]);            
	}

	mtx.lock();
	return result != WAIT_TIMEOUT;
}


YkCondition::~YkCondition()
{
	CloseHandle((HANDLE)dataArray[0]);
	CloseHandle((HANDLE)dataArray[1]);

	DeleteCriticalSection((CRITICAL_SECTION*)&dataArray[3]);
}


/*******************************************************************************/

static DWORD self_key=0xffffffff;
typedef unsigned (__stdcall *PTHREAD_START) (void *);

struct TLSKEYINIT 
{
	TLSKEYINIT(){ self_key=TlsAlloc(); }
	~TLSKEYINIT(){ TlsFree(self_key); }
};

extern TLSKEYINIT initializer;

TLSKEYINIT initializer;

//! \brief 得到当前线程的ID
//! \return 返回当前线程的ID
YkUint YkThread::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

YkThread::YkThread():tid(0), thlid(0)
{
}


YkThreadID YkThread::id() const 
{
	return tid;
}

YkUint YkThread::getcallingthreadid()
{
	return thlid;
}

YkBool YkThread::running() const 
{
	return tid!=0;
}


unsigned int CALLBACK YkThread::execute(void* thread)
{
	YkInt code=-1;

	((YkThread*)thread)->thlid = GetCurrentThreadId();

	TlsSetValue(self_key,thread);

	code=((YkThread*)thread)->run(); 

	CloseHandle(((YkThread*)thread)->tid);
	((YkThread*)thread)->tid=0;

	return code;
}


YkBool YkThread::start(YkUlong stacksize)
{
	YkUlong thd;
	tid=(YkThreadID)_beginthreadex((void*)NULL,(unsigned)stacksize,(PTHREAD_START)YkThread::execute,(void*)this,(unsigned)0,(unsigned*)&thd);

	return tid!=NULL;
}


YkBool YkThread::join(YkInt& code)
{
	HANDLE ttid=(HANDLE)tid;
	if(ttid && WaitForSingleObject(ttid,INFINITE)==WAIT_OBJECT_0)
	{
		GetExitCodeThread(ttid,(DWORD*)&code);
		tid=0;
		return true;
	}
	return false;
}


YkBool YkThread::join()
{
	HANDLE ttid=(HANDLE)tid;
	if(ttid && WaitForSingleObject(ttid,INFINITE)==WAIT_OBJECT_0)
	{
		tid=0;
		return true;
	}

	return false;
}


YkBool YkThread::cancel()
{
	HANDLE ttid=(HANDLE)tid;

	if(ttid && TerminateThread(ttid,0))
	{
		CloseHandle(ttid);
		tid=0;
		return true;
	}
	return false;
}


YkBool YkThread::detach()
{
	return tid!=0;
}


void YkThread::exit(YkInt code)
{	
	if(self())
	{ 
		self()->tid=0; 
	}
	_endthreadex(code);
}


void YkThread::yield()
{
	Sleep(0);
}


YkUlong YkThread::time()
{
	YkUlong now=0;
	GetSystemTimeAsFileTime((FILETIME*)&now);
	return (now-116444736000000000L)*100L;
}


void YkThread::sleep(YkUint nsec)
{
	Sleep((YkUlong)(nsec/1000000));
}


// Wake at appointed time
void YkThread::wakeat(YkUint nsec)
{
  nsec-=YkThread::time();
  if(nsec<0)
  {
	  nsec=0;
  }

  Sleep((DWORD)(nsec/1000000));
}


YkThreadID YkThread::current()
{
	return (YkThreadID)GetCurrentThreadId();
}


YkThread* YkThread::self()
{
	return (YkThread*)TlsGetValue(self_key);
}


// Set thread priority
YkBool YkThread::priority(YkThread::Priority prio)
{
	HANDLE ttid=(HANDLE)tid;
	if(ttid){
		int pri=0;
		switch(prio){
	  case PRIORITY_MINIMUM:
		  pri=(YkUint)THREAD_PRIORITY_IDLE;
		  break;
	  case PRIORITY_LOWER:
		  pri=(YkUint)THREAD_PRIORITY_BELOW_NORMAL;
		  break;
	  case PRIORITY_MEDIUM:
		  pri=(YkUint)THREAD_PRIORITY_NORMAL;
		  break;
	  case PRIORITY_HIGHER:
		  pri=(YkUint)THREAD_PRIORITY_ABOVE_NORMAL;
		  break;
	  case PRIORITY_MAXIMUM:
		  pri=(YkUint)THREAD_PRIORITY_HIGHEST;
		  break;
	  default:
		  pri=(YkUint)THREAD_PRIORITY_NORMAL;
		  break;
		}
		return SetThreadPriority(ttid,pri)!=0;
	}
	return false;
}


// Return thread priority
YkThread::Priority YkThread::priority() const {
  HANDLE ttid=(HANDLE)tid;
  Priority result=PRIORITY_ERROR;
  if(ttid){
    int pri=GetThreadPriority(ttid);
    if(pri!=THREAD_PRIORITY_ERROR_RETURN){
      switch(pri){
        case THREAD_PRIORITY_IDLE:
          result=PRIORITY_MINIMUM;
          break;
        case THREAD_PRIORITY_BELOW_NORMAL:
          result=PRIORITY_LOWER;
          break;
        case THREAD_PRIORITY_NORMAL:
          result=PRIORITY_MEDIUM;
          break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
          result=PRIORITY_HIGHER;
          break;
        case THREAD_PRIORITY_HIGHEST:
          result=PRIORITY_MAXIMUM;
          break;
        default:
          result=PRIORITY_DEFAULT;
          break;
        }
      }
    }
  return result;
  }


// Set thread scheduling policy
YkBool YkThread::policy(YkThread::Policy){
  return true;
  }


// Get thread scheduling policy
YkThread::Policy YkThread::policy() const {
  return POLICY_DEFAULT;
  }


// Destroy
YkThread::~YkThread(){
  HANDLE ttid=(HANDLE)tid;
  if(ttid){
    TerminateThread(ttid, 0);
    CloseHandle(ttid);
	tid = 0;
    }
  }

YkThreadEx::YkThreadEx()
{
	Init();
}

YkThreadEx::~YkThreadEx()
{
	if(m_pHandle != NULL)
	{
		try
		{
			CloseHandle(m_pHandle);
		}
		catch (...)
		{
			
		}
	}
}

YkHandle CALLBACK YkThreadEx::execute(void* thread)
{

	if(thread != NULL)
	{
		((YkThreadEx*)thread)->m_nID = GetCurrentThreadId();

		TlsSetValue(self_key,thread);

		try
		{ 
			((YkThreadEx*)thread)->m_nReturnValue = ((YkThreadEx*)thread)->m_pFun(((YkThreadEx*)thread)->m_pParameter);
		} 
		catch(...)
		{
		}
	}
	YkUint nReturnValue = 0; 
	if(thread != NULL)
	{
		nReturnValue = ((YkThreadEx*)thread)->m_nReturnValue;
		if(((YkThreadEx*)thread)->m_bManaged)
		{
			//这个方法里面会判断m_bManaged，m_bManaged中间有可能被人更改
			YkThreadManager::GetInstance().DestoryManagedThread(((YkThreadEx*)thread)->m_strName);
		}
	}
	
	_endthreadex(nReturnValue);
	return nReturnValue;
}



//! \brief 设置线程启动时候的堆栈大小
void YkThreadEx::SetStackSize(YkUint nSize)
{
	m_nStackSize = nSize;
}

//! \brief 设置线程的优先级
YkBool YkThreadEx::SetPriority(Priority prio)
{
	m_nPriority = prio;
	if(m_pHandle != NULL){
		YkUint pri;
		switch(prio){
			 case PRIORITY_MINIMUM:
			  pri=(YkUint)THREAD_PRIORITY_IDLE;
			  break;
		  case PRIORITY_LOWER:
			  pri=(YkUint)THREAD_PRIORITY_BELOW_NORMAL;
			  break;
		  case PRIORITY_MEDIUM:
			  pri=(YkUint)THREAD_PRIORITY_NORMAL;
			  break;
		  case PRIORITY_HIGHER:
			  pri=(YkUint)THREAD_PRIORITY_ABOVE_NORMAL;
			  break;
		  case PRIORITY_MAXIMUM:
			  pri=(YkUint)THREAD_PRIORITY_HIGHEST;
			  break;
		  default:
			  pri=(YkUint)THREAD_PRIORITY_NORMAL;
			  break;
		}
		return SetThreadPriority(m_pHandle,pri)!=0;
	}
	return TRUE;
}

// Return thread priority
YkThreadEx::Priority YkThreadEx::GetPriority() const {

	Priority result = m_nPriority;
	if(m_pHandle != NULL){
		int pri = GetThreadPriority(m_pHandle);
		if(pri!=THREAD_PRIORITY_ERROR_RETURN){
			switch(pri){
		case THREAD_PRIORITY_IDLE:
			result=PRIORITY_MINIMUM;
			break;
		case THREAD_PRIORITY_BELOW_NORMAL:
			result=PRIORITY_LOWER;
			break;
		case THREAD_PRIORITY_NORMAL:
			result=PRIORITY_MEDIUM;
			break;
		case THREAD_PRIORITY_ABOVE_NORMAL:
			result=PRIORITY_HIGHER;
			break;
		case THREAD_PRIORITY_HIGHEST:
			result=PRIORITY_MAXIMUM;
			break;
		default:
			result=PRIORITY_DEFAULT;
			break;
			}
		}
	}
	return result;
}



//! \brief 设置线程的调度策略
void YkThreadEx::SetPolicy(Policy ploi)
{
	m_nPolicy = ploi;
}

//! \brief 返回线程的状态
YkThreadEx::Thread_Status YkThreadEx::GetStatus() const
{
	DWORD code = 0;
	if(m_pHandle != NULL )
	{
		if(GetExitCodeThread(m_pHandle, &code))
		{
			if(code == STILL_ACTIVE)
			{
				return YkThreadEx::THREAD_RUNNING;
			}	
			return YkThreadEx::TERMINATED; 
		}
		else
		{
			return YkThreadEx::THREAD_UNKNOWN;
		}
	}
	else
	{
		return YkThreadEx::THREAD_NEW;
	}
}
//! \brief测试线程是否处于活动状态
YkBool YkThreadEx::IsAlive() const
{
	return GetStatus() == YkThreadEx::THREAD_RUNNING;
}

//!  \brief 开始线程
YkBool YkThreadEx::Start()
{

	if(IsAlive())
	{
		return FALSE;
	}
	if(m_pHandle != NULL)
	{
		if(CloseHandle(m_pHandle))
		{
			m_pHandle = NULL;
			m_nID = 0;
		}
		else
		{
			return FALSE;
		}

	}
	m_pHandle = (void*)_beginthreadex((void*)NULL,m_nStackSize,YkThreadEx::execute,this,0,&m_nID);
	if(m_pHandle != NULL)
	{
		YkUint pri;
		switch(m_nPriority){
			 case PRIORITY_MINIMUM:
				 pri=(YkUint)THREAD_PRIORITY_IDLE;
				 break;
			 case PRIORITY_LOWER:
				 pri=(YkUint)THREAD_PRIORITY_BELOW_NORMAL;
				 break;
			 case PRIORITY_MEDIUM:
				 pri=(YkUint)THREAD_PRIORITY_NORMAL;
				 break;
			 case PRIORITY_HIGHER:
				 pri=(YkUint)THREAD_PRIORITY_ABOVE_NORMAL;
				 break;
			 case PRIORITY_MAXIMUM:
				 pri=(YkUint)THREAD_PRIORITY_HIGHEST;
				 break;
			 default:
				 pri=(YkUint)THREAD_PRIORITY_NORMAL;
				 break;
		}
		SetThreadPriority(m_pHandle,pri);
		ResumeThread(m_pHandle);
	}

	return m_pHandle != NULL;
}

YkUint YkThreadEx::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

//!  \brief 挂起调用线程，直到线程结束
YkBool YkThreadEx::Join()
{
	if(m_pHandle != NULL && WaitForSingleObject(m_pHandle,INFINITE)==WAIT_OBJECT_0)
	{
		return TRUE;
	}
	return FALSE;
}

//!  \brief 获得线程结束的返回值
YkInt YkThreadEx::GetReturnValue()
{
	DWORD code = 0;
	if(!IsAlive())
	{
		GetExitCodeThread(m_pHandle,(DWORD*)&code);
		m_nReturnValue = code; 
	}
	return m_nReturnValue;
}

YkInt YkToolkit::GetCurProcessId()
{
	return ::GetCurrentProcessId();
}



YkMBString::Charset YkToolkit::GetCurCharset()
{	
	if(YkTextCodec::IsUseGlobalCharset())
	{
		if(YkTextCodec::GetGlobalCharset() != YkMBString::ANSI)
		{
			return YkTextCodec::GetGlobalCharset();
		}
		//iconv中本身就是没有ansi编码，如果返回ANSI会导致崩溃
	}

	return YkSystem::GetCurCharset();
}

YkBool YkToolkit::GetMemoryStatus(MemoryStatus& memoryStatus)
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memStatus);

	memoryStatus.m_nAvailablePhysical = RoundLong(memStatus.ullAvailPhys/1024.0);
	memoryStatus.m_nTotalPhysical = RoundLong(memStatus.ullTotalPhys/1024.0);
	// 可用虚拟内存 = 所有可用内存-可用物理内存
	memoryStatus.m_nAvailableVirtual = RoundLong((memStatus.ullAvailPageFile -memStatus.ullAvailPhys )/1024.0);
	// 所有虚拟内存 = 所有内存-所有物理内存
	memoryStatus.m_nTotalVirtual = RoundLong((memStatus.ullTotalPageFile-memStatus.ullTotalPhys)/1024.0);
    
	//获取内存的占有率
	memoryStatus.m_nMemoryLoad = memStatus.dwMemoryLoad;

	return TRUE;
}

YkBool YkToolkit::GetCurrentProcMemoryStatus(ProcesMemoryStatus& procesMemory)
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProc = GetCurrentProcess();
	if (GetProcessMemoryInfo(hProc,&pmc,sizeof(pmc)))
	{
		procesMemory.m_nWorkingSetSize = pmc.WorkingSetSize/1024;
		procesMemory.m_nQuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage/1024;
		return TRUE;
	}
	return FALSE;
}

YkUlong YkToolkit::GetDiskSpace(const YkString& strPath)
{
	YkString strPaths;
	YkInt pos=strPath.Find(_U("\\"),0);
	if (pos>0)
	{
		strPaths=strPath.Mid(0,pos+1);
	}
	else
	{
		strPaths=strPath;
	}

	YkString strCurrentPath = YkFile::GetCurrentPath();
	strPaths = YkFile::GetAbsolutePath(strCurrentPath,strPaths);
	strPaths = YkFile::GetDir(strPaths);
	
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	FreeBytesAvailableToCaller.QuadPart=0;
	GetDiskFreeSpaceEx(strPaths,&FreeBytesAvailableToCaller,
		&TotalNumberOfBytes, &TotalNumberOfFreeBytes);
	return FreeBytesAvailableToCaller.QuadPart;
}

}

#endif
