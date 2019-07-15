/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	CenterCtrlServ.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/
#include "CenterCtrlServ.h"

CCenterCtrlServ	g_dbChunnelServ;

CCenterCtrlServ&    GetApp()
{
        return g_dbChunnelServ;
}


CCenterCtrlServ::CCenterCtrlServ()
			:
			clientSockEvent_(this),
			dbCommSockEvent_(this),
			clientTcpAcceptor_(&clientSockEvent_),
			dbCommConnector_(&dbCommSockEvent_),
			dbCommPort_(0),
			lServerVersionId_(0), //add 2010-4-22
			m_state(-1)
{
}

CCenterCtrlServ::~CCenterCtrlServ()
{
}

int CCenterCtrlServ::initialize(int argc, char** argv)
{
	if(0 != clientTcpAcceptor_.create(clientIP_.c_str(), clientTcpPort_)) {
		_ERROR("ClientAcceptor:Create (IP=%s,Port=%d) Error!", clientIP_.c_str(), clientTcpPort_);
		return -1;
	}

	if(0 != dbCommConnector_.create(dbCommIP_.c_str(), dbCommPort_)) {
		_ERROR("DBCommConnector:Create (IP=%s,Port=%d) Error!", dbCommIP_.c_str(), dbCommPort_);
		dbCommConnTimer_.start(DBCOMMCONNTIMERID, DBCOMMCONNTIMERINTERVAL, this, 0);
	}
	else
	{
		m_state = 0;
	}
	keepAliveTimer_.start(KeepAliveClientID, KeepAliveClientInterVal, this, 0); //add 2011-8-15
	ACE_Reactor::instance()->register_handler(SIGHUP, this);

	return 0;
}

int CCenterCtrlServ::handle_signal(int signum, siginfo_t* siginfot, ucontext_t* ucontextt)
{
	int	ret = 0;

	std::fstream logfile;
	switch(signum) {
	case SIGHUP:
		logfile.open("CenterCtrlServ.dump",ios::out ) ;
		logfile << "" << std::endl;
		dumpUser("UserInfo   Dump: ",logfile);
		logfile << "" << std::endl;
		logfile.close();
		break;
	default:
		ret = CBaseServ::handle_signal(signum, siginfot, ucontextt);
		break;
	}

	return ret;
}

int CCenterCtrlServ::destroy()
{
	clientTcpAcceptor_.close();
	dbCommConnTimer_.kill();
	dbCommConnector_.close();
}

int CCenterCtrlServ::OnTimer(unsigned long timerID, void* arg)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
	//std::cerr << "Timer is :  " << timerID << std::endl;
#endif
	switch(timerID)
	{
	case DBCOMMCONNTIMERID:
#ifdef WEBCON_HAS_PRINT_DEBUG
		std::cerr << "connect to dbcom  : " << 
				dbCommIP_ << "::" << dbCommPort_  << "..........."<< std::endl;
#endif
		if(0 == dbCommConnector_.create(dbCommIP_.c_str() , dbCommPort_)) {
			m_state = 0;
			return 1;
		}
		break;
	case KeepAliveClientID: //add 2011-8-15
		{
			checkKeepAliveForClient();
		}
		break;
	default:
		_EMERG("CCenterCtrlServ::OnTimer(%d)->!!!UNKOWN!!!)", timerID);
		break;
	};

	return 0;
}

int CCenterCtrlServ::loadConfig(const char* pFile)
{
	CFileGetPara	ini;

	int		ret = 0;
	if(0 != (ret = ini.InitConfig(pFile))) {
		_ERROR("InitConfig(%s) Error(=%d)!", pFile, ret);
		return -1;
	}
	
	
	clientIP_ = ini.GetParaString("ClientAcceptor", "IP", "localhost");

	clientTcpPort_ = ini.GetParaInt("ClientAcceptor", "TcpPort", 0);
	if(0 == clientTcpPort_) {
		_ERROR("ClientAcceptorPort(=%d) Error!", clientTcpPort_);
		return -3;
	}
	dbCommIP_ = ini.GetParaString("DBCommServ", "IP", "localhost");
	dbCommPort_ = ini.GetParaInt("DBCommServ", "Port", 0);
	if(0 == dbCommPort_) {
		_ERROR("DBCommServPort(=%d) Error!", dbCommPort_);
		return -5;
	}

	lServerVersionId_ = ini.GetParaInt("ServerVersionInfo", "VersionId", 0); //add 2010-4-22
	
	return 0;
}

int CCenterCtrlServ::sendPacketToTcpClient(ACE_HANDLE sock,
					unsigned short pdutype, 
					const void* buf, unsigned long len)
{

	return clientTcpAcceptor_.sendPacket(sock, pdutype, buf, len);
}

int CCenterCtrlServ::sendPacketToDBComm(unsigned short pdutype, 
					const void* buf, unsigned long len)
{

	return dbCommConnector_.sendPacket(pdutype, buf, len);
}

void CCenterCtrlServ::OnDBCommConnectionClosed(ACE_HANDLE sock)
{
}


