#include "ClientNativeInterface.h"
#include <jni.h>

#include "SockStream.h"

#include "com_webcon_wp_utils_NativeInterface.h"

#define _FOR_PC_VER 	1

#if _FOR_PC_VER
#define LOGI  printf
#else
#include <android/log.h>
#define PATH_SEPARATOR ':' 
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#endif


CLock		g_userLock;
bool        	g_doWork = true;
JNIEnv 		*g_javaEnv = NULL;
jclass g_javaCls = 0;;
jmethodID g_javaMid = 0;
jfieldID g_fid = 0;
jobject g_obj = 0;
jbyteArray g_arg; 

jclass g_javaCls2 = 0;;
jmethodID g_javaMid2 = 0;

int stBufferManage::pushData(stTransUnit& atransunit,const void* data, unsigned long len)
{
	int bufferid = -1;
	mutexBuffreeList_.lock();
	if(m_BuffreeList.size() > 0)
	{
		bufferid = m_BuffreeList.front();
		m_BuffreeList.pop_front();
	}
	mutexBuffreeList_.unlock();
	if(-1 == bufferid )
	{
		printf("insertTrnasData error no free buffer\n") ;
		LOGI(" 99999999 error pushData no free buffer\n");
		return -1;
	}

	if(m_vSendBuf[bufferid].maxlen < len )
	{
		delete[] m_vSendBuf[bufferid].data;
		m_vSendBuf[bufferid].maxlen = len + 1024;
		m_vSendBuf[bufferid].data = new char[len + 1024] ;
	}
	memcpy(m_vSendBuf[bufferid].data,data,len);
	atransunit.bufferid = bufferid;
	atransunit.data = m_vSendBuf[bufferid].data;
	pthread_mutex_lock(&mutexTransList_);
	//mutexTransList_.lock();
	m_transUintList.push_back(atransunit);
	//mutexTransList_.unlock();
	pthread_mutex_unlock(&mutexTransList_);
	pthread_cond_signal(&m_rcond_);
	return 0;
}

int doTrnasData(const stTransUnit* ptransunit)
{
	if((NULL == ptransunit) || (NULL == g_javaEnv))
	{
		return 1;
	}
	CSockStream outBuf;
	unsigned long totalLen = 0;
	outBuf << totalLen << ptransunit->_pdu ; //add 2013-3-13
	outBuf << ptransunit->_msgType << ptransunit->_dataType;
#if (!(defined(_For_Pad_)) || !(defined(_For_CheZai_)))
	//LOGI(" doTrnasData 222\n");
	outBuf << ptransunit->_gid;
	outBuf.writeString(ptransunit->_userid);
#endif
	//outBuf << ptransunit->_roster_flag << ptransunit->_pdu << ptransunit->_datalen; //modify 2013-3-21
	outBuf << ptransunit->_roster_flag << ptransunit->_datalen; 
	outBuf.writeBytes(ptransunit->data,ptransunit->_datalen);

//add 2013-3-13
	outBuf.seek(0);
	totalLen = outBuf.length() - sizeof(unsigned long);
	outBuf << totalLen;
//end add 2013-3-13

	if(outBuf.length() > Max_ShareBuf_LEN)
	{
		LOGI(" eeeeeeeerrrrr error outBuf.length() > Max_ShareBuf_LEN\n");
		return 2;
	}
	g_javaEnv->SetByteArrayRegion(g_arg,0,outBuf.length(),(jbyte*)outBuf.getData());

#ifdef WEBCON_HAS_PRINT_DEBUG
	LOGI("totalLen=[%d] msgType[%d] dataType[%d] roste_flag[%d] pdu[%d] datalen[%d] \n",totalLen,ptransunit->_msgType,ptransunit->_dataType,ptransunit->_roster_flag,ptransunit->_pdu,ptransunit->_datalen);
#endif

//	g_javaEnv->SetObjectField(g_obj, g_fid,g_arg); //modify 201303013
	g_javaEnv->CallVoidMethod(g_obj, g_javaMid);
	//g_javaEnv->CallStaticObjectMethod(g_javaCls, g_javaMid, arg);
	//g_javaEnv->CallObjectMethod(g_obj, g_javaMid, arg);
/*
	char out_str[500];
	const char* p_ostr = out_str;
	sprintf(out_str,"msgtype[%d] dataType[%d] userid[%s] roster_flag[%d] pdu[%d] datalen[%d] arglen=[%d]\n",
ptransunit->_msgType,ptransunit->_dataType,ptransunit->_userid,ptransunit->_roster_flag,ptransunit->_pdu,ptransunit->_datalen,outBuf.length());
	jstring arg = g_javaEnv->NewStringUTF(p_ostr);
	jstring result = (jstring)g_javaEnv->CallStaticObjectMethod(g_javaCls, g_javaMid, arg);
*/
	//jbyte* ba = g_javaEnv->GetByteArrayElements(arg, JNI_FALSE);
	//g_javaEnv->ReleaseByteArrayElements(arg,ba,0);
	

	return 0;
}

