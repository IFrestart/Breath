/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandlerʹ�õĻ�����������࣬ȡ��ACE_MessageQueue��

 * @file	BlockQueue.h
 * @short	�������������
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CBlockQueue
 * @short	�������������
 **/

#ifndef __BLOCKQUEUE_H__
#define __BLOCKQUEUE_H__

#include <queue>
#include "SockBlock.h"
#include <assert.h>

class CBlockQueue
{
public:
	/**
	* ���캯��
	* @param highWaterMark ����������Ĭ��ֵΪ1024 * 1024��1M
	* @return 
	*/
	CBlockQueue(unsigned long highWaterMark = 1024 * 1024);

	/**
	* ��������
	* @return 
	*/
	~CBlockQueue();

	/**
	* ��ȡ����������С
	* @return ��������
	*/
	unsigned long high_water_mark() const;

	/**
	* ��������������С
	* @param highWaterMark ���õ�����������ֵ
	* @return 
	*/
	void high_water_mark(unsigned long highWaterMark);

	CSockBlock* getq();
	bool putq(CSockBlock*);

protected:
	/**
	* �������캯��������������ʹ������������
	* @param s ������Դ����
	* @return 
	*/
	CBlockQueue(const CBlockQueue& s);

	/**
	* ��ֵ����������������ʹ������������
	* @param s ��ֵ��Դ����
	* @return 
	*/
	CBlockQueue& operator= (const CBlockQueue& s);

private:
	/**
	* �ͷ�queue_�е����е���Դ
	* @return 
	*/
	void release();
	unsigned long		highWaterMark_;
	unsigned long		dataLen_;
	std::queue<CSockBlock*>	queue_;
};

#endif	/*__BLOCKQUEUE_H__*/
