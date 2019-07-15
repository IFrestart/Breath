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
 * 连接数据库
 * @return DB_CONNECTION_OK  连接成功  ; DB_ERROR 连接失败
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
 * 断开与数据库的连接
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
 * 检查数据库连接状态, 如果失败并尝试重新恢复当前连接
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
 * 执行sql命令
 * @param szQuery 要执行的命令
 * @param nLen    要执行命令的长度
 * @return > 0 返回结果的行数; 0 WHERE子句没有匹配，即没有受影响的记录 ; -1 出现错误
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

    //如果查询成功，0。如果发生一个错误，非0
    int nRet = mysql_real_query(mysql, szQuery, nQueryLen);

    if (nRet == 0) //query OK
    {
        //一个保存结果的MYSQL_RES结构。如果出现一个错误，NULL
        result = mysql_store_result(mysql);
        //一个错误代码值。如果没有错误发生，0
        nRet = mysql_errno(mysql);
        if (nRet == 0)
            //大于0表示正确结果记录条数。0表示没有匹配的记录。-1表示查询返回错误。对于SELECT语句，必须在mysql_store_result()之后调用
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
 * 使用时必须在调用exec()之后调用
 * @return NULL	使用错误或者没有更多的行; NOT NULL	正常结果
 */
char** CMyDB::getRow()
{
    if (result == NULL)
        return NULL;
    return mysql_fetch_row(result);
}

/**
 * 使用时必须在调用exec()之后调用
 * @return NULL	使用错误或者没有更多的行; NOT NULL	正常结果
 */
char** CMyDB::getRow(int nIdx)
{
    if (result == NULL || nIdx < 0 || nIdx >= m_nAffectedRows)
        return NULL;
    mysql_data_seek(result, nIdx);
    return mysql_fetch_row(result);
}

/**
 * 使用时必须在调用exec()之后调用
 * @return NULL	使用错误或者没有结果集合 ; NOT NULL	正常结果
 */

int* CMyDB::getRowLengths()
{
    if (result == NULL)
        return NULL;
    return (int*)mysql_fetch_lengths(result);
}

/**
 *
 * @return 0 没有查询或者当前查询没有更新AUTO_INCREMENT值; >0 当前查询更新的AUTO_INCREMENT值
 */
int	CMyDB::getLastInertID()
{
    if (connected == DB_CONNECTION_NONE)
        return 0;

    return (int)mysql_insert_id(mysql);
}

/**
 * 每次执行SQL请求后调用，释放资源
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
 * 转义字符串
 * @param to 原串
 * @param from 目的串
 * @param length 返回字符串的长度
 */
unsigned long CMyDB::realEscapeString(char *to, CCHAR *from, unsigned long length)
{
    return mysql_real_escape_string(mysql, to, from, length);
}

/**
 * Return status information
 * @return DB_CONNECTION_OK 连接正常 ; DB_CONNECTION_NONE 未连接或连接中断
 */
int CMyDB::status()
{
    return connected;
}
/**
 * 获取当前数据库连接的错误信息
 * @return DB_CONNECTION_OK 连接正常 ; DB_CONNECTION_NONE 未连接或连接中断
 */

char* CMyDB::errMsg()
{
    if (connected == DB_CONNECTION_NONE)
        return error;
    return (char*)mysql_error(mysql);
}
/**
 * 获取当前数据库连接的空闲时间，单位：秒
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
 * 返回正在执行和等待执行的SQL请求的数量
 */
int	CMyDB::GetIsLocked()
{
    return m_nDBIsLocked;
}

/**
 * 每次请求执行SQL语句前调用
 * @param  nJustTry 0 如果trylock不成功，继续lock;  != 0 如果trylock不成功，直接返回
 * @return 0 锁定成功 ; -1 锁定失败
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
 * 每次请求执行SQL语句后调用
 * @return 0 解锁成功 ; -1 解锁失败
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
 * 设置打印日志类
 */
void CMyDB::SetTraceType(int nMsgMod, int nMsgTrace)
{
    m_pDBTrace->SetModAndTrace(nMsgMod, nMsgTrace);
}
/**
 * 获取执行正确的SQL语句的数量
 * @return
 */
long CMyDB::GetUseSQLCount()
{
    return m_nUseSQLCount;
}
/**
 * 获取执行错误的SQL语句的数量
 * @return
 */
long CMyDB::GetSQLErrorCount()
{
    return m_nSQLErrorCount;
}

/**
 * 打印请求SQL语句信息和执行结果
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
 * 按请求SQL语句执行结果记录执行SQL语句的数量, 每次调用数量加1
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

