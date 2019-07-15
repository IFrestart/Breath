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

#define MSGTYPE_OnReceivedData			1	//收到数据回调函数
//OnReceivedData(const char *user_id, const void *data, unsigned long dataLen,unsigned short pdu,)

#define MSGTYPE_OnMemberRoster			2	//成员改变回调函数
//OnMemberRoster(const char *user_id, void* data, unsigned long dataLen,int roster_flag, unsigned long user_attr);
/** 成员改变回调函数
* @param nRosterFlag 改变标志
* 0:p2p加入  1: 服务器转发加入   2: 离开  3: 服务器异常 4 好友状态改变
*/

#define MSGTYPE_OnLogOut				3	//用户退出回调

//------------------------------------------
//回调函数
//@param msgType     消息类型(上面3种类型),当类型是MSGTYPE_OnLogOut时,其他参数无效
//@param void*       用户自定义指针
//@param gid         只针对会议模式,gid >0 时有用, 0时表该字段忽略
// @param dataType   数据类型(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包) 针对类型MSGTYPE_OnReceivedData 时有用
//@param user_id     用户账户,针对类型MSGTYPE_OnReceivedData 或 类型MSGTYPE_OnMemberRoster时用
//@param data	     接收到的数据或用户自定义数据，针对类型MSGTYPE_OnReceivedData 或 类型MSGTYPE_OnMemberRoster时用
//@param dataLen     接收到的数据长度 或 成员改变时相应成员自定义数据的长度
//@param pdu         只针对类型MSGTYPE_OnReceivedData时有用
//@param roster_flag 只针对类型MSGTYPE_OnMemberRoster时有用 具体含义见45行的上面解释
//@param user_attr   只针对类型MSGTYPE_OnMemberRoster时有用
//@return 0成功，-1失败
//------------------------------------------



typedef void (__stdcall *OnFCNetAcCallBack)(int msgType, void* pUserData,
											unsigned short gid, //只针对会议模式时有用,>0 时生效,结合其他参数用
											unsigned short dataType,
											const char *user_id,const void* data, unsigned long dataLen,
											unsigned short pdu, //只针对消息类型是(MSGTYPE_OnReceivedData) 有用
											int roster_flag, unsigned long user_attr);//只针对消息类型是(MSGTYPE_OnMemberRoster)有用


typedef struct _FCNETACINFO
{
	char szAccount_[MAX_USER_ACCOUNT_LEN];	//系统帐号
	char szPasswd_[MAX_USER_PASSWD_LEN];	//系统密码
	char szServerIp[16];					//服务器地址
	unsigned short nLocalPort;				//本地端口
	unsigned short nType_;					//要建立的数据通道类型  低8位中的 低1位tcp必达数据包 ,低2位udp聊天文本信息 ,
													//低3位rtp1音频数据,低4位视频数据  如1111 表示四个通道都要建立
	unsigned long  userAttr_;				//用户属性    高16位中的 低1位日志管理员用户 , 低2位数据库用户   这两位是零表示普通用户 
	OnFCNetAcCallBack pCallBack_;			//回调函数
	unsigned long userDataLen_;
	void* userData_;						//用户数据
	void* pUserData_;						//用户自定义指针
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
	unsigned long	status_;		/**< Group的状态位*/
	unsigned long	flag_;			/**< Group的标志位*/ //低8位中的 低1位tcp必达数据包 ,低2位udp聊天文本信息 ,
		//低3位rtp1音频数据,低4位视频数据  如1111 表示四个通道都要建立
	unsigned short	maxUserNum_;		/**< Group的最大用户数*/
	unsigned long	funcType_;		/**< Group的类型，客户端使用*/
	char			passwd_[16];		/**< Group的进入密码*/
	char			hostPasswd_[16];		/**< Group的主持人密码*/
	char			name_[64];		/**< Group的名字*/
	unsigned long	corpId_;		/**< Group所在的企业编号*/
	unsigned short	meetingType_;   /* 1 表示该企业的缺省会议 */
	char			introduce_[255];		/**< Group的介绍 */
	char			adminCode_[16];		/**< Group的管理员编号，客户端使用 */
	unsigned short	dataLen_;		/**< Group的自定义数据长度 */
	char*		lpdata_;		/**< Group的自定义数据 */
};
;

