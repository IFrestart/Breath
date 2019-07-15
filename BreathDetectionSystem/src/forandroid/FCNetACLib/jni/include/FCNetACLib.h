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

#define MSGTYPE_OnReceivedData			1	//收到数据回调函数
//OnReceivedData(const char *user_id, const void *data, unsigned long dataLen,unsigned short pdu,)

#define MSGTYPE_OnMemberRoster			2	//成员改变回调函数
//OnMemberRoster(const char *user_id, void* data, unsigned long dataLen,int roster_flag, unsigned long user_attr);
/** 成员改变回调函数
* @param nRosterFlag 改变标志
* 0:p2p加入  1: 服务器转发加入   2: 离开  3: 服务器异常 4 好友状态改变  5 账号另一地方登陆
* @param user_attr 改变标志
*         低16位中的高8为 0:正常  1:主讲  2: 可以发言  3: 禁止发言 4:处于申请发言状态
*/

#define MSGTYPE_OnLogOut				3	//用户退出回调

#define MSGTYPE_OnMeetInfoChanged			4	//会议信息的更改
#define MSGTYPE_OnSecondLogined			5	//用户另一地方登陆

//OnMeetInfochanged(unsigned short gid, const void* data,unsigned long dataLen int roster_flag);
/* @param data    会议信息数据流,顺序按gid + FCNetAcMeetInfo数据定义顺序,当删除会议时此字段无效
* @param nRosterFlag 改变标志
* 	1:添加会议,2: 会议删除 3: 会议修改
*/

//------------------------------------------
//回调函数
//@param msgType     消息类型(上面3种类型),当类型是MSGTYPE_OnLogOut时,其他参数无效
//@param void*       用户自定义指针
//@param gid         只针对会议模式,gid >=0 时有用,
//                          服务器异常,账号别的地方登陆、或非会议人员好友状态(一般是网络状态)改变时gid 填的是0; 
//							人员的上下线、会议的添加、删除、和修改等的通知，gid填的是缺省会议gid
//							会议人员的进出、会议数据的接收、会议人员的状态改过等 gid填的是实际gid
// @param dataType   数据类型(1: 必达控制包 2:聊天文本信息 3:音视频数据包 ) 针对类型MSGTYPE_OnReceivedData 时有用
//@param user_id     用户账户,针对类型MSGTYPE_OnReceivedData 或 类型MSGTYPE_OnMemberRoster时用
//@param data	     接收到的数据或用户自定义数据，针对类型MSGTYPE_OnReceivedData 或 类型MSGTYPE_OnMemberRoster时用
//@param dataLen     接收到的数据长度 或 成员改变时相应成员自定义数据的长度
//@param pdu         只针对类型MSGTYPE_OnReceivedData时有用
//@param roster_flag 只针对类型MSGTYPE_OnMemberRoster时有用 具体含义见45行的上面解释
//@param user_attr   只针对类型MSGTYPE_OnMemberRoster时有用
//@return 0成功，-1失败
//------------------------------------------



typedef void (__stdcall *OnFCNetAcCallBack)(int msgType, void* pUserData,
							unsigned short gid, //只针对会议模式时有用,>=0 时生效,结合其他参数用
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
	unsigned long  userAttr_;				//用户类型 普通用户-0; 手机用户-1; pad用户-2; 监控用户-3; 组织用户(信贷,分行)-4; 管理员用户-5
	OnFCNetAcCallBack pCallBack_;			//回调函数
	unsigned long userDataLen_;
	void* userData_;						//用户数据
	void* pUserData_;						//用户自定义指针
}FCNetAcInfo;


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
//   -1:初始化未成功; 1:端口占用; -4获取默认会议失败; -5 加入默认会议失败;-7 登陆超时；9是登陆时错误返回值是1;其它是服务器返回错误值
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
//发往数据库的数据
//@note:在FCNetACLogin成功之后才能调用
//in @param pdu     发送的数据标识
//in @param in_data 发送的数据信息
//in @param in_Len   发送数据的长度
//in @param retPdu   接收到的数据标识
//out @param out_data 接收到的数据信息
//out @param out_Len   接收到的数据的长度
//@return 0成功，<0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToDB(unsigned short pdu, const char *in_data, unsigned long in_Len,unsigned short& retPdu, const char*& out_data,unsigned long& out_Len);

//------------------------------------------
//发往服务器的数据
//@note:在FCNetACLogin成功之后才能调用
//in @param pdu     发送的数据标识
//in @param data 发送的数据信息
//in @param len   发送数据的长度
//@return 0成功，<0 失败
//------------------------------------------
FCNETAC_API int __stdcall FCNetACSendDataToServ(unsigned short pdu, const char *data, unsigned long len);


#endif	/*__FCNETAC_LIB_H__*/
