/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        HttpSocks5.cpp
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#include "HttpSocks5.h"


CHttpSocks5::CHttpSocks5()
{
	m_hSocket = INVALID_SOCKS5;
}

CHttpSocks5::~CHttpSocks5()
{
	Close();
}


SOCKS5_SOCKET CHttpSocks5::Create(const char * pLocalHost,	unsigned short nLocalPort,
				const char * pRemoteHost,unsigned short nRemotePort,
				const char * pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword) //��������
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
	if(ConnectHttpProxyServer(pRemoteHost,nRemotePort, pProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, &nErrCode) == INVALID_SOCKS5)
	{
		Close();
		return m_hSocket;
	}
	return m_hSocket;

}

SOCKS5_SOCKET CHttpSocks5::Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wRemoteHost,unsigned short nRemotePort,
				unsigned long wProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword) //��������
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

	char lpRemoteHost[16] = {0};
	m_CommFun.LToStrIp(wRemoteHost,lpRemoteHost);	

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

SOCKS5_SOCKET CHttpSocks5::ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  const char *pProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				  
				  )
{
	assert(m_hSocket != INVALID_SOCKS5);
	assert(pDestHost != NULL );
	assert(nDestPort != 0);
	assert(pProxyHost != NULL );
	assert(nProxyPort != 0);

	unsigned long wProxyHost = 0;
	wProxyHost = inet_addr(pProxyHost);
	if(wProxyHost == INADDR_NONE)
	{
		perror("error pProxyHost");
		return(INVALID_SOCKS5);
	}
	wProxyHost = ntohl(wProxyHost);

	return(ConnectHttpProxyServer(pDestHost,nDestPort, wProxyHost, nProxyPort,
				pProxyUserName, pProxyPassword, 2000, pErrCode) );
}



//���ܣ����ӷ�����(ֱ�����ӻ�ͨ��SOCKS5��������)
//����ֵ��
SOCKS5_SOCKET CHttpSocks5::ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  unsigned long wProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				 
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
		
	//�д�����������������ӵ����������
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

	//Э����֤����
	memset(sendBuf, 0, sizeof(sendBuf));
	
	if (NULL!=pProxyUserName && *pProxyUserName!=0)
	{
		assert(pProxyPassword!=NULL && *pProxyPassword!=0);	
		char userpass[200]={0};
		sprintf(userpass,"%s:%s",pProxyUserName,pProxyPassword);
	    char *result =  make_base64_string(userpass);	
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
	nRetCode = m_CommFun.ConsultProcess(sockClient, sendBuf, nSendLen, recvBuf, 
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


const char *base64_table =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *CHttpSocks5::make_base64_string(const char *str)
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