int CCenterCtrlServ::ConnetDB()
{
	dbCommConnTimer_.start(DBCOMMCONNTIMERID, DBCOMMCONNTIMERINTERVAL, this, 0);
	return 0;
}


int CCenterCtrlServ::OnPacketTimeOut (unsigned short pdutype,
					 const void *data, unsigned long len)
{
	#ifdef WEBCON_HAS_PRINT_DEBUG
	std::cerr << " int CCenterCtrlServ::OnPacketTimeOut (unsigned short pdutype" 
	<< "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% "<< std::endl;
	#endif
	_ERROR(" int CCenterCtrlServ::OnPacketTimeOut (unsigned short pdutype ,%%%%%%%%%%%%%%%%%%%%%%% ");

	return 1;
}

int CCenterCtrlServ::clearUserStatus(const UserID user)
{
	USER_Ex_it ex_it = findUser(user);	
	if(ex_it != endUser())
	{
		unsigned long status = ex_it->getStatus();
		status = status & 0xffff00ff;
		ex_it->setStatus(status);
	}
	else
	{
		return -1;
	}
	return 0;
}

void CCenterCtrlServ::dumpUser(const char* pre,std::fstream& logfile)
{
	USER_Ex_it it;
	logfile << pre << "==========allUser num = " << m_userInfo.size() << " ====================" << std::endl;
	for(it = beginUser(); it != endUser(); it++)
	{
		logfile << pre << "========accunt         = " << it->getAccount() << std::endl;
		it->dump(pre,logfile);
		//logfile << pre << "========void_ip=  [" << ip_to_string(it->videoAddr_.ip_) << " port[" << it->videoAddr_.port_ << "]" <<  std::endl;
	}
}

int CCenterCtrlServ::changeUserStatus(const UserID user,unsigned long status)
{
	USER_Ex_it ex_it = findUser(user);	
	if(ex_it != endUser())
	{
		ex_it->setStatus(status);
	}
	else
	{
		return -1;
	}
	return 0;
}

int  CCenterCtrlServ::updateUser(const UserID& user,ACE_HANDLE sock) 
{
	USER_Ex_it it ;
	it = findUser(user);  
	if(it == endUser() )
	{
		return 1;
	}
	it->setSock(sock);
	return 0;
}

int  CCenterCtrlServ::addUser(const UserInfoEx& user) 
{
	USER_Ex_it it ,tmpit;	
	it = std::find(m_userInfo.begin(),m_userInfo.end(),user);
	if(it != m_userInfo.end())
	{
		*it = user;
	}
	else
	{
		m_userInfo.push_back(user);
	}
	return 0;
}

int  CCenterCtrlServ::delUser(const UserID& user)
{
	USER_Ex_it it ,tmpit;	
	it = std::find(m_userInfo.begin(),m_userInfo.end(),user);
	if(it != m_userInfo.end())
	{
		m_userInfo.erase(it);
	}
	else
	{
		return 1;
	}
	return 0;
}

USER_Ex_it  CCenterCtrlServ::beginUser()
{
	return m_userInfo.begin();
}

USER_Ex_it  CCenterCtrlServ::endUser()
{
	return m_userInfo.end();
}

USER_Ex_it  CCenterCtrlServ::findUser(const UserID& user)
{
	return std::find(m_userInfo.begin(),m_userInfo.end(),user);

}

USER_Ex_it  CCenterCtrlServ::findUser(unsigned long userIndex) //add 2013-3-12
{
	//return std::find(m_userInfo.begin(),m_userInfo.end(),userIndex);
	USER_Ex_it it;
	for(it = beginUser(); it != endUser(); it++)
	{
		if(it->getUserID().uid_ == userIndex)
		{
			break;
		}
	}
	return it;
}

USER_Ex_it  CCenterCtrlServ::findUser(const char* szAccount)
{
	if(NULL == szAccount)
	{
		return endUser();
	}
	USER_Ex_it it;
	for(it = beginUser(); it != endUser(); it++)
	{
		if(0 == strncmp(szAccount,it->getAccount(),MAX_USERID_LEN))
		{
			break;
		}
	}
	return it;

}

int CCenterCtrlServ::checkKeepAliveForClient()
{
	USER_Ex_it it;
	unsigned long nowTmpTime = time(NULL);
	for(it = beginUser(); it != endUser(); )
	{
		if((nowTmpTime - it->getTime()) > OutTimeKeepAliveClient)
		{
			std::cout << "outTime for client keepalive user[" << it->getAccount() << "]" << std::endl;
			m_userInfo.erase(it++);
			continue;
		}
		it++;
	}
}

int  CCenterCtrlServ::dealKeepAlive(ACE_HANDLE sock, CSockStream& in_stream)
{
	UserID  user ;
	USER_Ex_it it ;
	//unsigned long  status = 0,flag  = 0;
	//unsigned short num1 = 0,num2 = 0;
	//in_stream >> user >> status >> flag >> num1;

	in_stream >> user ;

	it = findUser(user);  
	if(it == endUser() )
	{
		printf("dealKeepAlive error 11111111 not find user[%d] time[%lu] \n",user.uid_,time(NULL));
		return 0;
	}
	//#ifdef WEBCON_HAS_PRINT_DEBUG
	printf("dealKeepAlive from user[%d] account[%s] time[%lu] \n",user.uid_,it->getAccount(),time(NULL));
	//#endif
	it->setTime();

	return 0;
}

