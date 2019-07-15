/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP回调类, 提供了新源到达,数据接收到,错误发生等接口

 * @file	RTPEventCallBack.h
 * @short	RTP回调类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	IRTPEventCallBack
 * @short	RTP回调类
 **/

#ifndef	__RTPEVENTCALLBACK_H__
#define	__RTPEVENTCALLBACK_H__

#include "rtpdefines.h"

class IRTPEventCallBack
{
public:
	/**
	* 当有一个新的RTP连接到达时回调的函数
	* @param fromIP 新建的RTP的IP
	* @param fromPort 新建的RTP的Port
	* @return
	*/
	virtual int OnNewRTPSource(unsigned long fromIP, 
								unsigned short fromPort);

	/**
	* 当有一个RTP报文到达时回调的函数
	* @param sock 有报文到达RTP socket的FD
	* @param pdutype 报文的PDU类型
	* @param data 报文数据
	* @param len 报文数据长度
	* @param fromIP 报文来自的IP
	* @param fromPort 报文来自的Port
	* @return
	*/
	virtual int OnRTPPacketReceived(RTPSOCKET sock, unsigned short pdutype,
								const void* data, unsigned long len, 
								unsigned long fromIP, 
								unsigned short fromPort);

	/**
	* 当RTP连接丢包,离开或出错时回调的函数
	* @param errCode 取值为CRTPConnector::SSRCxxx < 0
	* @param fromIP 丢包,离开或出错的RTP 的IP
	* @param fromPort 丢包,离开或出错的RTP 的Port
	* @return
	*/
	virtual int OnErrorRTPSource(int errCode, 
								unsigned long fromIP, 
								unsigned short fromPort);
};

#endif	/*__RTPEVENTCALLBACK_H__*/