int stBufferManage::stopWork() //add 2014-7-24
{
	pthread_mutex_lock(&mutexTransList_);
	g_doWork = false;
	pthread_mutex_unlock(&mutexTransList_);
	pthread_cond_signal(&m_rcond_);
	
}

int stBufferManage::getData()
{
	stTransUnit* ptransunit = NULL;
	stTransUnit  atransunit;
	bool stopFlag = false;
	pthread_mutex_lock(&mutexTransList_);
	//mutexTransList_.lock();
	//m_transUintList.size() > 0)
//	{
	while(0 == (m_transUintList.size() ))
		pthread_cond_wait(&m_rcond_, &mutexTransList_);

		if(g_doWork) //add 2014-7-24
		{
			atransunit = m_transUintList.front();
			m_transUintList.pop_front();
			ptransunit = &atransunit;
		}
		else
		{
			stopFlag = true;
		}
//	}

	pthread_mutex_unlock(&mutexTransList_);
	if(stopFlag ) //add 2014-7-24
	{
		return 1;
	}
	//mutexTransList_.unlock();
	if(NULL != ptransunit)
	{	
		doTrnasData(ptransunit);

		mutexBuffreeList_.lock();
		m_BuffreeList.push_back(ptransunit->bufferid);
		mutexBuffreeList_.unlock();
		ptransunit->data = NULL;
	}
	else
	{
		//usleep(10); //modify by zjj  2011-6-13
		usleep(500); 
	}
	return 0;
}

stBufferManage g_buffMgr;

/** 成员改变回调函数
* @param nRosterFlag 改变标志
* 0:p2p加入  1: 服务器转发加入   2: 离开  3: 服务器异常 4 好友状态改变
 */
void __stdcall myCallBack(int msgType, void* pUserData,
						  unsigned short gid,unsigned short dataType,//new add
							const char *user_id,const void* data, unsigned long dataLen,
											unsigned short pdu,int roster_flag, unsigned long user_flag)
{
//	std::cout << std::endl << "myCallBack msgType =[" << msgType << "] gid =[" << gid << "] dataType=[" 
//		<< dataType  << "] roster_flag =[" << roster_flag << "] pdu[" << pdu << "] dataLen[" << dataLen << "]"  << std::endl;
	
#ifdef WEBCON_HAS_PRINT_DEBUG
	LOGI(" myCallBack msgType[%d] gid[%d] dataType[%d] roste_flag[%d] pdu[%d] datalen[%d]\n",msgType,gid,dataType,roster_flag,pdu,dataLen);
#endif
	//printUserInfo((STR_UserData*)pUserData);
	if (MSGTYPE_OnReceivedData == msgType)
	{
	}

	if (MSGTYPE_OnMemberRoster == msgType)
	{
		//std::cout << std::endl << "gid[" << gid << "] OnMemberRoster roste_flag[" << roster_flag << "]" << std::endl;
	}
	else if (MSGTYPE_OnLogOut == msgType)
	{
		std::cout << "myCallBack logout" << std::endl;		
	}

	stTransUnit atransunit ;
	atransunit._msgType = msgType;
	atransunit._gid = gid;
	atransunit._dataType = dataType;
	if(NULL == user_id)
	{
		strcpy(atransunit._userid,"");
	}
	else
		strcpy(atransunit._userid,user_id);
	
	atransunit._datalen = dataLen;
	atransunit._pdu = pdu;
	atransunit._roster_flag = roster_flag;
	if((MSGTYPE_OnMemberRoster == msgType) && ((4 == roster_flag)||(0 == roster_flag) || (1 == roster_flag))) //add 2013-2-1
	{
		CSockStream tmpBuf(64);
		LOGI("888888888888  user_flag=[%d]",user_flag);
		tmpBuf << user_flag;
		atransunit._datalen = sizeof(user_flag);
		g_buffMgr.pushData(atransunit,tmpBuf.getData(),tmpBuf.length());
	}
	else
	{
		g_buffMgr.pushData(atransunit,data,dataLen);
	}

	return ;
}

