//#include "StdAfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ACTCPClientProc.h"
#include "ACSocket.h"

#include <sys/ioctl.h>


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

char* g_pHeadPre = "POST /server/default.jsp HTTP/1.1\r\nHost: localhost\r\nContent-Length: ";

CACTCPClientProc::CACTCPClientProc(void)
{
	m_nStopFlag = 0;
	m_pSocket = NULL;

	m_pPackHead = NULL;
//	m_pPackData = NULL;  //begin modify by zjj 2009-5-25
	m_dwMaxBufLen = MaxReciveBufferLen;
	m_pPackData = new BYTE[m_dwMaxBufLen];
//end modify 
	m_bHeadFlag = TRUE;
	m_dwRHeadBytes = 0;
	m_dwRDataBytes = 0;

	memset(m_szSvrAddr, 0, 16);
	m_nSvrPort = 0;

	m_bHttpFlag = FALSE;
#ifdef _WIN32
	m_hRecvThread = NULL;
#else
	m_hRecvThread = 0;
	pthread_mutex_init(&m_mutexStopFlag,NULL);
#endif
}

CACTCPClientProc::~CACTCPClientProc(void)
{
	if (m_pPackHead != NULL)
	{
		delete []m_pPackHead;
		m_pPackHead = NULL;
	}

	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
#ifndef _WIN32
	pthread_mutex_destroy(&m_mutexStopFlag);
#endif
}

void CACTCPClientProc::Initialize(short nCommType, short nThreadNum, short nMaxConn,
	IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc)
{
	CACBaseMainProc::Initialize(nCommType, 1, 1, lpDataCallBack, lpHeadProc);
}

int CACTCPClientProc::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort,
	LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint)
{
	char szLocalHost[16];
	char szLocalMask[16];

	m_pSocket = new CACSocket();	
	if (m_pSocket->Create(SOCK_STREAM, lpLocalHost, nLocalPort,TRUE) == INVALID_SOCKET)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","BeginWork Create Error [%s][%d]",lpLocalHost,nLocalPort);
		delete m_pSocket;
		m_pSocket = NULL;
		return -1;
	}

	int nRet=m_pSocket->Connect(lpRemoteHost, nRemotePort);
	if (nRet== SOCKET_ERROR)
	{
#ifdef _WIN32
		DWORD dwError=GetLastError();
		LogTrace(__FILE__,__LINE__,"ReceiveError.txt","BeginWork Connect  Error=[%ld] [%s][%d]",dwError,lpRemoteHost,nRemotePort);	    
#else
		LogTrace(__FILE__,__LINE__,"ReceiveError.txt","BeginWork Connect  Error=[%d] [%s][%d]",errno,lpRemoteHost,nRemotePort);	    
#endif
		m_pSocket->Close();
		delete m_pSocket;
		m_pSocket = NULL;
		return nRet;
	}
										
	memset(szLocalHost, 0, sizeof(szLocalHost));
	memset(szLocalMask, 0, sizeof(szLocalMask));
	
	strncpy(m_szSvrAddr, lpRemoteHost, 16);
	m_nSvrPort = nRemotePort;

	DWORD dwThreadID = StartMainProc();
	CACSocket::GetSockName(m_pSocket->GetSocket(), szLocalHost, 
		&nLocalPort, szLocalMask);
	m_pDataCallBack->OnNewConnection(m_pSocket->GetSocket(), szLocalHost, 
		nLocalPort, szLocalMask, lpRemoteHost, nRemotePort);

	ThreadInfo thread;
	thread.dwThreadId = dwThreadID;
	thread.hThread = m_hRecvThread;
	thread.nPort = nLocalPort;
	//g_thredTable.push_back(thread);

	//::LogTrace(__FILE__, __LINE__, "thread.txt", "thread:%d, port:%d",
	//	dwThreadID, nLocalPort);
	return 0;
}

int CACTCPClientProc::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpRemoteHost, USHORT nRemotePort, LPCTSTR pProxyHost, 
	USHORT nProxyPort, LPCTSTR pProxyUserName, LPCTSTR pProxyPassword)
{
	char szLocalHost[16];
	char szLocalMask[16];

	m_pSocket = new CACSocket();	
	if (m_pSocket->Create(SOCK_STREAM, lpLocalHost, nLocalPort,TRUE) == INVALID_SOCKET)
	{
		delete m_pSocket;
		m_pSocket = NULL;
		return -1;
	}

	if (ConnectBySock5(lpRemoteHost,nRemotePort, pProxyHost, nProxyPort, pProxyUserName, 
		pProxyPassword, 2000) != 0)
	{
		m_pSocket->Close();
		delete m_pSocket;
		m_pSocket = NULL;
		return -1;
	}
	
	memset(szLocalHost, 0, sizeof(szLocalHost));
	memset(szLocalMask, 0, sizeof(szLocalMask));
	
	strncpy(m_szSvrAddr, lpRemoteHost, 16);
	m_nSvrPort = nRemotePort;

	DWORD dwThreadID = StartMainProc();
	CACSocket::GetSockName(m_pSocket->GetSocket(), szLocalHost, &nLocalPort, 
		szLocalMask);
	m_pDataCallBack->OnNewConnection(m_pSocket->GetSocket(), szLocalHost, nLocalPort, 
		szLocalMask, lpRemoteHost, nRemotePort);

	//::LogTrace(__FILE__, __LINE__, "thread.txt", "thread:%d, port:%d",
	//	dwThreadID, nLocalPort);
	return 0;	
}

