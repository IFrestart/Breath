/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	FileGetPara.cpp
 * @author	zhoujj <zhoujj@webcon.com.cn>
 * @date        05/2004
 * @short 
 **/
#include  "FileGetPara.h"

CFileGetPara::CFileGetPara()
{
	m_strRetbuf = NULL;
	m_strRetbuf = new char[MAX_FOR_RET_BUF + 1];
	m_doc = NULL;
}

CFileGetPara::~CFileGetPara()
{
	if(m_strRetbuf != NULL)
	{
		delete[] m_strRetbuf;
		m_strRetbuf = NULL;
	}
	UnloadConfig();
}

char* CFileGetPara::GetParaString(const char* section, const char* item_key, char* default_value)
{
	TiXmlNode* node = 0;
	TiXmlElement* element = 0;
	TiXmlText*  mytext = NULL;
	bool find = false;
	node = m_doc->RootElement();

	memset(m_strRetbuf,0,MAX_FOR_RET_BUF + 1);
	if((node == 0 ) || (node->ToElement() == 0))
	{
		goto finderror;
	}
	element = node->ToElement();
	//add by zjj 2007-08-20
	if(strcmp(element->Value() ,section) != 0)
	{
	//end add
		for( element = element->FirstChildElement(); element ; element = element->NextSiblingElement() )
		{
			if(strcmp(element->Value() ,section) == 0)
			{
				find = true;
				break;
			}
		}
		if(!find)
		{
			goto finderror;
		}
	//add by zjj 2007-08-20
	}
	//end add
	find = false;
	for( element = element->FirstChildElement(); element ; element = element->NextSiblingElement() )
	{
		if(strcmp(element->Value() ,item_key) == 0)
		{
			find = true;
			break;
		}
	}
	if(find)
	{
		mytext = element->FirstChild()->ToText() ;
		if(mytext)
		{
			if(strlen(mytext->Value())  > MAX_FOR_RET_BUF)
			{
				if (m_strRetbuf)		//add 2011-7-8
				{
					delete []m_strRetbuf;
				}
				m_strRetbuf = new char[strlen(mytext->Value()) + 1];
				memset(m_strRetbuf,0,strlen(mytext->Value()) + 1);
				strcpy(m_strRetbuf,mytext->Value());
			}
			else
			{
				strcpy(m_strRetbuf,mytext->Value());
			}
			return m_strRetbuf;
		}
	}

	finderror:
		if(default_value == NULL)
			return(NULL);
		if(strlen(default_value)  > MAX_FOR_RET_BUF)
		{
			if (m_strRetbuf)	//add 2011-7-8
			{
				delete []m_strRetbuf;
			}
			m_strRetbuf = new char[strlen(default_value) + 1];
			memset(m_strRetbuf,0,strlen(default_value) + 1);
			strcpy(m_strRetbuf,default_value);
		}
		else
			strcpy(m_strRetbuf,default_value);
		return m_strRetbuf;
}
int CFileGetPara::InitConfig(const char* file_name)
{
	bool loadOkay = false;
   	TiXmlDocument *pXMLDoc = NULL;
	pXMLDoc = new TiXmlDocument(file_name);
  	loadOkay = pXMLDoc->LoadFile();
	if(!loadOkay)
	{
		//printf( "Could not load test file %s. Error='%s'. Exiting.\n", file_name,pXMLDoc->ErrorDesc() );
		return(-1);
	}
	else
	{
   		m_doc = pXMLDoc;
	}
   	return 0;
}
void CFileGetPara::UnloadConfig( )
{
	if(m_doc)
	{
		m_doc->Clear();
		delete m_doc;
		m_doc = NULL;
	}
}

int CFileGetPara::GetParaInt(const char* section, const char* item_key, int default_value)
{
	char str_default[16] = {0};
	sprintf(str_default,"%d",default_value);
	if(GetParaString(section, item_key, str_default) != NULL)
	{
		return(atoi(m_strRetbuf));
	}
	return(-1);
}
	
bool CFileGetPara::GetParaBool(const char* section, const char* item_key, bool default_value)
{
	char str_default[16] = {0};
	if(default_value)
	{
		strcpy(str_default,"true");
	}
	else
	{
		strcpy(str_default,"false");
	}
	if(GetParaString(section, item_key, str_default) != NULL)
	{
		if(strcmp(m_strRetbuf,"true") == 0)
			return(true);
	}
	return(false);
}
	
int CFileGetPara::AddParaInt(const char * SectionName, const char * pKeyName, int item_value)
{
	return 0;
}
int CFileGetPara::AddParaBool(const char * SectionName, const char * pKeyName, bool item_value)
{
	return 0;
}
int CFileGetPara::AddParaString(const char * SectionName, const char * pKeyName, const char* item_value)
{
	return 0;
}
	
int CFileGetPara::ModifyParaInt(const char * SectionName, const char * pKeyName, int Newitem_value)
{
	return 0;
}
int CFileGetPara::ModifyParaBool(const char * SectionName, const char * pKeyName, bool Newitem_value)
{
	return 0;
}
int CFileGetPara::ModifyParaString(const char * SectionName, const char * pKeyName, const char* Newitem_value)
{
	return 0;
}

int CFileGetPara::DelPara(const char * SectionName, const char * pKeyName)
{
	return 0;
}

