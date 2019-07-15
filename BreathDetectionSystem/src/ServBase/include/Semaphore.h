
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "Common.h"
#include <ace/Synch.h>
class CSemaphore
{
public:
	CSemaphore(int count = 0);
	~CSemaphore();
	int lock();
	int unlock();
private:
	ACE_Thread_Semaphore m_Semaphore;
};

#endif //__SEMAPHORE_H__
