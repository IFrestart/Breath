LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libACE
LOCAL_SRC_FILES := libACE.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libtinyxml
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/tinyxmllib/include 

LOCAL_SRC_FILES := tinyxmllib/tinystr.cpp\
tinyxmllib/tinyxml.cpp\
tinyxmllib/tinyxmlerror.cpp\
tinyxmllib/tinyxmlparser.cpp\

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libsocks5
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/socks5/include $(LOCAL_PATH)/ServBase/include  $(LOCAL_PATH)/CommBase/include $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7/ $(LOCAL_PATH)/tinyxmllib/include

LOCAL_SRC_FILES := socks5/CommFunction.cpp\
socks5/HttpSocks5.cpp\
socks5/TcpSocks5.cpp\
socks5/test.cpp\
socks5/UdpSocks5.cpp\

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libjrtp
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include
LOCAL_SRC_FILES := jrtplib-2.7/rtpconnection.cpp\
jrtplib-2.7/rtpdestlist.cpp\
jrtplib-2.7/rtpiptable.cpp\
jrtplib-2.7/rtppacket.cpp\
jrtplib-2.7/rtprtcpmodule.cpp\
jrtplib-2.7/rtpsourcedescription.cpp\
jrtplib-2.7/rtptimeutil.cpp\
jrtplib-2.7/rtpcontributingsources.cpp\
jrtplib-2.7/rtperror.cpp\
jrtplib-2.7/rtplocalinfo.cpp\
jrtplib-2.7/rtppacketprocessor.cpp\
jrtplib-2.7/rtpsession.cpp\
jrtplib-2.7/rtpsources.cpp\
jrtplib-2.7/rtpdebug.cpp\
jrtplib-2.7/rtphandlers.cpp\
jrtplib-2.7/rtpmcasttable.cpp\
jrtplib-2.7/rtprandom.cpp\
jrtplib-2.7/rtpsourcedata.cpp\
jrtplib-2.7/rtpsrclist.cpp\

LOCAL_STATIC_LIBRARIES := libsocks5

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libCommBase
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/CommBase/include $(LOCAL_PATH)/ServBase/include $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include

LOCAL_SRC_FILES := CommBase/BlockQueue.cpp\
CommBase/HTTPHead.cpp\
CommBase/HTTPSvcHandler.cpp\
CommBase/IdDef.cpp\
CommBase/PacketTimeOut.cpp\
CommBase/RTPConnector.cc\
CommBase/RTPEventCallBack.cpp\
CommBase/SockBlock.cpp\
CommBase/SockEventCallBack.cpp\
CommBase/SockHeaderCallBack.cpp\
CommBase/SockStream.cpp\
CommBase/SvcManager.cpp\
CommBase/TCPAcceptor.cc\
CommBase/TcpAutoConnector.cpp\
CommBase/TCPConnector.cc\
CommBase/TCPMTAcceptor.cc\
CommBase/TCPSvHandler.cc\
CommBase/UDPConnector.cc\
CommBase/UDPSvcHandler.cc\

LOCAL_STATIC_LIBRARIES := libACE libjrtp libtinyxml libsocks5
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libServBase
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/ServBase/include $(LOCAL_PATH)/CommBase/include  $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include

LOCAL_SRC_FILES := ServBase/BaseServ.cpp\
ServBase/FileLogger.cpp\
ServBase/Mutex.cpp\
ServBase/RECSockHeader.cpp\
ServBase/SockHeader.cpp\
ServBase/ThreadCallBack.cpp\
ServBase/Timer.cpp\
ServBase/Tools.cpp\
ServBase/FileGetPara.cpp\
ServBase/Main.cpp\
ServBase/MyTraceLog.cpp\
ServBase/Semaphore.cpp\
ServBase/SysLogger.cpp\
ServBase/Thread.cpp\
ServBase/TimerEventCallBack.cpp\

LOCAL_STATIC_LIBRARIES := libCommBase libACE libjrtp libtinyxml libsocks5
include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE    := libSendConfig
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/SendConfig/include $(LOCAL_PATH)/CommBase/include $(LOCAL_PATH)/ServBase/include  $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include

LOCAL_SRC_FILES := SendConfig/SendConfig.cpp

LOCAL_STATIC_LIBRARIES := libServBase libCommBase libACE libjrtp libtinyxml libsocks5
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libNetCtrl
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/NetCtrlDll/include $(LOCAL_PATH)/ServBase/include  $(LOCAL_PATH)/CommBase/include $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include

LOCAL_SRC_FILES := NetCtrlDll/AllState.cpp\
NetCtrlDll/BaseState.cpp\
NetCtrlDll/NetCtrlCall.cpp\
NetCtrlDll/NetStatus.cpp\
NetCtrlDll/p2pConnectSockEvent.cpp\
NetCtrlDll/PacketManager.cpp\
NetCtrlDll/TransServSockEvent.cpp\
NetCtrlDll/AuthServSockEvent.cpp\
NetCtrlDll/MeetServMgr.cpp\
NetCtrlDll/NetCtrlDll.cpp\
NetCtrlDll/p2pAcceptSockEvent.cpp\
NetCtrlDll/P2PMgr.cpp\
NetCtrlDll/TransServMgr.cpp\
NetCtrlDll/UserListMgr.cpp\

LOCAL_STATIC_LIBRARIES := libSendConfig libServBase libCommBase libACE libjrtp libtinyxml libsocks5
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := myjni
#LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/include $(LOCAL_PATH)/ServBase/include $(LOCAL_PATH)/CommBase/include $(LOCAL_PATH)/SendConfig/include\
$(LOCAL_PATH)/jrtplib-2.7 $(LOCAL_PATH)/tinyxmllib/include $(LOCAL_PATH)/socks5/include $(LOCAL_PATH)/NetCtrlDll/include

LOCAL_SRC_FILES := Event.cpp\
FCNetACLib.cpp\
NetACClient.cpp\
WaitForObject.cpp\

LOCAL_STATIC_LIBRARIES := libACE libtinyxml libsocks5 libjrtp libCommBase libServBase libSendConfig libNetCtrl

include $(BUILD_SHARED_LIBRARY)
