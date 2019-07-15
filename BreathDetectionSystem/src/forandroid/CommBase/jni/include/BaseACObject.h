// BaseACObject.h: MyAcDll���ýӿ�ͷ�ļ�.

//#pragma once
#ifndef __BASEACOBJECT_H__
#define __BASEACOBJECT_H__

#include <vector>
#include "LinuxDef.h"
//#include "CommHead.h"
#include "SockHeaderCallBack.h"
#include "SockEventCallBack.h"

const short COMMTYPE_NULL = 0;
const short COMMTYPE_SERVER = 1;
const short COMMTYPE_CLIENT = 2;

typedef struct tagThreadInfo
{
	DWORD dwThreadId;
#ifdef _WIN32
	HANDLE hThread;
#else
	pthread_t hThread;
#endif
	USHORT nPort;
}ThreadInfo;

typedef std::vector<ThreadInfo> ThreadArray;

//extern ThreadArray g_thredTable;
//extern long g_nThreadCount;
#if 0
class IACDataCallBack
{
public:
	/** �յ�һ���������ݰ�ʱ�Ļص�����
	* @param wProtocolType Э������(Ϊ1ʱΪTCP,Ϊ2ʱΪUDP,������Ч)
	* @param hSocket �������ݵ��׽���
	* @param lpData �յ�������
	* @param dwSize ���ݳ���
	* @param lpRemoteHost �������ݵ�IP(����ΪNULL)
	* @param nRemotePort �������ݵĶ˿�
	* @param wPduType ���ݰ���PDU����
	* @param wExtParam ��չ����
	*/
	virtual int OnDataReceived(WORD wProtocolType, SOCKET hSocket, const BYTE* lpData,
		DWORD dwSize, LPCTSTR lpRemoteHost, USHORT nRemotePort, WORD wPduType,
		WORD wExtParam)=0;

	/** �յ�һ�鲻������TCP����ʱ�Ļص�����
	* @param hSocket �������ݵ��׽���
	* @param dwArrivedSize �ѽ��յ������ݳ���(��������ͷ)
	* @param dwTotoalSize Ҫ���յ������ܳ���(��������ͷ)
	*/
	virtual int OnBlockDataArriving(SOCKET hSocket, DWORD dwArrivedSize, 
		DWORD dwTotoalSize)=0;

	/** �ͻ��������Ϸ���������������յ��ͻ�������ʱ�Ļص�����
	* @param hSocket-�׽��־��
	* @param lpLocalHost ��������IP(����ΪNULL)
	* @param nLocalPort ���ض˿�(>0)
	* @param lpLocalMask ������������(����ΪNULL)
	* @param lpRemoteHost Զ������IP(����ΪNULL)
	* @param nRemotePort Զ�̶˿�(>0)
	*/
	virtual int OnNewConnection(SOCKET hSocket, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpLocalMask, LPCTSTR lpRemoteHost, USHORT nRemotePort)=0;

	/** TCP�����У��Է��ر�ʱ�Ļص�����
	* @param hSocket �׽��־��
	*/
	virtual int OnConnectionClosed(SOCKET hSocket)=0; 
};
#endif
//ͨ�Ŷ���ӿ�
class IACMainObject
{
public:
	/** ��ʼ��ͨ�Ŷ���
	* @param nCommType-ͨ�Ŷ�������(��Ϊ2-�ͻ���,1-������,������Ч)
	* @param nProtocolType-ͨ��Э��(��Ϊ1-TCPЭ��,2-UDPЭ��,������Ч)
	* @param nThreadNum-ͨ�Ŵ����߳���(nCommTypeΪ2ʱ����,Ϊ1ʱ����>0)
	* @param nMaxConn-�ܽ��ܵ����������(nCommTypeΪ2ʱ����,Ϊ1ʱ����>0)
	* @param lpACCommModuleCallBack-�������ݵĻص��ӿ�(����ΪNULL)
	* @param lpACCommonInfoOfModule-���ݰ�ͷ�����ص��ӿ�(����ΪNULL)
	*/
	virtual void Initialize(short nCommType, short nProtocolType, short nThreadNum, 
		short nMaxConn, ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc)=0;

	/** ��ʼ��HTTPͨ�Ŷ���
	* @param lpACCommModuleCallBack-�������ݵĻص��ӿ�(����ΪNULL)
	* @param lpACCommonInfoOfModule-���ݰ�ͷ�����ص��ӿ�(����ΪNULL)
	*/
	virtual void InitHttp(ISockEventCallBack * lpDataCallBack, ISockHeaderCallBack* lpHeadProc)=0;

