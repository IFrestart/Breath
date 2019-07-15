// ACUDPMainProcClass.cpp: implementation of the CACUDPMainProcClass class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include <stdio.h>

#include "ACUDPMainProcClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

#include "LogTrace.h"
//void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);

CACUDPMainProc::CACUDPMainProc(void)
{
	m_pACUDPSession=NULL;
}

CACUDPMainProc::~CACUDPMainProc(void)
{
	StopMainProc();
}

void CACUDPMainProc::Initialize(short nCommType, short nThreadNum, short nMaxConn, 
	ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc)
{	
	CACBaseMainProc::Initialize(nCommType, nThreadNum, 1, lpDataCallBack, lpHeadProc);
}

int CACUDPMainProc::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort,
	LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint)
{
	char szLocalHost[16], szLocalMask[16];
	
	memset(szLocalHost, 0, sizeof(szLocalHost));
	memset(szLocalMask, 0, sizeof(szLocalMask));

	m_pACUDPSession=new CACUDPSession();

	if(bMulPoint == TRUE)
	{
		if(m_pACUDPSession->InitializeSession(m_pDataCallBack,m_pHeadProc,lpLocalHost,
			nLocalPort,lpRemoteHost,nRemotePort) != 0)
		{
			return -1;
		}

	}
	else
	{
		if (m_pACUDPSession->InitializeSession(m_pDataCallBack, m_pHeadProc, 
			lpLocalHost, nLocalPort) != 0)
		{
			return -1;
		}
	}

	
	DWORD dwThreadID = m_pACUDPSession->StartSessionMainLoop();
	
		ThreadInfo thread;
		thread.dwThreadId = dwThreadID;
		thread.hThread = m_pACUDPSession->m_hRecvThread;
		thread.nPort = nLocalPort;
		//g_thredTable.push_back(thread);

	SOCKET hSocket = m_pACUDPSession->GetConnect()->GetSocket();
	CACSocket::GetSockName(hSocket, szLocalHost, &nLocalPort, szLocalMask);
	//m_pDataCallBack->OnNewConnection(hSocket, szLocalHost, nLocalPort, 
		//szLocalMask, lpRemoteHost, nRemotePort);
	m_pDataCallBack->OnNewConnection(hSocket);

	//::LogTrace(__FILE__, __LINE__, "thread.txt", "thread:%d, port:%d",
	//	dwThreadID, nLocalPort);

	return 0;
}

int CACUDPMainProc::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpRemoteHost, USHORT nRemotePort, LPCTSTR lpProxyHost,
	USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword)
{
	char szLocalHost[16], szLocalMask[16];
	
	memset(szLocalHost, 0, sizeof(szLocalHost));
	memset(szLocalMask, 0, sizeof(szLocalMask));

	m_pACUDPSession=new CACUDPSession();

	if (m_pACUDPSession->InitializeSession(m_pDataCallBack, m_pHeadProc, 
		lpLocalHost, nLocalPort, lpProxyHost, nProxyPort, lpProxyUserName,
		lpProxyPassword) != 0)
	{
		return -1;
	}
	
	m_pACUDPSession->StartSessionMainLoop();
	
	SOCKET hSocket = m_pACUDPSession->GetConnect()->GetSocket();
	CACSocket::GetSockName(hSocket, szLocalHost, &nLocalPort, szLocalMask);
	//m_pDataCallBack->OnNewConnection(hSocket, szLocalHost, nLocalPort, 
		//szLocalMask, lpRemoteHost, nRemotePort);
	m_pDataCallBack->OnNewConnection(hSocket);

	return 0;
}

int CACUDPMainProc::SendCommUDPData(WORD wPduType, const BYTE* lpBuf, 
	DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort,
	short nPackID, BOOL bSock5Flag)
{
	if (m_pACUDPSession)
	{
		return m_pACUDPSession->SendCommData(wPduType, lpBuf, dwSize, 
			bSync, lpRemoteHost, nRemotePort, nPackID, bSock5Flag);
	}

	return -1;
}

int CACUDPMainProc::StopMainProc(void)
{
	if (m_pACUDPSession != NULL)
	{
		m_pACUDPSession->StopSessionMainLoop();
		delete m_pACUDPSession;
		m_pACUDPSession = NULL;
	}

	return 0;
}

int CACUDPMainProc::CloseConnection(SOCKET hSocket)
{	
	m_pACUDPSession->CloseConnect();
	return 0;
}

