/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * 定时器的接口类

 * @file	TimerEventCallBack.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	定时器的接口类
 **/

#ifndef	__TIMEREVENTCALLBACK_H__
#define	__TIMEREVENTCALLBACK_H__

class ITimerEventCallBack
{
public:
	ITimerEventCallBack();
	virtual ~ITimerEventCallBack();
	
	/**
	* 定时器接口的回调函数
	* @param timerID 在CTimer::start()开始一个定时器时传入的自定义标识
	* @param arg 在CTimer::start()开始一个定时器时传入的自定义参数
	* @return 若返回值不为0,则中止此定时器
	* @see CTimer::start()
	*/
	virtual int OnTimer(unsigned long timerID, void* arg) = 0;

protected:

private:

};

#endif	/*__TIMEREVENTCALLBACK_H__*/

