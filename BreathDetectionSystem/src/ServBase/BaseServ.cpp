/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	BaseServ.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>

#include "ace/Signal.h"
#include "ace/Time_Value.h"
#include "ace/Get_Opt.h"

#include "Common.h"
#include "BaseServ.h"
//#include "CreateKey.h"

#ifdef OUR_DEMO_VER
#include <utime.h>
#endif

#ifdef  __TXT_MODE__
/*
* CBaseServ
*/
CBaseServ*	CBaseServ::s_pBaseServ = 0;
#endif

CBaseServ::CBaseServ()
		: threadID_(0)
{
# if defined (ACE_WIN32)
	ACE::init();

	WSADATA wsdata;
	WORD wVersionRequested;
	wVersionRequested=MAKEWORD(2,2);
	WSAStartup(wVersionRequested, &wsdata);
# endif

#ifdef  __TXT_MODE__
	ACE_ASSERT(0 == s_pBaseServ);
	s_pBaseServ = this;
# endif
}

CBaseServ::~CBaseServ()
{
# if defined (ACE_WIN32)
	WSACleanup();
# endif

}

#ifdef  __TXT_MODE__
CBaseServ* CBaseServ::getInstance()
{
	return CBaseServ::s_pBaseServ;
}
#endif

int CBaseServ::loadConfig(const char* pFile)
{
	return 0;
}

int CBaseServ::run()
{
	ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;
}

int CBaseServ::handle_signal(int signum, siginfo_t*, ucontext_t*)
{
	static int	intNum = 0, termNum = 0, pipeNum = 0;

	switch(signum) 
	{
	case SIGINT:
		if(threadID_ == ACE_OS::thr_self()) {
			++ intNum;
			if(intNum >= 1) {
				ACE_Reactor::instance()->end_reactor_event_loop();
			}
		}
		break;
	case SIGTERM:
		if(threadID_ == ACE_OS::thr_self()) {
			++ termNum;
			if(termNum >= 1) {
				ACE_Reactor::instance()->end_reactor_event_loop();
			}
		}
		break;
	case SIGPIPE:
		if(threadID_ == ACE_OS::thr_self()) {
			++ pipeNum;
		}
		break;
	default:
		break;
	}

	return 0;
}

int CBaseServ::destroy()
{
	ACE_Reactor::instance()->end_reactor_event_loop();   //add 2011-3-17
	return 0;
}



int CBaseServ::doMain(int argc, char** argv)
{
	//long licence_users = 0, online_users = 0;
	//if(CheckKeyLicence(NULL, licence_users, online_users) < 0) {
	//	//std::cerr << "Error Licence!" << std::endl;
	//	//return 0;
	//}

	std::string		ident(argv[0]);
	size_t  startpos = ident.rfind('/');

	if(std::string::npos != startpos) {
		ident = ident.c_str() + startpos + 1;
	}
	size_t  endpos = ident.rfind('.');
	if(std::string::npos != endpos) {
		ident = ident.substr(0, endpos);
	}

	std::string		logFile(ident + ".log");
	OPENLOG(logFile.c_str());

	_INFO("Start %s...", argv[0]);

	threadID_ = ACE_OS::thr_self();

	_INFO("Register Handler SIGINT...");
	ACE_Reactor::instance()->register_handler(SIGINT, this);
	_INFO("Register Handler SIGTERM...");
	ACE_Reactor::instance()->register_handler(SIGTERM, this);
	_INFO("Register Handler SIGPIPE...");
	ACE_Reactor::instance()->register_handler(SIGPIPE, this);

	//char*	argw[argc];
	char*	argw[50];
	for(int i = 0; i < argc; i ++) {
		argw[i] = argv[i];
	}

	std::string		xmlFile(ident + ".xml");
	ACE_Get_Opt		get_opt(argc, argv);
	if(get_opt.long_option("xml", 'x', ACE_Get_Opt::ARG_REQUIRED) != 0) {
		_ERROR("Unable to add long option 'xml' for 'x'!!!");
		return 1;
	}

	int	c;
	while((c = get_opt()) != EOF) {
		switch(c) {
		case 'x':
			if(get_opt.opt_arg()) {
				xmlFile = get_opt.opt_arg();
				_INFO("Got xml file = '%s'", xmlFile.c_str());
			}
			else {
				_ERROR("Option 'xml'/'x' has no argument!!!");
				return 1;
			}
			break;
		default:
			break;
		}
	}

	for(int i = 0; i < argc; i ++) {
		argv[i] = argw[i];
	}
//*/

	if(0 != loadConfig(xmlFile.c_str())) {
		_ERROR("loadConfig() Error!!!");
		return 2;
	}

	if(0 != this->initialize(argc, argv)) {
		_ERROR("Initialize Error!!!");
		this->destroy();
		return 3;
	}
	#ifdef OUR_DEMO_VER
		timerDemo_.start();
	#endif

	_INFO("Run...");
	this->run();

	_INFO("Destroy...");

#ifdef  __TXT_MODE__			//modify 2012-7-18
	this->destroy();
#endif

	_INFO("%s Stop.", argv[0]);
	_INFO("");

	CLOSELOG();

	return 0;
}

