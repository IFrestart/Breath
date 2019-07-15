// ACUDPSession.h: interface for the CACUDPSession class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACUDPSESSION_H__
#define __ACUDPSESSION_H__

#include "ACBaseSession.h"
#include "SockEventCallBack.h"
#include "SockHeaderCallBack.h"

class CACSocket;
class CACUDPSession: public CACBaseSession
{
public:
	CACUDPSession(void);
	virtual ~CACUDPSession(void);

public:
	int InitializeSession(ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc, 
		LPCTSTR lpLocalHost = NULL, USHORT nLocalPort = 0);

	int InitializeSession(ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort);
	int SendCommData(WORD wPduType, const BYTE* lpBuf, DWORD dwSize, 
		BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
		short nPackID, BOOL bSock5Flag);

	int StartSessionMainLoop(void);
	int StopSessionMainLoop(void);

	int InitializeSession(ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc, 
		LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpProxyHost, 
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);

private:
	int ConnectToProxy(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpProxyHost,
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword,
		DWORD dwTimeOut);
	int SendByProxy(const BYTE* lpBuf, DWORD dwSize, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort);
	int RecvFromProxy(BYTE* lpBuf, DWORD dwSize, char *lpRemoteHost, USHORT* lpRemotePort);

	int BuildSock5Head(LPCTSTR lpRemoteHost, USHORT nRemotePort, BYTE* lpHeadBuf, 
		int nBufLen);
	int ConsultProcess(const BYTE* lpSendBuf, DWORD dwSendLen, BYTE* lpRecvBuf, 
		DWORD* lpRecvLen, UINT nTimeOut);
	int SessionMainLoop(void);

#ifdef _WIN32
	static DWORD WINAPI RecvThreadHelper(LPVOID lpParameter);
#else
	static void* RecvThreadHelper(LPVOID lpParameter);
	long 	GetCurrentStopFlag();
	pthread_mutex_t m_mutexStopFlag;
#endif

	ISockEventCallBack* m_pDataCallBack;
	ISockHeaderCallBack* m_pHeadProc;
    
	long m_nStopFlag;
public:
#ifdef _WIN32
	HANDLE m_hRecvThread;
#else
	pthread_t m_hRecvThread;
#endif

	CACSocket* m_pSocks5;

	char m_szProxyHost[16];
	USHORT m_nProxyPort;
};
#endif
