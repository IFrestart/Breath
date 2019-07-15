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



		SubPduTranAVData_REQ_FromWin = 100,

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


/*================================================================*/
/* Server <-> Server */
/*================================================================*/
		CertServerBase	= ServerServerBase/*10000*/ + 0,
		WINDBROUTER0_REQ_FromWin = 10001,
		WINDBROUTER0_REQ_FromSub = 10002,
		WINDBROUTER0_REQ_FromCenter = 10003,
		WINDBROUTER0_RSP_ToCenter = 10004,
		WINDBROUTER0_RSP_ToSub = 10005,
		WINDBROUTER0_RSP_ToWin = 10006,
		TransData_REQ_FromWin = 10007,
		TransData_REQ_FromSub = 10008,
		TransData_RSP_ToSub = 10009,
		TransData_RSP_ToWin = 10010,

		OpROUTER0_REQ_FromCenter = 10011,
		OpROUTER0_REQ_FromSub = 10012,
		OpROUTER0_RSP_ToSub = 10013,
		OpROUTER0_RSP_ToCenter = 10014,

		WinSysServerReport_REQ_FromWinSys = 10015,
		WinSysServerReport_REQ_FromSub  = 10016,
		WinSysServerReport_RSP_ToSub = 10017,
		WinSysServerReport_RSP_ToWinSys =  10018,
		SubServerReport_REQ_FromSub  = 10019,
		SubServerReport_RSP_ToSub = 10020,
		
		

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


/*================================================================*/
/* Server <-> Server */
/*================================================================*/	  
		P2PClientBase						=  ClientClientBase /*30000*/  + 0,
/*================================================================*/
	/** End */
	};

};

#endif	/*__PDU_H__*/

