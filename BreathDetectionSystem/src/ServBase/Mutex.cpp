/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Mutex.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include "Mutex.h"


CMutex::CMutex()
{
}

CMutex::~CMutex()
{
}


int CMutex::lock()
{
	return mutex_.acquire();
}

int CMutex::unlock()
{
	return mutex_.release();
}

int CMutex::tryLock()
{
	return mutex_.tryacquire();
}

