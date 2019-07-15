// ACUDPSession.cpp: implementation of the CACUDPSession class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include <stdio.h>
#include <assert.h>

#include "ACUDPSession.h"
#include "ACCommonResource.h"

#ifdef __DEBUG
#define new DEBUG_NEW
#endif
#include "LogTrace.h"

//void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);
#ifdef _WIN32
void DelThreadIndex(HANDLE hThread);
#else
void DelThreadIndex(pthread_t hThread);
#endif

//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
DWORD WINAPI CACUDPSession::RecvThreadHelper(LPVOID lpParameter)
#else
void* CACUDPSession::RecvThreadHelper(LPVOID lpParameter)
#endif
{
	CACUDPSession * pThis = (CACUDPSession *)lpParameter;

	pThis->SessionMainLoop();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACUDPSession::CACUDPSession(void)
{
	m_pDataCallBack = NULL;
	m_pHeadProc = NULL;
	m_nStopFlag = 0;

	m_pSocks5 = NULL;
	memset(m_szProxyHost, 0, sizeof(m_szProxyHost));
	m_nProxyPort = 0;

#ifdef _WIN32
	m_hRecvThread = NULL;
#else
	m_hRecvThread = 0;
	pthread_mutex_init(&m_mutexStopFlag,NULL);
#endif
}

CACUDPSession::~CACUDPSession(void)
{
	if (m_pSocks5 != NULL)
	{
		m_pSocks5->Close();
		delete m_pSocks5;
		m_pSocks5 = NULL;
	}
#ifndef _WIN32
	pthread_mutex_destroy(&m_mutexStopFlag);
#endif
}

int CACUDPSession::InitializeSession(IACDataCallBack* lpDataCallBack,
	IACPackHead* lpHeadProc, LPCTSTR lpLocalHost, USHORT nLocalPort)
{
	if (InitializeSocket(SOCK_DGRAM, lpLocalHost, nLocalPort) != 0)
	{
		return -1;
	}

	//Star add 20050901
	/*
	SOCKET hSocket=m_ACSocket.GetSocket();

	const BOOL on = TRUE;
	::setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
	*/
	//


	m_pDataCallBack = lpDataCallBack;
	m_pHeadProc = lpHeadProc;

	return 0;
}

int CACUDPSession::InitializeSession(IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc, LPCTSTR lpLocalHost,
									 USHORT nLocalPort, LPCTSTR lpRemoteHost, USHORT nRemotePort)
{
	if (InitializeMulSocket(SOCK_DGRAM,lpRemoteHost,nRemotePort, lpLocalHost, nLocalPort) != 0)
	{
		return -1;
	}

	//Star add 20050901
	/*
	SOCKET hSocket=m_ACSocket.GetSocket();

	const BOOL on = TRUE;
	::setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
	*/
	//

	m_pDataCallBack = lpDataCallBack;
	m_pHeadProc = lpHeadProc;

	return 0;
}

int CACUDPSession::InitializeSession(IACDataCallBack* lpDataCallBack, 
	IACPackHead* lpHeadProc, LPCTSTR lpLocalHost, USHORT nLocalPort,
	LPCTSTR lpProxyHost, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword)
{
	int nErrCode = 0;
	if (InitializeSocket(SOCK_DGRAM, lpLocalHost, nLocalPort) != 0)
	{
		return -1;
	}

	//Star add 20050901
	/*
	SOCKET hSocket=m_ACSocket.GetSocket();

	const BOOL on = TRUE;
	::setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
	*/
	//

	char szLocalHost[16];
	char szLocalMask[16];

	CACSocket::GetSockName(GetConnect()->GetSocket(), szLocalHost, &nLocalPort,
		szLocalMask);
	
	if(ConnectToProxy(lpLocalHost, nLocalPort, lpProxyHost, nProxyPort, 
		lpProxyUserName, lpProxyPassword, 2000) == INVALID_SOCKET)
	{
		return(-1);
	}

	m_pDataCallBack = lpDataCallBack;
	m_pHeadProc = lpHeadProc;

	return 0;
}

int CACUDPSession::SendCommData(WORD wPduType, const BYTE* lpBuf, 
	DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
	short nPackID, BOOL bSock5Flag)
{
	int nResult = 0;
	if (bSync==TRUE)
	{
		int nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();

		BYTE* lpDataSend = new BYTE[dwSize+nPkgHeadSize];
		memset(lpDataSend, 0, dwSize+nPkgHeadSize);
			
		m_pHeadProc->GeneratePkgHead(wPduType, dwSize, lpDataSend, nPackID);

		memcpy(lpDataSend+nPkgHeadSize, lpBuf, dwSize);

		if(m_pSocks5!=NULL && bSock5Flag)
		{
			SendByProxy(lpDataSend, dwSize+nPkgHeadSize, lpRemoteHost, nRemotePort);
		}
		else
		{
			nResult = GetConnect()->SendTo((const char*)lpDataSend, dwSize+nPkgHeadSize, 
				lpRemoteHost, nRemotePort);
			if (nResult > 0)
			{
				nResult = 0;
			}
			else
			{
				nResult = -1;
			}
		}
		delete []lpDataSend;
		lpDataSend = NULL;	
	}
	else
	{
	}

	return nResult;
}

int CACUDPSession::StopSessionMainLoop(void)
{
#ifdef _WIN32
	InterlockedIncrement(&m_nStopFlag);

	::WaitForSingleObject(m_hRecvThread, INFINITE);

	if (m_hRecvThread != NULL)
	{
		CloseHandle(m_hRecvThread);
		DelThreadIndex(m_hRecvThread);
		m_hRecvThread = NULL;
	}
#else

	void *status;
	pthread_mutex_lock(&m_mutexStopFlag);
	m_nStopFlag += 1;
	pthread_mutex_unlock(&m_mutexStopFlag);
	if(0 != m_hRecvThread)
	{
		pthread_join(m_hRecvThread,&status);
	}
#endif
	return 0;
}

int CACUDPSession::StartSessionMainLoop(void)
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
#ifndef _WIN32
long CACUDPSession::GetCurrentStopFlag()
{
	long tmpflag=0;
	pthread_mutex_lock(&m_mutexStopFlag);
	tmpflag = m_nStopFlag;	
	pthread_mutex_unlock(&m_mutexStopFlag);
	return tmpflag;
}
#endif

int CACUDPSession::SessionMainLoop(void)
{
	fd_set ReadFDs;
	int nResult;
	CACSocket* pACSocket = GetConnect();
	WORD wPduType = 0;
	long nStopFlag = 0;
	//::InterlockedIncrement(&g_nThreadCount);

	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		nStopFlag = GetCurrentStopFlag();
#endif
		if (nStopFlag > 0)
		{
			break;
		}

		FD_ZERO(&ReadFDs);
		FD_SET(pACSocket->GetSocket(), &ReadFDs);
		
#ifdef _WIN32
		nResult = pACSocket->Select(0, &ReadFDs, ACCEPT_TIMEOUT_SET);
#else
		int maxfd = pACSocket->GetSocket() + 1;
		nResult = pACSocket->Select(maxfd, &ReadFDs, ACCEPT_TIMEOUT_SET);
#endif
		if(nResult > 0)
		{
			char szRemoteHost[16];
			USHORT nRemotePort = 0;
			BYTE dataRecv[UDPSOCKET_BUF_SIZE];

			memset(dataRecv, 0, UDPSOCKET_BUF_SIZE);
			memset(szRemoteHost, 0, sizeof(szRemoteHost));

			int nSize = 0;
			if(m_pSocks5 != NULL)
			{
				nSize = RecvFromProxy(dataRecv, UDPSOCKET_BUF_SIZE, szRemoteHost, 
					&nRemotePort);
				if (nSize<=0)
				{
					continue;
				}
			}
			else
			{
				nSize = pACSocket->ReceiveFrom((char*)dataRecv, UDPSOCKET_BUF_SIZE,
					szRemoteHost, &nRemotePort);
				if (nSize <= 0)
				{
					continue;
				}
			}

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				break;
			}

			int nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
			if (nSize < nPkgHeadSize)
			{
				continue;
			}
			
#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				break;
			}

			WORD wExtParam = 0;
			int nDataSize = m_pHeadProc->GetAndParsePkgHead(dataRecv, 
				&wPduType, &wExtParam);
			//add by kent for check headdata
			if(nDataSize < 0)
			{
				//assert(FALSE);
				break;
			}
			//add by end

			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(pdu:%d, size:%d)", 
			//	wPduType, nDataSize);
			//assert(nDataSize+nPkgHeadSize == nSize);
			if (nDataSize+nPkgHeadSize != nSize)
			{
				char* lpInvData = new char[nSize*3+50];
				char szTmp[5];
							
				memset(lpInvData, 0, nSize*3+50);
				strcpy(lpInvData, "Invalid Pack:");
							
				for (int i=0; i<nSize; i++)
				{
					sprintf(szTmp, "%02x ", dataRecv[i]);
					strcat(lpInvData, szTmp);
				}
	
				//::LogTrace(__FILE__, __LINE__, "invdata.txt", lpInvData);
				delete []lpInvData;
			}

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				break;
			}

			if (nDataSize+nPkgHeadSize <= nSize)
			{
				m_pDataCallBack->OnDataReceived(SOCK_DGRAM, pACSocket->GetSocket(), 
					dataRecv+nPkgHeadSize, nSize-nPkgHeadSize, szRemoteHost, 
					nRemotePort, wPduType, wExtParam);
			}
		}
	}	

	GetConnect()->Close();
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}

