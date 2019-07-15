// ACBaseMainProcClass.h: interface for the CACBaseMainProcClass class.
//////////////////////////////////////////////////////////////////////

//#pragma once
#ifndef __ACBASEMAINPROCCLASS_H__
#define __ACBASEMAINPROCCLASS_H__

#include <assert.h>
#include "BaseACObject.h"

class CACBaseMainProc  
{
public:
	virtual ~CACBaseMainProc(void);

public:
	virtual void Initialize(short nCommType, short nThreadNum, short nMaxConn,
		IACDataCallBack* lpDataCallBack, IACPackHead* lpHeadProc);

	virtual int BeginWork(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort, //Զ�̶˿�
		BOOL bMulPoint //�ಥ
		)=0;

	virtual int BeginWork(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort, //Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		)=0;

	virtual int BeginWorkHttp(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort, //Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		)=0;

	virtual int CloseConnection(SOCKET hSocket = INVALID_SOCKET)=0;

	virtual int SendCommUDPData(WORD wPduType,
		const BYTE* lpBuf, //Ҫ���͵�����(����ΪNULL)
		DWORD dwSize,//Ҫ���͵��ֽ���(>0)
		BOOL bSync, //���ͷ�ʽ(��ΪTRUE-ͬ������,FALSE-�첽����)
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort, //Զ�̶˿�
		short nPackID,
		BOOL bSock5Flag
		)
	{
		assert(!"CACBaseMainProc::SendCommUDPData");
		return 0;
	};

	virtual int SendCommTCPData(WORD wPduType,
		const BYTE* lpData, DWORD dwSize, BOOL bSync,
		SOCKET hSocket = INVALID_SOCKET
		)
	{
		assert(!"CACBaseMainProc::SendCommTCPData");
		return 0;
	}

	virtual int StopMainProc(void)
	{
		return 0;
	}

protected:
	CACBaseMainProc(void);

	short m_nCommType;
	short m_nThreadNum;
	short m_nMaxConn;

	IACDataCallBack* m_pDataCallBack;
	IACPackHead* m_pHeadProc;
};
#endif
