// ACSocket.cpp: implementation of the CACSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include "stdafx.h"
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <string.h>
#else
#include <sys/ioctl.h>
#include <net/if.h>
#include <string>
#endif

#include <assert.h>
#include <stdio.h>

#include "ACSocket.h"

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);

CACSocket::CACSocket(void)
{
	m_hSocket = INVALID_SOCKET;
	m_bAutoClose = TRUE;
}

void CACSocket::Attach(SOCKET hSocket)
{
	assert(hSocket != INVALID_SOCKET);
	assert(m_hSocket == INVALID_SOCKET);

	m_hSocket = hSocket;
	m_bAutoClose = TRUE;
}

CACSocket::~CACSocket()
{
	if (m_bAutoClose == TRUE)
	{
		Close();
	}
}

SOCKET CACSocket::Create(short nType, LPCTSTR lpLocalHost, USHORT nLocalPort,BOOL bClientFlag)
{
	int nSendBufSize = 0;
	int nRecvBufSize = 0;
	BOOL bReUseAddr = FALSE;

	assert(nType==SOCK_STREAM || nType==SOCK_DGRAM);
	assert(m_hSocket == INVALID_SOCKET);

	m_hSocket = socket(AF_INET, nType, 0);
	
	if (m_hSocket == INVALID_SOCKET)
	{
		return m_hSocket;
	}
	nSendBufSize = TCPSOCKET_BUF_SIZE;
	nRecvBufSize = TCPSOCKET_BUF_SIZE;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBufSize, 
		sizeof(int)) == SOCKET_ERROR)
	{
		Close();
		return m_hSocket;
	}
	
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&nSendBufSize, 
		sizeof(int)) == SOCKET_ERROR)
	{
		Close();
		return m_hSocket;
	}
	
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReUseAddr, 
		sizeof(bReUseAddr)) == SOCKET_ERROR)
	{
		Close();
		return m_hSocket;
	}
//add by zjj 2009-7-27 
	struct timeval tmpTVal;
	tmpTVal.tv_sec = 2;	
	tmpTVal.tv_usec = 0;	
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tmpTVal, 
		sizeof(tmpTVal)) == SOCKET_ERROR)
	{
		Close();
		return m_hSocket;
	}
//end add 

	//if(bClientFlag == FALSE) //modify by zjj 2012-10-10
	if((bClientFlag == FALSE) || ((SOCK_DGRAM == nType) && ((lpLocalHost != NULL) || (strcmp(lpLocalHost,"") != 0) || (0 != nLocalPort))))
	{
		if (Bind(lpLocalHost, nLocalPort) == SOCKET_ERROR)
		{
			Close();
			return m_hSocket;
		}
	}

	return m_hSocket;
}

//add by kent create MulBroadCast 
SOCKET CACSocket::CreateMulPoint(short nType, LPCTSTR lpRemoteHost, USHORT nRemotePort,
					  LPCTSTR lpLocalHost , USHORT nLocalPort)
{
	struct sockaddr_in local,remote ;

#ifdef _WIN32
	if((m_hSocket = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_MULTIPOINT_C_LEAF |
		WSA_FLAG_MULTIPOINT_D_LEAF | WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
#else
	m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);		
	if(m_hSocket < 0)
	{
		return INVALID_SOCKET;
	}
	int on = 1;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, 
		sizeof(int)) == SOCKET_ERROR)
	{
		Close();
		return m_hSocket;
	}
#endif

	local.sin_family = AF_INET;
	local.sin_port = htons(nLocalPort);

	if (lpLocalHost==NULL || strcmp(lpLocalHost,"")==0)
	{
		local.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		local.sin_addr.s_addr = inet_addr(lpLocalHost);
	}

	if(bind(m_hSocket, (struct sockaddr *)&local,sizeof(local)) == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	remote.sin_family = AF_INET;
	remote.sin_port = htons(nRemotePort);
	remote.sin_addr.s_addr = inet_addr(lpRemoteHost);

#ifndef _WIN32
	struct ip_mreq		mreq;
	//memcpy(&mreq.imr_multiaddr, &remote.sin_addr, sizeof(struct in_addr));
	mreq.imr_multiaddr.s_addr =  remote.sin_addr.s_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}
