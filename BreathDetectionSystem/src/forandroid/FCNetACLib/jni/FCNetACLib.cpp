// FCNetACLib.cpp : ���� DLL Ӧ�ó������ڵ㡣
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
//�����ʼ��
//@return 0�ɹ� <0ʧ��
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
//���練��ʼ��
//@return 0�ɹ� <0ʧ��
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
//ϵͳ����
//@note:��FCNetACInit�ɹ�֮����ܵ���
//@param user_id  �û���½���˻�
//@param user_flag �û��ϲ��ʶ
//@param user_data �û��Զ�������
//@param user_data_len
//@param OutTime �ȴ�ʱ�䣬Ĭ��Ϊ���޵ȴ����������Ĭ��ֵ��
//@return 0�ɹ� <0ʧ��
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
//ϵͳ�˳�
//@note:��FCNetACInit�ɹ�֮����ܵ���
//@return 0�ɹ� <0ʧ��
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
//�������ݿ������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param pdu     ���͵����ݱ�ʶ
//in @param in_data ���͵�������Ϣ
//in @param in_Len   �������ݵĳ���
//in @param retPdu   ���յ������ݱ�ʶ
//out @param out_data ���յ���������Ϣ
//out @param out_Len   ���յ������ݵĳ���
//@return 0�ɹ���<0 ʧ��
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
//����������������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param pdu     ���͵����ݱ�ʶ
//in @param data ���͵�������Ϣ
//in @param len   �������ݵĳ���
//@return 0�ɹ���<0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToServ(unsigned short pdu, const char *data, unsigned long len)
{	
	if (NULL == g_NetACClient) 
	{
		return -1;
	}
	return g_NetACClient->SendDataToServ(pdu,data,len);
}
