
#ifndef __THREADCALLBACK_H__
#define __THREADCALLBACK_H__
#include <pthread.h>
class IThreadCallBack
{
public:
	IThreadCallBack();
	virtual ~IThreadCallBack();
public:
	virtual void* threadProc(void* arg) = 0;
};


class myThreadCallBack : public IThreadCallBack
{
public:
        myThreadCallBack();
        ~myThreadCallBack();
public:
        //from IThreadCallBack
        void* threadProc(void* arg);
	unsigned long getCount();
private:
	unsigned long m_count;
	pthread_mutex_t         m_mutext_;
};


#endif //__THREADCALLBACK_H__
