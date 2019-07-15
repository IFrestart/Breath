/**
 * Copyright (c) 2016, HangZhou Webcon Centeroration.
 * @file     EDUDBMgr.cpp
 * @author   shizc <shizc@webcon.com.cn>
 * @short
 * @date     06/2016
 **/
#include <sys/timeb.h>
#include "EDUDBMgr.h"
#include <stdio.h>
#include <stdlib.h>

CEDUDBMgr::CEDUDBMgr (CDBConfigClass *pDBConfigClass)
{
    m_nDBStatus = EDUDB_ERR_DBCONN_LOST;
    m_pDBTrace = NULL;
    m_nDBIdxID = 0;
    int i = 0;

    for (i = 0; i < EDUDB_CONST_CONN_NUM; i++)
    {
        m_pDBIdx[i] = NULL;
    }

    memset (&m_DBInfo, 0, sizeof (DBConfigSvrInfoOfDB));
    strcpy (m_DBInfo.szSvrIP, "localhost");
    strcpy (m_DBInfo.szUName, "mmtuser");
    strcpy (m_DBInfo.szPassword, "mmt.nhuser.1234");
    m_DBInfo.shSvrPort = 0;
    strcpy (m_DBInfo.szDBName, "edudb");
    memset (m_chChCode, 0, sizeof (m_chChCode));
    strcpy (m_chChCode, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    memset (m_chNumCode, 0, sizeof (m_chNumCode));
    strcpy (m_chNumCode, "0123456789");
    SetDBConfigClass (pDBConfigClass);
}

CEDUDBMgr::~CEDUDBMgr ()
{
    FreeAll (EDUDB_ERR_DBCONN_LOST);
    if (m_pDBTrace)
        delete m_pDBTrace;
    m_pDBTrace = NULL;
}

void CEDUDBMgr::SetDBConfigClass (CDBConfigClass * pDBConfigClass)
{
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetDBConfigClass() begin\n");
    m_pDBConfigClass = pDBConfigClass;
    if (m_pDBConfigClass)
        SetTraceType (m_pDBConfigClass->GetMsgMod (),
                      m_pDBConfigClass->GetMsgTrace ());
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetDBConfigClass() end\n");
}

void CEDUDBMgr::SetTraceType (int nMsgMod, int nMsgTrace)
{
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetTraceType() begin\n");
    if (m_pDBTrace == NULL)
    {
        m_pDBTrace = new CMMTDBTrace (MMTDB_MODULE_EDUDBMGR);
        if (m_pDBTrace)
            m_pDBTrace->SetModAndTrace (nMsgMod, nMsgTrace);
    }
    int i = 0;
    for (i = 0; i < EDUDB_CONST_CONN_NUM; i++)
    {
        if (m_pDBIdx[i])
            m_pDBIdx[i]->SetTraceType (nMsgMod, nMsgTrace);
    }
    if (m_pDBTrace)
        m_pDBTrace->TraceFunc ("SetTraceType() end\n");
}

/**
 * ¡¨Ω”µΩ ˝æ›ø‚
 * @return Success, EDUDB_CONST_CONN_LOST: Failure
 */
int CEDUDBMgr::ConnectToDB (int nIdx)
{
    m_pDBTrace->TraceFunc ("/*************************************************/\n");
    m_pDBTrace->TraceFunc ("ConnectToDB() begin\n");
    if (m_pDBConfigClass == NULL)
    {
        m_pDBTrace->TraceError ("m_pDBConfigClass is NULL\n");
        return EDUDB_ERR_DBCONN_LOST;
    }
    if (m_pDBConfigClass->GetEDUDBSvrInfo (nIdx, m_DBInfo) == -1)
    {
        m_pDBTrace->TraceError("m_pDBConfigClass->GetEDUDBSvrInfo(%lu, m_DBInfo) failed\n", nIdx);
        return EDUDB_ERR_DBCONN_LOST;
    }
    m_nDBStatus = EDUDB_ERR_DBCONN_INIT;
    int i = 0;
    for (i = 0; i < EDUDB_CONST_CONN_NUM; i++)
    {
        m_pDBIdx[i] = new CMyDB ();
        if (m_pDBIdx[i] == NULL)
        {
            m_pDBTrace->TraceError("m_pDBIdx[%lu] = new CMyDB() failed\n", i);
            return FreeAll (EDUDB_ERR_DBCONN_LOST);
        }
        m_pDBIdx[i]->SetTraceType (m_pDBConfigClass->GetMsgMod (),
                                   m_pDBConfigClass->GetMsgTrace ());
        int ret = DB_ERROR;
        ret = m_pDBIdx[i]->connect (m_DBInfo.szSvrIP,
                                    m_DBInfo.szUName,
                                    m_DBInfo.szPassword,
                                    m_DBInfo.shSvrPort,
                                    m_DBInfo.szDBName);
        if (ret == DB_ERROR)
        {
            m_pDBTrace->
                    TraceError
                    ("Connect to edu db failed! m_pDBIdx[%lu] %s\n",
                     i, m_pDBIdx[i]->errMsg ());
            return FreeAll (EDUDB_ERR_DBCONN_LOST);
        }
    }
    m_nDBStatus = EDUDB_ERR_DBCONN_OK;
    m_pDBTrace->TraceH1 ("Connected all!!\n");
    m_pDBTrace->TraceFunc ("ConnectToDB() end\n");
    m_pDBTrace->
            TraceFunc
            ("/*************************************************/\n");
    return EDUDB_ERR_DBCONN_OK;
}

/**
 * Check DB version
 * @return DB_ERR_DBVERSION_ERROR

int CEDUDBMgr::CheckDBVersion(int nDBIndex)
{
    int nAffectedRows = m_pDBIdx[nDBIndex]->exec("select max(db_version_index)=16777217 from dbversion");
    char **row = NULL;
    if ( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("CheckDBVerison() - nAffectedRows=[%d]", RowsErr);
        return DB_ERR_DBDATA_ERROR;
    }
    else if (0 == nAffectedRows)
    {
        m_pDBTrace->TraceError("CheckDBVersion() - nAffectedRows=[0]\n");
        return DB_ERR_DBVERSION_ERROR;
    }
    else
    {
        if (NULL != (row=m_pDBIdx[nDBIndex]->getRow()))
        {
            if (1 == atoi(row[0]))
                return DB_ERR_SUCCESS;
        }
        else
        {
            m_pDBTrace->TraceError("CheckDBVerisosn() - row=NULL\n");
            return DB_ERR_DBVERSION_ERROR;
        }
    }
}
**/

/**
 * «Â≥˝ ˝æ›ø‚¡¨Ω”
 * @param nDBStatus EDUDB_ERR_DBCONN_LOST ªÚEDUDB_ERROR_DBCONN_OKªÚEDUDB_ERROR_DBCONN_INIT
 * @return EDUDB_ERR_DBCONN_LOST ªÚEDUDB_ERROR_DBCONN_OKªÚEDUDB_ERROR_DBCONN_INIT
 */
int CEDUDBMgr::FreeAll (int nDBStatus)
{
    m_pDBTrace->TraceFunc ("FreeAll() begin\n");
    m_nDBStatus = nDBStatus;
    int i = 0;
    for (i = 0; i < EDUDB_CONST_CONN_NUM; i++)
    {
        while (1)
        {
            if (m_pDBIdx[i] == NULL
                    || m_pDBIdx[i]->GetIsLocked () ==
                    EDUDB_CONST_ISUNLOCKED)
                break;
            m_pDBTrace->
                    TraceH2
                    ("FreeAll, DBIndex[%lu]=[%lu], sleep 1\n", i,
                     m_pDBIdx[i]->GetIsLocked ());
            sleep (1);
        }
        if (m_pDBIdx[i])
            delete m_pDBIdx[i];
        m_pDBIdx[i] = NULL;
        m_pDBTrace->TraceH2 ("FreeAll, connetion[%lu]\n", i);
    }
    m_nDBIdxID = 0;
    m_pDBTrace->TraceH1 ("FreeAll\n");
    m_pDBTrace->TraceFunc ("FreeAll() end\n");
    return m_nDBStatus;
}

/**
 * ºÏ≤È ˝æ›ø‚¡¨Ω”
 * @param nDBIndexID “™ºÏ≤Èµƒ ˝æ›ø‚¡¨Ω”∫≈
 * @return ’˝≥£ EDUDB_ERR_DBCONN_OK ; ∑«’˝≥£EDUDB_ERR_DBCONN_LOST
 */
int CEDUDBMgr::CheckConnection (int nDBIndex)
{
    if (nDBIndex < -1 && nDBIndex >= EDUDB_CONST_CONN_NUM)
        return EDUDB_ERR_DBCONN_NOCONN;
    if (m_nDBStatus != EDUDB_ERR_DBCONN_OK)
    {
        m_pDBTrace->TraceH3 ("CheckConnection, m_nDBStatus=[%lu]\n",
                             m_nDBStatus);
        if (m_nDBStatus == EDUDB_ERR_DBCONN_LOST)
        {
            m_pDBTrace->TraceH1 ("CheckConnection - lost\n");
            FreeAll (EDUDB_ERR_DBCONN_LOST);
            return ConnectToDB (m_DBInfo.nIdx);
        }
        return m_nDBStatus;
    }
    int i = 0;
    for (i = 0; i < EDUDB_CONST_CONN_NUM; i++)
    {
        if (m_pDBIdx[i]->GetIdleTime () > 180
                && m_pDBIdx[i]->GetIsLocked () == 0)
        {
            m_pDBTrace->TraceH4("CheckConnection, dbidx=[%lu], DBIdleTime=[%lu]\n",
                                i, m_pDBIdx[i]->GetIdleTime ());
            m_pDBIdx[i]->ThreadLock ();
            int nRet = m_pDBIdx[i]->db_ping ();
            if (nRet != 0)
            {
                m_pDBTrace->TraceH2("CheckConnection - db_ping, dbidx=[%lu], nRet=[%lu]\n",
                                    i, nRet);
                nRet = m_pDBIdx[i]->connect (m_DBInfo.szSvrIP,
                                             m_DBInfo.szUName,
                                             m_DBInfo.szPassword,
                                             m_DBInfo.shSvrPort,
                                             m_DBInfo.szDBName,
                                             DB_CONNFLAG_RECOON);
                if (nRet == DB_ERROR)
                {
                    m_pDBTrace->TraceH2("CheckConnection - reconnect, dbidx=[%lu], nRet=[%lu]\n",
                                        i, nRet);
                    m_nDBStatus = EDUDB_ERR_DBCONN_LOST;
                    m_pDBIdx[i]->ThreadUnlock ();
                    return FreeAll
                            (EDUDB_ERR_DBCONN_LOST);
                }
            }
            m_pDBIdx[i]->ThreadUnlock ();
        }
    }
    return EDUDB_ERR_DBCONN_OK;
}

unsigned int CEDUDBMgr::getMorningTime()
{
    time_t t = time(NULL);
    struct tm * tm= localtime(&t);
    if(tm->tm_hour != 9)
    {
        return 0;
    }
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return  mktime(tm);
}

int CEDUDBMgr::SetSleepTime(TimeXlh &stru,char *cxlh)
{
        int ret = RowsNot;
        ShiMian SMstru;
        memset(&SMstru,0,sizeof(ShiMian));
        if(ret != getSETime(stru, SMstru))
        {
	   m_pDBTrace->TraceError("getSETime error : %d \n",ret);
           return 0;
        }
        m_pDBTrace->TraceVarIn ("DBTimerTasker() stime=[%d], etime=[%d]\n",SMstru.dwStTime, SMstru.dwEndTime );
 
        strcpy(SMstru.szZhD_xlh,cxlh);
        SMstru.dwflagdate = stru.shGstime;
        SleepArray slArray;
        getSleepStatInfo(SMstru, slArray);
        int sAsize = slArray.size();

        int flagtime = 0;   //Êó∂Èó¥Ê†áËÆ∞
        int befortime = 0;  //Ââç‰∏Ä‰∏™Êó∂Èó¥   
        int allSleepCount= 0;   //ÊÄªÁù°Áú†ËÆ°Êï∞   
        int deepSleepCount = 0;
        int bedCount= 0;        //Âú®Â∫äËÆ°Êï∞
        int awakCount= 0;       //ËßâÈÜíËÆ°Êï∞
        int leaveCount = 0;     //Êó†‰∫∫ËÆ°Êï∞
        int awakleaveCount = 0; //ÂÖ•Áù°ÂâçÊó†‰∫∫   
        int NACount=0;         //ËøûÁª≠ËßâÈÜíËÆ°Êï∞
        int ahis = 0;
        int ahio = 0;
        WORD statflag = 0;   //Áä∂ÊÄÅÊ†áËØÜ
        SMstru.dwDelayTime ==0;
        for(int j=0; j<sAsize; j++)
        {
            flagtime = slArray[j].dwTime;
            statflag = slArray[j].wSleepStat1;
            ahis +=  slArray[j].dwAHIS;
            ahio +=  slArray[j].dwAHIO;
            if(statflag==4)
            {
                leaveCount++;
                if(SMstru.dwDelayTime==0)
                {
                    awakleaveCount++;
                }
            }
            else
            {
                bedCount++;
            }

            if(statflag == 3)
            {
                awakCount++;
                //ÊåÅÁª≠Ë∂ÖËøá5ÂàÜÈíüÔºàNA> 5ÔºâÁöÑËßâÈÜíÊ¨°Êï∞
                if(flagtime-befortime < 90)
                {
                    NACount++;
                }
                else
                {
                    if(NACount >= 5)
                    {
                        SMstru.dwNA++;
                    }
                    NACount = 1;
                }
                befortime = flagtime;
            }
            if(statflag < 3 )
            {
                allSleepCount++;
                if((SMstru.dwDelayTime == 0 && awakCount != 0)||(SMstru.dwDelayTime == 0 && allSleepCount >= 5))
                {
                    SMstru.dwDelayTime = flagtime - SMstru.dwStTime - awakleaveCount*60;
                }
                if(statflag == 0)
                {
                    deepSleepCount++;
                }
            }
        }
        if(allSleepCount==0)
        {
            SMstru.dwDelayTime = flagtime - SMstru.dwStTime - awakleaveCount*60;
        }
        SMstru.dwInBedTime = bedCount * 60;
        SMstru.dwAllSleep = allSleepCount * 60;

        int WASO=0;
        SMstru.dwWASO = 0;
        WASO = (bedCount-allSleepCount) * 60 - SMstru.dwDelayTime;
        if(WASO > 0)
        {
            SMstru.dwWASO   = WASO;
        }
        if(allSleepCount!=0)
        {
            SMstru.dwratio_of_sleep = deepSleepCount*100/allSleepCount;
        }
        SMstru.dwAHIS =ahis;
        SMstru.dwAHIO =ahio;

        m_pDBTrace->TraceVarIn ("DB ShiMian() stime=[%d], etime=[%d],\tDelayTime=[%d]\n"
                                "AllSleep=[%d], InBedTime=[%d]\n"
                                "WASO=[%d], NA=[%d]\n",
                                SMstru.dwStTime, SMstru.dwEndTime, SMstru.dwDelayTime,
                                SMstru.dwAllSleep, SMstru.dwInBedTime,
                                SMstru.dwWASO, SMstru.dwNA);

        insetSleepStat(SMstru);
}

/*∂® ±÷¥––»ŒŒÒ */
int CEDUDBMgr::DBTimerTasker ()
{
    if(getMorningTime() == 0)
    {
        return DB_ERR_SUCCESS;
    }
    FileDirArray Array;
    GetAllZhDxlh(Array);
    int size = Array.size();
    char* cxlh = NULL;
    for(int i=0;i < size; i++)
    {
        cxlh = Array[i].szZD_xlh;
        //m_pDBTrace->TraceVarIn ("DBTimerTasker() xlh=[%s]\n",cxlh );
        TimeXlh stru;
        memset(&stru,0,sizeof(TimeXlh));
        strcpy(stru.szZD_xlh,cxlh);
        m_pDBTrace->TraceVarIn ("DBTimerTasker() ZD_xlh=[%s]\n",stru.szZD_xlh );

        stru.shGstime = getMorningTime();
        //stru.shGstime = 1541635200;
	SetSleepTime(stru,cxlh);		
    }



    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::getSETime(TimeXlh &stru, ShiMian &SMstru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("setSleep_start_end_Time() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("setSleep_start_end_Time() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDId[ID_MAXLEN * 2 + 1];
    memset (szEscZDId, 0, sizeof(szEscZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(szEscZDId, stru.szZD_xlh, strlen(stru.szZD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //Êó∂Èó¥ÂÅèÁßªÊ†°ÂáÜÂÄº
   int hournum = 14*60*60;

    int i = 0;

    int startTime = stru.shGstime - 10*60*60;

    int startMaxTime = stru.shGstime - 17*60*60;
    int secnum = 30*60;
    do
    {
        oss.str ("");
        oss << "SELECT * "
            <<  "FROM T_sm_stat "
            <<      "WHERE ZhD_xlh='" << szEscZDId << "' and stat1!=4 "
            <<          "and stattime BETWEEN FROM_UNIXTIME("<< startTime << "- "<< (secnum*(i+1))  <<")"
            <<              "AND FROM_UNIXTIME("<< startTime << "-" << (secnum*i) <<")";
        nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
        if (nAffectedRows == RowsErr)
        {
            m_pDBIdx[nDBIndex]->ThreadUnlock();
            return DB_ERR_DBDATA_ERROR;
        }
        i++;
        if((startTime-secnum*i)<startMaxTime)
        {
            break;
        }
    }while (nAffectedRows >=25 );
    
    startTime = startTime - secnum*(i-1);

    secnum = 10*60;
    i= 0;
    do
    {
        oss.str ("");
        oss << "SELECT * "
            <<  "FROM T_sm_stat "
            <<      "WHERE ZhD_xlh='" << szEscZDId << "' and stat1!=4 "
            <<          "and stattime BETWEEN FROM_UNIXTIME("<< startTime << "- "<< (secnum*(i+1))  <<")"
            <<              "AND FROM_UNIXTIME("<< startTime << "-" << (secnum*i) <<")";
        nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
        if (nAffectedRows == RowsErr)
        {
            m_pDBIdx[nDBIndex]->ThreadUnlock();
            return DB_ERR_DBDATA_ERROR;
        }
        i++;
        if((startTime-secnum*(i-1))<startMaxTime)
        {
            break;
        }
    }while (nAffectedRows >= 7);

    SMstru.dwStTime =startTime - secnum*i;


    int j=0;
    startTime = stru.shGstime;
    secnum = 30*60;
    startMaxTime = stru.shGstime+60*60*3;
    do
    {
       
        oss.str ("");
        oss << "SELECT * "
            <<  "FROM T_sm_stat "
            <<      "WHERE ZhD_xlh='" << szEscZDId << "' and stat1!=4 "
            <<          "and stattime BETWEEN FROM_UNIXTIME("<< startTime << "+ "<< (secnum*j) <<")"
            <<              "AND FROM_UNIXTIME("<< startTime << "+" << secnum*(1+j) <<")";
        nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
        if (nAffectedRows == RowsErr)
        {
            m_pDBIdx[nDBIndex]->ThreadUnlock();
            return DB_ERR_DBDATA_ERROR;
        }
        j++;
        if((startTime + (secnum*j))>startMaxTime)
        {
            break;
        }
    }while (nAffectedRows >= 25 );
    startTime = startTime + (secnum*j);
    secnum = 10*60;
    startMaxTime = stru.shGstime - 60*60*3;
    j = 0 ;
    do
    {
       
        oss.str ("");
        oss << "SELECT * "
            <<  "FROM T_sm_stat "
            <<      "WHERE ZhD_xlh='" << szEscZDId << "' and stat1!=4 "
            <<          "and stattime BETWEEN FROM_UNIXTIME("<< startTime << "- "<< (secnum*(j+1)) <<")"
            <<              "AND FROM_UNIXTIME("<< startTime << "-" << (secnum*j) <<")";
        nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
        if (nAffectedRows == RowsErr)
        {
            m_pDBIdx[nDBIndex]->ThreadUnlock();
            return DB_ERR_DBDATA_ERROR;
        }
        j++;
        if((startTime - (secnum*(j-1)))<startMaxTime)
        {
            break;
        }
    }while (nAffectedRows >= 7 );
    
    SMstru.dwEndTime = startTime - (secnum*(j-1));
    if (SMstru.dwEndTime < SMstru.dwStTime)
    {
        m_pDBTrace->TraceError("SMstru.dwEndTime < SMstru.dwStTime\n");
        return DB_ERR_NOROW;
    }

       
    oss.str ("");
    oss << "SELECT UNIX_TIMESTAMP(min(stattime)), UNIX_TIMESTAMP(max(stattime))"
        <<  " FROM T_sm_stat"
        <<   " WHERE ZhD_xlh='" << szEscZDId << "' and stat1!=4"
        <<     " and stattime BETWEEN FROM_UNIXTIME(" << SMstru.dwStTime <<")"
        <<     " AND FROM_UNIXTIME(" << SMstru.dwEndTime << ")";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));

    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_NOROW;
    }

    if(row = m_pDBIdx[nDBIndex]->getRow());
    {
        int i = 0;
        if(row[i] == NULL)
        {
            m_pDBIdx[nDBIndex]->ThreadUnlock();
            return DB_ERR_NOROW;
        }

        SMstru.dwStTime = (DWORD)atoi(row[i]);i++;
        SMstru.dwEndTime = (DWORD)atoi (row[i]);i++;
        m_pDBTrace->TraceVarIn ("getSETime() stime=[%d], etime=[%d]\n",SMstru.dwStTime, SMstru.dwEndTime );
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//ªÒ»°ÀØ√ﬂ–≈œ¢ ˝æ›
int CEDUDBMgr::getSleepStatInfo(ShiMian &stru, SleepArray &Array)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("getSleepStatInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("getSleepStatInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscszClh[ID_MAXLEN * 2 + 1];
    memset (chEscszClh, 0, sizeof(chEscszClh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscszClh, stru.szZhD_xlh, strlen (stru.szZhD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT UNIX_TIMESTAMP(stattime), stat1,AHIS,AHIO"
        <<  " FROM T_sm_stat"
        <<   " WHERE ZhD_xlh='" << chEscszClh << "'"
        <<     " and stattime BETWEEN FROM_UNIXTIME(" << stru.dwStTime <<")"
        <<     " AND FROM_UNIXTIME(" << stru.dwEndTime << ")";

    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_NOROW;
    }

    Sleep Sstru;
    while(row = m_pDBIdx[nDBIndex]->getRow())
    {
        int i = 0;
        memset(&Sstru,0,sizeof(Sstru));
        Sstru.dwTime = atoi(row[i]);i++;
        Sstru.wSleepStat1 = atoi (row[i]);i++;
        Sstru.dwAHIS = atoi(row[i]);i++;
        Sstru.dwAHIO = atoi(row[i]);i++;
        //m_pDBTrace->TraceVarIn ("getSleepStatInfo() time=[%d], stat1=[%d]\n",Sstru.dwTime, Sstru.wSleepStat1 );
        Array.push_back(Sstru);
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//±£¥Ê»Îø‚
int CEDUDBMgr::insetSleepStat(ShiMian stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("insetSleepStat() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("insetSleepStat() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscszClh[ID_MAXLEN * 2 + 1];
    memset (chEscszClh, 0, sizeof(chEscszClh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscszClh, stru.szZhD_xlh, strlen (stru.szZhD_xlh));

    oss.str("");
    oss << "INSERT INTO V_sm_stat (ZhD_xlh, flagdate, sleepStart, sleepEnd, sleepLatency, sleepAll, sleepBed, waso, NA, ratio_of_sleep, AHIS, AHIO )"
        << " VALUES ('" << chEscszClh << "', date_sub(FROM_UNIXTIME("<< stru.dwflagdate << "),interval 1 day), FROM_UNIXTIME(" << stru.dwStTime
        << "), FROM_UNIXTIME(" << stru.dwEndTime << "), " << stru.dwDelayTime << ", " << stru.dwAllSleep
        << ", " << stru.dwInBedTime << ", " << stru.dwWASO << ", " << stru.dwNA << ", "
        << stru.dwratio_of_sleep << ", " << stru.dwAHIS << ", " << stru.dwAHIO <<") ";
    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

/*
 * ªÒ»° ˝æ›ø‚ID
 * @return  >=0	ø…”√µƒDBIndexID; EDUDB_ERR_DBCONN_LOST À˘”–¡¨Ω”“—æ≠∂œø™; EDUDB_ERR_DBCONN_BUSY À˘”–¡¨Ω”∂º√¶¬µ£®÷∏√ø∏ˆ¡¨Ω”∂º”–EDUDB_CONST_LOCK_MAXCOUNT∏ˆ√¸¡Óµ»¥˝£©
 */
int CEDUDBMgr::GetDBIndexID (short shType)
{
    if (m_nDBStatus != EDUDB_ERR_DBCONN_OK)
        return EDUDB_ERR_DBCONN_LOST;
    if ((++m_nDBIdxID) >= EDUDB_CONST_CONN_NUM)
        m_nDBIdxID = 0;
    int iCyc = 0;
    int nLockedCount = m_pDBIdx[m_nDBIdxID]->GetIsLocked ();
    int nLockedMin = m_nDBIdxID;
    while (m_pDBIdx[m_nDBIdxID]->GetIsLocked () != EDUDB_CONST_ISUNLOCKED)
    {
        if (nLockedCount > m_pDBIdx[m_nDBIdxID]->GetIsLocked ())
        {
            nLockedCount = m_pDBIdx[m_nDBIdxID]->GetIsLocked ();
            nLockedMin = m_nDBIdxID;
            if (nLockedCount == 0)
                break;
        }
        if ((++m_nDBIdxID) >= EDUDB_CONST_CONN_NUM)
            m_nDBIdxID = 0;
        if ((++iCyc) >= EDUDB_CONST_CONN_NUM)
            break;
    }
    if (nLockedCount >= EDUDB_CONST_LOCK_MAXCOUNT)
        return EDUDB_ERR_DBCONN_BUSY;
    m_nDBIdxID = nLockedMin;
    return nLockedMin;
}

//≤Èø¥open_id «∑Ò”Î÷’∂À“—∞Û∂®
int CEDUDBMgr::CheckBind(OpenidInfo stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("CheckBing() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("CheckBing() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscszClh[ID_MAXLEN * 2 + 1];
    memset (chEscszClh, 0, sizeof(chEscszClh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscszClh, stru.szServerid, strlen (stru.szServerid));

    char chEscOpenid[ID_MAXLEN * 2 + 1];
    memset (chEscOpenid, 0, sizeof(chEscOpenid));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscOpenid, stru.szOpenid, strlen (stru.szOpenid));

    int nAffectedRows = RowsInit;
    int result=DB_ERR_SUCCESS;

    oss.str ("");
    oss << "SELECT * FROM sqweixi WHERE ZD_xlh='" << chEscszClh
        << "' AND open_id='" << chEscOpenid << "' ";

    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DB_ERR_DBDATA_ERROR;
    }
    else if (nAffectedRows > RowsNot)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DBERR_BIND_EXIST;
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return result;
}

//ªÒ»°openid∞Û∂®ø®∫≈¡–±Ì
int CEDUDBMgr::GetBandZDList(OpenidInfo &stru, OpenidInfoArray &Array)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("GetBandZDList() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("GetBandZDList() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscOpenid[ID_MAXLEN * 2 + 1];
    memset (chEscOpenid, 0, sizeof(chEscOpenid));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscOpenid, stru.szOpenid, strlen (stru.szOpenid));

    oss.str("");
    if(stru.ushUserType == 1)
    {
        oss <<"select ZD_xlh from sqweixi where open_id = '" << chEscOpenid << "'";
    }
    else if(stru.ushUserType == 2)
    {
        oss <<"select DISTINCT ZD_xlh from sqweixi";
    }
    else
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DBERR_NOTBIND;
    }

    char **row = NULL;
    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("GetBandZDList() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    if( RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("GetBandZDList() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DBERR_NOTBIND;
    }

    OpenidInfo aOpinfo;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aOpinfo, 0, sizeof (aOpinfo));
        strcpy (aOpinfo.szServerid, row[i] ? row[i] : "\0");i++;
        Array.push_back(aOpinfo);
        //m_pDBTrace->TraceVarIn (" = [%s],  = [%s], \n",, );
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//Ω‚≥˝open_id∞Û∂®µƒ…Ë±∏
int CEDUDBMgr::DelBind(OpenidInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("DelBind() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("DelBind() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscOpenid[ID_MAXLEN * 2 + 1];
    memset (chEscOpenid, 0, sizeof(chEscOpenid));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscOpenid, stru.szOpenid, strlen (stru.szOpenid));

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szServerid, strlen (stru.szServerid));

    //m_pDBTrace->TraceVarOut("ifexit\n");
    nRet = CheckZhongDuan(stru.szServerid);\
    m_pDBTrace->TraceVarOut("ret=[%d]\n", nRet);
    if (nRet < 0)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return  nRet;
    }
    //m_pDBTrace->TraceVarOut("delopenid stat\n");
    oss.str("");
    oss <<"delete from sqweixi where open_id='" << chEscOpenid << "' and ZD_xlh='" << chEscServerId <<"'";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("DelBind() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}
//∞Û∂®…Ë±∏£®Œ¢–≈π´÷⁄∫≈”√£©
int CEDUDBMgr::BindBreath(OpenidInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("BindBreath() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("BindBreath() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscOpenid[ID_MAXLEN * 2 + 1];
    memset (chEscOpenid, 0, sizeof(chEscOpenid));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscOpenid, stru.szOpenid, strlen (stru.szOpenid));

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szServerid, strlen (stru.szServerid));

    char chEscPassword[ID_MAXLEN * 2 + 1];
    memset (chEscPassword, 0, sizeof(chEscOpenid));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscPassword, stru.szPassword, strlen (stru.szPassword));

    m_pDBTrace->TraceVarOut("CheckZhongDuan() - start\n");
    nRet = CheckZhongDuan(stru.szServerid);
    if (nRet != 0)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return  nRet;
    }
    m_pDBTrace->TraceVarOut("CheckBind() - start\n");
    nRet = CheckBind(stru);
    if (nRet != 0)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return nRet;
    }
    oss.str("");
    oss <<"insert into sqweixi(open_id, ZD_xlh, zo_pwd) values ('" << chEscOpenid << "', '" << chEscServerId <<"', '" << chEscPassword << "')";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("BindBreath() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;

}

//ºÏ≤‚÷’∂À…Ë±∏ «∑Ò¥Ê‘⁄
int CEDUDBMgr::CheckZhongDuan(char *ZhDXlh)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("CheckZhD_IfExist() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("CheckZhD_IfExist() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscszZhD_xlh[ID_MAXLEN * 2 + 1];
    memset (chEscszZhD_xlh, 0, sizeof(chEscszZhD_xlh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscszZhD_xlh, ZhDXlh, strlen (ZhDXlh));

    int nAffectedRows = RowsInit;
    int result=DB_ERR_SUCCESS;

    oss.str ("");
    oss << "select * from T_TerminalInfo where zhd_xlh='" << chEscszZhD_xlh << "'";

    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));

    if (nAffectedRows == RowsErr)
    {
        m_pDBTrace->TraceError("CheckZhongDuan() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("CheckZhongDuan() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DBERR_ZDID_NOTEXIST;
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return result;
}

//÷’∂À…Ë±∏»Îø‚
int CEDUDBMgr::AddTerminalInfo(ZhDInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("AddTerminalInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("AddTerminalInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscXlh[ID_MAXLEN * 2 + 1];
    memset (chEscXlh, 0, sizeof(chEscXlh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscXlh, stru.szXlh, strlen (stru.szXlh));

    int nAffectedRows = RowsInit;
    int result=DB_ERR_SUCCESS;

    oss.str ("");
    oss << "INSERT into T_TerminalInfo(zhd_xlh,zhd_type) "
        << "SELECT '" << chEscXlh << "', " << stru.dwType << " "
        <<  "FROM dual where not exists ("
        <<      "select zhd_xlh "
                    "from T_TerminalInfo "
                        "where zhd_xlh ='" << chEscXlh << "')";

    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (nAffectedRows == RowsErr)
    {
        m_pDBTrace->TraceError("AddTerminalInfo() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("AddTerminalInfo() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        result=DBERR_ZHDID_EXIST;
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}
//œ˙ªŸ÷’∂À…Ë±∏
int CEDUDBMgr::DelTerminalInfo(ZhDInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("DelTerminalInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("DelTerminalInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscXlh[ID_MAXLEN * 2 + 1];
    memset (chEscXlh, 0, sizeof(chEscXlh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscXlh, stru.szXlh, strlen (stru.szXlh));

    oss.str("");
    oss <<"delete from T_TerminalInfo where zhd_xlh='" << chEscXlh << "'";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("DelBind() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::GetTerminalList(PageList &List,FileDirArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetTerminalList() - GetDBIndexID=[%lu]\n",	nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetTerminalList() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //Õ≥º∆µ±«∞Ãıº˛œ¬±®æØ–≈œ¢ ˝¡ø
    oss.str ("");
    oss << "select count(*) from T_TerminalInfo";
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetTerminalList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }
    if(0 == nAffectedRows)
    {
        m_pDBTrace->TraceVarOut("GetTerminalList() - nAffectedRows=[0]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_SUCCESS;
    }

    //Õ≥º∆Ω·π˚
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wCount = (WORD) atoi(row[0]);

    WORD wListNum = List.wListNum;
    //º∆À„“≥ ˝
    WORD tmpCount = wCount % wListNum;
    List.wAllPageNum = wCount / wListNum;
    if(tmpCount > 0)
    {
        ++List.wAllPageNum;
    }

    //≤È’“±®æØ–≈œ¢
    oss.str ("");
    oss << "SELECT zhd_xlh "
        <<  "FROM T_TerminalInfo "
        <<      "ORDER BY ti_index DESC limit " << (List.wPageNum - 1) * List.wListNum << ", " << List.wListNum;

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetTerminalList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }
    if(0 == nAffectedRows)
    {
        m_pDBTrace->TraceVarOut("GetTerminalList() - nAffectedRows=[0]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_SUCCESS;
    }

    XLH aXlh;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aXlh, 0, sizeof (aXlh));
        strcpy (aXlh.szZD_xlh, row[i] ? row[i] : "\0");i++;
        Array.push_back(aXlh);
        //m_pDBTrace->TraceVarIn (" type=[%s],  time=[%s], \n",aAlarmInfo.wAlarmType,aAlarmInfo.wAlarmType );
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

//ªÒ»°±®æØ–≈œ¢
int CEDUDBMgr::GetBreathAlarmInfo(PageList &List, BreathAlarmInfo &stru, BreathAlarmInfoArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo() - GetDBIndexID=[%lu]\n",	nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscServerId[ID_MAXLEN * 2 + 1];
    memset (szEscServerId, 0, sizeof(stru.szServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (szEscServerId, stru.szServerId, strlen (stru.szServerId));
    //printf("GetGrantList() szEscUserID=[%s]\n",szEscUserID);

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //Õ≥º∆µ±«∞Ãıº˛œ¬±®æØ–≈œ¢ ˝¡ø
    oss.str ("");
    oss << "select count(*) from alarm_breath where type&" << stru.wAlarmType <<
           " and ZD_xlh='"<< szEscServerId << "' and alarm_time between FROM_UNIXTIME(" << stru.dwStartTime
        << ")and FROM_UNIXTIME(" << stru.dwLimitTime << ")";

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    //Õ≥º∆Ω·π˚
    //cout << "GetGrantList() before" << endl;
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wCount = (WORD) atoi(row[0]);
    //cout << "GetGrantList() after  wCount=" << wCount << endl;
    WORD wListNum = List.wListNum;
    //º∆À„“≥ ˝
    if ((wCount == 0)||(0 >= wListNum))
    {
        List.wAllPageNum = 0;
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_SUCCESS;
    }
    else
    {
        WORD tmpCount = wCount % wListNum;
        List.wAllPageNum = wCount / wListNum;
        if(tmpCount > 0)
        {
            ++List.wAllPageNum;
        }
    }

    //≤È’“±®æØ–≈œ¢
    oss.str ("");
    oss << "SELECT type, UNIX_TIMESTAMP(alarm_time), alarm_duration";
    oss << " FROM alarm_breath"
        << " WHERE type&" << stru.wAlarmType
        << " AND ZD_xlh='" << szEscServerId << "' AND alarm_time between FROM_UNIXTIME(" << stru.dwStartTime
        << ") AND FROM_UNIXTIME(" << stru.dwLimitTime << ")";
    oss << " ORDER BY alarm_time DESC limit " << (List.wPageNum - 1) * List.wListNum << ", " << List.wListNum;

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("getUserList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }


    BreathAlarmInfo aAlarmInfo;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aAlarmInfo, 0, sizeof (aAlarmInfo));
        aAlarmInfo.wAlarmType = atoi (row[i]);i++;
        aAlarmInfo.dwAlarmTime = atoi (row[i]);i++;
        aAlarmInfo.shAlarmdur = atoi (row[i]);i++;
        Array.push_back(aAlarmInfo);
        //m_pDBTrace->TraceVarIn (" type=[%s],  time=[%s], \n",aAlarmInfo.wAlarmType,aAlarmInfo.wAlarmType );
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::GetAlarmInfo2(PageList &List,BreathAlarmInfo2 &stru, BreathAlarmInfoArray2 &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo2() - GetDBIndexID=[%lu]\n",	nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo2() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscServerId[ID_MAXLEN * 2 + 1];
    memset (szEscServerId, 0, sizeof(stru.szServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (szEscServerId, stru.szServerId, strlen (stru.szServerId));
    //printf("GetBreathAlarmInfo2() szEscUserID=[%s]\n",szEscUserID);

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //Õ≥º∆µ±«∞Ãıº˛œ¬±®æØ–≈œ¢ ˝¡ø
    oss.str ("");
    oss << "select count(*) from alarm_info where type&" << stru.wAlarmType <<
           " and ZD_xlh='"<< szEscServerId << "' and alarm_time between FROM_UNIXTIME(" << stru.dwStartTime
        << ")and FROM_UNIXTIME(" << stru.dwLimitTime << ")";

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("GetBreathAlarmInfo2() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo2() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    //Õ≥º∆Ω·π˚
    //cout << "GetBreathAlarmInfo2() before" << endl;
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wCount = (WORD) atoi(row[0]);
    //cout << "GetBreathAlarmInfo2() after  wCount=" << wCount << endl;
    WORD wListNum = List.wListNum;
    //º∆À„“≥ ˝
    if ((wCount == 0)||(0 >= wListNum))
    {
        List.wAllPageNum = 0;
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_SUCCESS;
    }
    else
    {
        WORD tmpCount = wCount % wListNum;
        List.wAllPageNum = wCount / wListNum;
        if(tmpCount > 0)
        {
            ++List.wAllPageNum;
        }
    }

    //≤È’“±®æØ–≈œ¢
    oss.str ("");
    oss << "SELECT type, UNIX_TIMESTAMP(alarm_time), param";
    oss << " FROM alarm_info"
        << " WHERE type&" << stru.wAlarmType
        << " AND ZD_xlh='" << szEscServerId << "' AND alarm_time between FROM_UNIXTIME(" << stru.dwStartTime
        << ") AND FROM_UNIXTIME(" << stru.dwLimitTime << ")";
    oss << " ORDER BY alarm_time DESC limit " << (List.wPageNum - 1) * List.wListNum << ", " << List.wListNum;

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("GetBreathAlarmInfo2() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo2() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }


    BreathAlarmInfo2 aAlarmInfo;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aAlarmInfo, 0, sizeof (aAlarmInfo));
        aAlarmInfo.wAlarmType = atoi (row[i]);i++;
        aAlarmInfo.dwAlarmTime = atoi (row[i]);i++;
        aAlarmInfo.wAlarmParam = atoi (row[i]);i++;
        Array.push_back(aAlarmInfo);
        //m_pDBTrace->TraceVarIn (" type=[%s],  time=[%s], \n",aAlarmInfo.wAlarmType,aAlarmInfo.wAlarmType );
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}
/*
//ªÒ»°ºÏ≤‚ ˝æ›Œƒº˛
int CEDUDBMgr::GetDataInfo (PageList &List, DataInfo &stru, DataInfoArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetDataInfo() - GetDBIndexID=[%lu]\n",	nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetDataInfoo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscServerId[ID_MAXLEN * 2 + 1];
    memset (szEscServerId, 0, sizeof(stru.szNumber));
    m_pDBIdx[nDBIndex]->realEscapeString (szEscServerId, stru.szNumber, strlen (stru.szNumber));
    //printf("GetGrantList() szEscUserID=[%s]\n",szEscUserID);

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //Õ≥º∆µ±«∞Ãıº˛œ¬–≈œ¢ ˝¡ø
    oss.str ("");
    oss << "select count(*) from monitor_info where type&" << stru.wType <<
           " and BT_number='"<< szEscServerId << "' and starttime < FROM_UNIXTIME(" << stru.dwEndtime
        << ") and endtime > FROM_UNIXTIME(" << stru.dwStarttime << ")";

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetBreathAlarmInfo() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    //Õ≥º∆Ω·π˚
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wCount = (WORD) atoi(row[0]);
    WORD wListNum = List.wListNum;
    //º∆À„“≥ ˝
    if ((wCount == 0)||(0 >= wListNum))
    {
        List.wAllPageNum = 0;
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_SUCCESS;
    }
    else
    {
        WORD tmpCount = wCount % wListNum;
        List.wAllPageNum = wCount / wListNum;
        if(tmpCount > 0)
        {
            ++List.wAllPageNum;
        }
    }

    //≤È’“ºÏ≤‚–≈œ¢
    oss.str ("");
    oss << "select UNIX_TIMESTAMP(starttime), UNIX_TIMESTAMP(endtime), filename";
    oss << " from monitor_info WHERE type&" << stru.wType <<
           " and BT_number='"<< szEscServerId << "' and starttime < FROM_UNIXTIME(" << stru.dwEndtime
        << ") and endtime > FROM_UNIXTIME(" << stru.dwStarttime << ")";
    oss << " limit " << (List.wPageNum - 1) * List.wListNum << ", " << List.wListNum;

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if (-1 == nAffectedRows)
    {
        m_pDBTrace->TraceError ("getUserList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    DataInfo aAlarmInfo;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aAlarmInfo, 0, sizeof (aAlarmInfo));
        aAlarmInfo.dwStarttime = atoi (row[i]);i++;
        aAlarmInfo.dwEndtime = atoi (row[i]);i++;
        strcpy (aAlarmInfo.szFileName, row[i] ? row[i] : "\0");i++;
        Array.push_back(aAlarmInfo);
        //m_pDBTrace->TraceVarIn (" = [%s],  = [%s], \n",, );
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}
*/

//ÃÌº”ºÏ≤‚ ˝æ›À˜“˝
int CEDUDBMgr::AddDataInfo(DataInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("AddDataInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("AddDataInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDID[ID_MAXLEN * 2 + 1];
    memset ( szEscZDID, 0, sizeof(stru.szZhdXlh));
    m_pDBIdx[nDBIndex]->realEscapeString ( szEscZDID, stru.szZhdXlh, strlen (stru.szZhdXlh));

    char szEscFileName[ID_MAXLEN * 2 + 1];
    memset ( szEscFileName, 0, sizeof(stru.szFileName));
    m_pDBIdx[nDBIndex]->realEscapeString ( szEscFileName, stru.szFileName, strlen (stru.szFileName));

    oss.str("");
    oss << "INSERT INTO monitor_info "
        << "(ZD_xlh, type, UploadTime, filename) "
        << "VALUES "
        << "('" << szEscZDID << "', " << stru.wType
        << ", FROM_UNIXTIME(" << stru.dwUploadtime << "), '"
        << szEscFileName <<"') ";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("AddDataInfo() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//ªÒ»°∏…‘§ ±º‰≤Œ ˝
int CEDUDBMgr::GetIpeTime(GsTime &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("OnGetAbnormal() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("OnGetAbnormal() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szZD_xlh, strlen (stru.szZD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str("");
    oss << "SELECT IFNULL(g_time,0),recomtime "
        << "FROM gs_shij "
        << "WHERE ZD_xlh='"
        << chEscServerId << "'";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    if( RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        DB_ERR_TERMINAL_NOTEXIST;
    }
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        stru.shOwnTime = (WORD)atoi(row[0]);i++;
        stru.shRecomTime = (WORD)atoi(row[i]);i++;
        //m_pDBTrace->TraceVarIn (" type=[%s],  time=[%s], \n",aAlarmInfo.wAlarmType,aAlarmInfo.wAlarmType );
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//…Ë÷√∏……Ê ±º‰≤Œ ˝
int CEDUDBMgr::SetIpeTime(GsTime &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("SetIpeTime() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("SetIpeTime() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szZD_xlh, strlen (stru.szZD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str("");
    oss << "UPDATE gs_shij "
        << "SET g_time = " << stru.shOwnTime
        << " WHERE ZD_xlh = '"<< chEscServerId << "'";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::IfExistAbnormal(AbnormalStat &stru)
{
    m_pDBTrace->TraceVar("IfExistAbnormal() \n");
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("IfExistAbnormal() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("IfExistAbnormal() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szServerid, strlen (stru.szServerid));

    char **row = NULL;
    int nAffectedRows = RowsInit;
    oss.str("");
    oss << "SELECT UNIX_TIMESTAMP(sleepStart), UNIX_TIMESTAMP(sleepEnd) "
        <<    "FROM V_sm_stat "
        <<        "WHERE ZhD_xlh='" << chEscServerId << "' AND flagdate=DATE(FROM_UNIXTIME("<< stru.dwtime<< "))";
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError ("IfExistAbnormal() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }
    if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError ("IfExistAbnormal() - nAffectedRows=[0]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_NOROW;
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::UpdateAbnormal(AbnormalStat &Astru)
{
    ShiMian SMstru;
    memset(&SMstru,0,sizeof(ShiMian));
    TimeXlh stru;
    memset(&stru,0,sizeof(TimeXlh));
    strcpy(stru.szZD_xlh,Astru.szServerid);
    stru.shGstime = Astru.dwtime;
    int ret = RowsNot;
    if(ret != getSETime(stru, SMstru))
    {
        return 0;
    }
    //m_pDBTrace->TraceVarIn ("DBTimerTasker() stime=[%d], etime=[%d]\n",SMstru.dwStTime, SMstru.dwEndTime );

    strcpy(SMstru.szZhD_xlh,Astru.szServerid);
    SMstru.dwflagdate = stru.shGstime + 86400;
    SleepArray slArray;
    getSleepStatInfo(SMstru, slArray);
    int sAsize = slArray.size();

    int flagtime = 0;   // ±º‰±Íº«
    int befortime = 0;  //«∞“ª∏ˆ ±º‰
    int allSleepCount= 0;   //◊‹ÀØ√ﬂº∆ ˝
    int deepSleepCount = 0;
    int bedCount= 0;        //‘⁄¥≤º∆ ˝
    int awakCount= 0;       //æı–—º∆ ˝
    int leaveCount = 0;     //Œﬁ»Àº∆ ˝
    int NACount=0;         //¡¨–¯æı–—º∆ ˝
    int ahis = 0;
    int ahio = 0;
    WORD statflag = 0;   //◊¥Ã¨±Í ∂
    for(int j=0; j<sAsize; j++)
    {
        flagtime = slArray[j].dwTime;
        statflag = slArray[j].wSleepStat1;
        ahis +=  slArray[j].dwAHIS;
        ahio +=  slArray[j].dwAHIO;
        m_pDBTrace->TraceVarOut("UpdateAbnormal() AHIS=%d,AHIO=%d\n", slArray[j].dwAHIS, slArray[j].dwAHIO);
        if(SMstru.dwDelayTime==0 && statflag==4)
        {
            leaveCount++;
        }
        else
        {
            bedCount++;
        }

        if(statflag == 3)
        {
            awakCount++;
            //≥÷–¯≥¨π˝5∑÷÷”£®NA> 5£©µƒæı–—¥Œ ˝
            if(flagtime-befortime < 90)
            {
                NACount++;
            }
            else
            {
                if(NACount > 5)
                {
                    SMstru.dwNA++;
                }
                NACount = 1;
            }
            befortime = flagtime;
        }
        if(statflag < 3 )
        {
            allSleepCount++;
            if(SMstru.dwDelayTime==0)
            {
                SMstru.dwDelayTime = flagtime - SMstru.dwStTime - leaveCount*60;
            }
            if(statflag == 0)
            {
                deepSleepCount++;
            }
        }
    }
    if(SMstru.dwDelayTime==0)
    {
        SMstru.dwDelayTime = flagtime - SMstru.dwStTime - leaveCount*60;
    }
    SMstru.dwInBedTime = bedCount * 60;
    SMstru.dwAllSleep = allSleepCount * 60;
    SMstru.dwWASO   = awakCount * 60 - SMstru.dwDelayTime;
    SMstru.dwratio_of_sleep = deepSleepCount*100/allSleepCount;
    SMstru.dwAHIS =ahis;
    SMstru.dwAHIO =ahio;

//        m_pDBTrace->TraceVarIn ("DB ShiMian() stime=[%d], etime=[%d],\tDelayTime=[%d]\n"
//                                "AllSleep=[%d], InBedTime=[%d]\n"
//                                "WASO=[%d], NA=[%d]\n",
//                                SMstru.dwStTime, SMstru.dwEndTime, SMstru.dwDelayTime,
//                                SMstru.dwAllSleep, SMstru.dwInBedTime,
//                                SMstru.dwWASO, SMstru.dwNA);

    insetSleepStat(SMstru);
    return 0;
}

int CEDUDBMgr::GetAbnormal(AbnormalStat &stru,AbnormalStatArray &Array)
{
    if(stru.shtype == 0)
    {
        m_pDBTrace->TraceVarOut("IfExistAbnormal start \n");
        if (IfExistAbnormal(stru) == DB_ERR_NOROW)
        {   m_pDBTrace->TraceVarOut("UpdateAbnormal start \n");
            UpdateAbnormal(stru);
        }
    }
    m_pDBTrace->TraceVar("OnGetAbnormal() \n");
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("GetAbnormal() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("GetAbnormal() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscServerId[ID_MAXLEN * 2 + 1];
    memset (chEscServerId, 0, sizeof(chEscServerId));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscServerId, stru.szServerid, strlen (stru.szServerid));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    //
    switch(stru.shtype)
    {
        case 0:
            {   //≤È’“stru.dwtimeÀ˘‘⁄ÃÏø™ º∫ÕΩ· ¯ ±º‰
                oss.str("");
                oss << "SELECT UNIX_TIMESTAMP(sleepStart), UNIX_TIMESTAMP(sleepEnd) "
                    <<    "FROM V_sm_stat "
                    <<        "WHERE ZhD_xlh='" << chEscServerId << "' AND flagdate=DATE(FROM_UNIXTIME("<< stru.dwtime<< "))";
                nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
                if (RowsErr == nAffectedRows)
                {
                    m_pDBTrace->TraceError ("GetAbnormal() - nAffectedRows=[-1]\n");
                    m_pDBIdx[nDBIndex]->ThreadUnlock ();
                    return DB_ERR_DBDATA_ERROR;
                }
                if (RowsNot == nAffectedRows)
                {
                    m_pDBTrace->TraceError ("GetAbnormal() - nAffectedRows=[0]\n");
                    m_pDBIdx[nDBIndex]->ThreadUnlock ();
                    return DB_ERR_NOROW;
                }
                row = m_pDBIdx[nDBIndex]->getRow ();
                stru.dwSleepBTime = atoi(row[0]);
                stru.dwSleepETime = atoi(row[1]);

                //ªÒ»°stru.dwtimeÀ˘‘⁄ÃÏµƒ ˝æ›
                oss.str ("");
                oss << "SELECT HOUR(stattime),sum(AHIS),sum(AHIO),SUM(stat1=0)*100/count(stat1) "
                    <<  "FROM T_sm_stat "
                    <<      "WHERE stattime BETWEEN FROM_UNIXTIME(" << stru.dwSleepBTime << ") AND FROM_UNIXTIME(" << stru.dwSleepETime
                    <<              ") AND ZhD_xlh='" << chEscServerId << "' "
                    <<          "GROUP BY HOUR(stattime) ASC";
                nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
            }
            break;
        case 1:
            {
                oss.str("");
                oss << "SELECT DAYOFWEEK(flagdate),AHIS,AHIO,ratio_of_sleep "
                    << "FROM V_sm_stat "
                    << "WHERE DATE_FORMAT(flagdate,'%Y-%U')=FROM_UNIXTIME("
                    << stru.dwtime << ",'%Y-%U') AND ZhD_xlh='"
                    << chEscServerId << "'";
                nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
            }
            break;
        case 2:
        {
            oss.str("");
            oss << "SELECT DAYOFMONTH(flagdate),AHIS,AHIO,ratio_of_sleep "
                << "FROM V_sm_stat "
                << "WHERE DATE_FORMAT(flagdate,'%Y-%m')=FROM_UNIXTIME("
                << stru.dwtime << ",'%Y-%m') AND ZhD_xlh='"
                << chEscServerId << "'";
            nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
        }
            break;
        default:
            {
                m_pDBIdx[nDBIndex]->ThreadUnlock ();
                return DB_ERR_DATETYPE_WRONG;
            }
            break;
    }
    if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetAbnormal() - nAffectedRows=[0]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_NOROW;
    }
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError ("GetAbnormal() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    AbnormalStat aPush;
    row = RowsNot;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aPush, 0, sizeof (aPush));
        aPush.shNum = atoi (row[i]);i++;
        aPush.dwDi = atoi (row[i]);i++;
        aPush.dwZan = atoi (row[i]);i++;
        aPush.dwBi = atoi (row[i]);i++;
        Array.push_back(aPush);
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

//≤‚ ‘∫ÙŒ¸ ¬º˛±®∏Ê
int CEDUDBMgr::tryBreathEntRpt(BreathEntRptInfo &stru, BreathEntRptInfoArray &Array)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("tryBreathEntRpt() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("tryBreathEntRpt() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char chEscszClh[ID_MAXLEN * 2 + 1];
    memset (chEscszClh, 0, sizeof(chEscszClh));
    m_pDBIdx[nDBIndex]->realEscapeString (chEscszClh, stru.szZDxlh, strlen (stru.szZDxlh));

    char **row = NULL;
    int nAffectedRows=RowsInit;

    //Õ≥º∆◊‹∏ˆ ˝
    oss.str("");
    oss << "SELECT count(*) "
        <<      "FROM alarm_breath "
        <<          "WHERE ZD_xlh = '" << chEscszClh << "' AND alarm_duration >= 50 "
        <<              "AND alarm_time BETWEEN FROM_UNIXTIME(" << stru.dwSleepBTime
        <<              ") AND FROM_UNIXTIME(" << stru.dwSleepETime << ")";

    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wCount = (WORD) atoi(row[0]);
    m_pDBTrace->TraceTimer ("tryBreathEntRpt() - wCount=[%d]\n", wCount);
    oss.str ("");
    oss << "SELECT a.type, b.ct, b.Bi, b.su, b.ma, a.Ut "
        <<     "FROM (SELECT type,count(*) ct, round(count(*)*1000/" << wCount << ") Bi, "
        <<            "sum(alarm_duration) su, max(alarm_duration) ma "
        <<              "FROM alarm_breath "
        <<                  "WHERE ZD_xlh = '" << chEscszClh << "' AND alarm_duration >= 50 "
        <<                   "AND alarm_time BETWEEN FROM_UNIXTIME(" << stru.dwSleepBTime
        <<                   ") AND FROM_UNIXTIME(" << stru.dwSleepETime << ") "
        <<                "GROUP BY type) b "
        <<    "JOIN (SELECT type, alarm_duration, UNIX_TIMESTAMP(alarm_time) Ut "
        <<              "FROM alarm_breath "
        <<                  "WHERE ZD_xlh = '" << chEscszClh << "' AND alarm_duration >= 50 "
        <<                   "AND alarm_time BETWEEN FROM_UNIXTIME(" << stru.dwSleepBTime
        <<                   ") AND FROM_UNIXTIME(" << stru.dwSleepETime << ")) a "
        <<      "ON a.type= b.type AND a.alarm_duration=b.ma "
        <<          "ORDER BY a.type";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    BreathEntRptInfo aOpinfo;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&aOpinfo, 0, sizeof (aOpinfo));
        aOpinfo.wType = atoi(row[i]);i++;
        aOpinfo.wNum = atoi(row[i]);i++;
        aOpinfo.shBaifenbi = atoi(row[i]);i++;
        aOpinfo.shLiejishijian = atoi(row[i]);i++;
        aOpinfo.shZuichang = atoi(row[i]);i++;
        aOpinfo.Fashengshijian = atoi(row[i]);i++;
        Array.push_back(aOpinfo);
        //m_pDBTrace->TraceVarIn (" = [%s],  = [%s], \n",, );
    }

//    oss.str("");
//    oss << "SELECT AHIS+AHIO "
//        << "FROM T_sm_stat "
//        << "WHERE ZhD_xlh='"<< chEscszClh << "' "
//        << " AND stattime BETWEEN FROM_UNIXTIME(" << stru.dwSleepBTime
//        << ") AND FROM_UNIXTIME(" <<stru.dwSleepETime << ")";
//    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
//    if( RowsErr == nAffectedRows || RowsNot == nAffectedRows)
//    {
//        m_pDBTrace->TraceError("tryBreathEntRptt() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
//        m_pDBIdx[nDBIndex]->ThreadUnlock();
//        return DB_ERR_DBDATA_ERROR;
//    }
    row = m_pDBIdx[nDBIndex]->getRow ();
    WORD wAHI = 0;
    //wAHI = (WORD) atoi(row[0]);
    if(wAHI < 5)
    {
        stru.level=0x0;
        stru.SymptomType = 1;
    }
    else if(wAHI <= 15)
    {
        stru.level=0x1;
        stru.SymptomType = 2;
    }
    else if(wAHI <= 30)
    {
        stru.level=0x2;
        stru.SymptomType = 3;
    }
    else
    {
        stru.level=0x4;
        stru.SymptomType = 4;
    }
    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//ªÒ»°Œ¥¥¶¿ÌŒƒº˛ƒø¬º
int CEDUDBMgr::getUnprocDir(FileDirArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("getUnprocDir() - GetDBIndexID=[%lu]\n",
                                nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("getUnprocDir() - ThreadLock[%lu]=[%lu]\n",
                                nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT distinct ZD_xlh "
        << "FROM monitor_info "
        << "WHERE type=2 and flag=0 "
        << "Order by ZD_xlh";

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("getUnprocDir() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("getUnprocDir - nAffectedRows=[%d]\n", RowsNot);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_UNPROC_NOTEXIST;
    }

    XLH Dir;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        strcpy (Dir.szZD_xlh, row[i] ? row[i] : "\0");i++;
        Array.push_back(Dir);
        m_pDBTrace->TraceVarOut("Dir=[%s]\n", Dir.szZD_xlh);
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

//ªÒ»°◊¥Ã¨Œƒº˛¡–±Ì
int CEDUDBMgr::getStatFilelist(SFpara &stru, FileDirArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("getStatFilelist() - GetDBIndexID=[%lu]\n",
                                nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("getStatFilelist() - ThreadLock[%lu]=[%lu]\n",
                                nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char chXlh[ID_MAXLEN * 2 + 1];
    memset (chXlh, 0, sizeof(chXlh));
    m_pDBIdx[nDBIndex]->realEscapeString (chXlh, stru.ZhDXlh, strlen (stru.ZhDXlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT filename"
        << " FROM monitor_info"
        << " WHERE type=2 AND flag=0 AND ZD_xlh='" << chXlh << "'";
    if(stru.dwTime > 0)
    {
        oss << " AND UploadTime BETWEEN date_add(FROM_UNIXTIME(" << stru.dwTime << "), interval 4 hour)"
            << " AND date_add(FROM_UNIXTIME(" << stru.dwTime << "), interval 28 hour)";
    }
    if(stru.wNum > 0)
    {
        oss << " limit " << stru.wNum;
    }

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("getStatFilelist() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    if (RowsNot == nAffectedRows)
    {
        m_pDBTrace->TraceError("getStatFilelist - nAffectedRows=[%d]\n", RowsNot);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_UNPROC_NOTEXIST;
    }

    XLH Filename;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        strcpy (Filename.szZD_xlh, row[i] ? row[i] : "\0");i++;
        Array.push_back(Filename);
        m_pDBTrace->TraceVarOut("filename=[%s]\n", Filename.szZD_xlh);
    }

    WORD wCount = Array.size(); // µº ∏ˆ ˝
    oss.str("");
    oss << "UPDATE monitor_info SET flag = 1"
        << " WHERE ZD_xlh = '" << chXlh << "' and filename in (";

    for(int i = 0; i < wCount; i++)
    {
        if(i !=0)
        {
            oss << ", ";
        }
        oss << "'" <<  Array[i].szZD_xlh << "'";
    }
    oss << ")";
    nAffectedRows = RowsNot;
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("getStatFilelist() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::inStat_BaseSmoothPara(const char *xlh, DWORD num, SleepArray &Array, BreathAlarmInfo2 &SETime)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("inStat_BaseSmoothPara() - GetDBIndexID=[%lu]\n",
                                nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("inStat_BaseSmoothPara() - ThreadLock[%lu]=[%lu]\n",
                                nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char ZDId[ID_MAXLEN * 2 + 1];
    memset (ZDId, 0, sizeof(ZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(ZDId, xlh, strlen(xlh));

    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "INSERT INTO T_sm_stat(ZhD_xlh,stattime,BreathRate,AHIS,AHIO,stat1,stat2,HRate,RespRate) "
        << "VALUES ";
            for(DWORD i=0; i<num; i++)
            {
                if(i !=0)
                {
                    oss << ", ";
                }
             oss<< "('" << ZDId
                << "', FROM_UNIXTIME(" << Array[i].dwTime
                << "), " << Array[i].dwBreathRate
                << ", " << Array[i].dwAHIS << ", " << Array[i].dwAHIO
                << ", " << Array[i].wSleepStat1 << ", " << Array[i].wSleepStat2
                << ", " << Array[i].dwHeartRate << ", " << Array[i].dwRespRate<< ")";
            }
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("inStat_BaseSmoothPara - nAffectedRows=[%d]\n", RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    oss.str ("");
    oss << "update "
        << "T_sm_stat st JOIN("
        << "select ZD_xlh,alarm_time,alarm_duration "
        << "from alarm_breath "
        << "where ZD_xlh='" << ZDId <<"' "
        << "and alarm_time BETWEEN FROM_UNIXTIME(" << SETime.dwStartTime << ") AND FROM_UNIXTIME(" << SETime.dwLimitTime << ")) s "
        << "on st.ZhD_xlh=s.ZD_xlh and st.stattime=s.alarm_time and s.alarm_duration=1 "
        << "SET st.stat1=4,st.stat2=4";
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("inStat_BaseSmoothPara - nAffectedRows=[%d]\n", RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::GetSleepInfo(TimeXlh &stru, SleepArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetFileName() - GetDBIndexID=[%lu]\n",
                                nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetFileName() - ThreadLock[%lu]=[%lu]\n",
                                nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDId[ID_MAXLEN * 2 + 1];
    memset (szEscZDId, 0, sizeof(szEscZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(szEscZDId, stru.szZD_xlh, strlen(stru.szZD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str("");
    oss << "SELECT UNIX_TIMESTAMP(sleepStart), UNIX_TIMESTAMP(sleepEnd) "
        << "FROM V_sm_stat "
        << "WHERE ZhD_xlh = '" << szEscZDId
        << "' AND stattime = DATE_FORMAT("<< stru.shGstime << ", '%Y-%m-%d' )";
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("TimerTasker2 - nAffectedRows=[%d]\n", RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }
    int starttime =0;
    int endtime =0;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        starttime = atoi (row[i]);i++;
        endtime = atoi (row[i]);i++;
    }

    oss.str ("");
    oss << "SELECT UNIX_TIMESTAMP(stattime),stat1, stat2 "
        << "FROM T_sm_stat "
        << "WHERE ZhD_xlh = '" << szEscZDId
        << "' AND stattime BETWEEN FROM_UNIXTIME(" << starttime << ")"
        <<                   " AND FROM_UNIXTIME(" << endtime << ")";
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("TimerTasker2 - nAffectedRows=[%d]\n", RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    Sleep aPath;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        aPath.dwTime = atoi (row[i]);i++;
        aPath.wSleepStat1 = atoi (row[i]);i++;
        aPath.wSleepStat2 = atoi (row[i]);i++;
        Array.push_back(aPath);
        //m_pDBTrace->TraceVarOut("path=[%s]\n", aPath.szPath);
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::GetBRInfo(TimeXlh &stru, SleepArray &Array)
{
    int nDBIndex = GetDBIndexID ();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError ("GetBRInfo() - GetDBIndexID=[%lu]\n",
                                nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock ();
    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError ("GetBRInfo() - ThreadLock[%lu]=[%lu]\n",
                                nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDId[ID_MAXLEN * 2 + 1];
    memset (szEscZDId, 0, sizeof(szEscZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(szEscZDId, stru.szZD_xlh, strlen(stru.szZD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT UNIX_TIMESTAMP(stattime),HRate, RespRate "
        << "FROM T_sm_stat "
        << "WHERE ZhD_xlh = '" << szEscZDId
        << "' AND stattime BETWEEN date_sub(FROM_UNIXTIME(" << stru.shGstime << "), interval 4 hour)"
        <<                   " AND date_add(FROM_UNIXTIME(" << stru.shGstime << "), interval 28 hour)";

    m_pDBTrace->TraceVarOut("time is %u --------------\n",stru.shGstime);
    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("GetBRInfo - nAffectedRows=[%d]\n", RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    Sleep aPath;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        aPath.dwTime = atoi (row[i]);i++;
        aPath.dwHeartRate = atoi (row[i]);i++;
        aPath.dwRespRate = atoi (row[i]);i++;
        Array.push_back(aPath);
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}

//ªÒ»°À˘”–÷’∂À…Ë±∏–Ú¡–∫≈
int CEDUDBMgr::GetAllZhDxlh(FileDirArray &Array)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("GetAllZhDxlh() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("GetAllZhDxlh() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "select zhd_xlh from T_TerminalInfo;";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_NOROW;
    }

    XLH Xlh;
    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&Xlh, 0, sizeof (Xlh));
        strcpy (Xlh.szZD_xlh, row[i] ? row[i] : "\0");i++;
        Array.push_back(Xlh);
        m_pDBTrace->TraceVarIn ("GetAllZhDxlh xlh=[%s]\n",Xlh.szZD_xlh );
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock();

    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::GetInsomInfo(ShiMian &shim)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("GetInsomInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("GetInsomInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDId[ID_MAXLEN * 2 + 1];
    memset (szEscZDId, 0, sizeof(szEscZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(szEscZDId, shim.szZhD_xlh, strlen(shim.szZhD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT sleepLatency, sleepAll, sleepBed, waso, NA"
        <<  " FROM V_sm_stat"
        <<    " WHERE ZhD_xlh='" << szEscZDId << "' AND flagdate=DATE(FROM_UNIXTIME(" << shim.dwflagdate << "))";
    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    m_pDBTrace->TraceVarOut("time is %u --------------\n",shim.dwflagdate);
    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_NOROW;
    }

    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        shim.dwDelayTime = atoi (row[i]);i++;
        shim.dwAllSleep = atoi (row[i]);i++;
        shim.dwInBedTime = atoi (row[i]);i++;
        shim.dwWASO = atoi (row[i]);i++;
        shim.dwNA = atoi (row[i]);i++;
        //m_pDBTrace->TraceVarIn ("GetAllZhDxlh xlh=[%s]\n",Xlh.szZD_xlh );
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

int CEDUDBMgr::AddHX(HX &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("AddHX() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("AddHX() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char EscZhD[ID_MAXLEN * 2 + 1];
    memset ( EscZhD, 0, sizeof(stru.szZhD_xlh));
    m_pDBIdx[nDBIndex]->realEscapeString ( EscZhD, stru.szZhD_xlh, strlen (stru.szZhD_xlh));

    oss.str("");
    oss << "INSERT INTO T_sm_hx "
        << "(ZhD_xlh, stattime, HRate, RespRate) "
        << "VALUES "
        << "('" << EscZhD << "', FROM_UNIXTIME(" << stru.dwTime
        << "), " << stru.dwHeartRate << ", " << stru.dwRespRate <<")";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("AddHX() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //÷¥––≥…π¶
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}


int CEDUDBMgr::GetHX(HX &stru,PageList &plist,HXArray &Array)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("GetInsomInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("GetInsomInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    char szEscZDId[ID_MAXLEN * 2 + 1];
    memset (szEscZDId, 0, sizeof(szEscZDId));
    m_pDBIdx[nDBIndex]->realEscapeString(szEscZDId, stru.szZhD_xlh, strlen(stru.szZhD_xlh));

    char **row = NULL;
    int nAffectedRows = RowsInit;

    oss.str ("");
    oss << "SELECT UNIX_TIMESTAMP(stattime), RespRate, HRate "
        <<  "FROM T_sm_hx "
        <<    "WHERE ZhD_xlh='" << szEscZDId << "'"
        <<      "ORDER BY stattime DESC limit " << (plist.wPageNum - 1) * plist.wListNum << ", " << plist.wListNum;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    if (nAffectedRows == RowsErr)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }
    else if (RowsNot == nAffectedRows)
    {
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_NOROW;
    }


    while (row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i = 0;
        memset (&stru, 0, sizeof (stru));
        stru.dwTime = atoi (row[i]);i++;
        stru.dwRespRate = atoi (row[i]);i++;
        stru.dwHeartRate = atoi (row[i]);i++;
        Array.push_back(stru);
        //m_pDBTrace->TraceVarIn ("GetAllZhDxlh xlh=[%s]\n",Xlh.szZD_xlh );
    }

    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