int CACUDPSession::ConnectToProxy(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpProxyHost, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword, DWORD dwTimeOut)
{
	char sendBuf[300];
	char recvBuf[300] = {0};
	DWORD dwPos = 0;
	DWORD dwSendLen = 0;
	DWORD dwRecvLen = 0;
	int nRetCode = 0;
	ULONG lLocalHost = 0;
	
	m_pSocks5 = new CACSocket;
	m_pSocks5->Create(SOCK_STREAM);

	//连接到代理服务器
	if (m_pSocks5->Connect(lpProxyHost, nProxyPort) != 0)
	{
		return -1;
	}

	//协商认证方法
	memset(sendBuf, 0, sizeof(sendBuf));
	sendBuf[0] = 0x05;//代理版本号
	sendBuf[1] = 0x01; //认证方法数
	if (NULL!=lpProxyUserName && *lpProxyUserName!=0)
	{
	    assert(lpProxyPassword!=NULL && *lpProxyPassword!=0);
		sendBuf[2] = 0x02;//认证方法(通过用户名/密码认证)		
	}
	else
	{
		sendBuf[2] = 0x00;//认证方法(无验证需求)
	}	

	dwSendLen = 3;	
	dwRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));

	nRetCode = ConsultProcess((BYTE*)sendBuf, dwSendLen, (BYTE*)recvBuf, 
		&dwRecvLen, dwTimeOut);
	if (nRetCode != 0)
	{
		return -1;
	}

	if (recvBuf[0]!=0x05 || recvBuf[1]!=sendBuf[2])
	{
		return -1;
	}

	if (NULL!=lpProxyUserName && *lpProxyUserName!=0)
	{
		//认证用户名/密码
		memset(sendBuf, 0, sizeof(sendBuf));
		dwPos = 0;
		dwSendLen = 0;
		sendBuf[dwPos] = 0x01; //认证版本号
		dwPos += 1;
		dwSendLen += 1;

		sendBuf[dwPos] = (int)strlen(lpProxyUserName); //用户名长度
		dwPos += 1;
		dwSendLen += 1;

		memcpy(&sendBuf[dwPos], lpProxyUserName, strlen(lpProxyUserName));//用户名
		dwPos += (int)strlen(lpProxyUserName);
		dwSendLen += (int)strlen(lpProxyUserName);

		sendBuf[dwPos] = (int)strlen(lpProxyPassword);//密码长度
		dwPos += 1;
		dwSendLen += 1;

		memcpy(&sendBuf[dwPos], lpProxyPassword, strlen(lpProxyPassword));//密码
		dwPos += (int)strlen(lpProxyPassword);
		dwSendLen += (int)strlen(lpProxyPassword);

		dwRecvLen = sizeof(recvBuf);
		memset(recvBuf, 0, sizeof(recvBuf));
		nRetCode = ConsultProcess((BYTE*)sendBuf, dwSendLen, (BYTE*)recvBuf, 
			&dwRecvLen, dwTimeOut);
		if (nRetCode != 0)
		{
			return -1;
		}

		if (recvBuf[0]!=0x01 || recvBuf[1]!=0x00)
		{
			return -1;
		}
	}

	//连接目标服务器(通过代理)
	memset(sendBuf, 0, sizeof(sendBuf));
	dwPos = 0;
	dwSendLen = 0;

	sendBuf[dwPos] = 0x05;
	dwPos += 1;
	dwSendLen += 1;

	sendBuf[dwPos] = 0x03; //UDP
	dwPos += 1;
	dwSendLen += 1;

	sendBuf[dwPos] = 0x00;
	dwPos += 1;
	dwSendLen += 1;

	sendBuf[dwPos] = 0x01; //ipv4
	dwPos += 1;
	dwSendLen += 1;

	lLocalHost = inet_addr(lpLocalHost);	
	memcpy(&sendBuf[dwPos], &lLocalHost, 4);
	dwPos += 4;
	dwSendLen += 4;

	nLocalPort = htons(nLocalPort);
	memcpy(&sendBuf[dwPos], &nLocalPort, 2);
	dwPos += 2;
	dwSendLen += 2;

	dwRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	nRetCode = ConsultProcess((BYTE*)sendBuf, dwSendLen, (BYTE*)recvBuf, 
		&dwRecvLen, dwTimeOut);
	if (nRetCode != 0)
	{
		return -1;
	}

	if (recvBuf[0]!=0x05 || recvBuf[1]!=0x00)
	{
		return -1;
	}

	ULONG lProxyHost = 0;

	memcpy(&lProxyHost, &recvBuf[4], 4);
	CACSocket::AddrToStr(lProxyHost, m_szProxyHost);

	memcpy(&m_nProxyPort, &recvBuf[8],2);
	m_nProxyPort = ntohs(m_nProxyPort);

	return 0;;
}

