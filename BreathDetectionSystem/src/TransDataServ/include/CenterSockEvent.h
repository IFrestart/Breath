/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 CenterSockEvent.h
 * @author	 jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#ifndef	__CENTERSOCKEVENT_H__
#define	__CENTERSOCKEVENT_H__

#include "SockEventCallBack.h"

class CTransDataServ;

class CCenterSockEvent : public ISockEventCallBack
{
public:
	CCenterSockEvent(CTransDataServ* pServ) : pServ_(pServ) {}
	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
									const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:
	CTransDataServ*	pServ_;
};

#endif	/*__CENTERSOCKEVENT_H__*/

