#ifndef __COMMFUNCTION_H__
#define __COMMFUNCTION_H__

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
#include "LinuxDef.h"


#endif

class CCommFunction
{
public:
	static int ConsultProcess(SOCKS5_SOCKET sock, const char* pSendBuf, int nSendLen,
		char *pRecvBuf, int* pRecvLen, int nTimeOut);
	static int RecvWait(SOCKS5_SOCKET sock, unsigned int nTimeOut); //≥¨ ± ±º‰(∫¡√Î)
	static int Bind(SOCKS5_SOCKET sock, const char * lpLocalHost, USHORT nLocalPort);
	static int Bind(SOCKS5_SOCKET sock, unsigned long wLocalHost, USHORT nLocalPort);
	static int LToStrIp(unsigned long lIp, char *StrIp);
};

#endif
