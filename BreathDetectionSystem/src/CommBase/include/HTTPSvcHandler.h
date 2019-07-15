/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTP连接数据接收与发送处理类,提供了HTTP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	HTTPSvcHandler.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	HTTP连接数据接收与发送处理类
 **/

#ifndef	__HTTPSVCHANDLER_H__
#define	__HTTPSVCHANDLER_H__

#include "HTTPHead.h"
#include "TCPSvcHandler.h"

typedef CTCPSvcHandlerTpl<CHTTPHeader> CHTTPSvcHandler;

#endif	/*__HTTPSVCHANDLER_H__*/

