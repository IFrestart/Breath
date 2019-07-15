/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����TCP��,�ṩ����TCP�Ľ���,�ر�,�������ݵȲ���

 * @file	TCPConnector.h
 * @short	����TCP��
 * @author	zhoujj <zhoujj@webcon.com.cn>

 * @class	CTCPConnector
 * @short	����TCP��
 **/

#ifndef	__TCPCONNECTOR_H__
#define	__TCPCONNECTOR_H__

#include "Timer.h"

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

// ��װMyAcDll��UDP��TCP�����׽���
class CTCPConnector
{
public:
	/** ���캯��
	*/
	CTCPConnector(ISockEventCallBack* pCallBack=0);

	/** ��������
	*/
	~CTCPConnector(void);

	/**
	* ����һ��TCP socket
	* @param ipstr socketҪ���ӵ���IP,�ַ���
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* ����һ��TCP socket
	* @param ip socketҪ���ӵ���IP
	* @param port socketҪ���ӵ��Ķ˿ں�
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* �ر�TCP socket
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* ͨ��TCP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, const void* data, unsigned long len);

	/**
	* ����TCP socket�ײ��FD
	* @return ���ɹ��򷵻�-1
	*/
	ACE_HANDLE getSock() const;
#if 0
	/** ����ͨ�Ŷ���(��ʹ�ô���)
	* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost ����IP
	* @param nLocalPort �����˿�
	* @param lpszSvrAddr ������IP
	* @param nSvrPort �������˿� 	
	* @param bUseHttp �Ƿ�ʹ��HTTPЭ��(����HTTP�����HTTPЭ���½��,
	������Ե�����ʱ,��˫����ͬһ������,��ΪFALSE,�������ΪTRUE)	
	* @param bMulPoint �Ƿ��鲥
	* @return 0-�ɹ�, <0ʧ��
	*/
	int Start(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, BOOL bUseHttp,BOOL bMulPoint=FALSE);

	/** ����ͨ�Ŷ���(ʹ��SOCK5����)
	* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost ����IP
	* @param nLocalPort �����˿�
	* @param lpszSvrAddr ������IP
	* @param nSvrPort �������˿� 	
	* @param lpProxyAddr ���������IP 
	* @param nProxyPort ����������˿�
	* @param lpProxyUserName �����û���
	* @param lpProxyPassword ��������
	* @return 0-�ɹ�, <0ʧ��
	*/
	int StartSock5(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, LPCTSTR lpProxyAddr, 
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);

	/** ����ͨ�Ŷ���(ʹ��HTTP����)
	* @param wProtoType ͨ�Ŷ�������(SOCK_STREAM-TCP, SOCK_DGRAM-UDP)
	* @param lpLocalHost ����IP
	* @param nLocalPort �����˿�
	* @param lpszSvrAddr ������IP
	* @param nSvrPort �������˿� 	
	* @param lpProxyAddr ���������IP 
	* @param nProxyPort ����������˿�
	* @param lpProxyUserName �����û���
	* @param lpProxyPassword ��������
	* @return 0-�ɹ�, <0ʧ��
	*/
	int StartHttp(WORD wProtoType, LPCTSTR lpLocalHost, USHORT nLocalPort, 
		LPCTSTR lpszSvrAddr, USHORT nSvrPort, LPCTSTR lpProxyAddr, 
		USHORT nProxyPort, LPCTSTR lpProxyUserName, LPCTSTR lpProxyPassword);


	/** UDPͨ�Ŷ���������
	* @param wPduType ���ݰ���PDU���
	* @param lpData Ҫ���͵�����
	* @param dwSize ���ݳ���
	* @param bSync �Ƿ�ͬ������(Ŀǰֻ֧��ͬ������)
	* @param lpRemoteHost ���շ���IP
	* @param nRemotePort ���շ��Ķ˿�
	* @param wExtParam ��չ����(���ڳ������)
	* @param bSock5Flag	�����SOCK5��½��������,��Ե����Ӽ䷢�������Ƿ��
	SOCK5ͷ(��˫����ͬһ������ʱ,ӦΪFALSE)
	*/
	int SendCommUDPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, LPCTSTR lpRemoteHost, USHORT nRemotePort, 
		WORD wExtParam, BOOL bSock5Flag);
	/** TCPͨ�Ŷ���������
	* @param wPduType ���ݰ���PDU���
	* @param lpData Ҫ���͵�����
	* @param dwSize ���ݳ���
	* @param bSync �Ƿ�ͬ������(Ŀǰֻ֧��ͬ������)
	* @param hSocket ��������ʹ��
	*/
	int SendCommTCPData(WORD wPduType, const BYTE* lpData, 
		DWORD dwSize, BOOL bSync, SOCKET hSocket= INVALID_SOCKET);

#endif	
	/** ֹͣͨ�Ŷ���
	*/
	void Stop(int nType=0);

	///** ��ȡ����IP
	//*/
	//LPCTSTR GetLocalHost(void) const;

	///** ��ȡ������������
	//*/
	//LPCTSTR GetLocalMask(void) const;

	///** ��ȡ���ض˿�
	//*/
	//USHORT GetLocalPort(void);

	///** ��ȡͨ�Ŷ�����׽��־��
	//*/
	//SOCKET GetSocket(void);

	/*�ر�Socket
	*	
	*/
//	void CloseConnect();
	//void CloseConnect(SOCKET hSocket);
	void setHeadFlag(bool flag) {m_bHeadFlag = flag;}//add 2012-10-18
	int GetExitType();

private:
	/** ɾ��ͨ�Ŷ���(�����߳�ɾ��)
	*/
	void FreeObject(void);

	/** ֹͣͨ�Ŷ���ʵ���߳�
	*/
#ifdef _WIN32
	static DWORD WINAPI StopThreadHelper(LPVOID lpParam);
	static DWORD WINAPI StopMyselfThreadHelper(LPVOID lpParam);
#else
	static void* StopThreadHelper(LPVOID lpParam);
	static void* StopMyselfThreadHelper(LPVOID lpParam);
#endif

	IACMainObject* m_pAcCommObj; ///< MyAcDllͨ�Ŷ���ӿ�
	ISockEventCallBack* m_pCallBack; ///< ���ݻص��ӿ�ʵ�ֶ���
	//CACPackHead* m_pHeadInfo; ///< ���ݰ�ͷ�������
	CSockHeader* m_pHeadInfo; ///< ���ݰ�ͷ�������

	int m_nExitType;
	bool 		m_bHeadFlag; //add 2012-10-18

};

#endif	/*__TCPCONNECTOR_H__*/

