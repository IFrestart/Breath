/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 互斥体类,提供了加锁与解锁等操作

 * @file	Mutex.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	互斥体类
 **/

#ifndef	__CMUTEX_H__
#define	__CMUTEX_H__

//#include "ace/Thread_Mutex.h"
#include "LinuxDef.h"

class CMutex 
{
public:
	CMutex();
	~CMutex();

	/**
	* 锁定互斥体,若此互斥体已经被其它线程锁定,则阻塞直到锁定
	* @return 成功则返回0
	*/
	int lock();

	/**
	* 释放互斥体
	* @return 成功则返回0
	*/
	int unlock();

	/**
	* 测试是否能锁定互斥体
	* @return 若能锁定则返回0
	*/
	int tryLock();

protected:

private:
	//ACE_Thread_Mutex	mutex_;
	pthread_mutex_t	mutex_;
};

class CMutexKeeper
{
public:
	CMutexKeeper(CMutex& mutex) : mutex_(mutex)
	{
		mutex_.lock();
	}

	~CMutexKeeper()
	{
		mutex_.unlock();
	}

private:
	CMutex&		mutex_;
};

#endif	/*__MUTEX_H__*/

