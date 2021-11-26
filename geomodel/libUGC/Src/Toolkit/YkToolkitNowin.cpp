/*
 *
 * YkToolkitNowin.cpp
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

// 在没有定义win32宏的情况下，才能使用本文件
#ifndef WIN32 
#include <dirent.h> // for opendir(), readdir(), closedir() 
#include <sys/types.h> // for lstat
#include <sys/stat.h> // for lstat
#include <sys/statvfs.h> //for statvfs
#include "Toolkit/YkThread.h"
#include "Toolkit/YkToolkit.h"
#include "Toolkit/YkTextCodec.h"
#include "Stream/YkFile.h"
#include "Stream/YkDefines.h"
#include "Stream/YkFileStdio.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
// #include <X11/Xlib.h>
// #include <X11/Xutil.h>
// #include <X11/cursorfont.h>
#include <signal.h>
#if defined (__linux__)
#include <execinfo.h> //for backtrace
#endif

#include <pthread.h>
#include <semaphore.h>	
#include <errno.h>

#include <unistd.h> // for getwcd()
#include <dlfcn.h> // for dlopen() & dlsym()

#include <cstring>
#include <time.h>
#include <fstream>

#include <errno.h>     // for error

namespace Yk {
// Initialize mutex
YkMutex::YkMutex(YkBool recursive){
  pthread_mutexattr_t mutexatt;
  YKASSERT(sizeof(dataArray)>=sizeof(pthread_mutex_t));
  pthread_mutexattr_init(&mutexatt);
  pthread_mutexattr_settype(&mutexatt,recursive?PTHREAD_MUTEX_RECURSIVE:PTHREAD_MUTEX_DEFAULT);
  pthread_mutex_init((pthread_mutex_t*)dataArray,&mutexatt);
  pthread_mutexattr_destroy(&mutexatt);
  }


// Lock the mutex
void YkMutex::lock(){
  pthread_mutex_lock((pthread_mutex_t*)dataArray);
  }


// Try lock the mutex
YkBool YkMutex::trylock(){
  return pthread_mutex_trylock((pthread_mutex_t*)dataArray)==0;
  }


// Unlock mutex
void YkMutex::unlock(){
  pthread_mutex_unlock((pthread_mutex_t*)dataArray);
  }


// Test if locked
YkBool YkMutex::locked(){
  if(pthread_mutex_trylock((pthread_mutex_t*)dataArray)==16) return true;
  pthread_mutex_unlock((pthread_mutex_t*)dataArray);
  return false;
  }


// Delete mutex
YkMutex::~YkMutex(){
  pthread_mutex_destroy((pthread_mutex_t*)dataArray);
  }


/*******************************************************************************/

// Initialize read/write lock
YkReadWriteLock::YkReadWriteLock(){
  pthread_rwlockattr_t rwlockatt;
  YKASSERT(sizeof(dataArray)>=sizeof(pthread_rwlock_t));
  pthread_rwlockattr_init(&rwlockatt);
  pthread_rwlockattr_setkind_np(&rwlockatt,PTHREAD_RWLOCK_PREFER_READER_NP);
  pthread_rwlock_init((pthread_rwlock_t*)dataArray,&rwlockatt);
  pthread_rwlockattr_destroy(&rwlockatt);
  }


// Acquire read lock for read/write lock
void YkReadWriteLock::readLock(){
  pthread_rwlock_rdlock((pthread_rwlock_t*)dataArray);
  }


// Try to acquire read lock for read/write lock
bool YkReadWriteLock::tryReadLock(){
  return pthread_rwlock_tryrdlock((pthread_rwlock_t*)dataArray)==0;
  }


// Unlock read lock
void YkReadWriteLock::readUnlock(){
  pthread_rwlock_unlock((pthread_rwlock_t*)dataArray);
  }


// Acquire write lock for read/write lock
void YkReadWriteLock::writeLock(){
  pthread_rwlock_wrlock((pthread_rwlock_t*)dataArray);
  }


