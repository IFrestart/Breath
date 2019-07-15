/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP数据处理类,提供了读写各种基本类型及字符串的操作

 * @file	SockStream.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	TCP/UDP数据处理类
 **/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "SockStream.h"

CSockStream::CSockStream(unsigned long dataLen /*= 4096*/)
					: pBuffer_(new char[dataLen]),
					isNew_(true),
					maxLen_(dataLen),
					dataLen_(0),
					currentPos_(0)
{
}

CSockStream::CSockStream(const void* pBuffer, unsigned long dataLen)
					: pBuffer_((char*)pBuffer),
					isNew_(false),
					maxLen_(dataLen),
					dataLen_(dataLen),
					currentPos_(0)
{
}

CSockStream::~CSockStream()
{
	if(isNew_) {
		if (pBuffer_)	//add 2011-7-8
		{
			delete[] pBuffer_;
		}		
	}
}

long CSockStream::seek(unsigned long pos) const
{
	if(pos > maxLen_) {
		std::stringstream	s;
		s << "CSockStream::seek(pos=" << pos << ")";
		throw exception(s.str());
	}

	unsigned long oldPos = currentPos_;
	currentPos_ = pos;

	return oldPos;
}

unsigned long CSockStream::tell() const
{
	return currentPos_;
}

int CSockStream::skip(unsigned long pos) const
{
	if(currentPos_ + pos > maxLen_) {
		std::stringstream	s;
		s << "CSockStream::skip(pos=" << pos << ")";
		throw exception(s.str());
	}

	currentPos_ += pos;

	return 0;
}

int CSockStream::readBytes(void* pdata, unsigned long dataLen) const
{
	if(dataLen && ! pdata) {
		std::stringstream	s;
		s << "CSockStream:: readBytes pData == 0 and dataLen = " << dataLen;
		throw exception(s.str());
	}

	if(currentPos_ + dataLen > dataLen_) {
		std::stringstream	s;
		s << "CSockStream::readBytes(dataLen=" << dataLen << ")";
		throw exception(s.str());
	}

	if(dataLen != 0) {
		memcpy(pdata, pBuffer_ + currentPos_, dataLen);
		currentPos_ += dataLen;
	}

	return 0;
}

int CSockStream::writeBytes(const void* pdata, unsigned long dataLen)
{
	if(dataLen && ! pdata) {
		std::stringstream	s;
		s << "CSockStream:: writeBytes pData == 0 and dataLen = " << dataLen;
		throw exception(s.str());
	}

	if(dataLen != 0) {
		if(currentPos_ + dataLen > maxLen_) {

			maxLen_ = maxLen_  + dataLen * 2;
			char *newBuf = new char[maxLen_];
			memcpy(newBuf,pBuffer_, currentPos_);
			if (pBuffer_)		//add 2011-7-8
			{
				delete[] pBuffer_;
			}			
			pBuffer_ = newBuf;
		}

		memcpy(pBuffer_ + currentPos_, pdata, dataLen);
		currentPos_ += dataLen;
		if(dataLen_ < currentPos_) {
			dataLen_ = currentPos_;
		}
	}

	return 0;
}

int CSockStream::readString(std::string& str) const
{
	size_t	len = strlen(pBuffer_ + currentPos_) + 1;
	if(currentPos_ + len > dataLen_) {  
		std::stringstream	s;
		s << "CSockStream::readString(string)";
		throw exception(s.str());
	}
	str = pBuffer_ + currentPos_;
	currentPos_ += len;
	return 0;
}

int CSockStream::readString(char* pStr, unsigned long dataLen) const
{
	if(dataLen && ! pStr) {
		std::stringstream	s;
		s << "CSockStream:: readString pStr == 0 and dataLen = " << dataLen;
		throw exception(s.str());
	}

	if(dataLen) {
		size_t	len = strlen(pBuffer_ + currentPos_) + 1;
		if((len > dataLen) || (currentPos_ + len > dataLen_)) {  
			std::stringstream	s;
			s << "CSockStream::readString(dataLen=" << dataLen << ")";
			throw exception(s.str());
		}

		memcpy(pStr, pBuffer_ + currentPos_, len);
		currentPos_ += len;
	}

	return 0;
}

