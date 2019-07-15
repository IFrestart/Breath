//#pragma once
#ifndef __LINUXDEF_H__
#define __LINUXDEF_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "WinSock2.h"
#else

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include    <sys/un.h>
#include    <unistd.h>
#include <pthread.h>
#include <time.h>

#include <sys/ipc.h>
//#include <sys/msg.h> //modify 2012-8-24  zzzz
#include <sys/time.h>

#include <bitset>
#include <list>

#endif


#ifdef _WIN32
#define SOCKS5_ERRNO() WSAGetLastError()
#define SOCKS5_CLOSE closesocket
#define SOCKS5_SOCKET SOCKET
#define SOCKS5_ERROR SOCKET_ERROR
#define INVALID_SOCKS5 INVALID_SOCKET


#else /* !_WIN32 */
#define SOCKS5_ERRNO() (errno)
#define SOCKS5_CLOSE close
#define SOCKS5_SOCKET int
#define SOCKS5_ERROR -1
#define INVALID_SOCKS5 -1
#define ACE_HANDLE int

#define _INFO printf
#define _ERROR printf
#define ACE_ASSERT assert


#define TRUE 1
#define FALSE 0
#define ERROR_SUCCESS                    0L
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define WSAGetLastError() (errno)
#define GetLastError() (errno)
#define closesocket	close
#define MAX_PATH          260

#define CRITICAL_SECTION pthread_mutex_t
#define InitializeCriticalSection(m) pthread_mutex_init(m,NULL)
#define DeleteCriticalSection pthread_mutex_destroy
#define EnterCriticalSection pthread_mutex_lock
#define LeaveCriticalSection pthread_mutex_unlock
#define timeGetTime() time(NULL)

#define HANDLEFOREVENT 1
#define HANDLEFORSEM   2 
#define ATLASSERT  assert
#define _tcslen  strlen

#ifndef OUT
#define OUT
#endif




#define TIME_CALLBACK_FUNCTION      0x0000  /* callback is function */
#define TIME_PERIODIC   0x0001   /* program for continuous periodic event */


typedef int BOOL;
typedef unsigned char BYTE;
typedef BYTE* LPBYTE;
typedef unsigned short WORD;
typedef WORD* LPWORD;
typedef unsigned long DWORD;
typedef DWORD* LPDWORD;

typedef const char* LPCSTR;
typedef const char* LPCTSTR;
typedef char* LPSTR;
typedef char* LPTSTR;

typedef void* LPVOID;
typedef char TCHAR;
typedef unsigned int UINT;
typedef unsigned int UINT_PTR;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef long  LONG;

typedef void* WINAPI;

typedef unsigned int WPARAM;
typedef long  	LPARAM;
typedef long  	LRESULT;
typedef unsigned long  	DWORD_PTR;

typedef void *HINSTANCE;


//typedef void (*pfun)(unsigned long,void*) LPTIMECALLBACK;
//begin add for MultipleSignal
class MultipleObjects
{
public:
	MultipleObjects();
	~MultipleObjects();
	void reset();
	bool set(size_t index);
	bool reset(size_t index);
	void signal();
	long WaitForMultipleObjects(DWORD nCount, BOOL bWaitAll,DWORD dwMilliseconds);
private:
	pthread_cond_t          m_rcond_;
        pthread_mutex_t         m_mutext_;	
	std::bitset<16> 	m_bit;
}; 

typedef class NotifyMultipleObjects
{
public:
	NotifyMultipleObjects();
	~NotifyMultipleObjects();
	NotifyMultipleObjects(const NotifyMultipleObjects& other);
	NotifyMultipleObjects& operator= (const NotifyMultipleObjects& other);
        bool operator== ( const NotifyMultipleObjects& other ) const;
	void set();
	void reset();
	void signal();

	void setMultipleObjects(MultipleObjects* pUnit);
	void setId(long id);
	void setIndex(size_t index);
private:	
	MultipleObjects* m_pUnit;
	size_t 	m_opIndex;  
	long 	m_unitId;  
}NOTIFYMULTIPLEOBJECTS;
//end add

typedef std::list<NOTIFYMULTIPLEOBJECTS> NotifyMultipleObjectsList;
typedef std::list<NOTIFYMULTIPLEOBJECTS>::iterator IT_NotifyMultipleObjectsList;

class HandleForWin 
{
public:
	HandleForWin(void* attr,BOOL mflag, BOOL init, WORD type,void* name,long max=10);
	~HandleForWin();
	BOOL SetEvent();  //for event
	BOOL ResetEvent(); //for event
	BOOL Release(long releaseCount , long* resultCount); //for sem
	long WaitForSingleObject(long microseconds);

public:
	//begin add for MultipleSignal

 	BOOL addNotifyUnit(NOTIFYMULTIPLEOBJECTS& punit);
	BOOL delNotifyUnit(long unitId);
	/*
	 @param opFlag == 1  is SetEvent ; opFlag == 0  is ResetEvent
	*/
	BOOL eventNotifyMuilt(int opFlag);
	void dealHaveNotifyMuilt();
	//end add
	
private:
	pthread_cond_t          m_rcond_;
        pthread_mutex_t         m_mutext_;	
	WORD			m_type; //1 for event  ; 2 for sem
	BOOL                   	m_flag; 
	long                   	m_sig;  
	long                   	m_sigmax;  

		//begin add for MultipleSignal
        pthread_mutex_t         m_mutextList_;	
	NotifyMultipleObjectsList	m_NotifyList;
		//end add
}; 

typedef HandleForWin* HANDLE ;


class MessageHandle
{
public:
        MessageHandle();
        ~MessageHandle();
	bool CreateMsg(long id);
	bool RmMsg();
	bool LockReq(); 
	bool unLockReq();
	bool SignalReq(unsigned int type,bool sendflag = FALSE); //call by produce ; if call by  sendmessage flag is FALSE else by postmessage call is TRUE
	//int WaitReq(); //call by consume  //modified by zjj 2010-1-27
	int WaitReq(long microseconds = 0); //call by consume
	bool SignalRsp(unsigned int type);//call by consume
	int WaitRsp(unsigned int type,long microseconds = 0); //call by produce
	int getMsgQid(){return m_msqid;}
private:
        pthread_cond_t          m_ReqCond_;
        pthread_cond_t          m_RspCond_;
        pthread_mutex_t         m_ReqMutext_;
        pthread_mutex_t         m_RspMutext_;
        pthread_mutex_t         m_CountMutext_;
	int m_msqid;
	unsigned int m_ReqType;
	unsigned int m_ReqCount;
	unsigned int m_RspType;
};

typedef MessageHandle* HWND;

typedef struct My_MsgBuffer
{
	long	 _mtype;
	WPARAM	 _wparam;
	LPARAM   _lparam;
}MessageBuffer;

#endif /* !_WIN32 */


#define SOCKS5_SERVER                      1
#define SOCKS5_CLIENT                      2

#endif

