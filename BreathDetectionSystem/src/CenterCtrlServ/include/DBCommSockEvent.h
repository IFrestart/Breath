/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 DBCommSockEvent.h
 * @author	 chetang <chetang@webcon.com.cn>

 * @short 
 **/

#ifndef	__DBCOMMSOCKEVENT_H__
#define	__DBCOMMSOCKEVENT_H__

#include "SockEventCallBack.h"

class CCenterCtrlServ;

class CDBCommSockEvent : public ISockEventCallBack
{
public:
	CDBCommSockEvent(CCenterCtrlServ* pServ)
					: pServ_(pServ)
	{}
	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
									const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:
	CCenterCtrlServ*	pServ_;
};

#endif	/*__DBCOMMSOCKEVENT_H__*/

