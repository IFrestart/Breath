/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file     MMTDBModuleTrace.cpp
 * @author   white <white@webcon.com.cn>
 * @short
 * @date     03/2004
 **/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "MMTDBModuleTrace.h"



CMMTDBTrace::CMMTDBTrace()
{
	m_fp = NULL;
	m_nTrace = MMTDB_TRACE_ERROR;
	m_nMod = 0x1F;
	m_nSelfMod = 0;
	memset(m_szSelfMod, 0, MMTM_M_MAXLEN);
}

CMMTDBTrace::CMMTDBTrace(int nSelfMod)
{
	m_fp = NULL;
	m_nTrace = 0;
	m_nMod = 0;
	m_nSelfMod = 0;
	memset(m_szSelfMod, 0, MMTM_M_MAXLEN);

	SetSelfMod(nSelfMod);
}

CMMTDBTrace::~CMMTDBTrace()
{
	if (m_fp)
		fclose(m_fp);
	m_fp = NULL;
}

void CMMTDBTrace::TraceError(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_ERROR) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_ERROR);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceTimer(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_TIMER) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_TIMER);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceStatus(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_STATUS) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_STATUS);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceSQL(char *fmtstr, ... )
{
	if (((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE) || (m_nTrace & MMTDB_TRACE_SQL) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_SQL);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceVarIn(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_VARIN) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_VARIN);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceVarOut(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_VAROUT) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_VAROUT);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceVar(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_VAR) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_VAR);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceFunc(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_FUNC) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_FUNC);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	fflush(m_fp);
}

void CMMTDBTrace::TraceH1(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_HEART1) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_H1);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceH2(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_HEART2) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_H2);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceH3(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_HEART3) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_H3);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}

void CMMTDBTrace::TraceH4(char *fmtstr, ... )
{
	if ((m_nMod & m_nSelfMod) == MMTDB_TRACE_HIDE || (m_nTrace & MMTDB_TRACE_HEART4) == MMTDB_TRACE_HIDE)
		return;

	if (OpenTraceFile() != 0)
		return;

	TraceDateTime();
	fprintf(m_fp, "%s%s", m_szSelfMod, MMTM_T_H4);

	va_list argp;
	va_start(argp, fmtstr);
	vfprintf(m_fp, fmtstr, argp);
	va_end(argp);
	//fprintf(m_fp, "\n");
	fflush(m_fp);
}


int CMMTDBTrace::GetTraceStatus()
{
	return m_nTrace;
}

int CMMTDBTrace::GetModStatus()
{
	return m_nMod;
}

void CMMTDBTrace::SetModAndTrace(int nMod, int nTrace)
{
	m_nMod = nMod;
	m_nTrace = nTrace;

	TraceTimer("Mod=[%d], Trace=[%d]\n", nMod, nTrace);
}

int CMMTDBTrace::GetSelfMod(char* szSelfMod)
{
	if (szSelfMod)
		strcpy(m_szSelfMod,szSelfMod);

	return m_nSelfMod;
}

void CMMTDBTrace::SetSelfMod(int nSelfMod)
{
	switch(nSelfMod)
	{
	case MMTDB_MODULE_DBCONFIGCLASS:
	{
		strcpy(m_szSelfMod, MMTM_M_CFG);
		break;
	}
	case MMTDB_MODULE_DBCOMM:
	{
		strcpy(m_szSelfMod, MMTM_M_DBCOM);
		break;
	}
	case MMTDB_MODULE_MYDB:
	{
		strcpy(m_szSelfMod, MMTM_M_MYDB);
		break;
	}
	case MMTDB_MODULE_EDUDBMGR:
	{
		strcpy(m_szSelfMod, MMTM_M_EDUMGR);
		break;
	}
	case MMTDB_MODULE_EDUDBCOMM:
	{
		strcpy(m_szSelfMod, MMTM_M_EDUCOM);
		break;
	}
	case MMTDB_MODULE_TEST:
	{
		strcpy(m_szSelfMod, MMTM_M_TEST);
		break;
	}
	default:
	{
		m_nSelfMod = 0;
		memset(m_szSelfMod, 0, MMTM_M_MAXLEN);
		return;
	}
	}

	m_nSelfMod = nSelfMod;
}

//////////////////////////////////////////////////////////////////////////////////////
//private
int	CMMTDBTrace::OpenTraceFile()
{
	if (m_fp)
		return 0;

	struct stat logfile_stat;

	if(stat("ClusterDBCommServ.log", &logfile_stat) == -1)
	{
		m_fp = fopen("ClusterDBCommServ.log", "w");
	}
	else
	{
		m_fp = fopen("ClusterDBCommServ.log", "a");
	}

	if(m_fp == NULL)
		return -1;
	return 0;
}


//µ±Ç°Ê±¿Ì year-mon-day hour:min:sec msec
void CMMTDBTrace::TraceDateTime()
{
    struct tm *t;
	timeb curtime;
	ftime(&curtime);
	t = localtime(&(curtime.time));
    fprintf(m_fp, "[%04d-%02d-%02d %02d:%02d:%02d %03d]\t",t->tm_year+1900,
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, curtime.millitm);
}

