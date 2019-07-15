/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	SysLogger.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <stdio.h>
#include <stdarg.h>
#include "SysLogger.h"
#include "ace/Signal.h"

SysLogger* SysLogger::pSysLogger_ = 0;

SysLogger::SysLogger()
		: isOpen_(false), lineNo_(0)
{
	ACE_ASSERT(0 == pSysLogger_);
	pSysLogger_ = this;
}

SysLogger::~SysLogger()
{
}

SysLogger* SysLogger::getInstance()
{
	return pSysLogger_;
}

int SysLogger::open(const char* ident, int option, int facility)
{
	int		ret = -1;
	if(!isOpen_) {
		openlog(ident, option, facility);
		memset(buffer_, 0, sizeof(buffer_));
		isOpen_ = true;
		ret = 0;
	}

	return ret;
}

int SysLogger::close()
{
	int		ret = -1;
	if(isOpen_) {
		closelog();
		isOpen_ = false;
		ret = 0;
	}

	return ret;
}

int SysLogger::emerg(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_EMERG, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::alert(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_ALERT, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::crit(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_CRIT, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::error(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_ERR, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::warn(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_WARNING, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::notice(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_NOTICE, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::info(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_INFO, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::debug(const char* format, ...)
{
	int		ret = -1;
	if(isOpen_) {
		va_list	valist;
		va_start(valist, format);
		log(LOG_DEBUG, format, valist);
		va_end(valist);
		ret = 0;
	}

	return ret;
}

int SysLogger::log(int priority, const char* format, va_list va)
{
	snprintf(buffer_, MAXLOGLEN, "%s-%05d: ", fileName_.c_str(), lineNo_);
	size_t	len = strlen(buffer_);	

	if(len < MAXLOGLEN) {
		vsnprintf(buffer_ + len, MAXLOGLEN - len, format, va);

		len = strlen(buffer_);
		if(len < MAXLOGLEN) {
			strcat(buffer_, "\n");
		}
	}

	CMutexKeeper	mk(mutex_);
	syslog(priority, buffer_);

	return 0;
}

void SysLogger::setFaultPos(const char* fn, unsigned long ln)
{
	fileName_ = fn;
	lineNo_ = ln;
}

