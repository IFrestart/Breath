/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file	 TransDataServ.h
 * @author	 jjzhou <jjzhou@webcon.com.cn>

 * @short
 **/

#ifndef	__TRANSDATASERV_H__
#define	__TRANSDATASERV_H__

#define  WEBCON_HAS_FILELOG

#include "Common.h"
#include "CenterSockEvent.h"
#include "PacketTimeOut.h"
#include "TcpAutoConnector.h"
#include "TCPConnector.h"
#include <set>
#include <map>
#include "IdDef.h"
#include <iconv.h>
#include "Mutex.h"
#include "Thread.h"

#define MEETID 100
#define FUNCTYPE 5


class CTransDataServ : public CBaseServ, public ITimerEventCallBack , public IPacketTimeOut ,public IThreadCallBack
{
public:
    CTransDataServ();
    ~CTransDataServ();

    /* From CBaseServ */
    virtual int initialize(int argc, char** argv);
    virtual int destroy();
    virtual int handle_signal(int signum, siginfo_t*, ucontext_t*);

    /* From ITimerEventCallBack */
    virtual int OnTimer(unsigned long timerID, void* arg);

    virtual void* threadProc( void* arg );

    /* Send Packet to Client by tcp */
    int sendPacketToTcpClient(ACE_HANDLE sock,
                    unsigned short pdutype,
                    const void* buf, unsigned long len);

    virtual int OnPacketTimeOut(unsigned short pdutype,
             const void* data, unsigned long len)  ;

    int sendPacketToCenter(unsigned short pdutype, const void* buf, unsigned long len);

    //启动定时器
    int startTimer();
    //关闭定时器
    int stopTimer();

    int  noticeDBaddData(const char* device,const char* filename);

    enum {
            keepCONNTID = 2 ,
            SetIpeTime_REQ_ToCli    = 128,
            SetIpeTime_RSP_ToDB     = 129,
            keepConnectInterVal = 5000,
            OutTimeKeepAliveClient = 1800,
            MaxReadLen = 1200,
            NHSystemDPROUTER0_REQ_FromDP = 10023,
            NHSystemDPROUTER0_REQ_FromCenter = 10024,
            NHSystemDPROUTER0_RSP_ToCenter = 10025,
            NHSystemDPROUTER0_RSP_ToDP = 10026,
			AddDataInfo_Rsp_ToNHSystem =	128
         };

private:
    //读取配置文件
    virtual int loadConfig(const char* pFile);

    CCenterSockEvent    centerSockEvent_;
    std::string			centerIP_;      //链接服务器的ip
    unsigned short		centerTcpPort_; //链接端口
    CTimer				keepTimer_; //定时器
    CTcpAutoConnector   centerConnector_;       //链接connector的通道
    CThread             m_thread_;
    pthread_mutex_t     mutexdownList_;
};

extern CTransDataServ	g_dbChunnelServ;
extern CTransDataServ&  GetApp();

#endif	/*__TRANSDATASERV_H__*/
