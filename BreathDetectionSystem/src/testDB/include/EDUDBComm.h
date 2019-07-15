/**
 * Copyright (c) 2015, HangZhou Webcon Corporation.

 * @file     EDUDBComm.h
 * @author   shizc <shizc@webcon.com.cn>
 * @short
 * @date     07/2015
 **/
#ifndef __EDUDBCOMM_H__
#define __EDUDBCOMM_H__

#include <stdlib.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>

#include <pthread.h>
#include "EDUDBMgr.h"
#include "Common.h"
#include "TypeDef.h"

//#include "ByteStream.h"
#include "Timer.h"

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>

#define MAX_OUTBUF_SIZE 1024*8
#define TIMERTODB 1000*60
#define DELETEVERIFYCODE 1000*60*60

using namespace std;
//////////////////////////////////////////////////////////////////////////////////////
class CEDUDBComm:public ITimerEventCallBack
{
public:
    CEDUDBComm ();
    virtual ~ CEDUDBComm ();

public:
    /**
    * 初始化
    * @return	初始化成功 EDUDB_ERR_DBCONN_LOST;初始化失败 DB_ERR_SUCCESS
    */
    int Initialize ();

    /**
    * 检查数据库连接正常与否
    */
    void CheckConnection ();

    /**
     * 定时器间隔到时的回调函数
     * @param timerID 自己定义的类型
     * @param arg  要带的参数
     * @return	非零 表示退出计时器; 0 表示返回
    */
    virtual int OnTimer (DWORD timerID, void *arg);

    /**
    * 数据库定时例行过程
    */
    void DBTimerTasker ();

    /**
    * 设置打印信息模式和类型
    * @param nMsgMod
    * @param nMsgTrace
    * @return
    */
    void SetTraceType (int nMsgMod, int nMsgTrace);

    /**
    * 读取配置文件，将配置文件中的合法配置项（由组名、参数名、参数值构成）的相关值保存在类的成员变量中，并提供读取这些配置项相关值的接口。
    * @param pDBConfigClass CDBConfigClass的一实例
    * @return
    */
    void SetDBConfigClass (CDBConfigClass *pDBConfigClass);

public:
    /**
    * @param inbuf PDU拆包前接收到数据的缓冲区
    * @param inlen PDU拆包前接收到数据缓冲区的长度
    * @param outbuf PDU打包后的数据缓冲区
    * @param outlen PDU打包后的数据缓冲区长度
    */

//* 公众号绑定*****************************************************************************************/
    //检测openid是否存在
    //int OnCheckOpenId(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取openid绑定卡号列表
    int OnGetBandZDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //绑定设备（微信公众号用）
    int OnBindBreath(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //解除openid绑定的设备
    int OnDelOpenidBind(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取绑定设备列表
    int OnGetBandZhDList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
//**报警功能*******************************************************************************************/
    //获取报警信息
    int OnGetBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取报警信息2
    int OnGetAlarmInfo2(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取报警波形图
    int OnGetAlarmInfoData(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //添加报警信息
    int OnAddBreathAlarmInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //添加报警信息2
//    int OnAddSpaceStat(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取干涉时间参数
    int OnGetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //设置干涉时间参数
    int OnSetIpeTime(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

//**异常统计*******************************************************************************************/
    //添加检测数据索引
    int OnAddDataInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取检测数据文件
//    int OnGetDataInfo (CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //获取异常睡眠统计
    int OnGetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //导入统计结果
    int OnSetAbnormal(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //测试呼吸事件报告
    int OntryBreathEntRpt(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

//***************************************************************************************************/    
    //获取未处理文件目录
    int OngetUnprocDir(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //获取状态文件列表
    int OngetStatFilelist(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //入库根据平滑参数得到的状态结果
    int OninStat_BaseSmoothPara(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnGetSleepInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //获取呼吸和心率
    int OnGetBRInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnGetInsomInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    int OnAddHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    int OnGetHX(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

    //终端设备入库
    int OnAddTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    //销毁终端设备
    int OnDelTerminalInfo(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);
    int OnGetTerminalList(CVOID *inbuf, DWORD inlen, void *&outbuf,DWORD &outlen);

private:
    CEDUDBMgr *m_pEDUDBMgr;
    CDBConfigClass *m_pDBConfigClass;
    CMMTDBTrace *m_pDBTrace;
    CTimer m_pTimer;
    CTimer m_pTimer2;
    std::ostringstream oss;
};


#endif //__EDUDBCOMM_H__

