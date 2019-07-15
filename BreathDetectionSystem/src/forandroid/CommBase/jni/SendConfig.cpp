/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	SendConfig.cpp
 * @author	zhoujj <zhoujj@webcon.com.cn>
 * @date     	04/2004
 * @short 
 **/

#include "SendConfig.h"


CSendConfig::CSendConfig( )
{
	memset(&m_HeadNode,0,sizeof(m_HeadNode));
}

CSendConfig::~CSendConfig( )
{
}

int CSendConfig::open( const char* filename ) 
{ 
	bool loadOkay = false;
	TiXmlDocument *pXMLDoc = NULL;
	pXMLDoc = new TiXmlDocument(filename);
	loadOkay = pXMLDoc->LoadFile();
	if(!loadOkay)
	{
		//printf( "Could not load test file %s. Error='%s'. Exiting.\n", file_name,pXMLDoc->ErrorDesc() );
		delete pXMLDoc;
		pXMLDoc = NULL;
		return(-1);
	}
	if(GetHeadNode(pXMLDoc) < 0)
	{
		if(pXMLDoc)
		{
			pXMLDoc->Clear();
			delete pXMLDoc;
			pXMLDoc = NULL;
		}
		return(-1);
	}
	if(GetPduList(pXMLDoc) < 0)
	{
		printf("GetPduList error when open %s\n",filename);
		close( ) ;
	}
    if(pXMLDoc)
    {
       	pXMLDoc->Clear();
       	delete pXMLDoc;
       	pXMLDoc = NULL;
	}
	return(0);
}

int CSendConfig::GetHeadNode(TiXmlDocument *pXMLDoc)
{
	TiXmlNode* node = 0;
	TiXmlElement* element = 0;
	string  strRet; 
	long nValue = 0;
	short errCode = 0;

	node = pXMLDoc->RootElement();

	if((node == 0 ) || (node->ToElement() == 0))
	{
		return(-1);
	}
	element = node->ToElement();
	element = element->FirstChildElement();
	if(strcmp(element->Value(),"Host") != 0)
	{
		return(-1);
	}

	strRet = element->Attribute("Ip");
	if(strRet.c_str() != NULL)
	{
		m_HeadNode.ip = ip_to_long(strRet.c_str());
	}
	strRet = element->Attribute("Port");
	if(strRet.c_str() != NULL)
	{
	
		nValue = myatol(strRet.c_str(),errCode);
		if(errCode < 0)
		{
			return(-1);
		}
		m_HeadNode.port = (unsigned short)(nValue & 0xffff);
	}
	return(0);
}

