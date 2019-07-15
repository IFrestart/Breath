//#include "stdafx.h"

#include <assert.h>

//#include "callback.h"
#include "TCPConnector.h"
#include "LogTrace.h"
#include "SockStream.h"
//#include "MyACDll.h"
#include <pthread.h>
#include "SockHeader.h"

#include "Tools.h"

/** ���캯��
*/
CTCPConnector::CTCPConnector(ISockEventCallBack* pCallBack) : m_pCallBack(pCallBack)
{
	m_pAcCommObj = NULL;
	m_pHeadInfo = NULL;
	m_bHeadFlag = true; //add 2012-10-18
	m_nExitType=0;
}

/** ��������
*/
CTCPConnector::~CTCPConnector(void)
{
	Stop(); //add 2013-1-4
}

int CTCPConnector::GetExitType()
{
	return m_nExitType;
}

	/**
	* ����һ��TCP socket
	* @param ipstr socketҪ���ӵ���IP,�ַ���
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
int CTCPConnector::create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pAcCommObj != NULL)
	{
		printf("CTCPConnector::create  m_pAcCommObj != NULL\n");
		Stop();
	}
	
	m_pAcCommObj = new CACMainObject(); // ����ͨ�Ŷ���
	//m_pHeadInfo = new CACPackHead(); //������ͷ�������
	if(m_bHeadFlag) //add 2012-10-18
	{
		m_pHeadInfo = new CSockHeader(); //������ͷ�������
	}
	if(NULL != pSockEventCallBack)
	{
		m_pCallBack = pSockEventCallBack;
	}

	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, SOCK_STREAM, 1, 1, m_pCallBack, m_pHeadInfo);

	// ����ͨ�Ŷ���
	//int nRet=m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, lpszSvrAddr, nSvrPort,bMulPoint);
	int nRet=m_pAcCommObj->BeginWork(NULL, 0, ipstr, port,false);
	if ( nRet!= 0)
	{
		FreeObject();
		return nRet;
	}

	return 0;
}

	/**
	* ����һ��TCP socket
	* @param ip socketҪ���ӵ���IP
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
int CTCPConnector::create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create(ip_to_string(ip),port,pSockEventCallBack);
}

	/**
	* �ر�TCP socket
	* @return �ɹ��򷵻�0
	*/
int CTCPConnector::close()
{
	#if 1 //modify 2013-1-4
	if (m_pAcCommObj != NULL)
		m_pAcCommObj->CloseConnection();
	#else
	Stop(); 
	#endif
	return 0;
}

	/**
	* ͨ��TCP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
int CTCPConnector::sendPacket(unsigned short pdutype, const void* data, unsigned long len)
{
	//SOCKET hSocket = getSock();	
	bool bSync = true;
	if (NULL == m_pAcCommObj )
	{
		printf("CTCPConnector::sendPacket but m_pAcCommObj == NULL\n");
		return -2;
	}
	//int nResult = m_pAcCommObj->SendCommTCPData(wPduType, data, len, bSync, hSocket);
	int nResult = m_pAcCommObj->SendCommTCPData(pdutype, (const BYTE*)data, len, bSync);

	return nResult;
}

	/**
	* ����TCP socket�ײ��FD
	* @return ���ɹ��򷵻�-1
	*/
ACE_HANDLE CTCPConnector::getSock() const
{
	//if (m_pAcCommObj != NULL)
		//return m_pAcCommObj->GetSocket();
	//return -1;
	return 0;
}

#if 0
/** ����ͨ�Ŷ���(��ʹ�ô���)
* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost ����IP
* @param nLocalPort �����˿�
* @param lpszSvrAddr ������IP
* @param nSvrPort �������˿� 	
* @param bUseHttp �Ƿ�ʹ��HTTPЭ��(����HTTP�����HTTPЭ���½��,
������Ե�����ʱ,��˫����ͬһ������,��ΪFALSE,�������ΪTRUE)	
* @return 0-�ɹ�, <0ʧ��
*/
int CTCPConnector::Start(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpszSvrAddr, USHORT nSvrPort, BOOL bUseHttp,BOOL bMulPoint)
{
	//test function point
	assert(CreateACObject != NULL);

	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	
//	m_pAcCommObj = ::CreateACObject(); // ����ͨ�Ŷ���
	m_pAcCommObj = new CACMainObject(); // ����ͨ�Ŷ���
	//m_pHeadInfo = new CACPackHead(); //������ͷ�������

	// ��ʼ��ͨ�Ŷ���
	if (bUseHttp==TRUE && wProtoType==SOCK_STREAM) //��HTTPЭ��
	{
		m_pAcCommObj->InitHttp(m_pCallBack, m_pHeadInfo);
	}
	else //����HTTPЭ��
	{
		m_pAcCommObj->Initialize(COMMTYPE_CLIENT, wProtoType, 
			1, 1, m_pCallBack, m_pHeadInfo);
	}

	// ����ͨ�Ŷ���
	int nRet=m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, 
		lpszSvrAddr, nSvrPort,bMulPoint);
	if ( nRet!= 0)
	{
		FreeObject();
		return nRet;
	}

	return 0;
}