int CACUDPSession::SendByProxy(const BYTE* lpBuf, DWORD dwSize, LPCTSTR lpRemoteHost, 
	USHORT nRemotePort)
{
	assert(lpBuf != NULL);
	assert(dwSize > 0);
	assert(lpRemoteHost != NULL);
	assert(nRemotePort > 0);	
		
	char* lpDataSend = NULL;
	int nRetLen = 0;
	BYTE headBuf[256];
				
	memset(headBuf, 0, sizeof(headBuf));
	nRetLen = BuildSock5Head(lpRemoteHost, nRemotePort, headBuf, sizeof(headBuf));		
	
	lpDataSend	= new char[dwSize+nRetLen];
	memset(lpDataSend, 0, dwSize+nRetLen);	
	
	memcpy(lpDataSend, headBuf, nRetLen);						
	memcpy(lpDataSend+nRetLen, lpBuf, dwSize);		
	
	GetConnect()->SendTo(lpDataSend, dwSize+nRetLen, m_szProxyHost, m_nProxyPort);
	
	delete []lpDataSend;
	lpDataSend = NULL;

	return 0;
}

int CACUDPSession::BuildSock5Head(LPCTSTR lpRemoteHost, USHORT nRemotePort, 
	BYTE* lpHeadBuf, int nBufLen)
{
	int nPos = 0;
	int nLen = 0;
	ULONG lRemoteHost = 0;

	lpHeadBuf[nPos] = 0x00;//保留2字节的0
	nPos += 1;
	nLen += 1;

	lpHeadBuf[nPos] = 0x00;//保留2字节的0
	nPos += 1;
	nLen += 1;

	lpHeadBuf[nPos] = 0x00; //是否数据报分段重组标志
	nPos += 1;
	nLen += 1;

	lpHeadBuf[nPos] = 0x01; //Ipv4
	nPos += 1;
	nLen += 1;
	
	lRemoteHost = inet_addr(lpRemoteHost);	
	memcpy(&lpHeadBuf[nPos], &lRemoteHost, 4);
	nPos += 4;
	nLen += 4;
	
	nRemotePort = htons(nRemotePort);
	memcpy(&lpHeadBuf[nPos], &nRemotePort, 2);
	nPos += 2;
	nLen += 2;

	return (nLen);
}

