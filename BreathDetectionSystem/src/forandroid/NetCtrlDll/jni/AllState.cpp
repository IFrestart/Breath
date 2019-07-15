#include "AllState.h"
#include "NetCtrlCall.h"


int CSLoginAuthRequest::startObject(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	return (actObj->ConnectAuthServ());	
	
}

int CSLoginAuthRequest::SendData(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	actObj->SendAuthRequest();
	setAddTime();
	addSendTimes();
	return 0;
}

int CSLoginAuth::startObject(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	return 0;

}

int CSLoginAuth::SendData(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	actObj->SendLoginRequest();
	setAddTime();
	addSendTimes();
	return 0;
}


int CSLogoutAuth::startObject(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	return 0;

}

int CSLogoutAuth::SendData(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	actObj->SendLogoutRequest();
	setAddTime();
	addSendTimes();	
	return 0;
}


int CSTransJoin::startObject(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	return (actObj->ConnectTransServ());	
}

int CSTransJoin::SendData(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	actObj->SendJoinRequest();
	setAddTime();
	addSendTimes();
	return 0;
}
int CSTransLeave::startObject(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	return 0;
}

int CSTransLeave::SendData(void* obj)
{
	CNetCtrlCall * actObj = (CNetCtrlCall*)obj;
	actObj->SendLeaveRequest();
	setAddTime();
	addSendTimes();
	return 0;
}


