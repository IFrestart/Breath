/**
 * Copyright (c) 2015, HangZhou Webcon Corporation.

 * @file	 DBCommServ.h
 * @author	 shizc <shizc@webcon.com.cn>

 * @short   ���ݿ�ͨ�ŷ�����
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

/** �ϴ��ļ�ģ�� */
//��Ӽ�������ļ���������
{DBSUB_AddDataInfo_Req_FromNHSystem, &CEDUDBComm::OnAddDataInfo, DBSUB_AddDataInfo_Rsp_ToNHSystem},

//���openid
//{AddOpenId_Req_WebToDB ,&CEDUDBComm::OnAddOpenId, AddOpenId_Rsp_DBToWeb},
//openid���豸��΢�Ź��ں��ã�
{BindOpenid_Req_WebToDB, &CEDUDBComm::OnBindBreath,BindOpenid_Rsp_DBToWeb},
//����󶨣�΢�Ź��ں��ã�
{DelBindOpenid_Req_WebToDB, &CEDUDBComm::OnDelOpenidBind,DelBindOpenid_Rsp_DBToWeb},
//�ն��豸���
{AddTerminalInfo_REQ_FromWeb,&CEDUDBComm::OnAddTerminalInfo, AddTerminalInfo_RSP_ToWeb},
//�����ն��豸
{DelTerminalInfo_REQ_FromWeb,&CEDUDBComm::OnDelTerminalInfo, DelTerminalInfo_RSP_ToWeb},

//��ӱ�����Ϣ
{DBSUB_AddAlarmInfo_Req_FromNHSystem, &CEDUDBComm::OnAddBreathAlarmInfo, DBSUB_AddAlarmInfo_Rsp_ToNHSystem},
//��ӱ�������2
//{AddAlarmInfo2_Req_FromNHSystem, &CEDUDBComm::OnAddSpaceStat, AddAlarmInfo2_Rsp_ToNHSystem},

//���ø���ʱ��
{SetIpeTime_REQ_FromWeb,&CEDUDBComm::OnSetIpeTime,SetIpeTime_RSP_ToDB},

{AddHX_Req_WebToDB,&CEDUDBComm::OnAddHX,AddHX_Rsp_DBToWeb},
{0, NULL, 0}
};

struct PubPduFunList ReadSubPduList[] = {
//���openid
//{CheckOpenId_Req_WebToDB,&CEDUDBComm::OnCheckOpenId, CheckOpenId_Rsp_DBToWeb},
//��ȡopenid�󶨿����б�
{GetBandZDList_Req_WebToDB, &CEDUDBComm::OnGetBandZDList, GetBandZDList_Rsp_DBToWeb},
{GetBandZhDList_Req_WebToDB, &CEDUDBComm::OnGetBandZhDList,GetBandZhDList_Rsp_DBToWeb},

//�鿴��������豸
{GetTerminalList_REQ_FromWeb,&CEDUDBComm::OnGetTerminalList,GetTerminalList_RSP_ToWeb},

// ��ñ�����Ϣ
{GetAlarmInfoListPage_Req_FromCli, &CEDUDBComm::OnGetBreathAlarmInfo, GetAlarmInfoListPage_Rsp_ToCli},
// ��ü����Ϣ
//{GetDataInfoListPage_Req_FromCli, &CEDUDBComm::OnGetDataInfo, GetDataInfoListPage_Rsp_ToCli},
//��ȡ��������ͼ
{GetAlarmInfoData_Req_FromCli,&CEDUDBComm::OnGetAlarmInfoData,GetAlarmInfoData_Rsp_ToCli},

//���Ժ����¼�����
{tryBreathInfo_Req_WebToDB,&CEDUDBComm::OntryBreathEntRpt,tryBreathInfo_Rsp_DBToWeb},

//��ȡ����ʱ�����
{GetIpeTime_REQ_FromCli,&CEDUDBComm::OnGetIpeTime,GetIpeTime_RSP_ToCli},
//��ȡ������Ϣ2
{GetAlarmInfoListPage2_Req_FromCli, &CEDUDBComm::OnGetAlarmInfo2, GetAlarmInfoListPage2_Rsp_ToCli},

{GetSleepInfo_REQ_FromCli, &CEDUDBComm::OnGetSleepInfo, GetSleepInfo_RSP_ToCli},

//��ȡ����������
{GetBRInfo_REQ_FromCli, &CEDUDBComm::OnGetBRInfo, GetBRInfo_RSP_ToCli},

{GetInsomInfo_REQ_FromCli, &CEDUDBComm::OnGetInsomInfo,GetInsomInfo_RSP_ToCli},

// ��ȡ�쳣ͳ��
{Abnormal_statistics_Req_FromCLI,&CEDUDBComm::OnGetAbnormal,Abnormal_statistics_Rsp_ToCLI},

//
{getHX_Req_WebToDB,&CEDUDBComm::OnGetHX,getHX_Rsp_DBToWeb},
{0, NULL, 0}
};


