/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Timer.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "ace/Reactor.h"
#include "ace/Thread_Manager.h"

#include "Thread.h"
#include "Timer.h"
/*
MyTimer        g_myTimer;
MyTimer&       timerInstance()
{
        return g_myTimer;
}
*/

CTimer::CTimer()
		: timerID_(-1), userID_(0),
		pCallBack_(0), arg_(0)
{
}

CTimer::~CTimer()
{
	kill();
}

int CTimer::handle_timeout(const ACE_Time_Value& now, const void* act)
{
	if(pCallBack_->OnTimer(userID_, (void*)arg_)) {
		//ACE_Reactor::instance()->cancel_timer(this);
		timerID_ = -1;

		return -1;
	}

	return 0;
}

int CTimer::start(unsigned long timerID, unsigned long microseconds,
					ITimerEventCallBack* pCallBack, void* arg)
{
	struct timeval	timeval = { microseconds / 1000, (microseconds % 1000) * 1000 };

	return start(timerID, timeval, pCallBack, arg);
}

int CTimer::start(unsigned long timerID, struct timeval timeval,
					ITimerEventCallBack* pCallBack, void* arg)
{
	if(-1 != timerID_) {
		return -1;
	}

	userID_ = timerID;
	timeval_ = timeval;
	pCallBack_ = pCallBack;
	arg_ = arg;

	ACE_Time_Value	tv(timeval);

	timerID_ = ACE_Reactor::instance()->schedule_timer(this, arg, tv, tv);
	//timerID_ = timerInstance().schedule(this, arg, tv, tv);
//	timerID_ = timerInstance().schedule(this, NULL, tv, tv);

	return -1 == timerID_ ? -1 : 0;
}

int CTimer::kill()
{
	if(-1 == timerID_) {
		return -1;
	}

	int	ret = ACE_Reactor::instance()->cancel_timer(timerID_);
	//int	ret = timerInstance().cancel(timerID_);
	if(1 == ret) {
		timerID_ = -1;
	}

	return 1 == ret ? 0 : -1;
}

