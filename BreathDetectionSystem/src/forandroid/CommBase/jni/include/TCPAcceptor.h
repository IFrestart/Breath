/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ����TCP��,�ṩ����TCP�Ľ���,�ر�,�������ݵȲ���

 * @file	TCPAcceptor.h
 * @short	����TCP��
 * @author	zhoujj <zhoujj@webcon.com.cn>

 * @class	CTCPAcceptor
 * @short	����TCP��
 **/

#ifndef	__TCPACCEPTOR_H__
#define	__TCPACCEPTOR_H__

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

#include "SockEventCallBack.h"

//�๦������:MyAcDll �����׽��ְ�װ��
class CTCPAcceptor
{
public:
	/** ���캯��
	*/
	//CTCPAcceptor(CTCPAcceptorCallBack* pCallBack);
	CTCPAcceptor(ISockEventCallBack* pCallBack);

	/** ��������
	*/
	~CTCPAcceptor(void);

	/**
	* ����һ��TCP socket
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* ����һ��TCP socket
	* @param ipstr socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(const char* ipstr, unsigned short port, ISockEventCallBack* pSockEventCallBack = 0);

	/**
	* ����һ��TCP socket
	* @param ip socket��ʹ�õ�IP,��Ϊ0,��ϵͳ������е�IP
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @param pSockEventCallBack ����TCP�¼��Ļص��ӿ�
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned long ip, unsigned short port, ISockEventCallBack* pSockEventCallBack= 0);

	/**
	* �رռ�����socket������acceptor������socket
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* �رռ�����socket
	* @return �ɹ��򷵻�0
	*/
	int closeListen();

	/**
	* �ر�ָ����acceptor������socket
	* @param sock Ҫ�رյ�socketֵ
	* @return �ɹ��򷵻�0
	*/
	int close(ACE_HANDLE sock);

	/**
	* ȡ��TCP socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪTCP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* ͨ��TCP socket��������
	* @param sock �����ݷ��͵��ĸ�sock,���sockӦ������
	* ���TCPAcceptor����accept()������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param data socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(ACE_HANDLE sock, unsigned short pdutype, const void* data, unsigned long len);
#if 0
	/** ����ͨ�Ŷ���
	* @param lpLocalHost ����IP
	* @param nLocalPort �����˿�
	* @param lpMainObj ͨ�Ų�������ָ��
	* @return 0-�ɹ�, <0ʧ��
	*/
	int Start(USHORT nLocalPort, LPCTSTR lpLocalHost);


	/** ��������
	* @param wPduType ���ݰ���PDU���
	* @param lpData Ҫ���͵�����
	* @param dwSize ���ݳ���
	* @param hSocket �������ݵ��׽��־��
	*/
	int SendCommData(WORD wPduType, const BYTE* lpData, DWORD dwSize, SOCKET hSocket);
#endif
	/** ֹͣ��������
	*/
	void Stop(void);
	
	/** �ر�ͨ������
	* @param hSocket Ҫ�رյĻỰ���׽��־��
	*/
	void CloseConnection(SOCKET hSocket);

	void setHeadFlag(bool flag) {m_bHeadFlag = flag;}//add 2012-10-18

private:
	IACMainObject* m_pSvrObj; ///< MyAcDllͨ�Ŷ���ӿ�
	ISockEventCallBack* m_pCallBack; ///< ���ݻص��ӿ�ʵ�ֶ���
	//CACPackHead* m_pHeadInfo; ///< ���ݰ�ͷ�������
	CSockHeader* m_pHeadInfo; ///< ���ݰ�ͷ�������
	bool m_bHeadFlag ; //add 2012-10-18

};

#endif	/*__TCPACCEPTOR_H__*/

