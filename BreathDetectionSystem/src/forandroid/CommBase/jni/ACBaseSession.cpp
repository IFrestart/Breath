// ACBaseSession.cpp: implementation of the CACBaseSession class.
//
//////////////////////////////////////////////////////////////////////

#include "ACBaseSession.h"
#include "ACCommonResource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

CACBaseSession::CACBaseSession()
{
	m_bTerminated = FALSE;
}

CACBaseSession::~CACBaseSession()
{
	m_ACSocket.Close();
}

void CACBaseSession::TerminateSession()
{
	m_bTerminated=TRUE;
}

BOOL CACBaseSession::IsTerminated()
{
	return m_bTerminated;
}

CACSocket* CACBaseSession::GetConnect()
{
	return &m_ACSocket;
}

int CACBaseSession::InitializeSocket(short nProType, 
	LPCTSTR lpLocalHost, USHORT nLocalPort)
{	
	if (m_ACSocket.Create(nProType, lpLocalHost, nLocalPort) 
		== INVALID_SOCKET)
	{
		return -1;
	}

	return 0;
}

int CACBaseSession::InitializeMulSocket(short nProType,LPCTSTR lpRemoteHost,USHORT nRemotePort,
									 LPCTSTR lpLocalHost,USHORT nLocalPort)
{
	SOCKET nSocket = INVALID_SOCKET;
	nSocket = m_ACSocket.CreateMulPoint(nProType,lpRemoteHost,nRemotePort,lpLocalHost,nLocalPort);
	if(nSocket == INVALID_SOCKET || nSocket == SOCKET_ERROR)
	{
		return -1;
	}
	return 0;
}

void CACBaseSession::AttachSocket(SOCKET hSocket)
{
	m_ACSocket.Attach(hSocket);
	m_bTerminated = FALSE;
}

void CACBaseSession::CloseConnect()
{
	m_ACSocket.Close();
}
