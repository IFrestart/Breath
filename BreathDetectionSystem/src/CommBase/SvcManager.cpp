/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * svc_handler����ӿڣ���*Acceptor��*Connector��ʹ��

 * @file	SvcManager.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 	svc_handler����ӿ�
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