//typedef struct _FCNETACMEETINFO
//{
//	unsigned long	status_;		/**< Group的状态位*/
//	unsigned long	flag_;			/**< Group的标志位*/ //低8位中的 低1位tcp必达数据包 ,低2位udp聊天文本信息 ,
//	//低3位rtp1音频数据,低4位视频数据  如1111 表示四个通道都要建立
//	unsigned short	maxUserNum_;		/**< Group的最大用户数*/
//	unsigned long	funcType_;		/**< Group的类型，客户端使用*/
//	std::string		passwd_;		/**< Group的进入密码*/
//	std::string		hostPasswd_;		/**< Group的主持人密码*/
//	std::string		name_;		/**< Group的名字*/
//	unsigned long	corpId_;		/**< Group所在的企业编号*/
//	unsigned short	meetingType_;	/* 1 表示该企业的缺省会议 */
//	std::string		introduce_;		/**< Group的介绍 */
//	std::string		adminCode_;		/**< Group的管理员编号，客户端使用 */
//	unsigned short	dataLen_;		/**< Group的自定义数据长度 */
//	char*		lpdata_;		/**< Group的自定义数据 */
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
//网络初始化
//@return 0成功 <0失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACInit();

//------------------------------------------
//网络反初始化
//@return 0成功 <0失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACUninit();

//------------------------------------------
//系统登入
//@note:在FCNetACInit成功之后才能调用
//@param user_id  用户登陆的账户
//@param user_flag 用户上层标识
//@param user_data 用户自定义数据
//@param user_data_len
//@param OutTime 等待时间，默认为无限等待（建议采用默认值）
//@return 0成功 <0失败
//------------------------------------------

//FCNETAC_API int __stdcall FCNetACLogin(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = INFINITE);
FCNETAC_API int __stdcall FCNetACLogin(const FCNetAcInfo& fcNetAcInfo,unsigned long OutTime = TIME_OUT_INFINITE);

//------------------------------------------
//系统退出
//@note:在FCNetACInit成功之后才能调用
//@return 0成功 <0失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACLogout();

//------------------------------------------
//发送数据
//@note:在FCNetACLogin成功之后才能调用
// @param gid   资源id,非点对点发送时有用,当gid = 0时 用默认资源发送,当gid != 0时只有在FCNetACJoinMeet成功调用之后有效
// @param dataType 发送方式(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包)
//@param user_id  发送的目标用户账户
//@param data     发送的数据信息
//@param dataLen  发送数据的长度
//@param p2pFlag  true 表示 通过p2p发送，false 表示通过转发服务器发送，默认为true
//@return 0成功，<0 失败, 3 正在建立通道 稍后重发
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendData(unsigned short gid,unsigned short dataType,const char *user_id, unsigned short pdu, const char *data, unsigned int data_len,bool p2pFlag = true);

//------------------------------------------
//发送数据
//@note:在FCNetACLogin成功之后才能调用
// @param gid   资源id,非点对点发送时有用,当gid = 0时 用默认资源发送,当gid != 0时只有在FCNetACJoinMeet成功调用之后有效
// @param dataType 发送方式(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包)
//@param nNum     要发送用户的个数
//@param user_id  发送的所有目标用户账户
//@param data     发送的数据信息
//@param dataLen  发送数据的长度
//@param p2pFlag  true 表示 通过p2p发送，false 表示通过转发服务器发送，默认为true
//@return 0成功，<0 失败, 3 正在建立通道 稍后重发
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendData2(unsigned short gid,unsigned short dataType,unsigned short nNum,const char **user_id, unsigned short pdu, const char *data, unsigned int data_len,bool p2pFlag = true);


//------------------------------------------
//发送数据
//@note:在FCNetACLogin成功之后才能调用
//in @param pdu     发送的数据标识
//in @param in_data 发送的数据信息
//in @param in_Len   发送数据的长度
//out @param out_data 发送的数据信息
//out @param out_Len   发送数据的长度
//@return 0成功，<0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,char*& out_data,unsigned long& out_Len);



