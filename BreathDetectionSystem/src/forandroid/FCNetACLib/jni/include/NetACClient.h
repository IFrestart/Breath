/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 NetACClient.h
* @author	 zhoujj <zhoujj@webcon.com.cn>

* @class	NetACClient
* @short 	会议的应用程序类
**/

#ifndef	__NETAC_CLIENT_H__
#define	__NETAC_CLIENT_H__



//#define WEBCON_HAS_STDLOG
#ifdef USE_ACE_VER
#include "BaseServ.h"
#endif
#include "IdDef.h"
#include "NetCtrlInt.h"
#include "Timer.h"
#include "Thread.h"
#include "NetCtrlDll.h"
#include "FCNetACLib.h"
#include "Mutex.h"


#include "Event.h"

const unsigned short NetAC_TCP_PORT = 7501;


const unsigned int   NetAC_Msg_LogOut=1 ;

 

class IBaseCommCall;
class NetACClient;


#ifdef USE_ACE_VER
class NetACClient : public CBaseServ, public ITimerEventCallBack, public INetACDataCallBack,public IBaseCommCallBack,public IThreadCallBack
#else
class NetACClient : public ITimerEventCallBack, public INetACDataCallBack,public IBaseCommCallBack,public IThreadCallBack
#endif
{
public:
	//typedef	std::list<ResourceUnit *>::iterator  RSSUnit_it;

public:
	NetACClient();
	~NetACClient();

#ifdef USE_ACE_VER
	/* From CBaseServ */
	virtual int initialize(int, char**);
	virtual int destroy();
#else
	int initialize(int, char**);
	int destroy();
#endif

public:
	/* From IThreadCallBack */
	void*  threadProc(void* arg);

public:

	/** 接收数据回调函数
	* @param gid   会议id; id == 0 表示非会议数据回调; id > 0 会议数据回调
	* @param dataType 发送方式(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包)
	* @param lpData 收到的数据
	* @param dwSize 数据长度
	* @param wPduType PDU类型
	* @param lpSender 发送者索引信息
	*/
    virtual int OnDataReceived(unsigned short gid,unsigned short dataType,const void * lpData, DWORD dwSize, WORD wPduType,const UserID* lpSender=NULL);

	/** 成员改变回调函数
	* @param gid   会议id; id == 0 表示非会议成员回调; id > 0 会议成员回调
	* notice  当是3服务器异常时 参数lpMemberUser为NULL
	* @param nRosterFlag 改变标志
	* 1: 加入   2: 离开  3: 服务器异常 4 好友状态改变 
	* @param lpMemberUser 改变的成员索引信息
	*/
    virtual int OnMemberRoster(unsigned short gid,short nRosterFlag, const UserInfoEx* lpMemberUser);


public:
	//继承自 IBaseCommCallBack

	/** 获取认证码回调函数
	* @param nResult 结果(0-成功,<0失败)
	* @param dwKey 校验码编号
	* @param lpCode 校验码(密文)
	* @param wCodeSize 校验码长度
	*/
	virtual int OnUserRegister0(short nResult, DWORD dwKey, const BYTE* lpCode,	WORD wCodeSize);

	/** 新用户注册回调函数
	* @param nResult 结果(0-成功,<0失败)
	* @param lpszAccount 用户帐号
	*/
	virtual int OnUserRegister1(short nResult, LPCTSTR lpszAccount);

	/** 登录回调函数
	* @param nResult 结果(0-成功,其他表失败)
	* @param userInfo 用户信息	
	*/
	//virtual int OnLogined(short nResult, const UserInfo& userInfo);
	virtual int OnLogined(short nResult, const UserInfoEx& userInfo);

	/** 退出回调函数
	* @param nResult 结果(LOGOUT_TIMEOUT超时 LOGOUT_RELOG重登入)
	*/
	//virtual int OnLogout(short nResult);

public:
	void LoadCommLib();
	void FreeCommLib();

public:
	//interface for FCNetACLib 
	int waitInit();
	int waitLogOut();
	int setLogOutFlag(){m_logOutFlag = true;}
	//int LogIn(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = INFINITE);				
	int LogIn(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = TIME_OUT_INFINITE);				
	int LogOut();
	//------------------------------------------
	//发往数据库的数据
	//@note:在FCNetACLogin成功之后才能调用
	//in @param pdu     发送的数据标识
	//in @param in_data 发送的数据信息
	//in @param in_Len   发送数据的长度
	//in @param retPdu   接收到的数据标识
	//out @param out_data 接收到的数据信息
	//out @param out_Len   接收到的数据的长度
	//@return 0成功，<0 失败
	//------------------------------------------
	int SendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,unsigned short& retPdu,const char*& out_data,unsigned long& out_Len);

	int SendDataToServ(unsigned short pdu,const char* data,unsigned long len);


public:


private:

#ifdef USE_ACE_VER
	/* From CBaseServ */
	virtual int loadConfig(const char*);
	virtual int handle_signal(int signum, siginfo_t*, ucontext_t*);
#else
	int loadConfig(const char*);
	//int handle_signal(int signum, siginfo_t*, ucontext_t*);
#endif

	
	/* From ITimerEventCallBack */
	virtual int OnTimer(unsigned long, void*);

private:
	enum {KEEPALIVE_TIMER_ID = 1,LOGOUT_TIMER_ID = 2};
	enum {KEEPALIVE_TIMER_INTERVAL = 5000, LOGOUT_TIMER_INTERVAL = 500,CHECKALIVE_TIMER_INTERVAL = 10000}; 
	CTimer			keepAliveTimer_;		
	CTimer			logOutTimer_;			//add 2011-7-26

	long			timer_id;

	IBaseCommCall*  m_pNetCtrlCall;
	Addr			m_servAddr_;     //转发服务器地址

	char 		m_szUserId[MAX_USERID_LEN];
	char 		m_szPassword[MAX_USER_PASSWORD_LEN];

	unsigned short  m_localPort;   //本地监听端口

	CThread				m_testThread_;


#ifndef __LINUX__
	HINSTANCE			m_hInstComm;
#endif

	OnFCNetAcCallBack		m_pCallBack;
	void*				m_pUserData;	//用户自定义
	Event				m_initEvent;
	Event				m_loginEvent;
	Event				m_logoutEvent;
	Event				m_dbEvent;
	int					m_loginFlag;
	bool				m_logOutFlag;
	bool                m_bInitOk;
	bool				m_logOutOk;

	unsigned long		m_currentPmid;
	char			m_currentUser[MAX_USER_ACCOUNT_LEN];
	
	char*				m_pDBData;        //add 2011-8-1
	unsigned long		m_nDBDataLen;		//add 2011-8-1
	CMutex				m_DBDataMuex;
	unsigned long		m_currentdbPmid;
	unsigned short		m_currentRetPdu;
};

#ifdef  __TXT_MODE__
extern NetACClient& GetApp();
extern NetACClient g_testNetAC;
#endif


#endif	/*__NETAC_CLIENT_H__*/
