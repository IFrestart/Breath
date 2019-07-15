/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ϵͳ��־��,�ṩ�˴�,д��־,�رյȲ���

 * @file	SysLogger.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	ϵͳ��־��
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
	* �ر���־
	* @return ���ɹ�,�򷵻�0
	*/
	int close();

	/**
	* ����־
	* @param ident �ض���ʶ,����ʾ��ÿһ����־��¼��
	* @return ���ɹ�,�򷵻�0
	*/
	int open(const char*, int = LOG_CONS | LOG_PID, int = LOG_USER);

	/**
	* ����ȼ�ΪEMERG����־
	* @return ���ɹ�,�򷵻�0
	*/
	int emerg(const char*, ...);

	/**
	* ����ȼ�ΪEMERG����־
	* @return ���ɹ�,�򷵻�0
	*/
	int alert(const char*, ...);

	/**
	* ����ȼ�ΪCRIT����־
	* @return ���ɹ�,�򷵻�0
	*/
	int crit(const char*, ...);

	/**
	* ����ȼ�ΪERROR����־
	* @return ���ɹ�,�򷵻�0
	*/
	int error(const char*, ...);

	/**
	* ����ȼ�ΪWARN����־
	* @return ���ɹ�,�򷵻�0
	*/
	int warn(const char*, ...);

	/**
	* ����ȼ�ΪNOTICE����־
	* @return ���ɹ�,�򷵻�0
	*/
	int notice(const char*, ...);

	/**
	* ����ȼ�ΪINFO����־
	* @return ���ɹ�,�򷵻�0
	*/
	int info(const char*, ...);

	/**
	* ����ȼ�ΪDEBUG����־
	* @return ���ɹ�,�򷵻�0
	*/
	int debug(const char*, ...);

	/**
	* �Ǽ�д��־������ڵ�Դ���������к�
	* @return ��
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

