/**
 * @brief:
 * @author: shizc
 * @date:   2018-06-07
 * @file:   ProcDataServ
 */

#ifndef	__PROCDATASERV_H__
#define	__PROCDATASERV_H__

#include "SubPDU.h"
#include "FileProc.h"
#include "DBCommSockEvent.h"
#include "Common.h"
#include "PacketTimeOut.h"
#include "TCPConnector.h"
#include "IdDef.h"
#include "Logger.h"

#include<ctime>
#include <set>
#include <map>
#include <vector>
#include <iconv.h>
#include <time.h>

using namespace SubPDU;

#define DATADIR "/usr/local/Data/"

class CProcDataServ : public CBaseServ, public ITimerEventCallBack , public IPacketTimeOut
{
public:
    CProcDataServ();
    ~CProcDataServ();

	/* From CBaseServ */
	virtual int initialize(int argc, char** argv);
	virtual int destroy();
    //外部信号处理
	virtual int handle_signal(int signum, siginfo_t*, ucontext_t*);

	/* From ITimerEventCallBack */
    virtual int OnTimer(unsigned long timerID, void* arg);

    /* DBConnector begin ***********************************************************/
    //获取和设置与DB的连接状态
    int GetState() {return m_ConnDBCommState;}
    void SetState(int state) {m_ConnDBCommState = state;}
    //连接DB
    int ConnetDB();
    /* Send Packet to DBComm */
    int sendPacketToDBComm(unsigned short pdutype,
                            const void* buf, unsigned long len);
    //发送数据超时
    virtual int OnPacketTimeOut(unsigned short pdutype,
                                const void* data, unsigned long len);

        /***************获取状态文件*********************/
    void getUnprocDir();
    //获取设备下某天的文件列表
    void getOnedayFile(const char *Xlh, time_t);
    //获取设备下文件列表
    void getdirFile(const char* Xlh,int num);
    //获取num个文件
    void getFilelist(const char *Xlh, time_t t = 0, long num =0);
    /* end DB ************************************************************/

    enum {FILENUM = 1000};
protected:
    enum { DBCONNTIMERID = 12, PROCDATATIMERID = 24, KeepAliveClientID = 15 };
    enum
    {//时间:毫秒
        DBCOMMCONNTIMERINTERVAL = 30000,         //
        PROCDATATIMERINTERVAL   = 1000*60*10,   //
        KeepAliveClientInterVal = 3000,
        OutTimeKeepAliveClient = 1800
    };

private:
    //读取配置文件
    virtual int loadConfig(const char* pFile);

private:
    ArchiveFile             Dirlist;
    CTimer                  m_ProcTimer;
    CDBCommSockEvent        m_dbSockEvent;
    CTCPConnector           m_dbConnector;       //
    CTimer                  m_dbConnectTimer;
    std::string 			m_dbIP;
    unsigned short			m_dbTcpPort;
    int                     m_ConnDBCommState;        //状态位 0:已连接 -1:未连接
};

extern CProcDataServ	g_dbChunnelServ;
extern CProcDataServ&   GetApp();

#endif	/*__PROCDATASERV_H__*/
