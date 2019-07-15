/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 CenterCtrlServ.h
 * @author	 jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#ifndef	__CENTERCTRLSERV_H__
#define	__CENTERCTRLSERV_H__

#include "Common.h"
#include "ClientSockEvent.h"
#include "DBCommSockEvent.h"
#include "PacketTimeOut.h"
#include "TcpAutoConnector.h"
#include "TCPConnector.h"
#include <set>
#include <map>
#include "IdDef.h"
#include <iconv.h>

#define MEETID 100
#define FUNCTYPE 5


class CCenterCtrlServ : public CBaseServ, public ITimerEventCallBack , public IPacketTimeOut
{
public:
	CCenterCtrlServ();
	~CCenterCtrlServ();

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

	/* Send Packet to DBComm */
	int sendPacketToDBComm(unsigned short pdutype, 
					const void* buf, unsigned long len);


	int addSubServ(unsigned short sock,const void* buf ,unsigned long len);
	int addWinSysServ(unsigned short sock,const void* buf ,unsigned long len);
	int optWinSysServ(unsigned short sock,const void* buf ,unsigned long len);
	int dealTransData(unsigned short sock,const void* buf ,unsigned long len);

	int sendPacketToAlarmServ(unsigned short pdutype, const void* buf, unsigned long len);

	int getFrindUserWanAddr(unsigned short sock,unsigned short pdutype, const void* buf, unsigned long len); //add 2014-7-8
	int dealReportWebRtcConnectPort(const UserID& user, unsigned long fromIP, unsigned short fromPort); //add 2014-7-8

	void OnDBCommConnectionClosed(ACE_HANDLE sock);

	virtual int OnPacketTimeOut(unsigned short pdutype, 
			 const void* data, unsigned long len)  ;


	int GetState()
	{
		return m_state;
	}

	void SetState(int state)
	{
		m_state = state;
	}
	unsigned short getAreaId()const {return areaId_;}	

	int ConnetDB();


	int CHVerifiedCode (ACE_HANDLE sock, unsigned short pdutype,
								const void* data, unsigned long len ,
									 int clusterindex ,int  servindex);

	int VerifyWeb (ACE_HANDLE sock, unsigned short pdutype,
								const void* data, unsigned long len);


	unsigned long getServerVersionId() {return lServerVersionId_;} //add 2010-4-22
	unsigned short addRSServer(const RSServerID& rssid);
	int  	updateUser(const UserID& user,ACE_HANDLE sock); //add 2013-3-4
	int	addUser(const UserInfoEx& user); //modify 2011-2-28
	int 	changeUserStatus(const UserID user,unsigned long status);
	int  	clearUserStatus(const UserID user);
	int	delUser(const UserID& user);

	USER_Ex_it  beginUser();
	USER_Ex_it  endUser();
	USER_Ex_it  findUser(const UserID& user);
	USER_Ex_it  findUser(unsigned long userIndex); //add 2013-3-12
	USER_Ex_it  findUser(const char* szAccount);
	int sendPacketToDstUser(const UserID user,unsigned short pdutype, const void* buf, unsigned long len);
	int sendPacketToDstUser(unsigned long userindex,unsigned short pdutype, const void* buf, unsigned long len);

	int  checkKeepAliveForClient(); //add 2011-8-15
	int  dealKeepAlive(ACE_HANDLE sock, CSockStream& in_stream);

	/*********************************
	flag 1:   add rssid server
	     0:   del  rssid server
	************************************/
	unsigned short noticeAllRSServer(const RSServerID& rssid,unsigned short flag);
	unsigned short returnAllRSServerList(const RSServerID& rssid);
		/* Send Packet to all RSS */
	int sendPacketToAllRSServer(unsigned short pdutype, const void* buf, unsigned long len);
		/* Send Packet to RSS */
	int sendPacketToRSServer(ACE_HANDLE sock, unsigned short pdutype, const void* buf, unsigned long len);

//add 2012-4-16
	void registerNoticePubChannelToDB();
	void getGroupListFromDB();
	int  gotGroupList(const void* data, unsigned long len);
	void gotStoreGroupRsp(short ret,unsigned short gid,const void* data, unsigned long len);
	void gotModifyMeetInfoRsp(short ret,unsigned short gid,unsigned short noticePdu,const void* data, unsigned long len);
	int  connectWithDBComm(ACE_HANDLE sock);

//end add 2012-4-16
	void dumpUser(const char* pre,std::fstream& logfile) ;
//add 2013-3-11
	int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen);
	int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen);
	int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen);
//end add 2013-3-11

	typedef std::map<unsigned short, std::list<RSServerID> >::iterator CRSS_IT;

protected:
	enum { 
//		   RESALLOCCONNTIMERID = 12345, 
		   DBCOMMCONNTIMERID = 12346,KeepAliveClientID = 15 };
	//enum {	DBCOMMCONNTIMERINTERVAL = 5000 ,KeepAliveClientInterVal = 5000, OutTimeKeepAliveClient = 30};//modify 2013-1-22
	enum {	DBCOMMCONNTIMERINTERVAL = 5000 ,KeepAliveClientInterVal = 5000, OutTimeKeepAliveClient = 120};

private:
	virtual int loadConfig(const char* pFile);


	CClientSockEvent		clientSockEvent_;
	CTCPAcceptor			clientTcpAcceptor_;
	std::string			clientIP_;
	unsigned short			clientTcpPort_;

	CDBCommSockEvent		dbCommSockEvent_;
	CTcpAutoConnector 		dbCommConnector_;
//	CTCPConnector 			dbCommConnector_;
	CTimer				dbCommConnTimer_;
	CTimer				keepAliveTimer_; //add 2011-8-15
	std::string 			dbCommIP_;
	unsigned short			dbCommPort_;

	int m_state;

	unsigned long                   lServerVersionId_; //add 2010-4-22
	std::list<UserInfoEx>             m_userInfo;  //for save all use login
	unsigned short 			areaId_ ;

	std::map<unsigned short, std::list<RSServerID> > map_rss_;
	std::list<RSServerID>  		list_rss_;  //all sub server list
};

extern CCenterCtrlServ	g_dbChunnelServ;
extern CCenterCtrlServ&    GetApp();

#endif	/*__AUTHCTRLSERV_H__*/
