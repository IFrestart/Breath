/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 FCNetACLib.h
* @author	 zhoujj <zhoujj@webcon.com.cn>
* @class	FCNetACLib
**/

#ifndef	__FCNETAC_LIB_H__
#define	__FCNETAC_LIB_H__

#define __FCNET_API_DLLEXPORT__

#ifndef __LINUX__

#define TIME_OUT_INFINITE           0xFFFFFFFF  // Infinite timeout

#ifdef __FCNET_API_DLLEXPORT__
#define FCNETAC_API  extern "C" __declspec(dllexport)
#else
#define FCNETAC_API extern "C" __declspec(dllimport)

#endif 

#else

#define TIME_OUT_INFINITE           0x0FFFFFFF  // Infinite timeout
#define FCNETAC_API extern "C"
#define __stdcall
#endif

#include <string>
#include <vector>

#define MAX_USER_ACCOUNT_LEN		129
#define MAX_USER_PASSWD_LEN			17

#define MSGTYPE_OnReceivedData			1	//�յ����ݻص�����
//OnReceivedData(const char *user_id, const void *data, unsigned long dataLen,unsigned short pdu,)

#define MSGTYPE_OnMemberRoster			2	//��Ա�ı�ص�����
//OnMemberRoster(const char *user_id, void* data, unsigned long dataLen,int roster_flag, unsigned long user_attr);
/** ��Ա�ı�ص�����
* @param nRosterFlag �ı��־
* 0:p2p����  1: ������ת������   2: �뿪  3: �������쳣 4 ����״̬�ı�  5 �˺���һ�ط���½
* @param user_attr �ı��־
*         ��16λ�еĸ�8Ϊ 0:����  1:����  2: ���Է���  3: ��ֹ���� 4:�������뷢��״̬
*/

#define MSGTYPE_OnLogOut				3	//�û��˳��ص�

#define MSGTYPE_OnMeetInfoChanged			4	//������Ϣ�ĸ���
#define MSGTYPE_OnSecondLogined			5	//�û���һ�ط���½

//OnMeetInfochanged(unsigned short gid, const void* data,unsigned long dataLen int roster_flag);
/* @param data    ������Ϣ������,˳��gid + FCNetAcMeetInfo���ݶ���˳��,��ɾ������ʱ���ֶ���Ч
* @param nRosterFlag �ı��־
* 	1:��ӻ���,2: ����ɾ�� 3: �����޸�
*/

//------------------------------------------
//�ص�����
//@param msgType     ��Ϣ����(����3������),��������MSGTYPE_OnLogOutʱ,����������Ч
//@param void*       �û��Զ���ָ��
//@param gid         ֻ��Ի���ģʽ,gid >=0 ʱ����,
//                          �������쳣,�˺ű�ĵط���½����ǻ�����Ա����״̬(һ��������״̬)�ı�ʱgid �����0; 
//							��Ա�������ߡ��������ӡ�ɾ�������޸ĵȵ�֪ͨ��gid�����ȱʡ����gid
//							������Ա�Ľ������������ݵĽ��ա�������Ա��״̬�Ĺ��� gid�����ʵ��gid
// @param dataType   ��������(1: �ش���ư� 2:�����ı���Ϣ 3:����Ƶ���ݰ� ) �������MSGTYPE_OnReceivedData ʱ����
//@param user_id     �û��˻�,�������MSGTYPE_OnReceivedData �� ����MSGTYPE_OnMemberRosterʱ��
//@param data	     ���յ������ݻ��û��Զ������ݣ��������MSGTYPE_OnReceivedData �� ����MSGTYPE_OnMemberRosterʱ��
//@param dataLen     ���յ������ݳ��� �� ��Ա�ı�ʱ��Ӧ��Ա�Զ������ݵĳ���
//@param pdu         ֻ�������MSGTYPE_OnReceivedDataʱ����
//@param roster_flag ֻ�������MSGTYPE_OnMemberRosterʱ���� ���庬���45�е��������
//@param user_attr   ֻ�������MSGTYPE_OnMemberRosterʱ����
//@return 0�ɹ���-1ʧ��
//------------------------------------------



