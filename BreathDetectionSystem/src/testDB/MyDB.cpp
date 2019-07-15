/**
  * Copyright (c) 2004, HangZhou Webcon Corporation.
  * @file     MyDB.cpp
  * @author   white <white@webcon.com.cn>
  * @short
  * @date     03/2004
  **/


#include "MyDB.h"

#ifdef HOTBACK
#include <semaphore.h>
sem_t sem;

//Added by white, 2010-11-26, For server back
void *HandleData1(void* f){
    while(1){
        sem_wait(&sem);
        int j = 0;
        int k = 0;
        sem_getvalue(&sem, &k);
        for (j=0; j < k; j++)
        {
            sem_trywait(&sem);
        }
        system("/usr/local/WebCon/HA/sysdata.sh");
        sleep(2);
    }
}
//End add
#endif

CMyDB::CMyDB()
{
    m_pDBTrace = new CMMTDBTrace(MMTDB_MODULE_MYDB);

    mysql = NULL;
    result = NULL;
    connected = DB_CONNECTION_NONE;	// No connection yet
    m_nAffectedRows = -1;

    memset(error, 0, DB_ERROR_MAXLEN);
    strcpy(error, "No connection established");

    m_nUseSQLCount = 0;
    //m_nUseSQLGrpCount = 0;
    m_nSQLErrorCount = 0;
    //m_nSQLErrorGrpCount = 0;
    m_nDBIsLocked=0;

    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE_NP);

    //pthread_mutex_init(&m_mutex, NULL);
    pthread_mutex_init(&m_mutex, &m_attr);
}

CMyDB::~CMyDB()
{
    disconnect();		// Disconnect if connected to database
    pthread_mutexattr_destroy(&m_attr);
    if (m_pDBTrace)
        delete m_pDBTrace;
    m_pDBTrace = NULL;
}

/**
 * �������ݿ�
 * @return DB_CONNECTION_OK  ���ӳɹ�  ; DB_ERROR ����ʧ��
 */
int CMyDB::connect(char* host, char* user, char* passwd, unsigned short port, char* db, int nFlag)
{
    m_pDBTrace->TraceFunc("connect() begin, nFlag=[%d]\n", nFlag);

    if (nFlag == DB_CONNFLAG_RECOON)
    {
        disconnect();
    }

    mysql = mysql_init(NULL);
    if(mysql == NULL)
    {
        strcpy(error, "mysql_init() failed");
        return DB_ERROR;
    }

        if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, CLIENT_FOUND_ROWS) == NULL)
    {
        sprintf(error, "Failed to connect to database: Error: %s", mysql_error(mysql));

        mysql_close(mysql);
        mysql = NULL;
        return DB_ERROR;
    }

    connected = DB_CONNECTION_OK;

#ifdef HOTBACK
    //Added by white, 2010-11-26, For server back
    pthread_t t1;
    sem_init(&sem,0,0);
    pthread_create(&t1,NULL,HandleData1,NULL);
    //End add
#endif

    time(&m_timDBIdle);

    return connected;
}

/**
 * �Ͽ������ݿ������
 */
