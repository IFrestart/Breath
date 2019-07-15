// FCNetACLib.cpp : 定义 DLL 应用程序的入口点。
//
#ifndef __LINUX__
#include "stdafx.h"
#endif

#include "FCNetACLib.h"
#include "NetACClient.h"


#ifndef __LINUX__
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

NetACClient*   g_NetACClient = NULL;

#ifdef __LINUX__
pthread_t	g_hNetCtrlDllThread = 0;
#else
HANDLE		g_hNetCtrlDllThread = NULL ;
#endif
unsigned long	g_dwThreadId = 0;


#ifdef __LINUX__
void* NetCtrlDllWorkThread(void* param)
#else
DWORD WINAPI NetCtrlDllWorkThread(void* param)
#endif
{
	char *argv[10];
	char szModuleName[280];
	memset(szModuleName, 0, sizeof(szModuleName));

#ifdef __LINUX__
	usleep(10000);
	strcpy(szModuleName,"./NetACLib.xml");
#else
	Sleep(1000);

	char* pTmp = NULL;
	::GetModuleFileName(NULL, szModuleName, 280);
	pTmp = strrchr(szModuleName, '\\');
	if (pTmp != NULL)
	{
		sprintf(pTmp,"\\%s","NetACLib.xml");
		//*pTmp = '\0';
		
		
	}
	else
	{
		pTmp = strrchr(szModuleName, '/');
		if (pTmp != NULL)
		{
			sprintf(pTmp,"/%s","NetACLib.xml");
//			*pTmp = '\0';
		}
	}		
#endif
	argv[0] = (char *)szModuleName;
	argv[1] = NULL;
	int nRet = 0;
	if (NULL == g_NetACClient)
	{
		std::cout << "error " << __FILE__ << "::" << __LINE__ << std::endl;
		return 0;
	}
#ifdef USE_ACE_VER
	nRet = g_NetACClient->doMain(1,argv);
#endif
	return 0;
}

//------------------------------------------
//网络初始化
//@return 0成功 <0失败
//------------------------------------------

FCNETAC_API int __stdcall FCNetACInit()
{
	g_NetACClient = new NetACClient();
	if (NULL == g_NetACClient)
	{
		std::cout << "new NetACClient error " << __FILE__ << "::" << __LINE__ << std::endl;
		return -1;
	}
#ifdef USE_ACE_VER

#ifdef __LINUX__
	if(0  != pthread_create(&g_hNetCtrlDllThread, NULL, NetCtrlDllWorkThread, NULL))
	{
		std::cout << "pthread_create error " << __FILE__ << "::" << __LINE__ << std::endl;
		return -2;
	}
	g_dwThreadId = g_hNetCtrlDllThread;
#else
	g_hNetCtrlDllThread = CreateThread(NULL, 0, NetCtrlDllWorkThread,NULL, 0, &g_dwThreadId);
	if (NULL == g_hNetCtrlDllThread)
	{
		std::cout << "CreateThread error " << __FILE__ << "::" << __LINE__ << std::endl;
		return -2;
	}
	std ::cout << "have sucess createthread g_dwThreadId=" << g_dwThreadId << std::endl;
#endif


	int nRet = g_NetACClient->waitInit();
#else
	int nRet = g_NetACClient->initialize(1,NULL);
#endif
	if (0 != nRet)
	{
		std::cout << "error g_NetACClient->waitInit " << __FILE__ << "::" << __LINE__ << std::endl;
		delete g_NetACClient;
		g_NetACClient = NULL;
	}
	return nRet;
}

//------------------------------------------
//网络反初始化
//@return 0成功 <0失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACUninit()
{
	if (NULL == g_NetACClient) 
	{
		return -1;
	}
	delete g_NetACClient;
	g_NetACClient = NULL;
	return 0;

}

//------------------------------------------
//系统登入
//@note:在FCNetACInit成功之后才能调用
//@param user_id  用户登陆的账户
//@param user_flag 用户上层标识
//@param user_data 用户自定义数据
//@param user_data_len
//@param OutTime 等待时间，默认为无限等待（建议采用默认值）
//@return 0成功 <0失败
//------------------------------------------

FCNETAC_API int __stdcall FCNetACLogin(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime )
{
	if (NULL == g_NetACClient)
	{
		return -1;
	}
	return 	g_NetACClient->LogIn(fcNetAcInfo,OutTime);
}

//------------------------------------------
//系统退出
//@note:在FCNetACInit成功之后才能调用
//@return 0成功 <0失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACLogout()
{
	if (NULL == g_NetACClient) 
	{
		return -1;
	}
	return g_NetACClient->waitLogOut();
	//return 	g_NetACClient->LogOut();
//#ifdef __LINUX__
//	return pthread_kill(g_hNetCtrlDllThread,SIGUSR1);
//#else
//	std::cout << "bbbbbb PostThreadMessage " << std::endl;
//	PostThreadMessage(g_dwThreadId,NetAC_Msg_LogOut,0,0);
//	std::cout << "aaaaa PostThreadMessage " << std::endl;
//	return 0;		 
//#endif
}



//------------------------------------------
//发往数据库的数据
//@note:在FCNetACLogin成功之后才能调用
//in @param pdu     发送的数据标识
//in @param in_data 发送的数据信息
//in @param in_Len   发送数据的长度
//in @param retPdu   接收到的数据标识
//out @param out_data 接收到的数据信息
//out @param out_Len   接收到的数据的长度
//@return 0成功，<0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,unsigned short& retPdu,const char*& out_data,unsigned long& out_Len)
{
	if (NULL == g_NetACClient) 
	{
		return -1;
	}
	return g_NetACClient->SendDataToDB(pdu,in_data,in_Len,retPdu,out_data,out_Len);

}

//------------------------------------------
//发往服务器的数据
//@note:在FCNetACLogin成功之后才能调用
//in @param pdu     发送的数据标识
//in @param data 发送的数据信息
//in @param len   发送数据的长度
//@return 0成功，<0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToServ(unsigned short pdu, const char *data, unsigned long len)
{	
	if (NULL == g_NetACClient) 
	{
		return -1;
	}
	return g_NetACClient->SendDataToServ(pdu,data,len);
}
