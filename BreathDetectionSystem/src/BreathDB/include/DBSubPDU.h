#include "PDU.h"
//Sub PDU type define ------------------------------------------

#define USERNAME_TYPE 1     //检测用户名称

//End Sub PDU type define

namespace SubPDU
{
/*================================================================*/
/* Segment */
/*================================================================*/
const unsigned short
ServerClientBase1	= 0,		/**< 客户端与服务端之间的报文PDU值范围	(00000~09999)*/
ServerServerBase1	= 10000,	/**< 服务端与服务端之间的报文PDU值范围	(10000~19999)*/
ServerDBCommBase1	= 20000,	/**< 数据库端与服务端之间的报文PDU值范围 (20000~30000)*/
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

    //添加openid
    AddOpenId_Req_WebToDB       =70,
    AddOpenId_Rsp_DBToWeb       =71,

    //获取绑定终端列表
    GetBandZDList_Req_WebToDB     =72,
    GetBandZDList_Rsp_DBToWeb     =73,

    //openid绑定设备（微信公众号用）
    BindOpenid_Req_WebToDB        =74,
    BindOpenid_Rsp_DBToWeb        =75,

    //解除openid绑定设备（微信公众号用）
    DelBindOpenid_Req_WebToDB       =76,
    DelBindOpenid_Rsp_DBToWeb       =77,

    //获取状态文件列表
    getStatFilelist_REQ_TODBCOMM        =78,
    getStatFilelist_RSP_FROMDBCOMM      =79,

    //入库根据平滑参数得到的状态结果
    inStat_BaseSmoothPara_REQ_TODBCOMM          =80,
    inStat_BaseSmoothPara_RSP_FROMDBCOMM        =81,

    //获取未处理文件目录
    getUnprocDir_REQ_TODBCOMM           =82,
    getUnprocDir_RSP_FROMDBCOMM         =83,

    //获取绑定终端列表
    GetBandZhDList_Req_WebToDB          =84,
    GetBandZhDList_Rsp_DBToWeb          =85,

    //终端设备入库
    AddTerminalInfo_REQ_FromWeb         =86,
    AddTerminalInfo_RSP_ToWeb           =87,

    //销毁终端设备
    DelTerminalInfo_REQ_FromWeb         =90,
    DelTerminalInfo_RSP_ToWeb           =91,

    //查看所有入库设备
    GetTerminalList_REQ_FromWeb         =92,
    GetTerminalList_RSP_ToWeb           =93,

    //添加报警信息
    DBSUB_AddAlarmInfo_Req_FromNHSystem =94,
    DBSUB_AddAlarmInfo_Rsp_ToNHSystem   =95,

    /*======上传文件模块===============================================*/
    //添加检测数据索引
    DBSUB_AddDataInfo_Req_FromNHSystem  =96,
    DBSUB_AddDataInfo_Rsp_ToNHSystem    =97,

    //添加报警信息2
    AddAlarmInfo2_Req_FromNHSystem      =98,
    AddAlarmInfo2_Rsp_ToNHSystem        =99,

    //获取监控设备报警信息
    GetAlarmInfoListPage_Req_FromCli    =114,
    GetAlarmInfoListPage_Rsp_ToCli      =115,

    //获取监测数据
    GetDataInfoListPage_Req_FromCli     =116,
    GetDataInfoListPage_Rsp_ToCli       =117,

    //获取报警内容
    GetAlarmInfoData_Req_FromCli        =118,
    GetAlarmInfoData_Rsp_ToCli          =119,

    //查看呼吸事件
    GetBreathInfo_Req_WebToDB           =120,
    GetBreathInfo_Rsp_DBToWeb           =121,

    //测试呼吸事件
    tryBreathInfo_Req_WebToDB           =122,
    tryBreathInfo_Rsp_DBToWeb           =123,

    //获取干涉时间
    GetIpeTime_REQ_FromCli              =124,
    GetIpeTime_RSP_ToCli                =125,

    //设置干涉时间
    SetIpeTime_REQ_FromWeb              =126,
    SetIpeTime_RSP_ToDB                 =127,

    //获取报警信息2
    GetAlarmInfoListPage2_Req_FromCli   =130,
    GetAlarmInfoListPage2_Rsp_ToCli     =131,

