//#include "StdAfx.h"


#include "NetCtrlCall.h"



/** 构造函数
*/
CNetCtrlCall::CNetCtrlCall(void) 
				: m_AuthTcpEvent(this),m_TransTcpEvent(this),m_pActStatus_(NULL),
			m_packetMgr(this)
{

	memset(m_szUserId,0,MAX_USERID_LEN);
	memset(m_szUserPassword,0,MAX_USER_PASSWORD_LEN);
	
	m_lastTransTime = 0;
	m_lastAuthTime = 0;
	m_exceptLeave = false; //add 2013-3-22

	m_pNetACDataCallBack = NULL;
}



/** 析构函数
*/
CNetCtrlCall::~CNetCtrlCall(void)
{
	if(NULL != m_pActStatus_)
	{
		delete m_pActStatus_;
		m_pActStatus_ = NULL;
	}
}

/** 初始化通信对象(设置转发服务器IP地址和通用回调接口)
* @param rssAddr 转发服务器IP地址信息
* @param lpCommCallBack 通用回调接口(不能为NULL)
* @return 0-成功, <0失败
*/
int CNetCtrlCall::NetACInitialize(const Addr&  addr, 	IBaseCommCallBack * lpCommCallBack, USHORT nLocalPortBind)
{
	m_AuthServAddr = addr;
	m_TransServAddr.ip_ = addr.ip_;
	m_TransServAddr.port_ = addr.port_ + 2;
	m_pCommCallBack = lpCommCallBack;
	return 0;
}

int CNetCtrlCall::AllConnectClose()
{
	killTimer(KeepAliveTimer_ID);		//add 2011-7-11 for a bug
	m_TransConnector.close();//add 2012-12-28
	m_AuthConnector.close();  //add 2013-3-5 
	return 0;
}
 
int CNetCtrlCall::NetACUnInitialize()
{
	AllConnectClose();
	return 0;
}

/** 获取注册码
* @return 0-成功, <0失败
*/
int CNetCtrlCall::NetACRegister0(void)
{
	return 0;
}

/** 注册新用户
* @param byVersionType 版本类型
* @param dwTopOrganID  顶层组织ID
* @param wRegType 注册类型(现在为0)
* @param dwKey 验证编号
* @param lpszCode 验证码(明文)
* @param lpRegInfo 注册信息(不能为NULL)
* @param wRegInfoSize 注册信息(>0)
* @return 0-成功, <0失败
*/
int CNetCtrlCall::NetACRegister1(BYTE byVersionType, DWORD dwTopOrganID,WORD wRegType, DWORD dwKey, LPCTSTR lpszCode, 
	const BYTE* lpRegInfo, WORD wInfoSize)
{
	
	return 0;
}

/** 系统登录
* @param lpLoginInfo 登录信息
* @param wLoginInfoSize 登录信息长度
* @param lpExtLoginInfo 扩展信息
* @param wExtLoginInfoSize 扩展信息长度
* @param lpDataCallBack 数据回调接口(不能为NULL)
* @return 0-成功, <0失败
*/ 
int CNetCtrlCall::NetACLogin(const BYTE* lpLoginInfo, 
	WORD wInfoSize, const BYTE* lpExtLoginInfo, 
	WORD wExtLoginInfoSize, INetACDataCallBack* lpDataCB)
{
	assert(lpLoginInfo != NULL);
	if (lpLoginInfo == NULL)
	{
		return -1;
	}

	assert(wInfoSize > 0);
	if (wInfoSize == 0)
	{
		return -2;
	}

	assert(lpDataCB != NULL);
	if (lpDataCB == NULL)
	{
		return -3;
	}
	m_exceptLeave = false; //add 2013-3-22

    	m_pNetACDataCallBack = lpDataCB;
	CSockStream login_stream(lpLoginInfo,wInfoSize);
	login_stream >> m_wCertType;
	login_stream.readString(m_szUserId,MAX_USERID_LEN);
	login_stream.readString(m_szUserPassword,MAX_USER_PASSWORD_LEN);

	login_stream >> m_wNetType;  //add 2014-4-14
	unsigned long tmpUserStatus ,tmpLpdataLen;
	login_stream >> tmpUserStatus ;
	//login_stream >> tmpLpdataLen;
	m_myselfInfo.setAccount(m_szUserId);	//add 2012-6-6
//	tmpUserStatus = tmpUserStatus | 0x1;	//add 2012-7-12 设置在线 //modify 2012-12-3
	tmpUserStatus = (tmpUserStatus << 16 ) | 0x1;	
	m_myselfInfo.setStatus(tmpUserStatus);
	//m_myselfInfo.changeLpData(tmpLpdataLen,login_stream.getCurr());

	changeState(LOGINSTATE::SAUTHREQUEST);

		return 0;
}

