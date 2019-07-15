/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 系统日志类,提供了打开,写日志,关闭等操作

 * @file	SysLogger.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	系统日志类
 **/

#ifndef	__SYSLOGGER_H__
#define	__SYSLOGGER_H__

#include <string>


#ifdef _WIN32
#include <stdarg.h>
#else
#include <syslog.h>

#endif


#include "Mutex.h"

class SysLogger
{
public:
	static SysLogger* getInstance();

	/**
	* 关闭日志
	* @return 若成功,则返回0
	*/
	int close();

	/**
	* 打开日志
	* @param ident 特定标识,会显示在每一条日志记录中
	* @return 若成功,则返回0
	*/
	int open(const char*, int = LOG_CONS | LOG_PID, int = LOG_USER);

	/**
	* 输出等级为EMERG的日志
	* @return 若成功,则返回0
	*/
	int emerg(const char*, ...);

	/**
	* 输出等级为EMERG的日志
	* @return 若成功,则返回0
	*/
	int alert(const char*, ...);

	/**
	* 输出等级为CRIT的日志
	* @return 若成功,则返回0
	*/
	int crit(const char*, ...);

	/**
	* 输出等级为ERROR的日志
	* @return 若成功,则返回0
	*/
	int error(const char*, ...);

	/**
	* 输出等级为WARN的日志
	* @return 若成功,则返回0
	*/
	int warn(const char*, ...);

	/**
	* 输出等级为NOTICE的日志
	* @return 若成功,则返回0
	*/
	int notice(const char*, ...);

	/**
	* 输出等级为INFO的日志
	* @return 若成功,则返回0
	*/
	int info(const char*, ...);

	/**
	* 输出等级为DEBUG的日志
	* @return 若成功,则返回0
	*/
	int debug(const char*, ...);

	/**
	* 登记写日志语句所在的源程序名与行号
	* @return 无
	*/
	void setFaultPos(const char*, unsigned long);

protected:
	SysLogger();
	~SysLogger();

private:
	enum { MAXLOGLEN = 4096 };
	int log(int priority, const char*, va_list va);

	std::string		fileName_;
	unsigned long	lineNo_;
	bool			isOpen_;
	char			buffer_[MAXLOGLEN+1];
	CMutex		mutex_;
	static SysLogger* pSysLogger_;
};

#endif	/*__SYSLOGGER_H__*/

