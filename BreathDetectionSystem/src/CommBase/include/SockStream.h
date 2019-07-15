/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP数据处理类,提供了读写各种基本类型及字符串的操作

 * @file	SockStream.h
 * @short	TCP/UDP数据处理类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CSockStream
 * @short	TCP/UDP数据处理类
 **/

#ifndef __SOCKSTREAM_H__
#define __SOCKSTREAM_H__

#include <stdexcept>

#include "ace/OS.h"
#include <assert.h>

class CSockStream
{
public:
	/**
	* 构造用于写入的CSockStream对象,可写入的最大字节数
	* @param dataLen 数据的最大长度
	*/
	CSockStream(unsigned long dataLen = 4096);
	/**
	* 构造用于读出的CSockStream对象,假定pBuffer中有dataLen长度的数据
	* 任何对CSockStream对象的更改将影响给定的pBuffer中的数据
	* @param pBuffer 数据,应确保至少有dataLen长度
	* @param dataLen 数据长度
	*/
	CSockStream(const void* pBuffer, unsigned long dataLen);

	~CSockStream();

public:
	/**
	 * @class	exception
	 * @short	处理数据异常时抛出的异常类
	 */
	class exception : public std::out_of_range
	{
	public:
		exception(const std::string& e) : std::out_of_range(e)
		{
		}
	};

	/**
	* 将内部指针定位到某一位置
	* @param pos 将内部指针定位到的位置,此位置相对于开始位置(0)
	* @return 成功返回当前内部指针指的位置,否则返回-1
	*/
	long seek(unsigned long pos) const;

	/**
	* 返回内部指针所指的位置
	* @return 不成功返回-1
	*/
	unsigned long tell() const;

	/**
	* 跳过几个字节
	* @param pos 要跳过的字节数
	* @return 成功返回0
	*/
	int skip(unsigned long pos) const;

	/**
	* 读出几个字节
	* @param pdata 存放读出的数据,应当确保此指针至少能够容纳dataLen个字节
	* @param dataLen 要读出的字节数
	* @return 成功返回0
	*/
	int readBytes(void* pdata, unsigned long dataLen) const;

	/**
	* 写入几个字节
	* @param pdata 要写入的数据,应当确保此指针至少拥有dataLen个字节
	* @param dataLen 要写入的字节数
	* @return 成功返回0
	*/
	int writeBytes(const void* pdata, unsigned long dataLen);

	/**
	* 读出一个字符串(以0结尾)
	* @param pdata 存放读出的字符串,应当确保此指针至少能够容纳dataLen个字节
	* @param dataLen 要读出的字节数,包括结尾字符0
	* @return 成功返回0
	*/
	int readString(char* pdata, unsigned long dataLen) const;
	int readString(std::string& str) const;

	/**
	* 写入一个字符串(以0结尾)
	* @param pdata 要写入的字符串
	* @return 成功返回0
	*/
	int writeString(const char* pdata);
	int writeString(const std::string& s);

	/**
	* 写入一个bool数据
	* @param b 要写入的bool
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (bool b);

	/**
	* 写入一个char据
	* @param ch 要写入的ch
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (char ch);

	/**
	* 写入一个unsigned char据
	* @param ch 要写入的unsigned char
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (unsigned char ch);

	/**
	* 写入一个short据
	* @param s 要写入的short
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (short s);

	/**
	* 写入一个unsigned short据
	* @param s 要写入的unsigned short
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (unsigned short s);

	/**
	* 写入一个long据
	* @param l 要写入的long
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (long l);

	/**
	* 写入一个unsigned long据
	* @param l 要写入的unsigned long
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << (unsigned long l);
	
	CSockStream& operator << (long long ll);

	CSockStream& operator << (unsigned long long ll);

	CSockStream& operator << (const std::string&);

	/**
	* 读出一个bool
	* @param b 存放读出的bool
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (bool& b) const;

	/**
	* 读出一个char
	* @param ch 存放读出的char
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (char& ch) const;

	/**
	* 读出一个unsigned char
	* @param ch 存放读出的unsigned char
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (unsigned char& ch) const;

	/**
	* 读出一个short
	* @param s 存放读出的short
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (short& s) const;

	/**
	* 读出一个unsigned short
	* @param s 存放读出的unsigned short
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (unsigned short& s) const;

	/**
	* 读出一个long
	* @param l 存放读出的long
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (long& l) const;

	/**
	* 读出一个unsigned long
	* @param l 存放读出的unsigned long
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> (unsigned long& l) const;

	const CSockStream& operator >> (long long& ll) const;

	const CSockStream& operator >> (unsigned long long& ll) const;

	const CSockStream& operator >> (std::string& s) const;

	/**
	* 返回底层的缓冲区指针
	*/
	const char* getData() const;
	const char* getCurr() const;

	/**
	* 返回底层的数据的长度
	*/
	unsigned long length() const;

	/**
	* 返回底层的剩余数据的长度
	*/
	unsigned long left() const;

	int fail()
	{
		return 0;
	}

protected:
	/** 防止拷贝构造操作 */
	CSockStream(const CSockStream& s);
	/** 防止赋值操作 */
	CSockStream& operator= (const CSockStream& s);

private:
	char*			pBuffer_;	/**< 实际用于读写的缓冲区 */
	bool			isNew_;	/**< pBuffer是_否是用动态分配的 */
	unsigned long	maxLen_;	/**< 缓冲区的最大长度 */
	unsigned long	dataLen_;	/**< 缓冲区中的数据长度 */
	mutable
	unsigned long	currentPos_;	/**< 缓冲区中读写的当前位置 */
};

#endif	/*__SOCKSTREAM_H__*/
