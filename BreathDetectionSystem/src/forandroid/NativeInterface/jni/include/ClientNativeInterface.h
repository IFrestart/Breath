#ifndef __ClientNativeInterface_H__
#define __ClientNativeInterface_H__

#ifndef __LINUX__
#include <windows.h>
#endif
#include "FCNetACLib.h"

#include <iostream>
//#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include "Lock.h"
#include "SockEventCallBack.h"
#include "TimerEventCallBack.h"
#include "TCPConnector.h"

const short OPERATE_REQ_TOPAD     = 1000;
const short OPERATE_RSP_FromPAD   = 1001;
const short Max_ShareBuf_LEN      = 96000;
//const short Max_ShareBuf_LEN      = 9600;

//void __stdcall  myCallBack(int msgType, const char *user_id,const void* data, unsigned long dataLen,
//				 unsigned short pdu,int roster_flag, unsigned long user_flag);
void __stdcall  myCallBack(int msgType, const char *user_id,unsigned short gid,unsigned short DataType,const void* data, 
						   unsigned long dataLen,unsigned short pdu,int roster_flag, unsigned long user_flag);



struct stTransUnit
{
	stTransUnit()
	{
		data = NULL;
		bufferid = -1;
		_datalen = 0;
		_pdu = 0;
		_roster_flag = 0;	
		memset(_userid,0,sizeof(_userid));
	}
	~stTransUnit()
	{
		data = NULL;
		bufferid = -1;
	}

	unsigned short _msgType;
	unsigned short _gid ;
	unsigned short _dataType;
	unsigned short _pdu;
	char 		_userid[129];
	unsigned long	_roster_flag;
	int		bufferid;
	char   *data ;	
	unsigned long _datalen;
} ;

struct stTransBuffer
{
	char   *data ;	
	unsigned long   maxlen;
};

class stBufferManage
{
public:
	stBufferManage()
	{
		m_BufNum = 0;
		m_transUintList.clear();
		m_BuffreeList.clear();
		m_vSendBuf.clear();
		pthread_cond_init(&m_rcond_,NULL);
		pthread_mutex_init(&mutexTransList_,NULL);
	}
	~stBufferManage()
	{
		for(int i = 0; i < m_BufNum; i++)
		{
			delete[] m_vSendBuf[i].data;
			m_vSendBuf[i].data = NULL;
		}
		pthread_cond_destroy(&m_rcond_);
		pthread_mutex_destroy(&mutexTransList_);
	}

	int pushData(stTransUnit&,const void* data, unsigned long len);
	//int getData(stTransUnit&);
	int getData();

	int stopWork(); //add 2014-7-24

	int createBuffer(int buffernum)
	{
		m_BufNum = buffernum;
		m_vSendBuf.resize(m_BufNum);
		for(int i = 0 ; i < m_BufNum; i++)
		{
			m_vSendBuf[i].data = new char[DEFAULT_BUFFER_LEN];
			if(NULL == m_vSendBuf[i].data)
			{
				printf("createBuffer error and exit\n");
				exit(0);
			}
			m_vSendBuf[i].maxlen = DEFAULT_BUFFER_LEN;
			m_BuffreeList.push_back(i);
		}
		return 0;
	}
public:
	std::list<stTransUnit> m_transUintList;
	std::list<int> m_BuffreeList; 
	pthread_cond_t 		m_rcond_;
	pthread_mutex_t		mutexTransList_;
	CLock         		mutexBuffreeList_;
	std::vector<stTransBuffer>  m_vSendBuf ;
	int	 		m_BufNum;
private:
	//enum {DEFAULT_BUFFER_LEN = 9216};
	enum {DEFAULT_BUFFER_LEN = 3600};
};
	
#endif

