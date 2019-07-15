
/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 AuthServSockEvent.h
* @author	 zhoujj <zhoujj@webcon.com.cn>

* @class	AuthServSockEvent
* @short	��ͻ��˵�socket�¼��ص�
**/

#ifndef	__AUTHSERVSOCKEVENT_H__
#define	__AUTHSERVSOCKEVENT_H__

#include "SockEventCallBack.h"
class CNetCtrlCall;
class INetACDataCallBack;
class AuthServSockEvent : public ISockEventCallBack
{
public:
	AuthServSockEvent(CNetCtrlCall* pServ) : m_pServ(pServ),m_pDataCallBack(NULL)
	{}
	void setDataCallBack(INetACDataCallBack* pCallBack){ m_pDataCallBack = pCallBack;}
public:
	//�����Ǽ̳���ISockEventCallBack
	virtual int OnNewConnection(ACE_HANDLE sock);
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, 
		const void* data, unsigned long len);
	virtual int OnConnectionClosed(ACE_HANDLE sock);

	virtual int OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
		const void* data, unsigned long len,unsigned long fromIP,unsigned short fromPort);	
    
private:
	CNetCtrlCall* m_pServ;
	INetACDataCallBack*  m_pDataCallBack ;

};

#endif	/*__AUTHSERVSOCKEVENT_H__*/

