// ACTCPMainProcClass.cpp: implementation of the CACTCPMainProcClass class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <stdio.h>
#include <assert.h>

#include "ACTCPMainProcClass.h"
#include "ACCommonResource.h"
#include "LogTrace.h"

//void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);
#ifdef _WIN32
void DelThreadIndex(HANDLE hThread);
#else
void DelThreadIndex(pthread_t hThread);
#endif

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
DWORD WINAPI CACTCPMainProc::ListenThreadHelper(LPVOID lpParameter)
#else
void* CACTCPMainProc::ListenThreadHelper(LPVOID lpParameter)
#endif
{
	CACTCPMainProc * pThis = (CACTCPMainProc*)lpParameter;

	pThis->TCPServerListenProc();

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CACTCPMainProc::CACTCPMainProc(void)
{
	m_nStopFlag = 0;
	m_pSocketMainProc = NULL;
	m_pSessionMgr = NULL;

#ifdef _WIN32
	m_hListenThread = NULL;
#else
	m_hListenThread = 0;
	pthread_mutex_init(&m_mutexStopFlag,NULL);
#endif
}

CACTCPMainProc::~CACTCPMainProc(void)
{
	if (m_pSocketMainProc != NULL)
	{
		StopMainProc();
	}
#ifndef _WIN32
	pthread_mutex_destroy(&m_mutexStopFlag);
#endif
}

void CACTCPMainProc::Initialize(short nCommType, short nThreadNum, short nMaxConn, 
	IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc)
{
	CACBaseMainProc::Initialize(nCommType, nThreadNum, nMaxConn, lpDataCallBack, 
		lpHeadProc);
	
	if (m_nCommType == COMMTYPE_CLIENT)
	{
		m_nThreadNum = 1;
		m_nMaxConn = 1;
	}

	m_pSessionMgr = new CACTCPSessionManager[nThreadNum];
}

int CACTCPMainProc::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint)
{
	m_pSocketMainProc = new CACSocket();	
	if (m_pSocketMainProc->Create(SOCK_STREAM, lpLocalHost, nLocalPort) == INVALID_SOCKET)
	{
		//::LogTrace(__FILE__, __LINE__, "CACTCPMainProc.txt", "BeginWork Create localip[%s] localeport[%d] error ",
			//lpLocalHost, nLocalPort);
		delete m_pSocketMainProc;
		m_pSocketMainProc = NULL;
		return -1;
	}

	int nRet = 0;
	nRet = m_pSocketMainProc->Listen(5);
	if (nRet != 0)
	{
		//::LogTrace(__FILE__, __LINE__, "CACTCPMainProc.txt", "BeginWork linten ret[%d] error ", nRet);

		m_pSocketMainProc->Close();
		delete m_pSocketMainProc;
		m_pSocketMainProc = NULL;
		return -1;
	}

	for (int i=0; i<m_nThreadNum; i++)
	{
		m_pSessionMgr[i].InitSessionMgr(m_pDataCallBack, m_pHeadProc, 
			m_nMaxConn/m_nThreadNum);
		int nThread = m_pSessionMgr[i].StartMainLoop();

		ThreadInfo thread;
		thread.dwThreadId = nThread;
		thread.hThread = m_pSessionMgr[i].m_hRecvThread;
		thread.nPort = nLocalPort;
		//g_thredTable.push_back(thread);

		//::LogTrace(__FILE__, __LINE__, "thread.txt", "connect thread:%d, port:%d",
		//	nThread, nLocalPort);
	}

	DWORD dwThreadId = 0;

#ifdef _WIN32
	m_hListenThread = CreateThread(NULL, 0, ListenThreadHelper, (void *)this, 
		0, &dwThreadId);
#else
	if(0 == pthread_create(&m_hListenThread, NULL, ListenThreadHelper, (void *)this))
	{
		dwThreadId = m_hListenThread;	
		//::LogTrace(__FILE__, __LINE__, "CACTCPMainProc.txt", "BeginWork create listenthread threadid [%d]", dwThreadId);
	}
#endif

	ThreadInfo thread;
	thread.dwThreadId = dwThreadId;
	thread.hThread = m_hListenThread;
	thread.nPort = nLocalPort;

	//g_thredTable.push_back(thread);
	//::LogTrace(__FILE__, __LINE__, "thread.txt", "listen thread:%d, port:%d",
	//	m_hListenThread, nLocalPort);

	return 0;
}

int CACTCPMainProc::CloseConnection(SOCKET hSocket)
{
	CACTCPSession * pACTCPSession = NULL;
			
	for (int i=0; i<m_nThreadNum; i++)
	{
		pACTCPSession = m_pSessionMgr[i].GetSessionByFD(hSocket);
		if (pACTCPSession == NULL)
		{
			continue;
		}

		break;
	}
	
	if (pACTCPSession)
	{
		pACTCPSession->TerminateSession();
	}

	return 0;
}