int CSendConfig::GetPduList(TiXmlDocument *pXMLDoc)
{
	PDU_NODE m_pPduNode;
	TiXmlNode* node = 0;
	TiXmlElement* element = 0;
	unsigned short count = 0;
	node = pXMLDoc->RootElement();

	if((node == 0 ) || (node->ToElement() == 0))
	{
		   return(-1); 
	}
	element = node->ToElement();
	element = element->FirstChildElement(); 

    for( element = element->FirstChildElement(); element ; element = element->NextSiblingElement() )
	{
		if(strcmp(element->Value() ,"pdu") != 0)
		{
			return(-1);
		}
		m_pPduNode.no = count++;
		if(GetPduData(&m_pPduNode,element) < 0)
		{
			printf("format pdu [%s]\n",m_pPduNode.pduname.c_str());
			m_pPduNode.clear();
			return(-1);
		}
		m_PduList.push_back(m_pPduNode);
		m_pPduNode.clear();
	}
	m_HeadNode.packetnum = m_PduList.size();
	return(0);
}
int CSendConfig::GetPduData(PDU_NODE *m_pPduNode,TiXmlElement* element) 
{
	long nValue = 0;
	int pdulen = 0,retlen = 0;
	string fieldname ;
	string fieldvalue ;
	TiXmlAttribute *pfield = NULL;
	short errCode = 0;

	for(pfield = element->FirstAttribute(); pfield ; pfield = pfield->Next())
	{
		fieldname = pfield->Name();
		fieldvalue = pfield->Value();
		if(fieldname == "PduType")
		{
			nValue = myatol(fieldvalue.c_str(),errCode);
			if(errCode < 0)
			{
				return(-1);
			}
			m_pPduNode->pdutype =(unsigned short)(nValue & 0xffff);
			continue;
		}
		if(fieldname == "PduName")
		{
			m_pPduNode->pduname = fieldvalue;
			continue;
		}
		if(fieldname == "Loops")
		{
			nValue = myatol(fieldvalue.c_str(),errCode);
			if(errCode < 0)
			{
				return(-1);
			}
			m_pPduNode->loops =(unsigned short)(nValue & 0xffff);
			continue;
		}
		if(fieldname == "IsRsp")
		{
			if((fieldvalue == "true") || (fieldvalue == "True") || (fieldvalue == "TRUE"))
			{
				m_pPduNode->isrsp = 1;
			}
			continue;
		}
		if(fieldname == "RspPduType")
		{
			nValue = myatol(fieldvalue.c_str(),errCode);
			if(errCode < 0)
			{
					return(-1);
			}
			m_pPduNode->rspdata.rsppdutype = (unsigned short)(nValue & 0xffff);
			continue;
		}
		if(fieldname == "CheckType")
		{
			if((fieldvalue == "offset") || (fieldvalue == "Offset") || (fieldvalue == "OFFSET"))
			{
				m_pPduNode->rspdata.checktype = 1;
			}
			else if((fieldvalue == "len") || (fieldvalue == "Len") || (fieldvalue == "LEN"))
			{
				m_pPduNode->rspdata.checktype = 2;
			}
			else
				return (-1);
		}
	}
	if(m_pPduNode->isrsp == 1)
	{
		if(m_pPduNode->rspdata.rsppdutype == 0)
		{
			printf("sorry!!! format of pdu = [%s]'s rsppdutype is error\n",m_pPduNode->pduname.c_str());
			return (-1);
		}
		if(m_pPduNode->rspdata.checktype != 0)
		{
			if(BuildRspCheckNode(m_pPduNode,element) < 0)
			{
				printf("sorry!!! format of pdu = [%s] is error\n",m_pPduNode->pduname.c_str());
				return(-1);
			}
		}	
	}

	CSockStream ar(MAX_BUF_LEN);
	TiXmlElement* element_pdu = NULL;
	string KeyName,KeyValue;
    for( element_pdu = element->FirstChildElement(); element_pdu ; element_pdu = element_pdu->NextSiblingElement() )
	{
		KeyName = element_pdu->Attribute("DataType");
		if(KeyName.c_str() == NULL)
		{
			return (-1);
		}
		KeyValue = element_pdu->Attribute("Data");
		if(KeyValue.c_str() == NULL)
		{
			return (-1);
		}
		retlen = BuildPdu(ar,KeyName.c_str(),KeyValue.c_str());
		if(retlen < 0)
		{
			return(-1);
		}
		pdulen += retlen;
		if(pdulen >= MAX_BUF_LEN - MAX_INSERT_BUF)
		{
			printf("bufsize not enough ,need remalloc buf size \n");
		}
	}
    
	m_pPduNode->datalen =  ar.tell();
	m_pPduNode->data = new char[m_pPduNode->datalen];
	if(m_pPduNode->data == NULL)
	{
		return(-1);
	}
	memcpy(m_pPduNode->data,ar.getData(),m_pPduNode->datalen);
   	return(0);
}

