/**
  * Copyright (c) 2015, HangZhou Webcon Centeroration.

  * @file     EDUDBRes.h
  * @author   shizc <shizc@webcon.com.cn>
  * @short
  * @date     07/2015
  **/

#ifndef __EDUDBRES_H__
#define __EDUDBRES_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>

#include <unistd.h>
#include <stdarg.h>

#include "TypeDef.h"
#include "BaseArray.h"
#include "DBSubPDU.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Const code

#define EDUDB_CONST_CONN_NUM 			1
#define EDUDB_CONST_ISLOCKED			1
#define EDUDB_CONST_ISUNLOCKED			0
#define EDUDB_CONST_LOCK_MAXCOUNT		5

#define EDUDB_CONST_EASTONID_MAXLEN		32
#define EDUDB_CONST_EASTONID_MINLEN		8

#define EDUDB_CONST_CLIVERSION_MAXLEN	64

#define EDUDB_CONST_UNIXTIME_MIN		1041350400	/**< 2003-01-01 00:00:00 */
#define EDUDB_CONST_UNIXTIME_MAX		2147483647	/**< 2038-01-19 11:14:07 */

//Struct define
#define ID_MAXLEN			127     //ID����
#define USER_PWD_MAXLEN		17      //�û����볤��
#define ALARM_MASSAGE_LEN   1200    //������Ϣ����

/**
 *��������ն˻�����Ϣ
 */
typedef struct tagZhDnInfo
{
    char    szXlh[ID_MAXLEN];      //���к�
    char    szName[ID_MAXLEN];     //�ǳ�
    DWORD   dwindex;
    DWORD   dwType;                //����
    DWORD   dwMfgTime;             //��������
    DWORD   dwRegTime;             //��һ������ʱ��
}ZhDInfo, *LPZDINfO;
typedef std::vector<ZhDInfo> ZDInfoArray;

/**
 *��������ն˱�����Ϣ
 */
typedef struct tagBreathAlarmInfo
{
    WORD    wAlarmType;     //��������
    WORD    wMessageLen;    //��Ϣ����
    WORD    shAlarmdur;     //��������ʱ��
    DWORD   dwAlarmTime;    //����ʱ��
    DWORD   dwStartTime;    //��ʼʱ��
    DWORD   dwLimitTime;    //��ֹʱ��
    char    szServerId[ID_MAXLEN];     //�豸ID
    BYTE*   lpMassage;   //������Ϣ
}BreathAlarmInfo, *BREATHALARMINFO;
typedef std::vector<BreathAlarmInfo> BreathAlarmInfoArray;
typedef BreathAlarmInfoArray::iterator BreathAlarmInfoArray_it;

/**
 *��������ն˱�����Ϣ
 */
typedef struct tagBreathAlarmInfo2
{
    WORD    wAlarmType;     //��������
    WORD    wAlarmParam;     //���ͽ���
    DWORD   dwStartTime;    //��ʼʱ��
    DWORD   dwLimitTime;    //��ֹʱ��
    DWORD   dwAlarmTime;    //����ʱ��
    char    szServerId[ID_MAXLEN];     //�豸ID
}BreathAlarmInfo2, *BREATHALARMINFO2;
typedef std::vector<BreathAlarmInfo2> BreathAlarmInfoArray2;
typedef BreathAlarmInfoArray::iterator BreathAlarmInfoArray_it2;

/**
 *�����ļ���Ϣ
 */
typedef struct tagDataInfo
{
    WORD    wType;         //����
    short   shFlag;         //������
    DWORD   dwUploadtime;    //�ϴ�ʱ��
    char    szFileName[ID_MAXLEN];     //�ļ���
    char    szZhdXlh[ID_MAXLEN];       //�豸ID
}DataInfo, *DATAINFO;
typedef std::vector<DataInfo> DataInfoArray;
typedef DataInfoArray::iterator DataInfoArray_it;

/*��ҳ�б� */
typedef struct tagPageList
{
    WORD    wAllPageNum;    //��ҳ��
    WORD    wPageNum;       //����ĵڼ�ҳ
    WORD    wListNum;       //�����ÿҳ����
}PageList, *LPPageList;

/**
 *openid
 */
typedef struct tagOpenidInfo
{
    char    szOpenid[ID_MAXLEN];         //openid
    char    szServerid[ID_MAXLEN];       //�豸ID
    char    szPassword[USER_PWD_MAXLEN];
    WORD    ushUserType;
}OpenidInfo, *OPENIDINFO;
typedef std::vector<OpenidInfo> OpenidInfoArray;
typedef OpenidInfoArray::iterator OpenidInfoArray_it;

/**
 *�쳣ͳ����Ϣ
 */