#endif
	int optval = 8;
	if(setsockopt(m_hSocket,IPPROTO_IP,IP_MULTICAST_TTL,(char *)&optval,sizeof(int)) == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	optval = 0;
	if(setsockopt(m_hSocket,IPPROTO_IP,IP_MULTICAST_LOOP,(char *)&optval,sizeof(int)) == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

#ifdef _WIN32
	if(WSAJoinLeaf(m_hSocket,(SOCKADDR *)&remote,sizeof(remote),NULL,NULL,NULL,NULL,JL_RECEIVER_ONLY) 
		== INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
#endif

	return m_hSocket;
}
//add end

void CACSocket::Close(void)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		printf("CACSocket::Close m_hSocket=[%d] pid[%d]\n",m_hSocket,pthread_self());
		closesocket(m_hSocket);
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","Socket Closed!");
		m_hSocket = INVALID_SOCKET;
	}
}

int CACSocket::Listen(int nBackLog)
{
	assert(m_hSocket != INVALID_SOCKET);

	m_bAutoClose = TRUE;
	return listen(m_hSocket, nBackLog);
}

int CACSocket::Select(int nMaxSocket, fd_set* pReadFDs, unsigned int nTimeOut, 
	fd_set* pWriteFDs, fd_set* pExceptFDs)
{
	struct timeval tvSelect;

	assert(pReadFDs != NULL);
	//assert(m_hSocket != INVALID_SOCKET);

	memset(&tvSelect, 0, sizeof(tvSelect));
	tvSelect.tv_sec = nTimeOut/1000;
	tvSelect.tv_usec = 1000*(nTimeOut%1000);

	return select(nMaxSocket, pReadFDs, pWriteFDs, pExceptFDs, &tvSelect); 
}

SOCKET CACSocket::Accept(char* lpRemoteHost, USHORT* lpRemotePort)
{
	SOCKET hSocket = INVALID_SOCKET;
	struct sockaddr_in saRemote;
	#ifdef _WIN32
	int nAddrLen = sizeof(saRemote);
	#else
	socklen_t nAddrLen = sizeof(saRemote);
	#endif

	assert(m_hSocket != INVALID_SOCKET);
	
	memset(&saRemote, 0, sizeof(saRemote));
	hSocket=accept(m_hSocket, (struct sockaddr*)&saRemote, &nAddrLen);
	if (hSocket != INVALID_SOCKET)
	{
		if (lpRemoteHost != NULL)
		{
			#ifdef _WIN32
			AddrToStr(saRemote.sin_addr.S_un.S_addr, lpRemoteHost);
			#else
			AddrToStr(saRemote.sin_addr.s_addr, lpRemoteHost);
			#endif
		}

		if (lpRemotePort != NULL)
		{
			*lpRemotePort = ntohs(saRemote.sin_port);
		}
	}

	return hSocket;
}

int CACSocket::Connect(LPCTSTR pRemoteHost, USHORT nRemotePort)
{
	assert(m_hSocket != INVALID_SOCKET);
	assert(pRemoteHost != NULL);
	assert(nRemotePort != 0);

	struct sockaddr_in  saRemoteAddr;
	memset(&saRemoteAddr, 0, sizeof(saRemoteAddr));
	
	saRemoteAddr.sin_family = AF_INET;
	saRemoteAddr.sin_port = htons(nRemotePort);
	saRemoteAddr.sin_addr.s_addr =  inet_addr(pRemoteHost);
/*
	int nret1 = 0;
	nret1 = inet_pton(AF_INET, pRemoteHost, &saRemoteAddr.sin_addr);
	if(nret1 != 1 )
	{
		int error = errno;
		return -1;
	}
*/	
	m_bAutoClose = FALSE;
	int nRet = connect(m_hSocket, (struct sockaddr*)&saRemoteAddr, 
		sizeof(saRemoteAddr));
	return nRet;
}

