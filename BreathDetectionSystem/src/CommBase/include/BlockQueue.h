/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandler使用的缓冲区块队列类，取代ACE_MessageQueue类

 * @file	BlockQueue.h
 * @short	缓冲区块队列类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CBlockQueue
 * @short	缓冲区块队列类
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
	* 构造函数
	* @param highWaterMark 最大的容量，默认值为1024 * 1024即1M
	* @return 
	*/
	CBlockQueue(unsigned long highWaterMark = 1024 * 1024);

	/**
	* 析构函数
	* @return 
	*/
	~CBlockQueue();

	/**
	* 获取最大的容量大小
	* @return 最大的容量
	*/
	unsigned long high_water_mark() const;

	/**
	* 设置最大的容量大小
	* @param highWaterMark 设置的最大的容量的值
	* @return 
	*/
	void high_water_mark(unsigned long highWaterMark);

	CSockBlock* getq();
	bool putq(CSockBlock*);

protected:
	/**
	* 拷贝构造函数，屏蔽它以免使用它创建对象
	* @param s 拷贝的源对象
	* @return 
	*/
	CBlockQueue(const CBlockQueue& s);

	/**
	* 赋值函数，屏蔽它以免使用它创建对象
	* @param s 赋值的源对象
	* @return 
	*/
	CBlockQueue& operator= (const CBlockQueue& s);

private:
	/**
	* 释放queue_中的所有的资源
	* @return 
	*/
	void release();
	unsigned long		highWaterMark_;
	unsigned long		dataLen_;
	std::queue<CSockBlock*>	queue_;
};

#endif	/*__BLOCKQUEUE_H__*/
