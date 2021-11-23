/*
 *
 * YkThread.h
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

#ifndef AFX_YKTHREAD_H__5432FB9A_A247_481A_BE7E_F5BCB9E661E4__INCLUDED_
#define AFX_YKTHREAD_H__5432FB9A_A247_481A_BE7E_F5BCB9E661E4__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include "Stream/YkPlatform.h"

namespace Yk {

class YkCondition;

class TOOLKIT_API YkMutex {
  friend class YkCondition;
private:
	unsigned long dataArray[24];	WhiteBox_Ignore
private:
  YkMutex(const YkMutex&);
  YkMutex &operator=(const YkMutex&);
public:

  /// Initialize the mutex
  YkMutex(YkBool recursive=FALSE);

  /// Lock the mutex
  void lock();

  /// Return true if succeeded locking the mutex
  YkBool trylock();

  /// Return true if mutex is already locked
  YkBool locked();

  /// Unlock mutex
  void unlock();

  /// Delete the mutex
  ~YkMutex();
  };



/**
* An easy way to establish aValue correspondence between aValue C++ scope
* and aValue critical section is to simply declare an YkMutexLock
* at the beginning of the scope.
* The mutex will be automatically released when the scope is
* left (either by natural means or by means of an exception.
*/
class TOOLKIT_API YkMutexLock {
private:
  YkMutex& mtx;
private:
  YkMutexLock();
  YkMutexLock(const YkMutexLock&);
  YkMutexLock& operator=(const YkMutexLock&);

public:
  YkMutexLock(YkMutex& m):mtx(m){}

  /// Return reference to associated mutex
  YkMutex& mutex(){ return mtx; }


  /// Lock mutex
  void lock(){ mtx.lock(); }

  /// Return true if succeeded locking the mutex
  YkBool trylock(){ return mtx.trylock(); }

  /// Return true if mutex is already locked
  YkBool locked(){ return mtx.locked(); }

  /// Unlock mutex
  void unlock(){ mtx.unlock(); }

  /// Destroy and unlock associated mutex
  ~YkMutexLock(){ unlock(); }
  };



/**
* A read / write lock allows multiple readers but only aValue single
* writer.
*/
class TOOLKIT_API YkReadWriteLock {
private:
  Ykuval dataArray[24];
private:
  YkReadWriteLock(const YkReadWriteLock&);
  YkReadWriteLock &operator=(const YkReadWriteLock&);
public:

  /// Initialize the read/write lock
  YkReadWriteLock();

  /// Acquire read lock for read/write lock
  void readLock();

  /// Try to acquire read lock for read/write lock
  bool tryReadLock();

  /// Unlock read lock
  void readUnlock();

  /// Acquire write lock for read/write lock
  void writeLock();

  /// Try to acquire write lock for read/write lock
  bool tryWriteLock();

  /// Unlock write lock
  void writeUnlock();

  /// Delete the read/write lock
  ~YkReadWriteLock();
};

//! \brief 自动锁。
class YkAutoLock
{
public:
	explicit YkAutoLock(YkMutex& locker) : m_locker(locker) { m_locker.lock(); }
	virtual ~YkAutoLock() { m_locker.unlock(); }

private:
	YkAutoLock& operator= (const YkAutoLock&);

private:
	YkMutex& m_locker;
};

//! \brief 读者操作锁。
class YkReadLock
{
public:
	explicit YkReadLock(YkReadWriteLock& locker) : m_locker(locker) { m_locker.readLock(); }
	virtual ~YkReadLock() { m_locker.readUnlock(); }

private:
	YkReadLock(const YkReadLock&);
	YkReadLock& operator= (const YkReadLock&);

private:
	YkReadWriteLock& m_locker;
};

//! \brief 作者操作锁。
class YkWriteLock
{
public:
	explicit YkWriteLock(YkReadWriteLock& locker) : m_locker(locker) { m_locker.writeLock(); }
	virtual ~YkWriteLock() { m_locker.writeUnlock(); }

private:
	YkWriteLock(const YkWriteLock&);
	YkWriteLock& operator= (const YkWriteLock&);

private:
	YkReadWriteLock& m_locker;
};

/**
* A condition allows one or more threads to synchronize
* to an event.  When aValue thread calls wait, the associated
* mutex is unlocked while the thread is blocked.  When the
* condition becomes signaled, the associated mutex is
* locked and the thread(s) are reawakened.
*/
class TOOLKIT_API YkCondition {
private:
  Ykuval dataArray[12];
private:
  YkCondition(const YkCondition&);
  YkCondition& operator=(const YkCondition&);
public:

  /// Initialize the condition
  YkCondition();

  /**
  * Wait until condition becomes signalled, using given mutex,
  * which must already have been locked prior to this call.
  */
  void Wait(YkMutex& mtx);

  /**
  * Wait until condition becomes signalled, using given mutex,
  * which must already have been locked prior to this call.
  * Returns true if successful, false if timeout occurred.
  */
  YkBool Wait(YkMutex& mtx,YkUint nsec);
  /**
  * Wake or unblock aValue single blocked thread
  */
  void signal();

