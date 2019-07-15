
//#pragma once
#ifndef	__BASESTATE_H__
#define	__BASESTATE_H__

#include <time.h>


class CBaseState
{
public:
	//CBaseState(unsigned short nResult = 0,unsigned short maxTimes = 5);
	CBaseState(unsigned short nResult = 0,unsigned short maxTimes = 4); //modify 2013-5-10
	virtual ~CBaseState();

	unsigned long getAddTime(){return m_addTime_ ;}
	void setAddTime();
	unsigned short getMaxSendTimes(){return m_wMaxSendTimes;}
	unsigned short getSendTimes(){return m_wSendTimes;}
	void addSendTimes(){ m_wSendTimes++;}

	void clear();
	virtual int startObject(void * obj){return 0;}
	//virtual void SendData(void * obj){return ;}	
	virtual int SendData(void * obj){return 0;}	//modif 2013-2-27

	virtual int State()const {return 0;}

	unsigned short m_nResult;
private:
	unsigned long m_addTime_;
	unsigned short m_wSendTimes;     //若超时重发次数
	unsigned short m_wMaxSendTimes;  //若超时最多发送次数	
};

#endif



