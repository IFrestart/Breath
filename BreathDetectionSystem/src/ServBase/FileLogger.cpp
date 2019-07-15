/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	FileLogger.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "FileLogger.h"
#include "ace/Signal.h"
FileLogger* 	FileLogger::pLogger_ = 0;

FileLogger::FileLogger()
		: lineNo_(0)
{
	//ACE_ASSERT(0 == pLogger_);
      //  pLogger_ = this;
}

FileLogger::~FileLogger()
{
}

FileLogger* FileLogger::getInstance()
{
	if (0 == pLogger_)
	{
		static FileLogger g_FileLogger;
		pLogger_ = &g_FileLogger;
	}
	return pLogger_;
}

int FileLogger::open(const char* log_file_name)
{
	if (!log_file_name) {
		return false;
	}
	
	if(fp_) {
		fclose(fp_);
		fp_ = 0;
	}

	fp_ = fopen(log_file_name, "a");

	return fp_ ? 0 : -1;
}

int FileLogger::log(const char *format, ...)
{
	int		ret = -1;
	if(fp_) {
		struct tm*	t;
		time_t		curtime;

		time(&curtime);
		t = localtime(&curtime);  

        sprintf(buffer_, "%04d/%02d/%02d %02d:%02d:%02d\t%s-%05d: ", t->tm_year + 1900,
            t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,fileName_.c_str(), lineNo_);

		size_t	len = strlen(buffer_);	

		va_list		va;
		va_start(va, format);
		if(len < MAXLOGLEN) {
			vsnprintf(buffer_ + len, MAXLOGLEN - len, format, va);

			len = strlen(buffer_);
			if(len < MAXLOGLEN) {
				strcat(buffer_, "\n");
			}
		}
		va_end(va);

		CMutexKeeper	mk(mutex_);
		fprintf(fp_, buffer_);
		fflush(fp_);
		ret = 0;
	}

	return ret;
}

int FileLogger::close()
{
	if(fp_) {
		fclose(fp_);
		fp_ = 0;
	}

	return 0;

}

void FileLogger::setFaultPos(const char* fn, unsigned long ln)
{
	fileName_ = fn;
	lineNo_ = ln;
}