/** 退出系统
* @return 0-成功, <0失败
*/
int CNetCtrlCall::NetACLogout(void)
{
	//DestroyAllSession();
	if (!m_exceptLeave)
	{	
		for (int i = 0 ; i < 2; i++ )
		{
			SendLogoutRequest();
#ifdef VER_FOR_ANDROID_
			usleep(500);
#else
			ACE_OS::sleep(ACE_Time_Value(1, 500));
#endif
		}
	}

	m_pNetACDataCallBack = NULL; //add 2011-6-15
	//AllP2PConnectClose();//add 2011-6-15
	AllConnectClose();//modify 2011-7-11
	//changeState(LOGINSTATE::SLOGOUTAUTH);
	return 0;
}


/** 注册回调窗口类
*/
BOOL CNetCtrlCall::RegCallBackWndClass(void)
{
	return true;
}


/** 获取自己的通信信息
*/
UserInfoEx* CNetCtrlCall::GetMyselfInfo(void)
{
	return &m_myselfInfo;
}

void CNetCtrlCall::SetMyselfInfo(const UserInfo& userInfo)
{
	//m_myselfInfo = userInfo;//modify 2012-6-6
	(UserInfo&)m_myselfInfo = userInfo;
}


int CNetCtrlCall::SendKeepAlive()
{
	CSockStream out_stream1;

	out_stream1 << m_myselfInfo.getUserID();
	//out_stream1 << m_myselfInfo.getStatus();
	//out_stream1 << m_myselfInfo.getFlag();

//	unsigned short frinedNum1 = 0,frinedNum2 = 0;
	//unsigned long tmpIndex = out_stream1.tell();
//	out_stream1 << frinedNum1;

	SendTcpDataToAuthServ(PDU::KEEPALIVE_REQ_FROMCLI,out_stream1.getData(),out_stream1.length());
	SendTcpDataToTransServ(PDU::SESKEEPALIVE_REQ_FROMCLI,out_stream1.getData(),out_stream1.length());
	return 0;
}

int CNetCtrlCall::killTimer(unsigned short type)
{
	if (0 != type)
	{
		m_keepAliveTimer.kill();
	}
	else
	{
		m_sendDataTimer.kill();
	}
	return 0;
}

int CNetCtrlCall::startTimer(unsigned short type)
{
	if (0 != type)
	{
		m_lastAuthTime = time(0);
		m_keepAliveTimer.start(KeepAliveTimer_ID,KeepAliveTimer_VALUE,this,0);
	}
	else
	{
		m_sendDataTimer.start(SendDataTimer_ID,SendDataTimer_VALUE,this,0);
	}
	return 0;
}