int CSendConfig::BuildRspCheckNode(PDU_NODE *m_pPduNode,TiXmlElement* element)
{
	string strRet;
	long nValue = 0;
	short errCode = 0;

	if(m_pPduNode->rspdata.checktype == 1)
	{
		strRet = element->Attribute("Offset");
		if(strRet.c_str() != NULL)
		{
			nValue = myatol(strRet.c_str(),errCode);
			if(errCode < 0)
			{
				return(-1);
			}
			m_pPduNode->rspdata.offset = nValue;
		}
		else
		{
			return (-1);
		}
		strRet = element->Attribute("Type");
		if(strRet.c_str() != NULL)
		{
			if((strncmp(strRet.c_str(),"short",5) == 0) || (strncmp(strRet.c_str(),"Short",5) == 0) || (strncmp(strRet.c_str(),"SHORT",5) == 0))
			{
				m_pPduNode->rspdata.type = 1;
			}
			else if((strncmp(strRet.c_str(),"long",4) == 0) || (strncmp(strRet.c_str(),"Long",4) == 0) || (strncmp(strRet.c_str(),"LONG",4) == 0))
			{
				m_pPduNode->rspdata.type = 2;
			}
			else
			{
				return(-1);
			}
		}
		else
		{
			return (-1);
		}
		strRet = element->Attribute("CheckValue");
		if(strRet.c_str() != NULL)
		{
			nValue = myatol(strRet.c_str(),errCode);
			if(errCode < 0)
			{
				return(-1);
			}
			m_pPduNode->rspdata.checkvalue = nValue;
		}
		else
		{
			return (-1);
		}
	}
	if(m_pPduNode->rspdata.checktype == 2)
	{
		strRet = element->Attribute("Len");
		if(strRet.c_str() != NULL)
		{
			nValue = myatol(strRet.c_str(),errCode);
			if(errCode < 0)
			{
				return(-1);
			}
			m_pPduNode->rspdata.len = nValue;
		}
		else
		{
			return (-1);
		}
	}
	return (0);
}

int	CSendConfig::getServerAddr( unsigned long& ip, unsigned short& port) 
{
	ip = m_HeadNode.ip;
	port = m_HeadNode.port;
	return 0;
}

int CSendConfig::getPacketNum( unsigned short& packetnum)
{
	packetnum = m_HeadNode.packetnum;
	return 0;
}

