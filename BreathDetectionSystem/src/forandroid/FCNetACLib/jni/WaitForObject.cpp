#include "WaitForObject.h"
#include <sys/time.h>
#include <algorithm>

//begin add for MultipleSignal
MultipleObjects::MultipleObjects()
{
	pthread_cond_init(&m_rcond_,NULL);
        pthread_mutex_init(&m_mutext_,NULL);
	reset();
}
MultipleObjects::~MultipleObjects()
{
        pthread_cond_destroy(&m_rcond_);
        pthread_mutex_destroy(&m_mutext_);
}

long MultipleObjects::WaitForMultipleObjects(DWORD nCount, BOOL bWaitAll,DWORD dwMilliseconds)
{
	struct timeval now;
	struct timespec timeout;
	struct timezone tz;
	long retcode;

	gettimeofday(&now,&tz);
	//timeout.tv_sec = now.tv_sec + 5;
	//timeout.tv_nsec = now.tv_usec * 1000 ;
	timeout.tv_sec = now.tv_sec + dwMilliseconds / 1000;
	timeout.tv_nsec = now.tv_usec +  (dwMilliseconds % 1000) * 1000 ; // modify by zjj 2010-7-12
	//timeout.tv_nsec = now.tv_usec * 1000 +  (dwMilliseconds % 1000) * 1000000 ; //remodify by zjj 2010-10-10
	retcode = WAIT_OBJECT_0;
	bool canRet = FALSE;
	pthread_mutex_lock(&m_mutext_);
	while ((!canRet) && (retcode != ETIMEDOUT))
	{
		if (retcode == ETIMEDOUT) 
		{
			retcode = WAIT_TIMEOUT;
			break;
		} 
		else if((retcode == EBUSY )||(retcode == EINTR))
		{
			continue;
		}
		for(int i = 0; i < nCount ; i++)
		{	
			if(m_bit.test(i))	
			{
				retcode = WAIT_OBJECT_0 + i;
				if(!bWaitAll)
				{
					canRet = TRUE;
					break;
				}
				else
				{
					if(nCount == m_bit.count() )	
					{
						canRet = TRUE;
						retcode = nCount;
						break;
					}
				}
			}
		}
		if(!canRet)
		{
	      		retcode = pthread_cond_timedwait(&m_rcond_, &m_mutext_, &timeout);
		}
	}
	pthread_mutex_unlock(&m_mutext_);
	return retcode;
}

void MultipleObjects::signal()
{
	pthread_cond_signal(&m_rcond_);
}

void MultipleObjects::reset()
{
	m_bit.reset();
}
bool MultipleObjects::set(size_t index)
{
	if(index >=16)
	return FALSE;

	pthread_mutex_lock(&m_mutext_);
	m_bit.set(index);
	pthread_mutex_unlock(&m_mutext_);

	return TRUE;
}

bool MultipleObjects::reset(size_t index)
{
	if(index >=16)
	return FALSE;

	pthread_mutex_lock(&m_mutext_);
	m_bit.reset(index);
	pthread_mutex_unlock(&m_mutext_);

	return TRUE;
}

NotifyMultipleObjects::NotifyMultipleObjects() : m_pUnit(NULL),m_opIndex(0),m_unitId(0)
{
}

NotifyMultipleObjects::~NotifyMultipleObjects()
{
	m_pUnit = NULL;
	m_unitId = 0;
}

NotifyMultipleObjects::NotifyMultipleObjects(const NotifyMultipleObjects& other)
{
	m_pUnit = other.m_pUnit;
	m_opIndex = other.m_opIndex;
	m_unitId = other.m_unitId;
}

NotifyMultipleObjects& NotifyMultipleObjects::operator= (const NotifyMultipleObjects& other)
{
	if(this != &other)
	{
		m_pUnit = other.m_pUnit;
		m_opIndex = other.m_opIndex;
		m_unitId = other.m_unitId;
	}
	return(*this);
}

bool NotifyMultipleObjects::operator== ( const NotifyMultipleObjects& other ) const
{
	return(m_unitId == other.m_unitId);
}

void NotifyMultipleObjects::set()
{
	if(NULL != m_pUnit)
		m_pUnit->set(m_opIndex);
}

void NotifyMultipleObjects::reset()
{
	if(NULL != m_pUnit)
		m_pUnit->reset(m_opIndex);
}

void NotifyMultipleObjects::signal()
{
	if(NULL != m_pUnit)
		m_pUnit->signal();
}

void NotifyMultipleObjects::setMultipleObjects(MultipleObjects* pUnit)
{
	m_pUnit = pUnit;
}

void NotifyMultipleObjects::setId(long id)
{
	m_unitId = id;
}
void NotifyMultipleObjects::setIndex(size_t index)
{
	m_opIndex = index;
}

//end add 

