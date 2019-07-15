/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP回调接口,当TCP/UDP连接有事件发生时,回调
 * 事件包括:当有新连接到达时(TCP),当有TCP报文接收完成时,
 * 当有TCP数据接收到时,当有UDP报文接收完成时,
 * 当对方关闭或出错时(TCP/UDP.

 * @file	SockEventCallBack.h
 * @short	TCP/UDP回调接口
 * @author	chetang <chetang@webcon.com.cn>

 * @class	ISockEventCallBack
 * @short	TCP/UDP回调接口
 **/

#ifndef	__ISOCKEVENTCALLBACK_H__
#define	__ISOCKEVENTCALLBACK_H__

//#include "ace/OS.h"
#include "LinuxDef.h"

class ISockEventCallBack
{
public:
	ISockEventCallBack();
	virtual ~ISockEventCallBack();

	/**
	* 当有一个新的TCP连接到达时回调的函数
	* @param sock 新建的TCP socket的FD
	* @return
	*/
	virtual int OnNewConnection(ACE_HANDLE sock);

	/**
	* 当有一个TCP报文到达时回调的函数
	* @param sock 有报文到达TCP socket的FD
	* @param pdutype 报文的PDU类型
	* @param data 报文数据
	* @param len 报文数据长度
	* @return
	*/
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, const void*data, unsigned long len);

	/**
	* 当有TCP数据到达时回调的函数
	* @param sock 有报文到达TCP socket的FD
	* @param data 报文数据
	* @param len 报文数据长度
	* @return
	*/
	virtual int OnTCPSliceReceived(ACE_HANDLE sock, const void* data, unsigned long len);

	/**
	* 当有一个UDP报文到达时回调的函数
	* @param sock 有报文到达UDP socket的FD
	* @param pdutype 报文的PDU类型
	* @param data 报文数据
	* @param len 报文数据长度
	* @param fromIP 报文来自的IP
	* @param fromPort 报文来自的Port
	* @return
	*/
	virtual int OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
						const void* data, unsigned long len,
						unsigned long fromIP,
						unsigned short fromPort);

	/**
	* 当TCP/UDP连接关闭或出错时回调的函数
	* @param sock 关闭或出错的TCP socket的FD
	* @return
	*/
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:

private:

};

#endif	/*__SOCKEVENTCALLBACK_H__*/

