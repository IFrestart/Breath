/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 线程类,封装了线程的启动,停止等操作

 * @file	Thread.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	线程类
 **/

#ifndef	__THREAD_H__
#define	__THREAD_H__

#include "ace/Thread_Manager.h"

#include "ThreadCallBack.h"

class CThread
{
public:
	CThread();
	~CThread();

	/**
	* 测试线程自己是否被取消,当线程处理在一个
	* 死循环中时,应当在适当位置加上这个测试,用来
	* 判断其它线程是否要取消它,此时可以退出循环
	* 如:
	* while(1) {
	* ...
	*    if(CThread::testCancel()) {
	*       break;
	*    }
	* ...
	* }
	* ...这里清理为此线程分配的资源
	* ...
	* @return 成功则返回1
	* @see CThread::kill()
	*/
	static bool testCancel();

	/**
	* 派生一个线程
	* @param pCallBack 派生成功执行的接口函数,它是线程的主函数
	* @param arg 传给接口函数的参数
	* @return 成功则返回0
	*/
	int start(IThreadCallBack* pCallBack, void* arg);

	/**
	* 取消线程本身
	* 注意此函数只能在线程的接口函数中有测试线程是否退出的
	* 语句情况下才会发生作用
	* @return 成功则返回0
	* @see CThread::testCancel()
	*/
	int kill();

protected:

private:
	typedef ACE_thread_t THREAD_T;
	static void* threadProc(void* arg);

	IThreadCallBack*	pCallBack_;	/**< 线程的回调接口 */
	void*				arg_;	/**< 用户自定义的参数,在线程回调时原值传回 */

	THREAD_T			threadID_;	/**< 内部用的线程标识 */
};

#endif	/*__THREAD_H__*/

