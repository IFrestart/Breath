#include "PDU.h"
//Sub PDU type define ------------------------------------------

#define USERNAME_TYPE 1     //����û�����

//End Sub PDU type define

namespace SubPDU
{
/*================================================================*/
/* Segment */
/*================================================================*/
const unsigned short
ServerClientBase1	= 0,		/**< �ͻ���������֮��ı���PDUֵ��Χ	(00000~09999)*/
ServerServerBase1	= 10000,	/**< �����������֮��ı���PDUֵ��Χ	(10000~19999)*/
ServerDBCommBase1	= 20000,	/**< ���ݿ��������֮��ı���PDUֵ��Χ (20000~30000)*/
ClientClientBase1	= 30000;

/** Begin */
enum {

    /*================================================================*/
    /* Server <-> Client */
    /*================================================================*/
    /** Cert <-> Client */
    ClientBase				= ServerClientBase1/*0*/ + 0,

    AddUser_Req_WebToDB         =68,
    AddUser_Rsp_DBToWeb         =69,

    //���openid
    AddOpenId_Req_WebToDB       =70,
    AddOpenId_Rsp_DBToWeb       =71,

    //��ȡ���ն��б�
    GetBandZDList_Req_WebToDB     =72,
    GetBandZDList_Rsp_DBToWeb     =73,

    //openid���豸��΢�Ź��ں��ã�
    BindOpenid_Req_WebToDB        =74,
    BindOpenid_Rsp_DBToWeb        =75,

    //���openid���豸��΢�Ź��ں��ã�
    DelBindOpenid_Req_WebToDB       =76,
    DelBindOpenid_Rsp_DBToWeb       =77,

    //��ȡ״̬�ļ��б�
    getStatFilelist_REQ_TODBCOMM        =78,
    getStatFilelist_RSP_FROMDBCOMM      =79,

    //������ƽ�������õ���״̬���
    inStat_BaseSmoothPara_REQ_TODBCOMM          =80,
    inStat_BaseSmoothPara_RSP_FROMDBCOMM        =81,

    //��ȡδ�����ļ�Ŀ¼
    getUnprocDir_REQ_TODBCOMM           =82,
    getUnprocDir_RSP_FROMDBCOMM         =83,

    //��ȡ���ն��б�
    GetBandZhDList_Req_WebToDB          =84,
    GetBandZhDList_Rsp_DBToWeb          =85,

    //�ն��豸���
    AddTerminalInfo_REQ_FromWeb         =86,
    AddTerminalInfo_RSP_ToWeb           =87,

    //�����ն��豸
    DelTerminalInfo_REQ_FromWeb         =90,
    DelTerminalInfo_RSP_ToWeb           =91,

    //�鿴��������豸
    GetTerminalList_REQ_FromWeb         =92,
    GetTerminalList_RSP_ToWeb           =93,

    //��ӱ�����Ϣ
    DBSUB_AddAlarmInfo_Req_FromNHSystem =94,
    DBSUB_AddAlarmInfo_Rsp_ToNHSystem   =95,

    /*======�ϴ��ļ�ģ��===============================================*/
    //��Ӽ����������
    DBSUB_AddDataInfo_Req_FromNHSystem  =96,
    DBSUB_AddDataInfo_Rsp_ToNHSystem    =97,

    //��ӱ�����Ϣ2
    AddAlarmInfo2_Req_FromNHSystem      =98,
    AddAlarmInfo2_Rsp_ToNHSystem        =99,

    //��ȡ����豸������Ϣ
    GetAlarmInfoListPage_Req_FromCli    =114,
    GetAlarmInfoListPage_Rsp_ToCli      =115,

    //��ȡ�������
    GetDataInfoListPage_Req_FromCli     =116,
    GetDataInfoListPage_Rsp_ToCli       =117,

    //��ȡ��������
    GetAlarmInfoData_Req_FromCli        =118,
    GetAlarmInfoData_Rsp_ToCli          =119,

    //�鿴�����¼�
    GetBreathInfo_Req_WebToDB           =120,
    GetBreathInfo_Rsp_DBToWeb           =121,

    //���Ժ����¼�
    tryBreathInfo_Req_WebToDB           =122,
    tryBreathInfo_Rsp_DBToWeb           =123,

    //��ȡ����ʱ��
    GetIpeTime_REQ_FromCli              =124,
    GetIpeTime_RSP_ToCli                =125,

    //���ø���ʱ��
    SetIpeTime_REQ_FromWeb              =126,
    SetIpeTime_RSP_ToDB                 =127,

    //��ȡ������Ϣ2
    GetAlarmInfoListPage2_Req_FromCli   =130,
    GetAlarmInfoListPage2_Rsp_ToCli     =131,

    //
    GetSleepInfo_REQ_FromCli            =140,
    GetSleepInfo_RSP_ToCli              =141,

    //��ȡ����������
    GetBRInfo_REQ_FromCli            =160,
    GetBRInfo_RSP_ToCli              =161,

    //��ȡʧ����Ϣ
    GetInsomInfo_REQ_FromCli         =162,
    GetInsomInfo_RSP_ToCli           =163,

    //��ȡ�쳣ͳ��
    Abnormal_statistics_Req_FromCLI =203,
    Abnormal_statistics_Rsp_ToCLI   =204,

