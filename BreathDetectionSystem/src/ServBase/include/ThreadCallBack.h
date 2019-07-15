/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * �̵߳Ľӿ���

 * @file	ThreadCallBack.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	�̵߳Ľӿ���
 **/

#ifndef	__THREADCALLBACK_H__
#define	__THREADCALLBACK_H__

class IThreadCallBack
{
public:
	IThreadCallBack();
	virtual ~IThreadCallBack();

	/**
	* �߳̽ӿڵĻص�����
	* @param arg ��CThread::start()����һ���߳�ʱ������Զ������
	* @return �ɹ��򷵻�0
	* @see CThread::start()
	*/
	virtual void* threadProc(void* arg) = 0;

protected:

private:

};

#endif	/*__THREADCALLBACK_H__*/

