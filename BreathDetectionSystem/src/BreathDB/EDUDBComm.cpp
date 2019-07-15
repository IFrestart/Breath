/**
* Copyright(c) 2015, HangZhou Webcon Corporation.

* @file     EDUDBComm.cpp
* @author   shizc <shizc@webcon.com.cn>
* @short
* @date     07/2015
**/

#include <sys/timeb.h>
#include "EDUDBComm.h"

#define MAX_LIST_COUNT 255

CEDUDBComm::CEDUDBComm ()
{
    m_pEDUDBMgr = NULL;
    m_pDBTrace = NULL;
}

CEDUDBComm::~CEDUDBComm ()
{
    if (m_pEDUDBMgr)
        delete m_pEDUDBMgr;
    m_pEDUDBMgr = NULL;

    if (m_pDBTrace)
        delete m_pDBTrace;
    m_pDBTrace = NULL;
}

/**
 * 初始化
 * @return	初始化成功 EDUDB_ERR_DBCONN_LOST;初始化失败 DB_ERR_SUCCESS
 */
int CEDUDBComm::Initialize ()
{
    m_pDBTrace->TraceFunc ("Initialize() begin\n");

    if (m_pDBConfigClass == NULL)
    {
        m_pDBTrace->TraceError ("m_pDBConfigClass is NULL\n");
        return EDUDB_ERR_DBCONN_LOST;
    }

    if (m_pDBConfigClass->GetEDUDBSvrCount () <= 0)
    {
        m_pDBTrace->TraceError("m_pDBConfigClass->GetEDUDBSvrCount() = %d\n", m_pDBConfigClass->GetEDUDBSvrCount ());
        return EDUDB_ERR_DBCONN_LOST;
    }

    m_pEDUDBMgr = new CEDUDBMgr (m_pDBConfigClass);
    if (m_pEDUDBMgr == NULL)
    {
        m_pDBTrace->TraceError("m_pEDUDBMgr = new CEDUDBMgr(m_pDBConfigClass) failed\n");
        return EDUDB_ERR_DBCONN_LOST;
    }

    if (m_pEDUDBMgr->ConnectToDB (0) != EDUDB_ERR_DBCONN_OK)
    {
        m_pDBTrace->TraceError ("m_pEDUDBMgr->ConnectToDB(0) failed\n");
        return EDUDB_ERR_DBCONN_LOST;
    }

    assert (0 == m_pTimer.start (1, TIMERTODB, this, (void *) 0));
    assert (0 == m_pTimer2.start (2, DELETEVERIFYCODE, this, (void *) 0));

    m_pDBTrace->TraceH1 ("Initialize success!!!\n");
    m_pDBTrace->TraceFunc ("Initialize() end\n");

    return DB_ERR_SUCCESS;
}

/**
 * 定时器间隔到时的回调函数
 * @param timerID 自己定义的类型
 * @param arg  要带的参数
 * @return	非零 表示退出计时器; 0 表示返回
 */
int CEDUDBComm::OnTimer (DWORD timerID, void *arg)
{
    switch (timerID)
    {
    case 1:
        CheckConnection ();
        break;
    case 2:
        DBTimerTasker ();
        break;
    default:
        break;
    }
    return (0);
}

/**
 * 检查数据库连接正常与否
 */
void CEDUDBComm::CheckConnection ()
{
    m_pDBTrace->TraceFunc ("CheckConnection() begin\n");
    int nRet = 0;

    if (m_pEDUDBMgr)
    {
        nRet = m_pEDUDBMgr->CheckConnection (0);
    }

    time_t timeCur;
    time (&timeCur);
    m_pDBTrace->TraceVar ("nRet=[%d], [%d] CheckConnection\n", nRet,(int) timeCur);
    m_pDBTrace->TraceFunc ("CheckConnection() end\n");
}

/**
 * 数据库定时例行过程
 */
void CEDUDBComm::DBTimerTasker ()
{
    m_pDBTrace->TraceFunc ("DBTimerTasker() begin\n");
    int nRet = 0;

    if (m_pEDUDBMgr)
    {
        nRet = m_pEDUDBMgr->DBTimerTasker ();
    }
}

/*SetTraceType*/
void CEDUDBComm::SetTraceType (int nMsgMod, int nMsgTrace)
{
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetTraceType() begin\n");

    if (m_pDBTrace == NULL)
    {
        m_pDBTrace = new CMMTDBTrace (MMTDB_MODULE_EDUDBCOMM);
        if (m_pDBTrace)
            m_pDBTrace->SetModAndTrace (nMsgMod, nMsgTrace);
    }

    if (m_pEDUDBMgr)
        m_pEDUDBMgr->SetTraceType (nMsgMod, nMsgTrace);

    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetTraceType() end\n");
}

