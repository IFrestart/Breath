/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * �ļ���־��,�ṩ�˴�,д��־,�رյȲ���

 * @file	FileLogger.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	�ļ���־��
 **/

#ifndef	__FILELOGGER_H__
#define	__FILELOGGER_H__

#include <string>
#ifdef	WIN32
	#define vsnprintf _vsnprintf
#endif
#include "Mutex.h"

class FileLogger
{
public:
	static FileLogger* getInstance();

	/**
	* �ر���־
	* @return ���ɹ�,�򷵻�0
	*/
	int close();

	int open(const char*);
	int log(const char *, ...);

	/**
	* �Ǽ�д��־������ڵ�Դ���������к�
	* @return ��
	*/
	void setFaultPos(const char*, unsigned long);

private:
	FileLogger();
	~FileLogger();

	enum { MAXLOGLEN = 4096 };

	FILE*			fp_;

	std::string		fileName_;
	unsigned long	lineNo_;
	char			buffer_[MAXLOGLEN+1];
	CMutex		mutex_;
	static FileLogger*      pLogger_ ;
};

#endif	/*__FILELOGGER_H__*/

