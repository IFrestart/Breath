#ifndef __NETCTRLDLL_H__
#define __NETCTRLDLL_H__

#ifdef __LINUX__

class IBaseCommCall;
//#include "NetCtrlInt.h"

//extern "C" __declspec(dllexport) IBaseCommCall*  CreateNetCtrlObj(void);
//extern "C" __declspec(dllexport) void  DestroyNetCtrlObj(IBaseCommCall* pMyCommCall);

extern "C" IBaseCommCall*  CreateNetCtrlObj(void);
extern "C" void  DestroyNetCtrlObj(IBaseCommCall* pMyCommCall);

#else

class IBaseCommCall;
typedef IBaseCommCall * (WINAPI *PFN_NETCTRLOBJ_CREATE)(void);
typedef void (WINAPI *PFN_NETCTRLOBJ_DESTROY)(IBaseCommCall *pMyCommCall);

#endif

#endif