int CACSocket::Bind(LPCTSTR lpLocalHost, USHORT nLocalPort)
{
	struct sockaddr_in  saLocalAddr;

	assert(m_hSocket != INVALID_SOCKET);
	
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

	int nRet = bind(m_hSocket, (struct sockaddr*)&saLocalAddr, 
		sizeof(saLocalAddr));

	return nRet;
}


int CACSocket::Send(const char* lpBuf, DWORD dwSize)
{
	assert(lpBuf != NULL);
	assert(dwSize > 0);
	fd_set writeSet;

	FD_ZERO(&writeSet);

	//this->Select(0, NULL, 10, );
	int nResult = send(m_hSocket, lpBuf, dwSize, 0);
	return nResult;
}

int CACSocket::Receive(char* lpBuf, DWORD dwSize)
{
	if (m_hSocket==INVALID_SOCKET)
	{
		return -1;
	}
	assert(lpBuf != NULL);
	
	int nRet = recv(m_hSocket, lpBuf, dwSize, 0);
	if(nRet <= 0)
	{
		int Error=WSAGetLastError();
		LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error LastErrorCode=[%d]  nRet =[%ld]",Error,nRet);
	}
	return nRet;
}

//È¥µôHTTP°üÍ·
int CACSocket::ReceiveHttp(char *lpBuf,int &nLength)
{
	assert(m_hSocket != INVALID_SOCKET);
	int nRet = -1,nIndex = 0;
	while(1)
	{
		char szBuf[4];
		memset(szBuf,0,4);
		nRet = recv(m_hSocket, &szBuf[0], 1, 0);
		lpBuf[nIndex] = szBuf[0];
		nIndex++;
		if(nRet <= 0)
		{
			int Error=WSAGetLastError();
			//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error LastErrorCode=[%d]  nRet =[%ld]",Error,nRet);
			return nRet;
		}
		
		if(szBuf[0] == '\r')
		{
			for (int j=0;j<3;j++)
			{
				nRet = recv(m_hSocket, &szBuf[1+j], 1, 0);
				memcpy(&lpBuf[nIndex],&szBuf[1+j],1);
				nIndex ++;
				if(nRet <= 0)
				{
					int Error=WSAGetLastError();
					//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error LastErrorCode=[%d]  nRet =[%ld]",Error,nRet);
					return nRet;
				}
			}

			if(memcmp(szBuf,"\r\n\r\n",4) == 0)
			{
				break;
			}
			else
			{
				continue;
			}
		}
	}
	nLength = nIndex;
	return 1;	
}

int	CACSocket::GetField(const char* szBuf,const char* szSession,char *szValue,int nMaxLength)
{
	std::string strRespons;
	strRespons = szBuf;
	int nPos = -1;
	memset(szValue,0,nMaxLength);
	nPos = (int)strRespons.find(szSession,0);
	if(nPos != -1)
	{
		nPos += (int)strlen(szSession);
		nPos += 2;
		int nCr = (int)strRespons.find("\r\n",nPos);
		std::string strValue = strRespons.substr(nPos,nCr - nPos);
		strcpy(szValue,strValue.c_str());
		return (nCr - nPos);
	}
	else
	{
		return -1;
	}
}

