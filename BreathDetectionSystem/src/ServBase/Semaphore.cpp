
#include "Semaphore.h"

CSemaphore::CSemaphore(int count)
	: m_Semaphore(count)
{
}

CSemaphore::~CSemaphore()
{
}

int
CSemaphore::lock()
{
	return m_Semaphore.acquire();
}

int
CSemaphore::unlock()
{
	return m_Semaphore.release();
}