/** ����ͨ�Ŷ���(ʹ��SOCK5����)
* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost ����IP
* @param nLocalPort �����˿�
* @param lpszSvrAddr ������IP
* @param nSvrPort �������˿� 	
* @param lpProxyAddr ���������IP 
* @param nProxyPort ����������˿�
* @param lpProxyUserName �����û���
* @param lpProxyPassword ��������
* @return 0-�ɹ�, <0ʧ��
*/
int CTCPConnector::StartSock5(WORD wProtoType, LPCTSTR lpLocalHost, 
	USHORT nLocalPort, LPCTSTR lpszSvrAddr, USHORT nSvrPort, 
	LPCTSTR lpProxyAddr, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword)
{
	assert(CreateACObject != NULL);

	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	m_pAcCommObj = ::CreateACObject(); // ����ͨ�Ŷ���
	m_pHeadInfo = new CACPackHead(); //������ͷ�������

	// ��ʼ��ͨ�Ŷ���
	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, wProtoType, 1, 1, 
		m_pCallBack, m_pHeadInfo);

	// ����ͨ�Ŷ���
	if (m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, 
		lpszSvrAddr, nSvrPort, lpProxyAddr, nProxyPort, 
		lpProxyUserName, lpProxyPassword) != 0)
	{
		FreeObject();
		return -1;
	}

	return 0;
}

/** ����ͨ�Ŷ���(ʹ��HTTP����)
* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost ����IP
* @param nLocalPort �����˿�
* @param lpszSvrAddr ������IP
* @param nSvrPort �������˿� 	
* @param lpProxyAddr ���������IP 
* @param nProxyPort ����������˿�
* @param lpProxyUserName �����û���
* @param lpProxyPassword ��������
* @return 0-�ɹ�, <0ʧ��
*/
int CTCPConnector::StartHttp(WORD wProtoType, LPCTSTR lpLocalHost, 
	USHORT nLocalPort, LPCTSTR lpszSvrAddr, USHORT nSvrPort, 
	LPCTSTR lpProxyAddr, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword)
{
	assert(CreateACObject != NULL);

	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	m_pAcCommObj = ::CreateACObject(); // ����ͨ�Ŷ���
	m_pHeadInfo = new CACPackHead(); //������ͷ�������

	// ��ʼ��ͨ�Ŷ���
	m_pAcCommObj->InitHttp(m_pCallBack, m_pHeadInfo);

	// ����ͨ�Ŷ���
	if (m_pAcCommObj->BeginWorkHttp(lpLocalHost, nLocalPort, 
		lpszSvrAddr, nSvrPort, lpProxyAddr, nProxyPort, 
		lpProxyUserName, lpProxyPassword) != 0)
	{
		FreeObject();
		return -1;
	}

	return 0;
}

//void CTCPConnector::CloseConnect()//modify by zjj
void CTCPConnector::CloseConnect(SOCKET hSocket)

{
	if (m_pAcCommObj != NULL)
	{
	//	m_pAcCommObj->CloseConnection(GetSocket());
		m_pAcCommObj->CloseConnection(hSocket);
	}
}
#endif

/** ֹͣͨ�Ŷ���
*/
void CTCPConnector::Stop(int nType)//nType:1-should wait 0-need not wait
{
	
#if 0 //modify by zjj 2013-14
	//Destroy Comm, delete this and other action
	DWORD dwThreadId = 0;

	m_nExitType=nType;

#ifdef _WIN32
	HANDLE hStopThread = CreateThread(NULL, 0, StopThreadHelper, 
		this, 0, &dwThreadId);
#else
	pthread_t hStopThread = 0;
	if(0 == pthread_create(&hStopThread, NULL, StopThreadHelper, (void *)this));
#endif

	if (nType==1)
	{
#ifdef _WIN32
		::WaitForSingleObject(hStopThread, INFINITE);

		if (hStopThread != NULL)
		{
			::CloseHandle(hStopThread);
		}
#else
		pthread_join(hStopThread,NULL);
#endif

		//CreateThread(NULL, 0, StopMyselfThreadHelper,
		//	this, 0, &dwThreadId);
		//��Ȼ�ǿ����߳�ɾ���Լ�(��IE�лᷢ��crush)
		//�ָ�Ϊ����OpObjectMgrȥ����ɾ����OpObjectMgr�￪����һ���߳�
	//	OpObjectMgr::getInstance().release(this);
	}
#ifndef _WIN32
	else
	{
		pthread_detach(hStopThread);
	}
#endif
#endif
	
	FreeObject();
}


