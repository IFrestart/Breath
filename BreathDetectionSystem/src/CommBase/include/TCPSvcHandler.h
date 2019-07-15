/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP连接数据接收与发送处理类,提供了TCP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	TCPSvcHandler.h
 * @short	TCP连接数据接收与发送处理类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CTCPSvcHandlerTpl
 * @short	TCP连接数据接收与发送处理类
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
	* 打开TCP连接,继承自ACE_Svc_Handler<>,由CTCPConnector或CTCPAcceptor类调用
	* @return 成功则返回0
	* @see ACE_Svc_Handler<>::open(),CTCPConnector<>::open(),CTCPAcceptor<>::open()
	*/
	virtual int open(void* args = 0);

	/**
	* 继承自ACE_Event_Handler,在有数据到达时主动去读,
	* 当收到一个完整的报文时,回调
	* @see ACE_Event_Handler::handle_input()
	*/
	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);

	/**
	* 继承自ACE_Event_Handler,在有数据到达时主动去读,
	* 当收到一个完整的报文时,回调
	* @see ACE_Event_Handler::handle_output()
	*/
	virtual int handle_output(ACE_HANDLE h = ACE_INVALID_HANDLE);

	/**
	* 继承自ACE_Event_Handler,在socket出错时,回调
	* @see ACE_Event_Handler::handle_close()
	*/
	virtual int handle_close(ACE_HANDLE h = ACE_INVALID_HANDLE, 
						ACE_Reactor_Mask m = ACE_Event_Handler::ALL_EVENTS_MASK);
	virtual void destroy (void);

	/**
	* 通过TCP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	void initialize(ISvcManager* psm);

private:
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_NULL_SYNCH> PARENT;

	enum { READMASK = 0x01, WRITEMASK = 0x02 };
	enum { HIGWATERMARK = 16*1024*1024 };
	unsigned short		mask_;

	ISvcManager*		pSvcManager_;	/**< 有连接到达,报文接收到或出错时回调的接口*/
	SOCKHEADER		sockHeaderCallBack_;	/**< 处理TCP协议头回调的接口*/

	bool			isBegin_;	/**< 是否开始接收下一个报文*/
	bool			isHeader_;	/**< 是否开始接收报文头*/
	CSockBlock*		mbRecv_;	/**< 接收报文的缓冲区*/
	unsigned short		pduType_;	/**< 要接收报文的PDU类型*/
	unsigned long		wantLen_;	/**< 正在接收的报文还要接收多少字节的数据*/

	CSockBlock*		mbSend_;	/**< 发送报文的缓冲区*/
	CMutex			mutex_;
	CBlockQueue		blockQueue_;
};

#include "../TCPSvcHandler.cc"
typedef CTCPSvcHandlerTpl<CSockHeader> CTCPSvcHandler;

typedef CTCPSvcHandlerTpl<CRECSockHeader> CTCPRECSvcHandler;

#endif	/*__TCPSVCHANDLER_H__*/