int CACUDPSession::RecvFromProxy(BYTE* lpBuf, DWORD dwSize, 
	char *lpRemoteHost, USHORT* lpRemotePort)
{
	assert(lpBuf != NULL);
	assert(dwSize > 0);

	char* dataRecv = NULL;
	dataRecv = new char[dwSize+10];
	memset(dataRecv, 0, dwSize+10);	

	int nBytes = GetConnect()->ReceiveFrom(dataRecv, 
		dwSize+10, lpRemoteHost, lpRemotePort);
	if (strcmp(lpRemoteHost, m_szProxyHost)==0 
		&& (*lpRemotePort)==m_nProxyPort)
	{
		if(nBytes >= 10)
		{
			ULONG lRemoteHost = 0;

			memcpy(&lRemoteHost, &dataRecv[4], 4);
			memcpy(lpRemotePort, &dataRecv[8], 2);

			*lpRemotePort = ntohs(*lpRemotePort);	
			CACSocket::AddrToStr(lRemoteHost, lpRemoteHost);

			memcpy(lpBuf, dataRecv+10, nBytes-10);

			delete []dataRecv;
			return (nBytes-10);
		}
	}
	else
	{
		memcpy(lpBuf, dataRecv, nBytes);
	}

	delete []dataRecv;
	return nBytes;
}

