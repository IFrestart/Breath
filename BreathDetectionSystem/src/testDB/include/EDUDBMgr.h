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
#include <time.h>   //���ڲ������������
#include "DArrStack.h"	//��̬����ջ
#include "Classify.h"
//////////////////////////////////////////////////////////////////////////////////////

//rows SQL����ѯ���ؽ��
#define RowsInit    -1      //��ʼ��
#define RowsErr     -1      //���ش���
#define RowsNot     0       //�ս��

//��С�ļ���С
#define		FILE_MINLEN     52
//һ�������ݳ���
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
    * ֻ��3�������
    * EDUDB_ERR_DBCONN_LOST   ʧȥ�����ݿ������
    * EDUDB_ERR_DBCONN_OK     ���ӳɹ�
    * EDUDB_ERR_DBCONN_INIT   �������ڳ�ʼ��
    */
    int m_nDBStatus;    //�������ݿ�״̬

    CDBConfigClass *m_pDBConfigClass;
    CMMTDBTrace *m_pDBTrace;
    char m_chChCode[52 + 1];    //��ĸ����
    char m_chNumCode[10 + 1];   //���ֱ���

    /*����SQLʱʹ��*/
    std::ostringstream oss;

private:
    /*
     * ������ݿ�ID
     * @return  >=0	���õ�DBIndexID; EDUDB_ERR_DBCONN_LOST ���������Ѿ��Ͽ�; EDUDB_ERR_DBCONN_BUSY	�������Ӷ�æµ��ָÿ�����Ӷ���EDUDB_CONST_LOCK_MAXCOUNT������ȴ���
     */
    int GetDBIndexID (short shType = 0);
    int GetrealEscapeString (char* arr);

public:

    friend class CLockSession;  //DB�Ự�Զ��ͷ���d

    void SetDBConfigClass (CDBConfigClass * pDBConfigClass);
    void SetTraceType (int nMsgMod, int nMsgTrace);

    /**
     * ���ӵ����ݿ�
     * @return Success, EDUDB_CONST_CONN_LOST: Failure
     */
    int ConnectToDB (int nIdx);

    /**
     * Check database version
     **/
//    int CheckDBVersion(int nDBIndex);

    /**
     * ������ݿ�����
     * @param nDBStatus EDUDB_ERR_DBCONN_LOST ��EDUDB_ERROR_DBCONN_OK��EDUDB_ERROR_DBCONN_INIT
     * @return EDUDB_ERR_DBCONN_LOST ��EDUDB_ERROR_DBCONN_OK��EDUDB_ERROR_DBCONN_INIT
     */
    int FreeAll (int nDBStatus = EDUDB_ERR_DBCONN_LOST);

    /**
     * ������ݿ�����
     * @param nDBIndexID Ҫ�������ݿ����Ӻ�
     * @return ���� EDUDB_ERR_DBCONN_OK ; ������EDUDB_ERR_DBCONN_LOST
     */
    int CheckConnection (int nDBIndexID = -1);

    /**
     * ��ʱ��
     */
    int DBTimerTasker ();
    unsigned int getMorningTime();
    int getSETime(TimeXlh &stru, ShiMian &SMstru);
    //��ȡ˯����Ϣ����
    int getSleepStatInfo(ShiMian &stru, SleepArray &Array);
    //�������
    int insetSleepStat(ShiMian stru);
    int SetSleepTime(TimeXlh &stru,char *cxlh);
/************************************************************************************/
public:
//*��֤�������********************************/
    //����ն��Ƿ����
    int CheckZhongDuan(char* ZhDXlh);

    //�ն��豸���
    int AddTerminalInfo(ZhDInfo &stru);
    //�����ն��豸
    int DelTerminalInfo(ZhDInfo &stru);
    int GetTerminalList(PageList &List, FileDirArray &Array);

    //��һ�������ն��豸
    int firstStartZD(time_t starttime);

    //�鿴open_id�Ƿ����ն��Ѱ�
    int CheckBind(OpenidInfo stru);
    //��ȡopenid���豸�б�
    int GetBandZDList(OpenidInfo &stru, OpenidInfoArray &Array);
    //���豸��΢�Ź��ں��ã�
    //int BindBreathTerminal(OpenidInfo &stru);
    int BindBreath(OpenidInfo &stru);
    //���open_id�󶨵��豸
    int DelBind(OpenidInfo &stru);

//����������Ϣ begin*******************************************************************
//GetInfo
    //��ȡ������Ϣ
    int GetBreathAlarmInfo(PageList &List, BreathAlarmInfo &stru, BreathAlarmInfoArray &Array);
    //��ȡ��������ͼ
    int GetAlarmInfoData(BreathAlarmInfo &stru);
    //��ȡ������Ϣ2
    int GetAlarmInfo2(PageList &List,BreathAlarmInfo2 &stru, BreathAlarmInfoArray2 &Array);
    //��ȡ��������ļ�
//    int GetDataInfo (PageList &List, DataInfo &stru, DataInfoArray &Array);
//SetInfo
    //��ӱ�����Ϣ
    int AddBreathAlarmInfo(BreathAlarmInfo &stru);
    //��ӱ�����Ϣ2
    int AddSpaceStat(BreathAlarmInfo2 &stru);
    //ɾ��������Ϣ
    int DelBreathAlarmInfo(BreathAlarmInfo &stru);
    //��Ӽ����������
    int AddDataInfo(DataInfo &stru);
//����ʱ�����
    //��ȡ����ʱ�����
    int GetIpeTime(GsTime &stru);
    //���ø���ʱ�����
    int SetIpeTime(GsTime &stru);
//����������Ϣ end*******************************************************************


    //��ȡ�쳣˯��ͳ��
    int IfExistAbnormal(AbnormalStat &stru);
    int UpdateAbnormal(AbnormalStat &stru);
    int GetAbnormal(AbnormalStat &stru, AbnormalStatArray &Array);

    //���Ժ����¼�����
    int tryBreathEntRpt(BreathEntRptInfo &stru, BreathEntRptInfoArray &Array);

    //��ȡδ�����ļ�Ŀ¼
    int getUnprocDir(FileDirArray &Array);
    //��ȡ״̬�ļ��б�
    int getStatFilelist(SFpara &stru,FileDirArray &Array);
    //������ƽ�������õ���״̬���
    int inStat_BaseSmoothPara(const char *xlh, DWORD num, SleepArray &Array, BreathAlarmInfo2 &SETime);

    int GetSleepInfo(TimeXlh &stru, SleepArray &Array);
    int GetBRInfo(TimeXlh &stru, SleepArray &Array);

    //��ȡ�����ն��豸���к�
    int GetAllZhDxlh(FileDirArray &Array);

    int GetInsomInfo(ShiMian &shim);
//******************״̬�ļ����*********************
    int AddHX(HX &stru);
    int GetHX(HX &stru, PageList &plist, HXArray &Array);

};
/**
* DB�Ự�Զ��ͷ���
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