int CSendConfig::BuildPdu(CSockStream& ar,const char *KeyName,const char *KeyValue)
{
	long nValue = 0;
	unsigned short shTemp = 0;
	long l_temp1 = 0,l_temp2 = 0;
	char *p = NULL;
	short errCode = 0;

								/** < x | x~y */
	if((strncmp(KeyName,"byte",4) == 0) || (strncmp(KeyName,"Byte",4) == 0) || (strncmp(KeyName,"BYTE",4) == 0) )
	{
		BYTE chTemp = 0;
		p = strchr(KeyValue,'~') ;
		if(p != NULL)
		{
			sscanf(KeyValue,"%ld~%ld",&l_temp1,&l_temp2);
			chTemp = (BYTE)((rand() % (l_temp2 - l_temp1)) + l_temp1) & 0xff;
		}
		else
		{
			nValue = myatol(KeyValue,errCode);
			if(errCode < 0)
				return (-1);
			chTemp = (BYTE)(nValue & 0xff);
		}
		ar<<chTemp;
		return(sizeof(BYTE));
 	}
			/** <	xx | xx~yy */
	else if((strncmp(KeyName,"short",5) == 0) || (strncmp(KeyName,"Short",5) == 0) || (strncmp(KeyName,"SHORT",5) == 0))
	{
		p = strchr(KeyValue,'~') ;
		if(p != NULL)
		{
			sscanf(KeyValue,"%ld~%ld",&l_temp1,&l_temp2);
			shTemp = (short)((rand() % (l_temp2 - l_temp1)) + l_temp1) & 0xffff;
		}
		else
		{
			nValue = myatol(KeyValue,errCode);
			if(errCode < 0)
				return (-1);
			 shTemp = (short)(nValue & 0xffff);
		}
		ar<<shTemp;
		return(sizeof(short));
	}
		/** < xxxx | xxxx~yyyy */
	else if((strncmp(KeyName,"long",4) == 0) || (strncmp(KeyName,"Long",4) == 0)|| (strncmp(KeyName,"LONG",4) == 0))
	{
		p = strchr(KeyValue,'~') ;
		if(p != NULL)
		{
			sscanf(KeyValue,"%ld~%ld",&l_temp1,&l_temp2);
			nValue = (rand() % (l_temp2 - l_temp1)) + l_temp1 ;
		}
		else
		{
			nValue = myatol(KeyValue,errCode);
			if(errCode < 0)
				return (-1);
		}
		ar<<nValue;
		return(sizeof(long));
	}
			/** < 	cccccccccc | @xx */
	else if((strncmp(KeyName,"string",6) == 0) || (strncmp(KeyName,"String",6) == 0) || (strncmp(KeyName,"STRING",6) == 0))
	{
		if(KeyValue[0] == '@')
		{
			nValue = myatol(KeyValue + 1,errCode);
			if(errCode < 0)
				return (-1);
			if(nValue > MAX_INSERT_BUF)
			{
				perror("error in BuildPdu:: string nValue > MAX_INSERT_BUF \n");
				return(-1);
			}
			p = new char[nValue];
			if(p == NULL)
			{
				perror("new error in BuildPdu:: string\n");
				return(-1);
			}
			ar.writeBytes(p,nValue);
			delete[] p;
			p = NULL;
			return(nValue);			
		}
		else
		{
			ar.writeString(KeyValue );
			return(strlen(KeyValue) + 1);
		}
		
	}
			/** < void		xxxx */
	else if((strncmp(KeyName,"void",4) == 0)|| (strncmp(KeyName,"Void",4) == 0) || (strncmp(KeyName,"VOID",4) == 0)) 
	{
		nValue = myatol(KeyValue ,errCode);
		if(errCode < 0)
			return (-1);
		if(nValue > MAX_INSERT_BUF)
		{
			perror("error in BuildPdu:: void nValue > MAX_INSERT_BUF \n");
			return(-1);
		}
		p = new char[nValue];
		if(p == NULL)
		{
			perror("new error in BuildPdu::void\n");
			return(-1);
		}
		ar.writeBytes(p,nValue);
		delete[] p;
		p = NULL;
		return(nValue);			
	}
	return(0);
}


int CSendConfig::listPacket()
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if(it->isrsp == 1)
		{
			printf("%10d\t%-40s%10d\t%10d\t%10d\n",it->no,it->pduname.c_str(),it->datalen,it->pdutype,it->rspdata.rsppdutype);
		}
		else
		{
			printf("%10d\t%-40s%10d\t%10d\n",it->no,it->pduname.c_str(),it->datalen,it->pdutype);
		}
	}
}

int CSendConfig::getPacket( unsigned short no, void*& data, unsigned long& len, unsigned short& pdutype ) 
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->no == no )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		if(it->data != NULL)
		{
			if(it->datalen > len)
			{
				delete[] (char *)data;
				data = NULL;
				data = new char[it->datalen];
				if(data == NULL)
				{
					perror("new error in getPacket");
					exit(1);
				}
			}
			memcpy(data,it->data,it->datalen);
			pdutype = it->pdutype;
			len = it->datalen;
			return (0);
		}
		return(-1);
	}
}

