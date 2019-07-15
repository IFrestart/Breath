/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 服务器基类,为服务器类提供了一个通用的流程:
 * 包括读配置文件,初始化函数,销毁函数,以及循环驱动
 * 并提供了对TCP/UDP协议关的封装

 * @file	BaseServ.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	服务器基类
 **/

#ifndef	__BASESERV_H__
#define	__BASESERV_H__

#include "ace/OS.h"
#include "ace/Event_Handler.h"

#include "SockHeaderCallBack.h"
#include "TCPSvcHandler.h"
#include "Logger.h"

#ifdef OUR_DEMO_VER

#include "Timer.h"

#endif


class CBaseServ : public ACE_Event_Handler
{
//friend int ::ACE_TMAIN(int argc, char** argv);

public:
	CBaseServ();
	virtual ~CBaseServ();

#ifdef  __TXT_MODE__

	/**
	* 获取CBaseServ的唯一实例
	* @return 不成功则返回0
	*/
	static CBaseServ* getInstance();
#endif

	/**
	* 主函数,将执行文件的名字作为日志的标志打开日志;屏蔽SIGINT与SIGPIPE信号;
	* 并检查是否有"-ini"的默认参数,若存在,把它作为配置文件的名字,否则用执行文件的名字后缀
	* 上".ini"作为配置文件的名字,读取配置文件;依次执行初始化函数,循环驱动函数,与销毁函数,最后关闭日志
	* 其中每一步都记录到日志中.
	* @param argc 程序执行时所带的参数个数,包括执行文件
	* @param argv 程序执行时所带的参数,包括执行文件名
	* @return 成功则返回0
	* @see loadConfig(),initialize(),run(),destroy(),
	*/
	virtual int doMain(int argc, char** argv);

protected:
	/**
	* 读取配置文件,默认的配置文件名为argv[0].ini,与argv[0]在同一目录
	* 在initialize()前执行
	* @return 若读取不成功要中止程序 时则应该返回非0,此应用程序直接退出
	* 注意此时并不执行destroy()
	*/
	virtual int loadConfig(const char* pFile);

	/**
	* 在胠oadConfig()后的第一个函数,用于初始,如创建连接等等
	* @param argc 程序执行时所带的参数个数,包括执行文件
	* @param argv 程序执行时所带的参数,包括执行文件名
	* @return 若初始化不成功要中止程序 时则应该返回非0,此应用程序将调用destroy()后退出
	*/
	virtual int	initialize(int argc, char** argv) = 0;

	/**
	* ISockHeaderCallBack中的实现函数
	* @return 成功则返回0
	* @see ISockHeaderCallBack::run()
	*/
	virtual int run();

	/**
	* 清理函数,在应用程序即将退出前执行.用来执行释放在initialize()中申请的
	* 资源等工作.
	* @return 不成功则返回0
	*/
	virtual int destroy();

	/**
	* 信号处理函数
	* @param signum 接收到的信号值
	* @return 成功则返回0
	*/
	/* From ACE_Event_Handler */
	virtual int handle_signal(int signum, siginfo_t*, ucontext_t*);

private:

#ifdef OUR_DEMO_VER
	class TimerDemo : public ITimerEventCallBack
	{
	public:
		TimerDemo();
		~TimerDemo();
		int start();
		int kill();

	private:
		/* From ITimerEventCallBack */
		virtual int OnTimer(unsigned long, void*);
		int checkTimer();
		int lock_test(int fd);

	private:
		enum {TIMEDEMO_TIMER_ID = 1, TIMEDEMO_INTERVAL = 60000, OUT_OUR_DEMO_VER_TIME = 604800000};
	    //enum {TIMEDEMO_TIMER_ID = 1, TIMEDEMO_INTERVAL = 60000, OUT_OUR_DEMO_VER_TIME = 129600000};
		//enum {TIMEDEMO_TIMER_ID = 1, TIMEDEMO_INTERVAL = 60000, OUT_OUR_DEMO_VER_TIME = 2700000};
		//enum {TIMEDEMO_TIMER_ID = 1, TIMEDEMO_INTERVAL = 2000, OUT_OUR_DEMO_VER_TIME = 50000};
		enum { DEMO_START_TIME = 1111250000 };
		CTimer		checkTimer_;
		int			fd ;
		bool 		havelock;
		std::string Bstr;
		std::string Estr;
	};
	TimerDemo		timerDemo_;

	friend class TimerDemo;
#endif

#ifdef  __TXT_MODE__
	static CBaseServ*	s_pBaseServ;
#endif

	ACE_thread_t		threadID_;
	int					processID_ ;
};

//extern int ACE_TMAIN(int argc, char** argv);

#endif	/*__BASESERV_H__*/

