/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	TransServSockEvent.cpp
* @author	jjzhou <jjzhou@webcon.com.cn>

* @short 
**/

#ifdef USE_ACE_VER
#include "ace/OS.h"
#include "Logger.h"
#endif
#include "TransServSockEvent.h"
#include "NetCtrlCall.h"


int TransServSockEvent::OnNewConnection(ACE_HANDLE sock)
{
#if Debug_MyTrance_
	printf("TransServSockEvent::OnNewConnection sock=[%d]\n",sock);
#endif
	return 0;
}

int TransServSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	//printf("TransServSockEvent::OnConnectionClosed sock[%d]\n",sock);
	/*
	if (m_pServ)
	{
		m_pServ->TcpTransServConnectException();
	}*/
	return 0;
}

int TransServSockEvent::OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
										 const void* data, unsigned long len)
{
	return OnPacketReceived(sock,pdutype,data,len,0,0);
}

int TransServSockEvent::OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
									   const void* data, unsigned long len,unsigned long fromIP,unsigned short fromPort)
{
	//std::cout << "TransServSockEvent::OnUDPPacketReceived pdu[" << pdutype << "]" << std::endl;
	return OnPacketReceived(sock,pdutype,data,len,fromIP,fromPort);
}


int TransServSockEvent::OnPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
									const void* data, unsigned long len,unsigned long fromIP,unsigned short fromPort)
{
	CSockStream in_stream(data,len);
	unsigned short result = 0;

//	_INFO("!!!! TransServSockEvent::OnPacketReceived pdutype %d fromip[%d] fromport[%d]\n",pdutype,fromIP,fromPort);

	switch(pdutype)
	{
		//case PDU::TRANSFERINSESSIONO_RSP_TOCLI:
		case PDU::TransData_REQ_ToClient:
		{
			//SessionID sessid;
			//in_stream >> sessid ;

			if (m_pServ)
			{
				INetACDataCallBack* pCallBack = m_pServ->GetDataReciveCallBack();
                                if (pCallBack)
                                {
                                       pCallBack->OnDataReceived(0,CNetCtrlCall::type_AV_DATA,data,len,pdutype,NULL);
                                }
				//m_pServ->OnPacketReceived(sock,pdutype,data,len);
			}
		}
		break;
	case PDU::JOINSESSION_RSP_TOCLI:		
		{
			in_stream >> result ;
			if(0 == result)
			{
				if (m_pServ)
				{
					m_pServ->changeState(LOGINSTATE::SNORMAL);
					m_pServ->OnCallBack(result);	
				}
			}
			else
			{
				m_pServ->OnCallBack(result);	
			}
		}
		break;
		case PDU::SESKEEPALIVE_RSP_TOCLI:
		{
#if Debug_MyTrance_
			std::cout << " SESKEEPALIVE_RSP_TOCLI hhhhhhhhhh" << std::endl;
#endif
			in_stream >> result ;
			if(0 == result)
			{
				m_pServ->updateTransTime();
			}
		}
		break;
		case PDU::LEAVESESSION_RSP_TOCLI:
		{
			in_stream >> result ;

			//changeState(TRANSSTATE::SCLOSEED,result);//modify 2012-11-29
			//m_pServ->changeState(LOGINSTATE::SLEAVETRAS);

		}
		break;
			
	default:
		{
#if Debug_MyTrance_
			std::cout << " TransServSockEvent::OnPacketReceived  error pdu  [" << pdutype << "] " << std::endl;
#endif
			break;
		}
		break;
	}

	return 0;
}


