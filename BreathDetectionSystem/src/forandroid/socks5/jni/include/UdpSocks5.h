/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        UdpSocks5.h
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#ifndef _UDPSOCKET5_H
#define _UDPSOCKET5_H

#include "CommFunction.h"

class CUdpSocks5 
{
public:
	CUdpSocks5();
	~CUdpSocks5();

public:
	
	SOCKS5_SOCKET Create(const char * pLocalHost,	unsigned short nLocalPort,
				const char * pProxyHost, //代理服务器IP地址
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wProxyHost, //代理服务器IP地址
				unsigned short nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	void Close();
	SOCKS5_SOCKET GetSocket();
	unsigned short GetPort();
	unsigned short GetProxyBindPort();
	int Sendto(const char * lpBuf, unsigned long wSize, unsigned long wRemoteHost,
		unsigned short nRemotePort,int flags = 0);
	int ReceiveFrom(char* lpBuf, unsigned long wSize, unsigned long* pRemoteHost=NULL,
		unsigned short* pRemotePort=NULL,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectUdpProxyServer(
				  const char * pLocalHost, unsigned short nLocalPort,
				  const char * pProxyHost, //代理服务器IP地址
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码
				  );
	SOCKS5_SOCKET ConnectUdpProxyServer(
				  unsigned long wLocalHost, unsigned short nLocalPort,
				  unsigned long wProxyHost, //代理服务器IP地址
				  unsigned short nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码
				  );
	CCommFunction m_CommFun;
private:
	SOCKS5_SOCKET m_hSocket;    	
	SOCKS5_SOCKET m_TcpSocks5;   //协商后一直保持的tcp连接
	unsigned short m_ProxyBindPort;  
	char m_strProxyIp[16];

	int BuildUdpSocket5Head(char * outbuf,unsigned long wRemoteHost,unsigned short nRemotePort);	
};

#endif
