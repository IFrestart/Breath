/**
* Copyright (c) 2004, HangZhou Webcon Corporation. 

* @file	 FCNetACLib.h
* @author	 zhoujj <zhoujj@webcon.com.cn>
* @class	FCNetACLib
**/

#ifndef	__FCNETAC_LIB_H__
#define	__FCNETAC_LIB_H__

#define __FCNET_API_DLLEXPORT__
#define __LINUX__
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
* 0:p2p����  1: ������ת������   2: �뿪  3: �������쳣 4 ����״̬�ı�
*/

#define MSGTYPE_OnLogOut				3	//�û��˳��ص�

//------------------------------------------
//�ص�����
//@param msgType     ��Ϣ����(����3������),��������MSGTYPE_OnLogOutʱ,����������Ч
//@param void*       �û��Զ���ָ��
//@param gid         ֻ��Ի���ģʽ,gid >0 ʱ����, 0ʱ����ֶκ���
// @param dataType   ��������(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�) �������MSGTYPE_OnReceivedData ʱ����
//@param user_id     �û��˻�,�������MSGTYPE_OnReceivedData �� ����MSGTYPE_OnMemberRosterʱ��
//@param data	     ���յ������ݻ��û��Զ������ݣ��������MSGTYPE_OnReceivedData �� ����MSGTYPE_OnMemberRosterʱ��
//@param dataLen     ���յ������ݳ��� �� ��Ա�ı�ʱ��Ӧ��Ա�Զ������ݵĳ���
//@param pdu         ֻ�������MSGTYPE_OnReceivedDataʱ����
//@param roster_flag ֻ�������MSGTYPE_OnMemberRosterʱ���� ���庬���45�е��������
//@param user_attr   ֻ�������MSGTYPE_OnMemberRosterʱ����
//@return 0�ɹ���-1ʧ��
//------------------------------------------



typedef void (__stdcall *OnFCNetAcCallBack)(int msgType, void* pUserData,
											unsigned short gid, //ֻ��Ի���ģʽʱ����,>0 ʱ��Ч,�������������
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
	unsigned long  userAttr_;				//�û�����    ��16λ�е� ��1λ��־����Ա�û� , ��2λ���ݿ��û�   ����λ�����ʾ��ͨ�û� 
	OnFCNetAcCallBack pCallBack_;			//�ص�����
	unsigned long userDataLen_;
	void* userData_;						//�û�����
	void* pUserData_;						//�û��Զ���ָ��
}FCNetAcInfo;


struct FCNetAcMeetInfo
{
public:
	FCNetAcMeetInfo(): status_(0), flag_(0),maxUserNum_(0), funcType_(0),
			corpId_(0), meetingType_(0),dataLen_(0), lpdata_(0)
	{
		memset(&passwd_,0,sizeof(passwd_));
		memset(&hostPasswd_,0,sizeof(hostPasswd_));
		memset(&name_,0,sizeof(name_));
		memset(&introduce_,0,sizeof(introduce_));
		memset(&adminCode_,0,sizeof(adminCode_));
	}

	~FCNetAcMeetInfo()
	{
	}

	FCNetAcMeetInfo(const FCNetAcMeetInfo& that)
	{
		assign(that);
	}
	FCNetAcMeetInfo& operator= (const FCNetAcMeetInfo& that)
	{
		if(this != &that)
		{
			if (lpdata_)
			{
				delete[] lpdata_;
			}			
			lpdata_ = 0;
			assign(that);
		}
		return (*this);
	}
	void delLpData()
	{
		if (lpdata_)
		{
			delete[] lpdata_;
		}		
		lpdata_ = 0;
	}

protected:
	void assign(const FCNetAcMeetInfo& that)
	{
		status_		= that.status_;
		flag_		= that.flag_;
		maxUserNum_	= that.maxUserNum_;
		funcType_	= that.funcType_;
		strcpy(passwd_,that.passwd_);
		//passwd_		= that.passwd_;
		strcpy(hostPasswd_,that.hostPasswd_);
		//hostPasswd_	= that.hostPasswd_;
		strcpy(name_,that.name_);
		//name_		= that.name_;
		corpId_		= that.corpId_;
		meetingType_	= that.meetingType_;
		strcpy(introduce_,that.introduce_);
	//	introduce_	= that.introduce_;
		//adminCode_	= that.adminCode_;
		strcpy(adminCode_,that.adminCode_);
		dataLen_	= that.dataLen_;
		if (dataLen_)
		{
			lpdata_ 	= new char[that.dataLen_];
			//	assert(lpdata_);
			memcpy(lpdata_, that.lpdata_, that.dataLen_);
		}		
	}
public:
	unsigned long	status_;		/**< Group��״̬λ*/
	unsigned long	flag_;			/**< Group�ı�־λ*/ //��8λ�е� ��1λtcp�ش����ݰ� ,��2λudp�����ı���Ϣ ,
		//��3λrtp1��Ƶ����,��4λ��Ƶ����  ��1111 ��ʾ�ĸ�ͨ����Ҫ����
	unsigned short	maxUserNum_;		/**< Group������û���*/
	unsigned long	funcType_;		/**< Group�����ͣ��ͻ���ʹ��*/
	char			passwd_[16];		/**< Group�Ľ�������*/
	char			hostPasswd_[16];		/**< Group������������*/
	char			name_[64];		/**< Group������*/
	unsigned long	corpId_;		/**< Group���ڵ���ҵ���*/
	unsigned short	meetingType_;   /* 1 ��ʾ����ҵ��ȱʡ���� */
	char			introduce_[255];		/**< Group�Ľ��� */
	char			adminCode_[16];		/**< Group�Ĺ���Ա��ţ��ͻ���ʹ�� */
	unsigned short	dataLen_;		/**< Group���Զ������ݳ��� */
	char*		lpdata_;		/**< Group���Զ������� */
};
;

