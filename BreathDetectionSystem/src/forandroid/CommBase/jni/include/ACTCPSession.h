// ACTCPSession.h: interface for the CACTCPSession class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACTCPSESSION_H__
#define __ACTCPSESSION_H__

#include "ACBaseSession.h"

class CACTCPSession: public CACBaseSession
{
public:
	CACTCPSession(void);
	~CACTCPSession(void);

public:
	int ReadPkgHead(DWORD dwHeadSize);
	int ReadPkgData(void);

	int GetDateSize(); //add by zjj 2009-5-25
	//int ResetRecvBufData(void); modify by zjj 2009-5-25
	int ResetRecvBufData(BOOL bHeadFlag=TRUE);

	BOOL IsHeadSegment(void);
	void SetHeadSegmentFlag(BOOL bFlag);
	void CloseSessionConnect(void);
	
	void SetPayLoadSize(DWORD dwSize);
	DWORD GetPayLoadSize(void);

	void SetPduType(WORD wPduType);
	WORD GetPduType(void);

	void SetPackID(short nPackID);
	short GetPackID(void);

	const LPBYTE GetHeadData(void) const;
	const LPBYTE GetPayLoadData(void) const;
	DWORD GetRDataBytes(void);
private:
	enum {MAX_RECIVE_BUFFER_LEN = 1024 }; //add by zjj 2009-5-26
private:
	BYTE* m_pPackHead;
	int m_dwRHeadBytes;
	BYTE* m_pPackData;
	int m_dwRDataBytes;
	int m_dwDataSize;

	BOOL m_bIsHeadSegment;
	WORD m_wPduType;
	short m_nPackID;
	
	int m_dwMaxBuffSize ;//add by zjj 2009-5-26
};
#endif
