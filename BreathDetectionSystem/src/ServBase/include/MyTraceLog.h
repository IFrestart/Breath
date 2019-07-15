 /**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     MyTraceLog.h
  * @author   zhoujj <zhoujj@webcon.com.cn>
  * @short
  **/

#ifndef __MY_TRACE_LOG_H__
#define __MY_TRACE_LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef __LINUX__

#include <sys/time.h>
#include <time.h>
#include <sys/timeb.h>
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>
#include <iomanip>


#define TRACE_ERROR_MODE	1
#define TRACE_ALL_MODE	2

class MyTraceLog
{
public:
	MyTraceLog();
	~MyTraceLog();
	
public:
	void setTraceMode(int traceMode);
	int initialize(const char *filename);

	void TraceError(char *fmtstr, ... );
	void TraceVarIn(char *fmtstr, ... );
	void TraceVarOut(char *fmtstr, ... );
	void TraceVarOut2(char *fmtstr, ... );
	void TraceDumpHex(const void* data, unsigned long len, unsigned short numperline = 32);
	
private:
	int	OpenTraceFile(const char *filename);
	int TraceDateTime();

private:
	int m_traceMode;
	FILE*	m_fp;
};


#endif	//__MY_TRACE_LOG_H__

