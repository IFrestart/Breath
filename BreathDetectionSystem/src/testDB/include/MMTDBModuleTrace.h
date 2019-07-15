 /**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     MMTDBModuleTrace.h
  * @author   white <white@webcon.com.cn>
  * @short
  * @date     03/2004
  **/

#ifndef __MMTDB_MODULETRACE_H__
#define __MMTDB_MODULETRACE_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/timeb.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>
#include <iomanip>


#define MMTM_MAXLEN			2048
#define MMTM_FMT_MAXLEN		512

#define MMTM_T_MAXLEN		20
#define MMTM_M_MAXLEN		16

/**
 * Hide
 */
#define MMTDB_TRACE_HIDE		0x0

/**
 * Show
 *状态信息
 */
#define MMTDB_TRACE_ERROR		0x1		/**< 错误信息 */
#define MMTDB_TRACE_TIMER		0x2		/**< 定时器 */
#define MMTDB_TRACE_STATUS		0x4		/**< 状态统计信息 */
#define MMTDB_TRACE_SQL			0x8		/**< SQL语句 */

/**
 * 过程信息
 */
#define MMTDB_TRACE_VARIN		0x10	/**< 16	传入参数 */
#define MMTDB_TRACE_VAROUT		0x20	/**< 32	传出参数 */
#define MMTDB_TRACE_VAR			0x40	/**< 64	过程参数 */
#define MMTDB_TRACE_FUNC		0x80	/**< 128函数开始与结束 */

/**
 * 过程关键信息
 */
#define MMTDB_TRACE_HEART1		0x100	/**< 256	过程信息，很少出现 */
#define MMTDB_TRACE_HEART2		0x200	/**< 512	过程信息，出现有限次 */
#define MMTDB_TRACE_HEART3		0x400	/**< 1024	过程信息，不定时频繁出现 */
#define MMTDB_TRACE_HEART4		0x800	/**< 2048	过程信息，最频繁出现 */

#define MMTM_T_ERROR		" @-@    [error] -- "
#define MMTM_T_TIMER		" @--@   [timer] -- "
#define MMTM_T_STATUS		" @---@  [stat]  -- "
#define MMTM_T_SQL			" @----@ [sql]   -- "
#define MMTM_T_VARIN		" @=@    [var in] -- "
#define MMTM_T_VAROUT		" @==@   [var out] -- "
#define MMTM_T_VAR			" @===@  [var]   -- "
#define MMTM_T_FUNC			" @====@ [func]  -- "
#define MMTM_T_H1			" @+@    [h1]    -- "
#define MMTM_T_H2			" @++@   [h2]    -- "
#define MMTM_T_H3			" @+++@  [h3]    -- "
#define MMTM_T_H4			" @++++@ [h4]    -- "


#define MMTDB_MODULE_DBCONFIGCLASS		0x1
#define MMTDB_MODULE_DBCOMM				0x2
#define MMTDB_MODULE_MYDB				0x4
#define MMTDB_MODULE_EDUDBMGR			0x8
#define MMTDB_MODULE_EDUDBCOMM			0x10	//16
#define MMTDB_MODULE_TEST				0x1000	//4096


#define MMTM_M_CFG			" [DBCfg]   -- "
#define MMTM_M_DBCOM		" [DBComm]  -- "
#define MMTM_M_MYDB			" [CMyDB]   -- "
#define MMTM_M_EDUMGR		" [EDUMgr]  -- "
#define MMTM_M_EDUCOM		" [EDUComm] -- "
#define MMTM_M_TEST			" [TestEDU] -- "

class CMMTDBTrace
{
public:
	CMMTDBTrace();
	CMMTDBTrace(int nSelfMod);
	~CMMTDBTrace();
	
public:
	void TraceError(char *fmtstr, ... );
	void TraceTimer(char *fmtstr, ... );
	void TraceStatus(char *fmtstr, ... );
	void TraceSQL(char *fmtstr, ... );
	void TraceVarIn(char *fmtstr, ... );
	void TraceVarOut(char *fmtstr, ... );
	void TraceVar(char *fmtstr, ... );
	void TraceFunc(char *fmtstr, ... );
	void TraceH1(char *fmtstr, ... );
	void TraceH2(char *fmtstr, ... );
	void TraceH3(char *fmtstr, ... );
	void TraceH4(char *fmtstr, ... );
	
	int GetTraceStatus();
	int GetModStatus();
	void SetModAndTrace(int nMod, int nTrace);
	
	int GetSelfMod(char* szSelfMod);
	void SetSelfMod(int nSelfMod);
	
private:
	int	OpenTraceFile();
    //当前时刻
	void TraceDateTime();

private:
	int		m_nTrace;
	int		m_nMod;	
	int		m_nSelfMod;
	char	m_szSelfMod[MMTM_M_MAXLEN];
	FILE*	m_fp;
};


#endif	//__MMTDB_MODULETRACE_H__


