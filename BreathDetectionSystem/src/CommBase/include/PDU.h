/**
 * Copyright (c) 2004, HangZhou Webcon Corporation.
 * PDU值定义文件,定义了所有的
 * 客户端与服务端之间的报文
 * 服务端与服务端之间的报文
 * 数据库端与服务端之间的报文
 * PDU值的定义

 * @file	PDU.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	PDU值定义文件
 **/

#ifndef	__PDU_H__
#define	__PDU_H__


namespace PDU
{
/*================================================================*/
/* Segment */
/*================================================================*/
    const unsigned short
        ServerClientBase	= 0,		/**< 客户端与服务端之间的报文PDU值范围	(00000~09999)*/
        ServerServerBase	= 10000,	/**< 服务端与服务端之间的报文PDU值范围	(10000~19999)*/
        ServerDBCommBase	= 20000,	/**< 数据库端与服务端之间的报文PDU值范围(20000~30000)*/

        ClientClientBase	= 30000;

    /** Begin */
    enum {

/*================================================================*/
/* Server <-> Client */
/*================================================================*/
    /** Cert <-> Client */
        CertClientBase				= ServerClientBase/*0*/ + 0,

        MAINCERT_REQ_FROMCLI = 5,
        MAINCERT_RSP_TOCLI = 6,
        USERLOGIN_REQ_FROMCLI = 7,
        USERLOGIN_RSP_TOCLI = 8,
        USERLOGOUT_REQ_FROMCLI = 9,
        KEEPALIVE_REQ_FROMCLI = 10,
        KEEPALIVE_RSP_TOCLI = 11,

        OpROUTER0_REQ_FromClient = 12,
        OpROUTER0_RSP_ToClient = 13,
        TransData_REQ_ToClient = 14,
        TransData_RSP_FromClient = 15,

        DBROUTER0_REQ_FROMCLI = 31,
        DBROUTER0_RSP_TOCLI = 32,

        CenterROUTER0_REQ_FROMCLI = 33,
        CenterROUTER0_RSP_TOCLI = 34,

        GetServerOnLine_REQ_FROMCLI = 35,
        GetServerOnLine_RSP_TOCLI = 36,
        NoticeServerOnLine_REQ_FROMCLI = 37,
        NoticeOnLine_RSP_TOCLI = 38,
        NoticeSecondLogin_RSP_TOCLI = 39,

        DATEPROC_REQ_FROMCLI    =41,
        DATEPROC_RSP_TOCLI      =42,

        //SubPduTranAVData_REQ_FromWin = 100,

    /** SesRelate <-> Client */
        SesRelateClientBase			= ServerClientBase/*0*/ + 500,

                JOINSESSION_REQ_FROMCLI = 501,
                JOINSESSION_RSP_TOCLI = 502,
                JOINSESSIONO_RSP_TOCLI = 503,
                LEAVESESSION_REQ_FROMCLI = 504,
                LEAVESESSION_RSP_TOCLI = 505,
                LEAVESESSIONO_RSP_TOCLI = 506,
                TRANSFERINSESSION_REQ_FROMCLI = 507,
                TRANSFERINSESSION_RSP_TOCLI = 508,
                TRANSFERINSESSIONO_RSP_TOCLI = 509,

                /** keepalive */
                SESKEEPALIVE_REQ_FROMCLI = 514,
                SESKEEPALIVE_RSP_TOCLI = 515,
        DownloadData_REQ_FromClient = 516,
        DownlaodData_RSP_ToClient = 517,


/*================================================================*/
/* Server <-> Server */
/*================================================================*/
        CertServerBase	= ServerServerBase/*10000*/ + 0,
        NHSystemDBROUTER0_REQ_FromNHSystem = 10001,
        NHSystemDBROUTER0_RSP_ToNHSystem = 10002,
        TransData_REQ_FromNHSystem = 10007,
        TransData_RSP_ToNHSystem = 10010,

        OpROUTER0_REQ_FromCenter = 10011,
        OpROUTER0_REQ_FromSub = 10012,
        OpROUTER0_RSP_ToSub = 10013,
        OpROUTER0_RSP_ToCenter = 10014,

        NHSystemServerReport_REQ_FromNHSystem = 10015,
        NHSystemServerReport_REQ_FromSub  = 10016,
        NHSystemServerReport_RSP_ToSub = 10017,
        NHSystemServerReport_RSP_ToNHSystem =  10018,
        NHSystemServerSave_REQ_FromNHSystem  = 10019,
        NHSystemServerSave_RSP_ToNHSystem = 10020,
        UserLogoutO_REQ_FROMCenter = 10021,



/*================================================================*/
/* Server <-> DBComm */
/*================================================================*/
    /** Cert <-> DBComm */
        CertDBCommBase				= ServerDBCommBase/*20000*/ + 0,

        GETCHECKCODE_REQ_TODBCOMM,
        GETCHECKCODE_RSP_FROMDBCOMM,
        REGISTER_REQ_TODBCOMM,
        REGISTER_RSP_FROMDBCOMM,
        MAINCERT_REQ_TODBCOMM = 20005,
        MAINCERT_RSP_FROMDBCOMM = 20006,

        ROUTER0_REQ_TODBCOMM = 20007,
        ROUTER0_RSP_FROMDBCOMM = 20008,

        NHSystemDBROUTER0_REQ_FromCenter = 20009,
        NHSystemDBROUTER0_RSP_ToCenter = 20010,


        RelateDBCommBase			= ServerDBCommBase/*20000*/ + 500,
        DATEPROC_REQ_TODBCOMM       =21001,
        DATEPROC_RSP_FROMDBCOMM     =21002,



/*================================================================*/
/* Server <-> Server */
/*================================================================*/
        P2PClientBase						=  ClientClientBase /*30000*/  + 0
/*================================================================*/
    /** End */
    };

};

#endif	/*__PDU_H__*/

