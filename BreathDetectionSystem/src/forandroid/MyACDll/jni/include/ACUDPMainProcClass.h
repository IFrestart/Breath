// ACUDPMainProcClass.h: interface for the CACUDPMainProcClass class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACUDPMAINPROCCLASS_H__
#define __ACUDPMAINPROCCLASS_H__

#include "ACUDPSession.h"
#include "ACBaseMainProcClass.h"

class CACUDPMainProc: public CACBaseMainProc
{
public:
	CACUDPMainProc(void);
	virtual ~CACUDPMainProc(void);

public:
	//Inherit from CACBaseMainProcClass
	void Initialize(short nCommType, short nThreadNum, short nMaxConn,
		IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort,
		LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,
		LPCTSTR lpProxyHost, USHORT nProxyPort,
		LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);

	int BeginWorkHttp(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,
		LPCTSTR lpProxyHost, USHORT nProxyPort,
		LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword){return 0;}

	int SendCommUDPData(WORD wPduType, const BYTE* lpBuf, 
		DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, short nPackID, BOOL bSock5Flag);

	int CloseConnection(SOCKET hSocket=INVALID_SOCKET);
	int StopMainProc(void);

private:
	CACUDPSession* m_pACUDPSession;
};
#endif