  /**
  * Wake or unblock all blocked threads
  */
  void broadcast();

  /// Delete the condition
  ~YkCondition();
  };



/**
* A semaphore allows for protection of aValue resource that can
* be accessed by aValue fixed number of simultaneous threads.
*/
class TOOLKIT_API YkSemaphore {
private:
    Ykuval dataValues[16];

private:
  YkSemaphore(const YkSemaphore&);
  YkSemaphore& operator=(const YkSemaphore&);
public:

  /// Initialize semaphore with given count
  YkSemaphore(YkInt initial=1);

  /// Decrement semaphore
  void Wait();

  /// Non-blocking semaphore decrement; return true if locked
  YkBool TryWait();

  /// Increment semaphore
  void Post();

  /// Delete semaphore
  ~YkSemaphore();
  };



/**
* YkThread provides system-independent support for threads.
* Subclasses must implement the run() function do implement
* the desired functionality of the thread.
* The storage of the YkThread object is to be managed by the
* calling thread, not by the thread itself.
*/
class TOOLKIT_API YkThread {
private:
  volatile YkThreadID tid;
  volatile YkUint thlid;
private:
  YkThread(const YkThread&);
  YkThread &operator=(const YkThread&);

  static YkHandle YKCALLBACK execute(void*);

public:
  //! \brief 得到当前线程的ID
  //! \return 返回当前线程的ID
    static YkUint GetCurrentThreadId();

public:

  /// 线程策略级别
  enum Priority {
    PRIORITY_ERROR=-1,  /// Failed to get priority
    PRIORITY_DEFAULT,  	/// Default scheduling priority
    PRIORITY_MINIMUM,   /// Minimum scheduling priority
    PRIORITY_LOWER,     /// Lower scheduling priority
    PRIORITY_MEDIUM,    /// Medium priority
    PRIORITY_HIGHER,    /// Higher scheduling priority
    PRIORITY_MAXIMUM    /// Maximum scheduling priority
    };

  /// Thread scheduling policies
  enum Policy {
    POLICY_ERROR=-1,    /// Failed to get policy
    POLICY_DEFAULT,     /// Default scheduling
    POLICY_FIFO,        /// First in, first out scheduling
    POLICY_ROUND_ROBIN 	/// Round-robin scheduling 
    };

protected:

  /**
  * All threads execute by deriving the run method of YkThread.
  * If an uncaught exception was thrown, this function returns -1.
  */
  virtual YkInt run() = 0;

public:

  /// Initialize thread object.
  YkThread();

  /**
  * Return handle of this thread object.
  * This handle is valid in the context of the thread which
  * called start().
  */
  YkThreadID id() const;

  YkUint getcallingthreadid();

  /**
  *  返回表示线程是否属于运行状态
  */
  YkBool running() const;

  /**
  *  启动线程
  */
  YkBool start(YkUlong stacksize=0);	

  /**
  * 挂起调用线程，直到线程结束，Suspend calling thread until thread is done.
  */
  YkBool join();

  /**
  * 挂起调用线程，直到线程结束 Suspend calling thread until thread is done, and set code to the return value of run() or the argument passed into exit().
  * If an exception happened in the thread, return -1.
  */	  
  YkBool join(YkInt& code);

  /**
  * 取消线程 Cancel the thread
  */	  
  YkBool cancel();

  YkBool detach();

  /**
  * 退出线程
  */
  static void exit(YkInt code=0);
  
  static void yield();

  /**
  * 返回时间，Return time in nanoseconds since Epoch (Jan 1, 1970).
  */
  static YkUlong time();

  static void sleep(YkUint nsec);

  static void wakeat(YkUint nsec);

  static YkThread* self();

  static YkThreadID current();

  YkBool priority(Priority prio);

  Priority priority() const;

  YkBool policy(Policy plcy);

  Policy policy() const;
  
  virtual ~YkThread();
  };



class YkThreadEx;
class TOOLKIT_API YkThreadManager
{
	friend class YkThreadEx;
	//!  \brief 回调函数，线程的回调函数
	typedef  YkInt (*ThreadFunCallBack)(void*);
public:
	~YkThreadManager();
private:
	YkThreadManager();
	YkThreadManager(const YkThreadManager&);
	YkThreadManager &operator=(const YkThreadManager&);
public:
	//获取工厂类的唯一实例
	static YkThreadManager& GetInstance();
	//!  \brief 创建一个线程，根据线程名字，线程函数，线程函数参数，是否托管
	//!  isManaged 主要线程如果设置为托管，则在线程结束的时候自动调用YkThreadFactory 类删除自己
	//! 如果 strName有重名或者是空的，pFun为空上述情况会创建失败
	YkThreadEx* CreateThread(const YkString& strName,ThreadFunCallBack pFun,void* pParameter,YkBool isManaged =FALSE);
	//!  \brief 删除一个线程
	//!  \param 如果线程没有终止，线程会强制杀死，会有一些不可知的问题，不推荐这样做，推荐自己实现
	//thread正常杀死和return
	void DestoryThread(const YkString& strName);
	//!  \brief 获得当前机器的最优线程个数
	YkUint GetOptimalCount() const;
	//!  \brief 获取当前有效的线程个数，指管理线程的个数
	YkInt GetActiveCount();
	//!  \brief 获取线程
	//! \brief 如果线程不存在则返回NULL
	YkThreadEx* GetThread(const YkString& strName);
	//!  \brief 取消托管
	//! \brief 如果线程不存在则返回失败
	YkBool CancleManaged(const YkString& strName);
private:
	//!  \brief 删除一个线程,这个方法仅供YkThreadEx内部调用，和DestoryThread保持一致
	// 主要用于托管删除
	void DestoryManagedThread(const YkString& strName);

