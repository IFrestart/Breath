#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef __LINUX__
#include "LinuxDef.h"
#endif

class Event
{
public:
	Event(BOOL bManulReset = FALSE, BOOL bInitialState = FALSE);
	~Event();
public:
	int waitEvent(unsigned long lockTimer); //unit is milliscond
	int setEvent();
	int resetEvent();
private:
	HANDLE m_e;
};

#endif //__EVENT_H__
