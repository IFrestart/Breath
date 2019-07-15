/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPAcceptor.h
 * @short	被动TCP类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPMTAcceptorTpl
 * @short	被动TCP类
 **/

#ifndef	__TCPMTACCEPTOR_H__
#define	__TCPMTACCEPTOR_H__

#include <list>
#include <queue>

#include "ace/SOCK_Acceptor.h"
#include "ace/Acceptor.h"

#include "Timer.h"
#include "SvcManager.h"
#include "SockEventCallBack.h"
#include "TCPSvcHandler.h"

template <class SVCHANDLER>
class CTCPMTAcceptorTpl : public ISvcManager, public ACE_Acceptor<SVCHANDLER, ACE_SOCK_ACCEPTOR>
{
public:
	CTCPMTAcceptorTpl(ISockEventCallBack* pSockEventCallBack = 0);
	/**
	* 实现为空,会自动调用ACE_Acceptor的析构,
	* 进而调用handle_close() 
	*/
	~CTCPMTAcceptorTpl();

	/**
	* 创建一个TCP socket
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 创建一个TCP socket
	* @param ipstr socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack = 0);

	/**
	* 创建一个TCP socket
	* @param ip socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 关闭监听的socket及所有acceptor出来的socket
	* @return 成功则返回0
	*/
	int close();

	/**
	* 关闭监听的socket
	* @return 成功则返回0
	*/
	int closeListen();

	/**
	* 关闭指定的acceptor出来的socket
	* @param sock 要关闭的socket值
	* @return 成功则返回0
	*/
	int close(ACE_HANDLE sock);

	/**
	* 取得TCP socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为TCP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* 通过TCP socket发送数据
	* @param sock 将数据发送到哪个sock,这个sock应该是由
	* 这个TCPAcceptor对象accept()出来的
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
	int sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len);

protected:
	/**
	* 继承自ISvcManager
	*/
	virtual int OnSvcOpen(void*);
	virtual int OnPacketReceived(void*, unsigned short, const void*, unsigned long);
	virtual int OnSvcClose(void*);

	/**
	* 继承自ACE_Acceptor<>
	* @see ACE_Acceptor<>::make_svc_handler()
	*/
	virtual int make_svc_handler(SVCHANDLER*& sh);

private:
	/**
	 * @class	TimerHandler
	 * @short	定时器处理类，定时检查连接，将一段时间没有收到数据的连接关闭
	 */
	class TimerHandler : public ITimerEventCallBack
	{
	public:
		TimerHandler(CTCPMTAcceptorTpl<SVCHANDLER>*);
		int start();
		int kill();

	private:
		/* From ITimerEventCallBack */
		virtual int OnTimer(unsigned long timerID, void* arg);

	private:
		//enum {CHECK_TIMER_ID = 1, CHECK_TIMEOUT_INTERVAL = 5000, CLIENT_TIMEOUT_INTERVAL = 30};
		enum {CHECK_TIMER_ID = 1, CHECK_TIMEOUT_INTERVAL = 5000, CLIENT_TIMEOUT_INTERVAL = 120}; //modify 2013-4-3
		CTimer		checkTimer_;
		CTCPMTAcceptorTpl<SVCHANDLER>*	pAcceptor;
	};

	/**
	 * @class	SvcPair
	 * @short	svc_handler与其最后一次收到数据的时间 对
	 */
	struct SvcPair{
		SvcPair(SVCHANDLER*, unsigned long);
		bool operator== (ACE_HANDLE);

		SVCHANDLER*	handler_;
		unsigned long	lasttime_;
	};
	typedef typename std::list<SvcPair>::iterator	svcpair_it;
	typedef ACE_Acceptor<SVCHANDLER, ACE_SOCK_ACCEPTOR> PARENT;

	int create(const ACE_INET_Addr& addr, ISockEventCallBack* pSockEventCallBack);

	ISockEventCallBack*	pSockEventCallBack_;
	std::list<SvcPair>	svcpairList_;
	TimerHandler		timerHandler_;

	enum {SVC_HANDLER_NUM = 1000};
	SVCHANDLER		svcHandlerList_[SVC_HANDLER_NUM];
	std::queue<int>		unusedQueue_;
	
	friend class TimerHandler;
public:
	CMutex			mutex_;
};

#include "../TCPMTAcceptor.cc"
typedef CTCPMTAcceptorTpl<CTCPSvcHandler>	CTCPMTAcceptor;
#endif	/*__TCPMTACCEPTOR_H__*/

