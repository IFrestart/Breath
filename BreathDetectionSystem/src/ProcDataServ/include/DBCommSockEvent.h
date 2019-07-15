/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 DBCommSockEvent.h
 * @author	 chetang <chetang@webcon.com.cn>

 * @short    DBComm返回连接信息的事件处理
 **/

#ifndef	__DBSOCKEVENT_H__
#define	__DBSOCKEVENT_H__

#include "SockEventCallBack.h"
class CProcDataServ;

class CDBCommSockEvent : public ISockEventCallBack
{
public:
    CDBCommSockEvent(CProcDataServ* pServ)
					: pServ_(pServ)
    {}
	virtual int OnNewConnection(ACE_HANDLE sock);
    virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
                                    const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:
    CProcDataServ*	pServ_;
};

#endif	/*__DBSOCKEVENT_H__*/