int CACTCPClientProc::BeginWorkHttp(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpRemoteHost, USHORT nRemotePort, LPCTSTR pProxyHost, 
	USHORT nProxyPort, LPCTSTR pProxyUserName, LPCTSTR pProxyPassword)
{
	char szLocalHost[16];
	char szLocalMask[16];

	m_pSocket = new CACSocket();	
	if (m_pSocket->Create(SOCK_STREAM, lpLocalHost, nLocalPort,TRUE) == INVALID_SOCKET)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","BeginWorkHttp Create Error [%s][%d]",lpLocalHost,nLocalPort);
		delete m_pSocket;
		m_pSocket = NULL;
		return -1;
	}
			
	if (ConnectByHttp(lpRemoteHost,nRemotePort, pProxyHost, nProxyPort, 
		pProxyUserName, pProxyPassword, 2000) != 0)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","ConnectByHttp  Error [%s][%d]  pProxyHost=[%s][%d]",lpRemoteHost,nRemotePort,pProxyHost,nProxyPort);
		m_pSocket->Close();
		delete m_pSocket;
		m_pSocket = NULL;
		return -1;
	}
								
	memset(szLocalHost, 0, sizeof(szLocalHost));
	memset(szLocalMask, 0, sizeof(szLocalMask));
	
	strncpy(m_szSvrAddr, lpRemoteHost, 16);
	m_nSvrPort = nRemotePort;

	DWORD dwThreadID = StartMainProc();
	CACSocket::GetSockName(m_pSocket->GetSocket(), szLocalHost, 
		&nLocalPort, szLocalMask);
	m_pDataCallBack->OnNewConnection(m_pSocket->GetSocket(), 
		szLocalHost, nLocalPort, szLocalMask, lpRemoteHost, 
		nRemotePort);

	//::LogTrace(__FILE__, __LINE__, "thread.txt", "thread:%d, port:%d",
	//	dwThreadID, nLocalPort);

	return 0;
}

int CACTCPClientProc::CloseConnection(SOCKET hSocket)
{
	if (m_pSocket != NULL)
	{
		m_pSocket->Close();
	}

	return 0;
}
	
DWORD CACTCPClientProc::GetHttpHeadLength(void)
{
	return strlen(g_pHeadPre)+14;
}

