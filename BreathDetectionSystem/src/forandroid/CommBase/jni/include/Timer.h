
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
	* ������ʱ��
	* @param timerID �Զ���Ķ�ʱ����ʶ
	* @param milliseconds �����ٺ��봥��һ��
	* @param pCallBack ��ʱ����ʱʱ�ص��Ľӿ�
	* @param arg ��ʱ����ʱʱ���ص��Ľӿڴ��Ĳ���
	* @return �ɹ��򷵻�0
	*/
	int start(unsigned long timerID, unsigned long milliseconds,
						ITimerEventCallBack* pCallBack, void* arg = 0);
	/**
	* ֹͣ��ʱ��
	* @return �ɹ��򷵻�0
	*/
	int kill();

	/**
	* ��ʱ���������
	*/
	static int handle_OnTimer(unsigned long, void*);
	int handle_timeout();

private:
	unsigned long		userID_;	/**< �û��Զ���Ķ�ʱ����ʶ,�ڻص�ʱԭֵ���� */
	const void*			arg_;	/**< �û�������ʱ��ʱ�Զ���Ĳ���,�ڻص�ʱԭֵ���� */
	//struct timeval		timeval_;	/**< ��ʱ����� */
	unsigned long		timeval_;	/**< ��ʱ����� */
	ITimerEventCallBack*	pCallBack_;	/**< ��ʱ���ص��ӿ� */

	long				timerID_;	/**< �ڲ��õĶ�ʱ����ʶ */
};

#endif