// Try to acquire write lock for read/write lock
bool YkReadWriteLock::tryWriteLock(){
  return pthread_rwlock_trywrlock((pthread_rwlock_t*)dataArray)==0;
  }


// Unlock write lock
void YkReadWriteLock::writeUnlock(){
  pthread_rwlock_unlock((pthread_rwlock_t*)dataArray);
  }


// read/write lock
YkReadWriteLock::~YkReadWriteLock(){
  pthread_rwlock_destroy((pthread_rwlock_t*)dataArray);
  }

/*******************************************************************************/

// Initialize semaphore
YkSemaphore::YkSemaphore(YkInt initial)
{
	YKASSERT(sizeof(dataValues)>=sizeof(sem_t));
	sem_init((sem_t*)dataValues,0,(unsigned int)initial);
}


void YkSemaphore::Wait()
{
  sem_wait((sem_t*)dataValues);
}


YkBool YkSemaphore::TryWait()
{
  return sem_trywait((sem_t*)dataValues)==0;
}

void YkSemaphore::Post()
{
  sem_post((sem_t*)dataValues);
}


YkSemaphore::~YkSemaphore()
{
  sem_destroy((sem_t*)dataValues);
}

/*******************************************************************************/

YkCondition::YkCondition()
{
	YKASSERT(sizeof(dataArray)>=sizeof(pthread_cond_t));
	pthread_cond_init((pthread_cond_t*)dataArray,NULL);
}

void YkCondition::signal()
{
	pthread_cond_signal((pthread_cond_t*)dataArray);
}

void YkCondition::broadcast()
{
	pthread_cond_broadcast((pthread_cond_t*)dataArray);
}


void YkCondition::Wait(YkMutex& mtx)
{
	pthread_cond_wait((pthread_cond_t*)dataArray,(pthread_mutex_t*)&mtx.dataArray);
}


YkBool YkCondition::Wait(YkMutex& mtx,YkUint nsec)
{
	YkInt result=0;
	timespec ts;
	ts.tv_sec=nsec/1000000000;
	ts.tv_nsec=nsec%1000000000;
x:result=pthread_cond_timedwait((pthread_cond_t*)dataArray,(pthread_mutex_t*)&mtx.dataArray,&ts);

	if(result == 4) goto x;
	return result != 10060;
}


YkCondition::~YkCondition()
{
	pthread_cond_destroy((pthread_cond_t*)dataArray);
}


/*******************************************************************************/

// Thread local storage key for back-reference to YkThread
static pthread_key_t self_key;

struct TLSKEYINIT 
{
	TLSKEYINIT()
	{ 
		 pthread_key_create(&self_key,NULL); 
	}
	~TLSKEYINIT()
	{ 
		pthread_key_delete(self_key); 
	}
};

extern TLSKEYINIT initializer;

TLSKEYINIT initializer;

//! \brief 得到当前线程的ID
//! \return 返回当前线程的ID
YkUint YkThread::GetCurrentThreadId()
{
    return (YkUint)pthread_self();
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

void* YkThread::execute(void* thread)
{
	YkInt code=-1;
	((YkThread*)thread)->thlid = GetCurrentThreadId();
	pthread_setspecific(self_key,thread);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	try{ code=((YkThread*)thread)->run(); } catch(...){ }

	pthread_detach((pthread_t)((YkThread*)thread)->tid);
	((YkThread*)thread)->tid=0;
	return (void*)(YkUint)code;
}


YkBool YkThread::start(YkUlong stacksize)
{
	YkBool code=FALSE;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);

	if(stacksize){ pthread_attr_setstacksize(&attr,stacksize); }
	code=pthread_create((pthread_t*)&tid,&attr,YkThread::execute,(void*)this)==0;
	pthread_attr_destroy(&attr);
	return code;
}



// Suspend calling thread until thread is done
YkBool YkThread::join(YkInt& code){
  pthread_t ttid=(pthread_t)tid;
  void *trc=NULL;
  if(ttid && pthread_join(ttid,&trc)==0){
    code=(YkInt)(YkSizeT)trc;
    tid=0;
    return true;
    }
  return false;
  }


