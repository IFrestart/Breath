/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 日志类,提供了打开,写日志,关闭等操作

 * @file	Logger.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	日志类
 **/

#ifndef	__LOGGER_H__
#define	__LOGGER_H__

#ifdef	WEBCON_HAS_FILELOG
	#include "FileLogger.h"

	#define OPENLOG(ident)	FileLogger::getInstance()->open((ident))
	#define _EMERG	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _ALERT	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _CRIT	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _ERROR	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _WARN	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _NOTICE	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _INFO	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define _DEBUG	FileLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					FileLogger::getInstance()->log
	#define CLOSELOG()	FileLogger::getInstance()->close()
#elif defined(WEBCON_HAS_SYSLOG)
	#include "SysLogger.h"

	#define OPENLOG(ident)	SysLogger::getInstance()->open((ident))
	#define _EMERG	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->emerg
	#define _ALERT	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->alert
	#define _CRIT	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->crit
	#define _ERROR	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->error
	#define _WARN	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->warn
	#define _NOTICE	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->notice
	#define _INFO	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->info
	#define _DEBUG	SysLogger::getInstance()->setFaultPos(__FILE__, __LINE__);	\
					SysLogger::getInstance()->debug
	#define CLOSELOG()	SysLogger::getInstance()->close()
#elif defined(WEBCON_HAS_STDLOG)
	#define OPENLOG(ident)	
	#define _EMERG	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _ALERT	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _CRIT	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _ERROR	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _WARN	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _NOTICE	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _INFO	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define _DEBUG	fflush(0);printf("\n%s:%d: ", __FILE__, __LINE__);	\
					printf
	#define CLOSELOG()	printf("\n"); fflush(0)
#else
	#define OPENLOG(ident)	
	#define _EMERG	
	#define _ALERT	
	#define _CRIT	
	#define _ERROR	
	#define _WARN	
	#define _NOTICE	
	#define _INFO	
	#define _DEBUG	
	#define CLOSELOG()	
#endif	/*WEBCON_HAS_FILELOG*/

#endif	/*__LOGGER_H__*/

