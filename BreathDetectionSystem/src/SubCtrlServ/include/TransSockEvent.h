/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 TransSockEvent.h
 * @author	 jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#ifndef	__TRANSSOCKEVENT_H__
#define	__TRANSSOCKEVENT_H__

#include "SockEventCallBack.h"

class CSubCtrlServ;

class CTransSockEvent : public ISockEventCallBack
{
public:
	CTransSockEvent(CSubCtrlServ* pServ) : pServ_(pServ) {}
	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
									const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:
	CSubCtrlServ*	pServ_;
};

#endif	/*__TRANSSOCKEVENT_H__*/