//typedef struct _FCNETACMEETINFO
//{
//	unsigned long	status_;		/**< Group��״̬λ*/
//	unsigned long	flag_;			/**< Group�ı�־λ*/ //��8λ�е� ��1λtcp�ش����ݰ� ,��2λudp�����ı���Ϣ ,
//	//��3λrtp1��Ƶ����,��4λ��Ƶ����  ��1111 ��ʾ�ĸ�ͨ����Ҫ����
//	unsigned short	maxUserNum_;		/**< Group������û���*/
//	unsigned long	funcType_;		/**< Group�����ͣ��ͻ���ʹ��*/
//	std::string		passwd_;		/**< Group�Ľ�������*/
//	std::string		hostPasswd_;		/**< Group������������*/
//	std::string		name_;		/**< Group������*/
//	unsigned long	corpId_;		/**< Group���ڵ���ҵ���*/
//	unsigned short	meetingType_;	/* 1 ��ʾ����ҵ��ȱʡ���� */
//	std::string		introduce_;		/**< Group�Ľ��� */
//	std::string		adminCode_;		/**< Group�Ĺ���Ա��ţ��ͻ���ʹ�� */
//	unsigned short	dataLen_;		/**< Group���Զ������ݳ��� */
//	char*		lpdata_;		/**< Group���Զ������� */
//}FCNetAcMeetInfo;


struct FCNetAcMeetInfo2 : public FCNetAcMeetInfo
{
	FCNetAcMeetInfo2() : gid_(0),FCNetAcMeetInfo()
	{
	}

	~FCNetAcMeetInfo2()
	{
	}

	FCNetAcMeetInfo2(const FCNetAcMeetInfo2& that)
	{
		gid_ = that.gid_;
		assign(that);
	}

	FCNetAcMeetInfo2& operator= (const FCNetAcMeetInfo2& that)
	{
		if(this != &that)
		{
			if (lpdata_)
			{
				delete[] lpdata_;
			}			
			lpdata_ = 0;
			gid_ = that.gid_;
			assign(that);
		}
		return (*this);
	}

public:
	unsigned short	gid_;
};

typedef struct _FRIENDUSERSTATUS
{
	std::string  name_;
	unsigned long status_;
	unsigned long flag_;
}FriendUserStatus;

typedef std::vector<FriendUserStatus >  FrindUserStatusArray;

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
//��������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
// @param gid   ��Դid,�ǵ�Ե㷢��ʱ����,��gid = 0ʱ ��Ĭ����Դ����,��gid != 0ʱֻ����FCNetACJoinMeet�ɹ�����֮����Ч
// @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
//@param user_id  ���͵�Ŀ���û��˻�
//@param data     ���͵�������Ϣ
//@param dataLen  �������ݵĳ���
//@param p2pFlag  true ��ʾ ͨ��p2p���ͣ�false ��ʾͨ��ת�����������ͣ�Ĭ��Ϊtrue
//@return 0�ɹ���<0 ʧ��, 3 ���ڽ���ͨ�� �Ժ��ط�
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendData(unsigned short gid,unsigned short dataType,const char *user_id, unsigned short pdu, const char *data, unsigned int data_len,bool p2pFlag = true);

//------------------------------------------
//��������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
// @param gid   ��Դid,�ǵ�Ե㷢��ʱ����,��gid = 0ʱ ��Ĭ����Դ����,��gid != 0ʱֻ����FCNetACJoinMeet�ɹ�����֮����Ч
// @param dataType ���ͷ�ʽ(1: �ش���ư� 2:�����ı���Ϣ 3:��Ƶ���ݰ� 4: ��Ƶ���ݰ�)
//@param nNum     Ҫ�����û��ĸ���
//@param user_id  ���͵�����Ŀ���û��˻�
//@param data     ���͵�������Ϣ
//@param dataLen  �������ݵĳ���
//@param p2pFlag  true ��ʾ ͨ��p2p���ͣ�false ��ʾͨ��ת�����������ͣ�Ĭ��Ϊtrue
//@return 0�ɹ���<0 ʧ��, 3 ���ڽ���ͨ�� �Ժ��ط�
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendData2(unsigned short gid,unsigned short dataType,unsigned short nNum,const char **user_id, unsigned short pdu, const char *data, unsigned int data_len,bool p2pFlag = true);