typedef struct tagAbnormalStat
{
    char    szServerid[ID_MAXLEN];  //�豸id
    WORD    shNum;                  //��numСʱ/���/����
    WORD    shtype;                 //��ʾ����
    DWORD   Pin;                    //����Ƶ��
    DWORD   dwtime;                 //ѡ��ʱ��
    DWORD   dwSleepBTime;           //˯�߿�ʼʱ��
    DWORD   dwSleepETime;           //˯�߽���ʱ��
    DWORD   dwDi;                   //��ͨ��
    DWORD   dwZan;                  //������ͣ
    DWORD   AHI;
    DWORD   dwBi;                   //��˯����
}AbnormalStat, *ABNORMALSTAT;
typedef std::vector<AbnormalStat> AbnormalStatArray;
typedef AbnormalStatArray::iterator AbnormalStatArray_it;

/**
 *�����¼�
 */
typedef struct tagBreathEntRptInfo
{
    char    szZDxlh[ID_MAXLEN];     //�豸id
    WORD    level;          //���س̶�
    WORD    SymptomType;    //������ͣ����
    DWORD   dwSleepBTime;           //˯�߿�ʼʱ��
    DWORD   dwSleepETime;           //˯�߽���ʱ��
    WORD    wType;         //����
    WORD    wNum;         //����
    WORD    shBaifenbi;     //�ٷֱ�
    DWORD    shLiejishijian; //�ۼ�ʱ��
    WORD    shZuichang;     //����ʱ��
    DWORD    Fashengshijian;     //�����ʱ��
}BreathEntRptInfo, *BREATHENTRPTINFO;
typedef std::vector<BreathEntRptInfo> BreathEntRptInfoArray;
typedef BreathEntRptInfoArray::iterator BreathEntRptInfoArray_it;

typedef struct xlh
{
    char szZD_xlh[ID_MAXLEN];       //�豸���к�
}XLH;
typedef std::vector<XLH> FileDirArray;

//��ȡ״̬�ļ��б�
typedef struct statfilepara
{
    char    ZhDXlh[ID_MAXLEN];       //�豸���к�
    DWORD   dwTime;
    WORD    wNum;
} SFpara, *SFPARA;

/**
 *ʱ��,���кŲ���
 */
typedef struct tagTimeXlh
{
    char    szZD_xlh[ID_MAXLEN];
    DWORD   shGstime;
}TimeXlh;
typedef std::vector<TimeXlh> TimeXlhArr;

/**
 *����ʱ��
 */
typedef struct tagGsTime
{
    char    szZD_xlh[ID_MAXLEN];
    WORD   shRecomTime;
    WORD   shOwnTime;
}GsTime;
typedef std::vector<GsTime> GsTimeArr;

/**
 *���
*/
typedef struct tagZhenDuan
{
    char    level;          //���س̶�
    short   SymptomType;    //������ͣ����
}ZhenDuan;
typedef std::vector<ZhenDuan> ZhenDuanArray;

/**
 *��Ϣ
 */
typedef struct tagProc
{
    DWORD   dwAlarmTime;    //����ʱ��
    char   lpMassage[1024];   //������Ϣ
}Proc, *PROC;
typedef std::vector<Proc> ProcArray;
typedef ProcArray::iterator ProcArray_it;

typedef struct tagSleep
{
    DWORD  dwTime;         //״̬����ʱ��
    DWORD  dwBreathRate;
    DWORD  dwAHIS;
    DWORD  dwAHIO;
    WORD  wSleepStat1;    //״̬
    WORD  wSleepStat2;    //״̬
    DWORD dwRespRate;       //������
    DWORD dwHeartRate;      //������
}Sleep, *SLEEP;
typedef std::vector<Sleep> SleepArray;
typedef SleepArray::iterator SleepArray_it;

typedef struct tagShiMian
{
    char    szZhD_xlh[ID_MAXLEN];   //�ն����к�
    DWORD   dwflagdate;     //��������
    DWORD   dwStTime;   //��ʼʱ��
    DWORD   dwEndTime;  //����ʱ��
    DWORD   dwDelayTime;    //˯���ӳ�
    DWORD   dwAllSleep;     //��˯��ʱ��
    DWORD   dwInBedTime;    //�Դ�ʱ��
    DWORD   dwWASO;     //��˯������
    DWORD   dwNA;       //���Ѵ���
    DWORD   dwratio_of_sleep;
    DWORD   dwAHIS;     //��ͨ��
    DWORD   dwAHIO;
}ShiMian;
typedef std::vector<ShiMian> SMArray;

typedef struct tagHX
{
    char    szZhD_xlh[ID_MAXLEN];   //�ն����к�
    DWORD   dwLen;
    DWORD   dwTime;         //״̬����ʱ��
    DWORD   dwRespRate;       //������
    DWORD   dwHeartRate;      //������
    BYTE*   lpMassage;
}HX, *PHX;
typedef std::vector<HX> HXArray;
typedef HXArray::iterator HXArray_it;

#endif //__EDUDBRES_H__
