/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP���ݴ�����,�ṩ�˶�д���ֻ������ͼ��ַ����Ĳ���

 * @file	SockStream.h
 * @short	TCP/UDP���ݴ�����
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CSockStream
 * @short	TCP/UDP���ݴ�����
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
	* ��������д���CSockStream����,��д�������ֽ���
	* @param dataLen ���ݵ���󳤶�
	*/
	CSockStream(unsigned long dataLen = 4096);
	/**
	* �������ڶ�����CSockStream����,�ٶ�pBuffer����dataLen���ȵ�����
	* �κζ�CSockStream����ĸ��Ľ�Ӱ�������pBuffer�е�����
	* @param pBuffer ����,Ӧȷ��������dataLen����
	* @param dataLen ���ݳ���
	*/
	CSockStream(const void* pBuffer, unsigned long dataLen);

	~CSockStream();

public:
	/**
	 * @class	exception
	 * @short	���������쳣ʱ�׳����쳣��
	 */
	class exception : public std::out_of_range
	{
	public:
		exception(const std::string& e) : std::out_of_range(e)
		{
		}
	};

	/**
	* ���ڲ�ָ�붨λ��ĳһλ��
	* @param pos ���ڲ�ָ�붨λ����λ��,��λ������ڿ�ʼλ��(0)
	* @return �ɹ����ص�ǰ�ڲ�ָ��ָ��λ��,���򷵻�-1
	*/
	long seek(unsigned long pos) const;

	/**
	* �����ڲ�ָ����ָ��λ��
	* @return ���ɹ�����-1
	*/
	unsigned long tell() const;

	/**
	* ���������ֽ�
	* @param pos Ҫ�������ֽ���
	* @return �ɹ�����0
	*/
	int skip(unsigned long pos) const;

	/**
	* ���������ֽ�
	* @param pdata ��Ŷ���������,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
	* @param dataLen Ҫ�������ֽ���
	* @return �ɹ�����0
	*/
	int readBytes(void* pdata, unsigned long dataLen) const;

	/**
	* д�뼸���ֽ�
	* @param pdata Ҫд�������,Ӧ��ȷ����ָ������ӵ��dataLen���ֽ�
	* @param dataLen Ҫд����ֽ���
	* @return �ɹ�����0
	*/
	int writeBytes(const void* pdata, unsigned long dataLen);

	/**
	* ����һ���ַ���(��0��β)
	* @param pdata ��Ŷ������ַ���,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
	* @param dataLen Ҫ�������ֽ���,������β�ַ�0
	* @return �ɹ�����0
	*/
	int readString(char* pdata, unsigned long dataLen) const;
	int readString(std::string& str) const;

	/**
	* д��һ���ַ���(��0��β)
	* @param pdata Ҫд����ַ���
	* @return �ɹ�����0
	*/
	int writeString(const char* pdata);
	int writeString(const std::string& s);

	/**
	* д��һ��bool����
	* @param b Ҫд���bool
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (bool b);

	/**
	* д��һ��char��
	* @param ch Ҫд���ch
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (char ch);

	/**
	* д��һ��unsigned char��
	* @param ch Ҫд���unsigned char
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (unsigned char ch);

	/**
	* д��һ��short��
	* @param s Ҫд���short
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (short s);

	/**
	* д��һ��unsigned short��
	* @param s Ҫд���unsigned short
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (unsigned short s);

	/**
	* д��һ��long��
	* @param l Ҫд���long
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (long l);

	/**
	* д��һ��unsigned long��
	* @param l Ҫд���unsigned long
	* @return ���ɹ�,��fail() == true
	*/
	CSockStream& operator << (unsigned long l);
	
	CSockStream& operator << (long long ll);

	CSockStream& operator << (unsigned long long ll);

	CSockStream& operator << (const std::string&);

	/**
	* ����һ��bool
	* @param b ��Ŷ�����bool
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (bool& b) const;

	/**
	* ����һ��char
	* @param ch ��Ŷ�����char
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (char& ch) const;

	/**
	* ����һ��unsigned char
	* @param ch ��Ŷ�����unsigned char
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (unsigned char& ch) const;

	/**
	* ����һ��short
	* @param s ��Ŷ�����short
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (short& s) const;

	/**
	* ����һ��unsigned short
	* @param s ��Ŷ�����unsigned short
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (unsigned short& s) const;

	/**
	* ����һ��long
	* @param l ��Ŷ�����long
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (long& l) const;

	/**
	* ����һ��unsigned long
	* @param l ��Ŷ�����unsigned long
	* @return ���ɹ�,��fail() == true
	*/
	const CSockStream& operator >> (unsigned long& l) const;

	const CSockStream& operator >> (long long& ll) const;

	const CSockStream& operator >> (unsigned long long& ll) const;

	const CSockStream& operator >> (std::string& s) const;

	/**
	* ���صײ�Ļ�����ָ��
	*/
	const char* getData() const;
	const char* getCurr() const;

	/**
	* ���صײ�����ݵĳ���
	*/
	unsigned long length() const;

	/**
	* ���صײ��ʣ�����ݵĳ���
	*/
	unsigned long left() const;

	int fail()
	{
		return 0;
	}

protected:
	/** ��ֹ����������� */
	CSockStream(const CSockStream& s);
	/** ��ֹ��ֵ���� */
	CSockStream& operator= (const CSockStream& s);

private:
	char*			pBuffer_;	/**< ʵ�����ڶ�д�Ļ����� */
	bool			isNew_;	/**< pBuffer��_�����ö�̬����� */
	unsigned long	maxLen_;	/**< ����������󳤶� */
	unsigned long	dataLen_;	/**< �������е����ݳ��� */
	mutable
	unsigned long	currentPos_;	/**< �������ж�д�ĵ�ǰλ�� */
};

#endif	/*__SOCKSTREAM_H__*/
