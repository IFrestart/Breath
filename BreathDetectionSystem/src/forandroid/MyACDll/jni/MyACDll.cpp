// MyACDll.cpp : 定义 DLL 应用程序的入口点。
//
//#include "stdafx.h"

#ifdef _WIN32
#include <stdio.h>
#include "ACCommMainObject.h"
#pragma comment(lib, "ws2_32.lib")

//ThreadArray g_thredTable;
//long g_nThreadCount = 0;

void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);
#else
#include "MyACDll.h"
#endif


#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//引出函数
//功能：初始化WinSock
void WINAPI ACModuleInitialize()
{
	WSADATA wsdata;
	WORD wVersionRequested;
	wVersionRequested=MAKEWORD(2,2);

	WSAStartup(wVersionRequested, &wsdata);
}

//引出函数
//功能：终止WinSock
void WINAPI ACModuleUnInitialize()
{
	//::LogTrace(__FILE__, __LINE__, "stop.txt", "run thred count:%d", g_nThreadCount);
	/*
	for (DWORD i=0; i<g_thredTable.size(); i++)
	{
		//::LogTrace(__FILE__, __LINE__, "stop.txt", "threadid:%d,port:%d not exit", 
		//	g_thredTable[i].dwThreadId, g_thredTable[i].nPort);
		::WaitForSingleObject(g_thredTable[i].hThread, INFINITE);
	}

	g_thredTable.clear();
	*/
	WSACleanup();
}

//引出函数
//功能：创建通信对象
IACMainObject* WINAPI CreateACObject()
{
	return new CACMainObject();
}

//引出函数
//功能：删除通信对象
int WINAPI DestroyACObject(IACMainObject* pACMainObject)
{
	if (pACMainObject == NULL)
	{
		return -1;
	}
	
	delete (CACMainObject*)pACMainObject;
	return 0;
}
#else
//引出函数
//功能：创建通信对象
IACMainObject*  CreateACObject()
{
	return new CACMainObject();
}

//引出函数
//功能：删除通信对象
int  DestroyACObject(IACMainObject* pACMainObject)
{
	if (pACMainObject == NULL)
	{
		return -1;
	}
	
	delete (CACMainObject*)pACMainObject;
	return 0;
}
#endif
#ifdef _WIN32
void DelThreadIndex(HANDLE hThread)
#else
void DelThreadIndex(pthread_t hThread)
#endif
{
	/*
	for (DWORD i=0; i<g_thredTable.size(); i++)
	{
		if (g_thredTable[i].hThread == hThread)
		{
			g_thredTable.erase(g_thredTable.begin()+i);
		}
	}
	*/


}