    //��������ʺͺ�����
    AddHX_Req_WebToDB               =1101,
    AddHX_Rsp_DBToWeb               =1102,

    //�鿴�����ʺͺ�����
    getHX_Req_WebToDB               =1103,
    getHX_Rsp_DBToWeb               =1104,
    /*================================================================*/
    /* Server <-> DBComm */
    /*================================================================*/
    /** Cert <-> DBComm */
    DBCommBase                  = ServerDBCommBase1/*20000*/ + 0,


    AddUBGrant_Req_WebToDB      = 20100,
    AddUBGrant_Rsp_DBToWeb      = 20101,
    ModUBGrant_Req_WebToDB      = 20102,
    ModUBGrant_Rsp_DBToWeb      = 20103,
    DelUBGrant_Req_WebToDB      = 20104,
    DelUBGrant_Rsp_DBToWeb      = 20105,

    AddUBFollow_Req_WebToDB		=20106,
    AddUBFollow_Rsp_DBToWeb		=20107,
    ModUBFollow_Req_WebToDB     =20108,
    ModUBFollow_Rsp_DBToWeb     =20109,
    DelUBFollow_Req_WebToDB     =20110,
    DelUBFollow_Rsp_DBToWeb     =20111,

    DelZhan_Req_WinToDB		=20204,
    DelZhan_Rsp_DBToWin		=20205,

    GetZhan_Req_WinToDB     =20206,
    GetZhan_Rsp_DBToWin     =20207,
    /*================================================================*/
    /* Server <-> Server */
    /*================================================================*/
    P2PClientBase						=  ClientClientBase1 /*30000*/  + 0,
    /*================================================================*/
    /** End */
    Reserve  =0xFFFF    //Ԥ������ʱ��ʱ���
};

}


//Error code ---------------------------------------------------
#define DB_ERR_SUCCESS                      0
#define DBERR_BIND_EXIST                -1004       // �豸�Ѱ�
#define DBERR_ZDID_NOTEXIST             -1001       // �ն����кŲ�����
#define DBERR_NOTBIND                   -1002       // û�а��豸
#define DBERR_ZHDID_EXIST               -1003       // �ն����к��Ѵ���

#define DB_ERR_VERIFYCODE_WRONG         -4002		// ע��У������� */
#define DB_ERR_ACCOUNT_EXIST            -4003		// �û��Ѵ��� */

#define DB_ERR_VERSION_WRONG            -4004		// �汾��ƥ�� */
#define DB_ERR_ACCOUNT_NOTEXIST         -4005		// �ʺŲ����� */
#define DB_ERR_EASTONPASSWD_WRONG       -4006		// ������� */

#define DB_ERR_TERMINAL_NOTEXIST		-4007		// ָ���ն˲����� */

#define DB_ERR_DATETYPE_WRONG           -4008       // ָ������ʾ���ʹ���



#define ERR_USER_NOTEXIST_RELATE		-4011		// �������û������� */
#define ERR_USERTYPE_ERROR				-4012		// ָ�����û����ʹ��� */
#define ERR_USERTYPE_ERROR_RELATE		-4013		// �������û����ʹ��� */

#define EDUDB_ERR_NICK_NOTCHANGED		-4020       //nick is not changed
#define EDUDB_ERR_NOT_PHONEUSER			-4021       //user is not the owner of phone
#define EDUDB_ERR_EMPTY_VALUE			-4022       //empty result on request

#define DB_ERR_UNPROC_NOTEXIST          301         //û��δ�����ļ�

#define EDUDB_ERR_USER_NOTEXIST         -4001       //user not exist
#define EDUDB_ERR_NOT_SUBUSER			-4988       //not personal wisdom pension sub-account
#define EDUDB_ERR_VERIFYINFO_WRONG      -4989       //verify failed  2013-11-25
#define EDUDB_ERR_PDUPARA               -4990       //PDU�������� */
#define EDUDB_ERR_NOT_ADMIN             -4991       //not administer 2013-11-25
#define EDUDB_ERR_USERIDX_NOTEXIST      -4992       //user_index do not exist 2013-11-28
#define EDUDB_ERR_USERMAIL_EXIST		-4993       //user mail already exists
#define EDUDB_ERR_NOT_PERUSER			-4994       //not personal wisdom pension uaccount

#define EDUDB_ERR_DBCONN_BUSY			-4998		//���ݿⷱæ */
#define EDUDB_ERR_ROOTID_NOTEXIST		-4999       //root id does not exist
#define EDUDB_ERR_DBCONN_LOST			-4999		//ʧȥ�����ݿ������ */



//End Error code

//Server using ----------------------------------------------
#define EDUDB_ERR_DBCONN_OK				0
#define DB_ERR_CORPNAME_EXISTED         -4044
#define DB_ERR_DBVERSION_ERROR			-4045		// ���ݿ�汾����
#define EDUDB_ERR_DBCONN_NOCONN			-4996		// û�ж�Ӧ������
#define EDUDB_ERR_DBCONN_INIT			-4997		// �������ڳ�ʼ��
#define DB_ERR_NOROW                    -3001       // û��ƥ���¼
#define DB_ERR_DBDATA_ERROR             -4001		// ���ݴ������

