/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP连接数据接收与发送处理类,提供了TCP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	TCPSvcHandler.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short	TCP连接数据接收与发送处理类
 **/

#include <iostream>

#include "Logger.h"
#include "SvcManager.h"
#include "TCPSvcHandler.h"

template <class SOCKHEADER>
CTCPSvcHandlerTpl<SOCKHEADER>::CTCPSvcHandlerTpl()
						: pSvcManager_(0),
						isBegin_(false), mask_(0), mbRecv_(0), mbSend_(0)
{
}

template <class SOCKHEADER>
CTCPSvcHandlerTpl<SOCKHEADER>::~CTCPSvcHandlerTpl()
{
}

template <class SOCKHEADER>
void CTCPSvcHandlerTpl<SOCKHEADER>::initialize(ISvcManager* psm)
{
	//_INFO("CTCPSvcHandler::initialize() ...");
	closing_ = 0;

	pSvcManager_ = psm;
	isBegin_ = false;
	mask_ = 0;

	if(mbRecv_) {
		mbRecv_->release();
		delete mbRecv_;
		mbRecv_ = 0;
	}
	if(mbSend_) {
		mbSend_->release();
		delete mbSend_;
		mbSend_ = 0;
	}
}

template <class SOCKHEADER>
int CTCPSvcHandlerTpl<SOCKHEADER>::open(void* args)
{
	//_INFO("CTCPSvcHandler::open() ...");

	int		ret = PARENT::open(args);
	if(0 == ret) {
		if(HIGWATERMARK != blockQueue_.high_water_mark()) {
			blockQueue_.high_water_mark(HIGWATERMARK);
		}
		mask_ |= READMASK;

		if(pSvcManager_) {
			ret = pSvcManager_->OnSvcOpen(this);
		}
		//add 2011-8-3 by zjj 
		int nSocketBuffSize =  ACE_DEFAULT_MAX_SOCKET_BUFSIZ;
		// 修改接收缓冲区 
		int nRet = peer().set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int)); 
		//std::cout << "CTCPSvcHandlerTpl<SVCHANDLER>::open set_option rcvbuf ret[" << nRet << "] " << std::endl;

		// 修改发送缓冲区 
		nRet = peer().set_option(SOL_SOCKET, SO_SNDBUF, &nSocketBuffSize, sizeof(int)); 
		//std::cout << "CTCPSvcHandlerTpl<SVCHANDLER>::open set_option sendbuf ret[" << nRet << "] " << std::endl;

		//and add 
	}

	return ret;
}

