// ACBaseMainProcClass.h: interface for the CACBaseMainProcClass class.
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACBASEMAINPROCCLASS_H__
#define __ACBASEMAINPROCCLASS_H__

#include <assert.h>
#include "BaseACObject.h"

class CACBaseMainProc  
{
public:
	virtual ~CACBaseMainProc(void);

public:
	virtual void Initialize(short nCommType, short nThreadNum, short nMaxConn,
		IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc);

	virtual int BeginWork(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort, //远程端口
		BOOL bMulPoint //多播
		)=0;

	virtual int BeginWork(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort, //远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)=0;

	virtual int BeginWorkHttp(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort, //远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)=0;

	virtual int CloseConnection(SOCKET hSocket = INVALID_SOCKET)=0;

	virtual int SendCommUDPData(WORD wPduType,
		const BYTE* lpBuf, //要发送的数据(不能为NULL)
		DWORD dwSize,//要发送的字节数(>0)
		BOOL bSync, //发送方式(可为TRUE-同步发送,FALSE-异步发送)
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort, //远程端口
		short nPackID,
		BOOL bSock5Flag
		)
	{
		assert(!"CACBaseMainProc::SendCommUDPData");
		return 0;
	};

	virtual int SendCommTCPData(WORD wPduType,
		const BYTE* lpData, DWORD dwSize, BOOL bSync,
		SOCKET hSocket = INVALID_SOCKET
		)
	{
		assert(!"CACBaseMainProc::SendCommTCPData");
		return 0;
	}

	virtual int StopMainProc(void)
	{
		return 0;
	}

protected:
	CACBaseMainProc(void);

	short m_nCommType;
	short m_nThreadNum;
	short m_nMaxConn;

	IACDataCallBack* m_pDataCallBack;
	IACPackHead* m_pHeadProc;
};
#endif
