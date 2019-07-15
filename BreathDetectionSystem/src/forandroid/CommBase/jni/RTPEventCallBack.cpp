/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP�ص���, �ṩ����Դ����,���ݽ��յ�,�������Ƚӿ�

 * @file	RTPEventCallBack.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	RTP�ص���
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

