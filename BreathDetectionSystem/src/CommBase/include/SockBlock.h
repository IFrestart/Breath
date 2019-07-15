/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCPSvcHandler使用的缓冲区块类

 * @file	SockBlock.h
 * @short	缓冲区块类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CSockBlock
 * @short	缓冲区块类
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
	* 拷贝构造函数，屏蔽它以免使用它创建对象
	* @param s 拷贝的源对象
	* @return 
	*/
	CSockBlock(const CSockBlock& s);

	/**
	* 赋值函数，屏蔽它以免使用它创建对象
	* @param s 赋值的源对象
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
