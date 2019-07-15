/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPConnector.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short	被动TCP类
 **/

#include <iostream>
#include "ace/SOCK_Connector.h"
#include "ace/Connector.h"

#include "TCPConnector.h"


template <class SVCHANDLER>
CTCPConnectorTpl<SVCHANDLER>::CTCPConnectorTpl(ISockEventCallBack* pSockEventCallBack)
				: PARENT(ACE_Reactor::instance(), ACE_NONBLOCK),
				pSockEventCallBack_(pSockEventCallBack),
				timerHandler_(this)
{
}

template <class SVCHANDLER>
CTCPConnectorTpl<SVCHANDLER>::~CTCPConnectorTpl()
{
	close();
}


template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::create(const char* ipstr, unsigned short port, 
				ISockEventCallBack* pSockEventCallBack)
{
	return  create(ACE_INET_Addr(port, ipstr), pSockEventCallBack);
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::create(unsigned long ip, unsigned short port, 
				ISockEventCallBack* pSockEventCallBack)
{
	return  create(ACE_INET_Addr(port, ip), pSockEventCallBack);
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::create(const ACE_INET_Addr& addr,
				ISockEventCallBack* pSockEventCallBack)
{
	if(0 == pSockEventCallBack && 0 == pSockEventCallBack_) {
		return -1;
	}

	SVCHANDLER*	h = 0;
//modify by zjj 2006-7-2
	ACE_Time_Value timeout(1);
	ACE_Synch_Options options(ACE_Synch_Options::USE_TIMEOUT,timeout);
	int	ret = PARENT::connect(h, addr,options);
//end modify 
//	int	ret = PARENT::connect(h, addr);
	if(0 == ret) {
		if(pSockEventCallBack) {
			pSockEventCallBack_ = pSockEventCallBack;
		}
	}

	return ret;
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::close()
{
	PARENT::close();
	ACE_HANDLE	sock = getSock();
	if(ACE_INVALID_HANDLE != sock) {
		PARENT::reactor()->remove_handler(sock, ACE_Event_Handler::READ_MASK);
	}

	return 0;
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::sendPacket(unsigned short pdutype, const void* buf, unsigned long len)
{
//add by zjj
	ACE_HANDLE	sock = getSock();
	if(ACE_INVALID_HANDLE != sock) {
		return handler_.sendPacket(pdutype, buf, len);
	}
	else
	{
	//	std::cerr << "in CTCPConnectorTpl sendPacket pdutype = " << pdutype << " sendPacket ACE_INVALID_HANDLE sock " << std::endl;
	}
	return 0;
}

template <class SVCHANDLER>
ACE_HANDLE CTCPConnectorTpl<SVCHANDLER>::getSock() const
{
	return handler_.get_handle();
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::make_svc_handler(SVCHANDLER*& sh)
{
	//_INFO("CTCPConnector::make_svc_handler() ...");

	sh = &handler_;
	sh->initialize(this);
	sh->reactor(PARENT::reactor());

	return 0;
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::OnSvcOpen(void* arg)
{
	//_INFO("CTCPConnector::OnSvcOpen() ...");

	ACE_ASSERT(&handler_ == (SVCHANDLER*)arg);

	if(pSockEventCallBack_) {
		pSockEventCallBack_->OnNewConnection(handler_.get_handle());
	}

	lasttime_ = time(0);//add by zjj 2006-6-28
	timerHandler_.start();

	return 0;
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::OnPacketReceived(void* arg, unsigned short pduType, const void* data, unsigned long dataLen)
{
	ACE_ASSERT(&handler_ == (SVCHANDLER*)arg);
//modify by zjj 2006-6-28
//	if(2 != pduType && pSockEventCallBack_) {
	if((2 != pduType) && (19999 != pduType) && pSockEventCallBack_) {
		pSockEventCallBack_->OnTCPPacketReceived(handler_.get_handle(), pduType, data, dataLen); 
	}
//add by zjj
	lasttime_ = time(0);	

	return 0;
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::OnSvcClose(void* arg)
{
	//_INFO("CTCPConnector::OnSvcClose() ...");

	//ACE_ASSERT(&handler_ == (SVCHANDLER*)arg);//modify by zjj

	ACE_HANDLE	sock = handler_.get_handle();
	if(ACE_INVALID_HANDLE != sock) {
		if(pSockEventCallBack_) {
			pSockEventCallBack_->OnConnectionClosed(handler_.get_handle());
		}
		timerHandler_.kill();
	}

	return 0;
}

/**
 * TimerHandler
 */
template <class SVCHANDLER>
CTCPConnectorTpl<SVCHANDLER>::TimerHandler::TimerHandler(CTCPConnectorTpl<SVCHANDLER>* pServ)
		: pConnector(pServ)
{
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::TimerHandler::start()
{
	return keepAliveTimer_.start(KEEPALIVE_TIMER_ID, KEEPALIVE_INTERVAL, this, 0);
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::TimerHandler::kill()
{
	return keepAliveTimer_.kill();
}

template <class SVCHANDLER>
int CTCPConnectorTpl<SVCHANDLER>::TimerHandler::OnTimer(unsigned long timerID, void* arg)
{
	assert(timerID == KEEPALIVE_TIMER_ID);
//add by zjj 2006-6-28
/* //modify 2014-4-24
	unsigned long   now = time(0);
	unsigned long old = pConnector->getLasttime();
	if( now - old > CLIENT_TIMEOUT_INTERVAL)
	{
	//	std::cerr << "error now - lasttime_ > CLIENT_TIMEOUT_INTERVAL sock " << pConnector->getSock() << std::endl;
	//	pConnector->close();
	//	kill();
//		pConnector->OnSvcClose(NULL);
		return 0;
	}
*/
//end add
	pConnector->sendPacket(1, 0, 0);

	return 0;
}

