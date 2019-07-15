//NetCtrlCall.h: 调用接口实现头文件

#ifndef	__NETCTRLCALL_H__
#define	__NETCTRLCALL_H__

#include "NetCtrlInt.h"
#include "TCPAcceptor.h"
#include "TCPConnector.h"
//#include "TcpAutoConnector.h"
#include "AuthServSockEvent.h"
#include "TransServSockEvent.h"
#include "PDU.h"
#include "Timer.h"
#include "AllState.h"
#include <assert.h>
#include "PacketManager.h"
#include "Mutex.h"




//#include <Winbase.h>
//#include <Mmsystem.h>
#ifndef __LINUX__
#include <WinSock2.h>
#include <tchar.h>
#endif

#define WEBCON_HAS_STDLOG



/** 通信层调用接口实现类(通信层入口类)
 */
class CNetCtrlCall 
	: public IBaseCommCall	,public ITimerEventCallBack, public IPacketTimeOut
{
public:
	/** 构造函数
	*/
	CNetCtrlCall(void);

	/** 析构函数
	*/
	~CNetCtrlCall(void);

	/** 初始化通信对象(设置转发服务器IP地址和通用回调接口)
	* @param SvrAddr 转发服务器IP地址信息
	* @param lpCommCallBack 通用回调接口(不能为NULL)
	* @return 0-成功, <0失败
	*/
	virtual int NetACInitialize(const Addr&  addr, 	IBaseCommCallBack * lpCommCallBack, USHORT nLocalPortBind=0);
	/** 释放通讯对象
	* @return 0-成功, <0失败
	*/
	int NetACUnInitialize();

	int NetACLogin(const BYTE* lpLoginInfo, WORD wLoginInfoSize,                    
		const BYTE* lpExtLoginInfo, WORD wExtLoginInfoSize, 
		INetACDataCallBack* lpDataCallBack);

	/** 退出系统
	* @return 0-成功, <0失败
	*/
	int NetACLogout(void);

    virtual int NetACSendDataToDB(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync = true );
    virtual int NetACSendDataToServ(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync = false );
	
public:
	//发送keepalive函数处理
	virtual int SendKeepAlive(void);

	/* From ITimerEventCallBack */
	virtual int OnTimer(unsigned long, void*);

    /* From IPacketTimeOut */
     virtual int OnPacketTimeOut(unsigned short pdutype, const void*, unsigned long);

	//void changeState(unsigned short sid); //modify 2012-11-29
	void changeState(unsigned short sid,bool lockFlag = true);

	enum{ 	 SendDataTimer_ID = 1, SendDataTimer_VALUE = 2000,KeepAliveTimer_ID = 2, KeepAliveTimer_VALUE = 10000,LOGIN_TIME_OUT_VALUE = 600};
	enum{ 	 type_CTRL_DATA = 1, type_AV_DATA = 3};

	int startTimer(unsigned short type = 0);//type == 0 for sendDataTimer else for keepAliveTimer
	int killTimer(unsigned short type = 0);//type == 0 for sendDataTimer else for keepAliveTimer

	int ConnectTransServ();
	int ConnectAuthServ();
	int SendAuthRequest();
	int SendTcpDataToAuthServ(unsigned short pdutype,const void* buf, unsigned long len);
	int SendTcpDataToTransServ(unsigned short pdutype,const void* buf, unsigned long len);

	int SendLoginRequest();
	int SendLogoutRequest();
	int SendLeaveRequest();
	int SendJoinRequest();
	int CloseTcpAuthServConnect();
	int CloseTcpTransServConnect();//获取认证服务器后,关闭
	int TcpTransServConnectException();//tcp转发服务器通道异常处理
	//int OnCallBack(unsigned short errcode = 0);//modify 2012-11-29
	int OnCallBack(unsigned short errcode ,bool lockFlag = true);
	void updateAuthTime() {m_lastAuthTime = time(NULL);}
	void updateTransTime() {m_lastTransTime = time(NULL);}
	int AllConnectClose();



public:

	
	/** 获取通用回调接口
	*/
	IBaseCommCallBack* GetCommCallBack(void){return m_pCommCallBack;}
	
	/** 获取数据回调接口
	*/
	INetACDataCallBack* GetDataReciveCallBack(void){return m_pNetACDataCallBack;}	//add 2011-8-1

	/** 注册回调窗口类
	*/
	static BOOL RegCallBackWndClass(void);

	ACE_HANDLE getAuthServConnectSock(){return m_AuthConnector.getSock();}
	ACE_HANDLE getTransServConnectSock(){return m_TransConnector.getSock();}
	UserInfoEx* GetMyselfInfo(void) ;
	void SetMyselfInfo(const UserInfo& userInfo);

	int NetACRegister0(void);
	int NetACRegister1(BYTE byVersionType, DWORD dwTopOrganID,WORD wRegType, DWORD dwKey, LPCTSTR lpszCode, 
		const BYTE* lpRegInfo, WORD wInfoSize);

private:
	
	
	static LPCTSTR f_lpszClassName; ///<回调窗口类名


	UserInfoEx			m_myselfInfo;
	IBaseCommCallBack*		m_pCommCallBack; ///< 通用回调接口实例
	INetACDataCallBack*		m_pNetACDataCallBack;	//数据回调接口主实例


	CTimer				m_keepAliveTimer;
	CTimer				m_sendDataTimer;
	Addr				m_AuthServAddr;   //认证服务器地址信息
	Addr				m_TransServAddr;   //认证服务器地址信息


	CTCPConnector           m_TransConnector; 
	CTCPConnector           m_AuthConnector; 

	AuthServSockEvent		m_AuthTcpEvent;
	TransServSockEvent		m_TransTcpEvent;

	//CTCPAcceptor*			m_pTcpSvrObj;  //<p2p TCP侦听套接字AC对象
	//p2pAcceptSockEvent		m_acceptSockEvent;//客户端p2p TCP事件
	//unsigned short         	m_nLocalPortBind;  //本地监听端口
	unsigned long			m_lastAuthTime;//for keepalive
	unsigned long			m_lastTransTime;//for keepalive
	char				m_szUserId[MAX_USERID_LEN];     ///< 用户名 
	char				m_szUserPassword[MAX_USER_PASSWORD_LEN];  ///< 用户密码

	PacketManager			m_packetMgr;
	CMutex                                  m_statusMuex;
	CBaseState*                             m_pActStatus_;
	bool                                    m_exceptLeave ;
	unsigned short       m_wCertType;
	unsigned short                  m_wNetType; //add 2014-4-14
};

//DWORD GetLocalIndex(void);

#endif

//NetCtrlCall.h: 调用接口实现头文件