/**
 * SetDBConfigClass
 */
void CEDUDBComm::SetDBConfigClass (CDBConfigClass * pDBConfigClass)
{
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetDBConfigClass() begin\n");

    m_pDBConfigClass = pDBConfigClass;

    if (m_pDBConfigClass)
        SetTraceType (m_pDBConfigClass->GetMsgMod (),m_pDBConfigClass->GetMsgTrace ());

    if (m_pEDUDBMgr)
        m_pEDUDBMgr->SetDBConfigClass (m_pDBConfigClass);

    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetDBConfigClass() end\n");

}

//* 公众号绑定*****************************************************************************************/
#if 0
//检测openid是否存在
int CEDUDBComm::OnCheckOpenId(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    OpenidInfo stru;
    memset(&stru, 0, sizeof(DataInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szOpenid, sizeof (stru.szOpenid));
        m_pDBTrace->TraceVarIn ("OnCheckOpenId --serverID=[%s]\n",stru.szOpenid);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnCheckOpenId() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->CheckOpenId(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnCheckOpenId error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnCheckOpenId() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;

}
#endif

//获取openid绑定设备
int CEDUDBComm::OnGetBandZDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    OpenidInfo stru;
    memset(&stru, 0, sizeof(OpenidInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szOpenid, sizeof (stru.szOpenid));
        stru.ushUserType=1;
        m_pDBTrace->TraceVarIn ("OnGetBandZDList --OpenID=[%s]\n",stru.szOpenid);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetBandZDList() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        OpenidInfoArray Array;
        nRet = (short) m_pEDUDBMgr->GetBandZDList(stru,Array);
        if (DB_ERR_SUCCESS == nRet )
        {
            WORD wCount = Array.size ();
            CSockStream arDB ;
            arDB << nRet;
            arDB << wCount;
            //printf("OnGetBandZDList() wAllPageNum=[%d]\n",wAllPageNum);
            for (int i =0; i < wCount; i++ )
            {
                arDB.writeString (Array[i].szServerid);
                m_pDBTrace->TraceVarOut("OnGetBandZDList() - i=[%d], ServerId=[%s]\n",
                                        i, Array[i].szServerid);
            }
            m_pDBTrace->TraceVarIn("OnGetBandZDList() - nRet=[%d], wCount[%d]\n", nRet,wCount);

            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetUserIdList - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取openid绑定设备
int CEDUDBComm::OnGetBandZhDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    OpenidInfo stru;
    memset(&stru, 0, sizeof(OpenidInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szOpenid, sizeof (stru.szOpenid));
        stru.ushUserType = 2;
        m_pDBTrace->TraceVarIn ("OnGetBandZDList --OpenID=[%s]\n",stru.szOpenid);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetBandZDList() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        OpenidInfoArray Array;
        nRet = (short) m_pEDUDBMgr->GetBandZDList(stru,Array);
        if (DB_ERR_SUCCESS == nRet )
        {
            WORD wCount = Array.size ();
            CSockStream arDB ;
            arDB << nRet;
            arDB << wCount;
            //printf("OnGetBandZDList() wAllPageNum=[%d]\n",wAllPageNum);
            for (int i =0; i < wCount; i++ )
            {
                arDB.writeString (Array[i].szServerid);
                m_pDBTrace->TraceVarOut("OnGetBandZDList() - i=[%d], ServerId=[%s]\n",
                                        i, Array[i].szServerid);
            }
            m_pDBTrace->TraceVarIn("OnGetBandZDList() - nRet=[%d], wCount[%d]\n", nRet,wCount);

            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetUserIdList - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//绑定设备（微信公众号用）
int CEDUDBComm::OnBindBreath(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    OpenidInfo stru;
    memset(&stru, 0, sizeof(OpenidInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szServerid,sizeof(stru.szServerid));
        ar.readString (stru.szOpenid, sizeof (stru.szOpenid));
        ar.readString (stru.szPassword,sizeof(stru.szPassword));
        //m_pDBTrace->TraceVarIn ("BindBreath --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("BindBreath() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        m_pDBTrace->TraceVarOut("BindBreath start \n");
        nRet = (short) m_pEDUDBMgr->BindBreath(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("BindBreath error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("BindBreath() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}

//解除openid绑定的设备
int CEDUDBComm::OnDelOpenidBind(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    OpenidInfo stru;
    memset(&stru, 0, sizeof(OpenidInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        //m_pDBTrace->TraceVarOut ("DelBindBreath start\n");
        ar.readString (stru.szServerid,sizeof(stru.szServerid));
        ar.readString (stru.szOpenid, sizeof (stru.szOpenid));
        //m_pDBTrace->TraceVarOut ("DelBindBreath --szOpenid=[%s]\n",stru.szOpenid);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("DelBindBreath() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->DelBind(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("DelBindBreath error nRet:[%d] \n", nRet);
    }

    arDB << nRet;
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}

//**报警功能*******************************************************************************************/
//获取报警信息
int CEDUDBComm::OnGetBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    PageList List;
    memset(&List, 0, sizeof(List));
    BreathAlarmInfo stru;
    memset(&stru, 0, sizeof(stru));
    CSockStream ar ((BYTE *) inbuf, inlen);
    try
    {
        ar.readString(stru.szServerId,sizeof(stru.szServerId));
        ar >> stru.wAlarmType;
        ar >> stru.dwStartTime;
        ar >> stru.dwLimitTime;
        ar >> List.wPageNum;
        ar >> List.wListNum;
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetBreathAlarmInfo() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        BreathAlarmInfoArray Array;
        nRet = (short) m_pEDUDBMgr->GetBreathAlarmInfo(List, stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << List.wAllPageNum;
            arDB << List.wPageNum;
            arDB << wCount;

            //printf("OnGetChangZhanList() wAllPageNum=[%d]\n",wAllPageNum);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].wAlarmType;
                arDB << Array[i].dwAlarmTime;
                arDB << Array[i].shAlarmdur;
                m_pDBTrace->TraceVarOut("OnGetBreathAlarmInfo - i=[%d], AlarmType=[%d], AlarmTime=[%d], Alarmdur[%d]\n",
                                        i, Array[i].wAlarmType, Array[i].dwAlarmTime, Array[i].shAlarmdur);
            }

            m_pDBTrace->TraceVarOut("OnGetBreathAlarmInfo - nRet=[%d] wAllPageNum=[%d] wPageNum=[%d] wCount =[%d]\n",
                                    nRet,List.wAllPageNum,List.wPageNum, wCount);

            outlen = arDB.tell();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData(), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取报警信息
int CEDUDBComm::OnGetAlarmInfo2(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    PageList List;
    memset(&List, 0, sizeof(List));
    BreathAlarmInfo2 stru;
    memset(&stru, 0, sizeof(stru));
    CSockStream ar ((BYTE *) inbuf, inlen);
    try
    {
        ar.readString(stru.szServerId,sizeof(stru.szServerId));
        ar >> stru.wAlarmType;
        ar >> stru.dwStartTime;
        ar >> stru.dwLimitTime;
        ar >> List.wPageNum;
        ar >> List.wListNum;
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo2 Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetBreathAlarmInfo2() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        BreathAlarmInfoArray2 Array;
        nRet = (short) m_pEDUDBMgr->GetAlarmInfo2(List, stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << List.wAllPageNum;
            arDB << List.wPageNum;
            arDB << wCount;

            //printf("OnGetChangZhanList() wAllPageNum=[%d]\n",wAllPageNum);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].wAlarmType;
                arDB << Array[i].dwAlarmTime;
                arDB << Array[i].wAlarmParam;
                m_pDBTrace->TraceVarOut("OnGetBreathAlarmInfo - i=[%d], AlarmType=[%d], AlarmTime=[%d], wAlarmParam[%d]\n",
                                        i, Array[i].wAlarmType, Array[i].dwAlarmTime, Array[i].wAlarmParam);
            }

            m_pDBTrace->TraceVarOut("OnGetBreathAlarmInfo - nRet=[%d] wAllPageNum=[%d] wPageNum=[%d] wCount =[%d]\n",
                                    nRet,List.wAllPageNum,List.wPageNum, wCount);

            outlen = arDB.tell ();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取报警波形图
int CEDUDBComm::OnGetAlarmInfoData(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    BreathAlarmInfo stru;
    memset(&stru, 0, sizeof(stru));
    CSockStream ar ((BYTE *) inbuf, inlen);
    try
    {
        ar.readString(stru.szServerId,sizeof(stru.szServerId));
        ar >> stru.wAlarmType;
        ar >> stru.dwAlarmTime;
        cout << "OnGetAlarmInfoData--- Alarmtime=" << stru.dwAlarmTime << endl;
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetAlarmInfoData Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetAlarmInfoData() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        nRet = (short) m_pEDUDBMgr->GetAlarmInfoData(stru);

        if (DB_ERR_SUCCESS == nRet)
        {
            CSockStream arDB;
            arDB << nRet;
            arDB << stru.wAlarmType;
            arDB << stru.dwAlarmTime;
            arDB << stru.shAlarmdur;
            arDB << stru.wMessageLen;
            m_pDBTrace->TraceVarOut("OnGetAlarmInfoData - nRet=[%d], AlarmType=[%d], AlarmTime=[%d], Alarmdur[],MsgLen=[%d]\n",
                                    nRet,stru.wAlarmType, stru.dwAlarmTime,stru.shAlarmdur, stru.wMessageLen);
            arDB.writeBytes (stru.lpMassage,stru.wMessageLen);
            //cout << "OnGetAlarmInfoData()  len=" << stru.wMessageLen << endl;
            //dumpHex(stru.lpMassage,stru.wMessageLen);

            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetAlarmInfoData - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//添加报警信息
int CEDUDBComm::OnAddBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    BreathAlarmInfo stru;
    memset(&stru, 0, sizeof(BreathAlarmInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szServerId, sizeof (stru.szServerId));
        ar >> stru.wAlarmType;
        ar >> stru.dwAlarmTime;
        ar >> stru.shAlarmdur;
        ar >> stru.wMessageLen;

        m_pDBTrace->TraceVarIn ("OnAddBreathAlarmInfo -- ServerId=[%s],AlarmType=[%d],Alarmdur[%d],alarmtime=[%d],Len=[%d]\n",
                                stru.szServerId, stru.wAlarmType, stru.shAlarmdur, stru.dwAlarmTime, stru.wMessageLen);

        stru.lpMassage =new BYTE[stru.wMessageLen];
        if(ar.readBytes (stru.lpMassage,stru.wMessageLen) != 0)
        {
            m_pDBTrace->TraceError ("OnAddBreathAlarmInfo() \n");
            nRet = EDUDB_ERR_PDUPARA;
        }
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnAddBreathAlarmInfo() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->AddBreathAlarmInfo(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnAddBreathAlarmInfo error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnAddBreathAlarmInfo() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}

//添加报警信息2
#if 0
int CEDUDBComm::OnAddSpaceStat(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    BreathAlarmInfo2 stru;
    memset(&stru, 0, sizeof(BreathAlarmInfo2));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szServerId, sizeof (stru.szServerId));
        ar >> stru.wAlarmType;
        ar >> stru.dwAlarmTime;
        ar >> stru.wAlarmParam;

        m_pDBTrace->TraceVarIn ("OnAddBreathAlarmInfo2 -- ServerId=[%s],AlarmType=[%d],AlarmParam[%d],alarmtime=[%d]\n",
                                stru.szServerId, stru.wAlarmType, stru.wAlarmParam, stru.dwAlarmTime);

    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnAddBreathAlarmInfo2() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->AddSpaceStat(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnAddBreathAlarmInfo2 error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    arDB << stru.dwAlarmTime;
    m_pDBTrace->TraceVarIn("OnAddBreathAlarmInfo2() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}
#endif
//**异常统计*******************************************************************************************/
//添加检测数据索引
int CEDUDBComm::OnAddDataInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    DataInfo stru;
    memset(&stru, 0, sizeof(DataInfo));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szZhdXlh, sizeof (stru.szZhdXlh));
        ar >> stru.wType;
        ar >> stru.dwUploadtime;
        ar.readString (stru.szFileName, sizeof (stru.szFileName));
        m_pDBTrace->TraceVarIn ("OnAddDataInfo --ZhdID=[%s], type=[%d], Uploadtime=[%d], filename=[%s]\n",
                                stru.szZhdXlh, stru.wType, stru.dwUploadtime, stru.szFileName);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnAddDataInfo() Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->AddDataInfo(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnAddDataInfo error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnAddDataInfo() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }
    memcpy (outbuf, arDB.getData (), outlen);

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取异常睡眠统计
int CEDUDBComm::OnGetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    AbnormalStat stru;
    memset(&stru, 0, sizeof(AbnormalStat));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szServerid,sizeof(stru.szServerid));
        ar >> stru.shtype;
        ar >> stru.dwtime;
        //m_pDBTrace->TraceVarIn ("OnGetAbnormal --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetAbnormal Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetDataInfo() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        AbnormalStatArray Array;
        nRet = (short) m_pEDUDBMgr->GetAbnormal(stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << stru.shtype;
            if(stru.shtype == 0)
            {
                arDB << stru.dwSleepBTime;
                arDB << stru.dwSleepETime;
                m_pDBTrace->TraceVarOut("OnGetAbnormal - SleepBTime=[%d] dwSleepETime=[%d]\n",
                                        stru.dwSleepBTime, stru.dwSleepETime);

            }
            arDB << wCount;
            m_pDBTrace->TraceVarOut("OnGetAbnormal - nRet=[%d] wCount =[%d]\n",
                                    nRet,wCount);

            //printf("OOnGetAbnormal wAllPageNum=[%d]\n",wAllPageNum);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].shNum;
                arDB << Array[i].dwDi;
                arDB << Array[i].dwZan;
                arDB << Array[i].dwBi;
                m_pDBTrace->TraceVarOut("OnGetAbnormal - i=[%d], Num=[%d], Di=[%d], Zan=[%d], Bi=[%d]\n",
                                        i, Array[i].shNum, Array[i].dwDi, Array[i].dwZan, Array[i].dwBi);
            }

            outlen = arDB.tell ();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetBreathAlarmInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}
//导入统计结果
int CEDUDBComm::OnSetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    AbnormalStat stru;
    memset(&stru, 0, sizeof(AbnormalStat));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szServerid,sizeof(stru.szServerid));
        ar >> stru.dwtime;
        //m_pDBTrace->TraceVarIn ("OnGetAbnormal --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetAbnormal Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }
}

//测试呼吸事件报告
int CEDUDBComm::OntryBreathEntRpt(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    BreathEntRptInfo stru;
    memset(&stru, 0, sizeof(AbnormalStat));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szZDxlh,sizeof(stru.szZDxlh));
        ar >> stru.dwSleepBTime;
        ar >> stru.dwSleepETime;
        //m_pDBTrace->TraceVarIn ("OntryBreathEntRpt --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OntryBreathEntRpt Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OntryBreathEntRpt() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        BreathEntRptInfoArray Array;
        nRet = (short) m_pEDUDBMgr->tryBreathEntRpt(stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            m_pDBTrace->TraceVarOut("OntryBreathEntRpt - nRet=[%d] wCount =[%d]\n",
                                    nRet,wCount);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].wType;
                arDB << Array[i].wNum;
                arDB << Array[i].shBaifenbi;
                arDB << Array[i].shLiejishijian;
                arDB << Array[i].shZuichang;
                arDB << Array[i].Fashengshijian;
                m_pDBTrace->TraceVarOut("OntryBreathEntRpt - i=[%d], type=[%d], Num=[%d], Baifenbi=[%d], Liejishijian=[%d], Zuichangi=[%d] ,Fashengshijian=[%d]\n",
                                        i, Array[i].wType, Array[i].wNum, Array[i].shBaifenbi, Array[i].shLiejishijian,Array[i].shZuichang,Array[i].Fashengshijian);
            }
            arDB << stru.SymptomType;
            arDB << stru.level;
            m_pDBTrace->TraceVarOut("OntryBreathEntRpt -sym=[%d], level=[%d]\n", stru.SymptomType, stru.level);

            outlen = arDB.tell ();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OntryBreathEntRpt - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取干涉时间参数
int CEDUDBComm::OnGetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    GsTime stru;
    memset(&stru, 0, sizeof(GsTime));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szZD_xlh,sizeof(stru.szZD_xlh));
        //m_pDBTrace->TraceVarIn ("OnGetIpeTime --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetIpeTime Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetIpeTime() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        nRet = (short) m_pEDUDBMgr->GetIpeTime(stru);

        if (DB_ERR_SUCCESS == nRet)
        {
            CSockStream arDB;
            arDB << nRet;
            arDB << stru.shOwnTime;
            arDB << stru.shRecomTime;
            m_pDBTrace->TraceVarOut("OnGetIpeTime - nRet=[%d] OwnTime=[%d], RecomTime=[%d]\n",
                                    nRet,stru.shOwnTime, stru.shRecomTime);
            outlen = arDB.tell ();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetIpeTime - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//设置干涉时间参数
int CEDUDBComm::OnSetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    GsTime stru;
    memset(&stru, 0, sizeof(GsTime));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szZD_xlh,sizeof(stru.szZD_xlh));
        ar >> stru.shOwnTime;
        //m_pDBTrace->TraceVarIn ("OnSetIpeTime --serverID=[%s]\n",stru.szNumber);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnSetIpeTime Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnSetIpeTime() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        nRet = (short) m_pEDUDBMgr->SetIpeTime(stru);

        if (DB_ERR_SUCCESS == nRet)
        {
            CSockStream arDB;
            arDB << nRet;
            arDB.writeString (stru.szZD_xlh);
            arDB << stru.shOwnTime;
            m_pDBTrace->TraceVarOut("OnSetIpeTime - nRet=[%d]\n", nRet);
            outlen = arDB.tell ();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnSetIpeTime - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取未处理文件目录
int CEDUDBComm::OngetUnprocDir(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    WORD tmp;
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        ar >> tmp;
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("Proc Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {
        FileDirArray Array;
        nRet = (short) m_pEDUDBMgr->getUnprocDir(Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            //m_pDBTrace->TraceVarOut("OngetUnprocDir() - nRet=[%d] wCount =[%d]\n", nRet,wCount);
            for (int i = 0; i < wCount; i++)
            {
                arDB.writeString(Array[i].szZD_xlh);
                m_pDBTrace->TraceVarOut("OngetUnprocDir() -i=[%d], dir=[%s]\n",
                                        i, Array[i].szZD_xlh);
            }
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OngetUnprocDir() - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取状态文件列表
int CEDUDBComm::OngetStatFilelist(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    SFpara  stru;
    memset(&stru,0,sizeof(SFpara));
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.ZhDXlh,sizeof(stru.ZhDXlh));
        ar >> stru.dwTime;
        ar >> stru.wNum;
        m_pDBTrace->TraceVarOut("OngetStatFilelist() -Xlh=[%s], time=[%d], num=[%d]\n",
                                stru.ZhDXlh,stru.dwTime,stru.wNum);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("Proc Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {

        FileDirArray Array;
        nRet = (short) m_pEDUDBMgr->getStatFilelist(stru,Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            arDB.writeString(stru.ZhDXlh);
            //m_pDBTrace->TraceVarOut("OngetStatFilelist() - nRet=[%d] wCount =[%d]\n", nRet,wCount);
            for (int i = 0; i < wCount; i++)
            {
                arDB.writeString (Array[i].szZD_xlh);
                m_pDBTrace->TraceVarOut("OngetStatFilelist() -i=[%d], file=[%s]\n",
                                        i, Array[i].szZD_xlh);
            }
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }
}

//入库根据平滑参数得到的状态结果
int CEDUDBComm::OninStat_BaseSmoothPara(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    Sleep stru;
    SleepArray  Array;
    BreathAlarmInfo2 SETime;
    memset(&SETime,0,sizeof(BreathAlarmInfo2));
    DWORD num;
    char    ZhDXlh[ID_MAXLEN];
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString(ZhDXlh, sizeof(ZhDXlh));
        ar >> num;
        for(DWORD i= 0; i<num; i++)
        {
            memset(&stru, 0, sizeof(stru));
            ar >> stru.dwTime;
            ar >> stru.dwBreathRate;
            ar >> stru.dwAHIS;
            ar >> stru.dwAHIO;
            ar >> stru.wSleepStat1;
            ar >> stru.wSleepStat2;
            ar >> stru.dwHeartRate;
            ar >> stru.dwRespRate;
            Array.push_back(stru);
            if(i==0)
            {
                SETime.dwStartTime = stru.dwTime;
            }
            else if(i==(num-1))
            {
                SETime.dwLimitTime = stru.dwTime;
            }
        }
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OninStat_BaseSmoothPara Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        const char* Xlh= ZhDXlh;
        nRet = (short) m_pEDUDBMgr->inStat_BaseSmoothPara(Xlh,num, Array,SETime);
    }
    else
    {
        m_pDBTrace->TraceError ("OninStat_BaseSmoothPara error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OninStat_BaseSmoothPara - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OninStat_BaseSmoothPara - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

int CEDUDBComm::OnGetSleepInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    TimeXlh stru;
    memset(&stru, 0, sizeof(TimeXlh));

    CSockStream ar((BYTE *) inbuf, inlen);

    cout << "OnGetSleepInfo:len" << inlen << endl;
    try
    {
        ar.readString(stru.szZD_xlh, sizeof(stru.szZD_xlh));
        ar >> stru.shGstime;

    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnGetSleepInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {
        SleepArray Array;
        nRet = (short) m_pEDUDBMgr->GetSleepInfo(stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size()*2; //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            m_pDBTrace->TraceVarOut("OnGetSleepInfo - nRet=[%d] wCount =[%d]\n", nRet,wCount);
            for (int i = 0; i < wCount/2; i++)
            {
                arDB << Array[i].dwTime;
                arDB << Array[i].wSleepStat1;
                arDB << (Array[i].dwTime+30);
                arDB << Array[i].wSleepStat2;
                m_pDBTrace->TraceVarOut("OnGetSleepInfo - i=[%d], time=[%d], wSleepStat=[%d], wSleepStat=[%d]\n",
                                        i, Array[i].dwTime, Array[i].wSleepStat1, Array[i].wSleepStat2);
            }
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetSleepInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//获取呼吸和心率
int CEDUDBComm::OnGetBRInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    TimeXlh stru;
    memset(&stru, 0, sizeof(TimeXlh));
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        ar.readString(stru.szZD_xlh, sizeof(stru.szZD_xlh));
        ar >> stru.shGstime;

    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnGetBRInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {
        SleepArray Array;
        nRet = (short) m_pEDUDBMgr->GetBRInfo(stru, Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            m_pDBTrace->TraceVarOut("OnGetBRInfo - nRet=[%d] wCount =[%d]\n", nRet,wCount);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].dwTime;
                arDB << Array[i].dwHeartRate;
                arDB << Array[i].dwRespRate;
                m_pDBTrace->TraceVarOut("OnGetBRInfo - i=[%d], time=[%d], HeartRate=[%d], RespRate=[%d]\n",
                                        i, Array[i].dwTime, Array[i].dwHeartRate, Array[i].dwRespRate);
            }
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetBRInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

int CEDUDBComm::OnGetInsomInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    ShiMian shim;
    memset(&shim, 0, sizeof(ShiMian));
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        ar.readString(shim.szZhD_xlh, sizeof(shim.szZhD_xlh));
        ar >> shim.dwflagdate;

    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnGetInsomInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {

        nRet = (short) m_pEDUDBMgr->GetInsomInfo(shim);

        if (DB_ERR_SUCCESS == nRet)
        {
            CSockStream arDB;
            arDB << nRet;
            arDB << shim.dwDelayTime;
            arDB << shim.dwAllSleep;
            arDB << shim.dwInBedTime;
            arDB << shim.dwWASO;
            arDB << shim.dwNA;
            m_pDBTrace->TraceVarOut("OnGetInsomInfo - nRet=[%d], DelayTime=[%d], AllSleep=[%d]\n"
                                    "InBedTime=[%d], WASO=[%d], NA=[%d]\n",
                                    nRet, shim.dwDelayTime, shim.dwAllSleep,
                                    shim.dwInBedTime, shim.dwWASO, shim.dwNA
                                    );
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if(nRet == DB_ERR_NOROW)
    {
        m_pDBTrace->TraceVarOut("time again cacl------------------------\n");
        char* cxlh = NULL;
        cxlh = shim.szZhD_xlh;
        TimeXlh stru;
        memset(&stru,0,sizeof(TimeXlh));
        strcpy(stru.szZD_xlh,cxlh);
        stru.shGstime = shim.dwflagdate + 25*60*60;
        m_pEDUDBMgr->SetSleepTime(stru,cxlh);
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetInsomInfo - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

int CEDUDBComm::OnAddHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    HX stru;
    memset(&stru, 0, sizeof(HX));
    CSockStream ar ((BYTE *) inbuf, inlen);

    try
    {
        ar.readString (stru.szZhD_xlh, sizeof (stru.szZhD_xlh));
        ar >> stru.dwLen;
        ar >> stru.dwTime;
        stru.lpMassage =new BYTE[stru.dwLen-4];
        if(ar.readBytes(stru.lpMassage,stru.dwLen-12) !=0)
        {
            m_pDBTrace->TraceError ("OnAddHX() \n");
            nRet = EDUDB_ERR_PDUPARA;
        }
        ar >> stru.dwRespRate;
        ar >> stru.dwHeartRate;
        m_pDBTrace->TraceVarIn ("OnAddHX -- ServerId=[%s],Len=[%d],time=[%d],Resp=[%d],Heart=[%d]\n",
                                stru.szZhD_xlh, stru.dwLen, stru.dwTime, stru.dwRespRate, stru.dwHeartRate);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnAddHX Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->AddHX(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnAddHX error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnAddHX() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}


int CEDUDBComm::OnGetHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    HX stru;
    memset(&stru, 0, sizeof(HX));
    CSockStream ar((BYTE *) inbuf, inlen);
    PageList plist;
    memset(&plist, 0, sizeof(PageList));

    try
    {
        ar.readString(stru.szZhD_xlh, sizeof(stru.szZhD_xlh));
        ar >> plist.wPageNum;
        ar >> plist.wListNum;
//        m_pDBTrace->TraceVarOut("OnGetHX - xlh=[%s], page=[%d], num=[%d]\n",
//                                stru.szZhD_xlh, plist.wPageNum, plist.wListNum);

    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnGetHX Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }


    if (DB_ERR_SUCCESS == nRet)
    {

        HXArray Array;
        nRet = (short) m_pEDUDBMgr->GetHX(stru,plist,Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << wCount;
            m_pDBTrace->TraceVarOut("OnGetHX - nRet=[%d] wCount =[%d]\n", nRet,wCount);
            for (int i = 0; i < wCount; i++)
            {
                arDB << Array[i].dwTime;
                arDB << Array[i].dwRespRate;
                arDB << Array[i].dwHeartRate;
                m_pDBTrace->TraceVarOut("OnGetHX - i=[%d], time=[%d], RespRate=[%d], HeartRate=[%d]\n",
                                        i, Array[i].dwTime, Array[i].dwRespRate, Array[i].dwHeartRate);
            }
            outlen = arDB.tell ();
            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }
            memcpy (outbuf, arDB.getData (), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetHX - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}

//终端设备入库
int CEDUDBComm::OnAddTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    ZhDInfo stru;
    memset(&stru, 0, sizeof(ZhDInfo));
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        m_pDBTrace->TraceVarOut("OnAddTerminalInfo\n");
        ar.readString(stru.szXlh, sizeof(stru.szXlh));
        ar >> stru.dwType;
        m_pDBTrace->TraceVarOut("OnAddTerminalInfo - xlh=[%s], type=[%d]\n",
                                stru.szXlh, stru.dwType);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnAddTerminalInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->AddTerminalInfo(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnAddTerminalInfo error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnAddTerminalInfo() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}
//销毁终端设备
int CEDUDBComm::OnDelTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    ZhDInfo stru;
    memset(&stru, 0, sizeof(ZhDInfo));
    CSockStream ar((BYTE *) inbuf, inlen);

    try
    {
        ar.readString(stru.szXlh, sizeof(stru.szXlh));
        //        m_pDBTrace->TraceVarOut("OnDelTerminalInfo - xlh=[%s]\n",
        //                                stru.szZhD_xlh);
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError("OnDelTerminalInfo Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    CSockStream arDB ;
    if (DB_ERR_SUCCESS == nRet)
    {
        nRet = (short) m_pEDUDBMgr->DelTerminalInfo(stru);
    }
    else
    {
        m_pDBTrace->TraceError ("OnDelTerminalInfo error nRet:[%d] \n", nRet);
    }
    arDB << nRet;
    m_pDBTrace->TraceVarIn("OnDelTerminalInfo() - nRet=[%d]\n", nRet);
    outlen = arDB.tell ();

    if (outlen > MAX_OUTBUF_SIZE)
    {
        delete[](char *) outbuf;
        outbuf = NULL;
        outbuf = new char[outlen];
    }

    memcpy (outbuf, arDB.getData (), outlen);
    return nRet;
}


int CEDUDBComm::OnGetTerminalList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen)
{
    short nRet = DB_ERR_SUCCESS;
    PageList List;
    memset(&List, 0, sizeof(List));
    ZhDInfo stru;
    memset(&stru, 0, sizeof(stru));
    CSockStream ar ((BYTE *) inbuf, inlen);
    try
    {
        ar >> stru.dwindex;
        ar >> List.wPageNum;
        ar >> List.wListNum;
    }
    catch (CSockStream::exception e)
    {
        m_pDBTrace->TraceError ("OnGetTerminalList Exception:%s \n", e.what ());
        nRet = EDUDB_ERR_PDUPARA;
    }

    if (DB_ERR_SUCCESS == nRet)
    {
        //m_pDBTrace->TraceVarIn ("OnGetTerminalList() -  wpageNum[%d] wListNum=[%d] \n", userList.wPageNum,userList.wListNum);

        FileDirArray Array;
        nRet = (short) m_pEDUDBMgr->GetTerminalList(List,Array);

        if (DB_ERR_SUCCESS == nRet)
        {
            WORD wCount = Array.size(); //实际个数
            CSockStream arDB;
            arDB << nRet;
            arDB << List.wAllPageNum;
            arDB << List.wPageNum;
            arDB << wCount;

            //printf("OnGetTerminalList() wAllPageNum=[%d]\n",wAllPageNum);
            for (int i =0; i < wCount; i++ )
            {
                arDB.writeString (Array[i].szZD_xlh);
                m_pDBTrace->TraceVarOut("OnGetTerminalList() - i=[%d], ZhDXlh=[%s]\n",
                                        i, Array[i].szZD_xlh);
            }
            m_pDBTrace->TraceVarIn("OnGetTerminalList() - nRet=[%d], wCount[%d]\n", nRet,wCount);

            outlen = arDB.tell();

            if (outlen > MAX_OUTBUF_SIZE)
            {
                delete[](char *) outbuf;
                outbuf = NULL;
                outbuf = new char[outlen];
            }

            memcpy (outbuf, arDB.getData(), outlen);
        }
    }

    if (nRet != DB_ERR_SUCCESS)
    {
        CSockStream arDB (16);
        arDB << nRet;
        m_pDBTrace->TraceError ("OnGetTerminalList - nRet=[%d]\n",nRet);
        outlen = arDB.tell ();
        memcpy (outbuf, arDB.getData (), outlen);
    }
}
