/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	test.cpp
 * @author	zhoujj <zhoujj@webcon.com.cn>
 * @data	04/2004
 * @short 
 **/
#include "SendConfig.h"

int main()
{
	unsigned long ip = 0;
	unsigned short port = 0;
	unsigned short packetnum = 0;
	int ret = 0;

	CSendConfig *pSendConfig = NULL;

	pSendConfig = new CSendConfig;
	if(pSendConfig->open("./SendConfig.xml") < 0)
	{
		printf("error open file\n");
	}


/********getServerAddr****/

	ret = pSendConfig->getServerAddr(ip,port);
	printf("\n\n********getServerAdder ***************\n");
	printf("ip = [%lu] port = [%d]\n",ip,port);

/********getPacketNum   *****/
	ret = pSendConfig->getPacketNum(packetnum);
	printf("\n\n********getPacketNum packetnum =[%d] ***************\n",packetnum);


/********getPacket** by no***/
	{
		unsigned short no = 3,pdutype = 20;
		unsigned long len = 1024;
		char *data = NULL;
		data = new char[1024];
		ret = pSendConfig->getPacket( no, (void *&)data, len, pdutype ); 
		if(ret < 0)
		{
			printf("getPacket by no error \n");
		}
		printf("\n\n********getPacket by no ***************\n");
		printf("return get packet by no len =[%lu],pdutupe=[%d]\n",len,pdutype);
	
		delete [](char *)data;
	}


/********getPacket** by pdutype***/
	{
		unsigned short pdutype = 1000;
		unsigned long len = 1024;
		char *data = NULL;
		data = new char[1024];
		ret = pSendConfig->getPacket(pdutype ,(void *&)data, len); 
		if(ret < 0)
		{
			printf("getPacket by pdutype error \n");
		}
		printf("\n\n********getPacket by pdutype ***************\n");
		printf("return get getPacket by pdutype  len =[%lu],pdutupe=[%d]\n",len,pdutype);
		delete [](char *)data;
	}


/********getIsrspByno ***/
	{
		unsigned short no  = 1;
		bool res = false;
		unsigned short rsppdutype = 1001;
		ret = pSendConfig->getIsrspByNo(no, res,rsppdutype); 
		if(ret < 0)
		{
			printf("getIsrspByno  error \n");
		}
		printf("\n\n********getIsrspByno ***************\n");
		if(res )
		{
			printf(" getIsrspByNo is true,rsppdutype = [%d] \n",rsppdutype);
		}
		else
		{
			printf("getIsrspByNo is false\n");
		}
	}

/********getIsrspByPdutype ***/
	{
		unsigned short pdutype = 1001;
		unsigned short rsppdutype = 1001;
		bool res = false;
		ret = pSendConfig->getIsrspByPdutype(pdutype, res,rsppdutype); 
		if(ret < 0)
		{
			printf("getIsrspByPdutype   error \n");
		}
		printf("\n\n********getIsrspByPdutype ***************\n");
		if(res )
		{
			printf("getIsrspByPdutype  is true ,rsppdutype = [%d]\n",rsppdutype);
		}
		else
		{
			printf(" getIsrspByPdutype is false\n");
		}
	}

/********checkRspPdu ***/
	{
		unsigned short pdutype = 1000,rsppdutype=101;
		unsigned long len = 0;
		bool res = false;
		char *data = NULL;
		data = new char[1024];
		ret = pSendConfig->checkRspPdu(pdutype, rsppdutype,(void *)data,len,res); 
		if(ret < 0)
		{
			printf(" checkrspPdu error \n");
		}
		printf("\n\n********checkrspPdu  ***************\n");
		printf("return checkRspPdu len =[%lu],pdutupe=[%d]\n",len,pdutype);
		if(res )
		{
			printf("checkrspPdu is true\n");
		}
		else
		{
			printf("checkrspPdu is false\n");
		}
		delete [](char *)data;
	}

/********checkRspPdu ***/
	ret = pSendConfig->close();
	if(pSendConfig)
		delete pSendConfig;
	return 0;
	}
