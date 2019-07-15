/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	FileGetPara.h
 * @author	zhoujj <zhoujj@webcon.com.cn>
 * @date     	05/2004
 * @short 
 **/
#ifndef _FILE_GETPARA_
#define _FILE_GETPARA_

#include "tinyxml.h"

#define MAX_FOR_RET_BUF	256
class CFileGetPara 
{
public:

	CFileGetPara();

	~CFileGetPara();
	
	char* GetParaString(const char* section, const char* item_key, char* default_value);

	int GetParaInt(const char* section, const char* item_key, int default_value);
	
	bool GetParaBool(const char* section, const char* item_key, bool default_value);
	
	int AddParaInt(const char * SectionName, const char * pKeyName, int item_value);
	int AddParaBool(const char * SectionName, const char * pKeyName, bool item_value);
	int AddParaString(const char * SectionName, const char * pKeyName, const char* item_value);
	
	int ModifyParaInt(const char * SectionName, const char * pKeyName, int Newitem_value);
	int ModifyParaBool(const char * SectionName, const char * pKeyName, bool Newitem_value);
	int ModifyParaString(const char * SectionName, const char * pKeyName, const char* Newitem_value);

	int DelPara(const char * SectionName, const char * pKeyName);

	int InitConfig(const char* file_name);

	void UnloadConfig( );

private:
	char *m_strRetbuf;
	TiXmlDocument *m_doc;
};

#endif //_FILE_GETPARA_

