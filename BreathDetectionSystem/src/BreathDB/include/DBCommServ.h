/**
 * Copyright (c) 2015, HangZhou Webcon Corporation.

 * @file	 DBCommServ.h
 * @author	 shizc <shizc@webcon.com.cn>

 * @short   数据库通信服务器
 * @date    07/2015
 **/

#ifndef	DBCOMMSERV_H
#define	DBCOMMSERV_H

#include <list>
#include <queue>
#include "TCPAcceptor.h"
#include "TcpAutoConnector.h"
#include "BaseServ.h"
#include "EDUDBComm.h"
#include "DBConfigClass.h"
#include "ThreadCallBack.h"
#include "Thread.h"
#include "SockStream.h"
#include <fstream>

using namespace std;
using namespace SubPDU;
using namespace PDU;

#define WRITE_BUFFER 0
#define READ_BUFFER 1


struct Concurrent
{
public :

    Concurrent();

    Concurrent(const Concurrent &that);

    ~Concurrent();

    int set(int sock, short pdutype,CVOID *buf,int len);

    int getSock();

    short getPdutype();

    int getLen();

    CCHAR *getBuffer();

    Concurrent& operator= (const Concurrent& that);

private :
    int     m_sock;
    short   m_pdutype;
    int     m_len;
    char    *m_buffer;
};

typedef queue <Concurrent> write_Queue;
typedef queue <Concurrent> read_Queue;

typedef struct tagFunctionServerIDWithSock
{
    int     sock;
    WORD	wAreaIndex;
    WORD	wFuncType;
    WORD	wServerIndex;
} FunctionServerIDWithSock;

typedef int (CEDUDBComm::*pFun_PubPdu) (CVOID *inbuf, unsigned long inlen,
                                        void *&outbuf, unsigned long &outlen);
struct PubPduFunList
{
    WORD wReqPduType;
    pFun_PubPdu pFun;
    WORD wRspPduType;
};


typedef int (CEDUDBComm::*pFun_PubPduCenter) (CVOID *inbuf, unsigned long inlen,
                                              void *&outbuf, unsigned long &outlen,
                                              void *&outbuf2, unsigned long &outlen2);
struct PubPduFunListCenter
{
    WORD wReqPduType;
    pFun_PubPduCenter pFun;
    WORD wRspPduType;
};

struct PubPduFunListCenter SubPduListCenter[] = {
{0, NULL, 0}
};

struct PubPduFunList ChunnelSubPduList[] = {
{0, NULL, 0}
};


struct PubPduFunList WriteSubPduList[] = {

/** 上传文件模块 */
//添加检测数据文件属性索引
{DBSUB_AddDataInfo_Req_FromNHSystem, &CEDUDBComm::OnAddDataInfo, DBSUB_AddDataInfo_Rsp_ToNHSystem},

//添加openid
//{AddOpenId_Req_WebToDB ,&CEDUDBComm::OnAddOpenId, AddOpenId_Rsp_DBToWeb},
//openid绑定设备（微信公众号用）
{BindOpenid_Req_WebToDB, &CEDUDBComm::OnBindBreath,BindOpenid_Rsp_DBToWeb},
//解除绑定（微信公众号用）
{DelBindOpenid_Req_WebToDB, &CEDUDBComm::OnDelOpenidBind,DelBindOpenid_Rsp_DBToWeb},
//终端设备入库
{AddTerminalInfo_REQ_FromWeb,&CEDUDBComm::OnAddTerminalInfo, AddTerminalInfo_RSP_ToWeb},
//销毁终端设备
{DelTerminalInfo_REQ_FromWeb,&CEDUDBComm::OnDelTerminalInfo, DelTerminalInfo_RSP_ToWeb},

//添加报警信息
{DBSUB_AddAlarmInfo_Req_FromNHSystem, &CEDUDBComm::OnAddBreathAlarmInfo, DBSUB_AddAlarmInfo_Rsp_ToNHSystem},
//添加报警数据2
//{AddAlarmInfo2_Req_FromNHSystem, &CEDUDBComm::OnAddSpaceStat, AddAlarmInfo2_Rsp_ToNHSystem},

//设置干涉时间
{SetIpeTime_REQ_FromWeb,&CEDUDBComm::OnSetIpeTime,SetIpeTime_RSP_ToDB},

{AddHX_Req_WebToDB,&CEDUDBComm::OnAddHX,AddHX_Rsp_DBToWeb},
{0, NULL, 0}
};

