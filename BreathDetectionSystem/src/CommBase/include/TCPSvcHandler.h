/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP�������ݽ����뷢�ʹ�����,�ṩ��TCP���ӵĴ�,�ر��뷢�����ݵȲ���
 * ������һ���������ĵ���ʱ�����ʱִ�лص�.

 * @file	TCPSvcHandler.h
 * @short	TCP�������ݽ����뷢�ʹ�����
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPSvcHandlerTpl
 * @short	TCP�������ݽ����뷢�ʹ�����
 **/

#ifndef	__TCPSVCHANDLER_H__
#define	__TCPSVCHANDLER_H__

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"

#include "SockHeader.h"
#include "RECSockHeader.h"
#include "SockBlock.h"
#include "Mutex.h"
#include "BlockQueue.h"
#include "SvcManager.h"

template <class SOCKHEADER>
class CTCPSvcHandlerTpl: public ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_NULL_SYNCH> 
{
public: 
	CTCPSvcHandlerTpl();
	~CTCPSvcHandlerTpl();

	/* From ACE_Svc_Handler */
	/**
	* ��TCP����,�̳���ACE_Svc_Handler<>,��CTCPConnector��CTCPAcceptor�����
	* @return �ɹ��򷵻�0
	* @see ACE_Svc_Handler<>::open(),CTCPConnector<>::open(),CTCPAcceptor<>::open()
	*/
	virtual int open(void* args = 0);

	/**
	* �̳���ACE_Event_Handler,�������ݵ���ʱ����ȥ��,
	* ���յ�һ�������ı���ʱ,�ص�
	* @see ACE_Event_Handler::handle_input()
	*/
	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);

	/**
	* �̳���ACE_Event_Handler,�������ݵ���ʱ����ȥ��,
	* ���յ�һ�������ı���ʱ,�ص�
	* @see ACE_Event_Handler::handle_output()
	*/
	virtual int handle_output(ACE_HANDLE h = ACE_INVALID_HANDLE);

	/**
	* �̳���ACE_Event_Handler,��socket����ʱ,�ص�
	* @see ACE_Event_Handler::handle_close()
	*/
	virtual int handle_close(ACE_HANDLE h = ACE_INVALID_HANDLE, 
						ACE_Reactor_Mask m = ACE_Event_Handler::ALL_EVENTS_MASK);
	virtual void destroy (void);

	/**
	* ͨ��TCP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	void initialize(ISvcManager* psm);

private:
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_NULL_SYNCH> PARENT;

	enum { READMASK = 0x01, WRITEMASK = 0x02 };
	enum { HIGWATERMARK = 16*1024*1024 };
	unsigned short		mask_;

	ISvcManager*		pSvcManager_;	/**< �����ӵ���,���Ľ��յ������ʱ�ص��Ľӿ�*/
	SOCKHEADER		sockHeaderCallBack_;	/**< ����TCPЭ��ͷ�ص��Ľӿ�*/

	bool			isBegin_;	/**< �Ƿ�ʼ������һ������*/
	bool			isHeader_;	/**< �Ƿ�ʼ���ձ���ͷ*/
	CSockBlock*		mbRecv_;	/**< ���ձ��ĵĻ�����*/
	unsigned short		pduType_;	/**< Ҫ���ձ��ĵ�PDU����*/
	unsigned long		wantLen_;	/**< ���ڽ��յı��Ļ�Ҫ���ն����ֽڵ�����*/

	CSockBlock*		mbSend_;	/**< ���ͱ��ĵĻ�����*/
	CMutex			mutex_;
	CBlockQueue		blockQueue_;
};

#include "../TCPSvcHandler.cc"
typedef CTCPSvcHandlerTpl<CSockHeader> CTCPSvcHandler;

typedef CTCPSvcHandlerTpl<CRECSockHeader> CTCPRECSvcHandler;

#endif	/*__TCPSVCHANDLER_H__*/

