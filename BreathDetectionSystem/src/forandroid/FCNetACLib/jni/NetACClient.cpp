/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	NetACClient.cpp
* @author	jjzhou <jjzhou@webcon.com.cn>

* @short 
**/

#ifndef __LINUX__
#include "stdafx.h"
#endif


#include <iostream>
#include <fstream>
#include <string>

//#include "PDU.h"

#include "NetACClient.h"
#include "NetCtrlCall.h"
#include "FileGetPara.h"
#include "Tools.h"


//#ifdef  __TXT_MODE__
//NetACClient	g_testNetAC;
//
//NetACClient&	GetApp()
//{
//	return g_testNetAC;
//}
//
//#else
//
//int main(int argc,char* argv[])
//{
//	NetACClient  myTest;
//	int nRet = myTest.doMain(argc,argv);
//	return 0;
//}
//#endif





#ifdef __LINUX__
NetACClient::NetACClient() : m_pNetCtrlCall(NULL),m_pUserData(NULL),m_pDBData(NULL),m_nDBDataLen(0)
#else
NetACClient::NetACClient() : m_hInstComm(NULL) , m_pCallBack(NULL),m_pNetCtrlCall(NULL),m_pUserData(NULL),m_pDBData(NULL),m_nDBDataLen(0)
#endif

{
	memset(&m_szUserId,0,sizeof(m_szUserId));
	memset(&m_szPassword,0,sizeof(m_szPassword));
	timer_id = -1;
	m_loginFlag = -1;
	m_bInitOk = false;
	m_logOutFlag = false;
	m_logOutOk = false;
	memset(m_currentUser,0,sizeof(m_currentUser)); //add 2013-5-9
}

NetACClient::~NetACClient()
{
	destroy(); //notice here,when end doMain will call destory

	if (m_pDBData)
	{
		delete[] m_pDBData;
		m_pDBData = NULL;
	}
}


void NetACClient::LoadCommLib()
{
#ifdef __LINUX__
	if(NULL == m_pNetCtrlCall)
	{
		m_pNetCtrlCall = (IBaseCommCall *)CreateNetCtrlObj();
	}
#else
	assert(!m_hInstComm);
	m_hInstComm = LoadLibrary(_T("NetCtrlDll.dll"));
	if(m_hInstComm)
	{
		PFN_NETCTRLOBJ_CREATE pfnCommCreate = (PFN_NETCTRLOBJ_CREATE)GetProcAddress(m_hInstComm, _T("CreateNetCtrlObj"));
		if(pfnCommCreate)
		{
			m_pNetCtrlCall = pfnCommCreate();
		}
	}
#endif
}

void NetACClient::FreeCommLib()
{
#ifdef __LINUX__
	if(NULL != m_pNetCtrlCall)
	{
		DestroyNetCtrlObj(m_pNetCtrlCall);
		m_pNetCtrlCall = NULL;
	}
#else
	if(m_hInstComm)
	{
		PFN_NETCTRLOBJ_DESTROY pfnCommDestroy = (PFN_NETCTRLOBJ_DESTROY)GetProcAddress(m_hInstComm, _T("DestroyNetCtrlObj"));
		if(pfnCommDestroy)
		{
			assert(m_pNetCtrlCall);
			pfnCommDestroy(m_pNetCtrlCall);
		}

		m_pNetCtrlCall = NULL;

		FreeLibrary(m_hInstComm);
		m_hInstComm = NULL;
	}
#endif
}

#ifdef USE_ACE_VER
int NetACClient::handle_signal(int signum, siginfo_t* siginfot, ucontext_t* ucontextt)
{
	int	ret = 0;

	//std::fstream logfile;
	switch(signum) {
	//case SIGHUP:
		/*logfile.open("MeetingInfo.dump",ios::out ) ;
		logfile << "" << std::endl;
		GroupManager::getInstance().dump("Groups   Dump: ",logfile);
		logfile << "" << std::endl;
		SessionManager::getInstance().dump("Sessions Dump: ",logfile);
		logfile << "" << std::endl;
		logfile.close();*/

		break;
	//case SIGUSR1:
		//dump4Agent();
		//break;

	//case NetAC_Msg_LogOut:
	//	std::cout << "have catch hhhhhhhh NetAC_Msg_LogOut" << std::endl;
	//	std::cout << "11111111111111111111 bbbb logout" << std::endl;
	//	LogOut();
	//	std::cout << "2222222222222 after logout" << std::endl;
	//	break;
	default:
		ret = CBaseServ::handle_signal(signum, siginfot, ucontextt);
		break;
	}

	return ret;
}
#endif