int CACUDPSession::ConsultProcess(const BYTE* lpSendBuf, DWORD dwSendLen, 
	BYTE* lpRecvBuf, DWORD* lpRecvLen, UINT nTimeOut)
{
	int nRetCode = 0;
	fd_set readFds;

	assert(lpSendBuf!=NULL && dwSendLen>0);
	assert(lpRecvBuf!=NULL && lpRecvLen!=NULL && *lpRecvLen>0);

	////////////////////////////////////////////////////////
	DWORD dwSendCurPos=0;

	while (1)
	{
		nRetCode = m_pSocks5->Send((char*)(lpSendBuf+dwSendCurPos), dwSendLen-dwSendCurPos);

		if (nRetCode<0)
		{
			return -1;
		}

		if (nRetCode==0)
		{
			break;
		}

		dwSendCurPos+=nRetCode;

		if (dwSendCurPos>=dwSendLen)
		{
			break;
		}
	}

	nRetCode = dwSendCurPos;
	////////////////////////////////////////////////////////

	FD_ZERO(&readFds);
	FD_SET(m_pSocks5->GetSocket(), &readFds);
#ifdef _WIN32
	nRetCode = m_pSocks5->Select(0, &readFds, nTimeOut);
#else
	int maxfd = m_pSocks5->GetSocket() + 1;
	nRetCode = m_pSocks5->Select(maxfd, &readFds, nTimeOut);
#endif
	if (nRetCode <= 0)
	{
		int nRet = WSAGetLastError();
		return -1;
	}

	nRetCode = m_pSocks5->Receive((char*)lpRecvBuf, *lpRecvLen);
	if (nRetCode <= 0)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error Socket5 UDP ConsultProcess");
		return -1;
	}

	*lpRecvLen = nRetCode;
	return 0;
}