int CNetCtrlCall::OnTimer(unsigned long tid, void* arg)
{
#if Debug_MyTrance_
	//std::cout << "CNetCtrlCall::OnTimer tid[" << tid << "] pthread id =[" <<  ACE_OS::thr_self() << "]" << std::endl;
#endif
	time_t  now = time(0);
	if (KeepAliveTimer_ID == tid)
	{
		if ((now - m_lastAuthTime) > (LOGIN_TIME_OUT_VALUE  * 2))
		{
			m_exceptLeave = true;		//add 2011-7-26
			if (m_pNetACDataCallBack)
			{
				std::cout << "timeOut to AuterServer" << std::endl;
				m_pNetACDataCallBack->OnMemberRoster(0,3,NULL);
			}
			return 1;
		}
		if (m_exceptLeave)   //add 2013-3-21
		{
			return 1;
		}
#if Debug_MyTrance_
		printf("CNetCtrlCall::SendKeepAlive req time[%lu]\n",now);
#endif
		SendKeepAlive();
	//add for phone 2014-4-28
	#ifdef VER_FOR_ANDROID_
		SendTcpDataToAuthServ(1,NULL,0); 
	#endif
	//end add for phone 2014-4-28
		//checkTransUnit();
		return 0;
	}

	CMutexKeeper  mutexKeep(m_statusMuex); //add 2012-11-29
	if (NULL == m_pActStatus_)
	{
		return 0;
	}

	if (((now - m_pActStatus_->getAddTime()) > LOGIN_TIME_OUT_VALUE) || 	(m_pActStatus_->getSendTimes() > m_pActStatus_->getMaxSendTimes()))
	{
		//killTimer(); //modify 2012-10-16
		if(LOGINSTATE::SLOGOUTAUTH != m_pActStatus_->State())
		{
			//OnCallBack();//modify 2012-11-29
			OnCallBack(0,false);
		}
		return 1;
			
	}
	else
	{
		m_pActStatus_->SendData(this);
	}
	

	return 0;
}

void CNetCtrlCall::changeState(unsigned short sid,bool lockFlag)
{
	if(lockFlag)
	{
		CMutexKeeper  mutexKeep(m_statusMuex); //add 2012-11-29

		if (NULL != m_pActStatus_)
		{
			//killTimer();//modify 2012-10-16
			delete m_pActStatus_ ;
			m_pActStatus_ = NULL;
		}
		switch(sid) {
			case LOGINSTATE::SAUTHREQUEST:
				m_pActStatus_ = new CSLoginAuthRequest;
				break;
			case LOGINSTATE::SLOGINAUTH:
				m_pActStatus_ = new CSLoginAuth;
				break;
			case LOGINSTATE::SJOINSTRANS:
				m_pActStatus_ = new CSTransJoin;
				break;
			case LOGINSTATE::SLOGOUTAUTH:
				m_pActStatus_ = new CSLogoutAuth;
				break;		
			case LOGINSTATE::SLEAVETRAS:
				m_pActStatus_ = new CSTransLeave;
				break;		
			case LOGINSTATE::SNORMAL:
				{
					killTimer();//add 2012-10-16
					m_pActStatus_ = new CSLoginNormal ;	
					startTimer(KeepAliveTimer_ID);
					return ;
				}
				break;
		default:
				printf("error in changeSate no sid=[%d]\n",sid );
				return ;
		}
		if (NULL ==	m_pActStatus_)
		{
			printf("error CNetCtrlCall::changeState new m_pActStatus_ error \n");
			return;
		}
		int nRet = m_pActStatus_->startObject(this);
		if( 0 == nRet )
		{		
			startTimer();
			m_pActStatus_->SendData(this);
		}
		else
		{
			//OnCallBack();//modify 2012-11-29
			OnCallBack(0,false);
		}	
	}
	else
	{
		if (NULL != m_pActStatus_)
		{
			//killTimer();//modify 2012-10-16
			delete m_pActStatus_ ;
			m_pActStatus_ = NULL;
		}
		switch(sid) {
			case LOGINSTATE::SAUTHREQUEST:
				m_pActStatus_ = new CSLoginAuthRequest;
				break;
			case LOGINSTATE::SLOGINAUTH:
				m_pActStatus_ = new CSLoginAuth;
				break;
			case LOGINSTATE::SJOINSTRANS:
				m_pActStatus_ = new CSTransJoin;
				break;
			case LOGINSTATE::SLOGOUTAUTH:
				m_pActStatus_ = new CSLogoutAuth;
				break;		
			case LOGINSTATE::SLEAVETRAS:
				m_pActStatus_ = new CSTransLeave;
				break;		
			case LOGINSTATE::SNORMAL:
				{
					killTimer();//add 2012-10-16
					m_pActStatus_ = new CSLoginNormal ;	
					startTimer(KeepAliveTimer_ID);
					return ;
				}
				break;
		default:
				printf("error in changeSate no sid=[%d]\n",sid );
				return ;
		}
		if (NULL ==	m_pActStatus_)
		{
			printf("error CNetCtrlCall::changeState new m_pActStatus_ error \n");
			return;
		}
		int nRet = m_pActStatus_->startObject(this);
		if( 0 == nRet )
		{		
			startTimer();
			m_pActStatus_->SendData(this);
		}
		else
		{
			//OnCallBack(false);//modify 2012-11-29
			OnCallBack(0,false);
		}	
	}
}

