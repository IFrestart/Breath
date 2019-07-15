/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动HTTP类,提供被动HTTP的建立,关闭,发送数据等操作

 * @file	HTTPConnector.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	被动HTTP类
 **/

#ifndef	__HTTPCONNECTOR_H__
#define	__HTTPCONNECTOR_H__

#include "HTTPSvcHandler.h"
#include "TCPConnector.h"

typedef CTCPConnectorTpl<CHTTPSvcHandler>	CHTTPConnector;

#endif	/*__HTTPCONNECTOR_H__*/

