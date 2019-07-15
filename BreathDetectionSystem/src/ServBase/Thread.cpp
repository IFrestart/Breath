/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Thread.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>
#include <utility>

#include "Thread.h"


CThread::CThread()
			: threadID_(0), pCallBack_(0), arg_(0)
{
}

CThread::~CThread()
{
	kill();
}


void* CThread::threadProc(void* arg)
{
	CThread*	pThread = (CThread*)arg;

	pThread->pCallBack_->threadProc(pThread->arg_);
	pThread->threadID_ = 0;

	return 0;
}

bool CThread::testCancel()
{
	return ACE_Thread_Manager::instance()->testcancel(ACE_Thread::self()) != 0;
}

int CThread::start(IThreadCallBack* pCallBack, void* arg)
{
	if(0 != threadID_) {
		return -1;
	}

	pCallBack_ = pCallBack;
	arg_ = arg;
	int		ret = ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)CThread::threadProc,
														this,
														THR_NEW_LWP,// | THR_DETACHED,
														&threadID_);
	//std::cout << "CThread::start() = " << ret << std::endl;

	return -1 == ret ? -1 : 0;
}

int CThread::kill()
{
	if(0 == threadID_) {
		return -1;
	}

	int	ret = -1;
	if(0 == ACE_Thread_Manager::instance()->cancel(threadID_)
		&& 0 == ACE_Thread_Manager::instance()->join(threadID_)) {
		ret = 0;
	}

	//std::cout << "CThread::kill() = " << ret << std::endl;

	return ret;
}