void CACTCPClientProc::BuildHttpHead(DWORD dwDataSize, BYTE* lpHeadBuf)
{
	assert(lpHeadBuf != NULL);
	assert(dwDataSize > 0);

	DWORD dwPosIndex = 0; 
	char szDataLen[15];
	sprintf(szDataLen, "%010d\r\n\r\n", dwDataSize);

	memcpy(&lpHeadBuf[dwPosIndex], g_pHeadPre, strlen(g_pHeadPre));
	dwPosIndex += strlen(g_pHeadPre);

	memcpy(&lpHeadBuf[dwPosIndex], szDataLen, strlen(szDataLen));
	dwPosIndex += strlen(szDataLen);
}
//begin modify by zjj 2009-5-25
int CACTCPClientProc::SendCommTCPData(WORD wPduType, const BYTE* lpBuf, 
	DWORD dwSize, BOOL bSync, SOCKET hSocket)
{
	int nResult = 0;
	if (bSync==TRUE)
	{
		if (m_bHttpFlag == FALSE) //不使用HTTP协议
		{
//modify by zjj 2009-5-25
			int nPkgHeadSize = 0; 
			BYTE* lpDataSend = NULL;
			if(NULL == m_pHeadProc)
			{
				lpDataSend = new BYTE[dwSize+nPkgHeadSize];
				memset(lpDataSend, 0, dwSize+nPkgHeadSize);
			}
			else
			{
				nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
				lpDataSend = new BYTE[dwSize+nPkgHeadSize];
				memset(lpDataSend, 0, dwSize+nPkgHeadSize);
			
				m_pHeadProc->GeneratePkgHead(wPduType, dwSize, lpDataSend, 0);
			}
//end modify by zjj 2009-5-25
			memcpy(lpDataSend+nPkgHeadSize, lpBuf, dwSize);

			if (m_pSocket == NULL)
			{
				if(NULL != lpDataSend) //modify a bug by zjj 2010-7-8 
				{
					delete []lpDataSend; 
				}
				lpDataSend = NULL;
				return -2;
			}

			DWORD dwSendCurPos=0;
			
			while (1)
			{
				nResult = m_pSocket->Send((char*)(lpDataSend+dwSendCurPos), dwSize+nPkgHeadSize-dwSendCurPos);

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

			delete []lpDataSend;
			lpDataSend = NULL;
		}
		else //使用HTTP协议
		{
			DWORD dwPosIndex = 0;
			DWORD dwHttpHeadLen = GetHttpHeadLength();
			int nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
			BYTE* lpDataSend = new BYTE[dwHttpHeadLen+dwSize+nPkgHeadSize+4];
			memset(lpDataSend, 0, dwSize+nPkgHeadSize+dwHttpHeadLen);
		
			BuildHttpHead(dwSize+nPkgHeadSize, &lpDataSend[dwPosIndex]);
			dwPosIndex += dwHttpHeadLen;

			m_pHeadProc->GeneratePkgHead(wPduType, dwSize, &lpDataSend[dwPosIndex], 0);
			dwPosIndex += nPkgHeadSize;

			memcpy(&lpDataSend[dwPosIndex], lpBuf, dwSize);
			dwPosIndex += dwSize;

			//memcpy(&lpDataSend[dwPosIndex], "\r\n\r\n", 4);
			//dwPosIndex += 4;

			if (m_pSocket == NULL)
			{
				delete []lpDataSend;
				lpDataSend = NULL;

				return -2;
			}

			DWORD dwSendCurPos=0;

			while (1)
			{
				nResult = m_pSocket->Send((char*)(lpDataSend+dwSendCurPos), dwSize+nPkgHeadSize+dwHttpHeadLen-dwSendCurPos);

				if (nResult<=0)
				{
					break;
				}

				dwSendCurPos+=nResult;

				if (dwSendCurPos>=(dwSize+nPkgHeadSize+dwHttpHeadLen))
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

			delete []lpDataSend;
			lpDataSend = NULL;
		}
	}

	return nResult;
}

/*
int CACTCPClientProc::SendCommTCPData(WORD wPduType, const BYTE* lpBuf, 
	DWORD dwSize, BOOL bSync, SOCKET hSocket)
{
	int nResult = 0;
	if (bSync==TRUE)
	{
		if (m_bHttpFlag == FALSE) //不使用HTTP协议
		{
			int nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
			BYTE* lpDataSend = new BYTE[dwSize+nPkgHeadSize];
			memset(lpDataSend, 0, dwSize+nPkgHeadSize);
		
			m_pHeadProc->GeneratePkgHead(wPduType, dwSize, lpDataSend, 0);
			memcpy(lpDataSend+nPkgHeadSize, lpBuf, dwSize);

			if (m_pSocket == NULL)
			{
				return -2;
			}

			DWORD dwSendCurPos=0;
			
			while (1)
			{
				nResult = m_pSocket->Send((char*)(lpDataSend+dwSendCurPos), dwSize+nPkgHeadSize-dwSendCurPos);

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

			delete []lpDataSend;
			lpDataSend = NULL;
		}
		else //使用HTTP协议
		{
			DWORD dwPosIndex = 0;
			DWORD dwHttpHeadLen = GetHttpHeadLength();
			int nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
			BYTE* lpDataSend = new BYTE[dwHttpHeadLen+dwSize+nPkgHeadSize+4];
			memset(lpDataSend, 0, dwSize+nPkgHeadSize+dwHttpHeadLen);
		
			BuildHttpHead(dwSize+nPkgHeadSize, &lpDataSend[dwPosIndex]);
			dwPosIndex += dwHttpHeadLen;

			m_pHeadProc->GeneratePkgHead(wPduType, dwSize, &lpDataSend[dwPosIndex], 0);
			dwPosIndex += nPkgHeadSize;

			memcpy(&lpDataSend[dwPosIndex], lpBuf, dwSize);
			dwPosIndex += dwSize;

			//memcpy(&lpDataSend[dwPosIndex], "\r\n\r\n", 4);
			//dwPosIndex += 4;

			if (m_pSocket == NULL)
			{
				delete []lpDataSend;
				lpDataSend = NULL;

				return -2;
			}

			DWORD dwSendCurPos=0;

			while (1)
			{
				nResult = m_pSocket->Send((char*)(lpDataSend+dwSendCurPos), dwSize+nPkgHeadSize+dwHttpHeadLen-dwSendCurPos);

				if (nResult<=0)
				{
					break;
				}

				dwSendCurPos+=nResult;

				if (dwSendCurPos>=(dwSize+nPkgHeadSize+dwHttpHeadLen))
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

			delete []lpDataSend;
			lpDataSend = NULL;
		}
	}

	return nResult;
}
*/
//end modify by zjj 2009-5-25

#ifndef _WIN32
long CACTCPClientProc::GetCurrentStopFlag()
{
	long tmpflag=0;
	pthread_mutex_lock(&m_mutexStopFlag);
	tmpflag = m_nStopFlag;	
	pthread_mutex_unlock(&m_mutexStopFlag);
	return tmpflag;
}
#endif
int CACTCPClientProc::StopMainProc(void)
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
		m_hRecvThread = 0;
	}
#endif


	return 0;
}

void CACTCPClientProc::SetHttpFlag(void)
{
	m_bHttpFlag = TRUE;
}

