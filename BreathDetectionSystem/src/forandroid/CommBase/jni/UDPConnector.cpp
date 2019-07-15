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


/** 构造函数
*/
CUDPConnector::CUDPConnector(ISockEventCallBack* pCallBack) : m_pCallBack(pCallBack)
{
	m_pAcCommObj = NULL;
	m_pHeadInfo = NULL;

	m_nExitType=0;
}

/** 析构函数
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
	* 创建一个UDP socket
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @return 成功则返回0
	*/
int CUDPConnector::create(unsigned short port)
{
	return create(NULL,port);
}

	/**
	* 创建一个UDP socket
	* @param ipstr socket所使用的IP地址,若为0,则绑定所有的IP地址
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @return 成功则返回0
	*/
int CUDPConnector::create(const char* ipstr, unsigned short port)
{
	// 关闭已打开的通信对象
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}
	
	m_pAcCommObj = new CACMainObject(); // 创建通信对象
	//m_pHeadInfo = new CACPackHead(); //创建包头处理对象
	m_pHeadInfo = new CSockHeader(); //创建包头处理对象

	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, SOCK_DGRAM, 1, 1, m_pCallBack, m_pHeadInfo);

	// 启动通信对象
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
	* 关闭UDP socket
	* @return 成功则返回0
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
	* 通过UDP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param buf socket要发送的数据
	* @param len socket要发送数据的长度
	* @param ip socket将数据发送到哪个IP
	* @param port socket将数据发送到哪个PORT
	* @return 成功则返回0
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
	* 通过UDP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param buf socket要发送的数据
	* @param len socket要发送数据的长度
	* @param ipstr socket将数据发送到哪个IP
	* @param port socket将数据发送到哪个PORT
	* @return 成功则返回0
	*/
int CUDPConnector::sendPacket(unsigned short pdutype, const void* buf, unsigned long len, const char* ipstr, unsigned short port)
{
	bool bSync = true;
	int nResult = m_pAcCommObj->SendCommUDPData(pdutype,(const BYTE*)buf,len,bSync,ipstr,port,0,false);

	return nResult;
}

	/**
	* 取得UDP socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为UDP socket
	*/
ACE_HANDLE CUDPConnector::getSock() const
{
	//if (m_pAcCommObj != NULL)
		//return m_pAcCommObj->GetSocket();
	//return -1;
	return 0;
}

/** 停止通信对象
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
		//本然是开辟线程删除自己(在IE中会发生crush)
		//现改为交给OpObjectMgr去处理删除，OpObjectMgr里开辟了一个线程
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


/** 删除通信对象(不用线程删除)
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
/** UDP通信对象发送数据
* @param wPduType 数据包的PDU类别
* @param lpData 要发送的数据
* @param dwSize 数据长度
* @param bSync 是否同步发送(目前只支持同步发送)
* @param lpRemoteHost 接收方的IP
* @param nRemotePort 接收方的端口
* @param wExtParam 扩展参数(用于程序调试)
* @param bSock5Flag	如果用SOCK5登陆到服务器,点对点连接间发送数据是否加
SOCK5头(当双方在同一局域网时,应为FALSE)
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

/** TCP通信对象发送数据
* @param wPduType 数据包的PDU类别
* @param lpData 要发送的数据
* @param dwSize 数据长度
* @param bSync 是否同步发送(目前只支持同步发送)
* @param hSocket 本参数不使用
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

///** 获取本机IP
//*/
//LPCTSTR CUDPConnector::GetLocalHost(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalHost();
//}
//
///** 获取本机子网掩码
//*/
//LPCTSTR CUDPConnector::GetLocalMask(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalMask();
//}
//
///** 获取本地端口
//*/
//USHORT CUDPConnector::GetLocalPort(void)
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalPort();
//}
//
///** 获取通信对象的套接字句柄
//*/
//SOCKET CUDPConnector::GetSocket(void)
//{
//	assert(m_pAcCallBack != NULL);
//	return m_pAcCallBack->GetSocket();
//}

/** 停止通信对象实现线程
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

