/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP连接数据接收与发送处理类,提供了UDP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	 UDPConnector.h
 * @short 	UDP连接类
 * @author	 zhoujj <zhoujj@webcon.com.cn>

 * @class	CUDPConnector
 * @short 	UDP连接类
 **/

#ifndef	__UDPCONNECTOR_H__
#define	__UDPCONNECTOR_H__

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

// 包装MyAcDll中UDP和TCP连接套接字
class CUDPConnector
{
public:
	/** 构造函数
	*/
	CUDPConnector(ISockEventCallBack* pCallBack);

	/** 析构函数
	*/
	~CUDPConnector(void);

	/**
	* 创建一个UDP socket
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @return 成功则返回0
	*/
	int create(unsigned short port);

	/**
	* 创建一个UDP socket
	* @param ipstr socket所使用的IP地址,若为0,则绑定所有的IP地址
	* @param port socket所使用的端口号,若为0,则由系统自动选择一个
	* @return 成功则返回0
	*/
	int create(const char* ipstr, unsigned short port);

	/**
	* 关闭UDP socket
	* @return 成功则返回0
	*/
	int close();

	/**
	* 通过UDP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param buf socket要发送的数据
	* @param len socket要发送数据的长度
	* @param ip socket将数据发送到哪个IP
	* @param port socket将数据发送到哪个PORT
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, 
						const void* buf, unsigned long len,
						unsigned long ip, unsigned short port);

	/**
	* 通过UDP socket发送数据
	* @param pdutype socket要发送数据的PDU类型
	* @param buf socket要发送的数据
	* @param len socket要发送数据的长度
	* @param ipstr socket将数据发送到哪个IP
	* @param port socket将数据发送到哪个PORT
	* @return 成功则返回0
	*/
	int sendPacket(unsigned short pdutype, 
						const void* buf, unsigned long len,
						const char* ipstr, unsigned short port);

	/**
	* 取得UDP socket,若出错则返回ACE_INVALID_HANDLE
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为UDP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* 判断UDP socket是否已经关闭
	* @return 若已经关闭则返回true,否则返回false
	*/
	bool isClosed() const;
	/** 停止通信对象
	*/
	void Stop(int nType=0);

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

};

#endif	/*__UDPCONNECTOR_H__*/

