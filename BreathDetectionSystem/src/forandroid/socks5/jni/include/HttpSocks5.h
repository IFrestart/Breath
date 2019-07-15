/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file        HttpSocks5.h
 * @author      zhoujj <zhoujj@webcon.com.cn>
 * @data        05/2004
 * @short
 **/
#ifndef _HTTPSOCKET5_H
#define _HTTPSOCKET5_H

#include "CommFunction.h"

class CHttpSocks5 
{
public:
	CHttpSocks5();
	~CHttpSocks5();

public:
	SOCKS5_SOCKET Create(const char * pLocalHost,	unsigned short nLocalPort,
				const char * pRemoteHost,unsigned short nRemotePort,
				const char * pProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	unsigned short nLocalPort,
				unsigned long wRemoteHost,unsigned short nRemotePort,
				unsigned long wProxyHost, //���������IP��ַ
				unsigned short nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	void Close();
	SOCKS5_SOCKET GetSocket();
	int Send(const char * lpBuf, unsigned long wSize,int flags = 0);
	int Receive(char* lpBuf, unsigned long wSize,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  const char *pProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				  
				  );
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  unsigned short nDestPort, //Ŀ��������˿�
				  unsigned long wProxyHost, //���������IP��ַ
				  unsigned short nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				  
				  );

private:
	SOCKS5_SOCKET m_hSocket;  
	CCommFunction m_CommFun;
	char *make_base64_string(const char *str);
};
#endif
