//#include "stdafx.h"
#include "CommFunction.h"

int CCommFunction::ConsultProcess(SOCKS5_SOCKET sock, const char* pSendBuf, 
	int nSendLen, char *pRecvBuf, int* pRecvLen, int nTimeOut)
{
	int nRetCode = 0;

	assert(pSendBuf!=NULL && nSendLen>0);
	assert(pRecvBuf!=NULL && pRecvLen!=NULL && *pRecvLen>0);

	nRetCode = send(sock, pSendBuf, nSendLen, 0);
	if (SOCKS5_ERROR == nRetCode)
	{
		return -1;
	}

	nRetCode = RecvWait(sock, nTimeOut);
	if (nRetCode <= 0)
	{
		return -1;
	}

	nRetCode = recv(sock, pRecvBuf, *pRecvLen, 0);
	if (nRetCode <= 0)
	{
		int nError = SOCKS5_ERRNO();	
		return -1;
	}
	*pRecvLen = nRetCode;
	return 0;
}

//功能：等待接收数据
//返回值:

int CCommFunction::RecvWait(SOCKS5_SOCKET sock, //接收数据的套接字
			 unsigned int nTimeOut) //超时时间(毫秒)
{
	fd_set recvFds;
	struct timeval tvTimeOut;

	assert(sock != INVALID_SOCKS5);

	FD_ZERO(&recvFds);
	FD_SET(sock, &recvFds);

	tvTimeOut.tv_sec = nTimeOut/1000;
	tvTimeOut.tv_usec = (nTimeOut%1000)*1000;

#ifdef _WIN32
	return select(0, &recvFds, NULL, NULL, &tvTimeOut);
#else
	return select(sock + 1, &recvFds, NULL, NULL, &tvTimeOut);
#endif
}

int CCommFunction::Bind(SOCKS5_SOCKET sock, const char * lpLocalHost, 
	USHORT nLocalPort)
{
	struct sockaddr_in  saLocalAddr;

	assert(sock != INVALID_SOCKS5);
	
	memset(&saLocalAddr, 0, sizeof(saLocalAddr));
	saLocalAddr.sin_family = AF_INET;
	saLocalAddr.sin_port = htons(nLocalPort);
	if (lpLocalHost==NULL || strcmp(lpLocalHost,"")==0)
	{
		saLocalAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		saLocalAddr.sin_addr.s_addr = inet_addr(lpLocalHost);
	}

	int nRet = bind(sock, (struct sockaddr*)&saLocalAddr, 
		sizeof(saLocalAddr));

	return nRet;
}

int CCommFunction::Bind(SOCKS5_SOCKET sock, unsigned long wLocalHost, 
	USHORT nLocalPort)
{
	struct sockaddr_in  saLocalAddr;

	assert(sock != INVALID_SOCKS5);
	
	memset(&saLocalAddr, 0, sizeof(saLocalAddr));
	saLocalAddr.sin_family = AF_INET;
	saLocalAddr.sin_port = htons(nLocalPort);
	if (wLocalHost ==0)
	{
		saLocalAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		saLocalAddr.sin_addr.s_addr = htonl(wLocalHost);
	}

	int nRet = bind(sock, (struct sockaddr*)&saLocalAddr, 
		sizeof(saLocalAddr));

	return nRet;
}

int CCommFunction::LToStrIp(unsigned long lIp,char *StrIp)
{
	assert(StrIp != NULL);
	in_addr inDestip ;
    char *pDestip = NULL;	
	memset(&inDestip,0,sizeof(inDestip));
	#ifdef _WIN32
	inDestip.S_un.S_addr = htonl(lIp);
	#else
	inDestip.s_addr = htonl(lIp);
	#endif
	pDestip = inet_ntoa(inDestip);
	if((pDestip != NULL) &&(strlen(pDestip) < 16))
	{
		strcpy(StrIp,pDestip);
	}
	return (0);
}