int CACSocket::SendTo(LPCTSTR lpBuf, DWORD dwSize, LPCTSTR pRemoteHost,
	USHORT nRemotePort)
{
	struct sockaddr_in saRemoteAddr;

	assert(lpBuf != NULL);
	assert(dwSize > 0);
	assert(pRemoteHost != NULL);
	assert(nRemotePort > 0);

	memset(&saRemoteAddr, 0, sizeof(saRemoteAddr));
	saRemoteAddr.sin_family=AF_INET;
	saRemoteAddr.sin_port=htons(nRemotePort);
	saRemoteAddr.sin_addr.s_addr=inet_addr(pRemoteHost);
	int nRet = sendto(m_hSocket, lpBuf, dwSize, 0,				  
		(struct sockaddr*)&saRemoteAddr, sizeof(saRemoteAddr));

	return nRet;
}

int CACSocket::ReceiveFrom(char* lpBuf, DWORD dwSize, char* pRemoteHost,
	USHORT* pRemotePort)
{
	struct sockaddr_in saRemoteAddr;
	int nBytes = 0;
	#ifdef _WIN32
	int nAddrLen = sizeof(saRemoteAddr);
	#else
	socklen_t nAddrLen = sizeof(saRemoteAddr);
	#endif

	assert(m_hSocket != INVALID_SOCKET);
	assert(lpBuf != NULL);
	assert(dwSize > 0);

	memset(&saRemoteAddr, 0, nAddrLen);

	nBytes = recvfrom(m_hSocket, lpBuf, dwSize , 0,
		(struct sockaddr*)&saRemoteAddr, &nAddrLen);
	
	if(nBytes>=0)
	{
		if (pRemotePort != NULL)
		{
			*pRemotePort=ntohs(saRemoteAddr.sin_port);
		}

		if (pRemoteHost!= NULL)
		{
			#ifdef _WIN32
			AddrToStr(saRemoteAddr.sin_addr.S_un.S_addr, pRemoteHost);
			#else
			AddrToStr(saRemoteAddr.sin_addr.s_addr, pRemoteHost);
			#endif
		}
	}

	return nBytes;
}

int CACSocket::GetDataLength(void)
{
	assert(m_hSocket != INVALID_SOCKET);

	unsigned long nDataLen = 0;
	#ifdef _WIN32
	if (::ioctlsocket(m_hSocket, FIONREAD, &nDataLen) == SOCKET_ERROR)
	#else
	if (ioctl(m_hSocket, FIONREAD, &nDataLen) == SOCKET_ERROR)
	#endif
	{
		return -1;
	}

	return nDataLen;
}

SOCKET CACSocket::GetSocket(void)
{
	return m_hSocket;
}

int CACSocket::GetPeerName(char* lpRemoteHost, USHORT* lpRemotePort)
{
	assert(lpRemoteHost != NULL);
	assert(lpRemotePort != NULL);
	if (m_hSocket == INVALID_SOCKET)
	{
		return -1;
	}

	struct sockaddr_in saRemote;

	#ifdef _WIN32
	int nAddrLen = sizeof(saRemote);
	memset(&saRemote, 0, sizeof(saRemote));
	::getpeername(m_hSocket, (struct sockaddr*)&saRemote, &nAddrLen);
	AddrToStr(saRemote.sin_addr.S_un.S_addr, lpRemoteHost);
	#else
	socklen_t nAddrLen = sizeof(saRemote);
	memset(&saRemote, 0, sizeof(saRemote));
	getpeername(m_hSocket, (struct sockaddr*)&saRemote, &nAddrLen);
	AddrToStr(saRemote.sin_addr.s_addr, lpRemoteHost);
	#endif
	*lpRemotePort = ntohs(saRemote.sin_port); 
	
	return 0;
}

