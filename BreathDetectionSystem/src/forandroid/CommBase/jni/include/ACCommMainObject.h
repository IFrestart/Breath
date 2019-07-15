// ACCommMainObject.h: interface for the CACCommMainObject class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once

#ifndef __ACCOMMMAINOBJECT_H__
#define __ACCOMMMAINOBJECT_H__

#include "BaseACObject.h"
#include "ACBaseMainProcClass.h"

class CACMainObject: public IACMainObject
{

public:
	CACMainObject(void);
	virtual ~CACMainObject(void);

	void Initialize(short nCommType, short nProtocolType, short nThreadNum, 
		short nMaxConn, ISockEventCallBack * lpDataCb, ISockHeaderCallBack* lpHeadProc);
	void InitHttp(ISockEventCallBack * lpDataCb, ISockHeaderCallBack* lpHeadProc);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint);
	int BeginWork(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		);

	int BeginWorkHttp(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		);

	void CloseConnection(SOCKET hSocket = INVALID_SOCKET);

	int SendCommUDPData(WORD wPduType,
		const BYTE* lpData, //要发送的数据(不能为NULL)
		DWORD dwSize,//要发送的字节数(>0)
		BOOL bSync, //发送方式(可为TRUE-同步发送,FALSE-异步发送)
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort, //远程端口
		WORD nPackID,
		BOOL bSock5Flag
		);

	int SendCommTCPData(WORD wPduType, const BYTE* lpData, DWORD dwSize,
		BOOL bSync, SOCKET hSocket=INVALID_SOCKET);

private:
	short m_nProtocolType;
	CACBaseMainProc* m_pMainProc; //单个对象
};
#endif
