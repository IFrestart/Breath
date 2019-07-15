/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP�������ݽ����뷢�ʹ�����,�ṩ��UDP���ӵĴ�,�ر��뷢�����ݵȲ���
 * ������һ���������ĵ���ʱ�����ʱִ�лص�.

 * @file	UDPSvcHandler.h
 * @short	UDP�������ݽ����뷢�ʹ�����
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CUDPSvcHandlerTpl
 * @short	UDP�������ݽ����뷢�ʹ�����
 **/

#ifndef	__UDPSVCHANDLER_H__
#define	__UDPSVCHANDLER_H__

#include "ace/Svc_Handler.h"
#include "ace/Event_Handler.h"
#include "ace/SOCK_Dgram.h"

#include "SockHeader.h"
#include "SockEventCallBack.h"

template <class SOCKHEADER>
class CUDPSvcHandlerTpl: public ACE_Event_Handler
{
public: 
	CUDPSvcHandlerTpl(ISockEventCallBack*);
	~CUDPSvcHandlerTpl();

	/**
	* ��UDP����
	* @param port UDP socketҪ�󶨵ı��ض˿ں�
	* @return �ɹ��򷵻�0
	*/
	int open(unsigned short port);

	/**
	* ��UDP����
	* @param ipstr UDP socketҪ�󶨵ı���IP��ַ,��Ϊ0,��󶨱��ص�����IP
	* @param port UDP socketҪ�󶨵ı��ض˿ں�
	* @return �ɹ��򷵻�0
	*/
	int open(const char* ipstr, unsigned short port);

	/**
	* ȡ��UDP��socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪUDP socket
	*/
	ACE_HANDLE get_handle() const;

	/**
	* �ر�UDP����
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* ͨ��UDP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @param ip socket�����ݷ��͵��ĸ�IP
	* @param port socket�����ݷ��͵��ĸ�PORT
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len, unsigned long ip, unsigned short port);

protected:
	/**
	* �̳���ACE_Event_Handler,�������ݵ���ʱ����ȥ��,
	* ���յ�һ�������ı���ʱ,�ص�
	* @see ACE_Event_Handler::handle_input()
	*/
	virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);

	/**
	* �̳���ACE_Event_Handler,��socket����ʱ,�ص�
	* @see ACE_Event_Handler::handle_close()
	*/
	virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);

private:
	typedef ACE_Event_Handler PARENT;
	enum { MAXSOCKBUFFERLEN = 64*1024 };	/**< �������ݰ���С*/

	ISockEventCallBack*	pSockEventCallBack_;	/**< �б��Ľ��յ������ʱ�ص��Ľӿ�*/
	SOCKHEADER		sockHeaderCallBack_;	/**< ����UDPЭ��ͷ�ص��Ľӿ�*/
	ACE_SOCK_Dgram		dgram_;	/**< ��װUDP socket����,@see ACE_SOCK_Dgram*/
	
	unsigned char		recvBuf_[MAXSOCKBUFFERLEN];	/**< ���ջ�����*/
	unsigned char		sendBuf_[MAXSOCKBUFFERLEN];	/**< ���ͻ�����*/
};

#include "../UDPSvcHandler.cc"
typedef CUDPSvcHandlerTpl<CSockHeader> CUDPSvcHandler;

#endif	/*__UDPSVCHANDLER_H__*/

