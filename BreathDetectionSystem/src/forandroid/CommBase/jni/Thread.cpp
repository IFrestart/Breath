//#include "stdafx.h"
#include "Thread.h"

CThread::CThread()
#ifdef _WIN32
	: m_h(NULL)
#else
	: m_h(0)
#endif
	, m_id(0)
	, m_pCallBack(NULL)
	, m_arg(NULL)
{
}

CThread::~CThread()
{
}

void* 
CThread::threadProc(void* arg)
{
	CThread* thread = (CThread*) arg;
	return thread->m_pCallBack->threadProc(thread->m_arg);
}

int 
CThread::start(IThreadCallBack* pCallBack, void* arg)
{
	if(m_id)
		return -1;

	m_pCallBack = pCallBack;
	m_arg = arg;
#ifdef _WIN32
	m_h = ::CreateThread(NULL, 0, (DWORD (WINAPI*)(void *))CThread::threadProc, (void*)this, 0, &m_id);
	if(m_h == NULL)
		return -1;
#else
	if(0 == pthread_create(&m_h, NULL, threadProc, (void *)this))
	{
		m_id = m_h;
	}
#endif

	return 0;
}

int 
CThread::kill()
{
	if(m_id == 0)
		return -1;
#ifdef _WIN32
	::WaitForSingleObject(m_h, INFINITE);
	m_h = NULL;
#else
	pthread_join(m_h,NULL); //modified by zjj  2010-2-1
//	pthread_detach(m_h); //2012-10-16
	m_h = 0;
#endif
	m_id = 0;
	return 0;
}
