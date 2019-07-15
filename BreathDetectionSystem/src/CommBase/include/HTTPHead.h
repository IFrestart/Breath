/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTPЭ��ͷ

 * @file	HTTPHead.h
 * @short	HTTPЭ��ͷ
 * @author	eagle, chetang <chetang@webcon.com.cn>

 * @class	CHTTPHeader
 * @short	HTTPЭ��ͷ
 **/

#ifndef	__HTTPHEAD_H__
#define	__HTTPHEAD_H__

#include "ace/OS.h"
#include "ace/Event_Handler.h"

#include "SockHeader.h"


class CHTTPHeader : public CSockHeader
{
public:
	/**
	* ISockHeaderCallBack�е�ʵ�ֺ���
	* @return ����HTTPЭ��ͷ+�Զ���Э��ͷ�ĳ���
	* @see ISockHeaderCallBack::GetPacketHeaderSize()
	*/
	virtual unsigned short GetPacketHeaderSize();
	virtual unsigned short GetHttpHeaderSize();

	/**
	* ISockHeaderCallBack�е�ʵ�ֺ���
	* @return �ɹ��򷵻�0
	* @see ISockHeaderCallBack::GeneratePacketHeader()
	*/
	virtual int GeneratePacketHeader(void*, unsigned short, unsigned long);

private:
	enum {HTTP_HEADER_LENGTH = 82};
	typedef CSockHeader	PARENT;
};

#endif	/*__HTTPHEAD_H__*/

