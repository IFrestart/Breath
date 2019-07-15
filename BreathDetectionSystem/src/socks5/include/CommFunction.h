/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        CommFucticon.h
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#ifndef _COMMFUNCTION_H
#define _COMMFUNCTION_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "WinSock2.h"
#else

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include    <sys/un.h>
#include    <unistd.h>


#endif


#ifdef _WIN32
#define SOCKS5_ERRNO() WSAGetLastError()
#define SOCKS5_CLOSE closesocket
#define SOCKS5_SOCKET SOCKET
#define SOCKS5_ERROR SOCKET_ERROR
#define INVALID_SOCKS5 INVALID_SOCKET


#else /* !_WIN32 */
#define SOCKS5_ERRNO() (errno)
#define SOCKS5_CLOSE close
#define SOCKS5_SOCKET int
#define SOCKS5_ERROR -1
#define INVALID_SOCKS5 -1

#endif /* !_WIN32 */


#define SOCKS5_SERVER                      1
#define SOCKS5_CLIENT                      2


class CCommFunction
{
public:
	CCommFunction();
	~CCommFunction();

public:
	int ConsultProcess(SOCKS5_SOCKET sock,const char* pSendBuf, int nSendLen,char *pRecvBuf, int* pRecvLen, int nTimeOut);
	int RecvWait(SOCKS5_SOCKET sock,unsigned int nTimeOut); //≥¨ ± ±º‰(∫¡√Î)
	int Bind(SOCKS5_SOCKET sock,const char * lpLocalHost, unsigned short nLocalPort);
	int Bind(SOCKS5_SOCKET sock,unsigned long wLocalHost, unsigned short nLocalPort);
	int LToStrIp(unsigned long lIp,char *StrIp);

};

#endif