	YkMutex m_mutex;
	YkArray<YkThreadEx*> m_arrayThread;
};


class TOOLKIT_API YkThreadEx
{
	friend class YkThreadManager;

	static YkHandle YKCALLBACK execute(void*);

private:
	YkThreadEx(const YkThreadEx&);
	YkThreadEx &operator=(const YkThreadEx&);
public:
	YkThreadEx();
	~YkThreadEx();

  /// 线程策略级别
  enum Priority {
    PRIORITY_ERROR=-1,  /// Failed to get priority
    PRIORITY_DEFAULT,  	/// Default scheduling priority
    PRIORITY_MINIMUM,   /// Minimum scheduling priority
    PRIORITY_LOWER,     /// Lower scheduling priority
    PRIORITY_MEDIUM,    /// Medium priority
    PRIORITY_HIGHER,    /// Higher scheduling priority
    PRIORITY_MAXIMUM    /// Maximum scheduling priority
    };

  /// Thread scheduling policies
  enum Policy {
    POLICY_ERROR=-1,    /// Failed to get policy
    POLICY_DEFAULT,     /// Default scheduling
    POLICY_FIFO,        /// First in, first out scheduling
    POLICY_ROUND_ROBIN 	/// Round-robin scheduling 
    };

	enum Thread_Status
	{
	  //至今尚未启动的线程处于这种状态。 
	  THREAD_NEW,
	  //正在执行的线程处于这种状态。 
	  THREAD_RUNNING,
	  //受阻塞并等待某个监视器锁的线程处于这种状态。
	  THREAD_BLOCKED, 
	  //无限期地等待另一个线程来执行某一特定操作的线程处于这种状态。 
	  THREAD_WAITING,
	  //等待另一个线程来执行取决于指定等待时间的操作的线程处于这种状态。 
	  THREAD_TIMED_WAITING,
	  //已退出的线程处于这种状态。
	  TERMINATED,
	  // 未知错误
	  THREAD_UNKNOWN=0xFFFFFFFF,
	};

public:
	//! \brief 获取线程的ID
	//! \breif如果线程已经终止，则线程ID返回不准确
	YkUint GetID(){return m_nID;};

	//! \brief 获取线程的Name;
	YkString GetName();

	//! \brief 设置线程启动时候的堆栈大小
	//!   必须在start之前调用
	void SetStackSize(YkUint nSize);

	//! \brief 设置线程启动时候的优先级大小
	//!   最好在start之前调用
	YkBool SetPriority(Priority prio);
	YkThreadEx::Priority GetPriority() const;

	//! \brief 设置线程的调度策略，这个方法只在linux上面有效
	void SetPolicy(Policy ploi);

	//! \brief 返回线程的状态
	YkThreadEx::Thread_Status GetStatus() const;
	//! \brief测试线程是否处于活动状态
	YkBool IsAlive() const;

	//!  \brief 开始线程
	YkBool Start();

	//!  \brief 挂起调用线程，直到线程结束
	YkBool Join();

	//!  \brief 获得线程结束的返回值
    //! \return 返回当前线程的ID
	YkInt GetReturnValue();
    static YkUint GetCurrentThreadId();
	
	//!  \brief 让当前线程暂时睡眠一段时间，单位毫秒
	static void Sleep(YkUlong nMillsecond);

	//!  \brief 让当前线程暂停一段时间，让系统调度其他线程，和yield线程的意思保持一致
	static YkBool SwitchToThread();
private:
	void Init();
	//线程ID
	YkUint m_nID;
    YkInt m_nReturnValue;

	//线程名字
	YkString m_strName;
	//线程handle 在Linux中没有用到
	void* m_pHandle;
	//在Linux中没有handle的概念，m_nID不变，用下面这个变量表示状态 只在Linux下使用
	Thread_Status m_tSatus;

	YkBool m_bManaged;
	YkUint m_nStackSize;
	Priority m_nPriority;
	Policy m_nPolicy;

public:
	// 开放出来给线程池用
	YkThreadManager::ThreadFunCallBack m_pFun;
	void* m_pParameter;
 };

}

#endif




