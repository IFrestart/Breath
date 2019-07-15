
//#include "stdafx.h"
#include "ThreadCallBack.h"
#include <unistd.h>

IThreadCallBack::IThreadCallBack()
{
}

IThreadCallBack::~IThreadCallBack()
{
}

myThreadCallBack::myThreadCallBack()
{
	pthread_mutex_init(&m_mutext_,NULL);
	m_count = 0;
}

myThreadCallBack::~myThreadCallBack()
{
	pthread_mutex_destroy(&m_mutext_);
}

unsigned long myThreadCallBack::getCount()
{
	unsigned long tt;
	pthread_mutex_lock(&m_mutext_);
	tt = m_count++;
	pthread_mutex_unlock(&m_mutext_);
	return tt;
}

void* myThreadCallBack::threadProc(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&m_mutext_);
		m_count++;
		pthread_mutex_unlock(&m_mutext_);
		sleep(3);
	}
	return NULL;
}


