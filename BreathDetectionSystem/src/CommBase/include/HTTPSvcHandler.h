/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTP�������ݽ����뷢�ʹ�����,�ṩ��HTTP���ӵĴ�,�ر��뷢�����ݵȲ���
 * ������һ���������ĵ���ʱ�����ʱִ�лص�.

 * @file	HTTPSvcHandler.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	HTTP�������ݽ����뷢�ʹ�����
 **/

#ifndef	__HTTPSVCHANDLER_H__
#define	__HTTPSVCHANDLER_H__

#include "HTTPHead.h"
#include "TCPSvcHandler.h"

typedef CTCPSvcHandlerTpl<CHTTPHeader> CHTTPSvcHandler;

#endif	/*__HTTPSVCHANDLER_H__*/

