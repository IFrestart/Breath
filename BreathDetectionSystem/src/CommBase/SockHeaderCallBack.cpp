/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * TCP/UDP协议头回调接口,包括取协议头长度,根据PDU类型与报文体长度生成协议头,
 * 从协议头中取PDU类型与报文体长度等操作

 * @file	SockHeaderCallBack.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	TCP/UDP协议头回调接口
 **/

#include "SockHeaderCallBack.h"


ISockHeaderCallBack::ISockHeaderCallBack()
{
}

ISockHeaderCallBack::~ISockHeaderCallBack()
{
}


