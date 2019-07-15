/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        TcpSocks5.h
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#ifndef _TCPSOCKET5_H
#define _TCPSOCKET5_H

#include "CommFunction.h"

class CTcpSocks5
{
public:
	CTcpSocks5();
	~CTcpSocks5();

public:
	SOCKS5_SOCKET Create(const char * pLocalHost,	unsigned short nLocalPort,
				const char * pRemoteHost,unsigned short nRemotePort,
				const char * pProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword, //��������
				int type);	//connect��ʽ��bind��ʽ

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wRemoteHost,unsigned short nRemotePort,
				unsigned long wProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword, //��������
				int type);	//connect��ʽ��bind��ʽ

	void Close();
	SOCKS5_SOCKET GetSocket();
	int Send(const char * lpBuf, unsigned long wSize,int flags = 0);
	int Receive(char* lpBuf, unsigned long wSize,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectTcpProxyServer(const char *pDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  const char *pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode, //�������
				  int type
				  )	;
	SOCKS5_SOCKET ConnectTcpProxyServer(unsigned long wDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  unsigned long pProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode, //�������
				  int type
				  )	; 
private:
	SOCKS5_SOCKET m_hSocket; 
	CCommFunction m_CommFun;	
};
#endif