HandleForWin::HandleForWin(void* attr,BOOL mflag, BOOL init, WORD type,void* name,long max )
{
	pthread_cond_init(&m_rcond_,NULL);
        pthread_mutex_init(&m_mutext_,NULL);
	m_type = type;
	m_flag = mflag;
	if(init)
	{
		m_sig = 1;
	}
	else
	{
		m_sig = 0;
	}
	m_sigmax = max;

//begin add for MultipleSignal
        pthread_mutex_init(&m_mutextList_,NULL);
	m_NotifyList.clear();
//end add
	
}

HandleForWin::~HandleForWin()
{
        pthread_cond_destroy(&m_rcond_);
        pthread_mutex_destroy(&m_mutext_);

//begin add for MultipleSignal
        pthread_mutex_destroy(&m_mutextList_);
	m_NotifyList.clear();
//end add
}

//begin add for MultipleSignal
BOOL HandleForWin::addNotifyUnit(NOTIFYMULTIPLEOBJECTS& unit)
{
	IT_NotifyMultipleObjectsList unit_it;	

        pthread_mutex_lock(&m_mutextList_);
	unit_it = std::find(m_NotifyList.begin(),m_NotifyList.end(),unit); 
	if(m_NotifyList.end() != unit_it )
	{
        	pthread_mutex_unlock(&m_mutextList_);
		return FALSE;
	}
	m_NotifyList.push_back(unit);	
        pthread_mutex_unlock(&m_mutextList_);
	return TRUE;
}

BOOL HandleForWin::delNotifyUnit(long unitId)
{
	
	IT_NotifyMultipleObjectsList unit_it;	
	NOTIFYMULTIPLEOBJECTS tmpUnit ;
	tmpUnit.setId(unitId);
        pthread_mutex_lock(&m_mutextList_);
	unit_it = std::find(m_NotifyList.begin(),m_NotifyList.end(),tmpUnit);
	if(m_NotifyList.end() == unit_it )
	{
        	pthread_mutex_unlock(&m_mutextList_);
		return FALSE;
	}
	m_NotifyList.erase(unit_it);	
        pthread_mutex_unlock(&m_mutextList_);
	return TRUE;
}
/*
 @param opFlag == 1  is SetEvent ; opFlag == 0  is ResetEvent
*/
BOOL HandleForWin::eventNotifyMuilt(int opFlag)
{
        pthread_mutex_lock(&m_mutextList_);
	if(m_NotifyList.size() > 0)
	{
		IT_NotifyMultipleObjectsList unit_it;	
		for(unit_it = m_NotifyList.begin(); unit_it != m_NotifyList.end(); unit_it++)
		{
			if(1 == opFlag)
			{
				unit_it->set();
				unit_it->signal();
			}
			else if(0 == opFlag)
			{
				unit_it->reset();
			}
		}
	}
        pthread_mutex_unlock(&m_mutextList_);
}

void HandleForWin::dealHaveNotifyMuilt()
{
	pthread_mutex_lock(&m_mutext_);
	if(m_type == HANDLEFOREVENT)
	{
		if(!m_flag)
		{
			m_sig = 0;
		}
	}
	else if(m_type == HANDLEFORSEM)
	{
		if(m_sig > 0)
			m_sig--;
	}
	pthread_mutex_unlock(&m_mutext_);
}

//end add


BOOL HandleForWin::SetEvent()
{
	if(m_type != HANDLEFOREVENT)
	{
		return FALSE;
	}
        pthread_mutex_lock(&m_mutext_);
	m_sig = 1;
        pthread_mutex_unlock(&m_mutext_);
	pthread_cond_signal(&m_rcond_);


//begin add for MultipleSignal
	eventNotifyMuilt(1);
//end add
	return TRUE;;
}

BOOL HandleForWin::ResetEvent()
{
	if(m_type != HANDLEFOREVENT)
	{
		return FALSE;
	}
        pthread_mutex_lock(&m_mutext_);
	m_sig = 0;
        pthread_mutex_unlock(&m_mutext_);

//begin add for MultipleSignal
	eventNotifyMuilt(0);
//end add
	return TRUE;;
}

BOOL HandleForWin::Release(long releaseCount , long* resultCount)
{
	BOOL ret = TRUE;
	if(m_type != HANDLEFORSEM)
	{
		return FALSE;
	}
        pthread_mutex_lock(&m_mutext_);

	if(NULL != resultCount )
	{
	 	*resultCount = m_sig;
	}
	if(m_sig + releaseCount <= m_sigmax)
	{
		m_sig += releaseCount;
	}
	else
	{
		ret = FALSE;
	}
        pthread_mutex_unlock(&m_mutext_);
	if(ret)
	{
		pthread_cond_signal(&m_rcond_);
	}
//begin add for MultipleSignal
	eventNotifyMuilt(1);
//end add
	return ret;
}