template <class SOCKHEADER>
int CTCPSvcHandlerTpl<SOCKHEADER>::handle_input(ACE_HANDLE h)
{
	//_INFO("CTCPSvcHandler::handle_input() ...sock = %d", h);

	static const unsigned short	headerSize = sockHeaderCallBack_.GetPacketHeaderSize();

	if(!isBegin_) {
		isBegin_ = true;

		isHeader_ = true;
		mbRecv_ = new CSockBlock(headerSize);
		wantLen_ = headerSize;
	}

	int		ret = 0;
	while(wantLen_) {
		ret = peer().recv(mbRecv_->wr_ptr(), wantLen_);
		if(0 == ret) {
			//_ERROR("TCP  Recv(): Peer(sock=%4d) is Closed!!!", PARENT::get_handle());
			if(mbRecv_) {
				mbRecv_->release();
				delete mbRecv_;
				mbRecv_ = 0;
			}
			isBegin_ = false;
			return -1;
		}
		else
		if(-1 == ret) {
#ifdef __LINUX__            //modfiy 2011-8-5
			if(EAGAIN == errno || EINTR == errno) {
#else
			if(EAGAIN == errno || EINTR == errno || ACE_OS::last_error() == WSAEWOULDBLOCK) {			 
#endif
				//_ERROR("TCP  Recv() body: Peer(sock=%4d) is Error(=%d),do next.", PARENT::get_handle(), errno);
				return 0;
			}
			else {
				//_ERROR("TCP  Recv(): Peer(sock=%4d) is Error(=%d)!!!", PARENT::get_handle(), errno);
				//_ERROR("TCP  Recv(): Peer(sock=%4d) is Error(=%d)!!!", PARENT::get_handle(), ACE_OS::last_error());
				if(mbRecv_) {
					mbRecv_->release();
					delete mbRecv_;
					mbRecv_ = 0;
				}
				isBegin_ = false;
				return -1;
			}
		}
		else {
			//_INFO("TCP  Recv(): Peer(sock=%4d) ret (=%d)...", PARENT::get_handle(), ret);
			mbRecv_->wr_ptr(ret);
			wantLen_ -= ret;
		}
	}

	ret = 0;
	if(isHeader_) {
		isHeader_ = false;

		if(0 != sockHeaderCallBack_.ParsePacketHeader(mbRecv_->base(), pduType_, wantLen_)) {
			//_ERROR("TCP  ParsePacketHeader(): sock=%4d Error!!!", PARENT::get_handle());

			if(mbRecv_) {
				mbRecv_->release();
				delete mbRecv_;
				mbRecv_ = 0;
			}
			return -1;
		}
		if (mbRecv_)		//add 2011-7-8
		{
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;

		}
		if(0 != wantLen_) {
			mbRecv_ = new CSockBlock(wantLen_);
		}
	}

	if(0 == wantLen_) {
		if(pSvcManager_) {
			//_INFO("TCP  OnTCPPacketReceived(): sock=%4d pdu=%5d len=%5d...", PARENT::get_handle(), pduType_, mbRecv_ ? mbRecv_->size() : 0);
			ret = pSvcManager_->OnPacketReceived(this, pduType_, 
							mbRecv_ ? mbRecv_->base() : 0, 
							mbRecv_ ? mbRecv_->size() : 0);

		}
		if(mbRecv_) {
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;
		}
		isBegin_ = false;
	}

	return ret == 0 ? 0 : -1;
}

template <class SOCKHEADER>
int CTCPSvcHandlerTpl<SOCKHEADER>::handle_close(ACE_HANDLE h, ACE_Reactor_Mask m)
{
	//_INFO("CTCPSvcHandler::handle_close() ...sock = %d, m = %d", h, m);

	if(ACE_Event_Handler::READ_MASK & m) {
		mask_ &= ~READMASK;
		if(mbRecv_) {
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;
			isBegin_ = false;
		}
	}

	if(ACE_Event_Handler::WRITE_MASK & m) {
		mask_ &= ~WRITEMASK;
		if(mbSend_) {
			mbSend_->release();
			delete mbSend_;
			mbSend_ = 0;
		}
	}

	int		ret = 0;
	if(0 == mask_) {
		//_INFO("TCP  handle_close(): sock=%4d...", PARENT::get_handle());

		if(pSvcManager_) {
			pSvcManager_->OnSvcClose(this);
		}

		PARENT::handle_close(h, m);
	}

	return ret;
}

template <class SOCKHEADER>
void CTCPSvcHandlerTpl<SOCKHEADER>::destroy()
{
	//_INFO("CTCPSvcHandler::destroy() ... sock = %d", PARENT::get_handle());

	shutdown();
}

template <class SOCKHEADER>
int CTCPSvcHandlerTpl<SOCKHEADER>::handle_output(ACE_HANDLE h)
{
	//_INFO("CTCPSvcHandler::handle_output() ...sock = %d", h);
	if(0 == mbSend_) {
		CMutexKeeper	mk(mutex_);
		if(0 == (mbSend_ = blockQueue_.getq())) {
		//_INFO("sock = %d getq() == null!!!", h);
//add 2013-3-1 by zjj
#ifdef ACE_HAS_EVENT_POLL
		PARENT::reactor()->remove_handler(this, ACE_Event_Handler::WRITE_MASK);
		mask_ &= ~WRITEMASK;
		if(mbSend_) {
			mbSend_->release();
			delete mbSend_;
			mbSend_ = 0;
		}
			return 0; 
#endif
//end add
			return -1;
		}
	}

	int		ret = 0;
	size_t	wantLen = mbSend_->size() - (mbSend_->rd_ptr() - mbSend_->base());
			//_INFO("wantLen = %d.......", wantLen);
	while(wantLen) {
		ret = peer().send(mbSend_->rd_ptr(), wantLen);
		if(0 == ret) {
			_ERROR("TCP  Send(): Peer(sock=%4d) is Closed!!!", PARENT::get_handle());
			if(mbSend_) {
				mbSend_->release();
				delete mbSend_;
				mbSend_ = 0;
			}
			return -1;
		}
		else
		if(-1 == ret) {
#ifdef __LINUX__            //modfiy 2011-8-5
			if(EAGAIN == errno || EINTR == errno) {
#else
			if(EAGAIN == errno || EINTR == errno || ACE_OS::last_error() == WSAEWOULDBLOCK) {			 
#endif
				_ERROR("TCP  Send(): Peer(sock=%4d) is Error(=%d),do next.", PARENT::get_handle(), errno);

				return 0;
			}
			else {
				_ERROR("TCP  Send(): Peer(sock=%4d) is Error(=%d)!!!", PARENT::get_handle(), ACE_OS::last_error());

				if(mbSend_) {
					mbSend_->release();
					delete mbSend_;
					mbSend_ = 0;
				}
				return -1;
			}
		}
		else {
			//_INFO("TCP  Send(): Peer(sock=%4d) ret(=%d)...", PARENT::get_handle(), ret);
			mbSend_->rd_ptr(ret);
			wantLen -= ret;
		}
	}
	if (mbSend_)   //add 2011-7-8
	{
		mbSend_->release();
		delete mbSend_;
		mbSend_ = 0;
	}

	return 0;
}

template <class SOCKHEADER>
int CTCPSvcHandlerTpl<SOCKHEADER>::sendPacket(unsigned short pduType, const void* buf, unsigned long len)
{
	//_INFO("CTCPSvcHandler::sendPacket() ... pduType = %d, len = %d", pduType, len);

	unsigned short	headerSize = sockHeaderCallBack_.GetPacketHeaderSize();
	CSockBlock*	mb = new CSockBlock(headerSize + len);
	int		ret = -1;
	if(0 != sockHeaderCallBack_.GeneratePacketHeader(mb->wr_ptr(), pduType, len)) {
		if (mb)  //add 2011-7-8
		{
			mb->release();
			delete mb;
			mb = NULL;
		}		
		PARENT::reactor()->remove_handler(this, ACE_Event_Handler::READ_MASK);
	}
	else {
		mb->wr_ptr(headerSize);
		mb->copy((const char*)buf, len);

		bool	b = false;
		{
			CMutexKeeper	mk(mutex_);
			b = blockQueue_.putq(mb);
		}
		if(! b) {
//			_INFO("sock = %d putq() == false!!!", PARENT::get_handle());
			if (mb)			//add 2011-7-8
			{
				mb->release();
				delete mb;
				mb = NULL;
			}			
		}
		else {
			PARENT::reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);
			mask_ |= WRITEMASK;
			ret = 0;
		}
	}

	return ret;
}


