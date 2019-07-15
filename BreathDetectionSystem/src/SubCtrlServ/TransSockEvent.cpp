/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	TransSockEvent.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "PDU.h"
#include "SockStream.h"
#include "Tools.h"
#include "SubCtrlServ.h"
#include "TransSockEvent.h"

#include "ace/Log_Msg.h"
int CTransSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	pServ_->setConnectOkTrans(true);
}

int CTransSockEvent::OnTCPPacketReceived(ACE_HANDLE sock,
								unsigned short pdutype,
								const void* data, unsigned long len)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
	std::cerr << "CTransSockEvent::OnTCPPacketReceived(" << sock 
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
	case PDU::TransData_RSP_ToSub:
		_INFO("CTransSockEvent::OnTCPPacketReceived(TransData_RSP_ToSub)");
		{
			retpdu = PDU::TransData_RSP_ToWin ;
		}
		break;

	default:
		_ERROR("CTransSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
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
			_INFO("CTransSockEvent::OnTCPPacketReceived(TransData_RSP_ToSub) pdu format error!!");
			return 0;
		}
		pServ_->sendPacketToTcpClient((ACE_HANDLE)clientSock,retpdu, (char*)data + sizeof(short), len - sizeof(short));
	}

	return 0;
}

int CTransSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	_INFO("CTransSockEvent::OnConnectionClosed(%d)", sock);
	pServ_->setConnectOkTrans(false);

	return 0;
}

