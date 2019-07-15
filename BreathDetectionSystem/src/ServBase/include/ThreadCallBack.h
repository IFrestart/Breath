/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 线程的接口类

 * @file	ThreadCallBack.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	线程的接口类
 **/

#ifndef	__THREADCALLBACK_H__
#define	__THREADCALLBACK_H__

class IThreadCallBack
{
public:
	IThreadCallBack();
	virtual ~IThreadCallBack();

	/**
	* 线程接口的回调函数
	* @param arg 在CThread::start()派生一个线程时传入的自定义参数
	* @return 成功则返回0
	* @see CThread::start()
	*/
	virtual void* threadProc(void* arg) = 0;

protected:

private:

};

#endif	/*__THREADCALLBACK_H__*/