// Suspend calling thread until thread is done
YkBool YkThread::join(){
  YkInt code=-1;
  pthread_t ttid=(pthread_t)tid;
  if(ttid && pthread_join(ttid,NULL)==0){
    tid=0;
    return true;
    }
  return false;
  }


// Cancel the thread
YkBool YkThread::cancel(){
  YkInt code=-1;
  pthread_t ttid=(pthread_t)tid;
  if(ttid && pthread_cancel(ttid)==0){
    pthread_join(ttid,NULL);
    tid=0;
    return true;
    }
  return false;
  }


// Detach thread
YkBool YkThread::detach(){
  YkInt code=-1;
  pthread_t ttid=(pthread_t)tid;
  return ttid && pthread_detach(ttid)==0;
  }


// Exit calling thread
void YkThread::exit(YkInt code){
  if(self()){ self()->tid=0; }
  pthread_exit((void*)(YkUint)code);
  }


// Yield the thread
void YkThread::yield(){
  sched_yield();                // More portable than pthread_yield()
  }


// Get time in nanoseconds since Epoch
YkUlong YkThread::time(){
  const YkUint seconds=1000000000;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME,&ts);
  return ts.tv_sec*seconds+ts.tv_nsec;
  }


// Sleep for some time
void YkThread::sleep(YkUint nsec){
  const YkUint seconds=1000000000;
  const YkUint microseconds=1000;
  const YkUint milliseconds=1000000;
  struct timeval value;
  value.tv_usec=(nsec/microseconds)%milliseconds;
  value.tv_sec=YKROUND(nsec/seconds);
  select(1,0,0,0,&value);
  }


// Wake at appointed time
void YkThread::wakeat(YkUint nsec){
  const YkUint seconds=1000000000;
  const YkUint microseconds=1000;
  const YkUint milliseconds=1000000;
  struct timeval value;
  if(nsec<0) nsec=0;
  value.tv_usec=(nsec/microseconds)%milliseconds;
  value.tv_sec=nsec/seconds;
  select(1,0,0,0,&value);
  }


// Return pointer to calling thread's instance
YkThread* YkThread::self()
{
	return (YkThread*)pthread_getspecific(self_key);
}


// Return thread id of caller
YkThreadID YkThread::current(){
  return (YkThreadID)pthread_self();
  }


// Set thread priority
YkBool YkThread::priority(YkThread::Priority prio){
  pthread_t ttid=(pthread_t)tid;
  if(ttid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam(ttid,&plcy,&sched)==0){
#if defined(_POSIX_PRIORITY_SCHEDULING)
      int priomax=sched_get_priority_max(plcy);         // Note that range may depend on scheduling policy!
      int priomin=sched_get_priority_min(plcy);
#elif defined(PTHREAD_MINPRIORITY) && defined(PTHREAD_MAX_PRIORITY)
      int priomin=PTHREAD_MIN_PRIORITY;
      int priomax=PTHREAD_MAX_PRIORITY;
#else
      int priomin=0;
      int priomax=20;
#endif
      int priomed=(priomax+priomin)/2;
      switch(prio){
        case PRIORITY_MINIMUM:
          sched.sched_priority=priomin;
          break;
        case PRIORITY_LOWER:
          sched.sched_priority=(priomin+priomed)/2;
          break;
        case PRIORITY_MEDIUM:
          sched.sched_priority=priomed;
          break;
        case PRIORITY_HIGHER:
          sched.sched_priority=(priomax+priomed)/2;
          break;
        case PRIORITY_MAXIMUM:
          sched.sched_priority=priomax;
          break;
        default:
          sched.sched_priority=priomed;
          break;
        }
      return pthread_setschedparam(ttid,plcy,&sched)==0;
      }
    }
  return false;
  }


