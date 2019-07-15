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
#define ID_MAXLEN			127     //ID长度
#define USER_PWD_MAXLEN		17      //用户密码长度
#define ALARM_MASSAGE_LEN   1200    //报警信息长度

/**
 *呼吸检测终端基本信息
 */
typedef struct tagZhDnInfo
{
    char    szXlh[ID_MAXLEN];      //序列号
    char    szName[ID_MAXLEN];     //昵称
    DWORD   dwindex;
    DWORD   dwType;                //类型
    DWORD   dwMfgTime;             //出厂日期
    DWORD   dwRegTime;             //第一次启用时间
}ZhDInfo, *LPZDINfO;
typedef std::vector<ZhDInfo> ZDInfoArray;

/**
 *呼吸检测终端报警信息
 */
typedef struct tagBreathAlarmInfo
{
    WORD    wAlarmType;     //报警类型
    WORD    wMessageLen;    //消息长度
    WORD    shAlarmdur;     //报警持续时间
    DWORD   dwAlarmTime;    //报警时间
    DWORD   dwStartTime;    //起始时间
    DWORD   dwLimitTime;    //截止时间
    char    szServerId[ID_MAXLEN];     //设备ID
    BYTE*   lpMassage;   //报警信息
}BreathAlarmInfo, *BREATHALARMINFO;
typedef std::vector<BreathAlarmInfo> BreathAlarmInfoArray;
typedef BreathAlarmInfoArray::iterator BreathAlarmInfoArray_it;

/**
 *呼吸检测终端报警信息
 */
typedef struct tagBreathAlarmInfo2
{
    WORD    wAlarmType;     //报警类型
    WORD    wAlarmParam;     //类型解释
    DWORD   dwStartTime;    //起始时间
    DWORD   dwLimitTime;    //截止时间
    DWORD   dwAlarmTime;    //报警时间
    char    szServerId[ID_MAXLEN];     //设备ID
}BreathAlarmInfo2, *BREATHALARMINFO2;
typedef std::vector<BreathAlarmInfo2> BreathAlarmInfoArray2;
typedef BreathAlarmInfoArray::iterator BreathAlarmInfoArray_it2;

/**
 *数据文件信息
 */
typedef struct tagDataInfo
{
    WORD    wType;         //类型
    short   shFlag;         //处理标记
    DWORD   dwUploadtime;    //上传时刻
    char    szFileName[ID_MAXLEN];     //文件名
    char    szZhdXlh[ID_MAXLEN];       //设备ID
}DataInfo, *DATAINFO;
typedef std::vector<DataInfo> DataInfoArray;
typedef DataInfoArray::iterator DataInfoArray_it;

/*分页列表 */
typedef struct tagPageList
{
    WORD    wAllPageNum;    //总页数
    WORD    wPageNum;       //请求的第几页
    WORD    wListNum;       //请求的每页个数
}PageList, *LPPageList;

/**
 *openid
 */
typedef struct tagOpenidInfo
{
    char    szOpenid[ID_MAXLEN];         //openid
    char    szServerid[ID_MAXLEN];       //设备ID
    char    szPassword[USER_PWD_MAXLEN];
    WORD    ushUserType;
}OpenidInfo, *OPENIDINFO;
typedef std::vector<OpenidInfo> OpenidInfoArray;
typedef OpenidInfoArray::iterator OpenidInfoArray_it;

/**
 *异常统计信息
 */
typedef struct tagAbnormalStat
{
    char    szServerid[ID_MAXLEN];  //设备id
    WORD    shNum;                  //第num小时/礼拜/月日
    WORD    shtype;                 //显示类型
    DWORD   Pin;                    //呼吸频率
    DWORD   dwtime;                 //选择时间
    DWORD   dwSleepBTime;           //睡眠开始时间
    DWORD   dwSleepETime;           //睡眠结束时间
    DWORD   dwDi;                   //低通量
    DWORD   dwZan;                  //呼吸暂停
    DWORD   AHI;
    DWORD   dwBi;                   //深睡比例
}AbnormalStat, *ABNORMALSTAT;
typedef std::vector<AbnormalStat> AbnormalStatArray;
typedef AbnormalStatArray::iterator AbnormalStatArray_it;