int CSockStream::writeString(const char* pStr)
{
	if(pStr == 0) {
		std::stringstream	s;
		s << "CSockStream:: writeString pStr == 0";
		throw exception(s.str());
	}

	size_t	dataLen = strlen(pStr) + 1;
	if(currentPos_ + dataLen > maxLen_) {
		maxLen_ = maxLen_ + dataLen * 2;
		char *newBuf = new char[maxLen_];
		memcpy(newBuf,pBuffer_, currentPos_);
		if (pBuffer_)  //add 2011-7-8
		{
			delete[] pBuffer_;
		}
		pBuffer_ = newBuf;
	}

	memcpy(pBuffer_ + currentPos_, pStr, dataLen);
	currentPos_ += dataLen;

	if(dataLen_ < currentPos_) {
		dataLen_ = currentPos_;
	}

	return 0;
}

int CSockStream::writeString(const std::string& s)
{
	size_t	dataLen = strlen(s.c_str()) + 1;
	assert(dataLen <= s.length() + 1);
	if(currentPos_ + dataLen > maxLen_) {
		maxLen_ = maxLen_ + dataLen * 2;
		char *newBuf = new char[maxLen_ ];
		memcpy(newBuf,pBuffer_, currentPos_);
		if (pBuffer_)	//add 2011-7-8
		{
			delete[] pBuffer_;
		}		
		pBuffer_ = newBuf;
	}
	memcpy(pBuffer_ + currentPos_, s.c_str(), dataLen);
	currentPos_ += dataLen;

	if(dataLen_ < currentPos_) {
		dataLen_ = currentPos_;
	}

	return 0;
}

CSockStream& CSockStream::operator << (bool b)
{
	writeBytes(&b, sizeof(bool));
	return *this; 
}

CSockStream& CSockStream::operator << (unsigned char ch)
{ 
	writeBytes(&ch, sizeof(unsigned char));
	return *this; 
}

CSockStream& CSockStream::operator << (char ch)
{ 
	writeBytes(&ch, sizeof(char));
	return *this; 
}

CSockStream& CSockStream::operator << (short s)
{ 
	s = htons(s);
	writeBytes(&s, sizeof(short));
	return *this; 
}

CSockStream& CSockStream::operator << (unsigned short s)
{ 
	s = htons(s);
	writeBytes(&s, sizeof(unsigned short));
	return *this; 
}

CSockStream& CSockStream::operator << (long l)
{ 
	l = htonl(l);
	writeBytes(&l, sizeof(long));
	return *this; 
}

CSockStream& CSockStream::operator << (unsigned long l)
{ 
	l = htonl(l);
	writeBytes(&l, sizeof(unsigned long));
	return *this; 
}

CSockStream& CSockStream::operator << (long long ll)
{
	unsigned long highLL = 0;
	unsigned long lowLL = 0;
	unsigned long long tempLL = (unsigned long long)ll;
//begin modified by zjj 2010-3-16
/* 
	highLL = (unsigned long)(tempLL / 4294967296);
	lowLL = (unsigned long)(tempLL % 4294967296);
*/
	highLL = (unsigned long)(tempLL >> 32);
	tempLL = tempLL << 32;
	lowLL = (unsigned long)(tempLL >> 32);
//end modified by zjj 2010-3-16
	highLL = htonl(highLL);
	writeBytes(&highLL, sizeof(unsigned long));
	lowLL = htonl(lowLL);
	writeBytes(&lowLL, sizeof(unsigned long));
	return *this;
}

CSockStream& CSockStream::operator << (unsigned long long ll)
{
	unsigned long highLL = 0;
	unsigned long lowLL = 0;
//begin modified by zjj 2010-3-16
/*
	highLL = (unsigned long)(ll / 4294967296);
	lowLL = (unsigned long)(ll % 4294967296);
*/
	unsigned long long tempLL = (unsigned long long)ll;
	highLL = (unsigned long)(tempLL >> 32);
	tempLL = tempLL << 32;
	lowLL = (unsigned long)(tempLL >> 32);
//end modified by zjj 2010-3-16
	highLL = htonl(highLL);
	writeBytes(&highLL, sizeof(unsigned long));
	lowLL = htonl(lowLL);
	writeBytes(&lowLL, sizeof(unsigned long));
	return *this;
}

