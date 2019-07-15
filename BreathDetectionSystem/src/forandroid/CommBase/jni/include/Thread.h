
#ifndef __THREAD_H__
#define __THREAD_H__

#include "ThreadCallBack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include    <sys/un.h>
#include    <unistd.h>
#include <pthread.h>
#include <time.h>

#include <sys/ipc.h>
//#include <sys/msg.h> //modify 2012-8-24  zzzz
#include <sys/time.h>

#include <bitset>
#include <list>


class CThread
{
public:
	CThread();
	~CThread();
public:
	static void* threadProc(void* arg);
	int start(IThreadCallBack* pCallBack, void* arg);
	int kill();
	bool testCancel()const {return 0 == m_id;} //add 2012-10-16
public:
	IThreadCallBack* m_pCallBack;
	void* m_arg;
	pthread_t m_h;
	unsigned long m_id;
};

#endif //__THREAD_H__
