/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPConnector.h
 * @short	被动TCP类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPConnectorTpl
 * @short	被动TCP类
 **/

#ifndef	__TCPCONNECTOR_H__
#define	__TCPCONNECTOR_H__

#include "ace/SOCK_Connector.h"
#include "ace/Connector.h"

#include "Timer.h"
#include "SvcManager.h"
#include "SockEventCallBack.h"
#include "TCPSvcHandler.h"

template <class SVCHANDLER>
class CTCPConnectorTpl : public ISvcManager, public ACE_Connector<SVCHANDLER,ACE_SOCK_CONNECTOR>
{
public:
	CTCPConnectorTpl(ISockEventCallBack* = 0);
	/**
	* 实现为空,会自动调用ACE_Connector的析构,
	* 进而调用SVC_HANDLER::close() 
	* 进而调用SVC_HANDLER::handle_close() 
	*/
	~CTCPConnectorTpl();

	/**
	* 创建一个TCP socket
	* @param ipstr socket要连接到的IP,字符串
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 创建一个TCP socket
	* @param ip socket要连接到的IP
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 关闭TCP socket
	* @return 成功则返回0
	*/
	int close();

	/**
	* 通过TCP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	/**
	* 返回TCP socket底层的FD
	* @return 不成功则返回-1
	*/
	ACE_HANDLE getSock() const;

protected:
	/**
	* 继承自ISvcManager
	*/
	virtual int OnSvcOpen(void* arg);
	virtual int OnPacketReceived(void*, unsigned short, const void*, unsigned long);
	virtual int OnSvcClose(void* arg);

	/**
	* 继承自ACE_Connector<>
	* @see ACE_Connector<>::make_svc_handler()
	*/
	virtual int make_svc_handler(SVCHANDLER*& sh);
	unsigned long getLasttime(){return lasttime_;}//add by zjj

private:
	/**
	 * @class	TimerHandler
	 * @short	定时器处理类，定时发送KeepAlive
	 */
	class TimerHandler : public ITimerEventCallBack
	{
	public:
		TimerHandler(CTCPConnectorTpl<SVCHANDLER>*);
		int start();
		int kill();

	private:
		/* From ITimerEventCallBack */
		virtual int OnTimer(unsigned long timerID, void* arg);

	private:
	//	enum {KEEPALIVE_TIMER_ID = 1, KEEPALIVE_INTERVAL = 5000,CLIENT_TIMEOUT_INTERVAL = 120};
		enum {KEEPALIVE_TIMER_ID = 1, KEEPALIVE_INTERVAL = 60000,CLIENT_TIMEOUT_INTERVAL = 300}; //modif 2013-4-3
		CTimer		keepAliveTimer_;
		CTCPConnectorTpl<SVCHANDLER>*	pConnector;
	};

	int create(const ACE_INET_Addr&, ISockEventCallBack* pSockEventCallBack);
	typedef ACE_Connector<SVCHANDLER,ACE_SOCK_CONNECTOR> PARENT;

	ISockEventCallBack*	pSockEventCallBack_;
	SVCHANDLER		handler_;
	TimerHandler		timerHandler_;
	unsigned long	lasttime_; //add by zjj 2006-6-28

	friend class TimerHandler;
};

#include "../TCPConnector.cc"

typedef CTCPConnectorTpl<CTCPSvcHandler>	CTCPConnector;

#endif	/*__TCPCONNECTOR_H__*/