int CACTCPClientProc::SessionMainLoop(void)
{
	//add by chengwei 2004.12.15
	//SOCKET hSocket=INVALID_SOCKET;
	SOCKET hSocket = m_pSocket->GetSocket();
	fd_set ReadFDs;
	int nPkgHeadSize = 0;
	WORD wExtParam = 0;
	WORD wPduType = 0;
	int nReadDataSize = 0;
	long nStopFlag = 0;

	//获取数据包头长度
//begin modify by zjj 2009-5-25
	if(NULL == m_pHeadProc)
	{
		nPkgHeadSize = 0;
		m_bHeadFlag = FALSE;
		m_pPackHead = NULL;
	}
	else
	{
		nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
		m_bHeadFlag = TRUE;
		m_pPackHead = new BYTE[nPkgHeadSize];
	}
	//m_pPackData = NULL;
//end modify  by zjj 2009-5-25
	m_dwRHeadBytes = 0;
	m_dwRDataBytes = 0;
	m_dwDataSize = 0;

	//::InterlockedIncrement(&g_nThreadCount);
	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		nStopFlag = GetCurrentStopFlag();
#endif
		//正在关闭连接
		if (nStopFlag > 0)
		{
			CloseConnection();
			break;
		}

		FD_ZERO(&ReadFDs);
		FD_SET(m_pSocket->GetSocket(), &ReadFDs);
		
#ifdef _WIN32
		int nRet = m_pSocket->Select(0, &ReadFDs, ACCEPT_TIMEOUT_SET);
#else
		int maxfd = m_pSocket->GetSocket() + 1;
		int nRet = m_pSocket->Select(maxfd, &ReadFDs, ACCEPT_TIMEOUT_SET);
#endif
		if (nRet <= 0)
		{
#ifdef _WIN32
			Sleep(30);
#else
			usleep(30000);
#endif
			continue;
		}


		nReadDataSize = 0;
		//应该收数据包头
		if(m_bHeadFlag)
		{
			//接收数据
			nReadDataSize = m_pSocket->Receive((char*)m_pPackHead+m_dwRHeadBytes, 
				nPkgHeadSize-m_dwRHeadBytes);
			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(%d bytes)", nReadDataSize);

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nReadDataSize <= 0)
			{
				//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error SessionMainLoop1");
				CloseConnection();
				if (nStopFlag == 0)
				{
					m_pDataCallBack->OnConnectionClosed(hSocket);
				}
				break;
			}
			m_dwRHeadBytes += nReadDataSize;

			//未收到完整包头
			if (m_dwRHeadBytes < nPkgHeadSize)
			{
				continue;
			}
			
			//解开头，获取数据长度
			wExtParam = 0;
			wPduType = 0;

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				CloseConnection();
				break;
			}

			m_dwDataSize = m_pHeadProc->GetAndParsePkgHead(m_pPackHead, 
				&wPduType, &wExtParam);
			//add by kent for check headdata
			if(m_dwDataSize < 0)
			{
				assert("收到数据有误!");
				CloseConnection();
				break;
			}
			//add by end

			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(pdu:%d, len:%d)", 
			//	wPduType, m_dwDataSize);

			//数据体为空
			if (m_dwDataSize == 0)
			{
#ifdef _WIN32
				InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
				nStopFlag = GetCurrentStopFlag();
#endif
				if (nStopFlag > 0)
				{
					CloseConnection();
					break;
				}
				m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
					NULL, 0, m_szSvrAddr, m_nSvrPort, wPduType, 0);

				m_bHeadFlag = TRUE;
				m_dwRHeadBytes = 0;
			}
			else
			{
				//m_pPackData = new BYTE[m_dwDataSize]; //modify by zjj 2009-5-25
				if(m_dwDataSize > m_dwMaxBufLen)
				{
					m_dwMaxBufLen = m_dwDataSize + 256;
					delete[] m_pPackData;
					m_pPackData = new BYTE[m_dwMaxBufLen];
				}
				//end modify by zjj 2009-5-25

				m_dwRDataBytes = 0;
				m_bHeadFlag = FALSE;
			}

			continue;
		}
