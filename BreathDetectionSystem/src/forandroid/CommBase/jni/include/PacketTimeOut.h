/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ���Ľ��ճ�ʱ�ص��ӿ���

 * @file	PacketTimeOut.h
 * @short	���Ľ��ճ�ʱ�ص��ӿ���
 * @author	chetang <chetang@webcon.com.cn>

 * @class	IPacketTimeOut
 * @short	���Ľ��ճ�ʱ�ص��ӿ���
 **/

#ifndef	__IPACKETTIMEOUT_H__
#define	__IPACKETTIMEOUT_H__

class IPacketTimeOut
{
public:
	IPacketTimeOut();
	virtual ~IPacketTimeOut();

	/**
	* ����һ�����Ľ��ճ�ʱʱ�Ļص��ӿ�
	* @param pdutype ���ĵ�PDU����
	* @param data ��������
	* @param len �������ݳ���
	* @return
	*/
	virtual int OnPacketTimeOut(unsigned short pdutype, const void* data, unsigned long len) = 0;
};

#endif	/*__IPACKETTIMEOUT_H__*/

