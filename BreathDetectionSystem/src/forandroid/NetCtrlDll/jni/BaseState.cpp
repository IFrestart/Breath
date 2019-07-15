//#include "stdafx.h"
#include "BaseState.h"

CBaseState::CBaseState(unsigned short nResult,unsigned short maxTimes) 
	: m_nResult(nResult),m_addTime_(0),m_wMaxSendTimes(maxTimes),m_wSendTimes(0)
{
	clear();
}


CBaseState::~CBaseState()
{
}

void CBaseState::clear()
{
	m_nResult = 0;
	m_addTime_ = 0;
	m_wSendTimes = 0;	
}

void CBaseState::setAddTime()
{
	if (0 == m_addTime_)
	{
		m_addTime_ = time(0);
	}
	m_nResult = 0;
}