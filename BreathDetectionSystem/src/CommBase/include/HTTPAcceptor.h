/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����HTTP��,�ṩ����HTTP�Ľ���,�ر�,�������ݵȲ���

 * @file	HTTPAcceptor.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	����HTTP��
 **/

#ifndef	__HTTPACCEPTOR_H__
#define	__HTTPACCEPTOR_H__

#include "HTTPSvcHandler.h"
#include "TCPAcceptor.h"

typedef CTCPAcceptorTpl<CHTTPSvcHandler>	CHTTPAcceptor;

#endif	/*__HTTPACCEPTOR_H__*/

