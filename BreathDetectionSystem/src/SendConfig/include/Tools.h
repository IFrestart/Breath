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
* 将字符串的IP转换成以unsigned long(本地字节序)表示
* @return 不成功则返回0
*/
unsigned long ip_to_long( const char *ipstr );

/**
* 将以unsigned long(本地字节序)表示的IP转换成字符串
* @return 不成功则返回0
*/
char* ip_to_string( unsigned long ip );

/**
* 将指定的数据块以HEX的方式显示出来
* @return 无返回值
*/
void dumpHex( const void* data, unsigned long len, unsigned short numperline = 32 );

#endif	/*__TOOLS_H__*/

