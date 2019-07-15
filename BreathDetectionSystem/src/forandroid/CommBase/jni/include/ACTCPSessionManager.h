// ACTCPSessionManager.h: interface for the CACTCPSessionManager class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACTCPSESSIONMANAGER_H__
#define __ACTCPSESSIONMANAGER_H__

#include "ACTCPSession.h"
#include "SockEventCallBack.h"

class CACTCPSessionManager  
{
public:
	CACTCPSessionManager(void);
	virtual ~CACTCPSessionManager(void);

public:	
	int AddNewSession(SOCKET hSocket);
	void RemoveSession(SOCKET hSocket);
	
	void InitSessionMgr(ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc, 
		int nMaxSessionCount);

	int StartMainLoop(void);
	void StopMainLoop(void);

	CACTCPSession * GetSessionByFD(SOCKET hSocket);
	int GetSessionCount(void);

private:
	CACTCPSession * GetActiveSession(int nTimeOut);
	int ParsePackageData(CACTCPSession * pACTCPSession, int nPkgHeadSize);
	int SessionMgrMainLoop(void);
#ifdef _WIN32
	static DWORD WINAPI RecvThreadHelper(LPVOID lpParameter);
#else
	static  void* RecvThreadHelper(LPVOID lpParameter);
	pthread_mutex_t m_mutexStopFlag;
	int m_nMaxFd;
#endif

	ISockEventCallBack* m_pDataCallBack;
	ISockHeaderCallBack* m_pHeadProc;

	fd_set m_fdset;	 

	long m_nStopFlag;
public:
#ifdef _WIN32
	HANDLE m_hRecvThread;
#else
	pthread_t m_hRecvThread;
#endif
	int m_nMaxSessionCount;

	int m_nSessionCount;
	//CRITICAL_SECTION m_lockSessCount;
	CACTCPSession* m_pSessions;
};
#endif
