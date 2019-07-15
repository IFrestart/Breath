/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 报文接收超时回调接口类

 * @file	PacketTimeOut.h
 * @short	报文接收超时回调接口类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	IPacketTimeOut
 * @short	报文接收超时回调接口类
 **/

#ifndef	__IPACKETTIMEOUT_H__
#define	__IPACKETTIMEOUT_H__

class IPacketTimeOut
{
public:
	IPacketTimeOut();
	virtual ~IPacketTimeOut();

	/**
	* 当有一个报文接收超时时的回调接口
	* @param pdutype 报文的PDU类型
	* @param data 报文数据
	* @param len 报文数据长度
	* @return
	*/
	virtual int OnPacketTimeOut(unsigned short pdutype, const void* data, unsigned long len) = 0;
};

#endif	/*__IPACKETTIMEOUT_H__*/

