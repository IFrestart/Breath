#include <assert.h>

#include "NetCtrlCall.h"
#include "NetCtrlDll.h"

//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "User32.lib")
//#pragma comment(lib, "Winmm.lib")

#ifdef __LINUX__

IBaseCommCall * CreateNetCtrlObj(void)
{
	return new CNetCtrlCall();
}

void  DestroyNetCtrlObj(IBaseCommCall* pMyCommCall)
{
	if (pMyCommCall)		//add 2011-7-8
	{
		delete (CNetCtrlCall*)pMyCommCall;
	}
	
	pMyCommCall = NULL;
}

#else
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	return TRUE;
}

IBaseCommCall * WINAPI CreateNetCtrlObj(void)
{
	return new CNetCtrlCall();
}

void WINAPI DestroyNetCtrlObj(IBaseCommCall* pMyCommCall)
{
	if (pMyCommCall)		//add 2011-7-8
	{
		delete (CNetCtrlCall*)pMyCommCall;
	}
	pMyCommCall = NULL;
}
#endif
