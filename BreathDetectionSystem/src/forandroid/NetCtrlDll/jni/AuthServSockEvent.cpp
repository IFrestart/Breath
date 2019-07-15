/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	AuthServSockEvent.cpp
* @author	jjzhou <jjzhou@webcon.com.cn>

* @short 
**/

#ifdef USE_ACE_VER
#include "ace/OS.h"
#endif
#include "AuthServSockEvent.h"
#include "NetCtrlCall.h"
#include "Tools.h"

int AuthServSockEvent::OnNewConnection(ACE_HANDLE sock)
{
	//_INFO("AuthServSockEvent::OnNewConnection sock=[%d]",sock);
#if Debug_MyTrance_
	printf("AuthServSockEvent::OnNewConnection sock=[%d]  \n",sock);
#endif
	return 0;
}



int AuthServSockEvent::OnTCPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
										 const void* data, unsigned long len)
{
	//_INFO("info AuthServSockEvent::OnTCPPacketReceived sock=[%d] pdutype=[%d] len=[%d]",sock,pdutype,len);
	//printf("AuthServSockEvent::OnTCPPacketReceived sock=[%d] pdutype=[%d] len=[%d]\n",sock,pdutype,len);
	CSockStream in_stream(data,len);
	unsigned short result;
	switch(pdutype)
	{
		case PDU::TransData_REQ_ToClient:
		{
			//SessionID sessid;
			//in_stream >> sessid ;

			if (m_pServ)
			{
				INetACDataCallBack* pCallBack = m_pServ->GetDataReciveCallBack();
                                if (pCallBack)
                                {
                                       pCallBack->OnDataReceived(0,CNetCtrlCall::type_CTRL_DATA,data,len,pdutype,NULL);
                                }
				//m_pServ->OnPacketReceived(sock,pdutype,data,len);
			}
		}
		break;
		case PDU::MAINCERT_RSP_TOCLI:
			{
				UserID		user;
	
				in_stream >> result ;
				if (0 == result)
				{
					//in_stream >> user >> udpPort; //notice here user.cid_ == is null 
					in_stream >> user ;
					//in_stream >> corpId >> udpPort; //notice here user.cid_ == is null  //modify 2012-4-16
#if Debug_MyTrance_
					printf("return from db userindex=[%d]\n",user.uid_);
#endif
					//user.cid_ = m_pServ->GetClusterId();  

					//char lpLocalHost[16];
					//char lpLocalMask[16];
					//memset(lpLocalHost,0,sizeof(lpLocalHost));
					//memset(lpLocalMask,0,sizeof(lpLocalMask));
					//unsigned short localPort = 0;
					//getSockName(sock, lpLocalHost, &localPort, lpLocalMask);

#if Debug_MyTrance_
					printf("AuthServSockEvent::MAINCERT_RSP_TOCLI sock=[%d] localPort=[%d] lpLocalHost=%s] localMask[%s] lanPort[%d] \n",sock,localPort,lpLocalHost,lpLocalMask,m_pServ->GetLocalPortBind());
#endif
//					m_pServ->GetMyselfInfo()->setLanNetInfo(ip_to_long(lpLocalHost),ip_to_long(lpLocalMask));//modify 2013-1-22
					//Addr lanAddr;			
					//lanAddr.ip_ = ip_to_long(lpLocalHost);
					//lanAddr.port_ = m_pServ->GetLocalPortBind();
					//m_pServ->GetMyselfInfo()->setLanNetInfo(lanAddr,ip_to_long(lpLocalMask));

					m_pServ->GetMyselfInfo()->setUserId(user);
					m_pServ->changeState(LOGINSTATE::SLOGINAUTH);
				}
				else //modify a buf 2012-11-7 ; 
				{
					m_pServ->OnCallBack(result);	
				}
				//m_pServ->CloseTcpAuthServConnect();	//¹Ø±ÕÁ¬½Ó //add 2012-12-28 //modify 2013-3-5
			}
			break;
			
		case PDU::USERLOGIN_RSP_TOCLI:				//add 2014-4-24
			{
				UserID user;
				in_stream >> result;
				if (0 == result)		
				{
					m_pServ->changeState(LOGINSTATE::SJOINSTRANS);
				}
				else
				{
					m_pServ->OnCallBack(result);	
				}

			}
			break;

		case PDU::KEEPALIVE_RSP_TOCLI:
			{
				if (m_pServ)
				{
					//printf("KEEPALIVE_RSP_TOCLI  sock[%d] fromip[%lu] fromport[%d]\n",sock,fromIP,fromPort);
					m_pServ->updateAuthTime();
				}			
			}
			break;
/*
		PDU::SECONDLOGIN_RSP_TOCLI;	//add 2013-2-5
			{
				if (m_pServ)
				{
				//	m_pServ->setExceptLeaveServ();
					INetACDataCallBack* pCallBack = m_pServ->GetDataReciveCallBack();
					if (pCallBack)
					{
						pCallBack->OnMemberRoster(0,5,NULL);
					}	
				}
			}
			break;
*/

		default:
			break;
	}
	return 0;
}

int AuthServSockEvent::OnConnectionClosed(ACE_HANDLE sock)
{
	//_INFO("info AuthServSockEvent::OnConnectionClosed!");
#if Debug_MyTrance_
	printf("AuthServSockEvent::OnConnectionClosed sock=[%d]!\n",sock);
#endif

	if ((m_pServ) && (sock == m_pServ->getAuthServConnectSock()))      //add 2014-9-25
	{
		printf("22 AuthServSockEvent::OnConnectionClosed sock=[%d]!\n",sock);
		//m_pServ->setExceptLeaveServ();
		INetACDataCallBack* pCallBack = m_pServ->GetDataReciveCallBack();
		if (pCallBack)
		{
			pCallBack->OnMemberRoster(0,3,NULL);
		}	
	}

	return 0;
}

int AuthServSockEvent::OnUDPPacketReceived(ACE_HANDLE sock, unsigned short pdutype,
										 const void* data, unsigned long len,unsigned long fromIP,unsigned short fromPort)
{

	return 0;
}
