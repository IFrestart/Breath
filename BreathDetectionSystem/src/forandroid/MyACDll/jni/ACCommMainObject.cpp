// ACCommMainObject.cpp: implementation of the CACCommMainObject class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include <assert.h>

#include "ACCommMainObject.h"
#include "ACTCPMainProcClass.h"
#include "ACUDPMainProcClass.h"
#include "ACTCPClientProc.h"

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

#include "LogTrace.h"
//void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACMainObject::CACMainObject(void)
{
	m_nProtocolType = 0;
	m_pMainProc = NULL;
}

CACMainObject::~CACMainObject(void)
{
	if (m_pMainProc)
	{
		m_pMainProc->StopMainProc();
		delete m_pMainProc;
		m_pMainProc = NULL;
	}
}

void CACMainObject::Initialize(short nCommType, short nProtocolType, short nThreadNum, 
	short nMaxConn, IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc)
{
	assert(nCommType==COMMTYPE_SERVER || nCommType==COMMTYPE_CLIENT);
	assert(nProtocolType==SOCK_STREAM || nProtocolType==SOCK_DGRAM);
	assert(lpDataCallBack != NULL);
	//assert(lpHeadProc != NULL); modify by zjj 2009-5-25

	m_nProtocolType=nProtocolType;

	switch (m_nProtocolType)
	{
		case SOCK_STREAM:
		{
			if (COMMTYPE_CLIENT == nCommType)
			{
				m_pMainProc = (CACBaseMainProc*)new CACTCPClientProc();
			}
			else
			{
				m_pMainProc = (CACBaseMainProc*)new CACTCPMainProc();
			}
			break;
		}
		case SOCK_DGRAM:
		{
			m_pMainProc = (CACBaseMainProc*)new CACUDPMainProc();
			break;
		}
		default:
			break;
	}
	
	m_pMainProc->Initialize(nCommType, nThreadNum, nMaxConn, lpDataCallBack, lpHeadProc);
}

void CACMainObject::InitHttp(IACDataCallBack * lpDataCallBack, IACPackHead* lpHeadProc)
{
	assert(lpDataCallBack != NULL);
	assert(lpHeadProc != NULL);

	m_nProtocolType = SOCK_STREAM;
	CACTCPClientProc* pClientProc = new CACTCPClientProc;
	m_pMainProc = (CACBaseMainProc*)pClientProc;
	m_pMainProc->Initialize(COMMTYPE_CLIENT, 1, 1, lpDataCallBack, lpHeadProc);
	pClientProc->SetHttpFlag();
}

int CACMainObject::BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
	LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint)
{
	assert(m_pMainProc != NULL);

	return m_pMainProc->BeginWork(lpLocalHost, nLocalPort, lpRemoteHost, nRemotePort,bMulPoint);
}

int CACMainObject::BeginWork(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)
{
	assert(m_pMainProc != NULL);

	return m_pMainProc->BeginWork(lpLocalHost, nLocalPort, lpRemoteHost, 
		nRemotePort, pProxyHost, nProxyPort, pProxyUserName, pProxyPassword);
}

int CACMainObject::BeginWorkHttp(LPCTSTR lpLocalHost, //本地IP(不能为NULL)
		USHORT nLocalPort, //本地端口
		LPCTSTR lpRemoteHost, //远程IP(不能为NULL)
		USHORT nRemotePort ,//远程端口
		LPCTSTR pProxyHost, //代理服务器IP地址(无代理时为NULL或空串)
		USHORT nProxyPort, //代理服务器端口
		LPCTSTR pProxyUserName, //代理用户名(不需验证用户名/密码时为NULL或空串)
		LPCTSTR pProxyPassword //代理密码
		)
{
	assert(m_pMainProc != NULL);

	return m_pMainProc->BeginWorkHttp(lpLocalHost, nLocalPort, lpRemoteHost, 
		nRemotePort, pProxyHost,nProxyPort,pProxyUserName,pProxyPassword);
}

void CACMainObject::CloseConnection(SOCKET hSocket)
{
	assert(m_pMainProc != NULL);

	m_pMainProc->CloseConnection(hSocket);
}

int CACMainObject::SendCommUDPData(WORD wPduType, const BYTE* lpData, 
	DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
	WORD nPackID, BOOL bSock5Flag)
{
	assert(m_pMainProc != NULL);

	//::LogTrace(__FILE__, __LINE__, "server.txt", "SendCommUDPData(pdu:%d,size:%d)",
	//	wPduType, dwSize);

	return m_pMainProc->SendCommUDPData(wPduType, lpData, dwSize, bSync, 
		lpRemoteHost, nRemotePort, nPackID, bSock5Flag);
}

int CACMainObject::SendCommTCPData(WORD wPduType, const BYTE* lpData, 
	DWORD dwSize, BOOL bSync, SOCKET hSocket)
{
	assert(m_pMainProc != NULL);

	//::LogTrace(__FILE__, __LINE__, "server.txt", "SendCommTCPData(pdu:%d,size:%d)",
	//	wPduType, dwSize);
	return m_pMainProc->SendCommTCPData(wPduType, lpData, dwSize, 
		bSync, hSocket);
}