struct PubPduFunList WritePubPduList[] = {
 /*******************************************************
 * ��ͨ��pdu
 */

//�û���¼
//{MAINCERT_REQ_TODBCOMM, &CEDUDBComm::OnLogin, MAINCERT_RSP_FROMDBCOMM},
{inStat_BaseSmoothPara_REQ_TODBCOMM, &CEDUDBComm::OninStat_BaseSmoothPara, inStat_BaseSmoothPara_RSP_FROMDBCOMM},
{0, NULL, 0}
};

struct PubPduFunList ReadPubPduList[] = {
    /*******************************************************
 * ��ͨ��pdu
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
     * �������ݿ�������Ļص�����
     * @return 0 ��ɹ�
     */
    virtual int OnNewConnection (int sock);

    /**
     * ���ݵ����Ļص�����
     * @param sock ���ӵ�sock��
     * @param pdutype PDU����
     * @param data ���յ�������
     * @param len  ���յ������ݳ���
     * @return 0 ��ɹ�
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
     * ���ݵ����Ĵ����� ��OnTCPPacketReceived�����
     * @param sock ���ӵ�sock��
     * @param pdutype PDU����
     * @param data ���յ�������
     * @param len  ���յ������ݳ���
     * @return �� ������ ;��������
     */
    int dillWithPDU (int sock, unsigned short pdutype, CVOID *buf,
                     unsigned long len);

    void *WriteData(void *arg);				//���߳�

    void *ReadData(void *arg);				//д�߳�

protected:

    enum{ LOADTIMERID = 1000, LOADTIMEVAL = 10000 };

private:

    bool checkQueueEmpty(int type);		//�ж϶��������С�д���������Ƿ�Ϊ��

    int getQueueSize(int type);		//��ȡ���������д�С

    //Concurrent& getQueueTop(int type);		//��ȡ���������С�д�������е���Ԫ��

    Concurrent* getQueueTop(int type);		//��ȡ���������С�д�������е���Ԫ��

    void delQueueTop(int type);				//�ͷŶ��������С�д�������е���Ԫ��

    /**
     *  @�����ܵ����ݲ��뵽����β
     *  @param type :����д ���е�����
     *  @param Concurrent �ص������ݷ�װ�����
     */
    void insertQueue(int type,Concurrent &concurrent);

    /**
     * �������ļ��ﵼ��������Ϣ
     * @param pFile �����ļ���
     * @return �� ������ ;��������
     */
    int loadConfig (CCHAR *pFile);

private:
    CDBCommSockEvent m_tcpSockEvent;

    CTCPAcceptor m_tcpAcceptor;

    std::string m_ip;

    unsigned short m_port;

    unsigned short serverType_;

    unsigned short serverIndex_;

    std::list < int >m_sockList;        //���������ݿ���������ӵ�sock��

    CEDUDBComm *m_pEDUDBComm;

    CDBConfigClass *m_pDBConfigClass;

    void *senddata;

    void *senddata2;

    read_Queue  		read_queue;				//��Ŷ��������ݶ���

    write_Queue 		write_queue;			//���д�������ݶ���

    CThread 			m_writeThread;			//�������̶߳���

    CThread				m_readThread;			//д�����̶߳���

    ConcurrenReadThread readThread;

    ConcurrenWriteThread writeThread;
};

extern CDBCommServ g_theServ;

#endif	//DBCOMMSERV_H