//begin modify  by zjj 2009-5-25	
		if(NULL == m_pHeadProc)
		{
			m_dwDataSize = 0;
			nRet = ioctl(m_pSocket->GetSocket(),FIONREAD,&m_dwDataSize);
	//printf(" ACTCPClientProc.cpp after ioctl m_dwDataSize[%d] \n",m_dwDataSize);
			if(nRet < 0)
			{
				//printf("SessionMainLoop  ioctl error \n");
				continue;
			}
			else
			{
				if(m_dwDataSize > m_dwMaxBufLen)
				{
					m_dwMaxBufLen = m_dwDataSize + 256;
					delete[] m_pPackData;
					m_pPackData = new BYTE[m_dwMaxBufLen];
				}
			}
			m_dwRDataBytes = 0;
			nReadDataSize = 0;
			BOOL tmpBreak = FALSE;
			while(1)
			{

				//包头已接收，接收数据
				nReadDataSize = m_pSocket->Receive((char*)m_pPackData+m_dwRDataBytes, 
					m_dwDataSize-m_dwRDataBytes);
				//::LogTrace(__FILE__, __LINE__, "server.txt", "receive body(%d bytes)", 
				//	nReadDataSize);
				
				if (nReadDataSize <= 0)
				{

				//	LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error SessionMainLoop2");
					CloseConnection();
					m_pDataCallBack->OnConnectionClosed(hSocket);
					tmpBreak = TRUE;

					break;
				}

				m_dwRDataBytes += nReadDataSize;
				//分析数据
				if (m_dwDataSize == m_dwRDataBytes)
				{
		#ifdef _WIN32
					InterlockedExchange(&nStopFlag, m_nStopFlag);
		#else
					nStopFlag = GetCurrentStopFlag();
		#endif
					if (nStopFlag > 0)
					{
						CloseConnection();
						tmpBreak = TRUE;
						break;
					}

					m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
						m_pPackData, m_dwDataSize, m_szSvrAddr, m_nSvrPort, wPduType, 0);

					//delete []m_pPackData; //modify by zjj 2009-5-25
					//m_pPackData = NULL;
				//	m_bHeadFlag = TRUE; modify by zjj 2009-5-25
					m_dwRHeadBytes = 0;
					m_dwRDataBytes = 0;
					m_dwDataSize = 0;
					break;
				}
			}
			if(tmpBreak)
			{
				break;
			}
		}
		else
		{
			//包头已接收，接收数据
			nReadDataSize = m_pSocket->Receive((char*)m_pPackData+m_dwRDataBytes, 
				m_dwDataSize-m_dwRDataBytes);
			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive body(%d bytes)", 
			//	nReadDataSize);
			
			if (nReadDataSize <= 0)
			{
				//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error SessionMainLoop2");
				CloseConnection();
				m_pDataCallBack->OnConnectionClosed(hSocket);

				break;
			}

			m_dwRDataBytes += nReadDataSize;
			//分析数据
			if (m_dwDataSize == m_dwRDataBytes)
			{
	#ifdef _WIN32
				InterlockedExchange(&nStopFlag, m_nStopFlag);
	#else
				nStopFlag = GetCurrentStopFlag();
	#endif
				if (nStopFlag > 0)
				{
					CloseConnection();
					break;
				}

				m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
					m_pPackData, m_dwDataSize, m_szSvrAddr, m_nSvrPort, wPduType, 0);

				//delete []m_pPackData; //modify by zjj 2009-5-25
				//m_pPackData = NULL;
				m_bHeadFlag = TRUE;
				m_dwRHeadBytes = 0;
				m_dwRDataBytes = 0;
				m_dwDataSize = 0;
			}
		}
//end begin modify  by zjj 2009-5-25	
	}
	
	if (m_pPackHead != NULL)
	{
		delete []m_pPackHead;
		m_pPackHead = NULL;
	}

	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
	
	//Star del: when break we will always call CloseConnection
	//m_pSocket->Close();
	delete m_pSocket;
	m_pSocket = NULL;
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}

//modified by zjj 2009-5-25
/*
int CACTCPClientProc::SessionMainLoop(void)
{
	//add by chengwei 2004.12.15
	//SOCKET hSocket=INVALID_SOCKET;
	SOCKET hSocket = m_pSocket->GetSocket();
	fd_set ReadFDs;
	int nPkgHeadSize = 0;
	WORD wExtParam = 0;
	WORD wPduType = 0;
	int nReadDataSize = 0;
	long nStopFlag = 0;

	//获取数据包头长度

	nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
	m_bHeadFlag = TRUE;
	m_pPackHead = new BYTE[nPkgHeadSize];

	m_pPackData = NULL;
	m_dwRHeadBytes = 0;
	m_dwRDataBytes = 0;
	m_dwDataSize = 0;

	//::InterlockedIncrement(&g_nThreadCount);
	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		nStopFlag = GetCurrentStopFlag();
#endif
		//正在关闭连接
		if (nStopFlag > 0)
		{
			CloseConnection();
			break;
		}

		FD_ZERO(&ReadFDs);
		FD_SET(m_pSocket->GetSocket(), &ReadFDs);
		
#ifdef _WIN32
		int nRet = m_pSocket->Select(0, &ReadFDs, ACCEPT_TIMEOUT_SET);
#else
		int maxfd = m_pSocket->GetSocket() + 1;
		int nRet = m_pSocket->Select(maxfd, &ReadFDs, ACCEPT_TIMEOUT_SET);
#endif
		if (nRet <= 0)
		{
#ifdef _WIN32
			Sleep(30);
#else
			usleep(30000);
#endif
			continue;
		}


		nReadDataSize = 0;
		//应该收数据包头
		if(m_bHeadFlag)
		{
			//接收数据
			nReadDataSize = m_pSocket->Receive((char*)m_pPackHead+m_dwRHeadBytes, 
				nPkgHeadSize-m_dwRHeadBytes);
			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(%d bytes)", nReadDataSize);

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nReadDataSize <= 0)
			{
				//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error SessionMainLoop1");
				CloseConnection();
				if (nStopFlag == 0)
				{
					m_pDataCallBack->OnConnectionClosed(hSocket);
				}
				break;
			}
			m_dwRHeadBytes += nReadDataSize;

			//未收到完整包头
			if (m_dwRHeadBytes < nPkgHeadSize)
			{
				continue;
			}
			
			//解开头，获取数据长度
			wExtParam = 0;
			wPduType = 0;

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				CloseConnection();
				break;
			}

			m_dwDataSize = m_pHeadProc->GetAndParsePkgHead(m_pPackHead, 
				&wPduType, &wExtParam);
			//add by kent for check headdata
			if(m_dwDataSize < 0)
			{
				assert("收到数据有误!");
				CloseConnection();
				break;
			}
			//add by end

			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(pdu:%d, len:%d)", 
			//	wPduType, m_dwDataSize);

			//数据体为空
			if (m_dwDataSize == 0)
			{
#ifdef _WIN32
				InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
				nStopFlag = GetCurrentStopFlag();
#endif
				if (nStopFlag > 0)
				{
					CloseConnection();
					break;
				}
				m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
					NULL, 0, m_szSvrAddr, m_nSvrPort, wPduType, 0);

				m_bHeadFlag = TRUE;
				m_dwRHeadBytes = 0;
			}
			else
			{
				m_pPackData = new BYTE[m_dwDataSize];
				m_dwRDataBytes = 0;
				m_bHeadFlag = FALSE;
			}

			continue;
		}

		//包头已接收，接收数据
		nReadDataSize = m_pSocket->Receive((char*)m_pPackData+m_dwRDataBytes, 
			m_dwDataSize-m_dwRDataBytes);
		//::LogTrace(__FILE__, __LINE__, "server.txt", "receive body(%d bytes)", 
		//	nReadDataSize);
		
		if (nReadDataSize <= 0)
		{
			LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error SessionMainLoop2");
			CloseConnection();
			m_pDataCallBack->OnConnectionClosed(hSocket);

			break;
		}

		m_dwRDataBytes += nReadDataSize;
		//分析数据
		if (m_dwDataSize == m_dwRDataBytes)
		{
#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				CloseConnection();
				break;
			}

			m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
				m_pPackData, m_dwDataSize, m_szSvrAddr, m_nSvrPort, wPduType, 0);

			delete []m_pPackData;
			m_pPackData = NULL;
			m_bHeadFlag = TRUE;
			m_dwRHeadBytes = 0;
			m_dwDataSize = 0;
		}
	}
	
	if (m_pPackHead != NULL)
	{
		delete []m_pPackHead;
		m_pPackHead = NULL;
	}

	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
	
	//Star del: when break we will always call CloseConnection
	//m_pSocket->Close();
	delete m_pSocket;
	m_pSocket = NULL;
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}
*/