//------------------------------------------
//获取用户列表			
//@note:在FCNetACLogin成功之后才能调用
//in @param attr      要获取的用户属性
//in @param inPageNo  从第几页开始
//in @param inUserNum 想获取的个数
//out @param outTotalPageNum 返回总共的页数
//out @param userList  返回的用户列表
//@return >= 0 成功返回的用户个数，< 0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACGetUserList(unsigned long attr,unsigned short inPageNo, int inUserNum,unsigned short& outTotalPageNum,char** userList);


//获取与指定用户建立通道的模式
//@note:在FCNetACLogin成功之后才能调用
//@return <0 失败, >0 建立的通道模式   低6位的具体含义：
//										第1位 置1表示与该用户tcp通道建立成功 第4位置0表示与该用户建立的是p2p模式的tcp通道
//										  									 第4位置1表示与该用户建立的是服务器转发模式的tcp通道	
//										第2位 置1表示与该用户udp通道建立成功 第5位置0表示与该用户建立的是p2p模式的udp通道
//																			 第5位置1表示与该用户建立的是服务器转发模式的udp通道	
//										第3位 置1表示与该用户rtp通道建立成功 第6位置0表示与该用户建立的是p2p模式的rtp通道
//																			 第6位置1表示与该用户建立的是服务器转发模式的rtp通道					
//------------------------------------------
//FCNETAC_API int __stdcall FCNetACGetChunnelMode(const char* szAccount);


//创建讨论室			
//@note:在FCNetACLogin成功之后才能调用
//in @param meetinfo      要创建的会议信息
//@return > 0 成功返回的会议id  , < 0 失败
FCNETAC_API short __stdcall FCNetACCreateMeet(const FCNetAcMeetInfo& meetinfo,unsigned long OutTime = TIME_OUT_INFINITE);

//加入讨论室			
//@note:在FCNetACLogin成功之后才能调用
//in @param gid       要加入的会议
//in @param passwd    要加入的会议的密码
//@return == 0 成功  , < 0 失败
FCNETAC_API int __stdcall FCNetACJoinMeet(unsigned short gid,char* passwd,unsigned long OutTime = TIME_OUT_INFINITE);

//离开讨论室			
//@note:在FCNetACJoinMeet成功之后才能调用
//in @param gid       要离开的会议
//@return == 0 成功  , < 0 失败
FCNETAC_API int __stdcall FCNetACLeaveMeet(unsigned short gid,unsigned long OutTime = TIME_OUT_INFINITE);


//------------------------------------------
//获取会议列表			
//@note:在FCNetACLogin成功之后才能调用
//@note meetList的大小由调用方调用前就设定好
//in @param meetList 调用返回的会议列表
//@return >= 0 成功返回的会议个数数，< 0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACGetMeetingList(std::vector<FCNetAcMeetInfo2* >& meetList);
//------------------------------------------
//添加好友
//@note:在FCNetACLogin成功之后才能调用
//in  @param friendList  要添加的好友列表
//@return == 0 成功返回 , < 0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACAddFriendUserInfo(const std::vector<std::string >& friendList);


//------------------------------------------
//删除好友
//@note:在FCNetACLogin成功之后才能调用
//in  @param friendList  要删除的好友列表
//@return == 0 成功返回 , < 0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACRemoveFriendUserInfo(const std::vector<std::string >& friendList);


//------------------------------------------
//查询好友用户在线信息
//@note:在FCNetACLogin成功之后才能调用
//@note out_FriendList的大小由调用方调用前就设定好,并与in_FriendList大小一致
//in  @param in_FriendList  要查询的好友列表
//out @param out_FriendList 返回要查询好友列表的结果
//@return == 0 成功返回 , < 0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACFindFriendUserOnLineInfo(const std::vector<std::string >& in_FriendList,
														  FrindUserStatusArray& out_FriendList);

#endif	/*__FCNETAC_LIB_H__*/
