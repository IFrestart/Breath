//NetCtrlCall.h: ���ýӿ�ʵ��ͷ�ļ�

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



/** ͨ�Ų���ýӿ�ʵ����(ͨ�Ų������)
 */
class CNetCtrlCall 
	: public IBaseCommCall	,public ITimerEventCallBack, public IPacketTimeOut
{
public:
	/** ���캯��
	*/
	CNetCtrlCall(void);

	/** ��������
	*/
	~CNetCtrlCall(void);

	/** ��ʼ��ͨ�Ŷ���(����ת��������IP��ַ��ͨ�ûص��ӿ�)
	* @param SvrAddr ת��������IP��ַ��Ϣ
	* @param lpCommCallBack ͨ�ûص��ӿ�(����ΪNULL)
	* @return 0-�ɹ�, <0ʧ��
	*/
	virtual int NetACInitialize(const Addr&  addr, 	IBaseCommCallBack * lpCommCallBack, USHORT nLocalPortBind=0);
	/** �ͷ�ͨѶ����
	* @return 0-�ɹ�, <0ʧ��
	*/
	int NetACUnInitialize();

	int NetACLogin(const BYTE* lpLoginInfo, WORD wLoginInfoSize,                    
		const BYTE* lpExtLoginInfo, WORD wExtLoginInfoSize, 
		INetACDataCallBack* lpDataCallBack);

	/** �˳�ϵͳ
	* @return 0-�ɹ�, <0ʧ��
	*/
	int NetACLogout(void);

    virtual int NetACSendDataToDB(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync = true );
    virtual int NetACSendDataToServ(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync = false );
	
public:
	//����keepalive��������
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
	int CloseTcpTransServConnect();//��ȡ��֤��������,�ر�
	int TcpTransServConnectException();//tcpת��������ͨ���쳣����
	//int OnCallBack(unsigned short errcode = 0);//modify 2012-11-29
	int OnCallBack(unsigned short errcode ,bool lockFlag = true);
	void updateAuthTime() {m_lastAuthTime = time(NULL);}
	void updateTransTime() {m_lastTransTime = time(NULL);}
	int AllConnectClose();



public:

	
	/** ��ȡͨ�ûص��ӿ�
	*/
	IBaseCommCallBack* GetCommCallBack(void){return m_pCommCallBack;}
	
	/** ��ȡ���ݻص��ӿ�
	*/
	INetACDataCallBack* GetDataReciveCallBack(void){return m_pNetACDataCallBack;}	//add 2011-8-1

	/** ע��ص�������
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
	
	
	static LPCTSTR f_lpszClassName; ///<�ص���������


	UserInfoEx			m_myselfInfo;
	IBaseCommCallBack*		m_pCommCallBack; ///< ͨ�ûص��ӿ�ʵ��
	INetACDataCallBack*		m_pNetACDataCallBack;	//���ݻص��ӿ���ʵ��


	CTimer				m_keepAliveTimer;
	CTimer				m_sendDataTimer;
	Addr				m_AuthServAddr;   //��֤��������ַ��Ϣ
	Addr				m_TransServAddr;   //��֤��������ַ��Ϣ


	CTCPConnector           m_TransConnector; 
	CTCPConnector           m_AuthConnector; 

	AuthServSockEvent		m_AuthTcpEvent;
	TransServSockEvent		m_TransTcpEvent;

	//CTCPAcceptor*			m_pTcpSvrObj;  //<p2p TCP�����׽���AC����
	//p2pAcceptSockEvent		m_acceptSockEvent;//�ͻ���p2p TCP�¼�
	//unsigned short         	m_nLocalPortBind;  //���ؼ����˿�
	unsigned long			m_lastAuthTime;//for keepalive
	unsigned long			m_lastTransTime;//for keepalive
	char				m_szUserId[MAX_USERID_LEN];     ///< �û��� 
	char				m_szUserPassword[MAX_USER_PASSWORD_LEN];  ///< �û�����

	PacketManager			m_packetMgr;
	CMutex                                  m_statusMuex;
	CBaseState*                             m_pActStatus_;
	bool                                    m_exceptLeave ;
	unsigned short       m_wCertType;
	unsigned short                  m_wNetType; //add 2014-4-14
};

//DWORD GetLocalIndex(void);

#endif

//NetCtrlCall.h: ���ýӿ�ʵ��ͷ�ļ�


