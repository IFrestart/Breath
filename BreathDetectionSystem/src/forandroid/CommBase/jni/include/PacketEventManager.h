PacketEventManager.h
#ifndef __PACKETEVENTMANAGER_H__
#define __PACKETEVENTMANAGER_H__
struct PacketUnit
{
	
	IACDataCallBack* pCallBack_;	
};

struct CloseUnit
{
	SOCKET Socket_;
	IACDataCallBack* pCallBack_;	
};


class PacketEventManage
{
	public:
		AddPacketEvent(WORD wProtocolType, SOCKET hSocket, const BYTE* lpData, 
			DWORD dwSize, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
			WORD wPduType,WORD wExtParam,IACDataCallBack* pCallBack);
		AddCloseEvent (SOCKET hSocket,IACDataCallBack* pCallBack);
		doPacketEvent();
		doCloseEvent();
	private:
		std::list<PacketUnit> packetList_;
		std::list<CloseUnit> closeList_;
		CRITICAL_SECTION	eventLock_;
};

#endif