/**
 *呼吸事件
 */
typedef struct tagBreathEntRptInfo
{
    char    szZDxlh[ID_MAXLEN];     //设备id
    WORD    level;          //严重程度
    WORD    SymptomType;    //呼吸暂停类型
    DWORD   dwSleepBTime;           //睡眠开始时间
    DWORD   dwSleepETime;           //睡眠结束时间
    WORD    wType;         //类型
    WORD    wNum;         //次数
    WORD    shBaifenbi;     //百分比
    DWORD    shLiejishijian; //累计时间
    WORD    shZuichang;     //持续时间
    DWORD    Fashengshijian;     //最长发生时间
}BreathEntRptInfo, *BREATHENTRPTINFO;
typedef std::vector<BreathEntRptInfo> BreathEntRptInfoArray;
typedef BreathEntRptInfoArray::iterator BreathEntRptInfoArray_it;

typedef struct xlh
{
    char szZD_xlh[ID_MAXLEN];       //设备序列号
}XLH;
typedef std::vector<XLH> FileDirArray;

//获取状态文件列表
typedef struct statfilepara
{
    char    ZhDXlh[ID_MAXLEN];       //设备序列号
    DWORD   dwTime;
    WORD    wNum;
} SFpara, *SFPARA;

/**
 *时间,序列号参数
 */
typedef struct tagTimeXlh
{
    char    szZD_xlh[ID_MAXLEN];
    DWORD   shGstime;
}TimeXlh;
typedef std::vector<TimeXlh> TimeXlhArr;

/**
 *干涉时间
 */
typedef struct tagGsTime
{
    char    szZD_xlh[ID_MAXLEN];
    WORD   shRecomTime;
    WORD   shOwnTime;
}GsTime;
typedef std::vector<GsTime> GsTimeArr;

/**
 *诊断
*/
typedef struct tagZhenDuan
{
    char    level;          //严重程度
    short   SymptomType;    //呼吸暂停类型
}ZhenDuan;
typedef std::vector<ZhenDuan> ZhenDuanArray;

/**
 *信息
 */
typedef struct tagProc
{
    DWORD   dwAlarmTime;    //报警时间
    char   lpMassage[1024];   //报警信息
}Proc, *PROC;
typedef std::vector<Proc> ProcArray;
typedef ProcArray::iterator ProcArray_it;

typedef struct tagSleep
{
    DWORD  dwTime;         //状态所在时刻
    DWORD  dwBreathRate;
    DWORD  dwAHIS;
    DWORD  dwAHIO;
    WORD  wSleepStat1;    //状态
    WORD  wSleepStat2;    //状态
    DWORD dwRespRate;       //呼吸率
    DWORD dwHeartRate;      //心跳率
}Sleep, *SLEEP;
typedef std::vector<Sleep> SleepArray;
typedef SleepArray::iterator SleepArray_it;

typedef struct tagShiMian
{
    char    szZhD_xlh[ID_MAXLEN];   //终端序列号
    DWORD   dwflagdate;     //所在日期
    DWORD   dwStTime;   //开始时间
    DWORD   dwEndTime;  //结束时间
    DWORD   dwDelayTime;    //睡眠延迟
    DWORD   dwAllSleep;     //总睡眠时间
    DWORD   dwInBedTime;    //卧床时间
    DWORD   dwWASO;     //入睡后醒来
    DWORD   dwNA;       //觉醒次数
    DWORD   dwratio_of_sleep;
    DWORD   dwAHIS;     //低通量
    DWORD   dwAHIO;
}ShiMian;
typedef std::vector<ShiMian> SMArray;

typedef struct tagHX
{
    char    szZhD_xlh[ID_MAXLEN];   //终端序列号
    DWORD   dwLen;
    DWORD   dwTime;         //状态所在时刻
    DWORD   dwRespRate;       //呼吸率
    DWORD   dwHeartRate;      //心跳率
    BYTE*   lpMassage;
}HX, *PHX;
typedef std::vector<HX> HXArray;
typedef HXArray::iterator HXArray_it;

#endif //__EDUDBRES_H__
