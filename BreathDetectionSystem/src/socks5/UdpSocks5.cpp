/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        UdpSocks5.cpp
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#include "UdpSocks5.h"


CUdpSocks5::CUdpSocks5()
{
	m_hSocket = INVALID_SOCKS5;
	m_TcpSocks5 = INVALID_SOCKS5;	
	m_ProxyBindPort = 0;
	memset(m_strProxyIp,0,sizeof(m_strProxyIp));
}

CUdpSocks5::~CUdpSocks5()
{
	Close();
}

SOCKS5_SOCKET CUdpSocks5::Create(const char * pLocalHost,	unsigned short nLocalPort,				
				const char * pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword) //代理密码
{
	assert(m_hSocket == INVALID_SOCKS5);
	assert(nLocalPort != 0 );
	
	int nErrCode  =0;

	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
		
	if (m_hSocket == INVALID_SOCKS5)
	{
		return m_hSocket;
	}
	if (m_CommFun.Bind(m_hSocket,pLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}

	if(ConnectUdpProxyServer(pLocalHost,nLocalPort,pProxyHost, nProxyPort,
		 pProxyUserName, pProxyPassword, 2000, &nErrCode) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;
}

SOCKS5_SOCKET CUdpSocks5:: Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wProxyHost, //代理服务器IP地址
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword) //代理密码
{

	assert(m_hSocket == INVALID_SOCKS5);
	assert(nLocalPort != 0 );
	assert(wProxyHost != 0 );
	assert(nProxyPort != 0);
	
	int nErrCode  =0;

	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
		
	if (m_hSocket == INVALID_SOCKS5)
	{
		return m_hSocket;
	}
	if (m_CommFun.Bind(m_hSocket, wLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}
	m_TcpSocks5 = ConnectUdpProxyServer(wLocalHost,nLocalPort,wProxyHost, nProxyPort,
		 pProxyUserName, pProxyPassword, 2000, &nErrCode);
	if(m_TcpSocks5 == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;

}


void CUdpSocks5::Close()
{
	if (m_hSocket != INVALID_SOCKS5)
	{
		SOCKS5_CLOSE(m_hSocket);
		m_hSocket = INVALID_SOCKS5;
	}

	if(m_TcpSocks5 != INVALID_SOCKS5)
	{
		SOCKS5_CLOSE(m_TcpSocks5);
		m_TcpSocks5 = INVALID_SOCKS5;
	}
	m_ProxyBindPort = 0;
	memset(m_strProxyIp,0,sizeof(m_strProxyIp));
}

SOCKS5_SOCKET CUdpSocks5::GetSocket()
{
	return m_hSocket;
}

unsigned short CUdpSocks5::GetProxyBindPort()
{
	return m_ProxyBindPort;
}
unsigned short CUdpSocks5::GetPort()
{
	struct sockaddr_in  saLocalAddr;
	//socklen_t len = 0;
	#ifdef _WIN32
	int len = 0;
	#else
	socklen_t len = 0;
	#endif
	memset(&saLocalAddr,0,sizeof(saLocalAddr));
	len = sizeof(saLocalAddr);
    if (getsockname(m_hSocket,(struct sockaddr *)&saLocalAddr,&len) < 0)
	{
		return 0;
	}
    return (ntohs(saLocalAddr.sin_port));
}

void CUdpSocks5::SetSocket(SOCKS5_SOCKET sock)
{
	m_hSocket = sock;
}

SOCKS5_SOCKET CUdpSocks5::ConnectUdpProxyServer(
				  const char * pLocalHost, unsigned short nLocalPort,
				  const char * pProxyHost, //代理服务器IP地址
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码
				  )
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(nLocalPort != 0 );
	assert(pProxyHost != NULL );
	assert(nProxyPort != 0);

	unsigned long wLocalHost = 0,wProxyHost = 0;
	if ((pLocalHost != NULL) || (strcmp(pLocalHost,"") != 0))
	{
		wLocalHost = inet_addr(pLocalHost);
		if(wLocalHost == INADDR_NONE)
		{
			perror("error pLocalHost ");
			return(INVALID_SOCKS5);
		}
	}
	wLocalHost = ntohl(wLocalHost);

	wProxyHost = inet_addr(pProxyHost);
	if(wProxyHost == INADDR_NONE)
	{
		perror("error pProxyHost");
		return(INVALID_SOCKS5);
	}
	wProxyHost = ntohl(wProxyHost);

	m_TcpSocks5 = ConnectUdpProxyServer(wLocalHost,nLocalPort,wProxyHost, nProxyPort,
		 pProxyUserName, pProxyPassword, 2000, pErrCode);
	return(m_TcpSocks5);
}


SOCKS5_SOCKET CUdpSocks5::ConnectUdpProxyServer(
				  unsigned long wLocalHost, unsigned short nLocalPort,	
				  unsigned long wProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码
				  )
{
	SOCKS5_SOCKET sockClient = INVALID_SOCKS5;
	int nRetCode = SOCKS5_ERROR;
//	sockaddr_in destAddr;
	sockaddr_in proxyAddr;
	char sendBuf[300];
	char recvBuf[300] = {0};
	int nPos = 0;
	int nSendLen = 0;
	int nRecvLen = 0;
	unsigned long lTempIp = 0;

	assert(nLocalPort != 0);
	assert(wProxyHost != 0);
	assert(nProxyPort != 0);
	assert(pErrCode != NULL);
	

	//创建连接套接字
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKS5 == sockClient)
	{
		*pErrCode = -1;
		return sockClient;
	}

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


	sendBuf[nPos] = 0x03; //udp
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x00;
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x01; //ipv4
	nPos += 1;
	nSendLen += 1;

	wLocalHost = htonl(wLocalHost);	
	memcpy(&sendBuf[nPos], &wLocalHost, 4);
	nPos += 4;
	nSendLen += 4;

	nLocalPort = htons(nLocalPort);
	memcpy(&sendBuf[nPos], &nLocalPort, 2);
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

	unsigned short ntempPort = 0;
	in_addr inDestip ;
	char *pDestip = NULL;
	
	lTempIp = 0;
	memcpy(&lTempIp,&recvBuf[4],4);
	memcpy(&ntempPort,&recvBuf[8],2);
    memset(&inDestip,0,sizeof(inDestip));
	#ifdef _WIN32
	inDestip.S_un.S_addr = lTempIp;
	#else
	inDestip.s_addr = lTempIp;
	#endif
	pDestip = inet_ntoa(inDestip);
	ntempPort = ntohs(ntempPort);

	m_ProxyBindPort = ntempPort;
	if((pDestip != NULL) &&(strlen(pDestip) < 16))
	{
		strcpy(m_strProxyIp,pDestip);
		return (sockClient);
	}	
	else
	{
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}	
}

int CUdpSocks5::BuildUdpSocket5Head(char * outbuf,unsigned long wRemoteHost,unsigned short nRemotePort)
{
	char sendBuf[300] = {0};
	int nPos = 0;
	int nSendLen = 0;
	unsigned long lTempIp = 0;
	unsigned short port = 0;
	sendBuf[nPos] = 0x00;//保留2字节的0
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x00;//保留2字节的0
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x00; //是否数据报分段重组标志
	nPos += 1;
	nSendLen += 1;

	sendBuf[nPos] = 0x01; //Ipv4
	nPos += 1;
	nSendLen += 1;
	
	lTempIp = htonl(wRemoteHost);	
	memcpy(&sendBuf[nPos], &lTempIp, 4);
	nPos += 4;
	nSendLen += 4;
	
	port = htons(nRemotePort);
	memcpy(&sendBuf[nPos], &port, 2);
	nPos += 2;
	nSendLen += 2;

	memcpy(outbuf,sendBuf,nSendLen);
	return (nSendLen);
}

int CUdpSocks5::Sendto(const char * lpBuf, unsigned long wSize,  unsigned long  wRemoteHost,
					  unsigned short nRemotePort,int flags)
{
	struct sockaddr_in saRemoteAddr;

	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);
	assert(wRemoteHost > 0);
	assert(nRemotePort > 0);	

	memset(&saRemoteAddr, 0, sizeof(saRemoteAddr));
	saRemoteAddr.sin_family=AF_INET;
	saRemoteAddr.sin_port=htons((u_short )m_ProxyBindPort);
	saRemoteAddr.sin_addr.s_addr=inet_addr(m_strProxyIp);
	if(saRemoteAddr.sin_addr.s_addr == INADDR_NONE)
	{
		perror("error SendTo m_strProxyIp ");
		return(INVALID_SOCKS5);
	}
		
	char* lpDataSend = NULL;
	int nRetLen = 0;
	char UdpHead[256];
				
	memset(UdpHead,0,sizeof(UdpHead));
	nRetLen = BuildUdpSocket5Head(UdpHead,wRemoteHost, nRemotePort);		
	
	lpDataSend	= new char[wSize + nRetLen];
	memset(lpDataSend, 0, wSize + nRetLen);	
	
	memcpy(lpDataSend , UdpHead, nRetLen);						
	memcpy(lpDataSend + nRetLen, lpBuf, wSize);		
	
	int nRet = sendto(m_hSocket, lpDataSend, wSize + nRetLen, flags,				  
		(struct sockaddr*)&saRemoteAddr, sizeof(saRemoteAddr));
	
	delete []lpDataSend;
	lpDataSend=NULL;
	if(nRet == wSize + nRetLen)
	{
		return (int)wSize;
	}
	return nRet;
}

