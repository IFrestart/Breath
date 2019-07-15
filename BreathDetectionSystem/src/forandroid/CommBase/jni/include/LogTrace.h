//#pragma once
#ifndef __LOGTRACE_H__
#define __LOGTRACE_H__

#include "LinuxDef.h"

#define TRACE            

#ifdef TRACE
void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);

#define LOG_TRACE		LogTrace
#endif	//TRACE

#if _MSC_VER > 1200
#define NOOP			__noop
#else
#define NOOP			NULL
#endif // _MSC_VER > 1200

#ifndef LOG_TRACE
#define LOG_TRACE		NOOP
#endif

#endif