struct PubPduFunList ReadSubPduList[] = {
//检测openid
//{CheckOpenId_Req_WebToDB,&CEDUDBComm::OnCheckOpenId, CheckOpenId_Rsp_DBToWeb},
//获取openid绑定卡号列表
{GetBandZDList_Req_WebToDB, &CEDUDBComm::OnGetBandZDList, GetBandZDList_Rsp_DBToWeb},
{GetBandZhDList_Req_WebToDB, &CEDUDBComm::OnGetBandZhDList,GetBandZhDList_Rsp_DBToWeb},

//查看所有入库设备
{GetTerminalList_REQ_FromWeb,&CEDUDBComm::OnGetTerminalList,GetTerminalList_RSP_ToWeb},

// 获得报警信息
{GetAlarmInfoListPage_Req_FromCli, &CEDUDBComm::OnGetBreathAlarmInfo, GetAlarmInfoListPage_Rsp_ToCli},
// 获得检测信息
//{GetDataInfoListPage_Req_FromCli, &CEDUDBComm::OnGetDataInfo, GetDataInfoListPage_Rsp_ToCli},
//获取报警波形图
{GetAlarmInfoData_Req_FromCli,&CEDUDBComm::OnGetAlarmInfoData,GetAlarmInfoData_Rsp_ToCli},

//测试呼吸事件报告
{tryBreathInfo_Req_WebToDB,&CEDUDBComm::OntryBreathEntRpt,tryBreathInfo_Rsp_DBToWeb},

//获取干涉时间参数
{GetIpeTime_REQ_FromCli,&CEDUDBComm::OnGetIpeTime,GetIpeTime_RSP_ToCli},
//获取报警信息2
{GetAlarmInfoListPage2_Req_FromCli, &CEDUDBComm::OnGetAlarmInfo2, GetAlarmInfoListPage2_Rsp_ToCli},

{GetSleepInfo_REQ_FromCli, &CEDUDBComm::OnGetSleepInfo, GetSleepInfo_RSP_ToCli},

//获取呼吸和心率
{GetBRInfo_REQ_FromCli, &CEDUDBComm::OnGetBRInfo, GetBRInfo_RSP_ToCli},

{GetInsomInfo_REQ_FromCli, &CEDUDBComm::OnGetInsomInfo,GetInsomInfo_RSP_ToCli},

// 获取异常统计
{Abnormal_statistics_Req_FromCLI,&CEDUDBComm::OnGetAbnormal,Abnormal_statistics_Rsp_ToCLI},

//
{getHX_Req_WebToDB,&CEDUDBComm::OnGetHX,getHX_Rsp_DBToWeb},
{0, NULL, 0}
};


struct PubPduFunList WritePubPduList[] = {
 /*******************************************************
 * 非通道pdu
 */

//用户登录
//{MAINCERT_REQ_TODBCOMM, &CEDUDBComm::OnLogin, MAINCERT_RSP_FROMDBCOMM},
{inStat_BaseSmoothPara_REQ_TODBCOMM, &CEDUDBComm::OninStat_BaseSmoothPara, inStat_BaseSmoothPara_RSP_FROMDBCOMM},
{0, NULL, 0}
};

struct PubPduFunList ReadPubPduList[] = {
    /*******************************************************
 * 非通道pdu
 */
{getUnprocDir_REQ_TODBCOMM, &CEDUDBComm::OngetUnprocDir,getUnprocDir_RSP_FROMDBCOMM},
{getStatFilelist_REQ_TODBCOMM, &CEDUDBComm::OngetStatFilelist,getStatFilelist_RSP_FROMDBCOMM},
{0, NULL, 0}
};


class CDBCommServ;

