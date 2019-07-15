/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 SubCtrlServ.h
 * @author	 jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#ifndef	__CENTERCTRLSERV_H__
#define	__CENTERCTRLSERV_H__

#include "Common.h"
#include "ClientSockEvent.h"
#include "CenterSockEvent.h"
#include "TransSockEvent.h"
#include "PacketTimeOut.h"
#include "TcpAutoConnector.h"
#include "TCPConnector.h"
#include <set>
#include <map>
#include "IdDef.h"
#include <iconv.h>

#define MEETID 100
#define FUNCTYPE 5


class CSubCtrlServ : public CBaseServ, public ITimerEventCallBack , public IPacketTimeOut
{
public:
	CSubCtrlServ();
	~CSubCtrlServ();

	/* From CBaseServ */
	virtual int initialize(int argc, char** argv);
	virtual int destroy();
	virtual int handle_signal(int signum, siginfo_t*, ucontext_t*);

	/* From ITimerEventCallBack */
	virtual int OnTimer(unsigned long timerID, void* arg);

	/* Send Packet to Client by tcp */
	int sendPacketToTcpClient(ACE_HANDLE sock,
					unsigned short pdutype, 
					const void* buf, unsigned long len);

	virtual int OnPacketTimeOut(unsigned short pdutype, 
			 const void* data, unsigned long len)  ;
	int sendPacketToCenter(unsigned short pdutype, const void* buf, unsigned long len);
	int sendPacketToTrans(unsigned short pdutype, const void* buf, unsigned long len);
	int dealWinSysServerReport(ACE_HANDLE sock,const void* data,unsigned long len);

	int connectWithCenter(ACE_HANDLE sock);
	int startTimer();
	int stopTimer();
	int reportSelf();


	unsigned long getServerVersionId() {return lServerVersionId_;} //add 2010-4-22
	unsigned short addRSServer(const RSServerID& rssid);

	void dumpUser(const char* pre,std::fstream& logfile) ;

	void setConnectOkTrans(bool status) { bconnectOkTrans_ = status;}
	bool getConnectOkTrans() {return bconnectOkTrans_;}

protected:
	enum {	CenterCONNTID = 2 ,CenterConnectInterVal = 5000, OutTimeKeepAliveClient = 120};

private:
	virtual int loadConfig(const char* pFile);


	CClientSockEvent		clientSockEvent_;
	CTCPAcceptor			clientTcpAcceptor_;
	std::string			clientIP_;
	unsigned short			clientTcpPort_;

	std::string			centerIP_;
	unsigned short			centerTcpPort_;

	CCenterSockEvent                centerSockEvent_;
        CTCPConnector               	centerConnector_;

	CTransSockEvent                 transSockEvent_;
  //      CTCPConnector               	transConnector_;
        CTcpAutoConnector              	transConnector_;

	std::string			transIP_;
	unsigned short			transTcpPort_;

	CTimer				reportTimer_;

	unsigned long                   lServerVersionId_; //add 2010-4-22
	std::list<RSServerID>       	m_serverList;  //for save all winSysServ connect

	std::string			serialNumber_;
	bool 				bconnectOkTrans_;
};

extern CSubCtrlServ	g_dbChunnelServ;
extern CSubCtrlServ&    GetApp();

#endif	/*__AUTHCTRLSERV_H__*/