void CMyDB::disconnect()
{
    m_pDBTrace->TraceFunc("disconnect() begin\n");

    if (connected == DB_CONNECTION_NONE)
        return;

    connected = DB_CONNECTION_NONE;

    freeResult();
    if(mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}

/**
 * ������ݿ�����״̬, ���ʧ�ܲ��������»ָ���ǰ����
 * @return 0: server is alive, CR_COMMANDS_OUT_OF_SYNC: order is wrong, CR_SERVER_GONE_ERROR: mysql is dead, CR_UNKNOWN_ERROR
 */
int CMyDB::db_ping()
{
    m_pDBTrace->TraceFunc("db_ping() begin\n");

    //0: server is alive, CR_COMMANDS_OUT_OF_SYNC: order is wrong, CR_SERVER_GONE_ERROR: mysql is dead, CR_UNKNOWN_ERROR
    if (!mysql)
        return -4;
    int nRet = exec("SELECT 1");

    if (nRet >= 0)
        return 0;

    nRet = mysql_ping(mysql);
    if (nRet == 0)
        return 0;
    else if (nRet == CR_SERVER_GONE_ERROR)
        return -1;
    else if (nRet == CR_COMMANDS_OUT_OF_SYNC)
        return -2;
    else if (nRet == CR_UNKNOWN_ERROR)
        return -3;
}

/**
 * ִ��sql����
 * @param szQuery Ҫִ�е�����
 * @param nLen    Ҫִ������ĳ���
 * @return > 0 ���ؽ��������; 0 WHERE�Ӿ�û��ƥ�䣬��û����Ӱ��ļ�¼ ; -1 ���ִ���
 */
int CMyDB::exec(char* szQuery, int nLen)
{
    m_pDBTrace->TraceFunc("exec() begin\n");

    if (connected == DB_CONNECTION_NONE)
        return -1;

    freeResult();

    time(&m_timDBIdle);

    int nQueryLen;
    if (nLen <= 0)
        nQueryLen = strlen(szQuery);
    else
        nQueryLen = nLen;

    //�����ѯ�ɹ���0���������һ�����󣬷�0
    int nRet = mysql_real_query(mysql, szQuery, nQueryLen);

    if (nRet == 0) //query OK
    {
        //һ����������MYSQL_RES�ṹ���������һ������NULL
        result = mysql_store_result(mysql);
        //һ���������ֵ�����û�д�������0
        nRet = mysql_errno(mysql);
        if (nRet == 0)
            //����0��ʾ��ȷ�����¼������0��ʾû��ƥ��ļ�¼��-1��ʾ��ѯ���ش��󡣶���SELECT��䣬������mysql_store_result()֮�����
            m_nAffectedRows = (long)mysql_affected_rows(mysql);
    }

    AddUseSQLCount(nRet);

    traceSQL(nRet, nQueryLen, szQuery);

#ifdef HOTBACK
    //Added by white, 2010-11-26, For server back
    if (nRet == 0 && m_nAffectedRows > 0 && result == NULL)
    {
        sem_post(&sem);
        int k = 0;
        sem_getvalue(&sem, &k);

        m_pDBTrace->TraceStatus("Thread sem_numm++ [%d]\n", k);
    }
    //End add
#endif

    return m_nAffectedRows;
}

/**
 * ʹ��ʱ�����ڵ���exec()֮�����
 * @return NULL	ʹ�ô������û�и������; NOT NULL	�������
 */
char** CMyDB::getRow()
{
    if (result == NULL)
        return NULL;
    return mysql_fetch_row(result);
}

/**
 * ʹ��ʱ�����ڵ���exec()֮�����
 * @return NULL	ʹ�ô������û�и������; NOT NULL	�������
 */
char** CMyDB::getRow(int nIdx)
{
    if (result == NULL || nIdx < 0 || nIdx >= m_nAffectedRows)
        return NULL;
    mysql_data_seek(result, nIdx);
    return mysql_fetch_row(result);
}

/**
 * ʹ��ʱ�����ڵ���exec()֮�����
 * @return NULL	ʹ�ô������û�н������ ; NOT NULL	�������
 */

int* CMyDB::getRowLengths()
{
    if (result == NULL)
        return NULL;
    return (int*)mysql_fetch_lengths(result);
}

/**
 *
 * @return 0 û�в�ѯ���ߵ�ǰ��ѯû�и���AUTO_INCREMENTֵ; >0 ��ǰ��ѯ���µ�AUTO_INCREMENTֵ
 */
int	CMyDB::getLastInertID()
{
    if (connected == DB_CONNECTION_NONE)
        return 0;

    return (int)mysql_insert_id(mysql);
}

/**
 * ÿ��ִ��SQL�������ã��ͷ���Դ
 */
void CMyDB::freeResult()
{
    m_nAffectedRows = -1;
    if (result != NULL)
    {
        mysql_free_result(result);
        result = NULL;
    }
}

/**
 * ת���ַ���
 * @param to ԭ��
 * @param from Ŀ�Ĵ�
 * @param length �����ַ����ĳ���
 */
unsigned long CMyDB::realEscapeString(char *to, CCHAR *from, unsigned long length)
{
    return mysql_real_escape_string(mysql, to, from, length);
}

/**
 * Return status information
 * @return DB_CONNECTION_OK �������� ; DB_CONNECTION_NONE δ���ӻ������ж�
 */
int CMyDB::status()
{
    return connected;
}
/**
 * ��ȡ��ǰ���ݿ����ӵĴ�����Ϣ
 * @return DB_CONNECTION_OK �������� ; DB_CONNECTION_NONE δ���ӻ������ж�
 */

char* CMyDB::errMsg()
{
    if (connected == DB_CONNECTION_NONE)
        return error;
    return (char*)mysql_error(mysql);
}
/**
 * ��ȡ��ǰ���ݿ����ӵĿ���ʱ�䣬��λ����
 */

int	CMyDB::GetIdleTime()
{
    time_t timCurTime;
    time(&timCurTime);

    int nIdleTime = (int)timCurTime - (int)m_timDBIdle;
    time(&m_timDBIdle);

    return nIdleTime;
}

/**
 * ��������ִ�к͵ȴ�ִ�е�SQL���������
 */
int	CMyDB::GetIsLocked()
{
    return m_nDBIsLocked;
}

/**
 * ÿ������ִ��SQL���ǰ����
 * @param  nJustTry 0 ���trylock���ɹ�������lock;  != 0 ���trylock���ɹ���ֱ�ӷ���
 * @return 0 �����ɹ� ; -1 ����ʧ��
 */

int	CMyDB::ThreadLock(int nJustTry)
{
    m_nDBIsLocked++;

    // EBUSY  the mutex could not be acquired because it was currently locked.
    // EINVAL the mutex has not been properly initialized.
    int nRet = pthread_mutex_trylock(&m_mutex);
    m_pDBTrace->TraceH4("ThreadLock() - trylock, m_mutex=[%d], nRet=[%d]\n", &m_mutex, nRet);

    if (nRet == EBUSY && nJustTry == 0)
    {
        m_pDBTrace->TraceH3("ThreadLock() - low busy, count=[%d]\n", m_nDBIsLocked);

        // EINVAL 	the mutex has not been properly initialized.
        nRet = pthread_mutex_lock(&m_mutex);
    }

    if (nRet != 0)
    {
        m_nDBIsLocked--;
        m_pDBTrace->TraceError("ThreadLock() - lock failed, try=[%d], nRet=[%d]\n", nJustTry, nRet);
        return -1;
    }
    else
        //m_pDBTrace->TraceError("ThreadLock() - lock ok!\n");
        return 0;
}
/**
 * ÿ������ִ��SQL�������
 * @return 0 �����ɹ� ; -1 ����ʧ��
 */
int	CMyDB::ThreadUnlock()
{
    if (m_nDBIsLocked <= 0)
    {
        m_pDBTrace->TraceError("ThreadUnlock() - unlock failed, count=[%d]\n", m_nDBIsLocked);
        return -1;
    }

    int nRet = pthread_mutex_unlock(&m_mutex);
    m_pDBTrace->TraceH4("ThreadUnlock() - m_mutex=[%d], nRet=[%d], count=[%d]\n", &m_mutex, nRet, m_nDBIsLocked);

    m_nDBIsLocked--;

    if (nRet != 0)
    {
        m_pDBTrace->TraceError("ThreadUnlock() - unlock failed, nRet=[%d]\n", nRet);
        return -1;
    }
    else
    {
        //m_pDBTrace->TraceError("ThreadUnlock() - unlock ok\n");
        return 0;
    }
}


/**
 * ���ô�ӡ��־��
 */
void CMyDB::SetTraceType(int nMsgMod, int nMsgTrace)
{
    m_pDBTrace->SetModAndTrace(nMsgMod, nMsgTrace);
}
/**
 * ��ȡִ����ȷ��SQL��������
 * @return
 */
long CMyDB::GetUseSQLCount()
{
    return m_nUseSQLCount;
}
/**
 * ��ȡִ�д����SQL��������
 * @return
 */
long CMyDB::GetSQLErrorCount()
{
    return m_nSQLErrorCount;
}

/**
 * ��ӡ����SQL�����Ϣ��ִ�н��
 */

void CMyDB::traceSQL(int nRet, int nLen, char* szQuery)
{
    if (nRet == 0)
//        m_pDBTrace->TraceSQL("traceSQL() - rows=[%d], nLen=[%d], SQL_COUNTER=[%d], [%s]\n",
//                     m_nAffectedRows, nLen, m_nUseSQLCount, szQuery);
        m_pDBTrace->TraceSQL("traceSQL() - rows=[%d], nLen=[%d], SQL_COUNTER=[%d]\n",
                    m_nAffectedRows, nLen, m_nUseSQLCount);
    else
        m_pDBTrace->TraceError("traceSQL() - nRet=[%d], nLen=[%d], ERROR_SQL_COUNTER=[%d], errmsg=[%s], [%s]\n",
                       nRet, nLen, m_nSQLErrorCount, errMsg(), szQuery);
//        m_pDBTrace->TraceError("traceSQL() - nRet=[%d], nLen=[%d], ERROR_SQL_COUNTER=[%d], errmsg=[%s]\n",
//                    nRet, nLen, m_nSQLErrorCount, errMsg());
}

/**
 * ������SQL���ִ�н����¼ִ��SQL��������, ÿ�ε���������1
 */
void CMyDB::AddUseSQLCount(int nRet)
{
    if (nRet == 0)
    {
        m_nUseSQLCount++;
    }
    else
    {

        m_nSQLErrorCount++;
    }
}

