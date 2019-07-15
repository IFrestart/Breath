 /**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     MyTraceLog.cpp
  * @author   zhoujj <zhoujj@webcon.com.cn>
  * @short
  **/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "MyTraceLog.h"


MyTraceLog::MyTraceLog()
{
	m_fp = NULL;
	m_traceMode = TRACE_ALL_MODE;
}

MyTraceLog::~MyTraceLog()
{
	if (m_fp)
		fclose(m_fp);
	m_fp = NULL;
}

void MyTraceLog::setTraceMode(int traceMode)
{
	m_traceMode = traceMode;
}

int	MyTraceLog::initialize(const char *filename)
{
	if (OpenTraceFile(filename) != 0)
		return -1 ;
	return 0;
}

void MyTraceLog::TraceError(char *fmtstr, ... )
{
	if(((m_traceMode != TRACE_ALL_MODE) && (m_traceMode != TRACE_ERROR_MODE )) || (TraceDateTime() != 0))
		return;
	fprintf(m_fp, "----Error---- ");
	
	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	fflush(m_fp);
}

void MyTraceLog::TraceVarIn(char *fmtstr, ... )
{
	if((m_traceMode != TRACE_ALL_MODE) || (TraceDateTime() != 0))
		return;
	fprintf(m_fp, "----VarIn---- ");
	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	fflush(m_fp);
}

void MyTraceLog::TraceVarOut(char *fmtstr, ... )
{
	if((m_traceMode != TRACE_ALL_MODE) || (TraceDateTime() != 0))
		return;
	fprintf(m_fp, "----VarOut---- ");
	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	fflush(m_fp);
}

void MyTraceLog::TraceVarOut2(char *fmtstr, ... )
{
	if (m_fp == NULL) 
		return ;
	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	fflush(m_fp);
}
	
int	MyTraceLog::OpenTraceFile(const char *filename)
{
	if (m_fp)
		return 0;
	
	struct stat logfile_stat;
	
	if(stat(filename, &logfile_stat) == -1)
	{
		m_fp = fopen(filename, "w");
	}
	else
	{
		m_fp = fopen(filename, "a");
	}
	
	if(m_fp == NULL)
	        return -1;
	return 0;
}

int MyTraceLog::TraceDateTime()
{
	struct tm * t;
	
	if (m_fp == NULL) 
		return -1;
	timeb curtime;
	
	ftime(&curtime);
	
	t = localtime(&(curtime.time));
	
	fprintf(m_fp, "[%04d-%02d-%02d %02d:%02d:%02d %03d] ", t->tm_year+1900,
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, curtime.millitm);
	return 0;
}

void MyTraceLog::TraceDumpHex(const void* data, unsigned long len, unsigned short numperline)
{
	int line = len / numperline ;
	unsigned char *p = (unsigned char *)data;
	unsigned char buf[1024] = {0};
	long i = 0, j = 0;

	if((m_traceMode != TRACE_ALL_MODE) || (TraceDateTime() != 0))
		return;
	if(len % numperline)
		line++;
	fprintf(m_fp,"\n");
	fprintf(m_fp,"-------------------------------------------------------------------------\n");
	for(i = 0; i < line; i++)
	{
		fprintf(m_fp,"%02d: ",i);
		for(j = 0 ; (j < numperline) && (j < len - i * numperline) ; j++)
		{
			if(j == numperline / 2 )
				fprintf(m_fp,"- ",i);
			fprintf(m_fp,"%02x ",(*p++) & 0xff );
		}
		fprintf(m_fp,"\n");
	}
	fprintf(m_fp,"-------------------------------------------------------------------------\n");
	fflush(m_fp);
}
