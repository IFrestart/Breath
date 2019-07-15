// ACCommMainObject.h: interface for the CACCommMainObject class.
//
//////////////////////////////////////////////////////////////////////

//#pragma once

#ifndef __ACCOMMMAINOBJECT_H__
#define __ACCOMMMAINOBJECT_H__

#include "BaseACObject.h"
#include "ACBaseMainProcClass.h"

class CACMainObject: public IACMainObject
{

public:
	CACMainObject(void);
	virtual ~CACMainObject(void);

	void Initialize(short nCommType, short nProtocolType, short nThreadNum, 
		short nMaxConn, ISockEventCallBack * lpDataCb, ISockHeaderCallBack* lpHeadProc);
	void InitHttp(ISockEventCallBack * lpDataCb, ISockHeaderCallBack* lpHeadProc);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint);
	int BeginWork(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort ,//Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		);

	int BeginWorkHttp(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort ,//Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		);

	void CloseConnection(SOCKET hSocket = INVALID_SOCKET);

	int SendCommUDPData(WORD wPduType,
		const BYTE* lpData, //Ҫ���͵�����(����ΪNULL)
		DWORD dwSize,//Ҫ���͵��ֽ���(>0)
		BOOL bSync, //���ͷ�ʽ(��ΪTRUE-ͬ������,FALSE-�첽����)
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort, //Զ�̶˿�
		WORD nPackID,
		BOOL bSock5Flag
		);

	int SendCommTCPData(WORD wPduType, const BYTE* lpData, DWORD dwSize,
		BOOL bSync, SOCKET hSocket=INVALID_SOCKET);

private:
	short m_nProtocolType;
	CACBaseMainProc* m_pMainProc; //��������
};
#endif
