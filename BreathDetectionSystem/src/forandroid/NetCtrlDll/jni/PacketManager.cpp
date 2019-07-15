/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 *

 * @file	PacketManager.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short
 **/

#include "PacketManager.h"

PacketManager::E::E()
	: seed_(0), addTime_(0), pduType_(0), data_(0), len_(0)
{
}

void PacketManager::E::clear()
{
	pduType_ = 0;
	if (data_)  //add 2011-7-8
	{
		delete[] data_;
		data_ = 0;
	}	
	len_ = 0;
	addTime_ = 0;
}

bool PacketManager::E::unUsed()
{
	return 0 == addTime_;
}

void PacketManager::E::dump(const E& e)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
	std::cerr << "seed_ = " << e.seed_ << std::endl
		<< "pduType_ = " << e.pduType_ << std::endl
		<< "data_ = " << e.data_ << std::endl
		<< "len_ = " << e.len_ << std::endl
		<< "addTime_ = " << e.addTime_ << std::endl;
#endif
}

bool PacketManager::checkid(unsigned long id)
{
	unsigned short	index = getindex(id);
	unsigned short	seed = getseed(id);

	return index < maxNum_ && pduList_[index].seed_ == seed;
}

unsigned long PacketManager::makeid(unsigned short index, unsigned short seed) {
	return index << 16 | seed;
}

unsigned short PacketManager::getindex(unsigned long id) {
	return id >> 16;
}

unsigned short PacketManager::getseed(unsigned long id) {
	return id & 0x0000FFFF;
}

void PacketManager::movepos(unsigned short& pos) {
	pos ++;
	if(pos == maxNum_) {
	//	pos = 0;//modify 2014-7-17
		pos = 1;
	}
}

PacketManager::PacketManager(IPacketTimeOut* pCallBack, unsigned short timeout, unsigned short n)
	: pCallBack_(pCallBack), timeout_(timeout),
	pduList_(n), maxNum_(n), hasNum_(0),
	//insertPos_(0), checkPos_(0)  //modify 2014-7-17
	insertPos_(1), checkPos_(1)
{
}

unsigned long PacketManager::get()
{
	//int	i = 0;//modify 2014-7-17
	int	i = 1;
	CMutexKeeper  mutexKeep(mutex_);  //add 2014-5-7
	for(; i < maxNum_; i ++) {
		if(pduList_[insertPos_].unUsed()) {
			break;
		}

		movepos(insertPos_);
	}

	if(i == maxNum_) {
		assert(0);
	}
	pduList_[insertPos_].addTime_ = time(0); //add by zjj 2014-5-7

	return makeid(insertPos_, pduList_[insertPos_].seed_);
}

void PacketManager::set(unsigned long id, unsigned short pdutype, const void* data, unsigned short len)
{
	assert(checkid(id));

	unsigned short	index = getindex(id);

	pduList_[index].pduType_ = pdutype;
	char*	p = NULL;
	if(len) //modify 2011-7-8
	{
		p = new char[len];
		assert(p);
		memcpy(p, data, len);
	}
	pduList_[index].data_ = p;
	pduList_[index].len_ = len;
	pduList_[index].addTime_ = time(0);
	pduList_[index].outTimes_ = 0;

	hasNum_ ++;
	timer_.start(TIMERID, TIMERINTERVAL, this, 0);
		
	return;
}

unsigned short PacketManager::getOutTimes(unsigned long id)
{
	assert(checkid(id));
	unsigned short	index = getindex(id);
	return 	pduList_[index].outTimes_;
}


void PacketManager::clear(unsigned long id)
{
	assert(checkid(id));

	CMutexKeeper  mutexKeep(mutex_);  //add 2014-5-7
	unsigned short	index = getindex(id);
	pduList_[index].clear();
	hasNum_ --;
	if(0 == hasNum_) {
		timer_.kill();
	}

	if(insertPos_ > index) {
		insertPos_ = index;
	}

	return;
}

void PacketManager::clear()
{
	//for(int i = 0; hasNum_ && i < maxNum_; i ++) {//modify 2014-7-17
	for(int i = 1; hasNum_ && i < maxNum_; i ++) {
		if(! pduList_[i].unUsed()) {
			pduList_[checkPos_].clear();
			hasNum_ --;
		}
	}

	//insertPos_ = 0;//modify 2014-7-17
	//checkPos_ = 0;
	insertPos_ = 1;
	checkPos_ = 1;
}

void PacketManager::dump() const
{
	std::for_each(pduList_.begin() + 1, pduList_.end(), E::dump);
}

int PacketManager::OnTimer(unsigned long timerID, void* arg)
{
	time_t	now = time(0);

//	for(int i = 0; hasNum_ && i < maxNum_; i ++) {	//modify 2014-7-17
		for(int i = 1; hasNum_ && i < maxNum_; i ++) {
		if(! pduList_[checkPos_].unUsed() && now - pduList_[checkPos_].addTime_ > timeout_) {
			//If return zero, clear it.
			if(0 != pCallBack_->OnPacketTimeOut(pduList_[checkPos_].pduType_, 
						pduList_[checkPos_].data_, pduList_[checkPos_].len_)) {
				pduList_[checkPos_].clear();
				hasNum_ --;
			}
			else {
				//Need to update addTime
				pduList_[checkPos_].addTime_ = now;
				pduList_[checkPos_].outTimes_ ++;         //add by zjj 2011-2-25
			}
		}
		movepos(checkPos_);
	}

	return 0 == hasNum_ ? 1 : 0;
}