int NetACClient::initialize(int argc, char** argv)
{
	_INFO("NetACClient::initialize!");

	LoadCommLib();
	if (NULL == m_pNetCtrlCall)
	{
		printf("initialize  error m_pNetCtrlCall == NULL Error!");
		m_initEvent.setEvent();
		return 1;
	}
	m_bInitOk = true;
	//ACE_Reactor::instance()->register_handler(SIGUSR1, this);
	
	//ACE_Reactor::instance()->register_handler(NetAC_Msg_LogOut, this);

	m_initEvent.setEvent();
	return 0;
}

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
int NetACClient::SendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,unsigned short& retPdu,const char*& out_data,unsigned long& out_Len)
{
	int nRet = 0;
	if (NULL == m_pNetCtrlCall)
	{
		return -1;		
	}
	m_currentdbPmid = m_pNetCtrlCall->NetACSendDataToDB(pdu,in_data,in_Len);
	if (m_currentdbPmid < 0)
	{
		return -2;
	}
	unsigned long tmpWtime = 10000;
	m_dbEvent.waitEvent(tmpWtime);

	CMutexKeeper  mutexDbData(m_DBDataMuex);

	if (!m_pDBData)
	{
		nRet = -3;
		goto errorToDb;		
	}

	retPdu = m_currentRetPdu;
	out_Len = m_nDBDataLen;
	out_data = m_pDBData;
	return 0;

errorToDb:
	delete [] m_pDBData;
	m_pDBData = NULL;
	m_nDBDataLen = 0;
	return nRet;	
}

int NetACClient::SendDataToServ(unsigned short pdu,const char* data,unsigned long len)
{
	if (NULL == m_pNetCtrlCall)
	{
		return -1;		
	}
	return m_pNetCtrlCall->NetACSendDataToServ(pdu,data,len);
}

int NetACClient::waitInit()
{
	if (m_bInitOk)
	{
		return 0;
	}	

	m_initEvent.waitEvent(TIME_OUT_INFINITE);

	if (m_bInitOk)
	{		
		return 0;
	}
	else
	{
		std::cout << "NetACClient::Init error  " << __FILE__ << "::" << __LINE__ << std::endl;
		return -1;
	}
	
}

int NetACClient::waitLogOut()
{
	m_logOutFlag = true;
	m_pCallBack = NULL; //add 2013-4-19
	if (m_logOutOk)
	{
		return 0;
	}	
	m_logoutEvent.waitEvent(TIME_OUT_INFINITE);

	if (m_logOutOk)
	{		
		return 0;
	}
	else
	{
		std::cout << "NetACClient::waitLogOut error  " << __FILE__ << "::" << __LINE__ << std::endl;
		return -1;
	}

}