FCNetAcInfo g_fcinfo;

//int login(const char* ip,unsigned short port,const char* szAccount,const char* passwd) //modify 2014-3-18
int login(const char* ip,unsigned short port,const char* szAccount,const char* passwd,unsigned long userType,unsigned short netType)
{
	char userPData[129] = "kkkk";


	int nRet = 0;
	g_fcinfo.pUserData_ = (void*)&userPData;
	g_fcinfo.userData_ = NULL;
	g_fcinfo.userDataLen_ = 0;
	std::string strAccount,strPasswd,strNettype;
	//char tmpBuf[256];
	std::string strServerIp;
	unsigned short nLocalPort;

#if 0
	strServerIp = "192.168.1.82";
	nLocalPort = 9600;
	strAccount = "888888088";
	strPasswd = "111111";
#else
	strServerIp = ip;
	nLocalPort = port;
	strAccount = szAccount;
	strPasswd = passwd;
#endif
	//strAccount = "zhoujj@webcon.com.cn";

	strcpy(g_fcinfo.szServerIp,strServerIp.c_str());
	g_fcinfo.nLocalPort = nLocalPort;
	g_fcinfo.nType_ = netType;
	g_fcinfo.pCallBack_ = myCallBack;

	strcpy(g_fcinfo.szAccount_,strAccount.c_str());
	strcpy(g_fcinfo.szPasswd_,strPasswd.c_str());
/* //modif 2013-3-19
	sprintf(tmpBuf,"i am %s userData",strAccount.c_str());

	g_fcinfo.userDataLen_ = strlen(tmpBuf) + 1;
	if (g_fcinfo.userData_)
	{
		delete [] (char*)g_fcinfo.userData_;
		g_fcinfo.userData_ = NULL;

	}
	if (g_fcinfo.userDataLen_)
	{
		g_fcinfo.userData_ = new char[g_fcinfo.userDataLen_];	
		memcpy(g_fcinfo.userData_,tmpBuf,g_fcinfo.userDataLen_);
	}
*/

#if _FOR_PC_VER
	g_fcinfo.userAttr_ = 1;
#else
	g_fcinfo.userAttr_ = userType;
#endif
	g_doWork = true;

	//nRet = FCNetACLogin(g_fcinfo,TIME_OUT_INFINITE);
	nRet = FCNetACLogin(g_fcinfo,18000);
	return nRet;
}

int do_work_android()
{
	int ret = 0;
	g_buffMgr.createBuffer(100);
	while(1)
	{
		//g_buffMgr.getData(); //modify 2014-7-24
		ret = g_buffMgr.getData(); //modify 2014-7-24
		if(0 != ret)
		{
			break;
		}
		//usleep(100); //modif 2013-5-13
	}
}

int do_work()
{	
	int nRet = 0;
	char *UserList[100];
	for (int i = 0; i < 100; i++)
	{
		UserList[i] = new char[129];
	}
	{	
		printf("login ok!\n");
		unsigned short pduType = 1;
		unsigned short sendType = 1;
		unsigned short cmdNum = 1;
		unsigned short joinGid = 0;
		std::string strCmd,strUser;
		std::string sendText;
		unsigned char chP2pFlag; 
		bool p2pFlag  = false;
		bool startSend = false;
		const unsigned long sendDataLen = 1024 * 4 ;
		char strSendData[sendDataLen] ;

		while (g_doWork)
		{
	//		startSend = false;
			if(!startSend)
			{
				std::cout << "please input number" << std::endl;
				std::cout << "1: send data tu winSys 	" << std::endl;
				std::cout << "7: quit " << std::endl;
				
				std::cin >> cmdNum;
				if (!g_doWork)
				{
					break;
				}
				if (1 == cmdNum)
				{
				}
				else if (7 == cmdNum)
				{
					std::cout << "now quit system ..." << std::endl;
					break;
				}
	
			}

			#ifdef __LINUX__
			sleep(2);
			#else
			Sleep(1000);
			#endif

		//		nRet = FCNetACSendData(joinGid,sendType,strUser.c_str(),pduType,sendText.c_str(),strlen(sendText.c_str()) + 1,p2pFlag);
		}
	}
	
	std::cout << "begin call FCNetACLogout " << std::endl;
	FCNetACLogout();
	std::cout << "after call FCNetACLogout " << std::endl;


	std::cout << "begin call FCNetACUninit " << std::endl;
	FCNetACUninit();
	std::cout << "after		call FCNetACUninit " << std::endl;

	if (g_fcinfo.userData_)
	{
		delete [] (char*)g_fcinfo.userData_;
	}
	g_fcinfo.userData_ = NULL;


	return 0;
}