	/** ����AC����
	* @param lpLocalHost-����IP��ַ(Ϊ��ʱ��ϵͳ���㱾��IP)
	* @param nLocalPort-���ض˿�(Ϊ0ʱ��ϵͳ��ѡ�񱾵ض˿�)
	* @param lpRemoteHost-Զ��IP(Ϊ�ͻ���ʱ����ΪNULL,Ϊ������ʱΪNULL)
	* @param nRemotePort-Զ�̶˿�(Ϊ�ͻ���ʱ>0, Ϊ������ʱΪ0)
	* @param bMulPoint �Ƿ��鲥 FALSE ��TRUE �ǡ�
	* @return 0-�ɹ�,����ʧ��
	*/
	virtual int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, BOOL bMulPoint=FALSE)=0;

	/** ����AC���� tcp/udp����
	* @param lpLocalHost-����IP��ַ(Ϊ��ʱ��ϵͳ���㱾��IP)
	* @param nLocalPort-���ض˿�(Ϊ0ʱ��ϵͳ��ѡ�񱾵ض˿�)
	* @param lpRemoteHost-Զ��IP(Ϊ�ͻ���ʱ����ΪNULL,Ϊ������ʱΪNULL)
	* @param nRemotePort-Զ�̶˿�(Ϊ�ͻ���ʱ>0, Ϊ������ʱΪ0)
	* @return 0-�ɹ�,����ʧ��
	*/
	virtual int BeginWork(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort ,//Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		)=0;

	/** ����AC���� http����
	* @param lpLocalHost-����IP��ַ(Ϊ��ʱ��ϵͳ���㱾��IP)
	* @param nLocalPort-���ض˿�(Ϊ0ʱ��ϵͳ��ѡ�񱾵ض˿�)
	* @param lpRemoteHost-Զ��IP(Ϊ�ͻ���ʱ����ΪNULL,Ϊ������ʱΪNULL)
	* @param nRemotePort-Զ�̶˿�(Ϊ�ͻ���ʱ>0, Ϊ������ʱΪ0)
	* @return 0-�ɹ�,����ʧ��
	*/
	virtual int BeginWorkHttp(LPCTSTR lpLocalHost, //����IP(����ΪNULL)
		USHORT nLocalPort, //���ض˿�
		LPCTSTR lpRemoteHost, //Զ��IP(����ΪNULL)
		USHORT nRemotePort ,//Զ�̶˿�
		LPCTSTR pProxyHost, //���������IP��ַ(�޴���ʱΪNULL��մ�)
		USHORT nProxyPort, //����������˿�
		LPCTSTR pProxyUserName, //�����û���(������֤�û���/����ʱΪNULL��մ�)
		LPCTSTR pProxyPassword //��������
		)=0;

	/** �ر�TCP����
	* @param hSocket-�׽���
	*/
	virtual void CloseConnection(SOCKET hSocket = INVALID_SOCKET)=0;

	/** ����UDP���ݰ�
	* @param wPduType PDU����
	* @param lpData ��������
	* @param dwSize ���ݳ���
	* @param bSync ���ͷ�ʽ(��ΪTRUE-ͬ������,FALSE-�첽����)
	* @param lpszRemoteAddr Ŀ���ַ(����Ϊ��)
	* @param nRemotePort Ŀ��˿�(���������)
	* @param wExtParam ��չ����
	* @return 0-�ɹ�,����ʧ��
	*/
	virtual int SendCommUDPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, WORD wExtParam, BOOL bSock5Flag
		)=0;

	/** ����TCP���ݰ�
	* @param wPduType PDU����
	* @param lpData ��������
	* @param dwSize ���ݳ���
	* @param bSync ���ͷ�ʽ(��ΪTRUE-ͬ������,FALSE-�첽����)
	* @param hSocket �����׽���(Ϊ�ͻ���ʱ����)
	* @return 0-�ɹ�,����ʧ��
	*/
	virtual int SendCommTCPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, SOCKET hSocket= INVALID_SOCKET)=0;
};

//��������
//���ܣ���ʼ��WinSock
//void WINAPI ACModuleInitialize();

//��������
//���ܣ���ֹWinSock
//void WINAPI ACModuleUnInitialize();

//��������
//����:����ͨ�Ŷ���
//����ֵ:NULL-ʧ��, ��NULL-�ɹ�
//IACCommMainObject* WINAPI CreateACObject();

//��������
//����:ɾ��ͨ�Ŷ���
//����:pACCommMainObject-Ҫɾ����ͨ�Ŷ���(����ΪNULL)
//����ֵ:0-�ɹ�, ����-ʧ��
//int WINAPI DestroyACObject(IACCommMainObject* pACCommMainObject);

#endif
