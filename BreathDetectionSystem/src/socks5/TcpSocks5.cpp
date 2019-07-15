/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        TcpSocks5.cpp
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#include "TcpSocks5.h"


CTcpSocks5::CTcpSocks5()
{
	m_hSocket = INVALID_SOCKS5;
}

CTcpSocks5::~CTcpSocks5()
{
	Close();
}

SOCKS5_SOCKET CTcpSocks5::Create(const char * pLocalHost,	unsigned short nLocalPort,
				const char * pRemoteHost,unsigned short nRemotePort,
				const char * pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword,//代理密码
				int type)
{
	assert(m_hSocket == INVALID_SOCKS5);
	
	int nErrCode  =0;

	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
		
	if (m_hSocket == INVALID_SOCKS5)
	{
		return m_hSocket;
	}
	/*
	if (m_CommFun.Bind(m_hSocket,pLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}
	*/

	if(ConnectTcpProxyServer(pRemoteHost,nRemotePort, pProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, &nErrCode,type) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;

}

SOCKS5_SOCKET CTcpSocks5::Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wRemoteHost,unsigned short nRemotePort,
				unsigned long wProxyHost, //代理服务器IP地址
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword, //代理密码
				int type)	//connect方式或bind方式
{
	assert(m_hSocket == INVALID_SOCKS5);
	assert(wRemoteHost != 0 );
	assert(nRemotePort != 0 );
	assert(wProxyHost != 0 );
	assert(nProxyPort != 0);
	
	int nErrCode  =0;

	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
		
	if (m_hSocket == INVALID_SOCKS5)
	{
		return m_hSocket;
	}
	/*
	if (m_CommFun.Bind(m_hSocket,wLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}
	*/
	if(ConnectTcpProxyServer(wRemoteHost,nRemotePort, wProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, &nErrCode,type) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;
}


void CTcpSocks5::Close()
{
	if (m_hSocket != INVALID_SOCKS5)
	{
		SOCKS5_CLOSE(m_hSocket);
		m_hSocket = INVALID_SOCKS5;
	}
}

SOCKS5_SOCKET CTcpSocks5::GetSocket()
{
	return m_hSocket;
}

void CTcpSocks5::SetSocket(SOCKS5_SOCKET sock)
{
	m_hSocket = sock;
}

int CTcpSocks5::Send(const char * lpBuf, unsigned long wSize,int flags)
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);
	return send(m_hSocket, lpBuf, wSize, flags);
}

int CTcpSocks5::Receive(char* lpBuf, unsigned long wSize,int flags)
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);
	
	return recv(m_hSocket, lpBuf, wSize, flags);
}


SOCKS5_SOCKET CTcpSocks5::ConnectTcpProxyServer(const char *pDestHost,  //目标服务器IP地址 
				  unsigned short nDestPort, //目标服务器端口
				  const char *pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode, //错误代码
				  int type
				  )				 
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(pDestHost != NULL );
	assert(nDestPort != 0 );
	assert(pProxyHost != NULL );
	assert(nProxyPort != 0);

	unsigned long wRemoteHost = 0,wProxyHost = 0;	
	wRemoteHost = inet_addr(pDestHost);
	if(wRemoteHost == INADDR_NONE)
	{
		perror("error pDestHost");
		return(INVALID_SOCKS5);
	}
	wRemoteHost = ntohl(wRemoteHost);
	wProxyHost = inet_addr(pProxyHost);
	if(wProxyHost == INADDR_NONE)
	{
		perror("error pProxyHost");
		return(INVALID_SOCKS5);
	}
	wProxyHost = ntohl(wProxyHost);

	return (ConnectTcpProxyServer(wRemoteHost,nDestPort, wProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, pErrCode,type));
}

