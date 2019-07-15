
/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 TransServSockEvent.h
* @author	 zhoujj <zhoujj@webcon.com.cn>

* @class	TransServSockEvent
* @short	与客户端的socket事件回调
**/

#ifndef	__TRANSSERVSOCKEVENT_H__
#define	__TRANSSERVSOCKEVENT_H__

#include "SockEventCallBack.h"
//#include "RTPEventCallBack.h"

class CNetCtrlCall;


//class TransServSockEvent : public ISockEventCallBack , public IRTPEventCallBack
class TransServSockEvent : public ISockEventCallBack 
{
public:
	TransServSockEvent(CNetCtrlCall* pServ) : m_pServ(pServ)
	{}

	int OnPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
		const void* data, unsigned long len,unsigned long fromIP = 0,unsigned short fromPort =0);	

public:
	//下面部分继承自 ISockEventCallBack
	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
		const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);
	virtual int OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
		const void* data, unsigned long len,unsigned long fromIP,unsigned short fromPort);

private:

	CNetCtrlCall* m_pServ;
	
};

#endif	/*__TRANSSERVSOCKEVENT_H__*/

