/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	IdDef.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#include "IdDef.h"
//#define ACE_INVALID_HANDLE  (SOCKET)(~0)

/**
 * Addr
 **/
Addr::Addr()
	: ip_(0), port_(0)
{
}

bool Addr::operator== (const Addr& o) const
{
	return ip_ == o.ip_ && port_ == o.port_;
}

CSockStream& operator << (CSockStream& ss, const Addr& ip)
{
	ss << ip.ip_ << ip.port_;

	return ss;
}

const CSockStream& operator >> (const CSockStream& ss, Addr& ip)
{
	ss >> ip.ip_ >> ip.port_;

	return ss;
}

/**
 * ServerID
 **/
ServerID::ServerID()
{
	memset(szId_,0,sizeof(szId_));
}

ServerID::ServerID(const ServerID& other)
{
	memcpy(szId_,other.szId_,sizeof(szId_));
}

ServerID& ServerID::operator= (const ServerID& other)
{
	if(this != &other)
	{
		memset(szId_,0,sizeof(szId_));
		memcpy(szId_,other.szId_,sizeof(szId_));
	}
	return *this;
}

bool ServerID::operator== (const ServerID& o) const
{
	return (0 == strncmp(szId_,o.szId_,strlen(szId_)));
}

bool ServerID::empty() const
{
	return 0 == strlen(szId_);
}

CSockStream& operator << (CSockStream& ss, const ServerID& s)
{
	ss.writeString(s.szId_);
	return ss;
}

const CSockStream& operator >> (const CSockStream& ss, ServerID& s)
{
	ss.readString(s.szId_,MAX_USERID_LEN);
	return ss;
}

/**
 * RSServerID
 **/
RSServerID::RSServerID() 
	: sock_(0)
{
}
RSServerID::RSServerID(const ServerID& serverid,unsigned short sock) : serverid_(serverid),sock_(sock)
{ 
}

RSServerID& RSServerID::operator= (const RSServerID& other)
{
	if(this != &other)
	{
		serverid_ = other.serverid_ ;
		sock_ = other.sock_;
	}
	return *this;
}

bool RSServerID::operator== (const RSServerID& o) const
{
	return serverid_ == o.serverid_;
}

CSockStream& operator << (CSockStream& ss, const RSServerID& rssid)
{
	ss << rssid.serverid_ << rssid.sock_;
	return ss;
}

const CSockStream& operator >> (const CSockStream& ss, RSServerID& rssid)
{
	ss >>  rssid.serverid_ >> rssid.sock_;	
	return ss;
}

/**
 * UserID
 **/
UserID::UserID()
	: cid_(0), uid_(0)
{
}

bool UserID::operator== (const UserID& that) const
{
	return ((cid_ == that.cid_) && (uid_ == that.uid_));
}

CSockStream& operator << (CSockStream& ss, const UserID& u)
{
	ss << u.cid_ << u.uid_;

	return ss;
}

const CSockStream& operator >> (const CSockStream& ss, UserID& u)
{
	ss >> u.cid_ >> u.uid_;

	return ss;
}

bool UserID::operator !=( const UserID& userid )
{
        if( *this == userid )
                return false ;
        else
                return true ;
}

bool UserID::operator < ( const UserID &userid ) const
{
	if (cid_ < userid.cid_)
	{
		return true;
	}
	else if(cid_ > userid.cid_)
	{
		return false;
	}
	else if(uid_ < userid.uid_)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * UserInfo
 **/
UserInfo::UserInfo()
	: mstatus_(0), mflag_(0), sock_(ACE_INVALID_HANDLE)
{
	memset(&szAccount_,0,sizeof(szAccount_));
}

/**
 * UserInfo
 **/
UserInfo::UserInfo(const UserID& user)
	: mstatus_(0), mflag_(0), sock_(ACE_INVALID_HANDLE)
{
	userID_ = user;
	memset(&szAccount_,0,sizeof(szAccount_));
}

UserInfo::~UserInfo()
{
}

UserInfo::UserInfo(const UserInfo& other)
{
	memcpy(&userID_,&other.userID_,sizeof(other.userID_));
	mstatus_ = other.mstatus_;
	mflag_ = other.mflag_;
	sock_ = other.sock_;		//modify a bug 2011-7-8
	memcpy(szAccount_,other.szAccount_,sizeof(szAccount_));
}

UserInfo& UserInfo::operator= (const UserInfo& other)
{
	if(this != &other)
	{
		memcpy(&userID_,&other.userID_,sizeof(other.userID_));
		mstatus_ = other.mstatus_;
		mflag_ = other.mflag_;
		//sock_ = sock_; //modify a bug 2011-7-8
		sock_ = other.sock_;
		memcpy(szAccount_,other.szAccount_,sizeof(szAccount_));
	}
	return (*this);
}


bool UserInfo::operator== (const UserID& userID) const
{
	return userID_ == userID;
}

bool UserInfo::operator== (const UserInfo& other) const
{
	return userID_ == other.userID_;
}
/*
bool UserInfo::operator== (ACE_HANDLE sock) const
{
	return sock_ == sock;
}
*/

bool UserInfo::operator < ( const UserID& other ) const //add 2013-4-16
{
      return userID_.uid_ < other.uid_;
}

CSockStream& operator << (CSockStream& ss, const UserInfo& ui)
{
	ss.writeString(ui.szAccount_);
	ss << ui.userID_
		<< ui.mstatus_ << ui.mflag_ ;

	return ss;
}

const CSockStream& operator >> (const CSockStream& ss, UserInfo& ui)
{
	ss.readString(ui.szAccount_,MAX_USERID_LEN);
	ss >> ui.userID_
		>> ui.mstatus_ >> ui.mflag_ ;
	return ss;
}

const UserID& UserInfo::getUserID() const
{
	return userID_;
}
void UserInfo::setUserId(const UserID& user)
{
	userID_ = user;
}

ACE_HANDLE UserInfo::getSock() const
{
	return sock_;
}

void UserInfo::setSock(ACE_HANDLE sock)
{
	sock_ = sock;
}



unsigned long UserInfo::getStatus() const
{
	return mstatus_;
}

void UserInfo::setStatus(unsigned long status)
{
	mstatus_ = status;
}

unsigned long UserInfo::getFlag() const
{
	return mflag_;
}
void UserInfo::setFlag(unsigned long flag)
{
	mflag_ = flag;
}

/*
void UserInfo::setIP(unsigned long ip)
{
	ip_ = ip;
}
unsigned long UserInfo::getIP() const
{
	return ip_;
}
*/

const char* UserInfo::getAccount() const
{
	return szAccount_;
}

void UserInfo :: setAccount(const char* szUser)
{
	if (NULL == szUser)
	{
		return ;
	}
	memset(&szAccount_,0,sizeof(szAccount_));
	strncpy(szAccount_,szUser,strlen(szUser) + 1);
}