//功能：连接服务器(直接连接或通过SOCKS5代理连接)
//返回值：
SOCKS5_SOCKET CTcpSocks5::ConnectTcpProxyServer(unsigned long wDestHost,  //目标服务器IP地址 
				  unsigned short nDestPort, //目标服务器端口
				  unsigned long wProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode, //错误代码
				  int type
				  )				 
{
	SOCKS5_SOCKET sockClient = INVALID_SOCKS5;
//	sockClient = m_pSocketMainProc->GetSocket();
	sockClient = m_hSocket;
	
	int nRetCode = SOCKS5_ERROR;
//	sockaddr_in destAddr;
	sockaddr_in proxyAddr;
	char sendBuf[300];
	char recvBuf[300] = {0};
	int nPos = 0;
	int nSendLen = 0;
	int nRecvLen = 0;
	
	assert(pErrCode != NULL);
	assert(wDestHost != 0);
	assert(wProxyHost != 0);
	assert(pProxyUserName != NULL);	

	//有代理服务器，则先连接到代理服务器
	memset(&proxyAddr, 0, sizeof(proxyAddr));
	proxyAddr.sin_family = AF_INET;
	#ifdef _WIN32
	proxyAddr.sin_addr.S_un.S_addr = htonl(wProxyHost);
	#else
	proxyAddr.sin_addr.s_addr = htonl(wProxyHost);
	#endif
	proxyAddr.sin_port = htons(nProxyPort);

	nRetCode = connect(sockClient, (sockaddr*)&proxyAddr, sizeof(proxyAddr));
	if (SOCKS5_ERROR == nRetCode)
	{
		*pErrCode = -1;
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}

	//协商认证方法
	memset(sendBuf, 0, sizeof(sendBuf));
	sendBuf[0] = 0x05;//代理版本号
	sendBuf[1] = 0x01; //认证方法数
	if (NULL!=pProxyUserName && *pProxyUserName!=0)
	{
	    assert(pProxyPassword!=NULL && *pProxyPassword!=0);
		sendBuf[2] = 0x02;//认证方法(通过用户名/密码认证)		
	}
	else
	{
		sendBuf[2] = 0x00;//认证方法(无验证需求)
	}	
	nSendLen = 3;	
	nRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	nRetCode = m_CommFun.ConsultProcess(sockClient, sendBuf, nSendLen, recvBuf, 
		&nRecvLen, nTimeOut);
	if (nRetCode != 0)
	{
		*pErrCode = -1;
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}

	if (recvBuf[0]!=0x05 || recvBuf[1]!=sendBuf[2])
	{
		*pErrCode = -1;
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}

	if (NULL!=pProxyUserName && *pProxyUserName!=0)
	{
		//认证用户名/密码
		memset(sendBuf, 0, sizeof(sendBuf));
		nPos = 0;
		nSendLen = 0;
		sendBuf[nPos] = 0x01; //认证版本号
		nPos += 1;
		nSendLen += 1;

		sendBuf[nPos] = (int)strlen(pProxyUserName); //用户名长度
		nPos += 1;
		nSendLen += 1;

		memcpy(&sendBuf[nPos], pProxyUserName, strlen(pProxyUserName));//用户名
		nPos += (int)strlen(pProxyUserName);
		nSendLen += (int)strlen(pProxyUserName);

		sendBuf[nPos] = (int)strlen(pProxyPassword);//密码长度
		nPos += 1;
		nSendLen += 1;

		memcpy(&sendBuf[nPos], pProxyPassword, strlen(pProxyPassword));//密码
		nPos += (int)strlen(pProxyPassword);
		nSendLen += (int)strlen(pProxyPassword);

		nRecvLen = sizeof(recvBuf);
		memset(recvBuf, 0, sizeof(recvBuf));
		nRetCode = m_CommFun.ConsultProcess(sockClient, sendBuf, nSendLen, recvBuf, 
			&nRecvLen, nTimeOut);
		if (nRetCode != 0)
		{
			*pErrCode = -1;
			SOCKS5_CLOSE(sockClient);
			return INVALID_SOCKS5;
		}

		if (recvBuf[0]!=0x01 || recvBuf[1]!=0x00)
		{
			*pErrCode = -1;
			SOCKS5_CLOSE(sockClient);
			return INVALID_SOCKS5;
		}
	}

	//连接目标服务器(通过代理)
	memset(sendBuf, 0, sizeof(sendBuf));
	nPos = 0;
	nSendLen = 0;

	sendBuf[nPos] = 0x05;
	nPos += 1;
	nSendLen += 1;

	if(type == SOCKS5_CLIENT)
	{
		sendBuf[nPos] = 0x01; //Connect
	}	
	else
	{
		sendBuf[nPos] = 0x02; //BIND
	}
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x00;
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x01; //ipv4
	nPos += 1;
	nSendLen += 1;


	wDestHost = htonl(wDestHost);	
	memcpy(&sendBuf[nPos], &wDestHost, 4);
	nPos += 4;
	nSendLen += 4;
	nDestPort = htons(nDestPort);
	memcpy(&sendBuf[nPos], &nDestPort, 2);
	nPos += 2;
	nSendLen += 2;

	nRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	nRetCode = m_CommFun.ConsultProcess(sockClient, sendBuf, nSendLen, recvBuf, 
		&nRecvLen, nTimeOut);
	if (nRetCode != 0)
	{
		*pErrCode = -1;
		int nError = SOCKS5_ERRNO();	
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}
	if (recvBuf[0]!=0x05 || recvBuf[1]!=0x00)
	{
		*pErrCode = -1;
		int nError = SOCKS5_ERRNO();	
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}
	return sockClient;
}
