
#ifdef __LINUX__
#include  "WaitForObject.h"
#else
#include "stdafx.h"
#endif


#include "Event.h"

Event::Event(BOOL bManulReset, BOOL bInitialState)
: m_e(NULL)
{
	m_e = ::CreateEvent(NULL, bManulReset, bInitialState, NULL);
}

Event::~Event()
{
	::CloseHandle(m_e);
}

int Event::waitEvent(unsigned long lockTimer)
{
	DWORD dwRet = ::WaitForSingleObject(m_e, lockTimer);
	if(dwRet != WAIT_OBJECT_0)
	{
		if( dwRet == WAIT_TIMEOUT)
			return -2;
		else
			return -1;			
	}
	return 0;
}

int Event::setEvent()
{
	::SetEvent(m_e);
	return 0;
}

int Event::resetEvent()
{
	::ResetEvent(m_e);
	return 0;
}