#if defined (_FOR_PC_VER) || (_Pad_PC_VER)
int main(int argc, char *argv[])
{
	std::string strIp;
	short sPort = 9600;
	unsigned short nType = 0x5;
	
	int	nRet = FCNetACInit();
	if(0 != nRet )
	{	
		std::cout << "call FCNetACInit error nRet =[" << nRet << "]" << std::endl;
	}
	
	while(1)
	{
		std::string strPasswd,strAccount;
		strIp = "";
		//strPasswd = "111111";
		//strIp = "218.108.11.150";
		//strIp = "180.153.155.7";
		//strIp = "192.168.1.81";
		//strPasswd = "111111";
		//strIp = "202.107.195.117";
		std::cout << "please input login Server'ip (eg:202.107.195.117):" ;
		std::cin >> strIp;
		std::cout << "please input local port (eg:9600) :";
		std::cin >> sPort;
	//	strAccount = "asijack01"; //phone users
	//	strPasswd = "asijack";
		//strAccount = "zhoujj"; //phone users
		strPasswd = "zjj123456";
		//strAccount = "888888001"; //phone users
		//strPasswd = "123456";
		std::cout << "please input account (eg:888888026) :";
		std::cin >> strAccount;
		//std::cout << "please input passwd (eg:111111) :";
		//std::cin >> strPasswd;
		//nRet = login(strIp.c_str(),9600,"888888026",strPasswd.c_str());
		unsigned long userattr = 1;
		nRet = login(strIp.c_str(),sPort,strAccount.c_str(),strPasswd.c_str(),userattr,nType);
		//nRet = login(strIp.c_str(),9600,"888888023",strPasswd.c_str());
		if(0 != nRet )
		{
			std::cout << "call login error nRet =[" << nRet << "]" << std::endl;
			sleep(2);
			continue;
		}
		break;
	}

	return do_work();
}
#else

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_init
  (JNIEnv *env, jobject obj)
{
	int nRet = 0;
	std::cout << "begin call FCNetACInit " << std::endl;
	nRet = FCNetACInit();

	if (0 != nRet)
	{
		std::cout << "call FCNetACInit error nRet =[" << nRet << "]" << std::endl;
		std::cout << "please reLgin" << std::endl;
		//printf("call FCNetACInit error nRet[%d] \n",nRet);
	}
	std::cout << "after call FCNetACInit " << std::endl;
	return nRet;
}

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_unInit (JNIEnv *env, jobject obj)
{
	g_buffMgr.stopWork();	 //add 2014-7-24
	return FCNetACUninit();
}

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_logout(JNIEnv *env, jobject obj)
{
	LOGI(" now call logout\n");
	return FCNetACLogout();
}

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_login
  (JNIEnv *env, jobject obj, jstring jServIp, jint jServPort, jbyteArray jszAccount,jstring jpasswd,jint jterm,jint jnetType)
  {
	const char* ip = NULL;
	unsigned short port;
	const char* szAccount = NULL;
	const char* passwd = NULL;
	int nRet = 0;
	g_userList.clear();

	ip = env->GetStringUTFChars(jServIp, JNI_FALSE);
//	szAccount = env->GetStringUTFChars(jszAccount, JNI_FALSE);
	jbyte*   szAccount_   =   (jbyte*) env->GetByteArrayElements(jszAccount,JNI_FALSE);	
	passwd = env->GetStringUTFChars(jpasswd, JNI_FALSE);
	port = jServPort;	

	
	//LOGI("zzzzzzzzzz login   szAccount_ =[%s] len=[%d]\n",szAccount_,strlen((char*)szAccount_));
//	int nRet = login();
	//int nRet = FCNetACInit();
	//nRet = login(ip,port,szAccount,passwd);
	unsigned short netType = 0x5;

	#ifdef _For_ZhiHuiYangLao_
		netType = jnetType;
	#endif

	nRet = login(ip,port,(const char*)szAccount_,passwd,jterm,netType);
	//nRet = login("192.168.1.82",9600,"888888088","111111");

	env->ReleaseStringUTFChars(jServIp,ip);
	//env->ReleaseStringUTFChars(jszAccount,szAccount);
	env->ReleaseByteArrayElements(jszAccount,szAccount_,0);
	env->ReleaseStringUTFChars(jpasswd,passwd);

	if(0 != nRet)
	{
		return nRet;
	}

  	return nRet;
  }

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_CCallbackMethod_setCallBackObj
  (JNIEnv *env, jobject obj)
{
	g_obj = obj;
	return 0;
}

JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_CCallbackMethod_doWork
  (JNIEnv *env, jobject obj,jstring jpath, jstring jmethod, jstring jbufName)
{
	const char* javaPath = NULL;
	const char* javaMethod = NULL; 
	const char* bufName = NULL; 
	int nRet = 0;

	javaPath = env->GetStringUTFChars(jpath, JNI_FALSE);
	javaMethod = env->GetStringUTFChars(jmethod, JNI_FALSE);
	bufName = env->GetStringUTFChars(jbufName, JNI_FALSE);
	g_javaEnv = env;
	g_javaCls = g_javaEnv->FindClass(javaPath);
	if (g_javaCls != 0)
	{
		g_javaMid = g_javaEnv->GetMethodID(g_javaCls, javaMethod, "()V");
		if (g_javaMid == 0)
		{
			return 24;
		}
		g_fid = g_javaEnv->GetFieldID(g_javaCls, bufName, "[B");
		if (g_fid == 0)
		{
			return 25;
		}
		g_arg = g_javaEnv->NewByteArray(Max_ShareBuf_LEN);
		g_javaEnv->SetObjectField(g_obj, g_fid,g_arg); //add 201303013
	}
	env->ReleaseStringUTFChars(jpath,javaPath);
	env->ReleaseStringUTFChars(jmethod,javaMethod);
	env->ReleaseStringUTFChars(jbufName,bufName);

	do_work_android();
	jbyte* ba = g_javaEnv->GetByteArrayElements(g_arg, JNI_FALSE);
	g_javaEnv->ReleaseByteArrayElements(g_arg,ba,0);

	return 0;	
}


JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_getDBData
  //(JNIEnv *env, jobject jobj, jshort reqPdu,jbyteArray inData, jint inLen,short rspPdu,jbyteArray outData, jint outLen)
  (JNIEnv *env, jobject jobj, jshort reqPdu,jbyteArray inData, jint inLen,jbyteArray outData, jint outLen)
{
	 jbyte*   inData_   =   (jbyte*) env->GetByteArrayElements(inData,JNI_FALSE);	
	 jbyte*   outData_  =   (jbyte*) env->GetByteArrayElements(outData,JNI_FALSE);	
	const char* pRetData = NULL;
	unsigned long retLen = 0;
	unsigned short retPdu = 0;
	jint nRet = 0;
	nRet = FCNetACSendDataToDB((unsigned short)reqPdu,(const char*)inData_,inLen,retPdu,pRetData,retLen);
	if (retLen > outLen)
	{
		return -1;
	}
	//rspPdu = retPdu;
	outLen = retLen;
	memcpy(outData_,pRetData,outLen);
#ifdef WEBCON_HAS_PRINT_DEBUG
	LOGI(" after getData ret [%d] rspPdu=[%d] retLen=[%d] ",nRet,retPdu,retLen);
#endif
	env->ReleaseByteArrayElements(inData,inData_,0);
  	env->ReleaseByteArrayElements(outData,outData_,0);
	return outLen ;
}


JNIEXPORT jint JNICALL Java_com_webcon_wp_utils_NativeInterface_sendData
  (JNIEnv *env, jobject jobj, jshort byGid,jshort dataType,jstring jdstUser, jshort reqPdu,jbyteArray inData, jint inLen,jboolean p2pFlag)
	{
		const char* dstUser = NULL;
		dstUser = env->GetStringUTFChars(jdstUser, JNI_FALSE);
		if(!myFindUser2(dstUser))
		{
			env->ReleaseStringUTFChars(jdstUser,dstUser);
			return 1;
		}
	 	jbyte*   sendData_   =   (jbyte*) env->GetByteArrayElements(inData,JNI_FALSE);	
		int nRet = FCNetACSendData(byGid,dataType,dstUser,reqPdu,(const char*)sendData_,inLen,p2pFlag);
		env->ReleaseStringUTFChars(jdstUser,dstUser);
		env->ReleaseByteArrayElements(inData,sendData_,0);
		return nRet ;
	}


#endif
