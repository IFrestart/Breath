/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP连接数据接收与发送处理类,提供了UDP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	UDPSvcHandler.h
 * @short	UDP连接数据接收与发送处理类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CUDPSvcHandlerTpl
 * @short	UDP连接数据接收与发送处理类
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
	* 打开UDP连接
	* @param port UDP socket要绑定的本地端口号
	* @return 成功则返回0
	*/
	int open(unsigned short port);

	/**
	* 打开UDP连接
	* @param ipstr UDP socket要绑定的本地IP地址,若为0,则绑定本地的所有IP
	* @param port UDP socket要绑定的本地端口号
	* @return 成功则返回0
	*/
	int open(const char* ipstr, unsigned short port);

	/**
	* 取得UDP的socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为UDP socket
	*/
	ACE_HANDLE get_handle() const;

	/**
	* 关闭UDP连接
	* @return 成功则返回0
	*/
	int close();

	/**
	* 通过UDP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @param ip socket将数据发送到哪个IP
	* @param port socket将数据发送到哪个PORT
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len, unsigned long ip, unsigned short port);

protected:
	/**
	* 继承自ACE_Event_Handler,在有数据到达时主动去读,
	* 当收到一个完整的报文时,回调
	* @see ACE_Event_Handler::handle_input()
	*/
	virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);

	/**
	* 继承自ACE_Event_Handler,在socket出错时,回调
	* @see ACE_Event_Handler::handle_close()
	*/
	virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);

private:
	typedef ACE_Event_Handler PARENT;
	enum { MAXSOCKBUFFERLEN = 64*1024 };	/**< 最大的数据包大小*/

	ISockEventCallBack*	pSockEventCallBack_;	/**< 有报文接收到或出错时回调的接口*/
	SOCKHEADER		sockHeaderCallBack_;	/**< 处理UDP协议头回调的接口*/
	ACE_SOCK_Dgram		dgram_;	/**< 包装UDP socket的类,@see ACE_SOCK_Dgram*/
	
	unsigned char		recvBuf_[MAXSOCKBUFFERLEN];	/**< 接收缓冲区*/
	unsigned char		sendBuf_[MAXSOCKBUFFERLEN];	/**< 发送缓冲区*/
};

#include "../UDPSvcHandler.cc"
typedef CUDPSvcHandlerTpl<CSockHeader> CUDPSvcHandler;

#endif	/*__UDPSVCHANDLER_H__*/

