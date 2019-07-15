/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Mutex.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include "Mutex.h"


CMutex::CMutex()
{
	pthread_mutex_init(&mutex_,NULL);
}

CMutex::~CMutex()
{
	pthread_mutex_destroy(&mutex_);
}


int CMutex::lock()
{
	//return mutex_.acquire();
	return pthread_mutex_lock(&mutex_);
}

int CMutex::unlock()
{
	//return mutex_.release();
	return pthread_mutex_unlock(&mutex_);
}

int CMutex::tryLock()
{
	//return mutex_.tryacquire();
	return 0;
}

