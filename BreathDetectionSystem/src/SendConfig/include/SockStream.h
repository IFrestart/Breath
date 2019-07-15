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
	* ��������д���CSockStream����,��д�������ֽ���
	* @param dataLen ���ݵ���󳤶�
	*/
	CSockStream( unsigned long dataLen = 4096 );
	/**
	* �������ڶ�����CSockStream����,�ٶ�pBuffer����dataLen���ȵ�����
	* �κζ�CSockStream����ĸ��Ľ�Ӱ�������pBuffer�е�����
	* @param pBuffer ����,Ӧȷ��������dataLen����
	* @param dataLen ���ݳ���
	*/
	CSockStream( const void* pBuffer, unsigned long dataLen );

	~CSockStream( );

public:
	/**
	* ���ڲ�ָ�붨λ��ĳһλ��
	* @param pos ���ڲ�ָ�붨λ����λ��,��λ������ڿ�ʼλ��(0)
	* @return �ɹ����ص�ǰ�ڲ�ָ��ָ��λ��,���򷵻�-1
	*/
	long seek( unsigned long pos ) const;

	/**
	* �����ڲ�ָ����ָ��λ��
	* @return ���ɹ�����-1
	*/
	unsigned long tell(  ) const;

	/**
	* ���������ֽ�
	* @param pos Ҫ�������ֽ���
	* @return �ɹ�����0
	*/
	int skip( unsigned long pos ) const;

	/**
	* ���������ֽ�
	* @param pdata ��Ŷ���������,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
	* @param dataLen Ҫ�������ֽ���
	* @return �ɹ�����0
	*/
	int readBytes( void* pdata, unsigned long dataLen ) const;

	/**
	* д�뼸���ֽ�
	* @param pdata Ҫд�������,Ӧ��ȷ����ָ������ӵ��dataLen���ֽ�
	* @param dataLen Ҫд����ֽ���
	* @return �ɹ�����0
	*/
	int writeBytes( const void* pdata, unsigned long dataLen );

	/**
	* ����һ���ַ���(��'\0'��β)
	* @param pdata ��Ŷ������ַ���,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
	* @param dataLen Ҫ�������ֽ���,������β�ַ�'\0'
	* @return �ɹ�����0
	*/
	int readString( char* pdata, unsigned long dataLen ) const;

	/**
	* д��һ���ַ���(��'\0'��β)
	* @param pdata Ҫд����ַ���
	* @return �ɹ�����0
	*/
	int writeString( const char* pdata );

	/**
	* д����ֻ�����������
	* @param Ҫд�������
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << ( bool b );
	CSockStream& operator << ( char ch );
	CSockStream& operator << ( unsigned char ch );
	CSockStream& operator << ( short s );
	CSockStream& operator << ( unsigned short s );
	CSockStream& operator << ( long l );
	CSockStream& operator << ( unsigned long l );

	/**
	* �������ֻ�����������
	* @param ����������Ҫ��ŵ�λ��
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> ( bool& b ) const;
	const CSockStream& operator >> ( char& ch ) const;
	const CSockStream& operator >> ( unsigned char& ch ) const;
	const CSockStream& operator >> ( short& s ) const;
	const CSockStream& operator >> ( unsigned short& s ) const;
	const CSockStream& operator >> ( long& l ) const;
	const CSockStream& operator >> ( unsigned long& l ) const;

	/**
	* �ж�operator << ��operator >>���� �Ƿ�ɹ�
	* @return �����ɹ�,�򷵻�true
	*/
	bool fail( ) const;

	/**
	* ���صײ������ָ��
	*/
	const char* getData( ) const;

	/**
	* ���صײ�����ݵĳ���
	*/
	unsigned long length( ) const;

private:
	/* ��ֹ�Զ������������캯���븳ֵ���з� */
	CSockStream( const CSockStream& s );
	CSockStream& operator= ( const CSockStream& s );

	char*			m_pBuffer;
	/* m_pBuffer�Ƿ����ö�̬����� */
	bool			m_isNew;
	unsigned long	m_maxLen;
	unsigned long	m_dataLen;
	mutable
	unsigned long	m_currentPos;
	mutable
	unsigned long	m_failed;
};

#endif	/*__SOCKSTREAM_H__*/
