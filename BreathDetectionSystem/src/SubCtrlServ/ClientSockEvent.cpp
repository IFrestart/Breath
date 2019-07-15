/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	ClientSockEvent.cpp
 * @author	jjzhou <jjzhou@webcon.com.cn>

 * @short 
 **/

#include <iostream>
#include "PDU.h"
#include "SockStream.h"
#include "Tools.h"
#include "SubCtrlServ.h"
#include "ClientSockEvent.h"

int CClientSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	++ clientNum_;
	std::cerr << "CClientSockEvent::OnNewConnection(" << sock  << std::endl;

	return 0;
}

int CClientSockEvent::OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
								const void* data, unsigned long len)
{

#ifdef WEBCON_HAS_PRINT_DEBUG
	std::cerr << "CClientSockEvent::OnTCPPacketReceived(" << sock 
				<< "," << pdutype
				<< ",[" << data
				<< "]," << len
				<< ")" << std::endl;
	//dumpHex(data, len);
#endif

	CSockStream	in_stream(data, len) ;
	unsigned short reqPdu = 0;
	unsigned short subPdu = 0;

	switch(pdutype)
	{

	case PDU::WINDBROUTER0_REQ_FromWin:
		_INFO("CClientSockEvent::OnTCPPacketReceived(WINDBROUTER0_REQ_FromWin)");

		{
			reqPdu = PDU::WINDBROUTER0_REQ_FromSub;
		}
		break;
	case PDU::TransData_REQ_FromWin:
		{
			in_stream >> subPdu;
			reqPdu = PDU::TransData_REQ_FromSub;
		}
		break;
	case PDU::OpROUTER0_RSP_ToSub:
		{
			pServ_->sendPacketToCenter(PDU::OpROUTER0_RSP_ToCenter,data,len);
		}
		break;

	case PDU::WinSysServerReport_REQ_FromWinSys:
		_INFO("CClientSockEvent::OnTCPPacketReceived(WinSysServerReport_REQ_FromWinSys)");

		{
			pServ_->dealWinSysServerReport(sock,data,len);
			return 0;
		}
		break;
	default:
		_ERROR("CClientSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
		break;
	}
	if(0  != reqPdu)
	{
			CSockStream out_stream(len + 16);
			out_stream << (unsigned short)sock;
			out_stream.writeBytes(data,len);
			if(PDU::SubPduTranAVData_REQ_FromWin == subPdu)
			{
				pServ_->sendPacketToTrans(reqPdu,out_stream.getData(),out_stream.length());
			}
			else
			{
				pServ_->sendPacketToCenter(reqPdu,out_stream.getData(),out_stream.length());
			}

	}

	return 0;
}


int CClientSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	_INFO("CClientSockEvent::OnConnectionClosed(%d)", sock);
	-- clientNum_;

	return 0;
}

unsigned short CClientSockEvent::getClientNum() const
{
	return clientNum_;
}


int CClientSockEvent::OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
                                                const void* data, unsigned long len,
                                                unsigned long fromIP,
                                                unsigned short fromPort)
{
	return 0;
}

