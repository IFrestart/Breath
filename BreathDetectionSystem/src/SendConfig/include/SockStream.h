/*-------------------------------------------------------------------------*/
/*																		   */
/*	C++ utility header file												   */
/*	C++ utility for network encoding/decoding service					   */
/*																		   */
/*	SockStream.H															   */
/*																		   */
/*	Copyright ( c ) 1997 Stellar Computing Corp.							   */
/*  All rights reserved													   */
/*																		   */
/*-------------------------------------------------------------------------*/

#ifndef __SOCKSTREAM_H__
#define __SOCKSTREAM_H__
#include <netinet/in.h>


class CSockStream
{
public:
	/**
	* 构造用于写入的CSockStream对象,可写入的最大字节数
	* @param dataLen 数据的最大长度
	*/
	CSockStream( unsigned long dataLen = 4096 );
	/**
	* 构造用于读出的CSockStream对象,假定pBuffer中有dataLen长度的数据
	* 任何对CSockStream对象的更改将影响给定的pBuffer中的数据
	* @param pBuffer 数据,应确保至少有dataLen长度
	* @param dataLen 数据长度
	*/
	CSockStream( const void* pBuffer, unsigned long dataLen );

	~CSockStream( );

public:
	/**
	* 将内部指针定位到某一位置
	* @param pos 将内部指针定位到的位置,此位置相对于开始位置(0)
	* @return 成功返回当前内部指针指的位置,否则返回-1
	*/
	long seek( unsigned long pos ) const;

	/**
	* 返回内部指针所指的位置
	* @return 不成功返回-1
	*/
	unsigned long tell(  ) const;

	/**
	* 跳过几个字节
	* @param pos 要跳过的字节数
	* @return 成功返回0
	*/
	int skip( unsigned long pos ) const;

	/**
	* 读出几个字节
	* @param pdata 存放读出的数据,应当确保此指针至少能够容纳dataLen个字节
	* @param dataLen 要读出的字节数
	* @return 成功返回0
	*/
	int readBytes( void* pdata, unsigned long dataLen ) const;

	/**
	* 写入几个字节
	* @param pdata 要写入的数据,应当确保此指针至少拥有dataLen个字节
	* @param dataLen 要写入的字节数
	* @return 成功返回0
	*/
	int writeBytes( const void* pdata, unsigned long dataLen );

	/**
	* 读出一个字符串(以'\0'结尾)
	* @param pdata 存放读出的字符串,应当确保此指针至少能够容纳dataLen个字节
	* @param dataLen 要读出的字节数,包括结尾字符'\0'
	* @return 成功返回0
	*/
	int readString( char* pdata, unsigned long dataLen ) const;

	/**
	* 写入一个字符串(以'\0'结尾)
	* @param pdata 要写入的字符串
	* @return 成功返回0
	*/
	int writeString( const char* pdata );

	/**
	* 写入各种基本数据类型
	* @param 要写入的数据
	* @return 不成功,则fail() == true
	*/
	CSockStream& operator << ( bool b );
	CSockStream& operator << ( char ch );
	CSockStream& operator << ( unsigned char ch );
	CSockStream& operator << ( short s );
	CSockStream& operator << ( unsigned short s );
	CSockStream& operator << ( long l );
	CSockStream& operator << ( unsigned long l );

	/**
	* 读出各种基本数据类型
	* @param 读出的数据要存放的位置
	* @return 不成功,则fail() == true
	*/
	const CSockStream& operator >> ( bool& b ) const;
	const CSockStream& operator >> ( char& ch ) const;
	const CSockStream& operator >> ( unsigned char& ch ) const;
	const CSockStream& operator >> ( short& s ) const;
	const CSockStream& operator >> ( unsigned short& s ) const;
	const CSockStream& operator >> ( long& l ) const;
	const CSockStream& operator >> ( unsigned long& l ) const;

	/**
	* 判断operator << 与operator >>操作 是否成功
	* @return 若不成功,则返回true
	*/
	bool fail( ) const;

	/**
	* 返回底层的数据指针
	*/
	const char* getData( ) const;

	/**
	* 返回底层的数据的长度
	*/
	unsigned long length( ) const;

private:
	/* 防止自动产生拷贝构造函数与赋值运行符 */
	CSockStream( const CSockStream& s );
	CSockStream& operator= ( const CSockStream& s );

	char*			m_pBuffer;
	/* m_pBuffer是否是用动态分配的 */
	bool			m_isNew;
	unsigned long	m_maxLen;
	unsigned long	m_dataLen;
	mutable
	unsigned long	m_currentPos;
	mutable
	unsigned long	m_failed;
};

#endif	/*__SOCKSTREAM_H__*/
