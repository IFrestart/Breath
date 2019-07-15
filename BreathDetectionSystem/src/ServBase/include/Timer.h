/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 定时器类,封装了定时器的启动,停止等操作

 * @file	Timer.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	定时器类
 **/

#ifndef	__TIMER_H__
#define	__TIMER_H__

#include "ace/OS.h"
#include "ace/Event_Handler.h"

#include "TimerEventCallBack.h"
/*
#include "ace/Timer_Heap.h"
//#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Queue.h"

typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> MyTimer;
//typedef ACE_Timer_Node<ACE_Timer_Heap> MyTimer;
*/

class CTimer : public ACE_Event_Handler
{
public:
	CTimer();
	~CTimer();

	/**
	* 启动定时器
	* @param timerID 自定义的定时器标识
	* @param microseconds 隔多少毫秒触发一次
	* @param pCallBack 定时器到时时回调的接口
	* @param arg 定时器到时时给回调的接口传的参数
	* @return 成功则返回0
	*/
	int start(unsigned long timerID, unsigned long microseconds,
						ITimerEventCallBack* pCallBack, void* arg = 0);
	/**
	* 启动定时器
	* @param timerID 自定义的定时器标识
	* @param timeval 隔多少时间触发一次
	* @param pCallBack 定时器到时时回调的接口
	* @param arg 定时器到时时给回调的接口传的参数
	* @return 成功则返回0
	*/
	int start(unsigned long timerID, struct timeval timeval,
						ITimerEventCallBack* pCallBack, void* arg = 0);

	/**
	* 停止定时器
	* @return 成功则返回0
	*/
	int kill();

protected:
	/**
	* 继承自ACE_Event_Handler
	* @see ACE_Event_Handler::handle_timeout()
	*/
	int handle_timeout(const ACE_Time_Value& now, const void* act = 0);

private:
	unsigned long		userID_;	/**< 用户自定义的定时器标识,在回调时原值传回 */
	const void*			arg_;	/**< 用户启动定时器时自定义的参数,在回调时原值传回 */
	struct timeval		timeval_;	/**< 定时器间隔 */
	ITimerEventCallBack*	pCallBack_;	/**< 定时器回调接口 */

	long				timerID_;	/**< 内部用的定时器标识 */
};

//extern MyTimer        g_myTimer;
//extern MyTimer&       timerInstance();

#endif	/*__TIMER_H__*/