// Return thread priority
YkThread::Priority YkThread::priority() const {
  pthread_t ttid=(pthread_t)tid;
  Priority result=PRIORITY_ERROR;
  if(ttid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam(ttid,&plcy,&sched)==0){
#if defined(_POSIX_PRIORITY_SCHEDULING)
      int priomax=sched_get_priority_max(plcy);         // Note that range may depend on scheduling policy!
      int priomin=sched_get_priority_min(plcy);
#elif defined(PTHREAD_MINPRIORITY) && defined(PTHREAD_MAX_PRIORITY)
      int priomin=PTHREAD_MIN_PRIORITY;
      int priomax=PTHREAD_MAX_PRIORITY;
#else
      int priomin=0;
      int priomax=32;
#endif
      int priomed=(priomax+priomin)/2;
      if(sched.sched_priority<priomed){
        if(sched.sched_priority<=priomin){
          result=PRIORITY_MINIMUM;
          }
        else{
          result=PRIORITY_LOWER;
          }
        }
      else if(sched.sched_priority<priomed){
        if(sched.sched_priority>=priomax){
          result=PRIORITY_MAXIMUM;
          }
        else{
          result=PRIORITY_HIGHER;
          }
        }
      else{
        result=PRIORITY_MEDIUM;
        }
      return result;
      }
    }
  return result;
  }


// Set thread scheduling policy
YkBool YkThread::policy(YkThread::Policy plcy){
  pthread_t ttid=(pthread_t)tid;
  if(ttid){
    sched_param sched={0};
    int oldplcy=0;
    int newplcy=0;
    if(pthread_getschedparam(ttid,&oldplcy,&sched)==0){
      switch(plcy){
        case POLICY_FIFO:
          newplcy=SCHED_FIFO;
          break;
        case POLICY_ROUND_ROBIN:
          newplcy=SCHED_RR;
          break;
        default:
          newplcy=SCHED_OTHER;
          break;
        }
      return pthread_setschedparam(ttid,newplcy,&sched)==0;
      }
    }
  return false;
  }


// Get thread scheduling policy
YkThread::Policy YkThread::policy() const {
  pthread_t ttid=(pthread_t)tid;
  Policy result=POLICY_ERROR;
  if(ttid){
    sched_param sched={0};
    int plcy=0;
    if(pthread_getschedparam(ttid,&plcy,&sched)==0){
      switch(plcy){
        case SCHED_FIFO:
          result=POLICY_FIFO;
          break;
        case SCHED_RR:
          result=POLICY_ROUND_ROBIN;
          break;
        default:
          result=POLICY_DEFAULT;
          break;
        }
      }
    }
  return result;
  }


// Destroy; if it was running, stop it
YkThread::~YkThread(){
  pthread_t ttid=(pthread_t)tid;
  if(ttid){
    pthread_cancel(ttid);
  }
}
YkThreadEx::YkThreadEx()
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

YkThreadEx::~YkThreadEx()
{
	m_pHandle = NULL;
}

