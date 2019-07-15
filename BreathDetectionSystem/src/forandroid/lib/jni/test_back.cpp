#include<jni.h>
#include "Thread.h"
#include "Timer.h"
#include "ACSocket.h"
#include "Tools.h"
#include "TestSockEvent_rtp.h"



CThread	g_myThread;	
myThreadCallBack g_aUser;
Timer	g_Timer; 
unsigned long ggg_count = 0;

void myTimer(unsigned long id ,void* data)
{
	ggg_count++;	
}

int initThread()
{
	int aa = 10;
	return g_myThread.start(&g_aUser,(void*)&aa);
}

int initTimer()
{
	int aa = 10;
	return g_Timer.start(myTimer,2000,(void*)&aa);
}
char g_recive[256]={0};
int initNetWorktcp()
{
	CACSocket sock ;
	char sendBuf[256] = "hhh my test dsfsdfs\n" ;
	SOCKET ret =  sock.Create(SOCK_STREAM,0,9000,TRUE);
	if(INVALID_SOCKET == ret)
	{
		printf("error create\n");
		return 1;
	}	
	int nResult = sock.Connect("192.168.1.81",9877);
	if (nResult== SOCKET_ERROR)
	{
		printf("error connect\n");
		return 2;
	}
	printf("connect sucess=[%d]\n",nResult);
	int nlen = sock.Send(sendBuf, strlen(sendBuf));
	if(nlen < 0)
	{
		printf("error send\n");
		return 3;
	}
	printf("send sucess count=[%d], len=[%d]\n",nlen,strlen(sendBuf));

        fd_set readFds;

        FD_ZERO(&readFds);
        FD_SET(sock.GetSocket(), &readFds);
        int maxfd = sock.GetSocket() + 1;
        int nRetCode = sock.Select(maxfd, &readFds, 10000);
	if(nRetCode <= 0)
	{
		printf("error select\n");
		return 4;
	}

	nResult = sock.Receive(g_recive,nlen);
	if(nResult < 0)
	{
		printf("error recive\n");
		return 5;
	}

	printf("recive sucess count=[%d] grecive[%s]\n",nResult,g_recive);

	char lpLocalHost[16];
        char lpLocalMask[16];
        memset(lpLocalHost,0,sizeof(lpLocalHost));
        memset(lpLocalMask,0,sizeof(lpLocalMask));
        unsigned short localPort = 0;
	int sss = sock.GetSocket();
        getSockName(sss, lpLocalHost, &localPort, lpLocalMask);

	printf("sssss =[%s] port=[%d]\n", ip_to_string(getSockIP(sss)),getSockPort(sss));
 printf("AuthServSockEvent::MAINCERT_RSP_TOCLI sock=[%d] localPort=[%d] lpLocalHost=%s] localMask[%s] \n",sss,localPort,lpLocalHost,lpLocalMask);
	sock.Close();

	return 0;
}

int initNetWorkudpServ()
{
	CACSocket sock ;
	char sendBuf[256] = "hhh my test dsfsdfs serv\n" ;
	SOCKET ret =  sock.Create(SOCK_DGRAM,"192.168.1.81",9877,FALSE);
	if(INVALID_SOCKET == ret)
	{
		printf("error create\n");
		return 1;
	}	
	while(1)
	{
		fd_set readFds;

		FD_ZERO(&readFds);
		FD_SET(sock.GetSocket(), &readFds);
		int maxfd = sock.GetSocket() + 1;
		int nRetCode = sock.Select(maxfd, &readFds, 10000);
		if(nRetCode <= 0)
		{
			printf("error select\n");
			continue;
		}

		char remoteHost[16];
		unsigned short remotePort;
		memset(remoteHost,0,sizeof(remoteHost));
		int nlen = 256;
		int nResult = sock.ReceiveFrom(g_recive,nlen,remoteHost,&remotePort);

		if(nResult < 0)
		{
			printf("error recive\n");
			return 5;
		}

		printf("recive sucess count=[%d] grecive[%s]\n",nResult,g_recive);
	 printf("remoteHost [%s] remoteport[%d] \n",remoteHost,remotePort);
		nResult = sock.SendTo(sendBuf,strlen(sendBuf),remoteHost,remotePort);
		if(nResult < 0)
		{
			printf("error send\n");
			return 6;
		}
	}

	return 0;
}
int initNetWorkudpCli()
{
	CACSocket sock ;
	char sendBuf[256] = "hhh hhhhhhhhhhhh bbbbbbbbb cli\n" ;
	SOCKET ret =  sock.Create(SOCK_DGRAM,0,9000,TRUE);
	if(INVALID_SOCKET == ret)
	{
		printf("error create\n");
		return 1;
	}	
	int nResult = sock.SendTo(sendBuf,strlen(sendBuf),"192.168.1.81",9877);
	if(nResult < 0)
	{
		printf("error send\n");
		return 2;
	}
	fd_set readFds;

	FD_ZERO(&readFds);
	FD_SET(sock.GetSocket(), &readFds);
	int maxfd = sock.GetSocket() + 1;
	int nRetCode = sock.Select(maxfd, &readFds, 10000);
	if(nRetCode <= 0)
	{
		printf("error select\n");
		return 3;
	}

	char remoteHost[16];
	unsigned short remotePort;
	memset(remoteHost,0,sizeof(remoteHost));
	int nlen = 256;
	nResult = sock.ReceiveFrom(g_recive,nlen,remoteHost,&remotePort);

	if(nResult < 0)
	{
		printf("error recive\n");
		return 4;
	}

	printf("recive sucess count=[%d] grecive[%s]\n",nResult,g_recive);
 printf("remoteHost [%s] remoteport[%d] \n",remoteHost,remotePort);

	return 0;
}



long getCount()
{
	//return g_aUser.getCount();
	return strlen(g_recive);
}

#if 1

extern "C" {
    JNIEXPORT jint JNICALL Java_test_jnitest_JniTestA_init(JNIEnv *, jobject);
    JNIEXPORT jint JNICALL Java_test_jnitest_JniTestA_get(JNIEnv *, jobject);
};

JNIEXPORT jint JNICALL Java_test_jnitest_JniTestA_init
  (JNIEnv *env, jobject obj)
  {
  	//int ret = initThread();
  	//int ret = initTimer();
  	//int ret = initNetWorktcp();
  	//int ret = initNetWorkudpCli();
	int ret = initNetWorkrtpCli();
  	return ret;
  }

JNIEXPORT jint JNICALL Java_test_jnitest_JniTestA_get
 (JNIEnv *env, jobject obj)
  {
  	int count = getCount();
  	return count;
  }

#else
int main()
{
	//int ret = initThread();
  	//int ret = initNetWorktcp();
  	//int ret = initNetWorkudpCli();
  	//int ret = initNetWorkudpServ();
	int ret = initNetWorkrtpCli();
	//int ret = initNetWorkrtpServ();
	unsigned long rcount = 0;
	printf("ret  = [%d]\n",ret );
	while(1)
	{
		rcount = getCount();
		printf("count  = [%d]\n",rcount );
		sleep(5);
	}
	return ret;
}
#endif
