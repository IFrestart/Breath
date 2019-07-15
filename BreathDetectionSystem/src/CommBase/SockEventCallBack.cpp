/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP回调接口,当TCP/UDP连接有事件发生时,回调
 * 事件包括:当有新连接到达时(TCP),当有TCP报文接收完成时,
 * 当有TCP数据接收到时,当有UDP报文接收完成时,
 * 当对方关闭或出错时(TCP/UDP.

 * @file	SockEventCallBack.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	TCP/UDP回调接口
 **/

#include "SockEventCallBack.h"


ISockEventCallBack::ISockEventCallBack()
{
}

ISockEventCallBack::~ISockEventCallBack()
{
}


int ISockEventCallBack::OnNewConnection(ACE_HANDLE sock)
{
	return 0;
}

int ISockEventCallBack::OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
							const void* data, unsigned long len)
{
	return 0;
}

int ISockEventCallBack::OnTCPSliceReceived(ACE_HANDLE sock, const void* data, unsigned long len)
{
	return 0;
}

int ISockEventCallBack::OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
							const void* data, unsigned long len, 
							unsigned long fromIP, unsigned short fromPort)
{
	return 0;
}

int ISockEventCallBack::OnConnectionClosed(ACE_HANDLE sock)
{
	return 0;
}

