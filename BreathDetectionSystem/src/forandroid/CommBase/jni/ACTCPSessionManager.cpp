// ACTCPSessionManager.cpp: implementation of the CACTCPSessionManager class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include <assert.h>
#include "ACTCPSessionManager.h"
#include "ACCommonResource.h"
#include "stdio.h"

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
DWORD WINAPI CACTCPSessionManager::RecvThreadHelper(LPVOID lpParameter)
#else
void* CACTCPSessionManager::RecvThreadHelper(LPVOID lpParameter)
#endif
{
	CACTCPSessionManager * pACTCPSessionManager
		= (CACTCPSessionManager *)lpParameter;
	pACTCPSessionManager->SessionMgrMainLoop();

	return 0;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACTCPSessionManager::CACTCPSessionManager(void)
{
	m_nStopFlag=0;
	FD_ZERO(&m_fdset);
	
	m_pDataCallBack = NULL;
	m_pHeadProc = NULL;
	m_pSessions = NULL;
	m_nMaxSessionCount = 0;
	m_nSessionCount = 0;

#ifdef _WIN32
	m_hRecvThread = NULL;
#else
	m_hRecvThread = 0;
	pthread_mutex_init(&m_mutexStopFlag,NULL);
	m_nMaxFd = 0;
#endif
}

CACTCPSessionManager::~CACTCPSessionManager(void)
{
	StopMainLoop();
	
	delete []m_pSessions;
	m_pSessions = NULL;
	
	FD_ZERO(&m_fdset);
#ifndef _WIN32
	pthread_mutex_destroy(&m_mutexStopFlag);
	m_nMaxFd = 0;
#endif
}

void CACTCPSessionManager::InitSessionMgr(ISockEventCallBack* lpDataCallBack,
	ISockHeaderCallBack* lpHeadProc, int nMaxSessionCount)
{
	m_pDataCallBack = lpDataCallBack;
	m_pHeadProc = lpHeadProc;

	m_nMaxSessionCount = nMaxSessionCount;
	m_pSessions = new CACTCPSession[nMaxSessionCount];
	m_nSessionCount = 0;
}


int CACTCPSessionManager::AddNewSession(SOCKET hSocket)
{
	CACTCPSession* pACTCPSession = NULL;

	assert(m_pSessions != NULL);

	for (int i=0; i<m_nMaxSessionCount; i++)
	{
		if (m_pSessions[i].GetConnect()->GetSocket() == INVALID_SOCKET)
		{
			pACTCPSession = &m_pSessions[i];
			break;
		}
	}

	if (pACTCPSession == NULL)
	{
		return -1;
	}

	pACTCPSession->AttachSocket(hSocket);

	FD_SET(hSocket, &m_fdset);
	m_nSessionCount++;
#ifndef _WIN32
	if(hSocket > m_nMaxFd)
	{
		m_nMaxFd = hSocket;
	}
#endif

	return 0;
}

CACTCPSession * CACTCPSessionManager::GetSessionByFD(SOCKET hSocket)
{
	for (int i=0; i<m_nMaxSessionCount; i++)
	{
		if (m_pSessions[i].GetConnect()->GetSocket()==hSocket)
		{
			if (m_pSessions[i].IsTerminated()==FALSE)
			{
				return NULL;
			}

			return &m_pSessions[i];
		}
	}
	
	return NULL;
}

void CACTCPSessionManager::RemoveSession(SOCKET hSocket)
{
	FD_CLR(hSocket, &m_fdset);
	m_nSessionCount--;
}

CACTCPSession * CACTCPSessionManager::GetActiveSession(int nTimeOut)
{
	struct timeval tv;
	int nRet = 0;
	fd_set fdset = m_fdset;

	if (m_nSessionCount == 0)
	{
#ifdef _WIN32
		Sleep(50);
#else
		usleep(50000);
#endif
		return NULL;
	}

	for(int i=0; i<m_nMaxSessionCount; i++)
	{
		if(m_pSessions[i].IsTerminated()==TRUE &&
			m_pSessions[i].GetConnect()->GetSocket()!=INVALID_SOCKET)
		{
			return &m_pSessions[i];
		}
	}

	tv.tv_sec=nTimeOut/1000;
	tv.tv_usec=1000*(nTimeOut%1000);
	
#ifdef _WIN32
	nRet = select(0, &fdset, NULL, NULL, &tv);	
#else
	nRet = select(m_nMaxFd + 1, &fdset, NULL, NULL, &tv);	
#endif
	if(nRet<=0)
	{
		if (nRet < 0)
		{
#ifdef _WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
		}
		return NULL;
	}

	for(int i=0; i<m_nMaxSessionCount; i++)
	{
		if(FD_ISSET(m_pSessions[i].GetConnect()->GetSocket(), &fdset))
		{
			return &m_pSessions[i];
		}		
	}

	return NULL;
}

int CACTCPSessionManager::StartMainLoop(void)
{
	DWORD dwThreadId = 0;

#ifdef _WIN32
	m_hRecvThread = CreateThread(NULL, 0, RecvThreadHelper, 
		(void *)this, 0, &dwThreadId);
#else
	if(0 == pthread_create(&m_hRecvThread, NULL, RecvThreadHelper, (void *)this))
	{
		dwThreadId = m_hRecvThread;	
	}
#endif
	return dwThreadId;
}

void CACTCPSessionManager::StopMainLoop(void)
{
#ifdef _WIN32
	if (m_hRecvThread == NULL)
#else
	if (0 == m_hRecvThread )
#endif
	{
		return;
	}

	for (int i=0; i<m_nMaxSessionCount; i++)
	{
		if (m_pSessions[i].IsTerminated() == FALSE)
		{
			m_pSessions[i].TerminateSession();
		}
	}

#ifdef _WIN32
	InterlockedIncrement(&m_nStopFlag);
#else
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
#endif

	while(GetSessionCount()>0)
	{
#ifdef _WIN32
		Sleep(ACCEPT_TIMEOUT_SET);
#else
		usleep(ACCEPT_TIMEOUT_SET * 1000);
#endif
	}

#ifdef _WIN32
	InterlockedIncrement(&m_nStopFlag);

	::WaitForSingleObject(m_hRecvThread, INFINITE);
	CloseHandle(m_hRecvThread);
	DelThreadIndex(m_hRecvThread);
	m_hRecvThread = NULL;
#else
	void *status;
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
printf("CACTCPSessionManager::StopMainLoop 1111 \n");
	pthread_join(m_hRecvThread,&status);
printf("CACTCPSessionManager::StopMainLoop 2222 \n");
#endif
}

int CACTCPSessionManager::ParsePackageData(CACTCPSession * pACTCPSession,
	int nPkgHeadSize)
{
	char szRemoteHost[16];
	USHORT nRemotePort = 0;
	WORD wPduType = 0;
	unsigned long nDataLen = 0;
	int nRet = 0;

	memset(szRemoteHost, 0, sizeof(szRemoteHost));
	pACTCPSession->GetConnect()->GetPeerName(szRemoteHost, &nRemotePort);

	//数据包中包括包头
	if (pACTCPSession->IsHeadSegment()==TRUE)
	{
		WORD wExtParam = 0;
		//解开头，获取数据长度
		//nDataLen = m_pHeadProc->GetAndParsePkgHead(pACTCPSession->GetHeadData(), 
			//&wPduType, &wExtParam);
		nRet = m_pHeadProc->ParsePacketHeader(pACTCPSession->GetHeadData(), wPduType, nDataLen);

		//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(pdu:%d, size:%d)", 
		//	wPduType, nDataLen);

		//if (nDataLen < 0)
		if (nRet != 0)
		{
			pACTCPSession->ResetRecvBufData();
			return -1;
		}

		pACTCPSession->SetPduType(wPduType);
		pACTCPSession->SetHeadSegmentFlag(FALSE);
		//设置数据长度
		pACTCPSession->SetPayLoadSize(nDataLen);

		if (nDataLen == 0)
		{
			//m_pDataCallBack->OnDataReceived(SOCK_STREAM, 
				//pACTCPSession->GetConnect()->GetSocket(), NULL, 0,
				//szRemoteHost, nRemotePort, wPduType, wExtParam);
			m_pDataCallBack->OnTCPPacketReceived( pACTCPSession->GetConnect()->GetSocket(), wPduType,NULL, 0);
			//复位数据
			pACTCPSession->ResetRecvBufData();
		}

		return 0;
	}

	//通知上层收到了一个完整的TCP数据包
	//m_pDataCallBack->OnDataReceived(SOCK_STREAM,
		//pACTCPSession->GetConnect()->GetSocket(), 
		//pACTCPSession->GetPayLoadData(),
		//pACTCPSession->GetPayLoadSize(), szRemoteHost, nRemotePort, 
		//pACTCPSession->GetPduType(), pACTCPSession->GetPackID());
	m_pDataCallBack->OnTCPPacketReceived( pACTCPSession->GetConnect()->GetSocket(), 
		pACTCPSession->GetPduType(), pACTCPSession->GetPayLoadData(), pACTCPSession->GetPayLoadSize());
		
	//复位数据
	if(NULL != m_pHeadProc) //modify by zjj 2009-5-25
	{
		pACTCPSession->ResetRecvBufData();
	}
	else
	{
		pACTCPSession->ResetRecvBufData(FALSE);
	}	

	return 0;
}


int CACTCPSessionManager::SessionMgrMainLoop(void)
{
	SOCKET hSocket = INVALID_SOCKET;
	long nStopFlag = 0;

	//获取数据包头长度
	int nPkgHeadSize = 0;
	if(NULL != m_pHeadProc) //modify by zjj 2009-5-25
	{
		nPkgHeadSize = m_pHeadProc->GetPacketHeaderSize();
	}
	//::InterlockedIncrement(&g_nThreadCount);
	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		pthread_mutex_lock(&m_mutexStopFlag);
		nStopFlag = m_nStopFlag;
		pthread_mutex_unlock(&m_mutexStopFlag);
#endif
		//if (nStopFlag > 1)
		if (nStopFlag > 0) //modify by zjj 2009-10-30
		{
			break;
		}

		//获取活动会话
		CACTCPSession * pACTCPSession = GetActiveSession(ACCEPT_TIMEOUT_SET);		
		if (pACTCPSession == NULL)
		{
			continue;
		}
//add by zjj 2009-5-25
		if(NULL == m_pHeadProc)
		{
			pACTCPSession->ResetRecvBufData(FALSE);
			int nRet = pACTCPSession->GetDateSize() ;
			if(nRet < 0)
			{	
			 	continue;
			}
		}
//end add by zjj 2009-5-25

		//当前活动会话已终止
		if(pACTCPSession->IsTerminated()==TRUE)
		{
			//取终止会话的套接字
			hSocket = pACTCPSession->GetConnect()->GetSocket();
			//删除终止的会话
			RemoveSession(hSocket);
			//关闭终止会话的连接
			pACTCPSession->CloseSessionConnect();
			//通知上层，连接已关闭
			m_pDataCallBack->OnConnectionClosed(hSocket);
			continue;
		}

		//正在关闭连接
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		pthread_mutex_lock(&m_mutexStopFlag);
		nStopFlag = m_nStopFlag;
		pthread_mutex_unlock(&m_mutexStopFlag);
#endif
		if (nStopFlag > 0)
		{
			continue;
		}

		int nReadDataSize = 0;
		//应该收数据包头
		if(pACTCPSession->IsHeadSegment())
		{
			//接收数据
			nReadDataSize = pACTCPSession->ReadPkgHead(nPkgHeadSize);

			if (nReadDataSize <= 0)
			{
				pACTCPSession->TerminateSession();
				continue;
			}

			//未收到完整包头
			if (nReadDataSize < nPkgHeadSize)
			{
				continue;
			}
			
			//已收到完整包头，分析数据
			ParsePackageData(pACTCPSession, nPkgHeadSize);	
			continue;
		}

		if(NULL == m_pHeadProc) //modify by zjj 2009-5-25
		{
			while(1)
			{
				//包头已接收，接收数据
				nReadDataSize = pACTCPSession->ReadPkgData();
				////////////////////////////////////////////////
				if (nReadDataSize <= 0)
				{
					pACTCPSession->TerminateSession();
					break;
				}
				//分析数据
				if (pACTCPSession->GetRDataBytes() == pACTCPSession->GetPayLoadSize())
				{
					ParsePackageData(pACTCPSession, nPkgHeadSize);
					break;
				}
			}
		}
		else
		{
			//包头已接收，接收数据
			nReadDataSize = pACTCPSession->ReadPkgData();
			////////////////////////////////////////////////
			if (nReadDataSize <= 0)
			{
				pACTCPSession->TerminateSession();
				continue;
			}

			//分析数据
			if (pACTCPSession->GetRDataBytes() == pACTCPSession->GetPayLoadSize())
			{
				ParsePackageData(pACTCPSession, nPkgHeadSize);
			}
		}
		//end modify by zjj 2009-5-25
	}
	
#ifdef _WIN32
	InterlockedIncrement(&m_nStopFlag);
#else
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
#endif
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}
//modifiy by zjj 2009-5-25
/*
int CACTCPSessionManager::SessionMgrMainLoop(void)
{
	SOCKET hSocket = INVALID_SOCKET;
	long nStopFlag = 0;

	//获取数据包头长度
	int nPkgHeadSize = m_pHeadProc->GetPacketHeaderSize();
	//::InterlockedIncrement(&g_nThreadCount);
	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		pthread_mutex_lock(&m_mutexStopFlag);
		nStopFlag = m_nStopFlag;
		pthread_mutex_unlock(&m_mutexStopFlag);
#endif
		if (nStopFlag > 1)
		{
			break;
		}

		//获取活动会话
		CACTCPSession * pACTCPSession = GetActiveSession(ACCEPT_TIMEOUT_SET);		
		if (pACTCPSession == NULL)
		{
			continue;
		}

		//当前活动会话已终止
		if(pACTCPSession->IsTerminated()==TRUE)
		{
			//取终止会话的套接字
			hSocket = pACTCPSession->GetConnect()->GetSocket();
			//关闭终止会话的连接
			pACTCPSession->CloseSessionConnect();
			//删除终止的会话
			RemoveSession(hSocket);
			//通知上层，连接已关闭
			m_pDataCallBack->OnConnectionClosed(hSocket);
			continue;
		}

		//正在关闭连接
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		pthread_mutex_lock(&m_mutexStopFlag);
		nStopFlag = m_nStopFlag;
		pthread_mutex_unlock(&m_mutexStopFlag);
#endif
		if (nStopFlag > 0)
		{
			continue;
		}

		int nReadDataSize = 0;
		//应该收数据包头
		if(pACTCPSession->IsHeadSegment())
		{
			//接收数据
			nReadDataSize = pACTCPSession->ReadPkgHead(nPkgHeadSize);

			if (nReadDataSize <= 0)
			{
				pACTCPSession->TerminateSession();
				continue;
			}

			//未收到完整包头
			if (nReadDataSize < nPkgHeadSize)
			{
				continue;
			}
			
			//已收到完整包头，分析数据
			ParsePackageData(pACTCPSession, nPkgHeadSize);	
			continue;
		}

		//包头已接收，接收数据
		nReadDataSize = pACTCPSession->ReadPkgData();
		////////////////////////////////////////////////
		if (nReadDataSize <= 0)
		{
			pACTCPSession->TerminateSession();
			continue;
		}

		//分析数据
		if (pACTCPSession->GetRDataBytes() == pACTCPSession->GetPayLoadSize())
		{
			ParsePackageData(pACTCPSession, nPkgHeadSize);
		}
	}
	
#ifdef _WIN32
	InterlockedIncrement(&m_nStopFlag);
#else
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
#endif
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}
*/

int CACTCPSessionManager::GetSessionCount(void)
{
	return m_nSessionCount;
}
