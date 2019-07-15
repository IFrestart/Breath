/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * �ṩһЩ���õĺ���,����IP��ַ���ַ����볤����֮���ת��
 * ����ʮ�����Ƶĸ�ʽ��ӡ�ڴ���

 * @file	Tools.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	���ú���
 **/

#ifndef	__TOOLS_H__
#define	__TOOLS_H__

//#include "ace/OS.h"
#include "LinuxDef.h"

/**
* ���ַ�����IPת������unsigned long(�����ֽ���)��ʾ
* @param ipstr IP�ַ���,��ʽӦΪ"xxx.xxx.xxx.xxx"
* @return ���ɹ��򷵻�0
*/
unsigned long ip_to_long(const char *ipstr);

/**
* ����unsigned long(�����ֽ���)��ʾ��IPת�����ַ���
* 
* @return ���ɹ��򷵻�0
*/
const char* ip_to_string(unsigned long ip);

unsigned long getSockIP(ACE_HANDLE sock);
unsigned short getSockPort(ACE_HANDLE sock);

int getSockName(ACE_HANDLE hSocket, char* lpLocalHost, unsigned short* lpLocalPort,char* lpLocalMask);

/**
* ��ָ�������ݿ���HEX�ķ�ʽ��ʾ����
* @param data Ҫ��ӡ�����ݿ�ָ��
* @param len Ҫ��ӡ�����ݿ�ĳ���
* @param numperline ÿһ�д�ӡ���ֽڸ���,Ĭ��Ϊ32���ֽ�
* @return �޷���ֵ
*/
void dumpHex(const void* data, unsigned long len, unsigned short numperline = 32);

#endif	/*__TOOLS_H__*/