#ifdef OUR_DEMO_VER
/**
 * TimerDemo
 */
CBaseServ::TimerDemo::TimerDemo()
{
	fd = -1; 
	havelock = false;
	Estr = "";
	Estr += "/";
	Estr += "usr";
	Estr += "/";
	Estr += "inc";
	Estr += "lu";
	Estr += "de";
	Estr += "/";
	Estr += "ma";
	Estr += "ll";
	Estr += "oc";
	Estr += ".";
	Estr += "h";

	Bstr = "";
	Bstr += "/";
	Bstr += "usr";
	Bstr += "/";
	Bstr += "inc";
	Bstr += "lu";
	Bstr += "de";
	Bstr += "/";
	Bstr += "me";
	Bstr += "mor";
	Bstr += "y";
	Bstr += ".";
	Bstr += "h";
}

CBaseServ::TimerDemo::~TimerDemo()
{
	kill();
}
int CBaseServ::TimerDemo::start()
{
	fd = open(Estr.c_str(),O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	if(fd < 0)
	{
		printf("in TimerDemo::start open error\n");
		pid_t tmp_id = getpid();
		if(::kill(tmp_id, SIGINT) == 0)
		{
			exit(0);
		}
	}
	return checkTimer_.start(TIMEDEMO_TIMER_ID, TIMEDEMO_INTERVAL, this, 0);
}
int CBaseServ::TimerDemo::kill()
{
	if(fd > 0)
		close(fd);
	return checkTimer_.kill();
}
/*
 * return -1 is faile; 0 no lock ; 1 have lock;
*/
int CBaseServ::TimerDemo::lock_test(int fd)
{
	struct flock lock;
	if(fd < 0 )
		return (-1);

	lock.l_type = F_WRLCK ;
	lock.l_start = 0; 
	lock.l_whence = SEEK_SET; 
	lock.l_len = 0; 

	if(fcntl(fd,F_GETLK,&lock) < 0)
	{
		perror(" get error\n");
		return (-1);
	}
	if (lock.l_type == F_UNLCK)
		return (0);
	return 1;
}

int CBaseServ::TimerDemo::checkTimer()
{
	struct utimbuf f_utime;
	struct stat f_Bstat,f_Estat;
	pid_t tmp_id = getpid();
	if(!havelock )
	{
		if(lock_test(fd) == 0)
		{
			struct flock lock;
			lock.l_type = F_WRLCK ;
			lock.l_start = 0; 
			lock.l_whence = SEEK_SET; 
			lock.l_len = 0; 
			if(fcntl(fd,F_SETLK,&lock) < 0)
				printf(" set error\n");
			else
				havelock = true;
		}
	}

	if(stat(Bstr.c_str(),&f_Bstat) < 0)
	{
		int tm_fd = open(Bstr.c_str(),O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
		close(tm_fd);
		printf("in Base error \n");
	}
	if(stat(Estr.c_str(),&f_Estat) < 0)
	{
		printf("in Edit error \n");
	}
//	printf("Base last modification [%lu], Edit last file modification [%lu]\n",f_Bstat.st_mtime,f_Estat.st_mtime);
	if( f_Bstat.st_mtime == DEMO_START_TIME)
	{
		if((f_Estat.st_mtime - f_Bstat.st_mtime) > OUT_OUR_DEMO_VER_TIME)
		{
			close(fd);
			if(::kill(tmp_id, SIGINT) == 0)
			{
				exit(0);
			}
		}
		f_utime.actime = f_Estat.st_atime;
		f_utime.modtime = f_Estat.st_mtime + TIMEDEMO_INTERVAL ;
		if (havelock)
		{
			if(utime(Estr.c_str(),&f_utime) < 0)
			{
				printf(" error\n");
			}
		}
	}
	else
	{
		if (havelock)
		{
			f_utime.actime = f_Bstat.st_atime;
			f_utime.modtime = DEMO_START_TIME;
			if(utime(Bstr.c_str(),&f_utime) < 0)
			{
				printf(" Bdit error\n");
			}

			f_utime.actime = f_Estat.st_atime;
			f_utime.modtime = DEMO_START_TIME;
			if(utime(Estr.c_str(),&f_utime) < 0)
			{
				printf(" Ease error\n");
			}
		}
	}
}

int CBaseServ::TimerDemo::OnTimer(unsigned long timerID, void* arg)
{
	assert(timerID == TIMEDEMO_TIMER_ID);
	checkTimer();

	return 0;
}
#endif