int NetACClient::LogIn(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime)
{
	if (!m_bInitOk)
	{
		std::cout << "NetACClient ::LogIn error because not  initialize sucess  " << std::endl;
		return -1;
	}

	int nRet = 0;
	m_localPort = fcNetAcInfo.nLocalPort ;
	m_servAddr_.ip_ = ip_to_long(fcNetAcInfo.szServerIp);
	m_servAddr_.port_ = NetAC_TCP_PORT;

	//nRet = initRessUnit(fcNetAcInfo.nType_);
	//if (0 != nRet)
	//{
	//		std::cout << "NetACClient ::LogIn not find rssid type=[   "<< fcNetAcInfo.nType_ << "]" << std::endl;
	//		return -1;
	//}
	m_logOutFlag = false;		//modif a bug for reLogin 2011-8-1
	m_logOutOk = false;			//modify a bug for reLogin 2011-8-1
	nRet = m_pNetCtrlCall->NetACInitialize(m_servAddr_,this,m_localPort);
	if (0 != nRet)
	{
		_ERROR("nRet(=%d) Error!", nRet);
		std::cout << "call m_pNetCtrlCall->NetACInitialize error  " << __FILE__ << "::" << __LINE__ << std::endl;
		return nRet;
	}
	
	//memcpy(&m_szUserId, &fcNetAcInfo.szAccount_, MAX_USERID_LEN);
	//memcpy(&m_szPassword, &fcNetAcInfo.szPasswd_, MAX_USER_PASSWORD_LEN);
	strncpy(m_szUserId, fcNetAcInfo.szAccount_, strlen(fcNetAcInfo.szAccount_) + 1);
	strncpy(m_szPassword, fcNetAcInfo.szPasswd_, strlen(fcNetAcInfo.szPasswd_) + 1);
	
	CSockStream out_stream;
	unsigned short wCertType  = 0;
	out_stream << wCertType;
	out_stream.writeString(m_szUserId);
	out_stream.writeString(m_szPassword);
	out_stream << fcNetAcInfo.nType_;  //add 2014-3-14
	out_stream << fcNetAcInfo.userAttr_ ;
	//out_stream << fcNetAcInfo.userDataLen_;
	//out_stream.writeBytes(fcNetAcInfo.userData_,fcNetAcInfo.userDataLen_);

	m_pCallBack = fcNetAcInfo.pCallBack_;
	m_pUserData = fcNetAcInfo.pUserData_;

	m_loginFlag = -1;
	int nLoginFlag = 0;
	nRet = m_pNetCtrlCall->NetACLogin((BYTE *)out_stream.getData(),out_stream.length(), (BYTE *)&nLoginFlag, (WORD)sizeof(nLoginFlag), this);
	m_loginEvent.waitEvent(OutTime);
	//if (1 == m_loginFlag)
	if (0 == m_loginFlag) //modify 2012-11-7
	{
		return 0;
	}	
	else
	{
		nRet = m_loginFlag;
	}
errhere:
	if (m_pNetCtrlCall)
	{
		m_pNetCtrlCall->NetACUnInitialize();	//add 2011-7-11	
	}
	//return -1;
	if(-1 == m_loginFlag)
	{
		return -7;	//out Timer;
	}
	if(1 == m_loginFlag)
	{
		return 9; //modify 2012-11-7
	}
	return nRet; //modify 2012-11-7
}


int NetACClient::LogOut()
{
	//if (3 != m_loginFlag)  //modify 2011-7-11
	if (-1 == m_loginFlag)
	{
		m_logOutOk = true;
		return 0;
	}
//	m_loginFlag = -1; //modify 2012-12-21 ; because down modif by 2012-11-7 lead to a bug
	//if (m_pNetCtrlCall)  //modify 2012-11-7
	if ((m_pNetCtrlCall) && (0 == m_loginFlag))
	{
#if Debug_MyTrance_
		std::cout << "in NetACClient::LogOut() before call NetACLogout" << std::endl;
#endif
//		delAllRessUnit(); //add 2013-3-29 //modif a bug 2013-4-19 because every ressUnit is callback poit,del it callback will crash
		m_pNetCtrlCall->NetACLogout();

#if Debug_MyTrance_
		std::cout << "in NetACClient::LogOut() after call NetACLogout" << std::endl;
#endif
	}
	m_loginFlag = -1; //add 2012-12-21 ;
	m_logOutOk = true;
	//if (m_pCallBack) 
	if ((m_pCallBack)&& (m_bInitOk))  //modify 2013-10-25
	{
		m_pCallBack(MSGTYPE_OnLogOut,m_pUserData,0,0,NULL,NULL,0,0,3, 0);//add 2013-1-10
	}
	m_pCallBack = NULL;
	m_logoutEvent.setEvent();

	return 0;
		
}

int NetACClient::destroy()
{
	if (!m_bInitOk)
	{
		return 0;
	}
	m_bInitOk = false;
#if Debug_MyTrance_
	std::cout << "in NetACClient::destroy before call LogOut" << std::endl;
#endif
	LogOut();
#if Debug_MyTrance_
	std::cout << "in NetACClient::destroy after call LogOut" << std::endl;
#endif

#if Debug_MyTrance_
	std::cout << "in NetACClient::destroy before call CBaseServ::destroy()" << std::endl;
#endif

	//CBaseServ::destroy();//modify 2012-7-18

#if Debug_MyTrance_
	std::cout << "in NetACClient::destroy after call CBaseServ::destroy()" << std::endl;
#endif

	if (m_pNetCtrlCall)
	{
#if Debug_MyTrance_
		std::cout << "in NetACClient::destroy before call NetACUnInitialize" << std::endl;
#endif
		m_pNetCtrlCall->NetACUnInitialize();

#if Debug_MyTrance_
		std::cout << "in NetACClient::destroy after call NetACUnInitialize" << std::endl;
#endif

#if Debug_MyTrance_
		std::cout << "in NetACClient::destroy bfore call FreeCommLib" << std::endl;
#endif

#ifdef USE_ACE_VER
		CBaseServ::destroy();  //add 2012-7-18
#endif

		FreeCommLib();
#if Debug_MyTrance_
		std::cout << "in NetACClient::destroy after call FreeCommLib" << std::endl;
#endif

	}
	return 0;
}

