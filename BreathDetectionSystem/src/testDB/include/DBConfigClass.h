 /**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     DBConfigClass.h
  * @author   white <white@webcon.com.cn>
  * @short
  * @date     03/2004
  **/

#ifndef __DBCONFIGCLASS_H__
#define __DBCONFIGCLASS_H__


#include "TypeDef.h"
#include "FileGetPara.h"
#include "MMTDBModuleTrace.h"


#ifndef MAX_EDUDBSVR_COUNT
#define MAX_EDUDBSVR_COUNT		5
#endif


typedef struct tagDBConfigSvrInfo
{
	char			szSvrIP[32];
	unsigned short	shSvrPort;
} DBConfigSvrInfo, *LPDBCONFIGSVRINFO;

typedef struct tagDBConfigSvrInfoOfDB : public DBConfigSvrInfo
{
	char	szUName[32];
	char	szPassword[32];
	char	szDBName[32];
	int		nIdx;
} DBConfigSvrInfoOfDB, *LPDBCONFIGSVRINFOOFDB;

typedef CToolArrayBase<DBConfigSvrInfoOfDB, MAX_EDUDBSVR_COUNT>  CEDUDBConfigSvrArray;

class CDBConfigClass
{
public:
    CDBConfigClass(CCHAR *szFileName)
    {
    	m_iEDUDBSvrCount = 0;	
    	m_nMsgTrace = 0;
    	m_nMsgMod = 0;
    	
		m_pDBTrace = new CMMTDBTrace(MMTDB_MODULE_DBCONFIGCLASS);
    	
    	Init(szFileName);
    }
    ~CDBConfigClass()
    {
		if (m_pDBTrace)
            delete m_pDBTrace;
		m_pDBTrace = NULL;
    }

public:
    void Init(CCHAR* szFileName)
	{
		CFileGetPara *pFileGetPara = new CFileGetPara();
		int nRet = 0;
		if (szFileName)
			nRet = pFileGetPara->InitConfig(szFileName);
		else
		{
			printf("error initConfig \n");
			exit(1);
		}
		if (nRet != 0)
		{
			m_pDBTrace->TraceError("InitConfig failed! nRet=[%d]\n", nRet);
			return;
		}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get trace type
		m_nMsgTrace = pFileGetPara->GetParaInt("Trace", "MsgTrace", 0);
		m_nMsgMod = pFileGetPara->GetParaInt("Trace", "MsgMod", 0);
		
		SetTraceType(m_nMsgMod, m_nMsgTrace);
		
		m_pDBTrace->TraceVar("m_nMsgTrace=[%d]\n", m_nMsgTrace);
		m_pDBTrace->TraceVar("m_nMsgMod=[%d]\n", m_nMsgMod);
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//EDU DB svr
		m_iEDUDBSvrCount = pFileGetPara->GetParaInt("EDUDBSvr", "count", 0);
		
		m_pDBTrace->TraceVar("m_iEDUDBSvrCount=[%d]\n", m_iEDUDBSvrCount);
		if (m_iEDUDBSvrCount > 0)
		{
			for (int i = 0; i < m_iEDUDBSvrCount; i++)
			{
				char ipitem[16];
				memset(ipitem, 0, 16);
				char portitem[16];
				memset(portitem, 0, 16);
				char useritem[16];
				memset(useritem, 0, 16);
				char passworditem[16];
				memset(passworditem, 0, 16);
				char dbnameitem[16];
				memset(dbnameitem, 0, 16);
				sprintf(ipitem, "ipaddress%d", i);
				sprintf(portitem, "port%d", i);
				sprintf(useritem, "user%d", i);
				sprintf(passworditem, "password%d", i);
				sprintf(dbnameitem, "dbname%d", i);
				
				DBConfigSvrInfoOfDB struSvrInfoOfDB;
    			memset(&struSvrInfoOfDB, 0, sizeof(DBConfigSvrInfoOfDB));
				strcpy(struSvrInfoOfDB.szSvrIP, pFileGetPara->GetParaString("EDUDBSvr", ipitem, "localhost"));
				struSvrInfoOfDB.shSvrPort = (short)pFileGetPara->GetParaInt("EDUDBSvr", portitem, 0);
				strcpy(struSvrInfoOfDB.szUName, pFileGetPara->GetParaString("EDUDBSvr", useritem, ""));
				strcpy(struSvrInfoOfDB.szPassword, pFileGetPara->GetParaString("EDUDBSvr", passworditem, ""));
				strcpy(struSvrInfoOfDB.szDBName, pFileGetPara->GetParaString("EDUDBSvr", dbnameitem, ""));
				
				m_EDUDBSvrArray.Add(struSvrInfoOfDB);
				
				m_pDBTrace->TraceVar("m_EDUDBSvrArray[%d], szSvrIP=[%s], shSvrPort=[%d], "
					"szUName=[%s], szPassword=[%s], szDBName=[%s]\n", 
		    		i, 
		    		m_EDUDBSvrArray[i].szSvrIP, 
		    		m_EDUDBSvrArray[i].shSvrPort, 
		    		m_EDUDBSvrArray[i].szUName, 
		    		m_EDUDBSvrArray[i].szPassword, 
		    		m_EDUDBSvrArray[i].szDBName
		    		);
			}
		}
		else
			m_iEDUDBSvrCount = 0;
		
		
		if (pFileGetPara)
			delete pFileGetPara;
		pFileGetPara = NULL;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ED DB svr
	int		GetEDUDBSvrCount(){ return m_iEDUDBSvrCount; }
	int		GetEDUDBSvrInfo(int nIdx, DBConfigSvrInfoOfDB& struDBConfigSvrInfoOfDB)
	{
		m_pDBTrace->TraceFunc("GetEDUDBSvrInfo() begin\n");
		
		if (m_iEDUDBSvrCount <= 0 || nIdx < 0 || nIdx >= m_iEDUDBSvrCount)
			return -1;
		
		strcpy(struDBConfigSvrInfoOfDB.szSvrIP, m_EDUDBSvrArray[nIdx].szSvrIP);
		struDBConfigSvrInfoOfDB.shSvrPort = m_EDUDBSvrArray[nIdx].shSvrPort;
		if (strlen(m_EDUDBSvrArray[nIdx].szUName) > 0)
			strcpy(struDBConfigSvrInfoOfDB.szUName, m_EDUDBSvrArray[nIdx].szUName);
		if (strlen(m_EDUDBSvrArray[nIdx].szPassword) > 0)
			strcpy(struDBConfigSvrInfoOfDB.szPassword, m_EDUDBSvrArray[nIdx].szPassword);
		if (strlen(m_EDUDBSvrArray[nIdx].szDBName) > 0)
			strcpy(struDBConfigSvrInfoOfDB.szDBName, m_EDUDBSvrArray[nIdx].szDBName);
		struDBConfigSvrInfoOfDB.nIdx = nIdx;
		
		return 0;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get trace type
	//MMTDB_TRACE()
	int		GetMsgTrace() { return m_nMsgTrace; }
	//MMTDB_TRACE()
	int		GetMsgMod() { return m_nMsgMod; }
	void	ReInitTrace()
	{
		CFileGetPara *pFileGetPara = new CFileGetPara();
		int nRet = pFileGetPara->InitConfig("DBCommServ.ini");
		if (nRet != 0)
			return;
		
		m_nMsgTrace = pFileGetPara->GetParaInt("Trace", "MsgTrace", 0);
		m_nMsgMod = pFileGetPara->GetParaInt("Trace", "MsgMod", 0);
		
		m_pDBTrace->TraceTimer("m_nMsgTrace=[%d]\n", m_nMsgTrace);
		m_pDBTrace->TraceTimer("m_nMsgMod=[%d]\n", m_nMsgMod);
		
		if (pFileGetPara)
			delete pFileGetPara;
		pFileGetPara = NULL;
	}
	
    void SetTraceType(int nMsgMod, int nMsgTrace)
	{
		m_pDBTrace->SetModAndTrace(nMsgMod, nMsgTrace);
	}
	

private:

	int		m_iEDUDBSvrCount;
	CEDUDBConfigSvrArray	m_EDUDBSvrArray;
	int		m_nMsgTrace;		//MMTDB_TRACE()
	int		m_nMsgMod;			//MMTDB_TRACE()
	
	
private:
    CMMTDBTrace*	m_pDBTrace;
};



#endif	//__DBCONFIGCLASS_H__
