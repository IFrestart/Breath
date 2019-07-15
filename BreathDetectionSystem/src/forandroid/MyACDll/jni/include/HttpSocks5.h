#ifndef __HTTPSOCKET5_H__
#define __HTTPSOCKET5_H__

#include "CommFunction.h"

class CHttpSocks5 
{
public:
	CHttpSocks5();
	~CHttpSocks5();

public:
	SOCKS5_SOCKET Create(const char * pLocalHost,	USHORT nLocalPort,
				const char * pRemoteHost,USHORT nRemotePort,
				const char * pProxyHost, //代理服务器IP地址
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	USHORT nLocalPort,
				unsigned long wRemoteHost,USHORT nRemotePort,
				unsigned long wProxyHost, //代理服务器IP地址
				USHORT nProxyPort, //代理服务器端口
				const char * pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				const char * pProxyPassword); //代理密码

	void Close();
	SOCKS5_SOCKET GetSocket();
	int Send(const char * lpBuf, unsigned long wSize,int flags = 0);
	int Receive(char* lpBuf, unsigned long wSize,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //目标服务器IP地址 
				  USHORT nDestPort, //目标服务器端口
				  const char *pProxyHost, //代理服务器IP地址
				  USHORT nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码				  
				  );
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //目标服务器IP地址 
				  USHORT nDestPort, //目标服务器端口
				  unsigned long wProxyHost, //代理服务器IP地址
				  USHORT nProxyPort, //代理服务器端口
				  const char* pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
				  const char* pProxyPassword, //代理密码
				  unsigned int nTimeOut,	//超时时间
				  int* pErrCode //错误代码				  
				  );

private:
	SOCKS5_SOCKET m_hSocket;  
	//char *make_base64_string(const char *str);
};
#endif
