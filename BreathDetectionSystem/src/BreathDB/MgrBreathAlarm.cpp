/**
  * Copyright (c) 2019, HangZhou Webcon Corporation.
  * @file     BreathAlarmMgr.cpp
  * @author   shizc <shizc@webcon.com.cn>
  * @brief    报警信息相关函数
  * @date     2019-03-04
  **/
#include <sys/timeb.h>
#include "EDUDBMgr.h"
#include <stdio.h>
#include <stdlib.h>

//添加报警信息
int CEDUDBMgr::AddBreathAlarmInfo(BreathAlarmInfo &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("AddBreathAlarmInfo() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("AddBreathAlarmInfo() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //转义报警信息
    BYTE* lpMsg=(BYTE *) new char[2 * stru.wMessageLen + 1];
    m_pDBIdx[nDBIndex]->realEscapeString ((char *) lpMsg,(char *) stru.lpMassage,stru.wMessageLen);

    oss.str("");
    oss << "INSERT INTO alarm_breath (ZD_xlh, alarm_time, type, alarm_duration, msg_len, msg) "
        << "VALUES ('" << stru.szServerId << "', FROM_UNIXTIME(" << stru.dwAlarmTime
        << "), " << stru.wAlarmType << ", " << stru.shAlarmdur << ", "
        << stru.wMessageLen << ", '" << lpMsg <<"') ";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("AddBreathAlarmInfo() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //执行成功
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}

//查看报警信息加波形图
int CEDUDBMgr::GetAlarmInfoData(BreathAlarmInfo &stru)
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

    //查找报警信息
    oss.str ("");
    oss << "select type, UNIX_TIMESTAMP(alarm_time), alarm_duration, msg";
    oss << " from alarm_breath where type&" << stru.wAlarmType <<
           " and ZD_xlh='"<< szEscServerId << "' and alarm_time = FROM_UNIXTIME(" << stru.dwAlarmTime << ")";

    nAffectedRows =m_pDBIdx[nDBIndex]->exec ((char *) (oss.str ().c_str ()));
    //printf("getUserList() sql=[%s]\n",oss.str().c_str());
    if(nAffectedRows == RowsErr)
    {
        m_pDBTrace->TraceError ("getUserList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_TERMINAL_NOTEXIST;
    }

    if (nAffectedRows == RowsNot)
    {
        m_pDBTrace->TraceError ("getUserList() - nAffectedRows=[-1]\n");
        m_pDBIdx[nDBIndex]->ThreadUnlock ();
        return DB_ERR_DBDATA_ERROR;
    }

    if(row = m_pDBIdx[nDBIndex]->getRow ())
    {
        int i=0;
        stru.wAlarmType = atoi (row[i]);i++;
        stru.dwAlarmTime = atoi (row[i]);i++;
        stru.shAlarmdur = atoi (row[i]);i++;
        int *nLens = m_pDBIdx[nDBIndex]->getRowLengths ();
        stru.wMessageLen = nLens[i];
        stru.lpMassage = new BYTE[stru.wMessageLen];
        memcpy (stru.lpMassage, row[i],nLens[i]);i++;
    }
    else
    {
        cout << "GetAlarmInfoData RowNull" << endl;
    }
    m_pDBIdx[nDBIndex]->ThreadUnlock ();
    return DB_ERR_SUCCESS;
}


//添加场所状况
int CEDUDBMgr::AddSpaceStat(BreathAlarmInfo2 &stru)
{
    int nDBIndex = GetDBIndexID();
    if (nDBIndex < 0)
    {
        m_pDBTrace->TraceError("AddSpaceStat() - GetDBIndexID=[%lu]\n", nDBIndex);
        return nDBIndex;
    }
    int nRet = m_pDBIdx[nDBIndex]->ThreadLock();

    if (nRet != DB_ERR_SUCCESS)
    {
        m_pDBTrace->TraceError("AddSpaceStat() - ThreadLock[%lu]=[%lu]\n", nDBIndex, nRet);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    oss.str("");
    oss << "INSERT INTO alarm_info (ZD_xlh, alarm_time, type, param) VALUES ('" << stru.szServerId << "', FROM_UNIXTIME("
        << stru.dwAlarmTime << "), " << stru.wAlarmType << ", " << stru.wAlarmParam << ") ";

    int nAffectedRows=RowsInit;
    nAffectedRows = m_pDBIdx[nDBIndex]->exec((char*)oss.str().c_str());
    if( RowsErr == nAffectedRows)
    {
        m_pDBTrace->TraceError("AddSpaceStat() - sql=[%s]\n nAffectedRows=[%d]\n",oss.str().c_str(), RowsErr);
        m_pDBIdx[nDBIndex]->ThreadUnlock();
        return DB_ERR_DBDATA_ERROR;
    }

    //执行成功
    m_pDBIdx[nDBIndex]->ThreadUnlock();
    return DB_ERR_SUCCESS;
}