int CNetCtrlCall::ConnectTransServ()
{	
	if(0 != m_TransConnector.create(m_TransServAddr.ip_,m_TransServAddr.port_,&m_TransTcpEvent))
	{
		_ERROR("CNetCtrlCall::ConnectTranTcpServ  m_TransConnector->create error !");
		return 1;
	}
	return 0;
}


int CNetCtrlCall::ConnectAuthServ()
{
	if(0 != m_AuthConnector.create(m_AuthServAddr.ip_,m_AuthServAddr.port_,&m_AuthTcpEvent))
	{
		_ERROR("CNetCtrlCall::ConnectAuthServ  m_AuthConnector->create error !");
		return 1;
	}
	printf("m_AuthConnector->getsock=[%d]\n",m_AuthConnector.getSock());
	return 0;

}
int CNetCtrlCall:: CloseTcpAuthServConnect()
{
	//printf("CloseTcpAuthServConnect\n");
	return m_AuthConnector.close();
}

int CNetCtrlCall::TcpTransServConnectException()
{
#if Debug_MyTrance_
	std::cout << "CNetCtrlCall::TcpTransServConnectException" << std::endl;
#endif
	return 0;
}

int CNetCtrlCall:: CloseTcpTransServConnect()
{
	return m_TransConnector.close();
}


int CNetCtrlCall::SendAuthRequest()
{
	CSockStream req_stream;
	unsigned short wCertType = 0;
	unsigned short pduType = PDU::MAINCERT_REQ_FROMCLI;

	req_stream << wCertType ;
	req_stream.writeString(m_szUserId);
	req_stream.writeString(m_szUserPassword);
#if 0
	req_stream << m_myselfInfo.getStatus();			//add 2011-7-20 //begin modify 2012-12-3
#else
	unsigned long userType =  m_myselfInfo.getStatus();
	userType = userType >> 16;  		
	req_stream << userType; 		//end modify
#endif

	SendTcpDataToAuthServ(pduType,req_stream.getData(),req_stream.length());
	return 0;	
}

int CNetCtrlCall::NetACSendDataToServ(unsigned short pduType,const void* lpData,unsigned long dwSize,bool bSync )
{	
	unsigned long   pmid = 0;
	if(bSync)
	{
		pmid = m_packetMgr.get();
	}
	CSockStream out_stream(lpData,dwSize);
	out_stream << pmid ;
	out_stream.seek(0);
	
	if(bSync)
	{
		m_packetMgr.set(pmid,pduType,out_stream.getCurr(),out_stream.left());
	}
	int nRet = SendTcpDataToAuthServ(pduType,out_stream.getCurr(),out_stream.left());
	if (0 == nRet)
	{
		return pmid;
	}
	else
	{
		return -1;
	}
}

int CNetCtrlCall::NetACSendDataToDB(unsigned short pduType,const void* lpData,unsigned long dwSize ,bool bSync)
{
	return NetACSendDataToServ(pduType,lpData,dwSize,bSync);
}

int CNetCtrlCall::SendTcpDataToAuthServ(unsigned short pdutype,const void* buf, unsigned long len) //add 2-13-3-5
{
	return m_AuthConnector.sendPacket(pdutype,buf,len);				
}

int CNetCtrlCall::SendTcpDataToTransServ(unsigned short pdutype,const void* buf, unsigned long len) //add 2-13-3-5
{
	return m_TransConnector.sendPacket(pdutype,buf,len);				
}