typedef void (__stdcall *OnFCNetAcCallBack)(int msgType, void* pUserData,
							unsigned short gid, //ֻ��Ի���ģʽʱ����,>=0 ʱ��Ч,�������������
							unsigned short dataType,
							const char *user_id,const void* data, unsigned long dataLen,
							unsigned short pdu, //ֻ�����Ϣ������(MSGTYPE_OnReceivedData) ����
							int roster_flag, unsigned long user_attr);//ֻ�����Ϣ������(MSGTYPE_OnMemberRoster)����


typedef struct _FCNETACINFO
{
	char szAccount_[MAX_USER_ACCOUNT_LEN];	//ϵͳ�ʺ�
	char szPasswd_[MAX_USER_PASSWD_LEN];	//ϵͳ����
	char szServerIp[16];					//��������ַ
	unsigned short nLocalPort;				//���ض˿�
	unsigned short nType_;					//Ҫ����������ͨ������  ��8λ�е� ��1λtcp�ش����ݰ� ,��2λudp�����ı���Ϣ ,
													//��3λrtp1��Ƶ����,��4λ��Ƶ����  ��1111 ��ʾ�ĸ�ͨ����Ҫ����
	unsigned long  userAttr_;				//�û����� ��ͨ�û�-0; �ֻ��û�-1; pad�û�-2; ����û�-3; ��֯�û�(�Ŵ�,����)-4; ����Ա�û�-5
	OnFCNetAcCallBack pCallBack_;			//�ص�����
	unsigned long userDataLen_;
	void* userData_;						//�û�����
	void* pUserData_;						//�û��Զ���ָ��
}FCNetAcInfo;


//------------------------------------------
//�����ʼ��
//@return 0�ɹ� <0ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACInit();

//------------------------------------------
//���練��ʼ��
//@return 0�ɹ� <0ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACUninit();

//------------------------------------------
//ϵͳ����
//@note:��FCNetACInit�ɹ�֮����ܵ���
//@param user_id  �û���½���˻�
//@param user_flag �û��ϲ��ʶ
//@param user_data �û��Զ�������
//@param user_data_len
//@param OutTime �ȴ�ʱ�䣬Ĭ��Ϊ���޵ȴ����������Ĭ��ֵ��
//@return 0�ɹ� <0ʧ�� 
//   -1:��ʼ��δ�ɹ�; 1:�˿�ռ��; -4��ȡĬ�ϻ���ʧ��; -5 ����Ĭ�ϻ���ʧ��;-7 ��½��ʱ��9�ǵ�½ʱ���󷵻�ֵ��1;�����Ƿ��������ش���ֵ
//------------------------------------------

//FCNETAC_API int __stdcall FCNetACLogin(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = INFINITE);
FCNETAC_API int __stdcall FCNetACLogin(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = TIME_OUT_INFINITE);

//------------------------------------------
//ϵͳ�˳�
//@note:��FCNetACInit�ɹ�֮����ܵ���
//@return 0�ɹ� <0ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACLogout();


//------------------------------------------
//�������ݿ������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param pdu     ���͵����ݱ�ʶ
//in @param in_data ���͵�������Ϣ
//in @param in_Len   �������ݵĳ���
//in @param retPdu   ���յ������ݱ�ʶ
//out @param out_data ���յ���������Ϣ
//out @param out_Len   ���յ������ݵĳ���
//@return 0�ɹ���<0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,unsigned short& retPdu, const char*& out_data,unsigned long& out_Len);

//------------------------------------------
//����������������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param pdu     ���͵����ݱ�ʶ
//in @param data ���͵�������Ϣ
//in @param len   �������ݵĳ���
//@return 0�ɹ���<0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToServ(unsigned short pdu, const char *data, unsigned long len);


#endif	/*__FCNETAC_LIB_H__*/
