/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * UDP�������ݽ����뷢�ʹ�����,�ṩ��UDP���ӵĴ�,�ر��뷢�����ݵȲ���
 * ������һ���������ĵ���ʱ�����ʱִ�лص�.

 * @file	 UDPConnector.h
 * @short 	UDP������
 * @author	 zhoujj <zhoujj@webcon.com.cn>

 * @class	CUDPConnector
 * @short 	UDP������
 **/

#ifndef	__UDPCONNECTOR_H__
#define	__UDPCONNECTOR_H__

#include "BaseACObject.h"
#include "ACCommMainObject.h"
//#include  "HighCommCon.h"

class IACMainObject;
//class CACPackHead;
class CSockHeader;

// ��װMyAcDll��UDP��TCP�����׽���
class CUDPConnector
{
public:
	/** ���캯��
	*/
	CUDPConnector(ISockEventCallBack* pCallBack);

	/** ��������
	*/
	~CUDPConnector(void);

	/**
	* ����һ��UDP socket
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @return �ɹ��򷵻�0
	*/
	int create(unsigned short port);

	/**
	* ����һ��UDP socket
	* @param ipstr socket��ʹ�õ�IP��ַ,��Ϊ0,������е�IP��ַ
	* @param port socket��ʹ�õĶ˿ں�,��Ϊ0,����ϵͳ�Զ�ѡ��һ��
	* @return �ɹ��򷵻�0
	*/
	int create(const char* ipstr, unsigned short port);

	/**
	* �ر�UDP socket
	* @return �ɹ��򷵻�0
	*/
	int close();

	/**
	* ͨ��UDP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param buf socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @param ip socket�����ݷ��͵��ĸ�IP
	* @param port socket�����ݷ��͵��ĸ�PORT
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, 
						const void* buf, unsigned long len,
						unsigned long ip, unsigned short port);

	/**
	* ͨ��UDP socket��������
	* @param pdutype socketҪ�������ݵ�PDU����
	* @param buf socketҪ���͵�����
	* @param len socketҪ�������ݵĳ���
	* @param ipstr socket�����ݷ��͵��ĸ�IP
	* @param port socket�����ݷ��͵��ĸ�PORT
	* @return �ɹ��򷵻�0
	*/
	int sendPacket(unsigned short pdutype, 
						const void* buf, unsigned long len,
						const char* ipstr, unsigned short port);

	/**
	* ȡ��UDP socket,�������򷵻�ACE_INVALID_HANDLE
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪUDP socket
	*/
	ACE_HANDLE getSock() const;

	/**
	* �ж�UDP socket�Ƿ��Ѿ��ر�
	* @return ���Ѿ��ر��򷵻�true,���򷵻�false
	*/
	bool isClosed() const;
	/** ֹͣͨ�Ŷ���
	*/
	void Stop(int nType=0);

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

};

#endif	/*__UDPCONNECTOR_H__*/

