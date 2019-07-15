//NetCtrlInt.h-NetCtrlDll调用接口头文件

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

typedef std::vector<UserInfoEx> UserInfoExArray;  //供在线查找用


//class IBaseFuncComponent;

/** 接收数据回调接口
*/
class INetACDataCallBack
{
public:
	/** 接收数据回调函数
	* @param gid   会议id; id == 0 表示非会议数据回调; id > 0 会议数据回调
	* @param dataType 发送方式(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包)
	* @param lpData 收到的数据
	* @param dwSize 数据长度
	* @param wPduType PDU类型
	* @param lpSender 发送者索引信息
	*/
    virtual int OnDataReceived(unsigned short gid,unsigned short dataType,const void * lpData, DWORD dwSize, WORD wPduType,const UserID* lpSender=NULL)=0;

	/** 成员改变回调函数
	* @param gid   会议id; id == 0 表示非会议成员回调; id > 0 会议成员回调
	* notice  当是3服务器异常时 参数lpMemberUser为NULL
	* @param nRosterFlag 改变标志
	* 1: 加入   2: 离开  3: 服务器异常 4 好友状态改变 
	* @param lpMemberUser 改变的成员索引信息
	*/
    virtual int OnMemberRoster(unsigned short gid,short nRosterFlag, const UserInfoEx* lpMemberUser)=0;
};


/** 通用回调接口
*/
class IBaseCommCallBack
{
public:
	/** 获取认证码回调函数
	* @param nResult 结果(0-成功,<0失败)
	* @param dwKey 校验码编号
	* @param lpCode 校验码(密文)
	* @param wCodeSize 校验码长度
	*/
    virtual int OnUserRegister0(short nResult, DWORD dwKey, const BYTE* lpCode,
		WORD wCodeSize)=0;

	/** 新用户注册回调函数
	* @param nResult 结果(0-成功,<0失败)
	* @param lpszAccount 用户帐号
	*/
    virtual int OnUserRegister1(short nResult, LPCTSTR lpszAccount)=0;

	/** 登录回调函数
	* @param nResult 结果(0-成功,其他表失败)
	* @param userInfo 用户信息	
	*/
   // virtual int OnLogined(short nResult, const UserInfo& userInfo)=0;
	 virtual int OnLogined(short nResult, const UserInfoEx& userInfo)=0;

	/** 退出回调函数
	* @param nResult 结果(LOGOUT_TIMEOUT超时 LOGOUT_RELOG重登入)
	*/
   // virtual int OnLogout(short nResult)=0;//暂时平屏蔽，不作回调 

	//virtual void OnUserStatusChange(unsigned short gid, const UserID& user,unsigned long status)=0;

};

class IBaseCommCall;

#if 0
/** 发送数据组件调用接口
 */
class IBaseFuncComponent
{
public:
	/** 发送数据
	* @param wPduType PDU类型
	* @param lpData 数据
	* @param dwSize 数据长度
	* @param dataType 发送方式(1: 必达控制包 2:聊天文本信息 3:音频数据包 4: 视频数据包)
	* @param lpRecverArray 接收者(暂时保留)
	* @return 0-成功, 1:表示出错,2: 表示服务器未就绪, 3: 表示p2p未建立成功需等待	
    */

    virtual int SendDataTo(WORD wPduType, const BYTE* lpData, DWORD dwSize, 
		short dataType, const USERArray* lpRecverArray=NULL, bool p2pFlag = false)=0;

	///**设置数据回调指针
	//* @param pDataCallBack 回调指针
	//*/
	virtual void SetACDataCallBack(INetACDataCallBack *pDataCallBack) = 0;	
};
#endif

/** 通信层调用接口
 */
class IBaseCommCall 
{
public:
    /** 初始化通信对象(设置认证服务器IP地址和通用回调接口)
	* @param rssAddr 转发服务器IP地址信息
	* @param lpCommCallBack 通用回调接口(不能为NULL)
	* @return 0-成功, <0失败
    */
    virtual int NetACInitialize(const Addr& addr, 	IBaseCommCallBack * lpCommCallBack, USHORT nLocalPortBind=0)=0;

	/** 释放通讯对象
	* @return 0-成功, <0失败
	*/
	virtual int NetACUnInitialize()=0;


	/** 功能:获取注册码
	* @return 0-成功, <0失败
	*/
    virtual int NetACRegister0(void)=0;

	/** 注册新用户
	* @param byVersionType 版本类型
	* @param dwTopOrganID  顶层组织ID
	* @param wRegType 注册类型(现在为0)
	* @param dwKey 验证编号
	* @param lpszCode 验证码(明文)
	* @param lpRegInfo 注册信息(不能为NULL)
	* @param wRegInfoSize 注册信息长度(>0)
	* @return 0-成功, <0失败
	*/
	virtual int NetACRegister1(BYTE byVersionType, DWORD dwTopOrganID, WORD wRegType, DWORD dwKey, LPCTSTR lpszCode, 
		const BYTE* lpRegInfo, WORD wRegInfoSize)=0;

	/** 系统登录
	* @param lpLoginInfo 登录信息(不能为空)
	* @param wLoginInfoSize 登录信息长度(>0)
	* @param lpExtLoginInfo 扩展信息(不能为空)
	* @param wExtLoginInfoSize 扩展信息长度(>0)
	* @param lpDataCallBack 数据回调接口(不能为NULL)
	* @return 0-成功, <0失败
	*/ 
    virtual int NetACLogin(const BYTE* lpLoginInfo, WORD wLoginInfoSize,                    
		const BYTE* lpExtLoginInfo, WORD wExtLoginInfoSize, 
		INetACDataCallBack* lpDataCallBack)=0;

	/** 退出系统
	* @return 0-成功, <0失败
	*/
    virtual int NetACLogout(void)=0;       //zzzzzzzzzzzz 同步调用 并不用回调上层

	virtual int NetACSendDataToDB(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync=true )=0;
	virtual int NetACSendDataToServ(unsigned short subPdu,const void* lpData,unsigned long dwSize,bool bSync=false )=0;

};

/////<输出函数类型
//typedef int (WINAPI *NETCTRLINIT)(void);  ///<初始化NETCTRL
//typedef void (WINAPI *NETCTRLUNINIT)(void); ///<终止NETCTRL
//typedef IBaseCommCall* (WINAPI *CREATENETCTRLOBJ)(void); ///<创建NETCTRL对象
//typedef void (WINAPI *DESTROYNETCTRLOBJ)(IBaseCommCall* pMyCommCall); ///<删除NETCTRL对象

#endif