int CACTCPClientProc::HttpSessionMainLoop(void)
{
	//SOCKET hSocket=INVALID_SOCKET;
	SOCKET hSocket = m_pSocket->GetSocket();
	fd_set ReadFDs;
	int dwHttpHeadLen = 0;
	int nPkgHeadSize = 0;
	WORD wExtParam = 0;
	WORD wPduType = 0;
	int nReadDataSize = 0;
	long nStopFlag = 0;
	BYTE byHttpBuf[1024];

	//获取数据包头长度
	//dwHttpHeadLen = GetHttpHeadLength();
	nPkgHeadSize = m_pHeadProc->GetPkgHeadSize();
	m_bHeadFlag = TRUE;
	m_pPackHead = new BYTE[nPkgHeadSize];//PDU头
	m_pPackData = NULL;
	m_dwRHeadBytes = 0;
	m_dwRDataBytes = 0;
	m_dwDataSize = 0;
	char szValue[30];

	//::InterlockedIncrement(&g_nThreadCount);
	while (1)
	{
#ifdef _WIN32
		InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
		nStopFlag = GetCurrentStopFlag();
#endif
		//正在关闭连接
		if (nStopFlag > 0)
		{
			CloseConnection();
			break;
		}

		FD_ZERO(&ReadFDs);
		FD_SET(m_pSocket->GetSocket(), &ReadFDs);
		
#ifdef _WIN32
		int nRet = m_pSocket->Select(0, &ReadFDs, ACCEPT_TIMEOUT_SET);
#else
		int maxfd = m_pSocket->GetSocket() + 1;
		int nRet = m_pSocket->Select(maxfd, &ReadFDs, ACCEPT_TIMEOUT_SET);
#endif
		if (nRet <= 0)
		{
#ifdef _WIN32
			Sleep(30);
#else
			usleep(30000);
#endif
			continue;
		}


		nReadDataSize = 0;
		//应该收数据包头
		if(m_bHeadFlag)
		{
			//先收HTTP包头
			memset(byHttpBuf,0,1024);
			int nRet = m_pSocket->ReceiveHttp((char *)byHttpBuf,dwHttpHeadLen);
			if(nRet <= 0)
			{
				LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv HttpHead Socket Error HttpSessionMainLoop1");
				CloseConnection();

#ifdef _WIN32
				InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
				nStopFlag = GetCurrentStopFlag();
#endif
				if (nStopFlag == 0)
				{
					m_pDataCallBack->OnConnectionClosed(hSocket);
				}
				break;
			}

			//增加检查HTTP错误校验
			nRet=m_pSocket->GetField((char *)byHttpBuf,"Content-Length",szValue,30);
			if(nRet == -1)
			{
				continue;
			}
			if(atoi(szValue) < 16)
			{
				continue;
			}

			while(m_dwRHeadBytes < nPkgHeadSize)
			{
				//接收数据Head
				nReadDataSize = m_pSocket->Receive((char*)m_pPackHead+m_dwRHeadBytes, 
					nPkgHeadSize-m_dwRHeadBytes);
				//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(%d bytes)", 
				//	nReadDataSize);

				if (nReadDataSize <= 0)
				{
					LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error HttpSessionMainLoop1");
					CloseConnection();

#ifdef _WIN32
					InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
					nStopFlag = GetCurrentStopFlag();
#endif
					if (nStopFlag == 0)
					{
						m_pDataCallBack->OnConnectionClosed(hSocket);
					}
					break;
				}
				m_dwRHeadBytes += nReadDataSize;
			}
			
			//解开头，获取数据长度
			wExtParam = 0;
			wPduType = 0;

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				CloseConnection();
				break;
			}
			
			m_dwDataSize = m_pHeadProc->GetAndParsePkgHead(m_pPackHead, 
				&wPduType, &wExtParam);
			//add by kent for check headdata
			if(m_dwDataSize < 0)
			{
				/*
				assert("收到数据有误!");
				CloseConnection();
				break;
				*/
				if (m_pPackData)
				{
					delete []m_pPackData;
					m_pPackData = NULL;
				}
				m_bHeadFlag = TRUE;
				m_dwRHeadBytes = 0;
				m_dwDataSize = 0;

				continue;
			}
			//add by end
			//::LogTrace(__FILE__, __LINE__, "server.txt", "receive head(pdu:%d, len:%d)", 
			//	wPduType, m_dwDataSize);

			//数据体为空
			if (m_dwDataSize == 0)
			{
#ifdef _WIN32
				InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
				nStopFlag = GetCurrentStopFlag();
#endif
				if (nStopFlag > 0)
				{
					CloseConnection();
					break;
				}
				m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
					NULL, 0, m_szSvrAddr, m_nSvrPort, wPduType, 0);

				m_bHeadFlag = TRUE;
				m_dwRHeadBytes = 0;
			}
			else
			{
				m_pPackData = new BYTE[m_dwDataSize];
				memset(m_pPackData,0,m_dwDataSize);

				m_dwRDataBytes = 0;
				m_bHeadFlag = FALSE;
			}

			continue;
		}

		//包头已接收，接收数据
		nReadDataSize = m_pSocket->Receive((char*)m_pPackData+m_dwRDataBytes, 
			m_dwDataSize-m_dwRDataBytes);
		//::LogTrace(__FILE__, __LINE__, "server.txt", "receive body(%d bytes)", 
		//	nReadDataSize);
		
		if (nReadDataSize <= 0)
		{
			LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error HttpSessionMainLoop2");
			CloseConnection();

#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag == 0)
			{
				m_pDataCallBack->OnConnectionClosed(hSocket);
			}

			break;
		}

		m_dwRDataBytes += nReadDataSize;
		//分析数据
		if (m_dwDataSize == m_dwRDataBytes)
		{
#ifdef _WIN32
			InterlockedExchange(&nStopFlag, m_nStopFlag);
#else
			nStopFlag = GetCurrentStopFlag();
#endif
			if (nStopFlag > 0)
			{
				CloseConnection();
				break;
			}

			m_pDataCallBack->OnDataReceived(SOCK_STREAM, m_pSocket->GetSocket(), 
				m_pPackData, m_dwDataSize, m_szSvrAddr, m_nSvrPort, wPduType, 0);

			delete []m_pPackData;
			m_pPackData = NULL;
			m_bHeadFlag = TRUE;
			m_dwRHeadBytes = 0;
			m_dwDataSize = 0;
		}
	}
	
	if (m_pPackHead != NULL)
	{
		delete []m_pPackHead;
		m_pPackHead = NULL;
	}

	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
	
	//Star del: when break we will always call CloseConnection
	//m_pSocket->Close();
	delete m_pSocket;
	m_pSocket = NULL;
	//::InterlockedDecrement(&g_nThreadCount);
	return 0;
}

