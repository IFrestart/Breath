/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP�ص��ӿ�,��TCP/UDP�������¼�����ʱ,�ص�
 * �¼�����:���������ӵ���ʱ(TCP),����TCP���Ľ������ʱ,
 * ����TCP���ݽ��յ�ʱ,����UDP���Ľ������ʱ,
 * ���Է��رջ����ʱ(TCP/UDP.

 * @file	SockEventCallBack.h
 * @short	TCP/UDP�ص��ӿ�
 * @author	chetang <chetang@webcon.com.cn>

 * @class	ISockEventCallBack
 * @short	TCP/UDP�ص��ӿ�
 **/

#ifndef	__ISOCKEVENTCALLBACK_H__
#define	__ISOCKEVENTCALLBACK_H__

//#include "ace/OS.h"
#include "LinuxDef.h"

class ISockEventCallBack
{
public:
	ISockEventCallBack();
	virtual ~ISockEventCallBack();

	/**
	* ����һ���µ�TCP���ӵ���ʱ�ص��ĺ���
	* @param sock �½���TCP socket��FD
	* @return
	*/
	virtual int OnNewConnection(ACE_HANDLE sock);

	/**
	* ����һ��TCP���ĵ���ʱ�ص��ĺ���
	* @param sock �б��ĵ���TCP socket��FD
	* @param pdutype ���ĵ�PDU����
	* @param data ��������
	* @param len �������ݳ���
	* @return
	*/
	virtual int OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype, const void*data, unsigned long len);

	/**
	* ����TCP���ݵ���ʱ�ص��ĺ���
	* @param sock �б��ĵ���TCP socket��FD
	* @param data ��������
	* @param len �������ݳ���
	* @return
	*/
	virtual int OnTCPSliceReceived(ACE_HANDLE sock, const void* data, unsigned long len);

	/**
	* ����һ��UDP���ĵ���ʱ�ص��ĺ���
	* @param sock �б��ĵ���UDP socket��FD
	* @param pdutype ���ĵ�PDU����
	* @param data ��������
	* @param len �������ݳ���
	* @param fromIP �������Ե�IP
	* @param fromPort �������Ե�Port
	* @return
	*/
	virtual int OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
						const void* data, unsigned long len,
						unsigned long fromIP,
						unsigned short fromPort);

	/**
	* ��TCP/UDP���ӹرջ����ʱ�ص��ĺ���
	* @param sock �رջ�����TCP socket��FD
	* @return
	*/
	virtual int OnConnectionClosed(ACE_HANDLE sock);

protected:

private:

};

#endif	/*__SOCKEVENTCALLBACK_H__*/