class CDBCommSockEvent:public ISockEventCallBack
{
public:
    CDBCommSockEvent (CDBCommServ * pDBCommServ);
    ~CDBCommSockEvent ();

    /**
     * 连接数据库服务器的回调函数
     * @return 0 表成功
     */
    virtual int OnNewConnection (int sock);

    /**
     * 数据到达后的回调函数
     * @param sock 连接的sock号
     * @param pdutype PDU类型
     * @param data 接收到的数据
     * @param len  接收到的数据长度
     * @return 0 表成功
     */
    virtual int OnTCPPacketReceived (int sock, unsigned short pdutype,
                                     CVOID *data, unsigned long len);
    virtual int OnConnectionClosed (int sock);

private:  CDBCommServ * m_pDBCommServ;
};


class ConcurrenReadThread : public IThreadCallBack
{
public :

    ConcurrenReadThread();

    ~ConcurrenReadThread();

    /* From IThreadCallBack */
    void*  threadProc(void* arg);
};

class ConcurrenWriteThread : public IThreadCallBack
{
public :

    ConcurrenWriteThread();

    ~ConcurrenWriteThread();

    /* From IThreadCallBack */
    void*  threadProc(void* arg);
};

class CDBCommServ:public CBaseServ, public ITimerEventCallBack    
{
public:
    CDBCommServ ();
    ~CDBCommServ ();

    /**
     * From CBaseServ, ITimerEventCallBack
     */
    virtual int initialize (int argc, char **argv);
    virtual int destroy ();
    int OnTimer (unsigned long timerID, void *arg);
    int OnNewConnection (int sock);
    int OnConnectionClosed (int sock);

    /**
     * Send Packet to Client
     */
    int sendPacket (ACE_HANDLE sock, unsigned short pdutype,
                    CVOID *buf, unsigned long len);

    /**
     * 数据到达后的处理函数 在OnTCPPacketReceived里调用
     * @param sock 连接的sock号
     * @param pdutype PDU类型
     * @param data 接收到的数据
     * @param len  接收到的数据长度
     * @return 零 表正常 ;非零表出错
     */
    int dillWithPDU (int sock, unsigned short pdutype, CVOID *buf,
                     unsigned long len);

    void *WriteData(void *arg);				//读线程

    void *ReadData(void *arg);				//写线程

protected:

    enum{ LOADTIMERID = 1000, LOADTIMEVAL = 10000 };

private:

    bool checkQueueEmpty(int type);		//判断读操作队列、写操作队列是否为空

    int getQueueSize(int type);		//获取读操作队列大小

    //Concurrent& getQueueTop(int type);		//获取读操作队列、写操作队列的首元素

    Concurrent* getQueueTop(int type);		//获取读操作队列、写操作队列的首元素

    void delQueueTop(int type);				//释放读操作队列、写操作队列的首元素

    /**
     *  @将接受的数据插入到队列尾
     *  @param type :读、写 队列的类型
     *  @param Concurrent 回调的数据封装后的类
     */
    void insertQueue(int type,Concurrent &concurrent);

    /**
     * 从配置文件里导入配置信息
     * @param pFile 配置文件名
     * @return 零 表正常 ;非零表出错
     */
    int loadConfig (CCHAR *pFile);

private:
    CDBCommSockEvent m_tcpSockEvent;

    CTCPAcceptor m_tcpAcceptor;

    std::string m_ip;

    unsigned short m_port;

    unsigned short serverType_;

    unsigned short serverIndex_;

    std::list < int >m_sockList;        //所有与数据库服务器连接的sock表

    CEDUDBComm *m_pEDUDBComm;

    CDBConfigClass *m_pDBConfigClass;

    void *senddata;

    void *senddata2;

    read_Queue  		read_queue;				//存放读操作数据队列

    write_Queue 		write_queue;			//存放写操作数据队列

    CThread 			m_writeThread;			//读操作线程对象

    CThread				m_readThread;			//写操作线程对象

    ConcurrenReadThread readThread;

    ConcurrenWriteThread writeThread;
};

extern CDBCommServ g_theServ;

#endif	//DBCOMMSERV_H
