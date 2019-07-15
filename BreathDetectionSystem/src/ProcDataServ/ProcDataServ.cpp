/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	CenterCtrlServ.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/
#include "ProcDataServ.h"
#include <string.h>

CProcDataServ	g_dbChunnelServ;

CProcDataServ&    GetApp()
{
        return g_dbChunnelServ;
}

CProcDataServ::CProcDataServ()
			:
            m_dbSockEvent(this),
            m_dbConnector(&m_dbSockEvent),
            m_dbTcpPort(0),
            m_ConnDBCommState(-1)
{
}

CProcDataServ::~CProcDataServ()
{

}

/* From CBaseServ */
int CProcDataServ::initialize(int argc, char** argv)
{

    //连接DBServer
    ConnetDB();
    getUnprocDir();
    m_dbConnectTimer.start(DBCONNTIMERID, DBCOMMCONNTIMERINTERVAL, this, 0);
    //定时处理分析数据
    m_ProcTimer.start(PROCDATATIMERID, PROCDATATIMERINTERVAL, this, 0);

	ACE_Reactor::instance()->register_handler(SIGHUP, this);

	return 0;
}

int CProcDataServ::destroy()
{
    m_dbConnectTimer.kill();
    m_dbConnector.close();
    return 0;
}

//接收外部信号
int CProcDataServ::handle_signal(int signum, siginfo_t* siginfot, ucontext_t* ucontextt)
{
	int	ret = 0;
	return ret;
}

/* From ITimerEventCallBack */
int CProcDataServ::OnTimer(unsigned long timerID, void* arg)
{
    //LOG(DEBUG) << "Timer is :" << timerID;
    switch(timerID)
    {
        case DBCONNTIMERID:
        {
            if(0 != m_ConnDBCommState)
            {
                //_ERROR("DBComm not connected!");
                ConnetDB();
            }
        }
        break;

        case PROCDATATIMERID:
        {
            if(0 != m_ConnDBCommState)
            {
                _ERROR("DBComm not connected!");
            }
            else
            {
                getUnprocDir();
            }
            _DEBUG("ProcDataTimerID()\n");
        }
        break;

        default:
        {
            _ERROR("OnTimer(%d)->UNKOWN!!!)", timerID);
        }
        break;
    };

    return 0;
}

/* Send Packet to DBComm */
int CProcDataServ::sendPacketToDBComm(unsigned short pdutype,
                    const void* buf, unsigned long len)
{
    return m_dbConnector.sendPacket(pdutype, buf, len);
}

//连接DB
int CProcDataServ::ConnetDB()
{
    if (0 != m_dbConnector.create(m_dbIP.c_str(), m_dbTcpPort))
    {
        _ERROR("DBConnector:Create (IP=%s, Port=%d) Error!",
               m_dbIP, m_dbTcpPort);
        return -1;
    }

    SetState(0);
    _INFO("DBConnector Success");
	return 0;
}

//发送数据超时
int CProcDataServ::OnPacketTimeOut (unsigned short pdutype,
                     const void *data, unsigned long len)
{
    _ERROR("int CProcDataServ::OnPacketTimeOut.");
	return 1;
}

//读取配置文件
int CProcDataServ::loadConfig(const char* pFile)
{
    CFileGetPara	ini;

    int		ret = 0;
    if(0 != (ret = ini.InitConfig(pFile)))
    {
        _ERROR("InitConfig(%s) Error(=%d)!", pFile, ret);
        return -1;
    }

    m_dbIP = ini.GetParaString("DBCommServ", "IP", "localhost");
    m_dbTcpPort = ini.GetParaInt("DBCommServ", "Port", 0);
    if(0 == m_dbTcpPort)
    {
        _ERROR("DBCommServPort(=%s) Error!", m_dbTcpPort);
        return -5;
    }

    return 0;
}

//判断是否有未处理文件
void CProcDataServ::getUnprocDir()
{
    unsigned short pdu = getUnprocDir_REQ_TODBCOMM;
    CSockStream buf;
    short tmp = 23;
    buf << tmp;
    sendPacketToDBComm(pdu, buf.getData(), buf.length());
}

//获取某天的文件列表
void CProcDataServ::getOnedayFile(const char* Xlh,time_t t)
{
    getFilelist(Xlh,t);
}

//获取目录下文件列表
void CProcDataServ::getdirFile(const char* Xlh,int num)
{
    getFilelist(Xlh,0,num);
}

//获取文件列表
void CProcDataServ::getFilelist(const char* Xlh,time_t t, long num)
{
    unsigned short pdu = getStatFilelist_REQ_TODBCOMM;
    CSockStream buf;
    buf.writeString(Xlh);
    buf << t;
    buf << num;
    sendPacketToDBComm(pdu, buf.getData(), buf.length());
}
