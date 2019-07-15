/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * svc_handler����ӿڣ���*Acceptor��*Connector��ʹ��

 * @file	 SvcManager.h
 * @short 	svc_handler����ӿ�
 * @author	 chetang <chetang@webcon.com.cn>

 * @class	ISvcManager
 * @short 	svc_handler����ӿ�
 **/

#ifndef	__SVCMANAGER_H__
#define	__SVCMANAGER_H__

#include "ace/OS.h"

class ISvcManager
{
public:
	ISvcManager( );
	~ISvcManager( );
	
	virtual int OnSvcOpen(void*);
	virtual int OnPacketReceived(void*, unsigned short, const void*, unsigned long);
	virtual int OnSvcClose(void*);
};

#endif	/*__SVCMANAGER_H__*/

