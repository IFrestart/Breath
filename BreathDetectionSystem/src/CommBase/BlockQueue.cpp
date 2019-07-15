/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandler使用的缓冲区块队列类，取代ACE_MessageQueue类

 * @file	BlockQueue.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	缓冲区块队列类
 **/

#include <iostream>
#include "BlockQueue.h"

CBlockQueue::CBlockQueue(unsigned long highWaterMark /*= 1024 * 1024*/)
					: highWaterMark_(highWaterMark),
					dataLen_(0)
{
}

CBlockQueue::~CBlockQueue()
{
	release();
}


unsigned long CBlockQueue::high_water_mark() const
{
	return highWaterMark_;
}

void CBlockQueue::high_water_mark(unsigned long highWaterMark)
{
	highWaterMark_ = highWaterMark;
}

CSockBlock* CBlockQueue::getq()
{
	//printf("Before: getq dataLen_ = %d\n", dataLen_);
	if(dataLen_) {
		if(queue_.empty()) {
			printf("dataLen_ = %d BUT queue_.empty()\n", dataLen_);
			assert(0);
		}
		CSockBlock* tmp = queue_.front();
		dataLen_ -= tmp->size();
		queue_.pop();
		//printf("After : getq dataLen_ = %d\n", dataLen_);
		return tmp;
	}

	return 0;
}

bool CBlockQueue::putq(CSockBlock* sb)
{
	unsigned long	len = sb->size();

	//printf("Before: putq dataLen_ = %d\n", dataLen_);
	if(dataLen_ + len < highWaterMark_) {
		dataLen_ += len;
		queue_.push(sb);
		//printf("After : putq dataLen_ = %d\n", dataLen_);
		return true;
	}

	return false;
}

void CBlockQueue::release()
{
	while(! queue_.empty()) {
		CSockBlock*&	tmp = queue_.front();
		if (NULL != tmp)   //add 2011-7-8
		{
			dataLen_ -= tmp->size();
			delete tmp;
		}		
		queue_.pop();
	}
}

