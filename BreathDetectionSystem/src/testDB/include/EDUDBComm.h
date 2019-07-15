/**
 * Copyright (c) 2015, HangZhou Webcon Corporation.

 * @file     EDUDBComm.h
 * @author   shizc <shizc@webcon.com.cn>
 * @short
 * @date     07/2015
 **/
#ifndef __EDUDBCOMM_H__
#define __EDUDBCOMM_H__

#include <stdlib.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>

#include <pthread.h>
#include "EDUDBMgr.h"
#include "Common.h"
#include "TypeDef.h"

//#include "ByteStream.h"
#include "Timer.h"

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>

#define MAX_OUTBUF_SIZE 1024*8
#define TIMERTODB 1000*60
#define DELETEVERIFYCODE 1000*60*60

using namespace std;
//////////////////////////////////////////////////////////////////////////////////////
class CEDUDBComm:public ITimerEventCallBack
{
public:
    CEDUDBComm ();
    virtual ~ CEDUDBComm ();

public:
    /**
    * ��ʼ��
    * @return	��ʼ���ɹ� EDUDB_ERR_DBCONN_LOST;��ʼ��ʧ�� DB_ERR_SUCCESS
    */
    int Initialize ();

    /**
    * ������ݿ������������
    */
    void CheckConnection ();

    /**
     * ��ʱ�������ʱ�Ļص�����
     * @param timerID �Լ����������
     * @param arg  Ҫ���Ĳ���
     * @return	���� ��ʾ�˳���ʱ��; 0 ��ʾ����
    */
    virtual int OnTimer (DWORD timerID, void *arg);

    /**
    * ���ݿⶨʱ���й���
    */
    void DBTimerTasker ();

    /**
    * ���ô�ӡ��Ϣģʽ������
    * @param nMsgMod
    * @param nMsgTrace
    * @return
    */
    void SetTraceType (int nMsgMod, int nMsgTrace);

    /**
    * ��ȡ�����ļ����������ļ��еĺϷ��������������������������ֵ���ɣ������ֵ��������ĳ�Ա�����У����ṩ��ȡ��Щ���������ֵ�Ľӿڡ�
    * @param pDBConfigClass CDBConfigClass��һʵ��
    * @return
    */
    void SetDBConfigClass (CDBConfigClass *pDBConfigClass);

public:
    /**
    * @param inbuf PDU���ǰ���յ����ݵĻ�����
    * @param inlen PDU���ǰ���յ����ݻ������ĳ���
    * @param outbuf PDU���������ݻ�����
    * @param outlen PDU���������ݻ���������
    */

//* ���ںŰ�*****************************************************************************************/
    //���openid�Ƿ����
    //int OnCheckOpenId(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡopenid�󶨿����б�
    int OnGetBandZDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //���豸��΢�Ź��ں��ã�
    int OnBindBreath(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //���openid�󶨵��豸
    int OnDelOpenidBind(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ���豸�б�
    int OnGetBandZhDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
//**��������*******************************************************************************************/
    //��ȡ������Ϣ
    int OnGetBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ������Ϣ2
    int OnGetAlarmInfo2(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ��������ͼ
    int OnGetAlarmInfoData(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ӱ�����Ϣ
    int OnAddBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ӱ�����Ϣ2
//    int OnAddSpaceStat(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ����ʱ�����
    int OnGetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //���ø���ʱ�����
    int OnSetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

//**�쳣ͳ��*******************************************************************************************/
    //��Ӽ����������
    int OnAddDataInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ��������ļ�
//    int OnGetDataInfo (CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //��ȡ�쳣˯��ͳ��
    int OnGetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //����ͳ�ƽ��
    int OnSetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //���Ժ����¼�����
    int OntryBreathEntRpt(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

//***************************************************************************************************/    
    //��ȡδ�����ļ�Ŀ¼
    int OngetUnprocDir(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //��ȡ״̬�ļ��б�
    int OngetStatFilelist(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //������ƽ�������õ���״̬���
    int OninStat_BaseSmoothPara(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnGetSleepInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //��ȡ����������
    int OnGetBRInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnGetInsomInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnAddHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    int OnGetHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //�ն��豸���
    int OnAddTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //�����ն��豸
    int OnDelTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    int OnGetTerminalList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

private:
    CEDUDBMgr *m_pEDUDBMgr;
    CDBConfigClass *m_pDBConfigClass;
    CMMTDBTrace *m_pDBTrace;
    CTimer m_pTimer;
    CTimer m_pTimer2;
    std::ostringstream oss;
};


#endif //__EDUDBCOMM_H__

