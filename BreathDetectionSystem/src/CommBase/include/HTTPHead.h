/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTP协议头

 * @file	HTTPHead.h
 * @short	HTTP协议头
 * @author	eagle, chetang <chetang@webcon.com.cn>

 * @class	CHTTPHeader
 * @short	HTTP协议头
 **/

#ifndef	__HTTPHEAD_H__
#define	__HTTPHEAD_H__

#include "ace/OS.h"
#include "ace/Event_Handler.h"

#include "SockHeader.h"


class CHTTPHeader : public CSockHeader
{
public:
	/**
	* ISockHeaderCallBack中的实现函数
	* @return 返回HTTP协议头+自定义协议头的长度
	* @see ISockHeaderCallBack::GetPacketHeaderSize()
	*/
	virtual unsigned short GetPacketHeaderSize();
	virtual unsigned short GetHttpHeaderSize();

	/**
	* ISockHeaderCallBack中的实现函数
	* @return 成功则返回0
	* @see ISockHeaderCallBack::GeneratePacketHeader()
	*/
	virtual int GeneratePacketHeader(void*, unsigned short, unsigned long);

private:
	enum {HTTP_HEADER_LENGTH = 82};
	typedef CSockHeader	PARENT;
};

#endif	/*__HTTPHEAD_H__*/