int CNetCtrlCall::SendLoginRequest()
{	
	CSockStream req_stream;
	unsigned short pduType = PDU::USERLOGIN_REQ_FROMCLI;
	req_stream << m_myselfInfo;
	//SendUdpDataToAuthServ(pduType,req_stream.getData(),req_stream.length());//modify 2014-4-24
	SendTcpDataToAuthServ(pduType,req_stream.getData(),req_stream.length());
	return 0;
}

int CNetCtrlCall::SendLogoutRequest()
{	
	CSockStream req_stream;
	unsigned short pduType = PDU::USERLOGOUT_REQ_FROMCLI;
	unsigned short flag = 0;
	req_stream << m_myselfInfo.getUserID() << flag ;
	//SendUdpDataToAuthServ(pduType,req_stream.getData(),req_stream.length());//modify 2014-4-24
	SendTcpDataToAuthServ(pduType,req_stream.getData(),req_stream.length());
	return 0;
}

int	CNetCtrlCall::SendJoinRequest()
{
	unsigned short pduType = PDU::JOINSESSION_REQ_FROMCLI;
	CSockStream out_stream;
	out_stream << m_myselfInfo;

	SendTcpDataToTransServ(pduType,out_stream.getData(),out_stream.length());

	return 0;
}

int	CNetCtrlCall::SendLeaveRequest()
{
	unsigned short pduType = PDU::LEAVESESSION_REQ_FROMCLI;
	CSockStream out_stream(128);
	//out_stream << m_ResId;
	out_stream << GetMyselfInfo()->getUserID();
	
	SendTcpDataToTransServ(pduType,out_stream.getData(),out_stream.length());
	return 0;
}


int CNetCtrlCall::OnCallBack(unsigned short errcode,bool lockFlag)
{
	if(NULL == m_pCommCallBack)
	{
		return 1;
	}
	unsigned nResult = 0;
	if (0 != errcode)
	{
		m_pCommCallBack->OnLogined(errcode,m_myselfInfo);
		return 1;
	}
	if(lockFlag)
	{
		CMutexKeeper  mutexKeep(m_statusMuex); //add 2012-11-29
		if (NULL == m_pActStatus_)
		{
			return 1;
		}
		nResult = m_pActStatus_->State();
	}
	else
	{
		if (NULL == m_pActStatus_)
		{
			return 1;
		}
		nResult = m_pActStatus_->State();
	}

	if (nResult == 	LOGINSTATE::SNORMAL)
	{
		m_pCommCallBack->OnLogined(0,m_myselfInfo);
	}
	else
	{
		m_pCommCallBack->OnLogined(nResult,m_myselfInfo);			

	}
	return 0;
	
}

int CNetCtrlCall::OnPacketTimeOut(unsigned short pdutype, const void* data, unsigned long len)
{
	if (m_pCommCallBack)
	{
		CSockStream in_stream(data,len);
		unsigned long pmid = 0;
		
		in_stream >> pmid;

		if (m_packetMgr.getOutTimes(pmid) < 5)
		{
			SendTcpDataToAuthServ(pdutype,data,len);
		}
		else
		{	
			unsigned short rspPdu = 0;
			switch(pdutype)
			{
			case  PDU::DBROUTER0_REQ_FROMCLI:
				{
					rspPdu = PDU::DBROUTER0_RSP_TOCLI;
				}
				break;
			case  PDU::OpROUTER0_REQ_FromClient:
				{
					rspPdu = PDU::OpROUTER0_RSP_ToClient;
				}
				break;
			default:
				break;
			}
			if(0 != rspPdu)
			{
				unsigned short result = 1;//mean error
				CSockStream out_stream(len + 64);

				out_stream << result;
				out_stream.writeBytes(data,len);
				if (m_pNetACDataCallBack)
				{
					m_pNetACDataCallBack->OnDataReceived(0,type_CTRL_DATA,out_stream.getData(),out_stream.length(),rspPdu,NULL);
				}
			}
			return 1;	//add 2012-5-14 , if not return it to be a bug
		}
	}
	return 0;
}


