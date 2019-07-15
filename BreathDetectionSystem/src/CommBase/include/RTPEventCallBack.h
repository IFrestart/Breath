/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP�ص���, �ṩ����Դ����,���ݽ��յ�,�������Ƚӿ�

 * @file	RTPEventCallBack.h
 * @short	RTP�ص���
 * @author	chetang <chetang@webcon.com.cn>

 * @class	IRTPEventCallBack
 * @short	RTP�ص���
 **/

#ifndef	__RTPEVENTCALLBACK_H__
#define	__RTPEVENTCALLBACK_H__

#include "rtpdefines.h"

class IRTPEventCallBack
{
public:
	/**
	* ����һ���µ�RTP���ӵ���ʱ�ص��ĺ���
	* @param fromIP �½���RTP��IP
	* @param fromPort �½���RTP��Port
	* @return
	*/
	virtual int OnNewRTPSource(unsigned long fromIP, 
								unsigned short fromPort);

	/**
	* ����һ��RTP���ĵ���ʱ�ص��ĺ���
	* @param sock �б��ĵ���RTP socket��FD
	* @param pdutype ���ĵ�PDU����
	* @param data ��������
	* @param len �������ݳ���
	* @param fromIP �������Ե�IP
	* @param fromPort �������Ե�Port
	* @return
	*/
	virtual int OnRTPPacketReceived(RTPSOCKET sock, unsigned short pdutype,
								const void* data, unsigned long len, 
								unsigned long fromIP, 
								unsigned short fromPort);

	/**
	* ��RTP���Ӷ���,�뿪�����ʱ�ص��ĺ���
	* @param errCode ȡֵΪCRTPConnector::SSRCxxx < 0
	* @param fromIP ����,�뿪������RTP ��IP
	* @param fromPort ����,�뿪������RTP ��Port
	* @return
	*/
	virtual int OnErrorRTPSource(int errCode, 
								unsigned long fromIP, 
								unsigned short fromPort);
};

#endif	/*__RTPEVENTCALLBACK_H__*/

