/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����HTTP��,�ṩ����HTTP�Ľ���,�ر�,�������ݵȲ���

 * @file	HTTPConnector.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	����HTTP��
 **/

#ifndef	__HTTPCONNECTOR_H__
#define	__HTTPCONNECTOR_H__

#include "HTTPSvcHandler.h"
#include "TCPConnector.h"

typedef CTCPConnectorTpl<CHTTPSvcHandler>	CHTTPConnector;

#endif	/*__HTTPCONNECTOR_H__*/

