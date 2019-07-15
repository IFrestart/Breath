
#ifndef __UDPSOCKET5_H__
#define __UDPSOCKET5_H__

#include "CommFunction.h"

class CUdpSocks5 
{
public:
	CUdpSocks5(void);
	~CUdpSocks5(void);

public:
	SOCKS5_SOCKET Create(const char * pLocalHost,	USHORT nLocalPort,
				const char * pProxyHost, //代理服务器IP地址
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	USHORT nLocalPort,
				unsigned long wProxyHost, //代理服务器IP地址
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	void Close();
	SOCKS5_SOCKET GetSocket();
	int Sendto(const char * lpBuf, unsigned long wSize, unsigned long wRemoteHost,
		USHORT nRemotePort,int flags = 0);
	int ReceiveFrom(char* lpBuf, unsigned long wSize, unsigned long* pRemoteHost=NULL,
		USHORT* pRemotePort=NULL,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectUdpProxyServer(
				  const char * pLocalHost, USHORT nLocalPort,
				  const char * pProxyHost, //代理服务器IP地址
				  USHORT nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码
				  );
	SOCKS5_SOCKET ConnectUdpProxyServer(ULONG wLocalHost, USHORT nLocalPort,	
		ULONG wProxyHost, USHORT nProxyPort, LPCTSTR pProxyUserName, 
		LPCTSTR pProxyPassword, ULONG nTimeOut, int* pErrCode);
private:
	SOCKS5_SOCKET m_hSocket;    	
	SOCKS5_SOCKET m_TcpSocks5;   //协商后一直保持的tcp连接
	SOCKS5_SOCKET m_ProxySocks5;  
	char m_strProxyIp[16];

	int BuildUdpSocket5Head(char * outbuf,unsigned long wRemoteHost,USHORT nRemotePort);	
};

#endif
