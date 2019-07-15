#include "RTPConnector.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

//class myRtpTest : public IThreadCallBack
class myRtpTest : public IRTPEventCallBack
{
public:
        myRtpTest();
        ~myRtpTest();
public:
	int sendData(unsigned short pdu,const char* ip, unsigned short port,void* buf,unsigned long len);
	int startServ(unsigned short port);
	int stop();
        //from IThreadCallBack
 //       void* threadProc(void* arg);
public:
        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnNewRTPSource ( unsigned long fromIP, unsigned short fromPort) ;

        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnRTPPacketReceived ( int sock, unsigned short pdutype, const void* data, unsigned long len,
                            unsigned long fromIP, unsigned short fromPort);

        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnErrorRTPSource ( int errCode, unsigned long fromIP, unsigned short fromPort) ;
public:
	CRTPConnector 	  *pRtp;
};

myRtpTest::myRtpTest()
{
	pRtp = NULL;
}

int myRtpTest::stop()
{
	if(pRtp)
	{
		delete pRtp;
		pRtp = NULL;
	}
	return 0;
}

myRtpTest::~myRtpTest()
{
	stop();
}

int myRtpTest::startServ(unsigned short port)
{
	stop();
	pRtp = new CRTPConnector(this);
	return pRtp->create(port);
}

int myRtpTest::sendData(unsigned short pdu,const char* ip,unsigned short port,void* buf,unsigned long len)
{
	if(NULL == pRtp)
	{
		return -1;
	}	
	int ret = -1;
        ret = pRtp->clearDestinations();
        ret = pRtp->addDestination(ip, port);
        ret = pRtp->setSliceSize( 1024 * 10 );
        ret = pRtp->sendPacket ( pdu, buf, len) ;
	
}

int myRtpTest::OnNewRTPSource
						( 
							unsigned long fromIP,
                            unsigned short fromPort
						)
{
#if 0
	std::cerr << "myRtpTest :OnNewRTPSource(" 
				<< "fromip =[" << fromIP
				<< "] fromport = [" << fromPort
				<< "])" << std::endl;
#endif
	return 0;
}

int myRtpTest::OnRTPPacketReceived
						( 
							int sock,
							unsigned short pdutype,
                            const void* data,
							unsigned long len,
                            unsigned long fromIP,
                            unsigned short fromPort
						)
{
	std::cerr << std::endl << "		RTPPacketReceived( sock = [" << sock 
				<< "] pdutype = [" << pdutype
				<< "] len = [ " << len << "] data[" << (char*)data
				<< "] fromip = [" << ip_to_string(fromIP) <<"] fromport = [" << fromPort << "])" << std::endl;
	if(111 == pdutype)
	{
		char sendBuf[256] = "rsp  to  rtp cli\n" ;
		sendData(222,ip_to_string(fromIP),fromPort,sendBuf,strlen(sendBuf));
	}
/*
	char sendBuf[256] = "hhh rtp ccc serv\n" ;
	int ret = -1;
        ret = testRtpConnector_.clearDestinations();
        ret = testRtpConnector_.addDestination(fromIP, fromPort);
        ret = testRtpConnector_.setSliceSize( 1024 * 10 );
        ret = testRtpConnector_.sendPacket ( 112, sendBuf, strlen(sendBuf)) ;
*/
	//dumpHex(data, len);
	//printf(CMDPATH);
	fflush(stdout) ;

	return 0;
}

int myRtpTest::OnErrorRTPSource
						( 
							int errCode,
                            unsigned long fromIP,
                            unsigned short fromPort 
						) 
{
	std::cerr << "myRtpTest :OnErrorRTPSource( errcode = [" <<  errCode
				<< "] fromip =[" << fromIP
				<< "] fromport = [" << fromPort
				<< "])" << std::endl;
	return 0;
}

myRtpTest g_myRtpServ;
myRtpTest g_myRtpCli;

int initNetWorkrtpCli()
{
	char sendBuf[256] = "hhh rtp ccc cli\n" ;
	printf("1: send ; 2:stop\n");
	int ret = 0;
	int cmdNum  = 0;
	
	if(0 != g_myRtpCli.startServ(9000)) 
	{
		printf("error cli startServ\n");
		return 1;
	}
	while(1)
	{
		std::cin >> cmdNum;
		if(1 == cmdNum)
		{
			ret = g_myRtpCli.sendData ( 111,"192.168.1.82" ,9880,sendBuf, strlen(sendBuf)) ;
			if(0 != ret)
			{
				printf("g_myRtpCli.sendData error\n");
			}
		}
		else if(2 == cmdNum)
		{
			g_myRtpCli.stop();
			break;
		}
	}

	return ret;
}

int initNetWorkrtpServ()
{
	if(0 != g_myRtpServ.startServ(9880)) 
	{
		printf("error serv create\n");
		return 1;
	}
        //ret = g_myRtpServ.sendPacket ( 222, sendBuf, strlen(sendBuf)) ;
	return 0;
}