/** ɾ��ͨ�Ŷ���(�����߳�ɾ��)
*/
void CTCPConnector::FreeObject(void)
{
	//assert(DestroyACObject != NULL);
	//DestroyACObject(m_pAcCommObj);
	if(NULL != m_pAcCommObj)
	{
		delete (CACMainObject*)m_pAcCommObj;
		m_pAcCommObj = NULL;
	}
	
	if (m_pHeadInfo != NULL)
	{
		delete m_pHeadInfo;
		m_pHeadInfo = NULL;
	}
}

#if 0
/** UDPͨ�Ŷ���������
* @param wPduType ���ݰ���PDU���
* @param lpData Ҫ���͵�����
* @param dwSize ���ݳ���
* @param bSync �Ƿ�ͬ������(Ŀǰֻ֧��ͬ������)
* @param lpRemoteHost ���շ���IP
* @param nRemotePort ���շ��Ķ˿�
* @param wExtParam ��չ����(���ڳ������)
* @param bSock5Flag	�����SOCK5��½��������,��Ե����Ӽ䷢�������Ƿ��
SOCK5ͷ(��˫����ͬһ������ʱ,ӦΪFALSE)
*/
int CTCPConnector::SendCommUDPData(WORD wPduType, const BYTE* lpData, 
	DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
	WORD wExtParam, BOOL bSock5Flag)
{
	assert(m_pAcCommObj != NULL);

	int nResult = m_pAcCommObj->SendCommUDPData(wPduType, lpData, dwSize, 
		bSync, lpRemoteHost, nRemotePort, wExtParam, bSock5Flag);
	return nResult;
}

/** TCPͨ�Ŷ���������
* @param wPduType ���ݰ���PDU���
* @param lpData Ҫ���͵�����
* @param dwSize ���ݳ���
* @param bSync �Ƿ�ͬ������(Ŀǰֻ֧��ͬ������)
* @param hSocket ��������ʹ��
*/
int CTCPConnector::SendCommTCPData(WORD wPduType, const BYTE* lpData, 
	DWORD dwSize, BOOL bSync, SOCKET hSocket)
{
	assert(m_pAcCommObj != NULL);

	int nResult = m_pAcCommObj->SendCommTCPData(wPduType, lpData, 
		dwSize, bSync, hSocket);

	return nResult;
}
#endif

///** ��ȡ����IP
//*/
//LPCTSTR CTCPConnector::GetLocalHost(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalHost();
//}
//
///** ��ȡ������������
//*/
//LPCTSTR CTCPConnector::GetLocalMask(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalMask();
//}
//
///** ��ȡ���ض˿�
//*/
//USHORT CTCPConnector::GetLocalPort(void)
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalPort();
//}
//
///** ��ȡͨ�Ŷ�����׽��־��
//*/
//SOCKET CTCPConnector::GetSocket(void)
//{
//	assert(m_pAcCallBack != NULL);
//	return m_pAcCallBack->GetSocket();
//}

/** ֹͣͨ�Ŷ���ʵ���߳�
*/ 
#ifdef _WIN32
DWORD WINAPI CTCPConnector::StopThreadHelper(LPVOID lpParam)
#else
void*  CTCPConnector::StopThreadHelper(LPVOID lpParam)
#endif
{
	CTCPConnector* pThis = (CTCPConnector*)lpParam;

	if ((pThis->m_pAcCommObj) != NULL)
	{
		//assert(DestroyACObject != NULL);
		//DestroyACObject(pThis->m_pAcCommObj);
		delete (CACMainObject*)pThis->m_pAcCommObj;
		pThis->m_pAcCommObj = NULL;

	}
	
	if ((pThis->m_pHeadInfo) != NULL)
	{
		delete pThis->m_pHeadInfo;
		pThis->m_pHeadInfo = NULL;
	}

	if ((pThis->GetExitType())==0)
	{
		delete pThis;
	}

	return 0;
}

#ifdef _WIN32
DWORD WINAPI CTCPConnector::StopMyselfThreadHelper(LPVOID lpParam)
#else
void*  CTCPConnector::StopMyselfThreadHelper(LPVOID lpParam)
#endif
{
	CTCPConnector* pThis = (CTCPConnector*)lpParam;

	delete pThis;
	return 0;
}