CSockStream& CSockStream::operator << (const std::string& s)
{
	size_t	dataLen = strlen(s.c_str()) + 1;
	assert(dataLen <= s.length() + 1);
	if(currentPos_ + dataLen > maxLen_) {
		maxLen_ = maxLen_ + dataLen * 2;
		char *newBuf = new char[maxLen_ ];
		memcpy(newBuf,pBuffer_, currentPos_);
		if (pBuffer_)	//add 2011-7-8
		{
			delete[] pBuffer_;
		}		
		pBuffer_ = newBuf;
	}

	memcpy(pBuffer_ + currentPos_, s.c_str(), dataLen);
	currentPos_ += dataLen;

	if(dataLen_ < currentPos_) {
		dataLen_ = currentPos_;
	}

	return *this;
}

const CSockStream& CSockStream::operator >> (bool& b) const
{ 
	readBytes(&b, sizeof(bool));
	return *this; 
}

const CSockStream& CSockStream::operator >> (unsigned char& ch) const
{ 
	readBytes(&ch, sizeof(unsigned char));
	return *this; 
}

const CSockStream& CSockStream::operator >> (char& ch) const
{ 
	readBytes(&ch, sizeof(char));
	return *this; 
}

const CSockStream& CSockStream::operator >> (short& s) const
{ 
	readBytes(&s, sizeof(short));
	s = ntohs(s);
	return *this; 
}

const CSockStream& CSockStream::operator >> (unsigned short& s) const
{ 
	readBytes(&s, sizeof(unsigned short));
	s = ntohs(s);
	return *this; 
}

const CSockStream& CSockStream::operator >> (long& l) const
{ 
	readBytes(&l, sizeof(long));
	l = ntohl(l);
	return *this; 
}

const CSockStream& CSockStream::operator >> (unsigned long& l) const
{ 
	readBytes(&l, sizeof(unsigned long));
	l = ntohl(l);
	return *this;
}

const CSockStream& CSockStream::operator >> (long long& ll) const
{
	unsigned long highLL = 0;
	unsigned long lowLL = 0;
	readBytes(&highLL, sizeof(unsigned long));
	highLL = ntohl(highLL);
	readBytes(&lowLL, sizeof(unsigned long));
	lowLL = ntohl(lowLL);
//begin modified by zjj 2010-3-16
	//ll = (long long)(highLL*4294967296 + lowLL);
	unsigned long long temphigh = highLL ;
	unsigned long long templow = lowLL ;
	temphigh = temphigh << 32;
	ll = (long long)(temphigh + templow);
//end modified by zjj 2010-3-16
        return *this;
}

const CSockStream& CSockStream::operator >> (unsigned long long& ll) const
{
	unsigned long highLL = 0;
	unsigned long lowLL = 0;
	readBytes(&highLL, sizeof(unsigned long));
	highLL = ntohl(highLL);
	readBytes(&lowLL, sizeof(unsigned long));
	lowLL = ntohl(lowLL);
//begin modified by zjj 2010-3-16
	//ll = (unsigned long long)(highLL*4294967296 + lowLL);
	unsigned long long temphigh = highLL ;
	unsigned long long templow = lowLL ;
	temphigh <<= 32;
	ll = temphigh + templow;
//end modified by zjj 2010-3-16
	return *this;
}

const CSockStream& CSockStream::operator >> (std::string& s) const
{ 
	size_t	len = strlen(pBuffer_ + currentPos_) + 1;
	if(currentPos_ + len > dataLen_) {  
		std::stringstream	s;
		s << "CSockStream::operator >> std::string";
		throw exception(s.str());
	}

	s = pBuffer_ + currentPos_;
	currentPos_ += len;

	return *this;
}

const char* CSockStream::getData() const
{
	return pBuffer_;
}

const char* CSockStream::getCurr() const
{
	return pBuffer_ + currentPos_;
}

unsigned long CSockStream::length() const
{
	return dataLen_;
}
unsigned long CSockStream::left() const
{
	return dataLen_ - currentPos_ ;
}
