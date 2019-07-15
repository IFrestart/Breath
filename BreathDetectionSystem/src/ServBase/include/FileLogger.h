/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 文件日志类,提供了打开,写日志,关闭等操作

 * @file	FileLogger.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	文件日志类
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
	* 关闭日志
	* @return 若成功,则返回0
	*/
	int close();

	int open(const char*);
	int log(const char *, ...);

	/**
	* 登记写日志语句所在的源程序名与行号
	* @return 无
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

