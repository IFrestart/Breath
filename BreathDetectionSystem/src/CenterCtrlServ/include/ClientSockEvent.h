/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 ClientSockEvent.h
 * @author	 chetang <chetang@webcon.com.cn>

 * @short 
 **/

#ifndef	__CLIENTSOCKEVENT_H__
#define	__CLIENTSOCKEVENT_H__

#include "SockEventCallBack.h"

class CCenterCtrlServ;

class CClientSockEvent : public ISockEventCallBack
{
public:
	CClientSockEvent(CCenterCtrlServ* pServ)
					: pServ_(pServ), areaindex_(0)
	{}

	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
									const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

	virtual int OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
                                                const void* data, unsigned long len,
                                                unsigned long fromIP,
                                                unsigned short fromPort);

	unsigned short getClientNum() const;
	void setClusterID(unsigned short areaindex) { areaindex_ = areaindex; }

protected:
	int	VerifiedCode (ACE_HANDLE sock, unsigned short pdutype,
								const void* data, unsigned long len);

protected:
	CCenterCtrlServ*	pServ_;
	unsigned short	clientNum_;

	unsigned short areaindex_;
};

#endif	/*__CLIENTSOCKEVENT_H__*/

