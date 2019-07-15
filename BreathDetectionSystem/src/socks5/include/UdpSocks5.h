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
				const char * pProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

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
				  const char * pProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������
				  );
	SOCKS5_SOCKET ConnectUdpProxyServer(
				  unsigned long wLocalHost, unsigned short nLocalPort,
				  unsigned long wProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������
				  );
	CCommFunction m_CommFun;
private:
	SOCKS5_SOCKET m_hSocket;    	
	SOCKS5_SOCKET m_TcpSocks5;   //Э�̺�һֱ���ֵ�tcp����
	unsigned short m_ProxyBindPort;  
	char m_strProxyIp[16];

	int BuildUdpSocket5Head(char * outbuf,unsigned long wRemoteHost,unsigned short nRemotePort);	
};

#endif
