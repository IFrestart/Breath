#ifndef __WAITFOROBJECT_H__
#define __WAITFOROBJECT_H__

#include "LinuxDef.h"

#define INFINITE            0x0FFFFFFF  // Infinite timeout
#define STATUS_WAIT_0	((DWORD   )0x00000000L)    
#define WAIT_OBJECT_0	((STATUS_WAIT_0 ) + 0 )
#define WAIT_TIMEOUT                     258L    // dderror
#define WAIT_FAILED ((DWORD)0xFFFFFFFF)

HANDLE CreateEvent(void* attr,BOOL mflag, BOOL init, void* name);
HANDLE CreateSemaphore(void* attr,long initCount, long maxCount, void* name);

BOOL SetEvent(HANDLE h);
BOOL ResetEvent(HANDLE h);
BOOL ReleaseSemaphore(HANDLE h, long releaseCount, long* resultCount);

BOOL CloseHandle(HANDLE h);

long WaitForSingleObject(HANDLE event,long microseconds);
long WaitForMultipleObjects(DWORD nCount, HANDLE* lpHandles, BOOL bWaitAll,DWORD dwMilliseconds);

#endif