int NetACClient::loadConfig(const char* pFile)
{
	return 0;
}


/** ��ȡ��֤��ص�����
* @param nResult ���(0-�ɹ�,<0ʧ��)
* @param dwKey У������
* @param lpCode У����(����)
* @param wCodeSize У���볤��
*/
int NetACClient::OnUserRegister0(short nResult, DWORD dwKey, const BYTE* lpCode,	WORD wCodeSize)
{
	return 0;
}

/** ���û�ע��ص�����
* @param nResult ���(0-�ɹ�,<0ʧ��)
* @param lpszAccount �û��ʺ�
*/
int NetACClient::OnUserRegister1(short nResult, LPCTSTR lpszAccount)
{
	return 0;
}

/** ��¼�ص�����
* @param nResult ���(0-�ɹ�,<0ʧ��)
* @param wAreaIndex �������
* @param dwUserIndex �û����
* @param dwUserStatus �û�״̬
* @param dwUserFlag �û���־
* @param lpData ���������ص�����
* @param dwDataSize ���ݳ���
* @param lpCommonComponentCall ���ݷ��Ϳؼ�
*/
//int NetACClient::OnLogined(short nResult, const UserInfo& userInfo)
int NetACClient::OnLogined(short nResult, const UserInfoEx& userInfo)
{
#ifdef	VER_FOR_ANDROID_
	printf("NetACClient::OnLogined  nResult =[%d] pthreadid=[%d]\n",nResult, pthread_self());
#else
	printf("NetACClient::OnLogined  nResult =[%d] pthreadid=[%d]\n",nResult, ACE_OS::thr_self());
#endif
	int nRet = 0;
	//m_loginFlag = 1; //modif 2011-7-11

	if (0 == nResult)
	{
		logOutTimer_.start(LOGOUT_TIMER_ID,LOGOUT_TIMER_INTERVAL,this,0);
		//m_loginFlag = 1;		//add 2011-7-11	
		m_loginFlag = 0;		//modify 2012-11-7	
	}
	else
	{
		nRet = 1;
		//m_loginFlag = -1;		//add 2011-7-11
		m_loginFlag = nResult;		//modify 2012-11-7
	}

	m_loginEvent.setEvent();

	return 0;
}

int NetACClient::OnTimer(unsigned long timerID, void* arg)
{
	switch(timerID)
	{
	case LOGOUT_TIMER_ID:		//add 2011-7-26
		{
			if (m_logOutFlag)
			{
				std::cout << "11111111111111111111 bbbb logout" << std::endl;
				LogOut();
				std::cout << "2222222222222 after logout" << std::endl;
				return 1;
			}
		}
		break;;
	case KEEPALIVE_TIMER_ID:
		{
			//state_->keepAliveWithParent();
			char sendBuf[128] = "hello , how are you?";
			int nret = 0;
		//	nret = sendPacketToServ(123,sendBuf,strlen(sendBuf));
			std::cerr << "OnTimer have send (datalen = " << strlen(sendBuf) << ")" << std::endl;

		}
		break;

	default:
		_ERROR("TestClient::OnTimer(%d->!!!UNKOWN!!!)", timerID);
		break;
	};

	return 0;
}


void*  NetACClient::threadProc(void* arg)
{
	NetACClient* pTest = (NetACClient*)arg;
	while (1)
	{
#ifdef VER_FOR_ANDROID_
		usleep(1000);
#else
		ACE_OS::sleep(ACE_Time_Value(1, 1000));
#endif
	}
}

//�̳��� INetACDataCallBack

