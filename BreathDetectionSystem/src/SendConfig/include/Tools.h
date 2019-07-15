/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 Tools.h
 * @author	 chetang <chetang@webcon.com.cn>

 * @short 
 **/

#ifndef	__TOOLS_H__
#define	__TOOLS_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
* ���ַ�����IPת������unsigned long(�����ֽ���)��ʾ
* @return ���ɹ��򷵻�0
*/
unsigned long ip_to_long( const char *ipstr );

/**
* ����unsigned long(�����ֽ���)��ʾ��IPת�����ַ���
* @return ���ɹ��򷵻�0
*/
char* ip_to_string( unsigned long ip );

/**
* ��ָ�������ݿ���HEX�ķ�ʽ��ʾ����
* @return �޷���ֵ
*/
void dumpHex( const void* data, unsigned long len, unsigned short numperline = 32 );

#endif	/*__TOOLS_H__*/

