/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����TCP��,�ṩ����TCP�Ľ���,�ر�,�������ݵȲ���

 * @file	TCPAcceptor.h
 * @short	����TCP��
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPMTAcceptorTpl
 * @short	����TCP��
 **/

#ifndef	__TCPMTACCEPTOR_H__
#define	__TCPMTACCEPTOR_H__

#include <list>
#include <queue>

#include "ace/SOCK_Acceptor.h"
#include "ace/Acceptor.h"

#include "Timer.h"
#include "SvcManager.h"
#include "SockEventCallBack.h"
#include "TCPSvcHandler.h"

template <class SVCHANDLER>
class CTCPMTAcceptorTpl : public ISvcManager, public ACE_Acceptor<SVCHANDLER, ACE_SOCK_ACCEPTOR>
{
public:
	CTCPMTAcceptorTpl(ISockEventCallBack* pSockEventCallBack = 0);
	/**
	* ʵ��Ϊ��,���Զ�����ACE_Acceptor������,
	* ��������handle_close() 
	*/
	~CTCPMTAcceptorTpl();

	/**
	* ����һ��TCP socket
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* ����һ��TCP socket
	* @param ipstr socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack = 0);

	/**
	* ����һ��TCP socket
	* @param ip socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* �رռ�����socket������acceptor������socket
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* �رռ�����socket
	* @return �ɹ��򷵻�0
	*/
	int closeListen();

	/**
	* �ر�ָ����acceptor������socket
	* @param sock Ҫ�رյ�socketֵ
	* @return �ɹ��򷵻�0
	*/
	int close(ACE_HANDLE sock);

	/**
	* ȡ��TCP socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪTCP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* ͨ��TCP socket��������
	* @param sock �����ݷ��͵��ĸ�sock,���sockӦ������
	* ���TCPAcceptor����accept()������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len);

protected:
	/**
	* �̳���ISvcManager
	*/
	virtual int OnSvcOpen(void*);
	virtual int OnPacketReceived(void*, unsigned short, const void*, unsigned long);
	virtual int OnSvcClose(void*);

	/**
	* �̳���ACE_Acceptor<>
	* @see ACE_Acceptor<>::make_svc_handler()
	*/
	virtual int make_svc_handler(SVCHANDLER*& sh);

private:
	/**
	 * @class	TimerHandler
	 * @short	��ʱ�������࣬��ʱ������ӣ���һ��ʱ��û���յ����ݵ����ӹر�
	 */
	class TimerHandler : public ITimerEventCallBack
	{
	public:
		TimerHandler(CTCPMTAcceptorTpl<SVCHANDLER>*);
		int start();
		int kill();

	private:
		/* From ITimerEventCallBack */
		virtual int OnTimer(unsigned long timerID, void* arg);

	private:
		//enum {CHECK_TIMER_ID = 1, CHECK_TIMEOUT_INTERVAL = 5000, CLIENT_TIMEOUT_INTERVAL = 30};
		enum {CHECK_TIMER_ID = 1, CHECK_TIMEOUT_INTERVAL = 5000, CLIENT_TIMEOUT_INTERVAL = 120}; //modify 2013-4-3
		CTimer		checkTimer_;
		CTCPMTAcceptorTpl<SVCHANDLER>*	pAcceptor;
	};

	/**
	 * @class	SvcPair
	 * @short	svc_handler�������һ���յ����ݵ�ʱ�� ��
	 */
	struct SvcPair{
		SvcPair(SVCHANDLER*, unsigned long);
		bool operator== (ACE_HANDLE);

		SVCHANDLER*	handler_;
		unsigned long	lasttime_;
	};
	typedef typename std::list<SvcPair>::iterator	svcpair_it;
	typedef ACE_Acceptor<SVCHANDLER, ACE_SOCK_ACCEPTOR> PARENT;

	int create(const ACE_INET_Addr& addr, ISockEventCallBack* pSockEventCallBack);

	ISockEventCallBack*	pSockEventCallBack_;
	std::list<SvcPair>	svcpairList_;
	TimerHandler		timerHandler_;

	enum {SVC_HANDLER_NUM = 1000};
	SVCHANDLER		svcHandlerList_[SVC_HANDLER_NUM];
	std::queue<int>		unusedQueue_;
	
	friend class TimerHandler;
public:
	CMutex			mutex_;
};

#include "../TCPMTAcceptor.cc"
typedef CTCPMTAcceptorTpl<CTCPSvcHandler>	CTCPMTAcceptor;
#endif	/*__TCPMTACCEPTOR_H__*/