//------------------------------------------
//��������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param pdu     ���͵����ݱ�ʶ
//in @param in_data ���͵�������Ϣ
//in @param in_Len   �������ݵĳ���
//out @param out_data ���͵�������Ϣ
//out @param out_Len   �������ݵĳ���
//@return 0�ɹ���<0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,char*& out_data,unsigned long& out_Len);



//------------------------------------------
//��ȡ�û��б�			
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param attr      Ҫ��ȡ���û�����
//in @param inPageNo  �ӵڼ�ҳ��ʼ
//in @param inUserNum ���ȡ�ĸ���
//out @param outTotalPageNum �����ܹ���ҳ��
//out @param userList  ���ص��û��б�
//@return >= 0 �ɹ����ص��û�������< 0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACGetUserList(unsigned long attr,unsigned short inPageNo, int inUserNum,unsigned short& outTotalPageNum,char** userList);


//��ȡ��ָ���û�����ͨ����ģʽ
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//@return <0 ʧ��, >0 ������ͨ��ģʽ   ��6λ�ľ��庬�壺
//										��1λ ��1��ʾ����û�tcpͨ�������ɹ� ��4λ��0��ʾ����û���������p2pģʽ��tcpͨ��
//										  									 ��4λ��1��ʾ����û��������Ƿ�����ת��ģʽ��tcpͨ��	
//										��2λ ��1��ʾ����û�udpͨ�������ɹ� ��5λ��0��ʾ����û���������p2pģʽ��udpͨ��
//																			 ��5λ��1��ʾ����û��������Ƿ�����ת��ģʽ��udpͨ��	
//										��3λ ��1��ʾ����û�rtpͨ�������ɹ� ��6λ��0��ʾ����û���������p2pģʽ��rtpͨ��
//																			 ��6λ��1��ʾ����û��������Ƿ�����ת��ģʽ��rtpͨ��					
//------------------------------------------
//FCNETAC_API int __stdcall FCNetACGetChunnelMode(const char* szAccount);


//����������			
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param meetinfo      Ҫ�����Ļ�����Ϣ
//@return > 0 �ɹ����صĻ���id  , < 0 ʧ��
FCNETAC_API short __stdcall FCNetACCreateMeet(const FCNetAcMeetInfo& meetinfo,unsigned long OutTime = TIME_OUT_INFINITE);

//����������			
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in @param gid       Ҫ����Ļ���
//in @param passwd    Ҫ����Ļ��������
//@return == 0 �ɹ�  , < 0 ʧ��
FCNETAC_API int __stdcall FCNetACJoinMeet(unsigned short gid,char* passwd,unsigned long OutTime = TIME_OUT_INFINITE);

//�뿪������			
//@note:��FCNetACJoinMeet�ɹ�֮����ܵ���
//in @param gid       Ҫ�뿪�Ļ���
//@return == 0 �ɹ�  , < 0 ʧ��
FCNETAC_API int __stdcall FCNetACLeaveMeet(unsigned short gid,unsigned long OutTime = TIME_OUT_INFINITE);


//------------------------------------------
//��ȡ�����б�			
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//@note meetList�Ĵ�С�ɵ��÷�����ǰ���趨��
//in @param meetList ���÷��صĻ����б�
//@return >= 0 �ɹ����صĻ����������< 0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACGetMeetingList(std::vector<FCNetAcMeetInfo2* >& meetList);
//------------------------------------------
//��Ӻ���
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in  @param friendList  Ҫ��ӵĺ����б�
//@return == 0 �ɹ����� , < 0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACAddFriendUserInfo(const std::vector<std::string >& friendList);


//------------------------------------------
//ɾ������
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//in  @param friendList  Ҫɾ���ĺ����б�
//@return == 0 �ɹ����� , < 0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACRemoveFriendUserInfo(const std::vector<std::string >& friendList);


//------------------------------------------
//��ѯ�����û�������Ϣ
//@note:��FCNetACLogin�ɹ�֮����ܵ���
//@note out_FriendList�Ĵ�С�ɵ��÷�����ǰ���趨��,����in_FriendList��Сһ��
//in  @param in_FriendList  Ҫ��ѯ�ĺ����б�
//out @param out_FriendList ����Ҫ��ѯ�����б�Ľ��
//@return == 0 �ɹ����� , < 0 ʧ��
//------------------------------------------
FCNETAC_API int __stdcall FCNetACFindFriendUserOnLineInfo(const std::vector<std::string >& in_FriendList,
														  FrindUserStatusArray& out_FriendList);

#endif	/*__FCNETAC_LIB_H__*/
