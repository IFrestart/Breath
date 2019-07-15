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
#include "SubCtrlServ.h"
#include "CenterSockEvent.h"

#include "ace/Log_Msg.h"
int CCenterSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	pServ_->connectWithCenter(sock);
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
	case PDU::WINDBROUTER0_RSP_ToSub:
		_INFO("CCenterSockEvent::OnTCPPacketReceived(WINDBROUTER0_RSP_ToSub)");
		{
			retpdu = PDU::WINDBROUTER0_RSP_ToWin;
		}
		break;
	case PDU::TransData_RSP_ToSub:
		_INFO("CCenterSockEvent::OnTCPPacketReceived(TransData_RSP_ToSub)");
		{
			retpdu = PDU::TransData_RSP_ToWin ;
		}
		break;

	case PDU::WinSysServerReport_RSP_ToSub:
		_INFO("CCenterSockEvent::OnTCPPacketReceived(WinSysServerReport_RSP_ToSub)");
		{
			retpdu = PDU::WinSysServerReport_RSP_ToWinSys ;
		}
		break;
	case PDU::OpROUTER0_REQ_FromCenter:
		_INFO("CCenterSockEvent::OnTCPPacketReceived(OpROUTER0_REQ_FromCenter)");
		{
			retpdu = PDU::OpROUTER0_REQ_FromSub ;
		}
		break;


	case PDU::SubServerReport_RSP_ToSub:
		_INFO("CCenterSockEvent::OnTCPPacketReceived(SubServerReport_RSP_ToSub)");
		{
			CSockStream	in_stream(data, len) ;
			in_stream >> result;
			if(0 == result)
			{
				pServ_->stopTimer();
			}
			return 0;
		}
		break;

	default:
		_ERROR("CCenterSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
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
			_INFO("CCenterSockEvent::OnTCPPacketReceived(ROUTER0_RSP_FROMDBCOMM) pdu format error!!");
			return 0;
		}
		pServ_->sendPacketToTcpClient((ACE_HANDLE)clientSock,retpdu, (char*)data + sizeof(short), len - sizeof(short));
	}

	return 0;
}

int CCenterSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	_INFO("CCenterSockEvent::OnConnectionClosed(%d)", sock);

	return 0;
}

