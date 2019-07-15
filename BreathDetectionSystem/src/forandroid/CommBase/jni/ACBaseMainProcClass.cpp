// ACBaseMainProcClass.cpp: implementation of the CACBaseMainProcClass class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "ACBaseMainProcClass.h"

#ifdef __DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACBaseMainProc::CACBaseMainProc(void)
{
	m_nCommType = COMMTYPE_NULL;
	m_nThreadNum = 0;

}

CACBaseMainProc::~CACBaseMainProc(void)
{
	StopMainProc();
}

void CACBaseMainProc::Initialize(short nCommType, short nThreadNum, 
	short nMaxConn, ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc)
{
	assert(nCommType==COMMTYPE_SERVER || nCommType==COMMTYPE_CLIENT);
	assert(lpDataCallBack != NULL);
//	assert(lpHeadProc != NULL); //modify by zjj 2009-5-25

	m_nCommType = nCommType;
	m_nThreadNum = nThreadNum;
	m_nMaxConn = nMaxConn;

	m_pDataCallBack = lpDataCallBack;
	m_pHeadProc = lpHeadProc;
}