/** �������ݻص�����
* @param gid   ����id; id == 0 ��ʾ�ǻ������ݻص�; id > 0 �������ݻص�
* @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
* @param lpData �յ�������
* @param dwSize ���ݳ���
* @param wPduType PDU����
* @param lpSender ������������Ϣ
*/
int NetACClient::OnDataReceived(unsigned short gid,unsigned short type,const void* lpData, DWORD dwSize, WORD wPduType,const UserID* lpSender)
{
	//_INFO("NetACClient::OnDataReceived");
	////����� ֻ���������ݿ�ͨ�����ݻص�ʱ����
	
	//char tmpAccount[MAX_USERID_LEN]={0};
	unsigned long tmpPid = 0;
	unsigned short result = 0;
	if(PDU::TransData_REQ_ToClient == wPduType)
	{
		//m_pCallBack(MSGTYPE_OnReceivedData,m_pUserData,gid,type,tmpAccount,lpData,dwSize,wPduType,0,0);
		m_pCallBack(MSGTYPE_OnReceivedData,m_pUserData,gid,type,NULL,lpData,dwSize,wPduType,0,0);
		return 0;
	}

	else if((PDU::DBROUTER0_RSP_TOCLI == wPduType) || (PDU::OpROUTER0_RSP_ToClient == wPduType))
	{
		CSockStream in_stream(lpData,dwSize);
		in_stream >> result >> tmpPid ;
		if(0 != tmpPid) //add 2014-5-26
		{
			{
				CMutexKeeper  mutexDbData(m_DBDataMuex);
				if (tmpPid != m_currentdbPmid)
				{
					return 1;
				}
				if (m_pDBData)
				{
					delete [] m_pDBData;
					m_pDBData = NULL;
				}
				m_nDBDataLen = dwSize;
				m_pDBData = new char[m_nDBDataLen];
				m_currentRetPdu = wPduType;

				memcpy(m_pDBData,(char*)lpData + sizeof(unsigned long),m_nDBDataLen);
			}
			m_dbEvent.setEvent();
			return 0;
		}
	 	else //add 2014-5-26
		{
			//m_pCallBack(MSGTYPE_OnReceivedData,m_pUserData,gid,type,tmpAccount,lpData,dwSize,wPduType,0,0);
			m_pCallBack(MSGTYPE_OnReceivedData,m_pUserData,gid,type,NULL,lpData,dwSize,wPduType,0,0);
			return 0;
		}
	}

	return 0;
}


/** ��Ա�ı�ص�����
* @param gid   ����id; id == 0 ��ʾ�ǻ����Ա�ص�; id > 0 �����Ա�ص�
* notice  ����3�������쳣ʱ ����lpMemberUserΪNULL
* @param nRosterFlag �ı��־
* 0:p2p����  1: ������ת������   2: �뿪  3: �������쳣 4 ����״̬�ı�  5 �˺���һ�ط���½
* @param lpMemberUser �ı�ĳ�Ա������Ϣ
*/

//int NetACClient::OnMemberRoster(unsigned short gid,unsigned short type,short nRosterFlag, const UserInfoEx* lpMemberUser)
int NetACClient::OnMemberRoster(unsigned short gid,short nRosterFlag, const UserInfoEx* lpMemberUser)
{
	int nRet = 0;
	if (3 == nRosterFlag)	//����� ֻ�����ڸ�ת��������keepalive ��ʱ��,�����û��ص�½ǰ�߱��������߳�ʱ����
	{
		if (m_pCallBack)
		{
			//m_pCallBack(MSGTYPE_OnLogOut,m_pUserData,gid,0,NULL,NULL,0,0,nRosterFlag, 0);//modify 2013-1-10
			std::cout << " 11111111 OnMemberRoster m_logOutFlag = true  nRosterFlag == 3" << std::endl;
			m_logOutFlag = true;
		}
	//	clearUser();//modify 2012-7-11
		return 0;
	}
	else if (5 == nRosterFlag)   //add 2013-2-5
	{
		if (m_pCallBack)
		{
			m_pCallBack(MSGTYPE_OnSecondLogined,m_pUserData,0,0,NULL,NULL,0,0,nRosterFlag, 0);  //modify 2013-1-10
			std::cout << " 11111111 OnMemberRoster m_logOutFlag = true  nRosterFlag == 5" << std::endl;
			m_logOutFlag = true;
		}
		return 0;
	}
	else
	{
		if (NULL == lpMemberUser)
		{
			std::cout << "NetACClient ::OnMemberRoster   error NULL == lpMemberUser nRosterFlag[" << nRosterFlag << "]" << std::endl;		
			return 1;
		}		
		if ((0 == nRet) && m_pCallBack)
		{
			//m_pCallBack(MSGTYPE_OnMemberRoster,m_pUserData,gid,0,lpMemberUser->getAccount(),lpMemberUser->getLpData(),lpMemberUser->getLpDataLen(),
			//			0,nRosterFlag, lpMemberUser->getStatus());
		}
	}
	return 0;
}


