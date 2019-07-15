/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP回调类, 提供了新源到达,数据接收到,错误发生等接口

 * @file	RTPEventCallBack.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	RTP回调类
 **/

#include "RTPEventCallBack.h"

/*
* IRTPEventCallBack
*/
int IRTPEventCallBack::OnNewRTPSource(unsigned long fromIP, 
							unsigned short fromPort)
{
	return 0;
}

int IRTPEventCallBack::OnRTPPacketReceived(RTPSOCKET sock, unsigned short pdutype,
							const void* data, unsigned long len, 
							unsigned long fromIP, 
							unsigned short fromPort)
{
	return 0;
}

int IRTPEventCallBack::OnErrorRTPSource(int errCode, 
							unsigned long fromIP, 
							unsigned short fromPort)
{
	return 0;
}