YkHandle YKCALLBACK YkThreadEx::execute(void* thread)
{
    ((YkThreadEx*)thread)->m_nID = pthread_self();

	pthread_setspecific(self_key,thread);
 	YKASSERT(0==pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL));
 	YKASSERT(0==pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL));

	try
	{
		((YkThreadEx*)thread)->m_nReturnValue = ((YkThreadEx*)thread)->m_pFun(((YkThreadEx*)thread)->m_pParameter);
	}
	catch(...){ }

	YKASSERT(0==pthread_detach((pthread_t)((YkThreadEx*)thread)->m_nID));
	
	if(((YkThreadEx*)thread)->m_bManaged)
	{
		//这个方法里面会判断m_bManaged，m_bManaged中间有可能被人更改
		YkThreadManager::GetInstance().DestoryManagedThread(((YkThreadEx*)thread)->m_strName);
	}

	((YkThreadEx*)thread)->m_tSatus = TERMINATED;

	return (void*)((YkThreadEx*)thread)->m_nReturnValue;
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
	pthread_t ttid=(pthread_t)m_nID;
	if(ttid != 0)
	{
		sched_param SchedParam;
		YkInt nPolicy =0;
		if(pthread_getschedparam(ttid,&nPolicy,&SchedParam)==0)
		{
			YkInt priomax=sched_get_priority_max(nPolicy);
			YkInt priomin=sched_get_priority_min(nPolicy);
			const YkUint nSegment = 4;
			YkInt nStep = (priomax+priomin)/nSegment;
			YkInt nPriorty = 0;
			switch(m_nPriority)
			{
			case PRIORITY_MINIMUM:
				nPriorty=priomin;
				break;
			case PRIORITY_LOWER:
				nPriorty=priomin+nStep;
				break;
			case PRIORITY_MEDIUM:
				nPriorty=priomin + 2*nStep;
				break;
			case PRIORITY_HIGHER:
				nPriorty=priomin + 3*nStep;
				break;
			case PRIORITY_MAXIMUM:
				nPriorty=priomax;
				break;
			default:
				nPriorty=priomin + 2*nStep;
				break;
			}
			
			SchedParam.sched_priority = nPriorty;

			return pthread_setschedparam(ttid,nPolicy,&SchedParam)==0;
		}
	}
	return TRUE;
}

//! \brief 设置线程的调度策略
void YkThreadEx::SetPolicy(Policy ploi)
{
	m_nPolicy = ploi;
	YkInt nPolicy;
	if(m_nPolicy == POLICY_DEFAULT)
	{
		nPolicy = SCHED_OTHER;
	}
	else if(m_nPolicy == POLICY_FIFO)
	{
		nPolicy = SCHED_FIFO;
	}
	else if (m_nPolicy == POLICY_ROUND_ROBIN)
	{
		nPolicy = SCHED_RR;
	}
	pthread_t tid = (pthread_t)m_nID;
#ifdef _DEBUG
	if (m_nID!=0)
	{
		YKASSERT(0==pthread_setschedparam(tid, nPolicy, NULL));
	}
#endif
}

//! \brief 返回线程的状态
YkThreadEx::Thread_Status YkThreadEx::GetStatus() const
{
	return m_tSatus;
}
//! \brief测试线程是否处于活动状态
YkBool YkThreadEx::IsAlive() const
{
	return GetStatus() == YkThreadEx::THREAD_RUNNING;
}

//!  \brief 开始线程
YkBool YkThreadEx::Start()
{
	if (IsAlive())
	{
		return FALSE;
	}
	m_tSatus = THREAD_RUNNING;
	YkUint code=0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	YKASSERT(0 == pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED));
	// 设置堆栈大小
	if(m_nStackSize != 0)
	{ 
		YKASSERT(0==pthread_attr_setstacksize(&attr,m_nStackSize));
	}

	// 设置调度策略
	YkInt plcy=0;
	if(m_nPolicy == POLICY_DEFAULT)
	{
		plcy = SCHED_OTHER;
	}
	else if(m_nPolicy == POLICY_FIFO)
	{
		plcy = SCHED_FIFO;
	}
	else if (m_nPolicy == POLICY_ROUND_ROBIN)
	{
		plcy = SCHED_RR;
	}
	YKASSERT(0==pthread_attr_setschedpolicy(&attr,plcy));

	// 对于SCHED_OTHER调度策略下的线程没有优先级设置
	if (plcy != SCHED_OTHER)
	{
		sched_param sched={0};
		YkInt priomax=sched_get_priority_max(plcy);         
		YkInt priomin=sched_get_priority_min(plcy);
		const YkUint nSegment = 4;
		YkInt nStep = (priomax+priomin)/nSegment;
		switch(m_nPriority)
		{
		case PRIORITY_MINIMUM:
			sched.sched_priority=priomin;
			break;
		case PRIORITY_LOWER:
			sched.sched_priority=priomin+nStep;
			break;
		case PRIORITY_MEDIUM:
			sched.sched_priority=priomin + 2*nStep;
			break;
		case PRIORITY_HIGHER:
			sched.sched_priority=priomin + 3*nStep;
			break;
		case PRIORITY_MAXIMUM:
			sched.sched_priority=priomax;
			break;
		default:
			sched.sched_priority=priomin + 2*nStep;
			break;
		}
	
		YKASSERT(0 == pthread_attr_setschedparam(&attr,&sched));
	}
	pthread_t tid;
	code=pthread_create(&tid,&attr,YkThreadEx::execute,this);
	YKASSERT(code == 0);
	if(code == 0)
	{
		m_nID = (YkUint)tid;
	}
	
	pthread_attr_destroy(&attr);

	return code == 0;
}


