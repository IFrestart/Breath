/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 提供一些常用的函数,包括IP地址在字符串与长整型之间的转换
 * 与以十六进制的格式打印内存块等

 * @file	Tools.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	常用函数
 **/

#ifndef	__TOOLS_H__
#define	__TOOLS_H__

//#include "ace/OS.h"
#include "LinuxDef.h"

/**
* 将字符串的IP转换成以unsigned long(本地字节序)表示
* @param ipstr IP字符串,格式应为"xxx.xxx.xxx.xxx"
* @return 不成功则返回0
*/
unsigned long ip_to_long(const char *ipstr);

/**
* 将以unsigned long(本地字节序)表示的IP转换成字符串
* 
* @return 不成功则返回0
*/
const char* ip_to_string(unsigned long ip);

unsigned long getSockIP(ACE_HANDLE sock);
unsigned short getSockPort(ACE_HANDLE sock);

int getSockName(ACE_HANDLE hSocket, char* lpLocalHost, unsigned short* lpLocalPort,char* lpLocalMask);

/**
* 将指定的数据块以HEX的方式显示出来
* @param data 要打印的数据块指针
* @param len 要打印的数据块的长度
* @param numperline 每一行打印的字节个数,默认为32个字节
* @return 无返回值
*/
void dumpHex(const void* data, unsigned long len, unsigned short numperline = 32);

#endif	/*__TOOLS_H__*/