int CACTCPClientProc::StartMainProc(void)
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

#ifdef _WIN32
DWORD WINAPI CACTCPClientProc::RecvThreadHelper(LPVOID lpParameter)
#else
void* CACTCPClientProc::RecvThreadHelper(LPVOID lpParameter)
#endif
{
	CACTCPClientProc * pSession = (CACTCPClientProc *)lpParameter;

	if (pSession->m_bHttpFlag == FALSE)
	{
		pSession->SessionMainLoop();
	}
	else
	{
		pSession->HttpSessionMainLoop();
	}
	return 0;
}

int CACTCPClientProc::ConnectBySock5(LPCTSTR lpDestHost, USHORT nDestPort, 
	LPCTSTR lpProxyHost, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword, ULONG lTimeOut)
{	
	char sendBuf[300];
	char recvBuf[300] = {0};
	DWORD dwPos = 0;
	DWORD dwSendLen = 0;
	DWORD dwRecvLen = 0;
	int nRetCode = 0;
	ULONG nDestHost = 0;
	
	//连接到代理服务器
	if (m_pSocket->Connect(lpProxyHost, nProxyPort) != 0)
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
		&dwRecvLen, lTimeOut);
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
			&dwRecvLen, lTimeOut);
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

	sendBuf[dwPos] = 0x01; //Connect
	dwPos += 1;
	dwSendLen += 1;

	sendBuf[dwPos] = 0x00;
	dwPos += 1;
	dwSendLen += 1;

	sendBuf[dwPos] = 0x01; //ipv4
	dwPos += 1;
	dwSendLen += 1;

	nDestHost = ::inet_addr(lpDestHost);	
	memcpy(&sendBuf[dwPos], &nDestHost, 4);
	dwPos += 4;
	dwSendLen += 4;
	nDestPort = htons(nDestPort);
	memcpy(&sendBuf[dwPos], &nDestPort, 2);
	dwPos += 2;
	dwSendLen += 2;

	dwRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	nRetCode = ConsultProcess((BYTE*)sendBuf, dwSendLen, (BYTE*)recvBuf, 
		&dwRecvLen, lTimeOut);
	if (nRetCode != 0)
	{
		return -1;
	}

	if (recvBuf[0]!=0x05 || recvBuf[1]!=0x00)
	{
		return -1;
	}
	return 0;;
}

