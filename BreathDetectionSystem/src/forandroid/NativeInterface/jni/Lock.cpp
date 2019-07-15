
//#include "stdafx.h"
#ifndef __LINUX__
#include <windows.h>
#endif

#include "Lock.h"


CLock::CLock()
{
	InitializeCriticalSection(&m_cs);
}

CLock::~CLock()
{
	DeleteCriticalSection(&m_cs);
}

int 
CLock::lock()
{
	EnterCriticalSection(&m_cs);
	return 0;
}

int 
CLock::unlock()
{
	LeaveCriticalSection(&m_cs);
	return 0;
}

int 
CLock::trylock()
{
	//TryEnterCriticalSection(&m_cs);
	return 0;
}
