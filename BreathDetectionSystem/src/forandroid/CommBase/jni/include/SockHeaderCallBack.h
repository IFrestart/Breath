/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP协议头回调接口,包括取协议头长度,根据PDU类型与报文体长度生成协议头,
 * 从协议头中取PDU类型与报文体长度等操作

 * @file	SockHeaderCallBack.h
 * @short	TCP/UDP协议头回调接口
 * @author	chetang <chetang@webcon.com.cn>

 * @class	ISockHeaderCallBack 
 * @short	TCP/UDP协议头回调接口
 **/

#ifndef	__SOCKHEADERCALLBACK_H__
#define	__SOCKHEADERCALLBACK_H__


class ISockHeaderCallBack 
{
public:
	ISockHeaderCallBack();
	virtual ~ISockHeaderCallBack();

	/**
	* 取TCP/UDP报文头的长度
	*/
	virtual unsigned short GetPacketHeaderSize() = 0;

	/**
	* 根据给定的pduType与bodyLen生成TCP/UDP报文头存放到pHeader中,
	* 要确保pHeader的至少可存放TCP/UDP报文头长度
	* @param pHeader 生成的TCP/UDP报文头要存放的位置
	* @param pduType PDU类型
	* @param bodyLen 数据包长度(不包括TCP/UDP报文头)
	* @return 成功返回0
	*/
	virtual int GeneratePacketHeader(void* pHeader, 
							unsigned short pduType, unsigned long bodyLen) = 0;

	/**
	* 根据给定的TCP/UDP报文头取出pduType与bodyLen,
	* 要确保pHeader的至少存放有TCP/UDP报文头的长度
	* @param pHeader 生成的TCP/UDP报文头要存放的位置
	* @param pduType 存放PDU类型
	* @param bodyLen 存放数据包长度(不包括TCP/UDP报文头)
	* @return 成功返回0
	*/
	virtual int ParsePacketHeader(const void* pHeader, 
							unsigned short& pduType, unsigned long& bodyLen) = 0;

protected:

private:

};

#endif	/*__SOCKHEADERCALLBACK_H__*/

