/**
  * Copyright (c) 2016, HangZhou Webcon Centeroration.
  *
  * @file     EDUDBMgr.h
  * @author   shizc <shizc@webcon.com.cn>
  * @short
  * @date     05/2016
  **/

#ifndef __EDUDBMGR_H__
#define __EDUDBMGR_H__
#include <stdlib.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <pthread.h>

/**
 * For pthread_mutex_trylock() error message
 */

#include <errno.h>

#include <vector>
#include <stack>
#include "MyDB.h"
#include "PDU.h"
#include "EDUDBRes.h"
#include "DBConfigClass.h"
#include "MMTDBModuleTrace.h"
#include "IdDef.h"
#include <sstream>
#include <map>
#include <time.h>   //用于产生随机数种子
#include "DArrStack.h"	//动态数组栈
#include "Classify.h"
//////////////////////////////////////////////////////////////////////////////////////

//rows SQL语句查询返回结果
#define RowsInit    -1      //初始化
#define RowsErr     -1      //返回错误
#define RowsNot     0       //空结果

//最小文件大小
#define		FILE_MINLEN     52
//一分钟内容长度
#define		ONEMIN_LEN		52


using namespace std;


class CEDUDBMgr
{
public:
    CEDUDBMgr (CDBConfigClass * pDBConfigClass);
    virtual ~ CEDUDBMgr ();

private:
    DBConfigSvrInfoOfDB m_DBInfo;
    int m_nDBIdxID;
    CMyDB *m_pDBIdx[EDUDB_CONST_CONN_NUM];

    /**
    * 只有3种情况：
    * EDUDB_ERR_DBCONN_LOST   失去和数据库的连接
    * EDUDB_ERR_DBCONN_OK     连接成功
    * EDUDB_ERR_DBCONN_INIT   连接正在初始化
    */
    int m_nDBStatus;    //连接数据库状态

    CDBConfigClass *m_pDBConfigClass;
    CMMTDBTrace *m_pDBTrace;
    char m_chChCode[52 + 1];    //字母编码
    char m_chNumCode[10 + 1];   //数字编码

    /*构造SQL时使用*/
    std::ostringstream oss;

private:
    /*
     * 获得数据库ID
     * @return  >=0	可用的DBIndexID; EDUDB_ERR_DBCONN_LOST 所有连接已经断开; EDUDB_ERR_DBCONN_BUSY	所有连接都忙碌（指每个连接都有EDUDB_CONST_LOCK_MAXCOUNT个命令等待）
     */
    int GetDBIndexID (short shType = 0);
    int GetrealEscapeString (char* arr);

public:

    friend class CLockSession;  //DB会话自动释放锁d

    void SetDBConfigClass (CDBConfigClass * pDBConfigClass);
    void SetTraceType (int nMsgMod, int nMsgTrace);

    /**
     * 连接到数据库
     * @return Success, EDUDB_CONST_CONN_LOST: Failure
     */
    int ConnectToDB (int nIdx);

    /**
     * Check database version
     **/
//    int CheckDBVersion(int nDBIndex);

    /**
     * 清除数据库连接
     * @param nDBStatus EDUDB_ERR_DBCONN_LOST 或EDUDB_ERROR_DBCONN_OK或EDUDB_ERROR_DBCONN_INIT
     * @return EDUDB_ERR_DBCONN_LOST 或EDUDB_ERROR_DBCONN_OK或EDUDB_ERROR_DBCONN_INIT
     */
    int FreeAll (int nDBStatus = EDUDB_ERR_DBCONN_LOST);

    /**
     * 检查数据库连接
     * @param nDBIndexID 要检查的数据库连接号
     * @return 正常 EDUDB_ERR_DBCONN_OK ; 非正常EDUDB_ERR_DBCONN_LOST
     */
    int CheckConnection (int nDBIndexID = -1);

    /**
     * 定时器
     */
    int DBTimerTasker ();
    unsigned int getMorningTime();
    int getSETime(TimeXlh &stru, ShiMian &SMstru);
    //获取睡眠信息数据
    int getSleepStatInfo(ShiMian &stru, SleepArray &Array);
    //保存入库
    int insetSleepStat(ShiMian stru);
    int SetSleepTime(TimeXlh &stru,char *cxlh);
/************************************************************************************/
public:
//*认证功能相关********************************/
    //检测终端是否存在
    int CheckZhongDuan(char* ZhDXlh);

    //终端设备入库
    int AddTerminalInfo(ZhDInfo &stru);
    //销毁终端设备
    int DelTerminalInfo(ZhDInfo &stru);
    int GetTerminalList(PageList &List, FileDirArray &Array);