void CACSocket::AddrToStr(ULONG lIpAddr, char* lpIpAddr)  // lIpAddr is network byte order
{
	if (lpIpAddr == NULL)
	{
		return;
	}

	struct in_addr addr;
	#ifdef _WIN32
	addr.S_un.S_addr = ntohl(lIpAddr);
	sprintf(lpIpAddr, "%d.%d.%d.%d", addr.S_un.S_un_b.s_b4, addr.S_un.S_un_b.s_b3, 
		addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b1);
	#else
	char* pDestip = NULL;
//	addr.s_addr = ntohl(lIpAddr);
	addr.s_addr = lIpAddr;
	pDestip = inet_ntoa(addr);
	
	if((pDestip != NULL) &&(strlen(pDestip) < 16))
	{
		strcpy(lpIpAddr,pDestip);
	}
	#endif
}

int CACSocket::GetSockName(SOCKET hSocket, char* lpLocalHost, USHORT* lpLocalPort, 
	char* lpLocalMask)
{
	struct sockaddr_in saAddr;
	#ifdef _WIN32
	int nAddrLen = sizeof(saAddr);
	if (SOCKET_ERROR == ::getsockname(hSocket, 
		(struct sockaddr*)&saAddr, &nAddrLen))
	{
		return -1;
	}
	#else
	socklen_t nAddrLen = sizeof(saAddr);
	if (SOCKET_ERROR == getsockname(hSocket, 
		(struct sockaddr*)&saAddr, &nAddrLen))
	{
		return -1;
	}
	#endif

	if (lpLocalHost != NULL)
	{
		#ifdef _WIN32
		AddrToStr(saAddr.sin_addr.S_un.S_addr, lpLocalHost);
		#else
		AddrToStr(saAddr.sin_addr.s_addr, lpLocalHost);
		#endif
	}

	if (lpLocalPort != NULL)
	{
		*lpLocalPort = ntohs(saAddr.sin_port); 
	}

	if (lpLocalMask != NULL)
	{
		#ifdef _WIN32
		INTERFACE_INFO infBuf[10];
		DWORD dwInfSize = 0;

		if (::WSAIoctl(hSocket, SIO_GET_INTERFACE_LIST, NULL, 0, infBuf, 
			sizeof(infBuf), &dwInfSize, NULL, NULL) != SOCKET_ERROR)
		{
			DWORD dwCount = dwInfSize/sizeof(INTERFACE_INFO);
			if (dwCount > 10)
			{
				dwCount = 10;
			}

			for (DWORD i=0; i<dwCount; i++)
			{
				if (infBuf[i].iiAddress.AddressIn.sin_addr.S_un.S_addr 
					== saAddr.sin_addr.S_un.S_addr)
				{
					AddrToStr(infBuf[i].iiNetmask.AddressIn.sin_addr.S_un.S_addr, lpLocalMask);
					break;
				}
			}
		}
		#else

		int					 len;
		char				*ptr, buf[2048];
		struct ifconf		ifc;
		struct ifreq		*ifr;
		struct sockaddr_in	*sinptr,*addr;
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_req = (struct ifreq *) buf;

		ioctl(hSocket, SIOCGIFCONF, &ifc);

		for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
			ifr = (struct ifreq *) ptr;
			len = sizeof(struct sockaddr);
			ptr += sizeof(ifr->ifr_name) + len;	/* for next one in buffer */

			if (AF_INET == ifr->ifr_addr.sa_family) {
				sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
				if(saAddr.sin_addr.s_addr == sinptr->sin_addr.s_addr)
				{
					if(ioctl(hSocket,SIOCGIFNETMASK,ifr) == -1)
					{
						//printf("GetSockName ioctl SIOCGIFNETMASK  errno[%d] device name[%s] \n",errno,ifr->ifr_name);
						return 1;
					}
					addr = (struct sockaddr_in *)&ifr->ifr_addr;
        				strcpy(lpLocalMask , inet_ntoa(addr->sin_addr));
					break;
				}
			}
		}
	
		//printf("GetSockName lpLocalHost[%s] lpLocalPort[%d] localport[%d] lpLocalMask[%s]\n",lpLocalHost,*lpLocalPort,ntohs(saAddr.sin_port),lpLocalMask);
		#endif
	}
	return 0;
}
