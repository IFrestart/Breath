// ACSocket.h: interface for the CACSocket class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACSOCKET_H__
#define __ACSOCKET_H__

//#include <WinSock2.h>
#include "BaseACObject.h"

#define ACCEPT_TIMEOUT_SET                 100    //unit:ms

#define UDPSOCKET_BUF_SIZE	               64*1024
#define TCPSOCKET_BUF_SIZE                 64*1024;

class CACSocket  
{
public:
	CACSocket(void);
	virtual ~CACSocket(void);

public:
	SOCKET Create(short nType, LPCTSTR lpLocalHost = NULL, USHORT nLocalPort = 0,BOOL bClientFlag = FALSE);

	//add by kent
	SOCKET CreateMulPoint(short nType, LPCTSTR lpRemoteHost, USHORT nRemotePort,
							LPCTSTR lpLocalHost = NULL, USHORT nLocalPort = 0);
	//add end
	void Attach(SOCKET hSocket);
	void Close(void);
	int Listen(int nBackLog);
	SOCKET Accept(char* lpRemoteHost=NULL, USHORT* lpRemotePort=NULL);
	int Connect(LPCTSTR lpRemoteHost, USHORT nRemotePort);
	int Bind(LPCTSTR lpLocalHost=NULL, USHORT nLocalPort=0);

	int Select(int nMaxSocket, fd_set* pReadFDs, unsigned int nTimeOut, 
		fd_set* pWriteFDs=NULL, fd_set* pExceptFDs=NULL);
	int Send(const char* lpBuf, DWORD dwSize);

	int Receive(char* lpBuf, DWORD dwSize);

	//add by kent for HTTP
	int ReceiveHttp(char *lpBuf,int &nLength);
	//返回某个域值,-1表示不成功
	int	GetField(const char* szBuf,const char* szSession,char *szValue,int nMaxLength);	
	//add end

	int SendTo(LPCTSTR lpBuf, DWORD dwSize, LPCTSTR pRemoteHost, USHORT nRemotePort);
	int ReceiveFrom(char* lpBuf, DWORD dwSize, char* pRemoteHost=NULL,
		USHORT* pRemotePort=NULL);
	int GetDataLength(void);

	SOCKET GetSocket(void);
	int GetPeerName(char* lpRemoteHost, USHORT* lpRemotePort);

	static void AddrToStr(ULONG lIpAddr, char* lpIpAddr);
	static int GetSockName(SOCKET hSocket, char* lpLocalHost, 
		USHORT* lpLocalPort, char* lpLocalMask);

private:
	SOCKET m_hSocket;
	BOOL m_bAutoClose;
};
#endif
