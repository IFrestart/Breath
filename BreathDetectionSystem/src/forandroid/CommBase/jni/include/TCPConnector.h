/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 被动TCP类,提供被动TCP的建立,关闭,发送数据等操作

 * @file	TCPConnector.h
 * @short	被动TCP类
 * @author	zhoujj <zhoujj@webcon.com.cn>

 * @class	CTCPConnector
 * @short	被动TCP类
 **/

#ifndef	__TCPCONNECTOR_H__
#define	__TCPCONNECTOR_H__

#include "Timer.h"

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

// 包装MyAcDll中UDP和TCP连接套接字
class CTCPConnector
{
public:
	/** 构造函数
	*/
	CTCPConnector(ISockEventCallBack* pCallBack=0);

	/** 析构函数
	*/
	~CTCPConnector(void);

	/**
	* 创建一个TCP socket
	* @param ipstr socket要连接到的IP,字符串
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 创建一个TCP socket
	* @param ip socket要连接到的IP
	* @param port socket要连接到的端口号
	* @param pSockEventCallBack 处理TCP事件的回调接口
	* @return 成功则返回0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* 关闭TCP socket
	* @return 成功则返回0
	*/
	int close();

	/**
	* 通过TCP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param data socket要发送的数据
	* @param len socket要发送数据的长度
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	/**
	* 返回TCP socket底层的FD
	* @return 不成功则返回-1
	*/
	ACE_HANDLE getSock() const;
#if 0
	/** 启动通信对象(不使用代理)
	* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost 本机IP
	* @param nLocalPort 本机端口
	* @param lpszSvrAddr 服务器IP
	* @param nSvrPort 服务器端口 	
	* @param bUseHttp 是否使用HTTP协议(当用HTTP代理或HTTP协议登陆后,
	建立点对点连接时,如双方在同一局域网,则为FALSE,其它情况为TRUE)	
	* @param bMulPoint 是否组播
	* @return 0-成功, <0失败
	*/
	int Start(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, BOOL bUseHttp,BOOL bMulPoint=FALSE);

	/** 启动通信对象(使用SOCK5代理)
	* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost 本机IP
	* @param nLocalPort 本机端口
	* @param lpszSvrAddr 服务器IP
	* @param nSvrPort 服务器端口 	
	* @param lpProxyAddr 代理服务器IP 
	* @param nProxyPort 代理服务器端口
	* @param lpProxyUserName 代理用户名
	* @param lpProxyPassword 代理密码
	* @return 0-成功, <0失败
	*/
	int StartSock5(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, LPCTSTR lpProxyAddr, 
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);

	/** 启动通信对象(使用HTTP代理)
	* @param wProtoType 通信对象类型(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost 本机IP
	* @param nLocalPort 本机端口
	* @param lpszSvrAddr 服务器IP
	* @param nSvrPort 服务器端口 	
	* @param lpProxyAddr 代理服务器IP 
	* @param nProxyPort 代理服务器端口
	* @param lpProxyUserName 代理用户名
	* @param lpProxyPassword 代理密码
	* @return 0-成功, <0失败
	*/
	int StartHttp(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, LPCTSTR lpProxyAddr, 
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);


	/** UDP通信对象发送数据
	* @param wPduType 数据包的PDU类别
	* @param lpData 要发送的数据
	* @param dwSize 数据长度
	* @param bSync 是否同步发送(目前只支持同步发送)
	* @param lpRemoteHost 接收方的IP
	* @param nRemotePort 接收方的端口
	* @param wExtParam 扩展参数(用于程序调试)
	* @param bSock5Flag	如果用SOCK5登陆到服务器,点对点连接间发送数据是否加
	SOCK5头(当双方在同一局域网时,应为FALSE)
	*/
	int SendCommUDPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
		WORD wExtParam, BOOL bSock5Flag);
	/** TCP通信对象发送数据
	* @param wPduType 数据包的PDU类别
	* @param lpData 要发送的数据
	* @param dwSize 数据长度
	* @param bSync 是否同步发送(目前只支持同步发送)
	* @param hSocket 本参数不使用
	*/
	int SendCommTCPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, SOCKET hSocket= INVALID_SOCKET);

#endif	
	/** 停止通信对象
	*/
	void Stop(int nType=0);

	///** 获取本机IP
	//*/
	//LPCTSTR GetLocalHost(void) const;

	///** 获取本机子网掩码
	//*/
	//LPCTSTR GetLocalMask(void) const;

	///** 获取本地端口
	//*/
	//USHORT GetLocalPort(void);

	///** 获取通信对象的套接字句柄
	//*/
	//SOCKET GetSocket(void);

	/*关闭Socket
	*	
	*/
//	void CloseConnect();
	//void CloseConnect(SOCKET hSocket);
	void setHeadFlag(bool flag) {m_bHeadFlag = flag;}//add 2012-10-18
	int GetExitType();

private:
	/** 删除通信对象(不用线程删除)
	*/
	void FreeObject(void);

	/** 停止通信对象实现线程
	*/
#ifdef _WIN32
	static DWORD WINAPI StopThreadHelper(LPVOID lpParam);
	static DWORD WINAPI StopMyselfThreadHelper(LPVOID lpParam);
#else
	static void* StopThreadHelper(LPVOID lpParam);
	static void* StopMyselfThreadHelper(LPVOID lpParam);
#endif

	IACMainObject* m_pAcCommObj; ///< MyAcDll通信对象接口
	ISockEventCallBack* m_pCallBack; ///< 数据回调接口实现对象
	//CACPackHead* m_pHeadInfo; ///< 数据包头处理对象
	CSockHeader* m_pHeadInfo; ///< 数据包头处理对象

	int m_nExitType;
	bool 		m_bHeadFlag; //add 2012-10-18

};

#endif	/*__TCPCONNECTOR_H__*/

