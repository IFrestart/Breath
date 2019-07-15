/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ��ʱ���Ľӿ���

 * @file	TimerEventCallBack.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	��ʱ���Ľӿ���
 **/

#ifndef	__TIMEREVENTCALLBACK_H__
#define	__TIMEREVENTCALLBACK_H__

class ITimerEventCallBack
{
public:
	ITimerEventCallBack();
	virtual ~ITimerEventCallBack();
	
	/**
	* ��ʱ���ӿڵĻص�����
	* @param timerID ��CTimer::start()��ʼһ����ʱ��ʱ������Զ����ʶ
	* @param arg ��CTimer::start()��ʼһ����ʱ��ʱ������Զ������
	* @return ������ֵ��Ϊ0,����ֹ�˶�ʱ��
	* @see CTimer::start()
	*/
	virtual int OnTimer(unsigned long timerID, void* arg) = 0;

protected:

private:

};

#endif	/*__TIMEREVENTCALLBACK_H__*/