    //第一次启用终端设备
    int firstStartZD(time_t starttime);

    //查看open_id是否与终端已绑定
    int CheckBind(OpenidInfo stru);
    //获取openid绑定设备列表
    int GetBandZDList(OpenidInfo &stru, OpenidInfoArray &Array);
    //绑定设备（微信公众号用）
    //int BindBreathTerminal(OpenidInfo &stru);
    int BindBreath(OpenidInfo &stru);
    //解除open_id绑定的设备
    int DelBind(OpenidInfo &stru);

//报警与检测信息 begin*******************************************************************
//GetInfo
    //获取报警信息
    int GetBreathAlarmInfo(PageList &List, BreathAlarmInfo &stru, BreathAlarmInfoArray &Array);
    //获取报警波形图
    int GetAlarmInfoData(BreathAlarmInfo &stru);
    //获取报警信息2
    int GetAlarmInfo2(PageList &List,BreathAlarmInfo2 &stru, BreathAlarmInfoArray2 &Array);
    //获取检测数据文件
//    int GetDataInfo (PageList &List, DataInfo &stru, DataInfoArray &Array);
//SetInfo
    //添加报警信息
    int AddBreathAlarmInfo(BreathAlarmInfo &stru);
    //添加报警信息2
    int AddSpaceStat(BreathAlarmInfo2 &stru);
    //删除报警信息
    int DelBreathAlarmInfo(BreathAlarmInfo &stru);
    //添加检测数据索引
    int AddDataInfo(DataInfo &stru);
//干涉时间参数
    //获取干涉时间参数
    int GetIpeTime(GsTime &stru);
    //设置干涉时间参数
    int SetIpeTime(GsTime &stru);
//报警与检测信息 end*******************************************************************


    //获取异常睡眠统计
    int IfExistAbnormal(AbnormalStat &stru);
    int UpdateAbnormal(AbnormalStat &stru);
    int GetAbnormal(AbnormalStat &stru, AbnormalStatArray &Array);

    //测试呼吸事件报告
    int tryBreathEntRpt(BreathEntRptInfo &stru, BreathEntRptInfoArray &Array);

    //获取未处理文件目录
    int getUnprocDir(FileDirArray &Array);
    //获取状态文件列表
    int getStatFilelist(SFpara &stru,FileDirArray &Array);
    //入库根据平滑参数得到的状态结果
    int inStat_BaseSmoothPara(const char *xlh, DWORD num, SleepArray &Array, BreathAlarmInfo2 &SETime);

    int GetSleepInfo(TimeXlh &stru, SleepArray &Array);
    int GetBRInfo(TimeXlh &stru, SleepArray &Array);

    //获取所有终端设备序列号
    int GetAllZhDxlh(FileDirArray &Array);

    int GetInsomInfo(ShiMian &shim);
//******************状态文件相关*********************
    int AddHX(HX &stru);
    int GetHX(HX &stru, PageList &plist, HXArray &Array);

};
/**
* DB会话自动释放锁
* Usage:
CLockSession* cls = new CLockSession(this);
if(!cls.success()){
    return;
}
....
delete cls;
*/
class CLockSession
{

public:

    CLockSession(CEDUDBMgr* mgr):
        m_mgr(NULL),
        status(-1),
        m_dbidx(-1)
    {

        this->m_mgr = mgr;

        std::cout << "CLockSession constructor..."<< std::endl;

        int nDBIndex = mgr->GetDBIndexID ();
        std::cout << "get : nDBIndex=" << nDBIndex << std::endl;
        if (nDBIndex < 0)
        {
            std::cout << "Error : GetDBIndexID=" << nDBIndex << std::endl;
        }
        else
        {
            status = mgr->m_pDBIdx[nDBIndex]->ThreadLock();
            std::cout << "get : status=" << status << std::endl;

            if (status == 0)
            {
                m_dbidx = nDBIndex;
            }
            else
            {
                std::cout << "Error : ThreadLock :" << nDBIndex << std::endl;
            }
        }
    }

    ~CLockSession(){
        std::cout << "CLockSession destructor..." << std::endl;
        std::cout << "status=" << status <<" m_dbidx=" << m_dbidx << std::endl;

        if( (status == 0) && (m_dbidx != -1) ){
            std::cout << "Release...." << m_dbidx << std::endl;
            m_mgr->m_pDBIdx[m_dbidx]->ThreadUnlock();
        }
    }

    bool success(){return status>=0;}

    bool fail(){return status<0;}

    int GetDBIndex(){return m_dbidx;}

private:
    CEDUDBMgr* m_mgr;
    int status;
    int m_dbidx;
};

#endif //__EDUDBMGR_H__