int CACTCPMainProc::SendCommTCPData(WORD wPduType, const BYTE* lpBuf, 
	DWORD dwSize, BOOL bSync, SOCKET hSocket)
{
	int nResult = 0;
	if (bSync==TRUE)
	{
		int nPkgHeadSize = 0;
		BYTE* lpDataSend = NULL;
		if(NULL != m_pHeadProc) //add  by zjj 2009-5-25
		{
			nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
		}
		lpDataSend = new BYTE[dwSize+nPkgHeadSize];
		memset(lpDataSend, 0, dwSize+nPkgHeadSize);		

		if(NULL != m_pHeadProc) //add by zjj 2009-5-25
		{
			m_pHeadProc->GeneratePkgHead(wPduType, dwSize, lpDataSend, 0);
		}
		memcpy(lpDataSend+nPkgHeadSize, lpBuf, dwSize);

		////////////////////////////////////////
		DWORD dwSendCurPos=0;

		while (1)
		{
			nResult = send(hSocket, (char*)(lpDataSend+dwSendCurPos), dwSize+nPkgHeadSize-dwSendCurPos,0);

			if (nResult<=0)
			{
				break;
			}

			dwSendCurPos+=nResult;

			if (dwSendCurPos>=(dwSize+nPkgHeadSize))
			{
				break;
			}
		}

		if (nResult > 0)
		{
			nResult = 0;
		}
		else
		{
			nResult = -1;
		}

		////////////////////////////////////////////

		delete []lpDataSend;
		lpDataSend = NULL;
	}
	else
	{
	}

	return nResult;
}

int CACTCPMainProc::StopMainProc(void)
{
#ifdef _WIN32
	if (m_hListenThread != NULL)
	{
		InterlockedIncrement(&m_nStopFlag);
		::WaitForSingleObject(m_hListenThread, INFINITE);
		CloseHandle(m_hListenThread);
		DelThreadIndex(m_hListenThread);
		m_hListenThread = NULL;
	}
#else
	void *status;
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
	if (m_hListenThread != 0)
	{
printf("myacdll 111111 m_nStopFlag=[%d] \n",m_nStopFlag);
		pthread_join(m_hListenThread,&status);
printf("myacdll 22222222\n");
	}
#endif

	delete []m_pSessionMgr;
	m_pSessionMgr = NULL;

	delete m_pSocketMainProc;
	m_pSocketMainProc = NULL;

	return 0;
}

int CACTCPMainProc::TCPServerListenProc(void)
{
	fd_set ReadFDs;
	int nResult;
	char szLocalHost[16], szLocalMask[16];
	USHORT nLocalPort;

	//::InterlockedIncrement(&g_nThreadCount);
	
	//while (m_nStopFlag < 1) //modify by zjj2009-3-19
	while (1)
	{
#ifndef _WIN32
		long nStopFlag = 0;
		pthread_mutex_lock(&m_mutexStopFlag);
		nStopFlag = m_nStopFlag ;
		pthread_mutex_unlock(&m_mutexStopFlag);
		if(nStopFlag > 0)
		{
			break;
		}
#endif
		
		FD_ZERO(&ReadFDs);
		FD_SET(m_pSocketMainProc->GetSocket(), &ReadFDs);
		
#ifdef _WIN32
		nResult=m_pSocketMainProc->Select(0, &ReadFDs, ACCEPT_TIMEOUT_SET);
#else
		int maxfd = m_pSocketMainProc->GetSocket() + 1;
//printf("CACTCPMainProc::TCPServerListenProc 11111111111 sock[%d]  nStopFlag=[%d] pid=[%d]\n",m_pSocketMainProc->GetSocket(),nStopFlag,getpid());
		nResult=m_pSocketMainProc->Select(maxfd, &ReadFDs, ACCEPT_TIMEOUT_SET);
//printf("CACTCPMainProc::TCPServerListenProc 222222222 nStopFlag=[%d] pid[%d]\n",nStopFlag,getpid());

#endif
		if (nResult <= 0)
		{
			if(nResult < 0)
			{
				//::LogTrace(__FILE__, __LINE__, "CACTCPMainProc.txt", "TCPServerListenProc select error errno [%d]",errno);
			}
			usleep(200);
			continue;
		}
		char szRemoteHost[16];
		USHORT nRemotePort = 0;

		SOCKET hSocket=m_pSocketMainProc->Accept(szRemoteHost, &nRemotePort);			
		if (hSocket == INVALID_SOCKET)
		{
#ifdef _WIN32
			if(WSAGetLastError()==WSAEMFILE)
#else
			if((errno == ENFILE)||(errno == EMFILE))
#endif
			{
		//::LogTrace(__FILE__, __LINE__, "CACTCPMainProc.txt", "TCPServerListenProc accept error errno [%d]",errno);
#ifdef _WIN32
				Sleep(1000);
#else
				sleep(1);
#endif
				continue;
			}

			break;
		}
		
		int nBestMgrIndex = GetBestSessionMgrIndex();
		int nRet = m_pSessionMgr[nBestMgrIndex].AddNewSession(hSocket);
		if (nRet<0)
		{
			//::closesocket(hSocket);
			closesocket(hSocket);
			continue;
		}

		memset(szLocalHost, 0, sizeof(szLocalHost));
		memset(szLocalMask, 0, sizeof(szLocalMask));
		nLocalPort = 0;

		CACSocket::GetSockName(hSocket, szLocalHost, &nLocalPort, szLocalMask);
		m_pDataCallBack->OnNewConnection(hSocket, szLocalHost, nLocalPort, 
			szLocalMask, szRemoteHost, nRemotePort);
	}

	
#ifdef _WIN32
	//m_nStopFlag++; //modif by zjj 2009-3-19
#endif
	m_nStopFlag++; //modif by zjj 2009-10-30
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}

int CACTCPMainProc::GetBestSessionMgrIndex(void)
{
	int nBestMgrIndex = 0;

	for (int i=0; i<m_nThreadNum; i++)
	{
		if (m_pSessionMgr[nBestMgrIndex].GetSessionCount() > m_pSessionMgr[i].GetSessionCount())
		{
			nBestMgrIndex = i;
		}
	}

	return nBestMgrIndex;
}
