#ifndef __TCPSOCKET5_H__
#define __TCPSOCKET5_H__

#include "CommFunction.h"

class CACSocket;
class CTcpSocks5
{
public:
	CTcpSocks5(void);
	~CTcpSocks5(void);

public:
	SOCKS5_SOCKET Create(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpRemoteHost,
		USHORT nRemotePort, LPCTSTR lpProxyHost, USHORT nProxyPort, 
		LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword, int type);

	SOCKS5_SOCKET Create(ULONG lLocalHost, USHORT nLocalPort, ULONG lRemoteHost,
		USHORT nRemotePort, ULONG lProxyHost, USHORT nProxyPort, 
		LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword, int type);

	void Close(void);
	SOCKS5_SOCKET GetSocket(void);
	//int Send(const char * lpBuf, unsigned long wSize,int flags = 0);
	//int Receive(char* lpBuf, unsigned long wSize,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectTcpProxyServer(LPCTSTR *lpDestHost, USHORT nDestPort, 
		LPCTSTR *lpProxyHost, USHORT nProxyPort, LPCTSTR pProxyUserName, 
		LPCTSTR pProxyPassword, ULONG lTimeOut, int* pErrCode, int type);

	SOCKS5_SOCKET ConnectTcpProxyServer(ULONG wDestHost, USHORT nDestPort, 
		ULONG lProxyHost, USHORT nProxyPort, LPCTSTR pProxyUserName, 
		LPCTSTR lpProxyPassword, ULONG lTimeOut, int* pErrCode, int type); 
private:
	CACSocket* m_pSocket; 
};
#endif