int CUdpSocks5::ReceiveFrom(char* lpBuf, unsigned long wSize, unsigned long *pRemoteHost,
						   unsigned short* pRemotePort,int flags)
{
	struct sockaddr_in saRemoteAddr;
	int nBytes = 0;
	#ifdef _WIN32
	int nAddrLen = sizeof(saRemoteAddr);
	#else
	socklen_t nAddrLen = sizeof(saRemoteAddr);
	#endif

	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);

	memset(&saRemoteAddr, 0, nAddrLen);

	char* dataRecv = NULL;
	dataRecv	= new char[wSize + 10];
	memset(dataRecv, 0, wSize + 10);	

	nBytes = recvfrom(m_hSocket, dataRecv, wSize + 10, flags,
		(struct sockaddr*)&saRemoteAddr, &nAddrLen);
	
	if(nBytes>=10)
	{
		int nRetLen = 0;
		char UdpHead[11];						
		memset(UdpHead,0,sizeof(UdpHead));
		memcpy(UdpHead,dataRecv,10);
				
		unsigned short ntempPort = 0;		
		unsigned long 	lTempIp = 0;

		memcpy(&lTempIp,&UdpHead[4],4);
		memcpy(&ntempPort,&UdpHead[8],2);

		lTempIp = ntohl(lTempIp);
		ntempPort = ntohs(ntempPort);
			
		if (pRemotePort != NULL)
		{
			*pRemotePort= ntempPort;
		}

		if (pRemoteHost!= NULL)
		{
			*pRemoteHost = lTempIp;
		}
		memcpy(lpBuf,dataRecv + 10,nBytes - 10);
		delete []dataRecv;
		return (nBytes - 10);
	}
	delete []dataRecv;
	return nBytes;
}


