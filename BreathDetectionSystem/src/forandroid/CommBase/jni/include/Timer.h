
#ifndef _TIMER_H__
#define _TIMER_H__

#include "LinuxDef.h"

#include <list>
#include <sys/time.h>
#include <time.h>
//#include <signal.h>
#include <string>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "TimerEventCallBack.h"

struct TimerUnit
{
	TimerUnit()
	{
		m_totalCount = 0;
		m_currentCount = 0;
		m_pfun = NULL;
		m_tid = 0;
		m_flag = -1;
		m_data = NULL;
	}
	~TimerUnit()
	{
		m_totalCount = 0;
		m_currentCount = 0;
		m_pfun = NULL;
		m_tid = 0;
		m_flag = -1;
		m_data = NULL;
	}
	TimerUnit(const TimerUnit& other)
	{
		m_totalCount = other.m_totalCount;
		m_currentCount = other.m_currentCount;
		m_pfun = other.m_pfun;
		m_tid = other.m_tid;
		m_flag = other.m_flag;
		m_data = other.m_data;
	}

	TimerUnit& operator=(const TimerUnit& other)
	{
		if(this != &other)
		{
			m_totalCount = other.m_totalCount;
			m_currentCount = other.m_currentCount;
			m_pfun = other.m_pfun;
			m_tid = other.m_tid;
			m_flag = other.m_flag;
			m_data = other.m_data;
		}
		return (*this);
	}
	
	long m_totalCount; 
	long m_currentCount;
	//void (*m_pfun)(unsigned long,void *); //modify by zjj 2012-10-11
	int (*m_pfun)(unsigned long,void *);
	unsigned long m_tid;
	int 	      m_flag;		//add by zjj 2012-10-16
	void* m_data;
	bool operator==(unsigned long tid) const
	{
		return m_tid == tid;
	}
	bool operator<(unsigned long tid) const
	{
		return m_tid < tid;
	}
};

typedef TimerUnit TIMEUNIT;
typedef std::list<TIMEUNIT> LIST_TIMER;
typedef std::list<TIMEUNIT>::iterator IT_TIMER;


class Timer
{
public:
	Timer();
	~Timer();
	static Timer& getInstance();
	/*
	 *@param pfun  : callback When timer expires
	 *@param intervalMs  : interval milliseconds
	 *@param arg  : user define data
	 *@return 0 error ; >0 identifier of timer's id
	 */	
	//unsigned long start(void (*pfun)(unsigned long,void*),long intervalMs,void* arg);  //modify 2012-10-11
	unsigned long start(int (*pfun)(unsigned long,void*),long intervalMs,void* arg); 
	/*
	 *@param mid  : identifier of timer's id
	 *@return 0 sucess ; 1 error 
	 */	
	unsigned long kill(unsigned long mid);
	void handle_timeout();
private:
	bool open();
	bool close();
	IT_TIMER find( unsigned long tid);
	void updateNext();
	bool wait(unsigned int nTimeOut = INTERVAL_TIMER);
private:
	enum{INTERVAL_TIMER=1000};
	LIST_TIMER m_timerList;	
	pthread_mutex_t         m_mutext_;	
	pthread_mutexattr_t 	m_attr; //add 2012-12-16 by zjj
	bool m_isOpen;
	unsigned long m_nextId;
	pthread_t  m_TimerThreadId;
};

class CTimer
{
public:
	CTimer();
	~CTimer();

	/**
	* 启动定时器
	* @param timerID 自定义的定时器标识
	* @param milliseconds 隔多少毫秒触发一次
	* @param pCallBack 定时器到时时回调的接口
	* @param arg 定时器到时时给回调的接口传的参数
	* @return 成功则返回0
	*/
	int start(unsigned long timerID, unsigned long milliseconds,
						ITimerEventCallBack* pCallBack, void* arg = 0);
	/**
	* 停止定时器
	* @return 成功则返回0
	*/
	int kill();

	/**
	* 定时器调用入口
	*/
	static int handle_OnTimer(unsigned long, void*);
	int handle_timeout();

private:
	unsigned long		userID_;	/**< 用户自定义的定时器标识,在回调时原值传回 */
	const void*			arg_;	/**< 用户启动定时器时自定义的参数,在回调时原值传回 */
	//struct timeval		timeval_;	/**< 定时器间隔 */
	unsigned long		timeval_;	/**< 定时器间隔 */
	ITimerEventCallBack*	pCallBack_;	/**< 定时器回调接口 */

	long				timerID_;	/**< 内部用的定时器标识 */
};

#endif
