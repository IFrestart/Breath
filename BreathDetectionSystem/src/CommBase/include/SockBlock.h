/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandlerʹ�õĻ���������

 * @file	SockBlock.h
 * @short	����������
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CSockBlock
 * @short	����������
 **/

#ifndef __SOCKBLOCK_H__
#define __SOCKBLOCK_H__

class CSockBlock
{
public:
	CSockBlock(unsigned long dataLen = 4096);

	~CSockBlock();

	char* wr_ptr();
	unsigned long wr_ptr(long);
	void wr_ptr(char*);
	char* rd_ptr();
	unsigned long rd_ptr(long);

	char* base() const;
	void release();
	unsigned long size() const;
	bool copy(const char*, unsigned long);

protected:
	/**
	* �������캯��������������ʹ������������
	* @param s ������Դ����
	* @return 
	*/
	CSockBlock(const CSockBlock& s);

	/**
	* ��ֵ����������������ʹ������������
	* @param s ��ֵ��Դ����
	* @return 
	*/
	CSockBlock& operator= (const CSockBlock& s);

private:
	char*		pBuffer_;
	unsigned long	maxLen_;
	unsigned long	wrPos_;
	unsigned long	rdPos_;
};

#endif	/*__SOCKBLOCK_H__*/
