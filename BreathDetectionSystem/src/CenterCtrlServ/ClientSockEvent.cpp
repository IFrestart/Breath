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
#include "CenterCtrlServ.h"
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

	if(0 != (pServ_->GetState()))
	{
#ifdef WEBCON_HAS_PRINT_DEBUG
		std::cerr << "XXXXXXXXXXXXXXXXX  the connect db error XXXXXXXXXXXX" << std::endl;
		_ERROR("XXXXXXXXXXXXXXXXX  the connect db error XXXXXXXXXXXX \n");
#endif 
	    //pServ_->sendPacketToTcpClient (sock , PDU::GATENOREADY_RSP_TOCLI , 0 , 0);

		return 0;
	}

	CSockStream	in_stream(data, len) ;
	unsigned short reqDbPdu = 0;

	switch(pdutype)
	{

	case PDU::KEEPALIVE_REQ_FROMCLI:
		{
			//printf("have recive PDU::keepAlive: ip=[%s] port=[%d]\n",ip_to_string(fromIP),fromPort);
			pServ_->dealKeepAlive(sock,in_stream);
			pServ_->sendPacketToTcpClient( sock,PDU::KEEPALIVE_RSP_TOCLI,NULL , 0);
		}
		break;

	case PDU::DBROUTER0_REQ_FROMCLI:
		_INFO("CClientSockEvent::OnTCPPacketReceived(DBROUTER0_REQ_FROMCLI)");

		{
			reqDbPdu = PDU::ROUTER0_REQ_TODBCOMM;
		}
		break;

	case PDU::WINDBROUTER0_REQ_FromSub:
		_INFO("CClientSockEvent::OnTCPPacketReceived(DBROUTER0_REQ_FROMCLI)");

		{
			reqDbPdu = PDU::WINDBROUTER0_REQ_FromCenter;
		}
		break;

	case PDU::TransData_REQ_FromSub:
		_INFO("CClientSockEvent::OnTCPPacketReceived(DBROUTER0_REQ_FROMCLI)");

		{
			pServ_->dealTransData(sock,data,len);
		}
		break;
	case PDU::TransData_RSP_FromClient:
		_INFO("CClientSockEvent::OnTCPPacketReceived(DBROUTER0_REQ_FROMCLI)");

		{
			CSockStream	in_stream(data,len);
			unsigned short subCenterSock = 0;
			in_stream >> subCenterSock;
			pServ_->sendPacketToTcpClient(subCenterSock, PDU::TransData_RSP_ToSub, in_stream.getCurr(), in_stream.left());
		}
		break;

	case PDU::MAINCERT_REQ_FROMCLI:
		_INFO("CClientSockEvent::OnTCPPacketReceived(MAINCERT_REQ_FROMCLI)");

		{
#ifdef WEBCON_HAS_PRINT_DEBUG
			printf("have recive PDU::MAINCERT_REQ_FROMCLI: sock=[%d]\n",sock);
#endif
			reqDbPdu = PDU::MAINCERT_REQ_TODBCOMM;
		}
		break;
	case PDU::USERLOGIN_REQ_FROMCLI:
		{ 
			CSockStream	out_ss(sizeof(UserInfo));

			UserInfoEx  user;

			in_stream >> user;

			user.setSock(sock);
			user.setTime();

			unsigned short ret = pServ_->addUser(user);

			out_ss << ret ;

			pServ_->sendPacketToTcpClient(sock, PDU::USERLOGIN_RSP_TOCLI, out_ss.getData(), out_ss.length());
		}
		break;
	case PDU::USERLOGOUT_REQ_FROMCLI:
		{
			CSockStream	out_ss(sizeof(UserInfo));
			UserID  user ;
			unsigned short logoutFlag = 0;
			in_stream >> user >> logoutFlag;
			unsigned short ret = 0;
			std::cout << " logout before call delUser [" << ret << "] user [" << user.toString() << "]" << std::endl;
			ret = pServ_->delUser(user);
			std::cout << " logout after call delUser [" << ret << "] user [" << user.toString() << "]" << std::endl;
		}
		break;

	case PDU::OpROUTER0_REQ_FromClient:
		{
			pServ_->optWinSysServ(sock,data,len);
		}
		break;
	case PDU::OpROUTER0_RSP_ToCenter:
		{
			unsigned short clientSock = 0,ret = 0;
			CSockStream in_ss(data,len);
			in_ss >> clientSock;
			in_ss.seek(0);
			in_ss << ret;
			in_ss.seek(0);
			pServ_->sendPacketToTcpClient(clientSock, PDU::OpROUTER0_RSP_ToClient, in_ss.getCurr(), in_ss.left());
		}
		break;

	case PDU::SubServerReport_REQ_FromSub:
		{	
			CSockStream	out_ss(64);
			unsigned short ret = 0;
			ret = pServ_->addSubServ(sock,data,len);
			out_ss << ret;
			pServ_->sendPacketToTcpClient(sock, PDU::SubServerReport_RSP_ToSub, out_ss.getData(), out_ss.length());

		}
	case PDU::WinSysServerReport_REQ_FromSub:
		{
			pServ_->addWinSysServ(sock,data,len);
		}
	default:
		_ERROR("CClientSockEvent::OnTCPPacketReceived(%d->!!!UNKOWN!!!)", pdutype);
		break;
	}
	if(0 != reqDbPdu)
	{
		CSockStream	ostream(len + sizeof(short));
		ostream << (short)sock;
		ostream.writeBytes(data, len);
		pServ_->sendPacketToDBComm( reqDbPdu, ostream.getData(), ostream.length());
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


int	CClientSockEvent::VerifiedCode (ACE_HANDLE sock, unsigned short pdutype,
								const void* data, unsigned long len)
{
	CSockStream istream( data, len ) ;
	char meetcode[17],verifyid[17];
	char clusterid[17],servid[10];
	char *pcode , *pcluster ,*pservid;
	int codelen;

	memset(meetcode,0x00,sizeof(meetcode));
	memset(verifyid,0x00,sizeof(verifyid));
	memset(clusterid,0x00,sizeof(clusterid));
	memset(servid,0x00,sizeof(servid));
	try
	{
		istream.readString(meetcode,17);
		istream.readString(verifyid,17);
	}
	catch (CSockStream::exception e)
	{
		std::cerr << " the error :" << e.what() << std::endl;
	}
	codelen = strlen(meetcode);
	int i;
	for(i = 0, pcode = meetcode , pcluster = clusterid ; i< (codelen-6) ; i++)
	{
		*(pcluster++) = *(pcode++);
	}
	*pcluster ='\0';
	for( pservid = servid ,i = 0 ; i< 2 ; i++)
	{
		*(pservid++) = *(pcode++);
	}
	*pservid = '\0';

	int clusterindex = atoi(clusterid);
	int servindex= atoi(servid);
	
//	pServ_ -> CHVerifiedCode( sock , PDU::VERIFYMEETINGCODE_REQ_TODBCOMM , data , 
										//len , clusterindex , servindex); 

	return 0;
}

int CClientSockEvent::OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
                                                const void* data, unsigned long len,
                                                unsigned long fromIP,
                                                unsigned short fromPort)
{
	return 0;
}

