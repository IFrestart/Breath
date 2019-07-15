/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	DBCommSockEvent.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "PDU.h"
#include "SubPDU.h"
#include "ace/Log_Msg.h"
#include "SockStream.h"
#include "ProcDataServ.h"
#include "DBCommSockEvent.h"
#include "FileProc.h"
#include "Logger.h"

using namespace PDU;
using namespace SubPDU;

#define RSP_SUCCESS 0

int CDBCommSockEvent::OnNewConnection(ACE_HANDLE sock)
{
    //pServ_->connectWithDBComm(sock);
    return 0;
}

int CDBCommSockEvent::OnTCPPacketReceived(ACE_HANDLE sock,
								unsigned short pdutype,
                                const void* data, unsigned long len)
{
    //_DEBUG("TCPPacketReceived sock:%d, pdutype:%d, len:%d", sock, pdutype, len);
//dumpHex(data, len);


    unsigned short retpdu = 0;
	short	clientSock = 0 ;
    unsigned short result = 0;

	switch(pdutype)
	{
        case getUnprocDir_RSP_FROMDBCOMM:
        {
            short nRet;
            unsigned short wCount;
            CSockStream ar ((BYTE *) data, len);
            char   dirFile[63];

            ar >> nRet;
            if( nRet == RSP_SUCCESS)
            {
                ar >> wCount;
                int i = 0;
                for(i; i<wCount; i++)
                {
                    memset(dirFile, 0, sizeof(char)*63);
                    ar.readString(dirFile, sizeof(dirFile));
                    _DEBUG("dirFile:%s", dirFile);
                    const char* dir=dirFile;
                    pServ_->getdirFile(dir,CProcDataServ::FILENUM);
                    //_DEBUG("getdirFile send -------------------------------------------");
                }
            }
        }
        break;

        case getStatFilelist_RSP_FROMDBCOMM:
		{
            //_DEBUG("CDBSockEvent::OnTCPPacketReceived(PDU:%d)", getStatFilelist_RSP_FROMDBCOMM);
            short nRet;
            unsigned short wCount;
            CSockStream ar ((BYTE *) data, len);
            char filePath[255];
            char filename[63];
            char filedir[63];

            ar >> nRet;
            if( nRet == RSP_SUCCESS)
            {
                ar >> wCount;
                int i = 0;
                ar.readString(filedir, sizeof(filedir));
                _DEBUG("dir:%s",filedir);
                for(i; i<wCount; i++)
                {
                    memset(filePath,0,sizeof(filePath));
                    ar.readString(filename, sizeof(filename));
                    strcat(filePath,DATADIR);
                    strcat(filePath,filedir);
                    strcat(filePath,"/");
                    strcat(filePath,filename);               
                    SlStArr aArray;
                    statArr sArr;
                    StatFileProc    proc;
                    proc.getStat_BaseSmoothPara(filePath,aArray,sArr);
                    unsigned long num = aArray.size();
                    CSockStream arDB;
                    arDB.writeString(filedir);
                    arDB << num;
                    for(int j=0; j<num; j++)
                    {
                        arDB << aArray[j].dwTime;
                        arDB << aArray[j].dwBreathRate;
                        arDB << aArray[j].dwAHIS;
                        arDB << aArray[j].dwAHIO;
                        arDB << aArray[j].wSleepStat1;
                        arDB << aArray[j].wSleepStat2;
                        arDB << aArray[j].dwRespRate;
                        arDB << aArray[j].dwHeartRate;
                        _DEBUG("time:%d, Stat1:%d, Stat1:%d, RespRate:%d, HeartRate:%d" ,
                               aArray[j].dwTime ,aArray[j].wSleepStat1, aArray[j].wSleepStat1, aArray[j].dwRespRate, aArray[j].dwHeartRate);
                    }
                    pServ_->sendPacketToDBComm(inStat_BaseSmoothPara_REQ_TODBCOMM, arDB.getData(), arDB.length());
                    //_DEBUG("inStat_BaseSmoothPara send ------------------------------------------");
                }
            }
		}
        break;

        case inStat_BaseSmoothPara_RSP_FROMDBCOMM:
        {
            short nRet;
            CSockStream ar ((BYTE *) data, len);
            ar >> nRet;
//            if(0 != nRet)
//            {
//                LOG(ERROR) << "inStat_BaseSmoothPara nRet:" << nRet;
//            }
        }
        break;

	default:
        //_ERROR("CDBSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
		break;
	}

	if( 0 != retpdu)
	{
		CSockStream	stream(data, len) ;
		try
		{		
			stream >> clientSock ;
		}
		catch(CSockStream::exception e)
		{
            //_INFO("CDBSockEvent::OnTCPPacketReceived(ROUTER0_RSP_FROMDBCOMM) pdu format error!!");
			return 0;
		}
        //pServ_->sendPacketToTcpClient((ACE_HANDLE)clientSock,retpdu, (char*)data + sizeof(short), len - sizeof(short));
	}

	return 0;
}

int CDBCommSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{

//  pServ_->OnDBCommConnectionClosed(sock);
	pServ_->SetState(-1);
    _INFO("DBConnector closed!");
	return 0;
}
