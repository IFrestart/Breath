/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����TCP��,�ṩ����TCP�Ľ���,�ر�,�������ݵȲ���

 * @file	TCPConnector.h
 * @short	����TCP��
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPConnectorTpl
 * @short	����TCP��
 **/

#ifndef	__TCPCONNECTOR_H__
#define	__TCPCONNECTOR_H__

#include "ace/SOCK_Connector.h"
#include "ace/Connector.h"

#include "Timer.h"
#include "SvcManager.h"
#include "SockEventCallBack.h"
#include "TCPSvcHandler.h"

template <class SVCHANDLER>
class CTCPConnectorTpl : public ISvcManager, public ACE_Connector<SVCHANDLER,ACE_SOCK_CONNECTOR>
{
public:
	CTCPConnectorTpl(ISockEventCallBack* = 0);
	/**
	* ʵ��Ϊ��,���Զ�����ACE_Connector������,
	* ��������SVC_HANDLER::close() 
	* ��������SVC_HANDLER::handle_close() 
	*/
	~CTCPConnectorTpl();

	/**
	* ����һ��TCP socket
	* @param ipstr socketҪ���ӵ���IP,�ַ���
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* ����һ��TCP socket
	* @param ip socketҪ���ӵ���IP
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* �ر�TCP socket
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* ͨ��TCP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	/**
	* ����TCP socket�ײ��FD
	* @return ���ɹ��򷵻�-1
	*/
	ACE_HANDLE getSock() const;

protected:
	/**
	* �̳���ISvcManager
	*/
	virtual int OnSvcOpen(void* arg);
	virtual int OnPacketReceived(void*, unsigned short, const void*, unsigned long);
	virtual int OnSvcClose(void* arg);

	/**
	* �̳���ACE_Connector<>
	* @see ACE_Connector<>::make_svc_handler()
	*/
	virtual int make_svc_handler(SVCHANDLER*& sh);
	unsigned long getLasttime(){return lasttime_;}//add by zjj

private:
	/**
	 * @class	TimerHandler
	 * @short	��ʱ�������࣬��ʱ����KeepAlive
	 */
	class TimerHandler : public ITimerEventCallBack
	{
	public:
		TimerHandler(CTCPConnectorTpl<SVCHANDLER>*);
		int start();
		int kill();

	private:
		/* From ITimerEventCallBack */
		virtual int OnTimer(unsigned long timerID, void* arg);

	private:
	//	enum {KEEPALIVE_TIMER_ID = 1, KEEPALIVE_INTERVAL = 5000,CLIENT_TIMEOUT_INTERVAL = 120};
		enum {KEEPALIVE_TIMER_ID = 1, KEEPALIVE_INTERVAL = 60000,CLIENT_TIMEOUT_INTERVAL = 300}; //modif 2013-4-3
		CTimer		keepAliveTimer_;
		CTCPConnectorTpl<SVCHANDLER>*	pConnector;
	};

	int create(const ACE_INET_Addr&, ISockEventCallBack* pSockEventCallBack);
	typedef ACE_Connector<SVCHANDLER,ACE_SOCK_CONNECTOR> PARENT;

	ISockEventCallBack*	pSockEventCallBack_;
	SVCHANDLER		handler_;
	TimerHandler		timerHandler_;
	unsigned long	lasttime_; //add by zjj 2006-6-28

	friend class TimerHandler;
};

#include "../TCPConnector.cc"

typedef CTCPConnectorTpl<CTCPSvcHandler>	CTCPConnector;

#endif	/*__TCPCONNECTOR_H__*/

