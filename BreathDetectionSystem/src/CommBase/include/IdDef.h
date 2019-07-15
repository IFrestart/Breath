/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 与标识有关的数据结构

 * @file	IdDef.h
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short	与标识有关的数据结构
 **/

#ifndef	__IDDEF_H__
#define	__IDDEF_H__

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include "SockStream.h"

const unsigned short INVALID_SID     =  0xFFFF;

typedef int BOOL;
typedef unsigned char BYTE;
typedef BYTE* LPBYTE;
typedef unsigned short WORD;
typedef WORD* LPWORD;
typedef unsigned long DWORD;
typedef DWORD* LPDWORD;

typedef const char* LPCSTR;
typedef const char* LPCTSTR;
typedef char* LPSTR;
typedef char* LPTSTR;

typedef void* LPVOID;
typedef char TCHAR;
typedef unsigned int UINT;
typedef unsigned int UINT_PTR;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef long  LONG;

#define MAX_USERID_LEN		129



/**
 * @class	Addr
 * @short	网络地址，包括IP与PORT
 **/
struct Addr
{
public:
	Addr();
	bool operator== (const Addr&) const;

	/**
	 * 定义了与CSockStream <<的操纵，方便从包中解析
	 */
	friend CSockStream& operator << (CSockStream&, const Addr&);

	/**
	 * 定义了与CSockStream >>的操纵，方便组包
	 */
	friend const CSockStream& operator >> (const CSockStream&, Addr&);

	/**
	 * 将类的信息转换成字符串的形式
	 */
	std::string toString() const
	{
		std::stringstream       s;
		s << "(ip = " << ip_ << ", port = " << port_ << ')';
		return s.str();
	}

public:
	unsigned long	ip_;		/**< IP地址*/
	unsigned short	port_;		/**< PORT地址*/
};
/**
 * @class	ServerID
 * @short	服务器的标识
 **/
struct ServerID
{
public:
	ServerID();
	ServerID(const ServerID& other);
	ServerID& operator= (const ServerID& other);

	bool operator== (const ServerID&) const;
	bool empty() const;

	/**
	 * 定义了与CSockStream <<的操纵，方便从包中解析
	 */
	friend CSockStream& operator << (CSockStream&, const ServerID&);

	/**
	 * 定义了与CSockStream >>的操纵，方便组包
	 */
	friend const CSockStream& operator >> (const CSockStream&, ServerID&);

	/**
	 * 将类的信息转换成字符串的形式
	 */
	std::string toString() const
	{
		std::stringstream       s;
		s << "(snid = " << szId_ << ')';
		return s.str();
	}

public:
	char 	szId_[MAX_USERID_LEN];
};

struct RSServerID
{
public:
	RSServerID();
	RSServerID(const ServerID& serverid,unsigned short sock) ;
	RSServerID& operator= (const RSServerID& other);
	bool operator== (const RSServerID& o) const;
	friend CSockStream& operator << (CSockStream&, const RSServerID&);
	friend const CSockStream& operator >> (const CSockStream&, RSServerID&);
	std::string toString() const
	{
		std::stringstream       s;
		//s << serverid_.toString() << addr_.toString() << "sock_=[" << sock_ << "]" << std::endl;
		s << serverid_.toString() <<  "sock_=[" << sock_ << "]" << std::endl;
		return s.str();
	}
public:
	ServerID serverid_;
	//Addr     addr_;
	unsigned short sock_;
};

typedef std::list<RSServerID>::iterator RSS_it;


/**
 * @class	UserID
 * @short	用户的标识
 **/
struct UserID
{
public:
	UserID();
	bool operator== (const UserID&) const;
	bool operator !=( const UserID& userid );
	bool operator < ( const UserID &userid ) const;

	/**
	 * 定义了与CSockStream <<的操纵，方便从包中解析
	 */
	friend CSockStream& operator << (CSockStream&, const UserID&);

	/**
	 * 定义了与CSockStream >>的操纵，方便组包
	 */
	friend const CSockStream& operator >> (const CSockStream&, UserID&);

	/**
	 * 将类的信息转换成字符串的形式
	 */
	std::string toString() const
	{
		std::stringstream       s;
		s << "(cid = " << cid_ << ", uid = " << uid_ << ')';
		return s.str();
	}

public:
	unsigned short	cid_;		/**< Cluster ID */
	unsigned long	uid_;		/**< User ID */
};

/**
 * @class	UserInfo
 * @short	用户的信息
 **/
struct UserInfo
{
public:
	UserInfo();
	UserInfo(const UserID& user);
	UserInfo(const UserInfo& other);
	UserInfo& operator= (const UserInfo& other);
	~UserInfo();
	bool operator== (const UserID&) const;
	bool operator== (const UserInfo& other) const;

	bool operator < ( const UserID& other ) const; //add 2013-4-16

	/**
	 * 定义了与CSockStream <<的操纵，方便从包中解析
	 */
	friend CSockStream& operator << (CSockStream&, const UserInfo&);

	/**
	 * 定义了与CSockStream >>的操纵，方便组包
	 */
	friend const CSockStream& operator >> (const CSockStream&, UserInfo&);

	const UserID& getUserID() const;
	void setUserId(const UserID& user);
	ACE_HANDLE getSock() const;
	void setSock(ACE_HANDLE);

	unsigned long getStatus() const;
	void setStatus(unsigned long);
	unsigned long getFlag() const;
	void setFlag(unsigned long);


	void setAccount(const char*);
	const char* getAccount()const;

	void dump(const char* pre,std::fstream& logfile) const
	{
		logfile << pre << "userID       = " << userID_.toString() << std::endl;
		logfile << pre << "sock_        = " << sock_ << std::endl;
		logfile << pre << "mflag_       = " << mflag_ << std::endl;
		logfile << pre << "mstatus_     = " << mstatus_ << std::endl;

	}

protected:
	UserID		userID_;		/**< 用户的标识 */
	unsigned long	mstatus_;		/**< 用户的会议中的状态位 */
	unsigned long	mflag_;		/**< 用户的会议中的标记位 */
	ACE_HANDLE	sock_;		/**< 用户的当前连接的socket值 */
	char 		szAccount_[MAX_USERID_LEN];
};

typedef std::list<UserInfo>::iterator USER_it;

struct UserInfoEx : public UserInfo  	
{
public:
	UserInfoEx() {}
	UserInfoEx(const UserInfo& other) : UserInfo(other) 
	{	
		setTime();
	}
	UserInfoEx& operator= (const UserInfoEx& other)
	{
		 if(this != &other)
		{
			(UserInfo&)(*this) = (UserInfo&)other;
			lasttimer_ = other.lasttimer_ ;
		}
		return (*this);
	}
	void	setTime() { lasttimer_ = time(NULL);}
	unsigned long getTime() {return lasttimer_;}


	//Addr	      videoAddr_;
private:
	unsigned long lasttimer_;
};
typedef std::list<UserInfoEx>::iterator USER_Ex_it;


#endif	/*__IDDEF_H__*/

