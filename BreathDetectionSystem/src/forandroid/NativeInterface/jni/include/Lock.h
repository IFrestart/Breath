#ifndef __LOCK_H__
#define __LOCK_H__

#ifdef __LINUX__
#include "LinuxDef.h"
#endif

class CLock
{
public:
	CLock();
	~CLock();
public:
	int lock();
	int unlock();
	int trylock();
private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
	CAutoLock(CLock& mutex) : mutex_(mutex)
	{
		mutex_.lock();
	}

	~CAutoLock()
	{
		mutex_.unlock();
	}

private:
	CLock&		mutex_;
};

#endif //__LOCK_H__
