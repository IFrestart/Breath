/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * ��ʱ����,��װ�˶�ʱ��������,ֹͣ�Ȳ���

 * @file	Timer.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	��ʱ����
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
	* ������ʱ��
	* @param timerID �Զ���Ķ�ʱ����ʶ
	* @param microseconds �����ٺ��봥��һ��
	* @param pCallBack ��ʱ����ʱʱ�ص��Ľӿ�
	* @param arg ��ʱ����ʱʱ���ص��Ľӿڴ��Ĳ���
	* @return �ɹ��򷵻�0
	*/
	int start(unsigned long timerID, unsigned long microseconds,
						ITimerEventCallBack* pCallBack, void* arg = 0);
	/**
	* ������ʱ��
	* @param timerID �Զ���Ķ�ʱ����ʶ
	* @param timeval ������ʱ�䴥��һ��
	* @param pCallBack ��ʱ����ʱʱ�ص��Ľӿ�
	* @param arg ��ʱ����ʱʱ���ص��Ľӿڴ��Ĳ���
	* @return �ɹ��򷵻�0
	*/
	int start(unsigned long timerID, struct timeval timeval,
						ITimerEventCallBack* pCallBack, void* arg = 0);

	/**
	* ֹͣ��ʱ��
	* @return �ɹ��򷵻�0
	*/
	int kill();

protected:
	/**
	* �̳���ACE_Event_Handler
	* @see ACE_Event_Handler::handle_timeout()
	*/
	int handle_timeout(const ACE_Time_Value& now, const void* act = 0);

private:
	unsigned long		userID_;	/**< �û��Զ���Ķ�ʱ����ʶ,�ڻص�ʱԭֵ���� */
	const void*			arg_;	/**< �û�������ʱ��ʱ�Զ���Ĳ���,�ڻص�ʱԭֵ���� */
	struct timeval		timeval_;	/**< ��ʱ����� */
	ITimerEventCallBack*	pCallBack_;	/**< ��ʱ���ص��ӿ� */

	long				timerID_;	/**< �ڲ��õĶ�ʱ����ʶ */
};

//extern MyTimer        g_myTimer;
//extern MyTimer&       timerInstance();

#endif	/*__TIMER_H__*/

