
#include "LogTrace.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <assert.h>
#ifdef _WIN32
#include <TCHAR.h>
#include <time.h>
#else
#include <sys/time.h>
#include <stdarg.h>
#endif

void GetAppPath(char* lpAppPath, DWORD dwPathSize)
{
	char szModuleName[MAX_PATH];
	char* pTmp = NULL;

	memset(szModuleName, 0, sizeof(szModuleName));
#ifdef _WIN32
	::GetModuleFileName(NULL, szModuleName, MAX_PATH);
	pTmp = strrchr(szModuleName, '\\');
	if (pTmp != NULL)
	{
		*pTmp = '\0';
	}
	else
	{
		pTmp = strrchr(szModuleName, '/');
		if (pTmp != NULL)
		{
			*pTmp = '\0';
		}
	}
#else
	pTmp = getcwd(szModuleName,MAX_PATH);
#endif
	memset(lpAppPath, 0, dwPathSize);
	strncpy(lpAppPath, szModuleName, dwPathSize-1);
}

void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...)
{
	LPCTSTR lpSrcFile = NULL;
	char szLogFile[MAX_PATH];

	lpSrcFile = strrchr(lpFile, '\\');
	if (lpSrcFile == NULL)
	{
		lpSrcFile = strrchr(lpFile, '/');
		if (lpSrcFile == NULL)
		{
			lpSrcFile = lpFile;
		}
		else
		{
			lpSrcFile++;
		}
	}
	else
	{
		lpSrcFile++;
	}

	//::EnterCriticalSection(&g_hCritical);
	if (lpLogFile == NULL)
	{
		GetAppPath(szLogFile, sizeof(szLogFile));
		strcat(szLogFile, "\\CommLog.tx");
	}
	else
	{
		char* pTmp = NULL;
		
		pTmp = strrchr(lpLogFile, '\\');
		if (pTmp == NULL)
		{
			pTmp = strrchr(lpLogFile, '/');
			if (pTmp == NULL) //不包含路径
			{
				GetAppPath(szLogFile, sizeof(szLogFile));
#ifdef _WIN32
				strcat(szLogFile, "\\debuginfo\\");
#else
				strcat(szLogFile, "/debuginfo/");
#endif
				strcat(szLogFile, lpLogFile);
			}
			else //包含路径
			{
				strncpy(szLogFile, lpLogFile, sizeof(szLogFile));
			}
		}
		else //包含路径
		{
			strncpy(szLogFile, lpLogFile, sizeof(szLogFile));
		}
	}

	FILE* pLogFile = fopen(szLogFile, "a");

#ifdef _WIN32
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	fprintf(pLogFile, "time:%02d:%02d:%02d.%03d, file:%s,line:%d,msg:", curTime.wHour,
		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds, lpSrcFile, nLine); 
#else
	if(NULL == pLogFile)
	{
		return ;
	}

	time_t 		t ;
	struct tm 	*tm_today ;
	struct timeval tv ;

	gettimeofday( &tv, NULL ) ;

	t = tv.tv_sec ;
	tm_today = localtime( &t ) ;

	fprintf(pLogFile, "time:%02d:%02d:%02d.%03d, file:%s,line:%d,msg:", 
						tm_today->tm_hour,
						tm_today->tm_min,
						tm_today->tm_sec,
						tv.tv_usec, lpSrcFile, nLine); 
#endif

	va_list vaArgs;
	va_start(vaArgs, lpFormat);
	vfprintf(pLogFile, lpFormat, vaArgs); 
	va_end(vaArgs);

#ifdef _WIN32
	fprintf(pLogFile, "(%d)\n", GetCurrentThreadId()); 
#else
	fprintf(pLogFile, "(%d)\n", pthread_self()); 
#endif
	fflush(pLogFile);
	fclose(pLogFile);
	//::LeaveCriticalSection(&g_hCritical);
}
