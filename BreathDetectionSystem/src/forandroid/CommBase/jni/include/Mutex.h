/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ��������,�ṩ�˼���������Ȳ���

 * @file	Mutex.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	��������
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
	* ����������,���˻������Ѿ��������߳�����,������ֱ������
	* @return �ɹ��򷵻�0
	*/
	int lock();

	/**
	* �ͷŻ�����
	* @return �ɹ��򷵻�0
	*/
	int unlock();

	/**
	* �����Ƿ�������������
	* @return ���������򷵻�0
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

