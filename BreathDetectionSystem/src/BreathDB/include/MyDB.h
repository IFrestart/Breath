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
    * �������ݿ�
    * @return DB_CONNECTION_OK  ���ӳɹ�  ; DB_ERROR ����ʧ��
    */
   int connect(char* host, char* user, char* passwd, unsigned short port, char* db, int nFlag=DB_CONNFLAG_FIRSTCOON);	// Connect to db

   /**
    * �Ͽ������ݿ������
    */
   void disconnect();

   /**
    * ������ݿ�����״̬, ���ʧ�ܲ��������»ָ���ǰ����
    * @return 0: server is alive, CR_COMMANDS_OUT_OF_SYNC: �������, CR_SERVER_GONE_ERROR: mysql�ѹر�, CR_UNKNOWN_ERROR
    */
   int db_ping();

   /**
    * ִ��sql����
    * @param szQuery Ҫִ�е�����
    * @param nLen    Ҫִ������ĳ���
    * @return > 0 ���ؽ��������; 0 WHERE�Ӿ�û��ƥ�䣬��û����Ӱ��ļ�¼ ; -1 ���ִ���
    */
   int exec(char* szQuery, int nLen=0);

   /**
    * ʹ��ʱ�����ڵ���exec()֮�����
    * @return NULL	ʹ�ô������û�и������; NOT NULL	�������
    */
    char** getRow();

   /**
    * ʹ��ʱ�����ڵ���exec()֮�����
    * @return NULL	ʹ�ô������û�и������; NOT NULL	�������
    */
    char** getRow(int nIdx);

   /**
    * ʹ��ʱ�����ڵ���exec()֮�����
    * @return NULL	ʹ�ô������û�н������ ; NOT NULL	�������
    */
   int* getRowLengths();

   /**
    *
    * @return 0 û�в�ѯ���ߵ�ǰ��ѯû�и���AUTO_INCREMENTֵ; >0 ��ǰ��ѯ���µ�AUTO_INCREMENTֵ
    */
   int	getLastInertID();

   /**
    * ת���ַ���
    * @param to ԭ��
    * @param from Ŀ�Ĵ�
    * @param length �����ַ����ĳ���
    */
   unsigned long realEscapeString(char *to, CCHAR *from, unsigned long length);

   /**
    * ÿ��ִ��SQL�������ã��ͷ���Դ
    */
   void freeResult();

   /**
    * Return status information
    * @return DB_CONNECTION_OK �������� ; DB_CONNECTION_NONE δ���ӻ������ж�
    */
   int status();

   /**
    * ��ȡ��ǰ���ݿ����ӵĴ�����Ϣ
    * @return DB_CONNECTION_OK �������� ; DB_CONNECTION_NONE δ���ӻ������ж�
    */
   char* errMsg();

   /**
    * ��ȡ��ǰ���ݿ����ӵĿ���ʱ�䣬��λ����
    */
   int	GetIdleTime();

   /**
    * ��������ִ�к͵ȴ�ִ�е�SQL���������
    */
   int	GetIsLocked();

   /**
    * ÿ������ִ��SQL���ǰ����
    * @param  nJustTry 0 ���trylock���ɹ�������lock;  != 0 ���trylock���ɹ���ֱ�ӷ���
    * @return 0 �����ɹ� ; -1 ����ʧ��
    */
   int	ThreadLock(int nJustTry=0);
   /**
    * ÿ������ִ��SQL�������
    * @return 0 �����ɹ� ; -1 ����ʧ��
    */
   int	ThreadUnlock();


   /**
    * ��ȡִ����ȷ��SQL��������
    * @return
    */
   long GetUseSQLCount();
   //int GetUseSQLGrpCount();
   //int GetSQLErrorCount();

   /**
    * ��ȡִ�д����SQL��������
    * @return
    */
   long GetSQLErrorCount();
   //int GetSQLErrorGrpCount();

   /**
    * ���ô�ӡ��־��
    */
   void SetTraceType(int nMsgMod, int nMsgTrace);

private:
   /**
    * ��ӡ����SQL�����Ϣ��ִ�н��
    */
   void traceSQL(int nRet, int nLen, char* szQuery);

   /**
    * ������SQL���ִ�н����¼ִ��SQL��������, ÿ�ε���������1
    */
   void AddUseSQLCount(int nRet=0);

private:
   MYSQL* mysql;

   /**
    * ��ǰ���ݿ����ӵ�״̬
    */
   int connected;

   /**
    * Error description
    */
   char error[DB_ERROR_MAXLEN];

   /**
    * ����ִ�е�ǰSQL����󷵻صĽ��
    */
   MYSQL_RES* result;

   /**
    * ��¼ִ�е�ǰSQL����󷵻صĽ����¼����
    */
   long m_nAffectedRows;
   /**
    * �߳���
    */
   pthread_mutexattr_t  m_attr;
   pthread_mutex_t		m_mutex;

   /**
    * ��¼����ִ�к͵ȴ�ִ�е�SQL���������
    */
   int	m_nDBIsLocked;

   /**
    * ��¼��ǰ���ݿ����ӵĿ���ʱ��
    */
   time_t	m_timDBIdle;

   /**
    * ������, ��¼ִ����ȷ��SQL��������
    */
   long m_nUseSQLCount;

   /**
    * ������, ��¼ִ�д����SQL��������
    */
   long m_nSQLErrorCount;

   /**
    * ��ӡ��־��ר���࣬���Գ�һ�õ���־���滻
    */
   CMMTDBTrace*	m_pDBTrace;
};

#endif // MYDB_H_MYSQL_

