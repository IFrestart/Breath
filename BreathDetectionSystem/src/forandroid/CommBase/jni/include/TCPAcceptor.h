/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPAcceptor.h
 * @short	被动TCP类
 * @author	zhoujj <zhoujj@webcon.com.cn>

 * @class	CTCPAcceptor
 * @short	被动TCP类
 **/

#ifndef	__TCPACCEPTOR_H__
#define	__TCPACCEPTOR_H__

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

#include "SockEventCallBack.h"

//类功能描述:MyAcDll 侦听套接字包装类
class CTCPAcceptor
{
public:
	/** 构造函数
	*/
	//CTCPAcceptor(CTCPAcceptorCallBack* pCallBack);
	CTCPAcceptor(ISockEventCallBack* pCallBack);

	/** 析构函数
	*/
	~CTCPAcceptor(void);

	/**
	* 创建一个TCP socket
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 创建一个TCP socket
	* @param ipstr socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack = 0);

	/**
	* 创建一个TCP socket
	* @param ip socket所使用的IP,若为0,则系统会绑定所有的IP
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 关闭监听的socket及所有acceptor出来的socket
	* @return 成功则返回0
	*/
	int close();

	/**
	* 关闭监听的socket
	* @return 成功则返回0
	*/
	int closeListen();

	/**
	* 关闭指定的acceptor出来的socket
	* @param sock 要关闭的socket值
	* @return 成功则返回0
	*/
	int close(ACE_HANDLE sock);

	/**
	* 取得TCP socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为TCP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* 通过TCP socket发送数据
	* @param sock 将数据发送到哪个sock,这个sock应该是由
	* 这个TCPAcceptor对象accept()出来的
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
	int sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len);
#if 0
	/** 启动通信对象
	* @param lpLocalHost 本机IP
	* @param nLocalPort 本机端口
	* @param lpMainObj 通信层入口类的指针
	* @return 0-成功, <0失败
	*/
	int Start(USHORT nLocalPort, LPCTSTR lpLocalHost);


	/** 发送数据
	* @param wPduType 数据包的PDU类别
	* @param lpData 要发送的数据
	* @param dwSize 数据长度
	* @param hSocket 发送数据的套接字句柄
	*/
	int SendCommData(WORD wPduType, const BYTE* lpData, DWORD dwSize, SOCKET hSocket);
#endif
	/** 停止侦听对象
	*/
	void Stop(void);
	
	/** 关闭通信连接
	* @param hSocket 要关闭的会话的套接字句柄
	*/
	void CloseConnection(SOCKET hSocket);

	void setHeadFlag(bool flag) {m_bHeadFlag = flag;}//add 2012-10-18

private:
	IACMainObject* m_pSvrObj; ///< MyAcDll通信对象接口
	ISockEventCallBack* m_pCallBack; ///< 数据回调接口实现对象
	//CACPackHead* m_pHeadInfo; ///< 数据包头处理对象
	CSockHeader* m_pHeadInfo; ///< 数据包头处理对象
	bool m_bHeadFlag ; //add 2012-10-18

};

#endif	/*__TCPACCEPTOR_H__*/