//!  \brief 挂起调用线程，直到线程结束
YkBool YkThreadEx::Join()
{
	pthread_t tid=(pthread_t)m_nID;
	void *pRtnValue = NULL;
	if(m_nID != 0 && pthread_join(tid,&pRtnValue)==0)
	{
		m_nReturnValue = (YkInt)(pthread_t)pRtnValue;
		m_tSatus = TERMINATED;
	}

	return TRUE;
}

// 	//!  \brief 线程挂起
// 	void Suspend();
// 	//!  \brief 线程恢复
// 	void Resume();
//!  \brief 获得线程结束的返回值
YkInt YkThreadEx::GetReturnValue()
{
	void* pReturnValue = NULL;
	pthread_t tid = (pthread_t)m_nID;
	if (m_nID != 0 && IsAlive())
	{
		YKASSERT(pthread_join(tid, &pReturnValue) == 0);
		m_nReturnValue = (YkInt)(pthread_t)pReturnValue;
	}

	return m_nReturnValue;
}

//! \brief 得到当前线程的ID
//! \return 返回当前线程的ID
YkUint YkThreadEx::GetCurrentThreadId()
{
	return (YkUint)pthread_self();
}
    
YkThreadEx::Priority YkThreadEx::GetPriority() const
{
	pthread_t tid = (pthread_t)m_nID;
	Priority nRtnPriority = m_nPriority;
	
	if (tid != 0)
	{
		YkInt nPolicy = 0;
		sched_param SchedParam;
		YKASSERT(0==pthread_getschedparam(tid, &nPolicy, &SchedParam));
		// SCHED_OTHER不支持优先级
		if (nPolicy == SCHED_OTHER)
		{
			return nRtnPriority;
		}

		YkInt nPriority = SchedParam.sched_priority;
		
		YkInt nMaxPriority = sched_get_priority_max(nPolicy);
		YkInt nMinPriority = sched_get_priority_min(nPolicy);
		const YkUint nSegment = 4;
		YkInt nStep = (nMaxPriority-nMinPriority)/nSegment;
		if (nPriority>=nMinPriority+2*nStep && nPriority < nMinPriority+3*nStep)
		{
			nRtnPriority = PRIORITY_MEDIUM;
		}
		else if(nPriority>=nMinPriority && nPriority < nMinPriority+nStep)
		{
			nRtnPriority = PRIORITY_MINIMUM;
		}
		else if(nPriority>=nMinPriority+nStep && nPriority < nMinPriority+2*nStep)
		{
			nRtnPriority = PRIORITY_LOWER;
		}
		else if(nPriority>=nMinPriority+3*nStep && nPriority < nMaxPriority)
		{
			nRtnPriority = PRIORITY_HIGHER;
		}
		else if (nPriority == nMaxPriority)
		{
			nRtnPriority = PRIORITY_MAXIMUM;
		}
	}
	
	return nRtnPriority;
}

YkInt YkToolkit::GetCurProcessId()
{
	return (YkInt)getpid();
}

YkMBString::Charset YkToolkit::GetCurCharset()
{	
	
	if(YkTextCodec::IsUseGlobalCharset())
	{
		if(YkTextCodec::GetGlobalCharset() != YkMBString::ANSI)
		{
			return YkTextCodec::GetGlobalCharset();
			//iconv中本身就是没有ansi编码，如果返回ANSI会导致崩溃
		}
	}
	return YkSystem::GetCurCharset();	
	
}

