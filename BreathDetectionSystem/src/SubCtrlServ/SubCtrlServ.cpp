/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	SubCtrlServ.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/
#include "SubCtrlServ.h"

CSubCtrlServ	g_dbChunnelServ;

CSubCtrlServ&    GetApp()
{
        return g_dbChunnelServ;
}


CSubCtrlServ::CSubCtrlServ()
			:
			clientSockEvent_(this),
			centerSockEvent_(this),
			transSockEvent_(this),
			clientTcpAcceptor_(&clientSockEvent_),
			centerConnector_(&centerSockEvent_),
			transConnector_(&transSockEvent_),
			lServerVersionId_(0),
			bconnectOkTrans_(false)
{
}

CSubCtrlServ::~CSubCtrlServ()
{
}

int CSubCtrlServ::initialize(int argc, char** argv)
{
	if(0 != clientTcpAcceptor_.create(clientIP_.c_str(), clientTcpPort_)) {
		_ERROR("ClientAcceptor:Create (IP=%s,Port=%d) Error!", clientIP_.c_str(), clientTcpPort_);
		return -1;
	}
	if(0 != transConnector_.create(transIP_.c_str(), transTcpPort_)) {
                _ERROR("DBCommConnector:Create (IP=%s,Port=%d) Error!", transIP_.c_str(), transTcpPort_);
        }
	ACE_Reactor::instance()->register_handler(SIGHUP, this);

	return 0;
}

int CSubCtrlServ::handle_signal(int signum, siginfo_t* siginfot, ucontext_t* ucontextt)
{
	int	ret = 0;

	std::fstream logfile;
	switch(signum) {
	case SIGHUP:
		logfile.open("SubCtrlServ.dump",ios::out ) ;
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

int CSubCtrlServ::destroy()
{
	clientTcpAcceptor_.close();
}

int CSubCtrlServ::startTimer()
{
	reportTimer_.start(CenterCONNTID, CenterConnectInterVal, this, 0); //add 2011-8-15
	return 0;
}

int CSubCtrlServ::stopTimer()
{
	reportTimer_.kill();
	return 0;
}

int CSubCtrlServ::connectWithCenter(ACE_HANDLE sock)
{
	reportSelf();	
	startTimer();
}

int CSubCtrlServ::dealWinSysServerReport(ACE_HANDLE sock,const void* data,unsigned long len)
{
	CSockStream in_stream(data,len);
	RSServerID rssid;
	in_stream >> rssid.serverid_ ;
	rssid.sock_ = sock;
	bool needReReport = false;

        RSS_it it ;
        it = std::find(m_serverList.begin(),m_serverList.end(),rssid);
        if(it != m_serverList.end())
        {
                if (it->sock_ != sock)
                {
                	it->sock_ = sock;
			needReReport = true;
                }
        }
        else
        {
                m_serverList.push_back(rssid);
		needReReport = true;
        }
	if(needReReport)
	{
		unsigned short reportPdu = PDU::WinSysServerReport_REQ_FromSub	;
		CSockStream out_stream(MAX_USERID_LEN + 64);
		out_stream << rssid;
		sendPacketToCenter(reportPdu, out_stream.getData(), out_stream.length());
	}
}

int CSubCtrlServ::reportSelf()
{
	unsigned short reportPdu = PDU::SubServerReport_REQ_FromSub;
	unsigned short serverNum = m_serverList.size();

	CSockStream out_stream((serverNum + 1) * (sizeof(RSServerID)));

	out_stream.writeString(serialNumber_);
	out_stream << serverNum;

        RSS_it it ;
        it = m_serverList.begin();
	for(;it != m_serverList.end(); it++)
	{
		out_stream << *it;
	}
	sendPacketToCenter(reportPdu, out_stream.getData(), out_stream.length());
}

int CSubCtrlServ::OnTimer(unsigned long timerID, void* arg)
{
	switch(timerID)
	{
	case CenterCONNTID: //add 2011-8-15
		{
			reportSelf();
		}
		break;
	default:
		_EMERG("CSubCtrlServ::OnTimer(%d)->!!!UNKOWN!!!)", timerID);
		break;
	};

	return 0;
}

int CSubCtrlServ::loadConfig(const char* pFile)
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

	centerIP_ = ini.GetParaString("CenterCtrlServ", "IP", "localhost");

	centerTcpPort_ = ini.GetParaInt("CenterCtrlServ", "TcpPort", 0);
	if(0 == centerTcpPort_) {
		_ERROR("ClientAcceptorPort(=%d) Error!", centerTcpPort_);
		return -3;
	}

	transIP_ = ini.GetParaString("TransDataServ", "IP", "localhost");

	transTcpPort_ = ini.GetParaInt("TransDataServ", "TcpPort", 0);
	if(0 == transTcpPort_) {
		_ERROR("ClientAcceptorPort(=%d) Error!", transTcpPort_);
		return -3;
	}

	lServerVersionId_ = ini.GetParaInt("ServerVersionInfo", "VersionId", 0); //add 2010-4-22

	serialNumber_ = ini.GetParaString("SubCtrlServ", "SerialNumber", "001");
	
	return 0;
}


int CSubCtrlServ::sendPacketToCenter(unsigned short pdutype, const void* buf, unsigned long len)
{

        return centerConnector_.sendPacket(pdutype, buf, len);
}

int CSubCtrlServ::sendPacketToTrans(unsigned short pdutype, const void* buf, unsigned long len)
{

        return transConnector_.sendPacket(pdutype, buf, len);
}

int CSubCtrlServ::sendPacketToTcpClient(ACE_HANDLE sock,
					unsigned short pdutype, 
					const void* buf, unsigned long len)
{

	return clientTcpAcceptor_.sendPacket(sock, pdutype, buf, len);
}

int CSubCtrlServ::OnPacketTimeOut (unsigned short pdutype,
							 const void *data, unsigned long len)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
  std::cerr << " int CSubCtrlServ::OnPacketTimeOut (unsigned short pdutype" 
  		<< "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% "<< std::endl;
#endif
  _ERROR(" int CSubCtrlServ::OnPacketTimeOut (unsigned short pdutype ,%%%%%%%%%%%%%%%%%%%%%%% ");

  return 1;
}

void CSubCtrlServ::dumpUser(const char* pre,std::fstream& logfile)
{
}

