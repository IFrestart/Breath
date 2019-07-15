
#include "Timer.h"
#include <iostream>
#include <algorithm>



Timer::Timer() : m_isOpen(FALSE),m_nextId(1)
{
	pthread_mutexattr_init(&m_attr);
        pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE_NP);
//	pthread_mutex_init(&m_mutext_,NULL);
	pthread_mutex_init(&m_mutext_,&m_attr); //modify 2012-12-16 by zjj
}
Timer::~Timer()
{
	close();
	pthread_mutex_destroy(&m_mutext_);
	pthread_mutexattr_destroy(&m_attr); //add 2012-12-16 by zjj
}

Timer& Timer::getInstance()
{
	static Timer t;
	return t;
}
/*
	return 0 ; error
	return > 0 ; mid
*/
unsigned long Timer::start(int (*pfun)(unsigned long,void*),long intervalMs,void* arg)
{
	TIMEUNIT tUnit;
	tUnit.m_totalCount = (intervalMs )/INTERVAL_TIMER;
	if(0 != ((intervalMs ) % INTERVAL_TIMER))
	{
		tUnit.m_totalCount += 1;
	}
	tUnit.m_currentCount = 0;
	tUnit.m_pfun = pfun; 
	tUnit.m_data = arg; 
	pthread_mutex_lock(&m_mutext_);
	if(!m_isOpen)
	{
		if(FALSE == open())
		{
			pthread_mutex_unlock(&m_mutext_);
			return 0;
		}
	}
	tUnit.m_tid = m_nextId  ;
	tUnit.m_flag = m_nextId  ; //add 2012-10-16
	IT_TIMER it = find(m_nextId);
//printf("Timer::start id [%d] insert m_pfun =[%x] data[%x]\n",tUnit.m_tid,tUnit.m_pfun,tUnit.m_data);
	m_timerList.insert(it,tUnit);
	updateNext();
	pthread_mutex_unlock(&m_mutext_);
	return tUnit.m_tid;
}

unsigned long Timer::kill(unsigned long tid)
{
//add by zjj 2010-1-29
	if(0 == tid)
        {
                return 1;
        }
//end add by zjj 2010-1-29
	IT_TIMER it;

	pthread_mutex_lock(&m_mutext_);
	//it = find(tid); 
	it = std::find(m_timerList.begin(),m_timerList.end(),tid);
	if(it == m_timerList.end())
	{
		pthread_mutex_unlock(&m_mutext_);
		return 1;
	}
	//modify by zjj 2012-10-16
	/*
	if(it->m_tid < m_nextId)
	{
		m_nextId = it->m_tid;
	}
	m_timerList.erase(it); 
	*/
	it->m_flag = -1;
	//end modify by zjj 2012-10-16
	pthread_mutex_unlock(&m_mutext_);
	return 0;
}


/*
static void handle_timeout1(int mid)
{
	Timer::getInstance().handle_timeout();
}
bool Timer::open()
{
	struct sigaction act,oact;
	act.sa_handler = handle_timeout1;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags |= SA_INTERRUPT;
	if(sigaction(SIGALRM,&act,&oact) < 0)
	{
		return FALSE;
	}
	struct itimerval tval;
	tval.it_interval.tv_sec = 0;
	tval.it_interval.tv_usec = INTERVAL_TIMER;
	tval.it_value.tv_sec = 0;
	tval.it_value.tv_usec = INTERVAL_TIMER ;
	if(-1 == setitimer(ITIMER_REAL, &tval, NULL))
	{
		return FALSE;
	}
	return TRUE;
}

bool Timer::close()
{
	struct itimerval tval;
	tval.it_interval.tv_sec = 0;
	tval.it_interval.tv_usec = 0;
	tval.it_value.tv_sec = 0;
	tval.it_value.tv_usec = 0;
	if(-1 == setitimer(ITIMER_REAL, &tval, NULL))
	{
		return FALSE;
	}
	pthread_mutex_lock(&m_mutext_);
	m_timerList.clear();
	m_nextId = 1;
	pthread_mutex_unlock(&m_mutext_);
	return TRUE;
}

void Timer::handle_timeout()
{
	IT_TIMER it;
	pthread_mutex_lock(&m_mutext_);
	TIMEUNIT tUnit  ;
	for(it = m_timerList.begin(); it != m_timerList.end(); it++)
	{
		it->m_currentCount++;
		if(it->m_totalCount == it->m_currentCount)
		{
			it->m_currentCount = 0;
			it->m_pfun(it->m_tid,it->m_data);
		}
	}
	pthread_mutex_unlock(&m_mutext_);
}

*/

