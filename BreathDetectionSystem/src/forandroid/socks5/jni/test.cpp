#include "UdpSocks5.h"
#include "TcpSocks5.h"
#include "HttpSocks5.h"

#include "CommFunction.h"

int main(int argc, char * argv[])
{
#ifdef _WIN32
	WSACleanup();
#endif

#ifdef _WIN32
/*	WSADATA wsdata;
	WORD wVersionRequested;
	wVersionRequested=MAKEWORD(2,2);
	WSAStartup(wVersionRequested, &wsdata);
*/
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
#endif


	char sendBuf[256]="how are you\n";
	char RecvBuf[256]={0};
	int nlen = (int)strlen(sendBuf);
	int nRet = 0;
	unsigned short nRetPort = 0;
	
#ifdef _WIN32
	SOCKS5_SOCKET RetSock = INVALID_SOCKET;
#else
	SOCKS5_SOCKET RetSock = -1;
#endif
/*
	for(int i = 0; i < 100; i++)
	{

		CTcpSocks5 *pTcpSocket5 = NULL;
		pTcpSocket5 = new CTcpSocks5;
	   

		nRet = pTcpSocket5->Create("192.168.1.30",9990,"192.168.1.50",9877,"192.168.1.30",1080,"username","username",SOCKS5_CLIENT);

		if(nRet == -1)
		{
			printf("error = [%d]  i = [%d]\n",GetLastError(),i);		
			
		}
		else
		{
		    printf("sucess i = [%d]\n",i);
			nRet = pTcpSocket5->Send(sendBuf,nlen,0);
			printf("sendBuf = [%s],nlen = [%d]\n",sendBuf,nlen);
			nRet = pTcpSocket5->Receive(RecvBuf,nlen,0);
			printf("RecvBuf = [%s] nlen = [%d] \n",RecvBuf,nlen);

			RetSock = pTcpSocket5->GetSocket();
		}
		pTcpSocket5->Close();
		delete pTcpSocket5;
#ifdef _WIN32
		Sleep(2000);
#else
		sleep(2);
#endif
		
	}
	getchar();
	*/


	unsigned long  RemoteIp = 0;
	unsigned short RemotePort = 0;
	for(int i = 0; i < 100; i++)
	{
		CUdpSocks5 *pUdpSocket5 = NULL;
		pUdpSocket5 = new CUdpSocks5;
		nRet = pUdpSocket5->Create("192.168.1.30",9999,"192.168.1.30",1080,"username","username");
        if(nRet == -1)
		{
			printf("eerror **udp \n");
		}
		else
		{	
			printf("sucess i = [%d]\n",i);
			RemoteIp = inet_addr("192.168.1.50");
			RemoteIp = ntohl(RemoteIp);
			nRet = pUdpSocket5->Sendto(sendBuf,nlen,RemoteIp,9877);
            printf("sendBuf = [%s],nlen = [%d]\n",sendBuf,nlen);
			
			nRet = pUdpSocket5->ReceiveFrom(RecvBuf,nlen,&RemoteIp,&RemotePort);
			printf("RecvBuf = [%s] nlen = [%d] \n RemoteIp = [%d],RemotePort = [%d]\n",RecvBuf,nlen,RemoteIp,RemotePort);
			RetSock = pUdpSocket5->GetSocket();
			nRetPort = pUdpSocket5->GetPort();
			nRetPort = pUdpSocket5->GetProxyBindPort();
		}
	
		pUdpSocket5->Close();
		delete pUdpSocket5;
#ifdef _WIN32
		Sleep(2000);
#else
		sleep(2);
#endif
	}

/*
	for(int i = 0; i < 100; i++)
	{
		CHttpSocks5 *pHttpSocket5 = NULL;
		pHttpSocket5 = new CHttpSocks5;
	//	nRet = pHttpSocket5->Create("192.168.1.30",9999,"192.168.1.128",12346,"192.168.1.30",808,"username","username");
		nRet = pHttpSocket5->Create("192.168.1.30",9999,"192.168.1.50",9877,"192.168.1.30",808,"username","username");
		if(nRet == -1)
		{
			printf("error http**\n");
		}
		else
		{
			printf("sucess i = [%d]\n",i);
			nRet = pHttpSocket5->Send(sendBuf,nlen,0);
			printf("sendBuf = [%s],nlen = [%d]\n",sendBuf,nlen);
			nRet = pHttpSocket5->Receive(RecvBuf,nlen,0);
			printf("RecvBuf = [%s] nlen = [%d] \n",RecvBuf,nlen);
			RetSock = pHttpSocket5->GetSocket();
		
		}
	
		pHttpSocket5->Close();
		delete pHttpSocket5;
#ifdef _WIN32
		Sleep(2000);
#else
		sleep(2);
#endif
	}
*/
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
