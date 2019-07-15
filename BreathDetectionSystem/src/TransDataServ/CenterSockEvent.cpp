/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	CenterSockEvent.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "PDU.h"
#include "SockStream.h"
#include "Tools.h"
#include "TransDataServ.h"
#include "CenterSockEvent.h"

#include "ace/Log_Msg.h"
int CCenterSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	//pServ_->connectWithCenter(sock);
}

int CCenterSockEvent::OnTCPPacketReceived(ACE_HANDLE sock,
								unsigned short pdutype,
								const void* data, unsigned long len)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
	std::cerr << "CCenterSockEvent::OnTCPPacketReceived(" << sock 
				<< "," << pdutype
				<< ",[" << data
				<< "]," << len
				<< ")" << std::endl;

//	dumpHex(data, len);
#endif 

	unsigned short retpdu = 0;
	short	clientSock = 0 ;
	unsigned short result = 0;
	switch(pdutype)
	{
		case PDU::NHSystemDBROUTER0_RSP_ToNHSystem:
		{
			CSockStream     in_stream(data, len) ;
                        in_stream >> retpdu >> result;
			if(CTransDataServ::AddDataInfo_Rsp_ToNHSystem == retpdu)
			{
				if(0 != result)
				{
					printf("save data to db error %d)\n", result);
				}
			}
		}
		break;
	default:
		printf("CCenterSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)\n", pdutype);
		_ERROR("CCenterSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
		break;
	}

	return 0;
}

int CCenterSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	_INFO("CCenterSockEvent::OnConnectionClosed(%d)", sock);

	return 0;
}

