// ACTCPSession.cpp: implementation of the CACTCPSession class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include "ACTCPSession.h"
#include "ACCommonResource.h"

#include <sys/ioctl.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef __DEBUG
#define new DEBUG_NEW
#endif
#include "LogTrace.h"

//void LogTrace(LPCTSTR lpFile, int nLine, LPCTSTR lpLogFile, LPCTSTR lpFormat, ...);

CACTCPSession::CACTCPSession()
{
	m_pPackHead = NULL;
	m_pPackData = NULL;
	m_dwRHeadBytes = 0;
	m_dwRDataBytes = 0;
	m_dwDataSize = 0;

	m_bIsHeadSegment=TRUE;
	m_wPduType = 0;
	m_nPackID = 0;
//begin add by zjj 2009-5-26
	m_dwMaxBuffSize = MAX_RECIVE_BUFFER_LEN;
	m_pPackData = new BYTE[m_dwMaxBuffSize];
//end add by zjj 2009-5-26
}

CACTCPSession::~CACTCPSession()
{
	if (m_pPackHead != NULL)
	{
		delete []m_pPackHead;
		m_pPackHead = NULL;
	}

	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
}

int CACTCPSession::ReadPkgHead(DWORD dwHeadSize)
{
	if (m_pPackHead == NULL)
	{
		m_pPackHead = new BYTE[dwHeadSize];
	}

	int nRecvSize = GetConnect()->Receive((char *)m_pPackHead+m_dwRHeadBytes, 
		dwHeadSize-m_dwRHeadBytes);
	
	if (nRecvSize > 0)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error ReadPkgHead");
		m_dwRHeadBytes += nRecvSize;
	}

	return nRecvSize;
}

int CACTCPSession::ReadPkgData(void)
{
//modif by zjj 2009-5-26
/*
	if (m_pPackData == NULL)
	{
		m_pPackData = new BYTE[m_dwDataSize];
	}
*/
//end modif by zjj 2009-5-26

	int nRecvSize = GetConnect()->Receive((char *)m_pPackData+m_dwRDataBytes, 
		m_dwDataSize-m_dwRDataBytes);
	
	if (nRecvSize > 0)
	{
		//LogTrace(__FILE__,__LINE__,"ReceiveError.txt","recv Socket Error ReadPkgData");
		m_dwRDataBytes += nRecvSize;
	}

	return nRecvSize;
}

int CACTCPSession::GetDateSize() //add by zjj 2009-5-25
{
	m_dwDataSize = 0;
	int nRet = ioctl(GetConnect()->GetSocket(),FIONREAD,&m_dwDataSize);	
	if(nRet < 0)
	{
		//printf("CACTCPSession ioctl error \n");
		return nRet;
	}
	//printf("CACTCPSession after ioctl m_dwDataSize=[%d]\n",m_dwDataSize);
	if(m_dwMaxBuffSize < m_dwDataSize)	
	{
		m_dwMaxBuffSize = m_dwDataSize + 256;
		if (m_pPackData != NULL)
		{
			delete []m_pPackData;
			m_pPackData = NULL;
		}
		m_pPackData = new BYTE[m_dwMaxBuffSize];
		
	}
	return nRet ;
	
}
//int CACTCPSession::ResetRecvBufData() //modify by zjj 2009-5-25
int CACTCPSession::ResetRecvBufData(BOOL bHeadFlag)
{
	m_dwRHeadBytes = 0;
//modif by zjj 2009-5-26
/*
	if (m_pPackData != NULL)
	{
		delete []m_pPackData;
		m_pPackData = NULL;
	}
*/
//end modif by zjj 2009-5-26

	m_dwRDataBytes = 0;
	m_dwDataSize = 0;

	if(bHeadFlag)
		m_bIsHeadSegment = TRUE;
	else
		m_bIsHeadSegment = FALSE;
	
	m_wPduType = 0;
	m_nPackID = 0;

	return 0;
}

void CACTCPSession::SetPayLoadSize(DWORD dwSize)
{
	m_dwDataSize = dwSize;
//add by zjj 2009-5-26
	if(m_dwMaxBuffSize < m_dwDataSize)	
	{
		m_dwMaxBuffSize = m_dwDataSize + 256;
		if (m_pPackData != NULL)
		{
			delete []m_pPackData;
			m_pPackData = NULL;
		}
		m_pPackData = new BYTE[m_dwMaxBuffSize];
		
	}
//end add by zjj 2009-5-26
}

DWORD CACTCPSession::GetPayLoadSize(void)
{
	return m_dwDataSize;
}

void CACTCPSession::SetPduType(WORD wPduType)
{
	m_wPduType = wPduType;
}

USHORT CACTCPSession::GetPduType()
{
	return m_wPduType;
}

void CACTCPSession::SetPackID(short nPackID)
{
	m_nPackID = nPackID;
}

short CACTCPSession::GetPackID(void)
{
	return m_nPackID;
}

BOOL CACTCPSession::IsHeadSegment()
{
	return m_bIsHeadSegment;
}

void CACTCPSession::SetHeadSegmentFlag(BOOL bFlag)
{
	m_bIsHeadSegment = bFlag;
}

void CACTCPSession::CloseSessionConnect()
{
	CloseConnect();
}

const LPBYTE CACTCPSession::GetHeadData(void) const
{
	return m_pPackHead;
}

const LPBYTE CACTCPSession::GetPayLoadData(void) const
{
	return m_pPackData;
}

DWORD CACTCPSession::GetRDataBytes(void)
{
	return m_dwRDataBytes;
}
