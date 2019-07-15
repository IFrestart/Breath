/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	 SendConfig.h
 * @author	 zhoujj <zhoujj@webcon.com.cn>
 * @date    	 04/2004
 * @short 
 **/

#ifndef	__SENDCONFIG_H__
#define	__SENDCONFIG_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "SockStream.h"
#include "tinyxml.h"
#include "Tools.h"
#include <list>

//#ifndef PRINT_DEBUG
//#define PRINT_DEBUG
//#endif

typedef unsigned char BYTE;


#define MAX_BUF_LEN 1024*200
#define MAX_INSERT_BUF 1024*100

using namespace std;
typedef struct headnode
{
	unsigned long ip;
	unsigned short port;
	unsigned short packetnum;
}HEAD_NODE;

typedef struct rspnode
{
	unsigned short rsppdutype;
	unsigned short checktype; /** < 1:offset ; 2:len */
/**
 offset
 */
	unsigned long offset;
	unsigned short type ; /** < 1:short ; 2:long */
	unsigned long checkvalue;

/**
 len
 */
	unsigned long len;

}RSP_NODE;

struct PduNode
{
//public:
	unsigned short no;
	unsigned short pdutype;
	string	  	   pduname;
	unsigned short isrsp;/** < need or not return; 1:return; 0:not return*/
	unsigned short loops;


	void *data;
	unsigned long datalen;
	RSP_NODE rspdata;
	PduNode()
	{
		no = 0;
		pdutype = 0;
		pduname.clear();
		isrsp = 0;
		loops = 1;
		datalen = 0;
		memset(&rspdata,0,sizeof(RSP_NODE));
		data = NULL;
	}
	~PduNode()
	{
		if(data != NULL)
		{
			delete[] (char *)data;
			data = NULL;
		}
	}
	void DeleteData()
	{
		if(data != NULL)
		{
			delete[] (char *)data;
			data = NULL;
		}
	}
	void clear()
	{
		no = 0;
		pdutype = 0;
		pduname.clear();
		isrsp = 0;
		loops = 1;
		datalen = 0;
		memset(&rspdata,0,sizeof(RSP_NODE));
		DeleteData();
	}
	PduNode(const PduNode& other)
	{
		no = other.no;
		pdutype = other.pdutype;
		pduname = other.pduname;
		isrsp = other.isrsp;
		loops = other.loops;
		datalen = other.datalen;
		memcpy(&rspdata,&other.rspdata,sizeof(RSP_NODE));
		data = new char[datalen];
		memcpy(data, other.data, datalen);
	}
		
	PduNode& operator= (const PduNode& other)
	{
		if(this != &other)
		{
			no = other.no;
			pdutype = other.pdutype;
			pduname = other.pduname;
			isrsp = other.isrsp;
			loops = other.loops;
			datalen = other.datalen;
			memcpy(&rspdata,&other.rspdata,sizeof(RSP_NODE));
			if(data != NULL)
			{
				delete[] (char *)data;
			}
			data = new char[datalen];
			memcpy(data, other.data, datalen);
		}
		return(*this);
	}

	bool operator== ( const PduNode& other ) const
	{
		return((no == other.no)	|| (pdutype == other.pdutype));
	}
};
typedef PduNode PDU_NODE;

class CSendConfig 
{
public:
	CSendConfig( );
	~CSendConfig( );

	int open( const char* filename ) ;
	int getServerAddr( unsigned long& ip, unsigned short& port);
	int getPacketNum( unsigned short& packetnum);
	int getPacket( unsigned short no, void*& data, unsigned long& len, unsigned short& pdutype ); 
	int getPacket( unsigned short pdutype, void*& data, unsigned long& len);
	int getLoopsByNo( unsigned short no ,unsigned short& loop);
	int getLoopsByPdutype( unsigned short pdutype ,unsigned short& loop);

	int getIsrspByNo( unsigned short no ,bool& res,unsigned short& rsppdutype);
	int getIsrspByPdutype( unsigned short pdutype ,bool& res,unsigned short& rsppdutype);
	int checkRspPdu( unsigned short pdutype, unsigned short rsppdutype, const void* data, unsigned long len ,bool& res) ;
	int close() ;
	int listPacket();

protected:
	int GetHeadNode(TiXmlDocument *pXMLDoc);
	int GetPduList(TiXmlDocument *pXMLDoc);
	int GetPduData(PDU_NODE *m_pPduNode,TiXmlElement* element);
	int BuildPdu(CSockStream& ar,const char *KeyName,const char *KeyValue);
	int BuildRspCheckNode(PDU_NODE *m_pPduNode,TiXmlElement* element);
	long myatol(const char *buf,short& errCode);

private:
	typedef std::list<PduNode>::iterator   PduNode_it;

	std::list<PduNode> m_PduList;

	HEAD_NODE m_HeadNode;
};

#endif	/*__SENDCONFIG_H__*/

