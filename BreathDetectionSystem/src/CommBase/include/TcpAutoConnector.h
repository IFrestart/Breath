/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 TcpAutoConnector.h
 * @author	 zhoujj <zhoujj@webcon.com.cn>

 * @short 
 **/

#ifndef	__TCPAUTOCONNECTOR_H__
#define	__TCPAUTOCONNECTOR_H__

#include "Common.h"

class CTcpAutoConnector :  public ITimerEventCallBack, public CTCPConnector, public ISockEventCallBack
{
public:
	CTcpAutoConnector(ISockEventCallBack* pSockEventCallBack);
	~CTcpAutoConnector();
	int create(const char* ip, unsigned short port,
			unsigned short pdutype = 0,unsigned short servertype = 0,unsigned short serverindex = 0,
			unsigned long serverip = 0,unsigned short serverport = 0,unsigned long timevalue=5000);

	int create(unsigned long ip, unsigned short port,
			unsigned short pdutype = 0,unsigned short servertype = 0,unsigned short serverindex = 0,
			unsigned long serverip = 0,unsigned short serverport = 0,unsigned long timevalue=5000);
	
	int close();

private:
	virtual int OnNewConnection( ACE_HANDLE sock );
	virtual int OnTCPPacketReceived( ACE_HANDLE sock, unsigned short pdutype,
									const void* data, unsigned long len );
	virtual int OnConnectionClosed( ACE_HANDLE sock );

	/* From ITimerEventCallBack */
	virtual int OnTimer(unsigned long timerID, void* arg);
	int reportself();

	CTimer			timer_;
	unsigned long 		timeValue_;

	unsigned long		connIp_;
	unsigned short		connPort_;

	unsigned short		connPduType;
	unsigned short		connServerType;
	unsigned short		connServerIndex;

	unsigned long		connServerIp;
	unsigned short		connServerPort;

	ISockEventCallBack*	pSockEventCallBack_;
};

#endif	/* __TCPAUTOCONNECTOR_H__ */

