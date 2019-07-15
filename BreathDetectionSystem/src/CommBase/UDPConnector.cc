/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP连接数据接收与发送处理类,提供了UDP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	UDPConnector.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short 	UDP连接类
 **/

#include <iostream>

#include "UDPConnector.h"


template <class SVCHANDLER>
CUDPConnectorTpl<SVCHANDLER>::CUDPConnectorTpl(ISockEventCallBack* pSockEventCallBack)
				: pSockEventCallBack_(pSockEventCallBack), 
				handler_(pSockEventCallBack)
{
}

template <class SVCHANDLER>
CUDPConnectorTpl<SVCHANDLER>::~CUDPConnectorTpl()
{
	close();
}

template <class SVCHANDLER>
int CUDPConnectorTpl<SVCHANDLER>::create(unsigned short port)
{
	return handler_.open(port);
}

template <class SVCHANDLER>
int CUDPConnectorTpl<SVCHANDLER>::create(const char* ipstr, unsigned short port)
{
	return handler_.open(ipstr, port);
}

template <class SVCHANDLER>
int CUDPConnectorTpl<SVCHANDLER>::close()
{
	return handler_.close();
}

template <class SVCHANDLER>
int CUDPConnectorTpl<SVCHANDLER>::sendPacket(unsigned short pdutype, 
								const void* buf, unsigned long len,
								unsigned long ip, unsigned short port)
{
	return handler_.sendPacket(pdutype, buf, len, ip, port);
}

template <class SVCHANDLER>
int CUDPConnectorTpl<SVCHANDLER>::sendPacket(unsigned short pdutype, 
								const void* buf, unsigned long len,
								const char* ipstr, unsigned short port)
{
    ACE_INET_Addr   addr;

    if(0 != addr.set(port, ipstr)) {
        return -1;
    }
    
	return handler_.sendPacket(pdutype, buf, len, addr.get_ip_address(), port);
}

template <class SVCHANDLER>
ACE_HANDLE CUDPConnectorTpl<SVCHANDLER>::getSock() const
{
	return handler_.get_handle();
}

template <class SVCHANDLER>
bool CUDPConnectorTpl<SVCHANDLER>::isClosed() const
{
	return 	ACE_INVALID_HANDLE == handler_.get_handle();
}


