// BaseACObject.h: MyAcDll调用接口头文件.

//#pragma once
#ifndef __BASEACOBJECT_H__
#define __BASEACOBJECT_H__

#include <vector>
#include "LinuxDef.h"
//#include "CommHead.h"
#include "SockHeaderCallBack.h"
#include "SockEventCallBack.h"

const short COMMTYPE_NULL = 0;
const short COMMTYPE_SERVER = 1;
const short COMMTYPE_CLIENT = 2;

typedef struct tagThreadInfo
{
	DWORD dwThreadId;
#ifdef _WIN32
	HANDLE hThread;
#else
	pthread_t hThread;
#endif
	USHORT nPort;
}ThreadInfo;

typedef std::vector<ThreadInfo> ThreadArray;

//extern ThreadArray g_thredTable;
//extern long g_nThreadCount;
#if 0
class IACDataCallBack
{
public:
	/** 收到一个完整数据包时的回调函数
	* @param wProtocolType 协议类型(为1时为TCP,为2时为UDP,其它无效)
	* @param hSocket 接收数据的套接字
	* @param lpData 收到的数据
	* @param dwSize 数据长度
	* @param lpRemoteHost 发送数据的IP(不能为NULL)
	* @param nRemotePort 发送数据的端口
	* @param wPduType 数据包的PDU类型
	* @param wExtParam 扩展参数
	*/
	virtual int OnDataReceived(WORD wProtocolType, SOCKET hSocket, const BYTE* lpData,
		DWORD dwSize, LPCTSTR lpRemoteHost, USHORT nRemotePort, WORD wPduType,
		WORD wExtParam)=0;

	/** 收到一块不完整的TCP数据时的回调函数
	* @param hSocket 接收数据的套接字
	* @param dwArrivedSize 已接收到的数据长度(不包括包头)
	* @param dwTotoalSize 要接收的数据总长度(不包括包头)
	*/
	virtual int OnBlockDataArriving(SOCKET hSocket, DWORD dwArrivedSize, 
		DWORD dwTotoalSize)=0;

	/** 客户端连接上服务器或服务器接收到客户端连接时的回调函数
	* @param hSocket-套接字句柄
	* @param lpLocalHost 本地主机IP(不能为NULL)
	* @param nLocalPort 本地端口(>0)
	* @param lpLocalMask 本地子网掩码(不能为NULL)
	* @param lpRemoteHost 远程主机IP(不能为NULL)
	* @param nRemotePort 远程端口(>0)
	*/
	virtual int OnNewConnection(SOCKET hSocket, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpLocalMask, LPCTSTR lpRemoteHost, USHORT nRemotePort)=0;

	/** TCP连接中，对方关闭时的回调函数
	* @param hSocket 套接字句柄
	*/
	virtual int OnConnectionClosed(SOCKET hSocket)=0; 
};
#endif
//通信对象接口
class IACMainObject
{
public:
	/** 初始化通信对象
	* @param nCommType-通信对象类型(可为2-客户端,1-服务器,其它无效)
	* @param nProtocolType-通信协议(可为1-TCP协议,2-UDP协议,其它无效)
	* @param nThreadNum-通信处理线程数(nCommType为2时忽略,为1时必须>0)
	* @param nMaxConn-能接受的最大连接数(nCommType为2时忽略,为1时必须>0)
	* @param lpACCommModuleCallBack-接收数据的回调接口(不能为NULL)
	* @param lpACCommonInfoOfModule-数据包头分析回调接口(不能为NULL)
	*/
	virtual void Initialize(short nCommType, short nProtocolType, short nThreadNum, 
		short nMaxConn, ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc)=0;

	/** 初始化HTTP通信对象
	* @param lpACCommModuleCallBack-接收数据的回调接口(不能为NULL)
	* @param lpACCommonInfoOfModule-数据包头分析回调接口(不能为NULL)
	*/
	virtual void InitHttp(ISockEventCallBack * lpDataCallBack, ISockHeaderCallBack* lpHeadProc)=0;

	/** 启动AC对象
	* @param lpLocalHost-本机IP地址(为空时由系统计算本机IP)
	* @param nLocalPort-本地端口(为0时由系统计选择本地端口)
	* @param lpRemoteHost-远程IP(为客户端时不能为NULL,为服务器时为NULL)
	* @param nRemotePort-远程端口(为客户端时>0, 为服务器时为0)
	* @param bMulPoint 是否组播 FALSE 否，TRUE 是。
	* @return 0-成功,其它失败
	*/
	virtual int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, BOOL bMulPoint=FALSE)=0;

	/** 启动AC对象 tcp/udp代理
	* @param lpLocalHost-本机IP地址(为空时由系统计算本机IP)
	* @param nLocalPort-本地端口(为0时由系统计选择本地端口)
	* @param lpRemoteHost-远程IP(为客户端时不能为NULL,为服务器时为NULL)
	* @param nRemotePort-远程端口(为客户端时>0, 为服务器时为0)
	* @return 0-成功,其它失败
	*/
	virtual int BeginWork(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)=0;

	/** 启动AC对象 http代理
	* @param lpLocalHost-本机IP地址(为空时由系统计算本机IP)
	* @param nLocalPort-本地端口(为0时由系统计选择本地端口)
	* @param lpRemoteHost-远程IP(为客户端时不能为NULL,为服务器时为NULL)
	* @param nRemotePort-远程端口(为客户端时>0, 为服务器时为0)
	* @return 0-成功,其它失败
	*/
	virtual int BeginWorkHttp(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)=0;

	/** 关闭TCP连接
	* @param hSocket-套接字
	*/
	virtual void CloseConnection(SOCKET hSocket = INVALID_SOCKET)=0;

	/** 发送UDP数据包
	* @param wPduType PDU类型
	* @param lpData 包体数据
	* @param dwSize 数据长度
	* @param bSync 发送方式(可为TRUE-同步发送,FALSE-异步发送)
	* @param lpszRemoteAddr 目标地址(不能为空)
	* @param nRemotePort 目标端口(必须大于零)
	* @param wExtParam 扩展参数
	* @return 0-成功,其它失败
	*/
	virtual int SendCommUDPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, WORD wExtParam, BOOL bSock5Flag
		)=0;

	/** 发送TCP数据包
	* @param wPduType PDU类型
	* @param lpData 包体数据
	* @param dwSize 数据长度
	* @param bSync 发送方式(可为TRUE-同步发送,FALSE-异步发送)
	* @param hSocket 发送套接字(为客户端时忽略)
	* @return 0-成功,其它失败
	*/
	virtual int SendCommTCPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, SOCKET hSocket= INVALID_SOCKET)=0;
};

//引出函数
//功能：初始化WinSock
//void WINAPI ACModuleInitialize();

//引出函数
//功能：终止WinSock
//void WINAPI ACModuleUnInitialize();

//引出函数
//功能:创建通信对象
//返回值:NULL-失败, 非NULL-成功
//IACCommMainObject* WINAPI CreateACObject();

//引出函数
//功能:删除通信对象
//参数:pACCommMainObject-要删除的通信对象(不能为NULL)
//返回值:0-成功, 其他-失败
//int WINAPI DestroyACObject(IACCommMainObject* pACCommMainObject);

#endif