    //
    GetSleepInfo_REQ_FromCli            =140,
    GetSleepInfo_RSP_ToCli              =141,

    //获取呼吸和心率
    GetBRInfo_REQ_FromCli            =160,
    GetBRInfo_RSP_ToCli              =161,

    //获取失眠信息
    GetInsomInfo_REQ_FromCli         =162,
    GetInsomInfo_RSP_ToCli           =163,

    //获取异常统计
    Abnormal_statistics_Req_FromCLI =203,
    Abnormal_statistics_Rsp_ToCLI   =204,

    //添加心跳率和呼吸率
    AddHX_Req_WebToDB               =1101,
    AddHX_Rsp_DBToWeb               =1102,

    //查看心跳率和呼吸率
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
    Reserve  =0xFFFF    //预留功能时暂时替代
};

}


//Error code ---------------------------------------------------
#define DB_ERR_SUCCESS                      0
#define DBERR_BIND_EXIST                -1004       // 设备已绑定
#define DBERR_ZDID_NOTEXIST             -1001       // 终端序列号不存在
#define DBERR_NOTBIND                   -1002       // 没有绑定设备
#define DBERR_ZHDID_EXIST               -1003       // 终端序列号已存在

#define DB_ERR_VERIFYCODE_WRONG         -4002		// 注册校验码错误 */
#define DB_ERR_ACCOUNT_EXIST            -4003		// 用户已存在 */

#define DB_ERR_VERSION_WRONG            -4004		// 版本不匹配 */
#define DB_ERR_ACCOUNT_NOTEXIST         -4005		// 帐号不存在 */
#define DB_ERR_EASTONPASSWD_WRONG       -4006		// 密码错误 */

#define DB_ERR_TERMINAL_NOTEXIST		-4007		// 指定终端不存在 */

#define DB_ERR_DATETYPE_WRONG           -4008       // 指定的显示类型错误



#define ERR_USER_NOTEXIST_RELATE		-4011		// 关联的用户不存在 */
#define ERR_USERTYPE_ERROR				-4012		// 指定的用户类型错误 */
#define ERR_USERTYPE_ERROR_RELATE		-4013		// 关联的用户类型错误 */

#define EDUDB_ERR_NICK_NOTCHANGED		-4020       //nick is not changed
#define EDUDB_ERR_NOT_PHONEUSER			-4021       //user is not the owner of phone
#define EDUDB_ERR_EMPTY_VALUE			-4022       //empty result on request

#define DB_ERR_UNPROC_NOTEXIST          301         //没有未处理文件

#define EDUDB_ERR_USER_NOTEXIST         -4001       //user not exist
#define EDUDB_ERR_NOT_SUBUSER			-4988       //not personal wisdom pension sub-account
#define EDUDB_ERR_VERIFYINFO_WRONG      -4989       //verify failed  2013-11-25
#define EDUDB_ERR_PDUPARA               -4990       //PDU参数错误 */
#define EDUDB_ERR_NOT_ADMIN             -4991       //not administer 2013-11-25
#define EDUDB_ERR_USERIDX_NOTEXIST      -4992       //user_index do not exist 2013-11-28
#define EDUDB_ERR_USERMAIL_EXIST		-4993       //user mail already exists
#define EDUDB_ERR_NOT_PERUSER			-4994       //not personal wisdom pension uaccount

#define EDUDB_ERR_DBCONN_BUSY			-4998		//数据库繁忙 */
#define EDUDB_ERR_ROOTID_NOTEXIST		-4999       //root id does not exist
#define EDUDB_ERR_DBCONN_LOST			-4999		//失去和数据库的连接 */



//End Error code

//Server using ----------------------------------------------
#define EDUDB_ERR_DBCONN_OK				0
#define DB_ERR_CORPNAME_EXISTED         -4044
#define DB_ERR_DBVERSION_ERROR			-4045		// 数据库版本不对
#define EDUDB_ERR_DBCONN_NOCONN			-4996		// 没有对应的连接
#define EDUDB_ERR_DBCONN_INIT			-4997		// 连接正在初始化
#define DB_ERR_NOROW                    -3001       // 没有匹配记录
#define DB_ERR_DBDATA_ERROR             -4001		// 数据处理错误

