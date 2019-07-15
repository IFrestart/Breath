// ACTCPMainProcClass.h: interface for the CACTCPMainProcClass class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACTCPMAINPROCCLASS_H__
#define __ACTCPMAINPROCCLASS_H__

#include "ACSocket.h"
#include "ACBaseMainProcClass.h"
#include "ACTCPSessionManager.h"

class CACTCPMainProc: public CACBaseMainProc 
{
public:
	CACTCPMainProc(void);
	virtual ~CACTCPMainProc(void);

	//Inherit from CACBaseMainProcClass
	void Initialize(short nCommType, short nThreadNum, short nMaxConn,
		IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort,
		LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint);
	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,
		LPCTSTR pProxyHost, USHORT nProxyPort,
		LPCTSTR pProxyUserName, LPCTSTR pProxyPassword){return -1;};

	int BeginWorkHttp(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,
		LPCTSTR pProxyHost, USHORT nProxyPort,
		LPCTSTR pProxyUserName, LPCTSTR pProxyPassword){return -1;};

	int SendCommTCPData(WORD wPduType, const BYTE* lpBuf, DWORD dwSize, 
		BOOL bSync, SOCKET hSocket=INVALID_SOCKET);

private:
	int GetBestSessionMgrIndex(void);
	int TCPServerListenProc(void);
	int CloseConnection(SOCKET hSocket = INVALID_SOCKET);
	int StopMainProc(void);

#ifdef _WIN32
	static DWORD WINAPI ListenThreadHelper(LPVOID lpParameter);
	HANDLE m_hListenThread;
#else
	static void* ListenThreadHelper(LPVOID lpParameter);
	pthread_mutex_t m_mutexStopFlag;
	pthread_t m_hListenThread;
#endif

	CACTCPSessionManager* m_pSessionMgr; //会话数组

	long m_nStopFlag;
	CACSocket * m_pSocketMainProc;
};
#endif
