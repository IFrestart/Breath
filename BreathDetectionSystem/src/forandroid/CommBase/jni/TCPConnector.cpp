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

/** 构造函数
*/
CTCPConnector::CTCPConnector(ISockEventCallBack* pCallBack) : m_pCallBack(pCallBack)
{
	m_pAcCommObj = NULL;
	m_pHeadInfo = NULL;
	m_bHeadFlag = true; //add 2012-10-18
	m_nExitType=0;
}

/** 析构函数
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
	* 创建一个TCP socket
	* @param ipstr socket要连接到的IP,字符串
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
int CTCPConnector::create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	// 关闭已打开的通信对象
	if (m_pAcCommObj != NULL)
	{
		printf("CTCPConnector::create  m_pAcCommObj != NULL\n");
		Stop();
	}
	
	m_pAcCommObj = new CACMainObject(); // 创建通信对象
	//m_pHeadInfo = new CACPackHead(); //创建包头处理对象
	if(m_bHeadFlag) //add 2012-10-18
	{
		m_pHeadInfo = new CSockHeader(); //创建包头处理对象
	}
	if(NULL != pSockEventCallBack)
	{
		m_pCallBack = pSockEventCallBack;
	}

	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, SOCK_STREAM, 1, 1, m_pCallBack, m_pHeadInfo);

	// 启动通信对象
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
	* 创建一个TCP socket
	* @param ip socket要连接到的IP
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
int CTCPConnector::create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create(ip_to_string(ip),port,pSockEventCallBack);
}

	/**
	* 关闭TCP socket
	* @return 成功则返回0
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
	* 通过TCP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
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
	* 返回TCP socket底层的FD
	* @return 不成功则返回-1
	*/
ACE_HANDLE CTCPConnector::getSock() const
{
	//if (m_pAcCommObj != NULL)
		//return m_pAcCommObj->GetSocket();
	//return -1;
	return 0;
}

#if 0
/** 启动通信对象(不使用代理)
* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost 本机IP
* @param nLocalPort 本机端口
* @param lpszSvrAddr 服务器IP
* @param nSvrPort 服务器端口 	
* @param bUseHttp 是否使用HTTP协议(当用HTTP代理或HTTP协议登陆后,
建立点对点连接时,如双方在同一局域网,则为FALSE,其它情况为TRUE)	
* @return 0-成功, <0失败
*/
int CTCPConnector::Start(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpszSvrAddr, USHORT nSvrPort, BOOL bUseHttp,BOOL bMulPoint)
{
	//test function point
	assert(CreateACObject != NULL);

	// 关闭已打开的通信对象
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	
//	m_pAcCommObj = ::CreateACObject(); // 创建通信对象
	m_pAcCommObj = new CACMainObject(); // 创建通信对象
	//m_pHeadInfo = new CACPackHead(); //创建包头处理对象

	// 初始化通信对象
	if (bUseHttp==TRUE && wProtoType==SOCK_STREAM) //用HTTP协议
	{
		m_pAcCommObj->InitHttp(m_pCallBack, m_pHeadInfo);
	}
	else //不用HTTP协议
	{
		m_pAcCommObj->Initialize(COMMTYPE_CLIENT, wProtoType, 
			1, 1, m_pCallBack, m_pHeadInfo);
	}

	// 启动通信对象
	int nRet=m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, 
		lpszSvrAddr, nSvrPort,bMulPoint);
	if ( nRet!= 0)
	{
		FreeObject();
		return nRet;
	}

	return 0;
}

/** 启动通信对象(使用SOCK5代理)
* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost 本机IP
* @param nLocalPort 本机端口
* @param lpszSvrAddr 服务器IP
* @param nSvrPort 服务器端口 	
* @param lpProxyAddr 代理服务器IP 
* @param nProxyPort 代理服务器端口
* @param lpProxyUserName 代理用户名
* @param lpProxyPassword 代理密码
* @return 0-成功, <0失败
*/
int CTCPConnector::StartSock5(WORD wProtoType, LPCTSTR lpLocalHost, 
	USHORT nLocalPort, LPCTSTR lpszSvrAddr, USHORT nSvrPort, 
	LPCTSTR lpProxyAddr, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword)
{
	assert(CreateACObject != NULL);

	// 关闭已打开的通信对象
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	m_pAcCommObj = ::CreateACObject(); // 创建通信对象
	m_pHeadInfo = new CACPackHead(); //创建包头处理对象

	// 初始化通信对象
	m_pAcCommObj->Initialize(COMMTYPE_CLIENT, wProtoType, 1, 1, 
		m_pCallBack, m_pHeadInfo);

	// 启动通信对象
	if (m_pAcCommObj->BeginWork(lpLocalHost, nLocalPort, 
		lpszSvrAddr, nSvrPort, lpProxyAddr, nProxyPort, 
		lpProxyUserName, lpProxyPassword) != 0)
	{
		FreeObject();
		return -1;
	}

	return 0;
}

/** 启动通信对象(使用HTTP代理)
* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
* @param lpLocalHost 本机IP
* @param nLocalPort 本机端口
* @param lpszSvrAddr 服务器IP
* @param nSvrPort 服务器端口 	
* @param lpProxyAddr 代理服务器IP 
* @param nProxyPort 代理服务器端口
* @param lpProxyUserName 代理用户名
* @param lpProxyPassword 代理密码
* @return 0-成功, <0失败
*/
int CTCPConnector::StartHttp(WORD wProtoType, LPCTSTR lpLocalHost, 
	USHORT nLocalPort, LPCTSTR lpszSvrAddr, USHORT nSvrPort, 
	LPCTSTR lpProxyAddr, USHORT nProxyPort, LPCTSTR lpProxyUserName, 
	LPCTSTR lpProxyPassword)
{
	assert(CreateACObject != NULL);

	// 关闭已打开的通信对象
	if (m_pAcCommObj != NULL)
	{
		Stop();
	}

	m_pAcCommObj = ::CreateACObject(); // 创建通信对象
	m_pHeadInfo = new CACPackHead(); //创建包头处理对象

	// 初始化通信对象
	m_pAcCommObj->InitHttp(m_pCallBack, m_pHeadInfo);

	// 启动通信对象
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

/** 停止通信对象
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
int CTCPConnector::SendCommUDPData(WORD wPduType, const BYTE* lpData, 
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
int CTCPConnector::SendCommTCPData(WORD wPduType, const BYTE* lpData, 
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
//LPCTSTR CTCPConnector::GetLocalHost(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalHost();
//}
//
///** 获取本机子网掩码
//*/
//LPCTSTR CTCPConnector::GetLocalMask(void) const
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalMask();
//}
//
///** 获取本地端口
//*/
//USHORT CTCPConnector::GetLocalPort(void)
//{
//	assert(m_pAcCallBack != NULL);
//
//	return m_pAcCallBack->GetLocalPort();
//}
//
///** 获取通信对象的套接字句柄
//*/
//SOCKET CTCPConnector::GetSocket(void)
//{
//	assert(m_pAcCallBack != NULL);
//	return m_pAcCallBack->GetSocket();
//}

/** 停止通信对象实现线程
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

