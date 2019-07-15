/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动HTTP类,提供被动HTTP的建立,关闭,发送数据等操作

 * @file	HTTPAcceptor.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	被动HTTP类
 **/

#ifndef	__HTTPACCEPTOR_H__
#define	__HTTPACCEPTOR_H__

#include "HTTPSvcHandler.h"
#include "TCPAcceptor.h"

typedef CTCPAcceptorTpl<CHTTPSvcHandler>	CHTTPAcceptor;

#endif	/*__HTTPACCEPTOR_H__*/

