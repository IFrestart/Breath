/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPAcceptor.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short	被动TCP类
 **/

#include <iostream>
#include <algorithm>

template <class SVCHANDLER>
CTCPAcceptorTpl<SVCHANDLER>::CTCPAcceptorTpl(ISockEventCallBack* pSockEventCallBack)
				: pSockEventCallBack_(pSockEventCallBack),
				timerHandler_(this)
{
}

template <class SVCHANDLER>
CTCPAcceptorTpl<SVCHANDLER>::~CTCPAcceptorTpl()
{
	close();
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::create(unsigned short port,
						ISockEventCallBack* pSockEventCallBack)
{
	//_INFO("CTCPAcceptor::create() ...port = %d", port);

	return  create(ACE_INET_Addr(port), pSockEventCallBack);
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::create(const char* ipstr, unsigned short port,
						ISockEventCallBack* pSockEventCallBack)
{
	//_INFO("CTCPAcceptor::create() ...ipstr = %s, port = %d", ipstr, port);

	return  create(ACE_INET_Addr(port, ipstr), pSockEventCallBack);
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::create(unsigned long ip, unsigned short port,
						ISockEventCallBack* pSockEventCallBack)
{
	//_INFO("CTCPAcceptor::create() ...ipstr = %lx, port = %d", ip, port);

	return  create(ACE_INET_Addr(port, ip), pSockEventCallBack);
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::create(const ACE_INET_Addr& addr,
						ISockEventCallBack* pSockEventCallBack)
{
	_INFO("CTCPAcceptor::create() ...addr");

	if(0 == pSockEventCallBack && 0 == pSockEventCallBack_) {
	        _INFO("CTCPAcceptor::create() ...-1");
		return -1;
	}

	if(ACE_INVALID_HANDLE != getSock()) {
		return -2;
	}

	int	ret = PARENT::open(addr, ACE_Reactor::instance(), ACE_NONBLOCK);
	if(0 == ret) {
		if(pSockEventCallBack) {
			pSockEventCallBack_ = pSockEventCallBack;
		}
		for(int i = 0; i < SVC_HANDLER_NUM; i ++) {
			unusedQueue_.push(i);
		}
	}

	return ret;
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::closeListen()
{
	//_INFO("CTCPAcceptor::closeListen() ...");

	if(ACE_INVALID_HANDLE == getSock()) {
		return -1;
	}

	return PARENT::close();
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::close()
{
	//_INFO("CTCPAcceptor::close() ...");

	if(ACE_INVALID_HANDLE == getSock()) {
		return -1;
	}

	for(svcpair_it it = svcpairList_.begin(); it != svcpairList_.end();) {
		svcpair_it	tit = it;
		it ++;
		close(tit->handler_->get_handle());
		//svcpairList_.erase(tit);
	}

	return PARENT::close();
}

template <class SVCHANDLER>
ACE_HANDLE CTCPAcceptorTpl<SVCHANDLER>::getSock() const
{
	//_INFO("CTCPAcceptor::getSock() ...");

	return PARENT::get_handle();
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::make_svc_handler(SVCHANDLER*& sh)
{
	//_INFO("CTCPAcceptor::make_svc_handler() ...");

	int	ret = -1;

	if(! unusedQueue_.empty()) {
		int	ii = unusedQueue_.front();
		unusedQueue_.pop();
		//_INFO("   pop() = %d", ii);
		sh = &svcHandlerList_[ii];
		sh->initialize(this);
		sh->reactor(PARENT::reactor());
		ret = 0;
	}

	return ret;
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::sendPacket(ACE_HANDLE sock,
					unsigned short pdutype, 
					const void* buf, unsigned long len)
{
	//_INFO("CTCPAcceptor::sendPacket() ...sock = %d, pdutype = %d, len = %d", sock, pdutype, len);

	svcpair_it	it = std::find(svcpairList_.begin(), svcpairList_.end(), sock);

	int	ret = -1;
	if(svcpairList_.end() != it) {
		ret = it->handler_->sendPacket(pdutype, buf, len);
	}

	return ret;
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::close(ACE_HANDLE sock)
{
	//_INFO("CTCPAcceptor::close() ...sock = %d", sock);

	svcpair_it	it = std::find(svcpairList_.begin(), svcpairList_.end(), sock);

//	ACE_ASSERT(svcpairList_.end() != it);
//add by zjj 2006-8-16
	if(it == svcpairList_.end())
	{
//		std::cerr << "close ACE_ASSERT(svcpairList_.end() != it)" << std::endl;
		return 0;
	}
//end add by zjj 2006-8-16
	return it->handler_->handle_close(sock);
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::OnSvcOpen(void* arg)
{
	//_INFO("CTCPAcceptor::OnSvcOpen() ...");

	if(svcpairList_.empty()) {
		timerHandler_.start();
	}

	SVCHANDLER*	sh = (SVCHANDLER*)arg;
	svcpairList_.push_back(SvcPair(sh, time(0)));

	if(pSockEventCallBack_) {
		pSockEventCallBack_->OnNewConnection(sh->get_handle()); 
	}

	int	ret = 0;
	if(unusedQueue_.empty()) {
		ret = -1;
	}

	return ret;
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::OnPacketReceived(void* arg, unsigned short pduType, const void* data, unsigned long dataLen)
{
	ACE_HANDLE	sock = ((SVCHANDLER*)arg)->get_handle();

	//_INFO("CTCPAcceptor::OnPacketReceived() ...sock = %d, pduType = %d, len = %d",
	//		sock, pduType, dataLen);

	if(1 != pduType && pSockEventCallBack_) {
		pSockEventCallBack_->OnTCPPacketReceived(sock, pduType, data, dataLen); 
	}

	svcpair_it	it = std::find(svcpairList_.begin(), svcpairList_.end(), sock);
//	ACE_ASSERT(svcpairList_.end() != it);
//add by zjj 2006-8-16
	if(it == svcpairList_.end())
	{
//		std::cerr << "OnPacketReceived ACE_ASSERT(svcpairList_.end() != it)" << std::endl;
		return 0;
	}
//end add by zjj 2006-8-16
	it->lasttime_ = time(0);

//add by zjj 2006-6-28
/*  modify 2014-4-24
	if(1 == pduType)
	{	
		//it->handler_->sendPacket(19999, 0, 0); //modify 2014-4-24
	}
*/
	

	return 0;
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::OnSvcClose(void* arg)
{
	//_INFO("CTCPAcceptor::OnSvcClose() ...");

	ACE_ASSERT(arg);
	SVCHANDLER*	sh = (SVCHANDLER*)arg;
	ACE_HANDLE	sock = sh->get_handle();

	if(ACE_INVALID_HANDLE != sock) {
		if(pSockEventCallBack_) {
			pSockEventCallBack_->OnConnectionClosed(sock);
		}

		svcpair_it	it = std::find(svcpairList_.begin(), svcpairList_.end(), sock);
//modified 2006-7-24
		//ACE_ASSERT(svcpairList_.end() != it); //modified 2006-7-24
		if(it == svcpairList_.end())
		{
//			std::cerr << "OnSvcClose ACE_ASSERT(svcpairList_.end() != it)" << std::endl;
			return 0;
		}
//end modified
		svcpairList_.erase(it);
	}

	if(svcpairList_.empty()) {
		timerHandler_.kill();
	}

	unusedQueue_.push(sh - svcHandlerList_);

	return 0;
}

/**
 * TimerHandler
 */
template <class SVCHANDLER>
CTCPAcceptorTpl<SVCHANDLER>::TimerHandler::TimerHandler(CTCPAcceptorTpl<SVCHANDLER>* pServ)
		: pAcceptor(pServ)
{
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::TimerHandler::start()
{
	return checkTimer_.start(CHECK_TIMER_ID, CHECK_TIMEOUT_INTERVAL, this, 0);
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::TimerHandler::kill()
{
	return checkTimer_.kill();
}

template <class SVCHANDLER>
int CTCPAcceptorTpl<SVCHANDLER>::TimerHandler::OnTimer(unsigned long timerID, void* arg)
{
	ACE_ASSERT(timerID == CHECK_TIMER_ID);

	unsigned long	now = time(0);
	for(svcpair_it it = pAcceptor->svcpairList_.begin(); it != pAcceptor->svcpairList_.end(); ) {
		if(now - it->lasttime_ > CLIENT_TIMEOUT_INTERVAL) {
			svcpair_it	tit = it;
			it ++;
			pAcceptor->close(tit->handler_->get_handle()); //temp hide
		}
		else {
			it ++;
		}
	}

	return 0;
}

/**
 * TimerHandler
 */
template <class SVCHANDLER>
CTCPAcceptorTpl<SVCHANDLER>::SvcPair::SvcPair(SVCHANDLER* h, unsigned long lt)
	: handler_(h), lasttime_(lt)
{
}

template <class SVCHANDLER>
bool CTCPAcceptorTpl<SVCHANDLER>::SvcPair::operator== (ACE_HANDLE sock)
{
	return handler_->get_handle() == sock;
}