int CACTCPClientProc::ConnectByHttp(LPCTSTR lpDestHost, USHORT nDestPort, 
	LPCTSTR lpProxyHost, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword, ULONG lTimeOut)
{	
	char sendBuf[300];
	char recvBuf[300] = {0};	
	DWORD dwSendLen = 0;
	DWORD dwRecvLen = 0;
	
	assert(lpDestHost != NULL);
	assert(lpProxyHost != 0);
	assert(lpProxyUserName != NULL);	
		
	//连接到代理服务器
	if (m_pSocket->Connect(lpProxyHost, nProxyPort) != 0)
	{
		return -1;
	}

	//协商认证方法
	memset(sendBuf, 0, sizeof(sendBuf));
	
	if (NULL!=lpProxyUserName && *lpProxyUserName!=0)
	{
		assert(lpProxyPassword!=NULL && *lpProxyPassword!=0);	
		char userpass[200]={0};
		sprintf(userpass,"%s:%s", lpProxyUserName, lpProxyPassword);
	    char *result =  make_base64_string(userpass);	
		sprintf(sendBuf, "CONNECT %s:%d HTTP/1.1\r\nProxy-Authorization: Basic %s\r\nAccept: */*\r\nContent-Type: text/html\r\nProxy-Connection: Keep-Alive\r\nContent-length: 0\r\n\r\n",
			lpDestHost, nDestPort, result);
		delete []result;
	}
	else
	{
		sprintf(sendBuf, "CONNECT %s:%d HTTP/1.1\r\nAccept: */*\r\nContent-Type: text/html\r\nProxy-Connection: Keep-Alive\r\nContent-length: 0\r\n\r\n", lpDestHost, nDestPort);  
	}

	dwSendLen = (int)strlen(sendBuf);		
	dwRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	int nRetCode = ConsultProcess((BYTE*)sendBuf, dwSendLen, (BYTE*)recvBuf, 
		&dwRecvLen, lTimeOut);
	if (nRetCode != 0)
	{
		return -1;
	}

	nRetCode = atoi(strchr(recvBuf,' '));
	if(nRetCode != 200)
	{
		return -1;
	}

	return 0;
}

int CACTCPClientProc::ConsultProcess(const BYTE* lpSendBuf, DWORD dwSendLen, 
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
		nRetCode = m_pSocket->Send((char*)(lpSendBuf+dwSendCurPos), dwSendLen-dwSendCurPos);

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
	FD_SET(m_pSocket->GetSocket(), &readFds);
#ifdef _WIN32
	nRetCode = m_pSocket->Select(0, &readFds, nTimeOut);
#else
	int maxfd = m_pSocket->GetSocket() + 1;
	nRetCode = m_pSocket->Select(maxfd, &readFds, nTimeOut);
#endif
	if (nRetCode <= 0)
	{
		return -1;
	}

	nRetCode = m_pSocket->Receive((char*)lpRecvBuf, *lpRecvLen);
	if (nRetCode <= 0)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error ConsultProcess");
		return -1;
	}

	*lpRecvLen = nRetCode;
	return 0;
}

const char *base64_table =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* CACTCPClientProc::make_base64_string(const char *str)
{
    static char *buf;
    unsigned char *src;
    char *dst;
    int bits, data;
	size_t src_len, dst_len;
    /* make base64 string */
    src_len = strlen(str);
    dst_len = (src_len+2)/3*4;
    buf = new char[dst_len+1];
    bits = data = 0;
    src = (unsigned char *)str;
    dst = buf;
    while ( dst_len-- ) {
	if ( bits < 6 ) {
	    data = (data << 8) | *src;
	    bits += 8;
	    if ( *src != 0 )
		src++;
	}
	*dst++ = base64_table[0x3F & (data >> (bits-6))];
	bits -= 6;
    }
    *dst = '\0';
    /* fix-up tail padding */
    switch ( src_len%3 ) {
    case 1:
	*--dst = '=';
    case 2:
	*--dst = '=';
    }
    return buf;
}
