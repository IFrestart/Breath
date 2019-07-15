/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 报文管理类,处理保存,超时处理

 * @file	PacketManager.h
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @class	PacketManager
 * @short	报文的管理类，处理应该有回包的报文发出后一段时间内没有返回的情形
 **/

#ifndef	__PACKETMANAGER_H__
#define	__PACKETMANAGER_H__

#include <vector>
#include <functional>
#include <iostream>
#include "Timer.h"
#include <assert.h>
#include "PacketTimeOut.h"
#include <algorithm>
#include "Mutex.h"

class PacketManager : public ITimerEventCallBack
{
private:
	/**
	 * @class	E
	 * @short	数据报文的信息，包括数据、其加入时间等
	 */
	struct E
	{
		E();

		unsigned short	seed_;
		unsigned short	pduType_;
		const char*	data_;
		unsigned short	len_;
		unsigned long	addTime_;
		unsigned short  outTimes_ ;  //add by zjj  2011-2-25  超时次数

		void clear();
		bool unUsed();
		static void dump(const E& e);
	};

	std::vector<E>	pduList_;
	unsigned short	maxNum_;
	unsigned short	insertPos_;
	unsigned short	checkPos_;
	unsigned short	hasNum_;
	typedef std::vector<E>::iterator	e_it;
	typedef std::vector<E>::const_iterator	e_const_it;

	enum {TIMERID = 1, TIMERINTERVAL = 1000};
	unsigned short	timeout_;
	CTimer		timer_;
	CMutex          mutex_;	  //add by zjj 2014-5-7

	IPacketTimeOut*	pCallBack_;

	bool checkid(unsigned long id);
	unsigned long makeid(unsigned short index, unsigned short seed);
	unsigned short getindex(unsigned long id);
	unsigned short getseed(unsigned long id);
	void movepos(unsigned short& pos);

public:
	PacketManager(IPacketTimeOut* pCallBack, unsigned short timeout = 2, unsigned short n = 10000);

	unsigned long get();
	unsigned short getOutTimes(unsigned long id);//add by zjj 2011-2-25
	void set(unsigned long id, unsigned short pdutype, const void* data, unsigned short len);
	void clear(unsigned long id);
	void clear();
	void dump() const;

private:
	virtual int OnTimer(unsigned long timerID, void* arg);
};

#endif	/*__PACKETMANAGER_H__*/

