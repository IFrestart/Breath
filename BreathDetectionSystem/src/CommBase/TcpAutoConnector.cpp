/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	TcpAutoConnector.cpp
 * @author	zhoujj <zhoujj@webcon.com.cn>

 * @short 
 **/

#include "TcpAutoConnector.h"


CTcpAutoConnector::CTcpAutoConnector(ISockEventCallBack* pSockEventCallBack)
			: connIp_(0), connPort_(0),
			connPduType(0), connServerType(0), connServerIndex(0),
			connServerIp(0), connServerPort(0),
			CTCPConnector(this),
			pSockEventCallBack_(pSockEventCallBack)
{
	ACE_ASSERT(pSockEventCallBack);
}

CTcpAutoConnector::~CTcpAutoConnector()
{
	close();
}

int CTcpAutoConnector::close( )
{
	CTCPConnector::close();
	timer_.kill();
	return 0;
}

int CTcpAutoConnector::OnNewConnection( ACE_HANDLE sock )
{
	return pSockEventCallBack_->OnNewConnection(sock);
}

int CTcpAutoConnector::OnTCPPacketReceived( ACE_HANDLE sock, unsigned short pdutype,
	const void* data, unsigned long len )
{
	return pSockEventCallBack_->OnTCPPacketReceived(sock,pdutype,data,len);
}

int CTcpAutoConnector::OnConnectionClosed( ACE_HANDLE sock )
{
	timer_.start(connPort_, timeValue_, this, 0);
	return pSockEventCallBack_->OnConnectionClosed(sock);
}

int CTcpAutoConnector::create(const char* ip, unsigned short port, 
				unsigned short pdutype,unsigned short servertype,unsigned short serverindex,
				unsigned long serverip, unsigned short serverport,unsigned long timevalue)
{
	connIp_ = ip_to_long(ip);
	return create(connIp_,port,pdutype,servertype,serverindex,serverip,serverport,timevalue);
}

int CTcpAutoConnector::create(unsigned long ip, unsigned short port, 
				unsigned short pdutype,unsigned short servertype,unsigned short serverindex,
				unsigned long serverip, unsigned short serverport,unsigned long timevalue)
{
	connIp_ = ip;
	connPort_ = port;

	connPduType = pdutype;
	connServerType = servertype;
	connServerIndex = serverindex;

	connServerIp  = serverip;
	connServerPort = serverport;

	timeValue_ = timevalue;
	
	if(0 != CTCPConnector::create(ip, port)) {
		timer_.start(connPort_, timeValue_, this, 0);
	}
	else {
		reportself();
	}

	return 0;
}

int CTcpAutoConnector::reportself()
{
	if(connPduType != 0) {
		CSockStream ostream(32);

		ostream << connServerType << connServerIndex << connServerIp << connServerPort;
		CTCPConnector::sendPacket(connPduType,ostream.getData(),ostream.tell());
	}

	return 0;
}

int CTcpAutoConnector::OnTimer(unsigned long timerID, void* arg)
{
	ACE_ASSERT(timerID == connPort_);

	if(0 == CTCPConnector::create(connIp_, connPort_)) {
		printf("in onTime create success ip = [%lu] port = [%d]\n",connIp_,connPort_);
		reportself();
		return 1;
	}

	return 0;
}

