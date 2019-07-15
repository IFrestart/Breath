/*
   Here's a small example: it asks for a portbase and a destination and starts
   sending packets to that destination.

   IMPORTANT:
   	This sample was written for unix-like environments. For a Windows
   	platform, you'll need to perform Winsock initialisation through
   	WSAStartup.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rtpsession.h"
#include "rtppacket.h"

/*
   This function checks if there was a RTP error. If so, it displays an error
   message and exists.
*/

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		char *errstr;
		
		// This function gets the error string for a certain error code
		errstr = RTPGetErrorString(rtperr);
		
		printf("Error:\n");
		printf("%s\n",errstr);
		exit(-1);
	}
}

/*
   The main routine
*/

int main( int argc, char** argv )
{
	RTPSession sess;
	int portbase = 0;
	char destipstr[256] = {0}, proxyipstr[256] = {0};
	int destport = 0, proxyport = 0;
	unsigned long destip = 0, proxyip = 0;
	char proxyuser[256] = {0}, proxypassword[256] = {0};
	bool hasproxy = false, hasproxyauth = false;
	int status = 0;

	portbase = 8000;
	strcpy( destipstr, "127.0.0.1" );
	destport = 8002;
	switch( argc ) {
	case 8:
		strncpy( proxyuser, argv[7], 255 );
		strncpy( proxypassword, argv[6], 255 );
		hasproxyauth = true;

	case 6:
		proxyport = atoi(argv[5]);
		strncpy( proxyipstr, argv[4], 255 );

		proxyip = inet_addr( proxyipstr );
		if( INADDR_NONE == proxyip ) {
			printf( "Bad IP address specified!\n" );
			return -1;
		}
		proxyip = ntohl(proxyip);
		hasproxy = true;

	case 4:
		destport = atoi( argv[3] );
		strncpy( destipstr, argv[2], 255 );
		portbase = atoi( argv[1] );

		destip = inet_addr( destipstr );
		if( INADDR_NONE == destip ) {
			printf( "Bad IP address specified!\n" );
			return -1;
		}
		destip = ntohl(destip);

		break;
	default:
		printf( "\nusage:%s portbase desthost destport proxyhost proxyport proxyuser proxypassword\n", argv[0] );
		exit( 0 );
		break;
	}
	printf( "\nPortbase=%d DestHost='%s' DestPort=%d ProxyHost='%s' ProxyPort=%d ProxyUser='%s' ProxyPassword='%s'\n\n",
		portbase, destipstr, destport, proxyipstr, proxyport, proxyuser, proxypassword );
		
#ifdef	WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return 1;
	}
	
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return 1; 
	}
#endif

	/*
	   Now, we'll create a RTP session, set the destination, send some
	   packets and poll for incoming data.
	*/
	
	status = sess.Create(portbase);	
	checkerror(status);

	if( hasproxy ) {
		sess.SetProxyAddress(proxyip, proxyport);
	}
	
	if( hasproxyauth ) {
		sess.SetProxyUserPassword(proxyuser, proxypassword);
	}

	status = sess.AddDestination(destip,destport);
	checkerror(status);
	
	char*		dataPtr = "1234567890";
	int			recvPackNum = 0;
	do
	{
		printf("\nSending packet(len=%d) ...\n", strlen(dataPtr));
		
		// send the packet
		status = sess.SendPacket(dataPtr,strlen(dataPtr),0,false,(unsigned long)1);
		checkerror(status);
		
		// poll for incoming data
		status = sess.PollData();
		
		// check incoming packets
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				int		packX, packCC, packM, packPT;
				unsigned long	packSeqNum, packTimeStamp, packSSRC, packPayLoadLen;
				unsigned char*	packPayLoad;
				int		ii;
				unsigned long	jj;
				
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					// You can examine the data here
					packX = pack->HasHeaderExtension() ? 1 : 0;
					packCC = pack->GetNumberOfCSRCs();
					packM = pack->IsMarked() ? 1 : 0;
					packPT = pack->GetPayloadType();
					packSeqNum = pack->GetExtendedSequenceNumber();
					packTimeStamp = pack->GetTimeStamp();
					packSSRC= pack->GetSSRC();
					packPayLoadLen = pack->GetPayloadLength();

					printf("Got %dth packet !\n", recvPackNum );
					++ recvPackNum;
					printf("X=%d ", packX );
					printf("CC=%d ", packCC );
					printf("M=%d ", packM );
					printf("PT=%d ", packPT );
					printf("SeqNum=%lu\t", packSeqNum );
					printf("TimeStamp=%lu\t", packTimeStamp );
					printf("SSRC=%ld\n", packSSRC );
					printf( "[" );
					for( ii = 0; ii < packCC; ++ ii ) {
						unsigned int	packCSRC = pack->GetCSRC( ii );
						printf("CSRC%02d=%04lx ", ii, packCSRC );
					}
					printf( "]\n" );

					printf("PayLoadLen=%d\n", packPayLoadLen );
					packPayLoad = pack->GetPayload( );
					printf( "[" );
					for( jj = 0; jj < packPayLoadLen; ++ jj ) {
						printf("%02d:%02x ", jj, packPayLoad[jj] );
					}
					printf( "]\n" );
					printf( "\n" );
					
					// we don't longer need the packet, so
					// we'll delete it
					delete pack;
				}
			} while (sess.GotoNextSourceWithData());
		}
		
		printf("Press enter to send the next packet\n");
		getchar();
	}while(1);
#ifdef	WIN32
	WSACleanup( );
#endif
	
	return 0;
}
