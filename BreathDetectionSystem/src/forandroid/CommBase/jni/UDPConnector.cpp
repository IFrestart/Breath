//#include "stdafx.h"

#include <assert.h>

//#include "callback.h"
#include "UDPConnector.h"
#include "LogTrace.h"
#include "SockStream.h"
//#include "MyACDll.h"
#include <pthread.h>
#include "SockHeader.h"

#include "Tools.h"


/** ���캯��
*/
CUDPConnector::CUDPConnector(ISockEventCallBack* pCallBack) : m_pCallBack(pCallBack)
{
	m_pAcCommObj = NULL;
	m_pHeadInfo = NULL;

	m_nExitType=0;
}

/** ��������
*/
CUDPConnector::~CUDPConnector(void)
{
	Stop(); //add 2013-1-4
}

int CUDPConnector::GetExitType()
{
	return m_nExitType;
}

	/**
	* ����һ��UDP socket
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @return �ɹ��򷵻�0
	*/
int CUDPConnector::create(unsigned short port)
{
	return create(NULL,port);
}

	/**
	* ����һ��UDP socket
	* @param ipstr socket��ʹ�õ�IP��ַ,��Ϊ0,������е�IP��ַ
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @return �ɹ��򷵻�0
	*/
int CUDPConnector::create(const char* ipstr, unsigned short port)
{
	// �ر��Ѵ򿪵�ͨ�Ŷ���
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}
	
	m_pAcCommObj = new CACMainObject(); // ����ͨ�Ŷ���
	//m_pHeadInfo = new CACPackHead(); //������ͷ�������
	m_pHeadInfo = new CSockHeader(); //������ͷ�������

	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, SOCK_DGRAM, 1, 1, m_pCallBack, m_pHeadInfo);

	// ����ͨ�Ŷ���
	//int nRet=m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, lpszSvrAddr, nSvrPort,bMulPoint);
	int nRet=m_pAcCommObj->BeginWork(ipstr, port, NULL, 0,false);
	if ( nRet!= 0)
	{
		FreeObject();
		return nRet;
	}

	return 0;
}

	/**
	* �ر�UDP socket
	* @return �ɹ��򷵻�0
	*/
int CUDPConnector::close()
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
	* ͨ��UDP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param buf socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @param ip socket�����ݷ��͵��ĸ�IP
	* @param port socket�����ݷ��͵��ĸ�PORT
	* @return �ɹ��򷵻�0
	*/
int CUDPConnector::sendPacket(unsigned short pdutype, const void* buf, unsigned long len, unsigned long ip, unsigned short port)
{
	//SOCKET hSocket = getSock();	
	bool bSync = true;
	//int nResult = m_pAcCommObj->SendCommTCPData(wPduType, data, len, bSync, hSocket);
	int nResult = m_pAcCommObj->SendCommUDPData(pdutype,(const BYTE*)buf,len,bSync,ip_to_string(ip),port,0,false);

	return nResult;
}

	/**
	* ͨ��UDP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param buf socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @param ipstr socket�����ݷ��͵��ĸ�IP
	* @param port socket�����ݷ��͵��ĸ�PORT
	* @return �ɹ��򷵻�0
	*/
int CUDPConnector::sendPacket(unsigned short pdutype, const void* buf, unsigned long len, const char* ipstr, unsigned short port)
{
	bool bSync = true;
	int nResult = m_pAcCommObj->SendCommUDPData(pdutype,(const BYTE*)buf,len,bSync,ipstr,port,0,false);

	return nResult;
}

	/**
	* ȡ��UDP socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪUDP socket
	*/
ACE_HANDLE CUDPConnector::getSock() const
{
	//if (m_pAcCommObj != NULL)
		//return m_pAcCommObj->GetSocket();
	//return -1;
	return 0;
}

/** ֹͣͨ�Ŷ���
*/
void CUDPConnector::Stop(int nType)//nType:1-should wait 0-need not wait
{
#if 0	//modify 2013-1-4
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
void CUDPConnector::FreeObject(void)
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
int CUDPConnector::SendCommUDPData(WORD wPduType, const BYTE* lpData, 
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
int CUDPConnector::SendCommTCPData(WORD wPduType, const BYTE* lpData, 
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
//LPCTSTR CUDPConnector::GetLocalHost(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalHost();
//}
//
///** ��ȡ������������
//*/
//LPCTSTR CUDPConnector::GetLocalMask(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalMask();
//}
//
///** ��ȡ���ض˿�
//*/
//USHORT CUDPConnector::GetLocalPort(void)
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalPort();
//}
//
///** ��ȡͨ�Ŷ�����׽��־��
//*/
//SOCKET CUDPConnector::GetSocket(void)
//{
//	assert(m_pAcCallBack != NULL);
//	return m_pAcCallBack->GetSocket();
//}

/** ֹͣͨ�Ŷ���ʵ���߳�
*/ 
#ifdef _WIN32
DWORD WINAPI CUDPConnector::StopThreadHelper(LPVOID lpParam)
#else
void*  CUDPConnector::StopThreadHelper(LPVOID lpParam)
#endif
{
	CUDPConnector* pThis = (CUDPConnector*)lpParam;

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
DWORD WINAPI CUDPConnector::StopMyselfThreadHelper(LPVOID lpParam)
#else
void*  CUDPConnector::StopMyselfThreadHelper(LPVOID lpParam)
#endif
{
	CUDPConnector* pThis = (CUDPConnector*)lpParam;

	delete pThis;
	return 0;
}

