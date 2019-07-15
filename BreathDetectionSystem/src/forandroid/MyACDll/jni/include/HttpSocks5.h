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
				const char * pProxyHost, //���������IP��ַ
				USHORT nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	USHORT nLocalPort,
				unsigned long wRemoteHost,USHORT nRemotePort,
				unsigned long wProxyHost, //���������IP��ַ
				USHORT nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	void Close();
	SOCKS5_SOCKET GetSocket();
	int Send(const char * lpBuf, unsigned long wSize,int flags = 0);
	int Receive(char* lpBuf, unsigned long wSize,int flags = 0);

public:
	void SetSocket(SOCKS5_SOCKET sock);
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  USHORT nDestPort, //Ŀ��������˿�
				  const char *pProxyHost, //���������IP��ַ
				  USHORT nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				  
				  );
	SOCKS5_SOCKET ConnectHttpProxyServer(const char* pDestHost,  //Ŀ�������IP��ַ 
				  USHORT nDestPort, //Ŀ��������˿�
				  unsigned long wProxyHost, //���������IP��ַ
				  USHORT nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������				  
				  );

private:
	SOCKS5_SOCKET m_hSocket;  
	//char *make_base64_string(const char *str);
};
#endif