// 几个静态函数,只供本cpp内部使用
static YkBool GetPhysicalMemory(YkUlong& nAvailable, YkUlong& nTotal)
{
    YkUlong nPageSize = sysconf(_SC_PAGE_SIZE); // System memory page size

#if defined(__linux__)
    YkUlong nAvailPhyPages = sysconf(_SC_AVPHYS_PAGES); //Number of physical memory pages not currently in use by system
    YkUlong nTotalPhyPages = sysconf(_SC_PHYS_PAGES); // _SC_PHYS_PAGES Total number of pages of physical memory in system
#endif
    
    //modified end
    nAvailable = YkToolkit::RoundLong(nPageSize*nAvailPhyPages/1024.0);
    nTotal = YkToolkit::RoundLong(nPageSize*nTotalPhyPages/1024.0);
	return TRUE;
}

static YkBool GetVirtualMemory(YkUlong& nAvailable, YkUlong& nTotal)
{
	YkString strTempFile;
	YkFile::GetTemporaryFileName(YkFile::GetTemporaryPath(), _U("ugc"), clock(), strTempFile);	

#if defined (__linux__)
	strTempFile = _U("/proc/meminfo");
	YkFileStdio fileTemp;
	if( fileTemp.Open(strTempFile.Cstr(), YkStreamLoad))
	{
		while (!fileTemp.IsEOF())
		{
			YkMBString strInfo;
			fileTemp.ReadLine(strInfo);
			YKTRACE((_U("info:%s\n"),strInfo.Cstr()));
			if (strInfo.Find("SwapTotal") > -1)
			{
				strInfo.TrimLeft("SwapTotal:");

				YKTRACE((_U("SwapTotal:%s\n"),strInfo.Cstr()));
				strInfo.TrimRight("kB");
				YKTRACE((_U("after kb,SwapTotal:%s\n"),strInfo.Cstr()));

				nTotal = strInfo.ToULong();
				YKTRACE((_U("SwapTotal value:%lld\n"),nTotal  ));
			}
			else if (strInfo.Find("SwapFree") > -1)
			{
			
				strInfo.TrimLeft("SwapFree:");

				YKTRACE((_U("SwapFree:%s\n"),strInfo.Cstr()));
				strInfo.TrimRight("kB");
				YKTRACE((_U("after kb,SwapFree:%s\n"),strInfo.Cstr()));

				nAvailable = strInfo.ToULong();
				YKTRACE((_U("SwapFree value:%lld\n"),nAvailable ));
			}
		}
		fileTemp.Close();
		return TRUE;
	}
	return FALSE;
#else 
	YKTRACE((_U("the OS must be tested to GetVirtualMemory")));
	return FALSE;
#endif
}

YkBool YkToolkit::GetMemoryStatus(MemoryStatus& memoryStatus)
{
	if (!GetPhysicalMemory(memoryStatus.m_nAvailablePhysical, memoryStatus.m_nTotalPhysical))
	{
		return FALSE;
	}

	if (!GetVirtualMemory(memoryStatus.m_nAvailableVirtual, memoryStatus.m_nTotalVirtual))
	{
		memoryStatus.m_nAvailableVirtual = 0; //memoryStatus.m_nAvailablePhysical;
		memoryStatus.m_nTotalVirtual = 0; //memoryStatus.m_nTotalPhysical;
		return FALSE;
	}
	return TRUE;
}

YkUlong YkToolkit::GetDiskSpace(const YkString& strPath)
{
	struct statvfs disk_statvfs;
	YkInt nError;
	YkMBString strMB;
	UNICODE2MBSTRING(strPath, strMB);
	nError = statvfs(strMB.Cstr(), &disk_statvfs); 
	if(nError != 0)
	{
		return 0;
	}
	else
	{
		return disk_statvfs.f_bavail*disk_statvfs.f_bsize; 
	}
}

}

#endif
