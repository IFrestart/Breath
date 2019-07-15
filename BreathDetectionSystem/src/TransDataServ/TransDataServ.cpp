/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	TransDataServ.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/
#include "TransDataServ.h"

CTransDataServ	g_dbChunnelServ;

CTransDataServ&    GetApp()
{
        return g_dbChunnelServ;
}


CTransDataServ::CTransDataServ()
            :centerSockEvent_(this),
            centerConnector_(&centerSockEvent_)
{
	 pthread_mutex_init(&mutexdownList_,NULL);
}

CTransDataServ::~CTransDataServ()
{
	 pthread_mutex_destroy(&mutexdownList_);
}

int CTransDataServ::initialize(int argc, char** argv)
{
    std::cout << "init start" << std::endl;
 	if(0 != centerConnector_.create(centerIP_.c_str(), centerTcpPort_)) {
                _ERROR("DBCommConnector:Create (IP=%s,Port=%d) Error!", centerIP_.c_str(), centerTcpPort_);
        }
	ACE_Reactor::instance()->register_handler(SIGHUP, this);
	int i = 1;
	m_thread_.start(this,(void*)i);
    std::cout << "init ok" << std::endl;
	return 0;
}

int CTransDataServ::handle_signal(int signum, siginfo_t* siginfot, ucontext_t* ucontextt)
{
	int	ret = 0;

	std::fstream logfile;
	switch(signum) {
        case SIGHUP:
        default:
		ret = CBaseServ::handle_signal(signum, siginfot, ucontextt);
		break;
	}

	return ret;
}

int CTransDataServ::destroy()
{
    return 0;
    //clientTcpAcceptor_.close();
}

//启动定时器
int CTransDataServ::startTimer()
{
	keepTimer_.start(keepCONNTID, keepConnectInterVal, this, 0); 
	return 0;
}

//关闭定时器
int CTransDataServ::stopTimer()
{
	keepTimer_.kill();
	return 0;
}

//定时任务
int CTransDataServ::OnTimer(unsigned long timerID, void* arg)
{
	switch(timerID)
	{
	case keepCONNTID: 
		{
            printf("OnTime\n");
		}
		break;
	default:
		_EMERG("CTransDataServ::OnTimer(%d)->!!!UNKOWN!!!)", timerID);
		break;
	};

	return 0;
}

int CTransDataServ::loadConfig(const char* pFile)
{
    std::cout << "loadconfig start" << std::endl;
	CFileGetPara	ini;

	int		ret = 0;
	if(0 != (ret = ini.InitConfig(pFile))) {
		_ERROR("InitConfig(%s) Error(=%d)!", pFile, ret);
		return -1;
	}
    std::cout << "centerIp load" << std::endl;
	centerIP_ = ini.GetParaString("CenterCtrlServ", "IP", "localhost");
    std::cout << "centerId:" << centerIP_ << std::endl;
	centerTcpPort_ = ini.GetParaInt("CenterCtrlServ", "Port", 0);
	if(0 == centerTcpPort_) {
		_ERROR("ClientAcceptorPort(=%d) Error!", centerTcpPort_);
		return -3;
	}
    std::cout << "centerPort:" << centerTcpPort_ << "loadconfig ok!" << std::endl;
	return 0;
}

int CTransDataServ::sendPacketToCenter(unsigned short pdutype, const void* buf, unsigned long len)
{

        return centerConnector_.sendPacket(pdutype, buf, len);
}


int CTransDataServ::sendPacketToTcpClient(ACE_HANDLE sock,
					unsigned short pdutype, 
					const void* buf, unsigned long len)
{
    //return clientTcpAcceptor_.sendPacket(sock, pdutype, buf, len);
}

int CTransDataServ::OnPacketTimeOut (unsigned short pdutype,
							 const void *data, unsigned long len)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
  std::cerr << " int CTransDataServ::OnPacketTimeOut (unsigned short pdutype" 
        << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% "<< std::endl;
#endif
  _ERROR(" int CTransDataServ::OnPacketTimeOut (unsigned short pdutype ,%%%%%%%%%%%%%%%%%%%%%%% ");

  return 1;
}

void* CTransDataServ::threadProc( void* arg )
{
    while(1)
    {
        unsigned short ctime = 142;
        CSockStream     out_ss(512);
        unsigned short pduType = NHSystemDPROUTER0_REQ_FromDP;
        unsigned short subPdu = SetIpeTime_REQ_ToCli;
        char strTemp[120] = "354701815751934";
        out_ss.writeString(strTemp);
        out_ss << subPdu;
        out_ss << ctime;

        printf("sendPacketToCenter pduType=[%d] subPdu=[%d] ctime[%d]\n",pduType,subPdu,ctime);
        sendPacketToCenter(pduType, out_ss.getData(), out_ss.length());
        printf("outlen:%d\n",out_ss.length());
        sleep(10);
    }
}

int CTransDataServ::noticeDBaddData(const char* device,const char* filename)
{
#if 0
    struct tm  btime;
	unsigned short dataType = 0;
        sscanf(filename,"%d_%d_%d_%d_%d_",&btime.tm_year,&btime.tm_mon,&btime.tm_mday,&btime.tm_hour,&btime.tm_min);

        //printf("yar=[%d]  mon=[%d] day[%d] hour=[%d] min=[%d]\n",btime.tm_year,btime.tm_mon,btime.tm_mday,btime.tm_hour,btime.tm_min);
        btime.tm_year = btime.tm_year - 1900;
        btime.tm_mon = btime.tm_mon - 1;
        btime.tm_sec = 0;

        time_t lbtime = mktime(&btime);
        time_t letime = time(NULL);
	if(NULL != strstr(filename,"nhBreath"))
	{
//printf("noticeDBaddData 111111111111 \n");
		dataType = 1;
	}
	else if(NULL != strstr(filename,"nhStates"))
	{
//printf("noticeDBaddData 222222 \n");
		dataType = 2;
	}
	else
	{
//printf("noticeDBaddData 3333 \n");
		return 1;
	}

        CSockStream     out_ss(512);
	unsigned short pduType = PDU::NHSystemDBROUTER0_REQ_FromNHSystem;
	unsigned short subPdu = AddDataInfo_Req_FromNHSystem;
		
	out_ss << subPdu;
	out_ss.writeString(device);
	out_ss << dataType;
	out_ss << (unsigned long )lbtime;
	out_ss << (unsigned long )letime;
	out_ss.writeString(filename);

//printf("noticeDBaddData dataType=[%d] device=[%s] filename[%s]\n",dataType,device,filename);
	sendPacketToCenter(pduType, out_ss.getData(), out_ss.length());
#endif
    return 0;
}
