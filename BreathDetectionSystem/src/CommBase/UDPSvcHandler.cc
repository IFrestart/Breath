/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP连接数据接收与发送处理类,提供了UDP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	UDPSvcHandler.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short	UDP连接数据接收与发送处理类
 **/

#include <iostream>

#include "Logger.h"
#include "UDPSvcHandler.h"


template <class SOCKHEADER>
CUDPSvcHandlerTpl<SOCKHEADER>::CUDPSvcHandlerTpl(ISockEventCallBack* pSockEventCallBack)
						: 
						pSockEventCallBack_(pSockEventCallBack),
						ACE_Event_Handler(ACE_Reactor::instance())
{
}

template <class SOCKHEADER>
CUDPSvcHandlerTpl<SOCKHEADER>::~CUDPSvcHandlerTpl()
{
}


template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::open(unsigned short port)
{
	return open(0, port);
}

template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::open(const char* ipstr, unsigned short port)
{
	ACE_INET_Addr	addr;
	
	if(0 == ipstr) {
		if(0 != addr.set(port)) {
			return -1;
		}
	}
	else {
		if(0 != addr.set(port, ipstr)) {
			return -1;
		}
	}

	int		ret = -1;
	if(0 == dgram_.open(addr)
		&& 0 == PARENT::reactor()->register_handler(this, ACE_Event_Handler::READ_MASK)
		&& 0 == dgram_.enable(ACE_NONBLOCK)) {
		ret = 0;
	}

	return ret;
}

template <class SOCKHEADER>
ACE_HANDLE CUDPSvcHandlerTpl<SOCKHEADER>::get_handle() const
{
	return dgram_.get_handle();
}

template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::handle_input(ACE_HANDLE h)
{
	ACE_INET_Addr	addr;
	int				ret = 0;
	long			readlen = 0;
	if(-1 == (readlen = dgram_.recv(recvBuf_, sizeof(recvBuf_), addr))) {
		if(EAGAIN == errno || EINTR == errno) {
			_ERROR("UDP Recv():Peer is Error(=%d),do next.", errno);
			return 0;
		}
		else {
			_ERROR("UDP Recv():Peer is Error(=%d)!!!", errno);
			//return -1;
			return 0;//modify by zjj 2011-6-15
		}
	}
	else {
		if(pSockEventCallBack_) {
			unsigned short	headerLen = sockHeaderCallBack_.GetPacketHeaderSize();
			unsigned short	pduType;
			unsigned long	bodyLen;

			if(0 == sockHeaderCallBack_.ParsePacketHeader(recvBuf_, pduType, bodyLen)) {
				if(bodyLen + headerLen == readlen) {
	//add by zjj 2006-8-14
					if(1 == pduType)
					{
						sendPacket(19999,0,0, addr.get_ip_address(), addr.get_port_number());
					}
					else
					{
	//end by zjj 2006-8-14
					ret = pSockEventCallBack_->OnUDPPacketReceived(get_handle(),
															pduType,
															recvBuf_ + headerLen, bodyLen, 
															addr.get_ip_address(), 
															addr.get_port_number());
					} //add 2006-8-14
				}
				/*
				else {
					ret = -1;
				}
				*/
			}
		}
	}

	return 0 == ret ? 0 : -1;
}

template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::handle_close(ACE_HANDLE h, ACE_Reactor_Mask m)
{
	int	ret = -1;
	if(ACE_INVALID_HANDLE != h && pSockEventCallBack_) {
		pSockEventCallBack_->OnConnectionClosed(h);
		dgram_.close();
		ret = 0;
	}

	return ret;
}

template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::close()
{
	return PARENT::reactor()->remove_handler(this, ACE_Event_Handler::READ_MASK);
}

template <class SOCKHEADER>
int CUDPSvcHandlerTpl<SOCKHEADER>::sendPacket(unsigned short pduType, 
							const void* buf, unsigned long len,
							unsigned long ip, unsigned short port)
{
	if(port == 0 || ip == 0) {
		_ERROR("CUDPSvcHandlerTpl<SOCKHEADER>::sendPacket(!!!Invalid IP-PORT!!!)");
		return -1;
	}

	if(0 != sockHeaderCallBack_.GeneratePacketHeader(sendBuf_,
														pduType, len)) {
		return -1;
	}

	unsigned short	headerSize = sockHeaderCallBack_.GetPacketHeaderSize();
	if(headerSize + len > MAXSOCKBUFFERLEN) {
		_ERROR("CUDPSvcHandlerTpl<SOCKHEADER>::sendPacket(!!!Packet Too Large!!!)");
		return -1;
	}

	memcpy(sendBuf_ + headerSize, buf, len);

	unsigned long	dataLen = headerSize + len;
	ACE_INET_Addr	addr;

	if(0 != addr.set(port, ip)) {
		return -1;
	}

	if(-1 == dgram_.send(sendBuf_, dataLen, addr)) {
		_ERROR("CUDPSvcHandlerTpl<SOCKHEADER>::sendPacket() Send Error(=%d:%s)!!!", errno, strerror(errno));
		/*
		   PARENT::reactor()->remove_handler(this, ACE_Event_Handler::READ_MASK);
		*/
		return -1;
	}

	return 0;
}

