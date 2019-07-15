/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTP协议头

 * @file	HTTPHead.cpp
 * @author	eagle,chetang <chetang@webcon.com.cn>

 * @short	HTTP协议头
 **/

#include <iostream>

#include "ace/Signal.h"
#include "ace/Time_Value.h"
#include <string>

#include "HTTPHead.h"
#include "SockStream.h"


/*
* CHTTPHeader
*/
unsigned short CHTTPHeader::GetPacketHeaderSize()
{
	return GetHttpHeaderSize() + PARENT::GetPacketHeaderSize();
}

unsigned short CHTTPHeader::GetHttpHeaderSize()
{
	return HTTP_HEADER_LENGTH;
}

int CHTTPHeader::GeneratePacketHeader(void* pHeader, unsigned short pduType, unsigned long bodyLen)
{
	char 	data[100];
	int	ret = -1;

	sprintf(data, 
		"POST /server/default.jsp HTTP/1.1\r\nHost: localhost\r\nContent-Length: %10d\r\n\r\n",
		bodyLen + PARENT::GetPacketHeaderSize());
	if(0 == PARENT::GeneratePacketHeader((char*)pHeader + GetHttpHeaderSize(), pduType, bodyLen)) {
		CSockStream		sockStream(pHeader, GetPacketHeaderSize());
		sockStream.writeBytes(data, GetHttpHeaderSize());
		ret = 0;
	}

	return ret;
}

