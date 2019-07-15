
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
				const char * pProxyHost, //���������IP��ַ
				USHORT nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

	SOCKS5_SOCKET Create(unsigned long wLocalHost,	USHORT nLocalPort,
				unsigned long wProxyHost, //���������IP��ַ
				USHORT nProxyPort, //����������˿�
				const char * pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				const char * pProxyPassword); //��������

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
				  const char * pProxyHost, //���������IP��ַ
				  USHORT nProxyPort, //����������˿�
				  const char* pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
				  const char* pProxyPassword, //��������
				  unsigned int nTimeOut,	//��ʱʱ��
				  int* pErrCode //�������
				  );
	SOCKS5_SOCKET ConnectUdpProxyServer(ULONG wLocalHost, USHORT nLocalPort,	
		ULONG wProxyHost, USHORT nProxyPort, LPCTSTR pProxyUserName, 
		LPCTSTR pProxyPassword, ULONG nTimeOut, int* pErrCode);
private:
	SOCKS5_SOCKET m_hSocket;    	
	SOCKS5_SOCKET m_TcpSocks5;   //Э�̺�һֱ���ֵ�tcp����
	SOCKS5_SOCKET m_ProxySocks5;  
	char m_strProxyIp[16];

	int BuildUdpSocket5Head(char * outbuf,unsigned long wRemoteHost,USHORT nRemotePort);	
};

#endif