long HandleForWin::WaitForSingleObject(long dwMilliseconds)
{
	struct timeval now;
	struct timespec timeout;
	struct timezone tz;
	long retcode;

	pthread_mutex_lock(&m_mutext_);
	gettimeofday(&now,&tz);
	//timeout.tv_sec = now.tv_sec + 5;
	//timeout.tv_nsec = now.tv_usec * 1000 ;
	timeout.tv_sec = now.tv_sec + dwMilliseconds / 1000;
	timeout.tv_nsec = now.tv_usec +  (dwMilliseconds % 1000) * 1000 ; //modify by zjj 2010-7-12
	//timeout.tv_nsec = now.tv_usec * 1000 +  (dwMilliseconds % 1000) * 1000000 ; //remodify by zjj 2010-10-10
	retcode = WAIT_OBJECT_0;
	while ((0 == m_sig)&&(retcode != ETIMEDOUT)) 
	{
	      	retcode = pthread_cond_timedwait(&m_rcond_, &m_mutext_, &timeout);
		if (retcode == ETIMEDOUT) 
		{
			retcode = WAIT_TIMEOUT;
			break;
		} 
		else if((retcode == EBUSY )||(retcode == EINTR))
		{
			continue;
		}
		else
		{
			retcode = WAIT_OBJECT_0;
			break;
		}
	}
	if(m_type == HANDLEFOREVENT)
	{
		if(!m_flag)
		{
			m_sig = 0;
		}
	}
	else if(m_type == HANDLEFORSEM)
	{
		if(m_sig > 0)
			m_sig--;
	}
	pthread_mutex_unlock(&m_mutext_);
	return retcode;
}


HANDLE CreateEvent(void* attr,BOOL mflag, BOOL init, void* name)
{
	HandleForWin *pevent = new HandleForWin(attr,mflag,init,HANDLEFOREVENT,name);	
	return pevent;
}


BOOL SetEvent(HANDLE h)
{
	h->SetEvent();
}

BOOL ResetEvent(HANDLE h)
{
	h->ResetEvent();
}

BOOL CloseHandle(HANDLE h)
{
	delete h;
	h = NULL;
	return TRUE;
}

HANDLE CreateSemaphore(void* attr,long initCount, long maxCount, void* name)
{
	HandleForWin *pevent = new HandleForWin(attr,TRUE,initCount,HANDLEFORSEM,name,maxCount);	
	return pevent;
}

BOOL ReleaseSemaphore(HANDLE h, long releaseCount, long* resultCount)
{
	return h->Release( releaseCount ,  resultCount);
}

long WaitForSingleObject(HANDLE h,long dwMilliseconds)
{
	return h->WaitForSingleObject(dwMilliseconds);
}

//NNN notice here need modify for linux
/*
long WaitForMultipleObjects(DWORD nCount, HANDLE* lpHandles, BOOL bWaitAll,DWORD dwMilliseconds)
{
	DWORD retcode = 0;
	int count = 0;
	for(int i = 0; i < nCount; i++)
	{
		retcode = WaitForSingleObject(lpHandles[i], dwMilliseconds);
		if(retcode == WAIT_OBJECT_0)
		{
			if(!bWaitAll)
			{
				//printf("return i\n");
				return i;
			}
			else
			{
				count++;
			}
		}
		else
		{
			retcode = WAIT_TIMEOUT;
		}
	}
	if((bWaitAll)&&(nCount == count))
	{
		return count;
	}
	else
	{
		return WAIT_TIMEOUT;
	}
}
*/

long WaitForMultipleObjects(DWORD nCount, HANDLE* lpHandles, BOOL bWaitAll,DWORD dwMilliseconds)
{
	struct timeval now;
	struct timespec timeout;
	struct timezone tz;
	long retcode;

	MultipleObjects* pbitUnit = new MultipleObjects();
	NotifyMultipleObjects tmpUnit;
	tmpUnit.setMultipleObjects(pbitUnit);
	tmpUnit.setId((long)pbitUnit);
	for(int i = 0; i < nCount; i++)
	{
		tmpUnit.setIndex(i);
		if(NULL != lpHandles[i]) //add 2010-2-1 by zjj
		{
			lpHandles[i]->addNotifyUnit(tmpUnit);
		}
	}
	retcode = pbitUnit->WaitForMultipleObjects(nCount,bWaitAll,dwMilliseconds);
	if((ETIMEDOUT != retcode ) && (retcode < (WAIT_OBJECT_0 + nCount)))
	{
		if(NULL != lpHandles[retcode])  //add 2010-2-1 by zjj
		{
			lpHandles[retcode]->dealHaveNotifyMuilt();
		}
	}

	for(int i = 0; i < nCount; i++)
	{
		if(NULL != lpHandles[i])  //add 2010-2-1 by zjj
		{
			lpHandles[i]->delNotifyUnit((long)pbitUnit);
			if(nCount == retcode)
			{
				lpHandles[i]->dealHaveNotifyMuilt();
			}
		}
	}
	delete pbitUnit ;
	pbitUnit = NULL;
	return retcode;
}
