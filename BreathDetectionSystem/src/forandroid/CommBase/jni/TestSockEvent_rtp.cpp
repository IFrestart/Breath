#include "TestSockEvent_rtp.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

int TestSockEvent_rtp::OnNewRTPSource
						( 
							unsigned long fromIP,
                            unsigned short fromPort
						)
{
#if 0
	std::cerr << "TestSockEvent_rtp :OnNewRTPSource(" 
				<< "fromip =[" << fromIP
				<< "] fromport = [" << fromPort
				<< "])" << std::endl;
#endif
	return 0;
}

int TestSockEvent_rtp::OnRTPPacketReceived
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

int TestSockEvent_rtp::OnErrorRTPSource
						( 
							int errCode,
                            unsigned long fromIP,
                            unsigned short fromPort 
						) 
{
	std::cerr << "TestSockEvent_rtp :OnErrorRTPSource( errcode = [" <<  errCode
				<< "] fromip =[" << fromIP
				<< "] fromport = [" << fromPort
				<< "])" << std::endl;
	return 0;
}

