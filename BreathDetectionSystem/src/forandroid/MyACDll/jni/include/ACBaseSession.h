// ACBaseSession.h: interface for the CACBaseSession class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACBASESESSION_H__
#define __ACBASESESSION_H__

#include "ACSocket.h"

class CACBaseSession  
{
public:
	virtual ~CACBaseSession(void);

	void CloseConnect(void);
	void TerminateSession(void);
	BOOL IsTerminated(void);
	CACSocket* GetConnect(void);
	void AttachSocket(SOCKET hSocket);	

protected:
	CACBaseSession(void);

	int InitializeSocket(short nProType, LPCTSTR lpLocalHost = NULL,  
		USHORT nLocalPort = 0);

	int InitializeMulSocket(short nProType,LPCTSTR lpRemoteHost,USHORT nRemotePort,
		LPCTSTR lpLocalHost = NULL, USHORT nLocalPort = 0);

protected:
	CACSocket m_ACSocket;

private:
	BOOL m_bTerminated;
};
#endif
