//#include "stdafx.h"
#include "HttpSocks5.h"


CHttpSocks5::CHttpSocks5()
{
	m_hSocket = INVALID_SOCKS5;
}

CHttpSocks5::~CHttpSocks5()
{
	Close();
}


SOCKS5_SOCKET CHttpSocks5::Create(const char * pLocalHost,	USHORT nLocalPort,
				const char * pRemoteHost,USHORT nRemotePort,
				const char * pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword) //代理密码
{
	assert(m_hSocket == INVALID_SOCKS5);
		
	int nErrCode  =0;

	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
		
	if (m_hSocket == INVALID_SOCKS5)
	{
		return m_hSocket;
	}
	if (CCommFunction::Bind(m_hSocket,pLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}
	if(ConnectHttpProxyServer(pRemoteHost,nRemotePort, pProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, &nErrCode) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;

}

SOCKS5_SOCKET CHttpSocks5::Create(unsigned long wLocalHost,	USHORT nLocalPort,
				unsigned long wRemoteHost,USHORT nRemotePort,
				unsigned long wProxyHost, //代理服务器IP地址
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword) //代理密码
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
	if (CCommFunction::Bind(m_hSocket,wLocalHost, nLocalPort) == SOCKS5_ERROR)
	{
		Close();
		return m_hSocket;
	}

	char lpRemoteHost[16] = {0};
	CCommFunction::LToStrIp(wRemoteHost,lpRemoteHost);	

	if(ConnectHttpProxyServer(lpRemoteHost,nRemotePort, wProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, &nErrCode) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;
}


void CHttpSocks5::Close()
{
	if (m_hSocket != INVALID_SOCKS5)
	{
		SOCKS5_CLOSE(m_hSocket);
		m_hSocket = INVALID_SOCKS5;
	}

}

SOCKS5_SOCKET CHttpSocks5::GetSocket()
{
	return m_hSocket;
}

void CHttpSocks5::SetSocket(SOCKS5_SOCKET sock)
{
	m_hSocket = sock;
}

int CHttpSocks5::Send(const char * lpBuf, unsigned long wSize,int flags)
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);
	return send(m_hSocket, lpBuf, wSize, flags);
}

int CHttpSocks5::Receive(char* lpBuf, unsigned long wSize,int flags)
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(lpBuf != NULL);
	assert(wSize > 0);
	
	return recv(m_hSocket, lpBuf, wSize, flags);
}

SOCKS5_SOCKET CHttpSocks5::ConnectHttpProxyServer(const char* pDestHost,  //目标服务器IP地址 
				  USHORT nDestPort, //目标服务器端口
				  const char *pProxyHost, //代理服务器IP地址
				  USHORT nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码				  
				  )
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(pDestHost != NULL );
	assert(nDestPort != 0);
	assert(pProxyHost != NULL );
	assert(nProxyPort != 0);

	unsigned long wProxyHost = 0;
	wProxyHost = inet_addr(pProxyHost);
	wProxyHost = ntohl(wProxyHost);

	return(ConnectHttpProxyServer(pDestHost,nDestPort, wProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, pErrCode) );
}



//功能：连接服务器(直接连接或通过SOCKS5代理连接)
//返回值：
SOCKS5_SOCKET CHttpSocks5::ConnectHttpProxyServer(const char* pDestHost,  //目标服务器IP地址 
				  USHORT nDestPort, //目标服务器端口
				  unsigned long wProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
				  USHORT nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码				 
				  )				 
{
	SOCKS5_SOCKET sockClient = INVALID_SOCKS5;
	//sockClient = m_pSocketMainProc->GetSocket();
	sockClient = m_hSocket;
	
	int nRetCode = SOCKS5_ERROR;
	sockaddr_in proxyAddr;
	char sendBuf[300];
	char recvBuf[300] = {0};
	
	int nSendLen = 0;
	int nRecvLen = 0;
	

	assert(pErrCode != NULL);
	assert(pDestHost != NULL);
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
	
	if (NULL!=pProxyUserName && *pProxyUserName!=0)
	{
		assert(pProxyPassword!=NULL && *pProxyPassword!=0);	
		char userpass[200]={0};
		sprintf(userpass,"%s:%s",pProxyUserName,pProxyPassword);
	    char *result =  0;//make_base64_string(userpass);	
		sprintf(sendBuf,"CONNECT %s:%d HTTP/1.1\r\nProxy-Authorization: Basic %s\r\n\r\n",pDestHost,nDestPort,result);
		delete []result;
	}
	else
	{
		sprintf(sendBuf,"CONNECT %s:%d HTTP/1.1\r\n\r\n",pDestHost,nDestPort);  
	}

	nSendLen = (int)strlen(sendBuf);		
	nRecvLen = sizeof(recvBuf);
	memset(recvBuf, 0, sizeof(recvBuf));
	nRetCode = CCommFunction::ConsultProcess(sockClient, sendBuf, nSendLen, recvBuf, 
		&nRecvLen, nTimeOut);
	if (nRetCode != 0)
	{
		*pErrCode = -1;
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}
	int nRet = atoi(strchr(recvBuf,' '));
	if(nRet != 200)
	{
		*pErrCode = -1;
		SOCKS5_CLOSE(sockClient);
		return INVALID_SOCKS5;
	}

	return sockClient;
}

