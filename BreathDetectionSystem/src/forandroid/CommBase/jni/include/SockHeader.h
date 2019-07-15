/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 定义了TCP/UDP数据的报文头

 * @file	SockHeader.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	报文头类
 **/

#ifndef	__SOCKHEADER_H__
#define	__SOCKHEADER_H__

//#include "ace/OS.h"

#include "SockHeaderCallBack.h"


class CSockHeader : public ISockHeaderCallBack
{
public:
	/**
	* ISockHeaderCallBack中的实现函数
	* @return 返回TCP/UDP协议头的长度
	* @see ISockHeaderCallBack::GetPacketHeaderSize()
	*/
	virtual unsigned short GetPacketHeaderSize();

	/**
	* ISockHeaderCallBack中的实现函数
	* @return 成功则返回0
	* @see ISockHeaderCallBack::GeneratePacketHeader()
	*/
	virtual int GeneratePacketHeader(void* pHeader, 
							unsigned short pduType, unsigned long bodyLen);

	/**
	* ISockHeaderCallBack中的实现函数
	* @return 成功则返回0
	* @see ISockHeaderCallBack::ParsePacketHeader()
	*/
	virtual int ParsePacketHeader(const void* pHeader, 
							unsigned short& pduType, unsigned long& bodyLen);

private:
	enum {PACKET_VERSION_NO = 0x0101, MAX_PACKET_SIZE = 10 * 1024 * 1024};

	unsigned long	bodyLen_;	/**< 报文体长度 */
	unsigned short	dataType_;	/**< 数据类型 */
	unsigned long	verify_;	/**< 校验码 */
	unsigned short	version_;	/**< 协议的版本号 */
	unsigned short	pduType_;	/**< 报文类型 */
};

#endif /*__SOCKHEADER_H__*/

