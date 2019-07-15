/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * �߳���,��װ���̵߳�����,ֹͣ�Ȳ���

 * @file	Thread.h
 * @author	chetang <chetang@webcon.com.cn>

 * @short	�߳���
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
	* �����߳��Լ��Ƿ�ȡ��,���̴߳�����һ��
	* ��ѭ����ʱ,Ӧ�����ʵ�λ�ü����������,����
	* �ж������߳��Ƿ�Ҫȡ����,��ʱ�����˳�ѭ��
	* ��:
	* while(1) {
	* ...
	*    if(CThread::testCancel()) {
	*       break;
	*    }
	* ...
	* }
	* ...��������Ϊ���̷߳������Դ
	* ...
	* @return �ɹ��򷵻�1
	* @see CThread::kill()
	*/
	static bool testCancel();

	/**
	* ����һ���߳�
	* @param pCallBack �����ɹ�ִ�еĽӿں���,�����̵߳�������
	* @param arg �����ӿں����Ĳ���
	* @return �ɹ��򷵻�0
	*/
	int start(IThreadCallBack* pCallBack, void* arg);

	/**
	* ȡ���̱߳���
	* ע��˺���ֻ�����̵߳Ľӿں������в����߳��Ƿ��˳���
	* �������²Żᷢ������
	* @return �ɹ��򷵻�0
	* @see CThread::testCancel()
	*/
	int kill();

protected:

private:
	typedef ACE_thread_t THREAD_T;
	static void* threadProc(void* arg);

	IThreadCallBack*	pCallBack_;	/**< �̵߳Ļص��ӿ� */
	void*				arg_;	/**< �û��Զ���Ĳ���,���̻߳ص�ʱԭֵ���� */

	THREAD_T			threadID_;	/**< �ڲ��õ��̱߳�ʶ */
};

#endif	/*__THREAD_H__*/