int CSendConfig::getPacket( unsigned short pdutype, void*& data, unsigned long& len ) 
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->pdutype == pdutype )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		if(it->data != NULL)
		{
			if(it->datalen > len)
			{
				delete[] (char *)data;
				data = NULL;
				data = new char[it->datalen];
				if(data == NULL)
				{
					perror("new error in getPacket");
					exit(1);
				}
			}
			memcpy(data,it->data,it->datalen);
			len = it->datalen;
			return (0);
		}
		return(-1);
	}
}
int CSendConfig::getIsrspByNo( unsigned short no ,bool& ret,unsigned short& rsppdutype)
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->no == no )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		if(it->isrsp == 1)
		{
			ret = true;
			rsppdutype = it->rspdata.rsppdutype ;
		}
		else
		{
			ret = false;
		}
	}
	return (0);
}

int CSendConfig::getLoopsByNo( unsigned short no ,unsigned short& loop)
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->no == no )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		loop = it->loops;
	}
	return (0);
}

int CSendConfig::getLoopsByPdutype( unsigned short pdutype ,unsigned short& loop)
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->pdutype == pdutype )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		loop = it->loops;
	}
	return (0);
}

int CSendConfig::getIsrspByPdutype( unsigned short pdutype ,bool& ret,unsigned short& rsppdutype)
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->pdutype == pdutype )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	else
	{
		if(it->isrsp == 1)
		{
			ret = true;
			rsppdutype = it->rspdata.rsppdutype ;
		}
		else
		{
			ret = false;
		}
	}
	return (0);
}

int CSendConfig::checkRspPdu( unsigned short pdutype, unsigned short rsppdutype, const void* data, unsigned long len,bool& ret ) 
{
	PduNode_it	it;
	for( it = m_PduList.begin(); it != m_PduList.end(); ++ it ) 
	{
		if( it->pdutype == pdutype )
		{
			break;
		}
	}
	if( it == m_PduList.end() )
	{
		return (-1);
	}
	if(it->rspdata.rsppdutype == rsppdutype)
	{
		if (it->rspdata.checktype == 1) //check by offset
		{
			int ntemplen = 0;
			if( it->rspdata.type == 1)
			{
				ntemplen = sizeof(unsigned short);
			}
			if( it->rspdata.type == 2)
			{
				ntemplen = sizeof(unsigned long);
			}
			if(len - ntemplen < it->rspdata.offset)
			{
				ret = false;
				return(0);
			}
			char *p = (char *)data;
			p += it->rspdata.offset;
			CSockStream ar((BYTE *)p,len - it->rspdata.offset);
			if( it->rspdata.type == 1)
			{
				unsigned short shValue = 0;
				ar>>shValue;
				if(shValue == (unsigned short)(it->rspdata.checkvalue & 0xffff))
				{
					ret = true;
				}
				else
				{
					ret = false;
				}
			}
			if( it->rspdata.type == 2)
			{
				unsigned long checkValue = 0;
				ar>>checkValue;
				if(checkValue == it->rspdata.checkvalue)
				{
					ret = true;
				}
				else
				{
					ret = false;
				}
			}
		}
		else if (it->rspdata.checktype == 2) //check by len
		{
	//		unsigned long valuelen ;
		//	CSockStream ar((BYTE *)data,len);
	//		ar>>valuelen;
			if(it->rspdata.len == len)
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}
	}
	else
	{
		ret = false;
	}
	return (0);
}


int CSendConfig::close( ) 
{
	m_PduList.clear();
	return 0;
}
long CSendConfig::myatol(const char *buf,short& errCode ) 
{
	char *p = NULL;
	p  = (char *)buf;
	int count = 0;
	long value = 0;
	
	if((strncmp(buf,"0x",2) == 0) || (strncmp(buf,"0X",2) == 0))
	{
		sscanf(buf + 2,"%x",&value);
		return value;
	}

	while((*p != '\0')&&(count < 11))
	{
		if((*p >= '0') && (*p <= '9'))
		{
			p++;
		}
		else
		{
			printf("in myatol error \n");
			return(-1);
		}
		count++;
	}
	if(count >=11)
	{
		printf("in myatol error \n");
		errCode = -1;
		return(-1);
	}
	value = atoll(buf);
	errCode = 0;
	return value;
}



