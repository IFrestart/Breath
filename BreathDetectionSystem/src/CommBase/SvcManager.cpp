/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * svc_handler管理接口，被*Acceptor与*Connector类使用

 * @file	SvcManager.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 	svc_handler管理接口
 **/

#include <iostream>

#include "SvcManager.h"


ISvcManager::ISvcManager( )
{
}

ISvcManager::~ISvcManager( )
{
}

int ISvcManager::OnSvcOpen(void* arg)
{
	return 0;
}

int ISvcManager::OnPacketReceived(void*, unsigned short, const void*, unsigned long)
{
	return 0;
}

int ISvcManager::OnSvcClose(void* arg)
{
	return 0;
}

