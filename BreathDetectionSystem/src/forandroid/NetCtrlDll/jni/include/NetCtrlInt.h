//NetCtrlInt.h-NetCtrlDll���ýӿ�ͷ�ļ�

#ifndef	__NETCTRLINT_H__
#define	__NETCTRLINT_H__


#include "IdDef.h"
#include <vector>
#include <map>


#define Debug_MyTrance_ 0
#define MAX_USER_PASSWORD_LEN   17

#if 0

#ifdef VER_FOR_ANDROID_

#include <jni.h>
#include <android/log.h>
#define PATH_SEPARATOR ':'
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#else

#define LOGI  printf
#endif

#endif

typedef std::vector<UserInfoEx> UserInfoExArray;  //�����߲�����


//class IBaseFuncComponent;

/** �������ݻص��ӿ�
*/
class INetACDataCallBack
{
public:
	/** �������ݻص�����
	* @param gid   ����id; id == 0 ��ʾ�ǻ������ݻص�; id > 0 �������ݻص�
	* @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
	* @param lpData �յ�������
	* @param dwSize ���ݳ���
	* @param wPduType PDU����
	* @param lpSender ������������Ϣ
	*/
    virtual int OnDataReceived(unsigned short gid,unsigned short dataType,const void * lpData, DWORD dwSize, WORD wPduType,const UserID* lpSender=NULL)=0;

	/** ��Ա�ı�ص�����
	* @param gid   ����id; id == 0 ��ʾ�ǻ����Ա�ص�; id > 0 �����Ա�ص�
	* notice  ����3�������쳣ʱ ����lpMemberUserΪNULL
	* @param nRosterFlag �ı��־
	* 1: ����   2: �뿪  3: �������쳣 4 ����״̬�ı� 
	* @param lpMemberUser �ı�ĳ�Ա������Ϣ
	*/
    virtual int OnMemberRoster(unsigned short gid,short nRosterFlag, const UserInfoEx* lpMemberUser)=0;
};


/** ͨ�ûص��ӿ�
*/
class IBaseCommCallBack
{
public:
	/** ��ȡ��֤��ص�����
	* @param nResult ���(0-�ɹ�,<0ʧ��)
	* @param dwKey У������
	* @param lpCode У����(����)
	* @param wCodeSize У���볤��
	*/
    virtual int OnUserRegister0(short nResult, DWORD dwKey, const BYTE* lpCode,
		WORD wCodeSize)=0;

	/** ���û�ע��ص�����
	* @param nResult ���(0-�ɹ�,<0ʧ��)
	* @param lpszAccount �û��ʺ�
	*/
    virtual int OnUserRegister1(short nResult, LPCTSTR lpszAccount)=0;

	/** ��¼�ص�����
	* @param nResult ���(0-�ɹ�,������ʧ��)
	* @param userInfo �û���Ϣ	
	*/
   // virtual int OnLogined(short nResult, const UserInfo& userInfo)=0;
	 virtual int OnLogined(short nResult, const UserInfoEx& userInfo)=0;

	/** �˳��ص�����
	* @param nResult ���(LOGOUT_TIMEOUT��ʱ LOGOUT_RELOG�ص���)
	*/
   // virtual int OnLogout(short nResult)=0;//��ʱƽ���Σ������ص� 

	//virtual void OnUserStatusChange(unsigned short gid, const UserID& user,unsigned long status)=0;

};

class IBaseCommCall;

#if 0
/** ��������������ýӿ�
 */
class IBaseFuncComponent
{
public:
	/** ��������
	* @param wPduType PDU����
	* @param lpData ����
	* @param dwSize ���ݳ���
	* @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
	* @param lpRecverArray ������(��ʱ����)
	* @return 0-�ɹ�, 1:��ʾ����,2: ��ʾ������δ����, 3: ��ʾp2pδ�����ɹ���ȴ�	
    */

    virtual int SendDataTo(WORD wPduType, const BYTE* lpData, DWORD dwSize, 
		short dataType, const USERArray* lpRecverArray=NULL, bool p2pFlag = false)=0;

	///**�������ݻص�ָ��
	//* @param pDataCallBack �ص�ָ��
	//*/
	virtual void SetACDataCallBack(INetACDataCallBack *pDataCallBack) = 0;	
};
#endif

/** ͨ�Ų���ýӿ�
 */
class IBaseCommCall 
{
public:
    /** ��ʼ��ͨ�Ŷ���(������֤������IP��ַ��ͨ�ûص��ӿ�)
	* @param rssAddr ת��������IP��ַ��Ϣ
	* @param lpCommCallBack ͨ�ûص��ӿ�(����ΪNULL)
	* @return 0-�ɹ�, <0ʧ��
    */
    virtual int NetACInitialize(const Addr& addr, 	IBaseCommCallBack * lpCommCallBack, USHORT nLocalPortBind=0)=0;

	/** �ͷ�ͨѶ����
	* @return 0-�ɹ�, <0ʧ��
	*/
	virtual int NetACUnInitialize()=0;


	/** ����:��ȡע����
	* @return 0-�ɹ�, <0ʧ��
	*/
    virtual int NetACRegister0(void)=0;

	/** ע�����û�
	* @param byVersionType �汾����
	* @param dwTopOrganID  ������֯ID
	* @param wRegType ע������(����Ϊ0)
	* @param dwKey ��֤���
	* @param lpszCode ��֤��(����)
	* @param lpRegInfo ע����Ϣ(����ΪNULL)
	* @param wRegInfoSize ע����Ϣ����(>0)
	* @return 0-�ɹ�, <0ʧ��
	*/
	virtual int NetACRegister1(BYTE byVersionType, DWORD dwTopOrganID, WORD wRegType, DWORD dwKey, LPCTSTR lpszCode, 
		const BYTE* lpRegInfo, WORD wRegInfoSize)=0;

	/** ϵͳ��¼
	* @param lpLoginInfo ��¼��Ϣ(����Ϊ��)
	* @param wLoginInfoSize ��¼��Ϣ����(>0)
	* @param lpExtLoginInfo ��չ��Ϣ(����Ϊ��)
	* @param wExtLoginInfoSize ��չ��Ϣ����(>0)
	* @param lpDataCallBack ���ݻص��ӿ�(����ΪNULL)
	* @return 0-�ɹ�, <0ʧ��
	*/ 
    virtual int NetACLogin(const BYTE* lpLoginInfo, WORD wLoginInfoSize,                    
		const BYTE* lpExtLoginInfo, WORD wExtLoginInfoSize, 
		INetACDataCallBack* lpDataCallBack)=0;

	/** �˳�ϵͳ
	* @return 0-�ɹ�, <0ʧ��
	*/
    virtual int NetACLogout(void)=0;       //zzzzzzzzzzzz ͬ������ �����ûص��ϲ�

	virtual int NetACSendDataToDB(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync=true )=0;
	virtual int NetACSendDataToServ(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync=false )=0;

};

/////<�����������
//typedef int (WINAPI *NETCTRLINIT)(void);  ///<��ʼ��NETCTRL
//typedef void (WINAPI *NETCTRLUNINIT)(void); ///<��ֹNETCTRL
//typedef IBaseCommCall* (WINAPI *CREATENETCTRLOBJ)(void); ///<����NETCTRL����
//typedef void (WINAPI *DESTROYNETCTRLOBJ)(IBaseCommCall* pMyCommCall); ///<ɾ��NETCTRL����

#endif