void Timer::handle_timeout()
{
	IT_TIMER it;
	while(1)
	{
		wait(INTERVAL_TIMER);
		pthread_mutex_lock(&m_mutext_);
		if(!m_isOpen)
		{
			pthread_mutex_unlock(&m_mutext_);
			break;
		}
		TIMEUNIT tUnit  ;
		//for(it = m_timerList.begin(); it != m_timerList.end(); it++) //modify 2012-10-11
		for(it = m_timerList.begin(); it != m_timerList.end(); )
		{
//add 2012-10-16
			if(-1 == it->m_flag)
			{
				if(it->m_tid < m_nextId)
				{
					m_nextId = it->m_tid;
				}
				m_timerList.erase(it++);
				continue;
			}
//end add 2012-10-16
			it->m_currentCount++;
			if(it->m_totalCount == it->m_currentCount)
			{
				it->m_currentCount = 0;
//modify by zjj 2012-10-11
				//it->m_pfun(it->m_tid,it->m_data);
				if(it->m_pfun(it->m_tid,it->m_data)) 
				{
					if(it->m_tid < m_nextId)
					{
						m_nextId = it->m_tid;
					}
					m_timerList.erase(it++);
					continue;
				}
//end modify by zjj 2012-10-11

			}
			it++;  //add by zjj 2012-10-11
		}
		pthread_mutex_unlock(&m_mutext_);
	}
}
static void* DealTimerProc(void* param)
{
	Timer* ptime  = (Timer* )param;
	ptime->handle_timeout();
}

bool Timer::open()
{
	if(0 != pthread_create(&m_TimerThreadId, NULL, DealTimerProc, (void *)this))
	{
		return FALSE;
	}
	m_isOpen = TRUE;
	return TRUE;
}
	
bool Timer::wait(unsigned int nTimeOut)
{
 //       fd_set recvFds;
        //FD_ZERO(&recvFds);
        //FD_SET(sock, &recvFds);

        struct timeval tvTimeOut;
        tvTimeOut.tv_sec = nTimeOut/1000;
        tvTimeOut.tv_usec = (nTimeOut%1000)*1000;
        //return select(0, &recvFds, NULL, NULL, &tvTimeOut);
        return select(0, NULL, NULL, NULL, &tvTimeOut);
}

bool Timer::close()
{
	pthread_mutex_lock(&m_mutext_);
	m_timerList.clear();
	m_nextId = 1;
	m_isOpen = FALSE;
	pthread_mutex_unlock(&m_mutext_);
	pthread_join(m_TimerThreadId,NULL);	
	return TRUE;
}

void Timer::updateNext()
{
        m_nextId++;
        IT_TIMER it = find(m_nextId);
        for(; it != m_timerList.end(); it ++, m_nextId ++) {
                if(m_nextId < it->m_tid) {
                        break;
                }
        }
}

IT_TIMER Timer::find( unsigned long tid)
{
	return std::lower_bound(m_timerList.begin(),m_timerList.end(),tid);
}
#if 0
unsigned long timeSetEvent( UINT   uDelay, UINT uResolution, int (*lpTimeProc)(unsigned long,void*) , DWORD_PTR dwUser,UINT fuEvent )
{
	unsigned long tid = 0;
	tid = Timer::getInstance().start(lpTimeProc,uDelay,(void*)dwUser);
	return tid;
}

unsigned long timeKillEvent(UINT tid)
{
	return Timer::getInstance().kill(tid);
}

unsigned long SetTimer(  void* dwUser, UINT nElapse, UINT uDelay, int (*lpTimeProc)(unsigned long,void*))
{
        unsigned long tid = 0;
        tid = Timer::getInstance().start(lpTimeProc,uDelay,(void*)dwUser);
        return tid;
}

unsigned long KillTimer(void* dwUser,UINT tid)
{
        return Timer::getInstance().kill(tid);
}
#endif

CTimer::CTimer()
		: timerID_(-1), userID_(0),
		pCallBack_(0), arg_(0)
{
}

CTimer::~CTimer()
{
	kill();
}

//int CTimer::handle_timeout(const ACE_Time_Value& now, const void* act)
int CTimer::handle_OnTimer(unsigned long now,  void* act)
{
	CTimer* pTimer = (CTimer*) act;
	return pTimer->handle_timeout();
}

int CTimer::handle_timeout()
{
	int nRet = pCallBack_->OnTimer(userID_, (void*)arg_);
	if(nRet)
	{
		timerID_ = -1;
	}
	return nRet;
}

int CTimer::start(unsigned long timerID, unsigned long milliseconds,
					ITimerEventCallBack* pCallBack, void* arg)
{

	if(-1 != timerID_) {
		return -1;
	}

	userID_ = timerID;
	timeval_ = milliseconds;
	pCallBack_ = pCallBack;
	arg_ = arg;

	//ACE_Time_Value	tv(timeval);

	//timerID_ = ACE_Reactor::instance()->schedule_timer(this, arg, tv, tv);
        timerID_ = Timer::getInstance().start(handle_OnTimer,milliseconds,(void*)this);
	if(0 == timerID_)
	{
		timerID_ = -1;
	}
	return -1 == timerID_ ? -1 : 0;
}

int CTimer::kill()
{
	if(-1 == timerID_) {
		return -1;
	}

	//int	ret = ACE_Reactor::instance()->cancel_timer(timerID_);
	int ret =  Timer::getInstance().kill(timerID_);
	if(0 == ret) {
		timerID_ = -1;
	}

	return 0 == ret ? 0 : -1;
}

