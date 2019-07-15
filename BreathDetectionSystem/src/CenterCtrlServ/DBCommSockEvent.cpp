/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	DBCommSockEvent.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "PDU.h"
#include "SockStream.h"
#include "Tools.h"
#include "CenterCtrlServ.h"
#include "DBCommSockEvent.h"

#include "ace/Log_Msg.h"
int CDBCommSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	pServ_->connectWithDBComm(sock);
}

int CDBCommSockEvent::OnTCPPacketReceived(ACE_HANDLE sock,
								unsigned short pdutype,
								const void* data, unsigned long len)
{
#ifdef WEBCON_HAS_PRINT_DEBUG
//	std::cerr << "CDBCommSockEvent::OnTCPPacketReceived(" << sock 
//				<< "," << pdutype
//				<< ",[" << data
//				<< "]," << len
//				<< ")" << std::endl;

//	dumpHex(data, len);
#endif 

	unsigned short retpdu = 0;
	short	clientSock = 0 ;
	unsigned short result = 0;
	switch(pdutype)
	{
	case PDU::ROUTER0_RSP_FROMDBCOMM:
		_INFO("CDBCommSockEvent::OnTCPPacketReceived(ROUTER0_RSP_FROMDBCOMM)");
		{
			retpdu = PDU::DBROUTER0_RSP_TOCLI;
			CSockStream	in_stream(data, len) ;
			in_stream >> clientSock;
			in_stream.seek(0);
			in_stream << result;
			in_stream.seek(0);
			pServ_->sendPacketToTcpClient((ACE_HANDLE)clientSock,retpdu, in_stream.getCurr(),in_stream.left());
			
			return 0;
		}
		break;

	case PDU::WINDBROUTER0_RSP_ToCenter:
		_INFO("CDBCommSockEvent::OnTCPPacketReceived(CDBcomm_sp)");
		{
			retpdu = PDU::WINDBROUTER0_RSP_ToCenter;
		}
		break;

/*
	case PDU::GETCHECKCODE_RSP_FROMDBCOMM:
		_INFO("CDBCommSockEvent::OnTCPPacketReceived(GETCHECKCODE_RSP_FROMDBCOMM)");

		{
			retpdu = PDU::GETCHECKCODE_RSP_TOCLI;
		}
		break;
	case PDU::REGISTER_RSP_FROMDBCOMM:
		_INFO("CDBCommSockEvent::OnTCPPacketReceived(REGISTER_RSP_FROMDBCOMM)");

		{
			retpdu = PDU::REGISTER_RSP_TOCLI;
		}
		break;
*/
	case PDU::MAINCERT_RSP_FROMDBCOMM:
		_INFO("CDBCommSockEvent::OnTCPPacketReceived(MAINCERT_RSP_FROMDBCOMM)");

		{
		printf("have recvie from db pdu PDU::MAINCERT_RSP_FROMDBCOMM: \n");
			retpdu = PDU::MAINCERT_RSP_TOCLI;
		}
		break;


	default:
		_ERROR("CDBCommSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
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
			_INFO("CDBCommSockEvent::OnTCPPacketReceived(ROUTER0_RSP_FROMDBCOMM) pdu format error!!");
			return 0;
		}
		pServ_->sendPacketToTcpClient((ACE_HANDLE)clientSock,retpdu, (char*)data + sizeof(short), len - sizeof(short));
	}

	return 0;
}

int CDBCommSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	_INFO("CDBCommSockEvent::OnConnectionClosed(%d)", sock);

//	pServ_->OnDBCommConnectionClosed(sock);

	pServ_->SetState(-1);
	pServ_->ConnetDB();

	return 0;
}

