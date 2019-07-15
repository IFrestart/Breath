// MyACDll.cpp : 定义 DLL 应用程序的入口点。
//
//#include "stdafx.h"

#include <stdio.h>
#include "ACCommMainObject.h"

#ifdef _WIN32
void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);
void DelThreadIndex(HANDLE hThread);
#else
void DelThreadIndex(pthread_t hThread);
#endif
void GetAppPath(char* lpAppPath, DWORD dwPathSize);
IACMainObject*  CreateACObject();
int  DestroyACObject(IACMainObject* pACMainObject);