int  CCenterCtrlServ::connectWithDBComm(ACE_HANDLE sock)
{
	return 0;
}

int CCenterCtrlServ::optWinSysServ(unsigned short sock,const void* buf ,unsigned long len)
{
	CSockStream	out_ss(64 + len);
	CSockStream	in_ss(buf,len);
	unsigned short ret = 0;
	unsigned long pmid = 0;
	RSServerID rsid;	

	RSS_it   rsitb,rsite,rsit;
	CRSS_IT crsit;
	
	unsigned short nWinSubSock = 0, nSubCenterSock = 0;

	in_ss >> pmid >> rsid.serverid_;

	for(crsit = map_rss_.begin(); crsit != map_rss_.end(); crsit++)
	{
		rsitb = crsit->second.begin();
		rsite = crsit->second.end();
		rsit =  find (rsitb,rsite,rsid);
		if(rsit != rsite)
		{
			nSubCenterSock = crsit->first;
			nWinSubSock = rsit->sock_;
			break;
		}
	}

	if(0 != nWinSubSock)
	{
		out_ss << nWinSubSock;
		out_ss << sock;
		out_ss << pmid ;
		out_ss.writeBytes(in_ss.getCurr(),in_ss.left());
		sendPacketToTcpClient(nSubCenterSock, PDU::OpROUTER0_REQ_FromCenter, out_ss.getData(), out_ss.length());
	}
	else
	{
		ret = 1;
		out_ss << ret;
		out_ss << pmid ;
		out_ss.writeBytes(in_ss.getCurr(),in_ss.left());
		sendPacketToTcpClient(sock, PDU::OpROUTER0_RSP_ToClient, out_ss.getData(), out_ss.length());
		
	}
	return 0;
}

int CCenterCtrlServ::dealTransData(unsigned short sock,const void* buf ,unsigned long len)
{
	CSockStream	in_ss(buf,len);
	unsigned short ret = 0;
	unsigned short winSubSock = 0, subCenterSock = sock;
	unsigned short subPdu = 0, nUserNum = 0;
	unsigned long index = 0;

	in_ss >> winSubSock >> subPdu >> nUserNum;

	std::vector<unsigned short>  vSock;
	vSock.resize(nUserNum);
	UserID user;

	USER_Ex_it ex_it; 

	for(int i = 0; i < nUserNum ; i++)
	{
		in_ss >> user;
		ex_it = findUser(user);	
		if(ex_it != endUser() )
		{
			vSock[i] = ex_it->getSock();
		}
		else
		{
			vSock[i] = 0;
		}
	}
	index = in_ss.tell();
	in_ss.seek((index - (3 * sizeof(unsigned short))));
	in_ss << subCenterSock << winSubSock << subPdu;
	in_ss.seek(index);


	for(int i = 0; i < nUserNum ; i++)
	{
		if(0 != vSock[i])
		{
			sendPacketToTcpClient(vSock[i], PDU::TransData_REQ_ToClient, in_ss.getCurr(), in_ss.left());
		}
	}

	return 0;
	
}

int CCenterCtrlServ::addWinSysServ(unsigned short sock,const void* buf ,unsigned long len)
{
	CSockStream	out_ss(64);
	CSockStream	in_ss(buf,len);
	unsigned short ret = 0;
	RSServerID rsid;	

	in_ss >> rsid;

	RSS_it   rsitb,rsite,rsit;
	rsitb = map_rss_[sock].begin();
	rsite = map_rss_[sock].end();
	rsit = std::find(rsitb,rsite,rsid);
	if(rsit != rsite)
	{
		*rsit = rsid;
	}
	else
	{
		list_rss_.push_back(rsid);
	}
	
	out_ss << rsid.sock_;
	out_ss << ret;
	sendPacketToTcpClient(sock, PDU::WinSysServerReport_RSP_ToSub, out_ss.getData(), out_ss.length());
	return 0;
}

int CCenterCtrlServ::addSubServ(unsigned short sock,const void* buf ,unsigned long len)
{
	RSS_it   rsit;
	RSServerID rsid;
	
	CSockStream in_stream(buf,len);
	unsigned short nNum = 0 ;
	in_stream >> rsid.serverid_;	
	rsid.sock_ = sock;
	
	rsit = std::find(list_rss_.begin(),list_rss_.end(),rsid);
	if(rsit != list_rss_.end())
	{
		*rsit = rsid;
	}
	else
	{
		list_rss_.push_back(rsid);
	}

	map_rss_[sock].clear();

	in_stream >> nNum;
	for(int i = 0; i < nNum; i++)
	{
		in_stream >> rsid;
		map_rss_[sock].push_back(rsid);
	}
	return 0;
}

