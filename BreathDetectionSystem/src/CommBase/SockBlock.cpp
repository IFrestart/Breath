/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandler使用的缓冲区块类

 * @file	SockBlock.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	缓冲区块类
 **/

#include <iostream>
#include <cstring>
#include "SockBlock.h"

CSockBlock::CSockBlock(unsigned long dataLen /*= 4096*/)
					: pBuffer_(new char[dataLen]),
					maxLen_(dataLen),
					wrPos_(0),
					rdPos_(0)
{
}

CSockBlock::~CSockBlock()
{
	release();
}

char* CSockBlock::wr_ptr()
{
	return pBuffer_ + wrPos_;
}

unsigned long CSockBlock::wr_ptr(long pos)
{
	unsigned long tmp = wrPos_;
	wrPos_ += pos;
	return tmp;
}

void CSockBlock::wr_ptr(char* ptr)
{
	wrPos_ = ptr - pBuffer_;
}

char* CSockBlock::rd_ptr()
{
	return pBuffer_ + rdPos_;
}

unsigned long CSockBlock::rd_ptr(long pos)
{
	unsigned long tmp = rdPos_;
	rdPos_ += pos;
	return tmp;
}


char* CSockBlock::base() const
{
	return pBuffer_;
}

void CSockBlock::release()
{
	if (pBuffer_)	//add 2011-7-7 by zjj
	{
		delete[] pBuffer_;
		pBuffer_ = 0;
	}	
}

unsigned long CSockBlock::size() const
{
	return wrPos_;
}

bool CSockBlock::copy(const char* data, unsigned long len)
{
	if(wrPos_ + len <= maxLen_) {
		memcpy(pBuffer_ + wrPos_, data, len);
		wrPos_ += len;
		return true;
	}

	return false;
}

