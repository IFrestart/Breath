/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 NetACClient.h
* @author	 zhoujj <zhoujj@webcon.com.cn>

* @class	NetACClient
* @short 	�����Ӧ�ó�����
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

	/** �������ݻص�����
	* @param gid   ����id; id == 0 ��ʾ�ǻ������ݻص�; id > 0 �������ݻص�
	* @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
	* @param lpData �յ�������
	* @param dwSize ���ݳ���
	* @param wPduType PDU����
	* @param lpSender ������������Ϣ
	*/
    virtual int OnDataReceived(unsigned short gid,unsigned short dataType,const void * lpData, DWORD dwSize, WORD wPduType,const UserID* lpSender=NULL);

	/** ��Ա�ı�ص�����
	* @param gid   ����id; id == 0 ��ʾ�ǻ����Ա�ص�; id > 0 �����Ա�ص�
	* notice  ����3�������쳣ʱ ����lpMemberUserΪNULL
	* @param nRosterFlag �ı��־
	* 1: ����   2: �뿪  3: �������쳣 4 ����״̬�ı� 
	* @param lpMemberUser �ı�ĳ�Ա������Ϣ
	*/
    virtual int OnMemberRoster(unsigned short gid,short nRosterFlag, const UserInfoEx* lpMemberUser);


public:
	//�̳��� IBaseCommCallBack

	/** ��ȡ��֤��ص�����
	* @param nResult ���(0-�ɹ�,<0ʧ��)
	* @param dwKey У������
	* @param lpCode У����(����)
	* @param wCodeSize У���볤��
	*/
	virtual int OnUserRegister0(short nResult, DWORD dwKey, const BYTE* lpCode,	WORD wCodeSize);

	/** ���û�ע��ص�����
	* @param nResult ���(0-�ɹ�,<0ʧ��)
	* @param lpszAccount �û��ʺ�
	*/
	virtual int OnUserRegister1(short nResult, LPCTSTR lpszAccount);

	/** ��¼�ص�����
	* @param nResult ���(0-�ɹ�,������ʧ��)
	* @param userInfo �û���Ϣ	
	*/
	//virtual int OnLogined(short nResult, const UserInfo& userInfo);
	virtual int OnLogined(short nResult, const UserInfoEx& userInfo);

	/** �˳��ص�����
	* @param nResult ���(LOGOUT_TIMEOUT��ʱ LOGOUT_RELOG�ص���)
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
	//�������ݿ������
	//@note:��FCNetACLogin�ɹ�֮����ܵ���
	//in @param pdu     ���͵����ݱ�ʶ
	//in @param in_data ���͵�������Ϣ
	//in @param in_Len   �������ݵĳ���
	//in @param retPdu   ���յ������ݱ�ʶ
	//out @param out_data ���յ���������Ϣ
	//out @param out_Len   ���յ������ݵĳ���
	//@return 0�ɹ���<0 ʧ��
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
	Addr			m_servAddr_;     //ת����������ַ

	char 		m_szUserId[MAX_USERID_LEN];
	char 		m_szPassword[MAX_USER_PASSWORD_LEN];

	unsigned short  m_localPort;   //���ؼ����˿�

	CThread				m_testThread_;


#ifndef __LINUX__
	HINSTANCE			m_hInstComm;
#endif

	OnFCNetAcCallBack		m_pCallBack;
	void*				m_pUserData;	//�û��Զ���
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
