//#include "stdafx.h"

#include <assert.h>

#include "LogTrace.h"
#include <pthread.h>

#include "TCPAcceptor.h"
#include "LogTrace.h"
#include "SockStream.h"
//#include "MyACDll.h"
#include <pthread.h>
#include "SockHeader.h"
#include "Tools.h"

/** ���캯��
*/
CTCPAcceptor::CTCPAcceptor(ISockEventCallBack* pCallBack):m_pCallBack(pCallBack)
{
	m_pSvrObj = NULL;
	m_pHeadInfo = NULL;
	m_bHeadFlag = true; //add 2012-10-18
}

/** ��������
*/
CTCPAcceptor::~CTCPAcceptor(void)
{
	close();
}

	/**
	* ����һ��TCP socket
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::create(unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create((const char*)NULL,port,pSockEventCallBack);
}

	/**
	* ����һ��TCP socket
	* @param ipstr socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack )
{
	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pSvrObj != NULL)
	{
		Stop();
	}

	//m_pSvrObj = ::CreateACObject(); // ����ͨ�Ŷ���
	m_pSvrObj = new CACMainObject(); // ����ͨ�Ŷ���
	//m_pHeadInfo = new CACPackHead(); //������ͷ�������
	if(m_bHeadFlag) //add 2012-10-18
	{
		m_pHeadInfo = new CSockHeader(); //������ͷ�������
	}
	if(NULL != pSockEventCallBack)
	{
		m_pCallBack = pSockEventCallBack;
	}

	// ��ʼ��ͨ�Ŷ���
#ifdef _WIN32
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 10, 20, 
		m_pCallBack, m_pHeadInfo);
#else
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 2, 4, 
		m_pCallBack, m_pHeadInfo);
#endif

	// ����ͨ�Ŷ���
	if (m_pSvrObj->BeginWork(ipstr, port, NULL, 0) != 0)
	{
		Stop();
		return -1;
	}

	return 0;
}

	/**
	* ����һ��TCP socket
	* @param ip socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create(ip_to_string(ip),port,pSockEventCallBack);
}

	/**
	* �رռ�����socket������acceptor������socket
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::close()
{
	Stop();
	return 0; //zzzz
}

	/**
	* �رռ�����socket
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::closeListen()
{
	Stop();
	return 0; //zzzz
}

	/**
	* �ر�ָ����acceptor������socket
	* @param sock Ҫ�رյ�socketֵ
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::close(ACE_HANDLE sock)
{
	if(NULL == m_pSvrObj)
		return -1;
	m_pSvrObj->CloseConnection(sock); 
	return 0;
}

	/**
	* ȡ��TCP socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪTCP socket
	*/
ACE_HANDLE CTCPAcceptor::getSock() const
{
	return 0;//zzzz
}

	/**
	* ͨ��TCP socket��������
	* @param sock �����ݷ��͵��ĸ�sock,���sockӦ������
	* ���TCPAcceptor����accept()������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
int CTCPAcceptor::sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len)
{
	if((INVALID_SOCKET == sock) ||(NULL == m_pSvrObj))
		return -1;
	return m_pSvrObj->SendCommTCPData(pdutype, (const BYTE*)data, len, TRUE, sock);
}
#if 0
/** ����ͨ�Ŷ���
* @param lpLocalHost ����IP
* @param nLocalPort �����˿�
* @param lpMainObj ͨ�Ų�������ָ��
* @return 0-�ɹ�, <0ʧ��
*/
int CTCPAcceptor::Start(USHORT nLocalPort, LPCTSTR lpLocalHost)
{
	assert(CreateACObject != NULL);

	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pSvrObj != NULL)
	{
		Stop();
	}

	//m_pSvrObj = ::CreateACObject(); // ����ͨ�Ŷ���
	m_pSvrObj = CreateACObject(); // ����ͨ�Ŷ���
	//m_pHeadInfo = new CACPackHead(); //������ͷ�������
	m_pHeadInfo = new CSockHeader(); //������ͷ�������

	// ��ʼ��ͨ�Ŷ���
#ifdef _WIN32
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 10, 20, 
		m_pCallBack, m_pHeadInfo);
#else
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 2, 4, 
		m_pCallBack, m_pHeadInfo);
#endif

	// ����ͨ�Ŷ���
	if (m_pSvrObj->BeginWork(lpLocalHost, nLocalPort, NULL, 0) != 0)
	{
		Stop();
		return -1;
	}

	return 0;
}


/** ��������
* @param wPduType ���ݰ���PDU���
* @param lpData Ҫ���͵�����
* @param dwSize ���ݳ���
* @param hSocket �������ݵ��׽��־��
*/
int CTCPAcceptor::SendCommData(WORD wPduType, const BYTE* lpData, 
	DWORD dwSize, SOCKET hSocket)
{
	assert(hSocket != INVALID_SOCKET);
	assert(m_pSvrObj != NULL);

	return m_pSvrObj->SendCommTCPData(wPduType, lpData, dwSize, 
		TRUE, hSocket);
}
#endif

/** �ر�ͨ������
* @param hSocket Ҫ�رյĻỰ���׽��־��
*/

/** ֹͣ��������
*/
void CTCPAcceptor::Stop(void)
{
	if (m_pSvrObj != NULL)
	{
		//assert(DestroyACObject != NULL);
	//	DestroyACObject(m_pSvrObj);
		delete (CACMainObject*)m_pSvrObj;
		
		m_pSvrObj = NULL;
	}
	
	if (m_pHeadInfo != NULL)
	{
		delete m_pHeadInfo;
		m_pHeadInfo = NULL;
	}
}
void CTCPAcceptor::CloseConnection(SOCKET hSocket)
{
	m_pSvrObj->CloseConnection(hSocket); 
}

