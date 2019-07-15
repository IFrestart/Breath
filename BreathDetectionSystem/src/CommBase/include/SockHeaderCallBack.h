/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDPЭ��ͷ�ص��ӿ�,����ȡЭ��ͷ����,����PDU�����뱨���峤������Э��ͷ,
 * ��Э��ͷ��ȡPDU�����뱨���峤�ȵȲ���

 * @file	SockHeaderCallBack.h
 * @short	TCP/UDPЭ��ͷ�ص��ӿ�
 * @author	chetang <chetang@webcon.com.cn>

 * @class	ISockHeaderCallBack 
 * @short	TCP/UDPЭ��ͷ�ص��ӿ�
 **/

#ifndef	__SOCKHEADERCALLBACK_H__
#define	__SOCKHEADERCALLBACK_H__


class ISockHeaderCallBack 
{
public:
	ISockHeaderCallBack();
	virtual ~ISockHeaderCallBack();

	/**
	* ȡTCP/UDP����ͷ�ĳ���
	*/
	virtual unsigned short GetPacketHeaderSize() = 0;

	/**
	* ���ݸ�����pduType��bodyLen����TCP/UDP����ͷ��ŵ�pHeader��,
	* Ҫȷ��pHeader�����ٿɴ��TCP/UDP����ͷ����
	* @param pHeader ���ɵ�TCP/UDP����ͷҪ��ŵ�λ��
	* @param pduType PDU����
	* @param bodyLen ���ݰ�����(������TCP/UDP����ͷ)
	* @return �ɹ�����0
	*/
	virtual int GeneratePacketHeader(void* pHeader, 
							unsigned short pduType, unsigned long bodyLen) = 0;

	/**
	* ���ݸ�����TCP/UDP����ͷȡ��pduType��bodyLen,
	* Ҫȷ��pHeader�����ٴ����TCP/UDP����ͷ�ĳ���
	* @param pHeader ���ɵ�TCP/UDP����ͷҪ��ŵ�λ��
	* @param pduType ���PDU����
	* @param bodyLen ������ݰ�����(������TCP/UDP����ͷ)
	* @return �ɹ�����0
	*/
	virtual int ParsePacketHeader(const void* pHeader, 
							unsigned short& pduType, unsigned long& bodyLen) = 0;

protected:

private:

};

#endif	/*__SOCKHEADERCALLBACK_H__*/

