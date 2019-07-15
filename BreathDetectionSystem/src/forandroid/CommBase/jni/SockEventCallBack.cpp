/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP�ص��ӿ�,��TCP/UDP�������¼�����ʱ,�ص�
 * �¼�����:���������ӵ���ʱ(TCP),����TCP���Ľ������ʱ,
 * ����TCP���ݽ��յ�ʱ,����UDP���Ľ������ʱ,
 * ���Է��رջ����ʱ(TCP/UDP.

 * @file	SockEventCallBack.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	TCP/UDP�ص��ӿ�
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

