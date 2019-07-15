/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.

 * @file     MyDB.h
 * @author   white <white@webcon.com.cn>
 * @short
 * @date     03/2004
 **/

#ifndef MYDB_H_MYSQL
#define MYDB_H_MYSQL

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <stdarg.h>

#include <pthread.h>
#include <errno.h>			//For pthread_mutex_trylock() error message

#include "mysql.h"
#include "errmsg.h"
#include "MMTDBModuleTrace.h"
#include "TypeDef.h"

#define DB_CONNECTION_OK	0
#define DB_ERROR 5

#define DB_CONNECTION_NONE	1

#define DB_ERROR_MAXLEN		128

#define DB_CONNFLAG_FIRSTCOON	0
#define DB_CONNFLAG_RECOON		1

class CMyDB
{
public:
   CMyDB();
   virtual ~CMyDB();

   /**
    * 连接数据库
    * @return DB_CONNECTION_OK  连接成功  ; DB_ERROR 连接失败
    */
   int connect(char* host, char* user, char* passwd, unsigned short port, char* db, int nFlag=DB_CONNFLAG_FIRSTCOON);	// Connect to db

   /**
    * 断开与数据库的连接
    */
   void disconnect();

   /**
    * 检查数据库连接状态, 如果失败并尝试重新恢复当前连接
    * @return 0: server is alive, CR_COMMANDS_OUT_OF_SYNC: 命令错误, CR_SERVER_GONE_ERROR: mysql已关闭, CR_UNKNOWN_ERROR
    */
   int db_ping();

   /**
    * 执行sql命令
    * @param szQuery 要执行的命令
    * @param nLen    要执行命令的长度
    * @return > 0 返回结果的行数; 0 WHERE子句没有匹配，即没有受影响的记录 ; -1 出现错误
    */
   int exec(char* szQuery, int nLen=0);

   /**
    * 使用时必须在调用exec()之后调用
    * @return NULL	使用错误或者没有更多的行; NOT NULL	正常结果
    */
    char** getRow();

   /**
    * 使用时必须在调用exec()之后调用
    * @return NULL	使用错误或者没有更多的行; NOT NULL	正常结果
    */
    char** getRow(int nIdx);

   /**
    * 使用时必须在调用exec()之后调用
    * @return NULL	使用错误或者没有结果集合 ; NOT NULL	正常结果
    */
   int* getRowLengths();

   /**
    *
    * @return 0 没有查询或者当前查询没有更新AUTO_INCREMENT值; >0 当前查询更新的AUTO_INCREMENT值
    */
   int	getLastInertID();

   /**
    * 转义字符串
    * @param to 原串
    * @param from 目的串
    * @param length 返回字符串的长度
    */
   unsigned long realEscapeString(char *to, CCHAR *from, unsigned long length);

   /**
    * 每次执行SQL请求后调用，释放资源
    */
   void freeResult();

   /**
    * Return status information
    * @return DB_CONNECTION_OK 连接正常 ; DB_CONNECTION_NONE 未连接或连接中断
    */
   int status();

   /**
    * 获取当前数据库连接的错误信息
    * @return DB_CONNECTION_OK 连接正常 ; DB_CONNECTION_NONE 未连接或连接中断
    */
   char* errMsg();

   /**
    * 获取当前数据库连接的空闲时间，单位：秒
    */
   int	GetIdleTime();

   /**
    * 返回正在执行和等待执行的SQL请求的数量
    */
   int	GetIsLocked();

   /**
    * 每次请求执行SQL语句前调用
    * @param  nJustTry 0 如果trylock不成功，继续lock;  != 0 如果trylock不成功，直接返回
    * @return 0 锁定成功 ; -1 锁定失败
    */
   int	ThreadLock(int nJustTry=0);
   /**
    * 每次请求执行SQL语句后调用
    * @return 0 解锁成功 ; -1 解锁失败
    */
   int	ThreadUnlock();


   /**
    * 获取执行正确的SQL语句的数量
    * @return
    */
   long GetUseSQLCount();
   //int GetUseSQLGrpCount();
   //int GetSQLErrorCount();

   /**
    * 获取执行错误的SQL语句的数量
    * @return
    */
   long GetSQLErrorCount();
   //int GetSQLErrorGrpCount();

   /**
    * 设置打印日志类
    */
   void SetTraceType(int nMsgMod, int nMsgTrace);

private:
   /**
    * 打印请求SQL语句信息和执行结果
    */
   void traceSQL(int nRet, int nLen, char* szQuery);

   /**
    * 按请求SQL语句执行结果记录执行SQL语句的数量, 每次调用数量加1
    */
   void AddUseSQLCount(int nRet=0);

private:
   MYSQL* mysql;

   /**
    * 当前数据库连接的状态
    */
   int connected;

   /**
    * Error description
    */
   char error[DB_ERROR_MAXLEN];

   /**
    * 保存执行当前SQL请求后返回的结果
    */
   MYSQL_RES* result;

   /**
    * 记录执行当前SQL请求后返回的结果记录数量
    */
   long m_nAffectedRows;
   /**
    * 线程锁
    */
   pthread_mutexattr_t  m_attr;
   pthread_mutex_t		m_mutex;

   /**
    * 记录正在执行和等待执行的SQL请求的数量
    */
   int	m_nDBIsLocked;

   /**
    * 记录当前数据库连接的空闲时间
    */
   time_t	m_timDBIdle;

   /**
    * 计数器, 记录执行正确的SQL语句的数量
    */
   long m_nUseSQLCount;

   /**
    * 计数器, 记录执行错误的SQL语句的数量
    */
   long m_nSQLErrorCount;

   /**
    * 打印日志的专门类，将以程一堂的日志类替换
    */
   CMMTDBTrace*	m_pDBTrace;
};

#endif // MYDB_H_MYSQL_

