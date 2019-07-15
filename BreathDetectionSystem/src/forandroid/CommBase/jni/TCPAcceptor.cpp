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

/** 构造函数
*/
CTCPAcceptor::CTCPAcceptor(ISockEventCallBack* pCallBack):m_pCallBack(pCallBack)
{
	m_pSvrObj = NULL;
	m_pHeadInfo = NULL;
	m_bHeadFlag = true; //add 2012-10-18
}

/** 析构函数
*/
CTCPAcceptor::~CTCPAcceptor(void)
{
	close();
}

	/**
	* 创建一个TCP socket
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
int CTCPAcceptor::create(unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create((const char*)NULL,port,pSockEventCallBack);
}

	/**
	* 创建一个TCP socket
	* @param ipstr socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
int CTCPAcceptor::create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack )
{
	// 关闭已打开的通信对象
	if (m_pSvrObj != NULL)
	{
		Stop();
	}

	//m_pSvrObj = ::CreateACObject(); // 创建通信对象
	m_pSvrObj = new CACMainObject(); // 创建通信对象
	//m_pHeadInfo = new CACPackHead(); //创建包头处理对象
	if(m_bHeadFlag) //add 2012-10-18
	{
		m_pHeadInfo = new CSockHeader(); //创建包头处理对象
	}
	if(NULL != pSockEventCallBack)
	{
		m_pCallBack = pSockEventCallBack;
	}

	// 初始化通信对象
#ifdef _WIN32
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 10, 20, 
		m_pCallBack, m_pHeadInfo);
#else
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 2, 4, 
		m_pCallBack, m_pHeadInfo);
#endif

	// 启动通信对象
	if (m_pSvrObj->BeginWork(ipstr, port, NULL, 0) != 0)
	{
		Stop();
		return -1;
	}

	return 0;
}

	/**
	* 创建一个TCP socket
	* @param ip socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
int CTCPAcceptor::create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack)
{
	return create(ip_to_string(ip),port,pSockEventCallBack);
}

	/**
	* 关闭监听的socket及所有acceptor出来的socket
	* @return 成功则返回0
	*/
int CTCPAcceptor::close()
{
	Stop();
	return 0; //zzzz
}

	/**
	* 关闭监听的socket
	* @return 成功则返回0
	*/
int CTCPAcceptor::closeListen()
{
	Stop();
	return 0; //zzzz
}

	/**
	* 关闭指定的acceptor出来的socket
	* @param sock 要关闭的socket值
	* @return 成功则返回0
	*/
int CTCPAcceptor::close(ACE_HANDLE sock)
{
	if(NULL == m_pSvrObj)
		return -1;
	m_pSvrObj->CloseConnection(sock); 
	return 0;
}

	/**
	* 取得TCP socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为TCP socket
	*/
ACE_HANDLE CTCPAcceptor::getSock() const
{
	return 0;//zzzz
}

	/**
	* 通过TCP socket发送数据
	* @param sock 将数据发送到哪个sock,这个sock应该是由
	* 这个TCPAcceptor对象accept()出来的
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
int CTCPAcceptor::sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len)
{
	if((INVALID_SOCKET == sock) ||(NULL == m_pSvrObj))
		return -1;
	return m_pSvrObj->SendCommTCPData(pdutype, (const BYTE*)data, len, TRUE, sock);
}
#if 0
/** 启动通信对象
* @param lpLocalHost 本机IP
* @param nLocalPort 本机端口
* @param lpMainObj 通信层入口类的指针
* @return 0-成功, <0失败
*/
int CTCPAcceptor::Start(USHORT nLocalPort, LPCTSTR lpLocalHost)
{
	assert(CreateACObject != NULL);

	// 关闭已打开的通信对象
	if (m_pSvrObj != NULL)
	{
		Stop();
	}

	//m_pSvrObj = ::CreateACObject(); // 创建通信对象
	m_pSvrObj = CreateACObject(); // 创建通信对象
	//m_pHeadInfo = new CACPackHead(); //创建包头处理对象
	m_pHeadInfo = new CSockHeader(); //创建包头处理对象

	// 初始化通信对象
#ifdef _WIN32
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 10, 20, 
		m_pCallBack, m_pHeadInfo);
#else
	m_pSvrObj->Initialize(COMMTYPE_SERVER, SOCK_STREAM, 2, 4, 
		m_pCallBack, m_pHeadInfo);
#endif

	// 启动通信对象
	if (m_pSvrObj->BeginWork(lpLocalHost, nLocalPort, NULL, 0) != 0)
	{
		Stop();
		return -1;
	}

	return 0;
}


/** 发送数据
* @param wPduType 数据包的PDU类别
* @param lpData 要发送的数据
* @param dwSize 数据长度
* @param hSocket 发送数据的套接字句柄
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

/** 关闭通信连接
* @param hSocket 要关闭的会话的套接字句柄
*/

/** 停止侦听对象
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

