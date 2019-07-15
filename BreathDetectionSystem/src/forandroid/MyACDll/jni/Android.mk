LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := socks5
LOCAL_SRC_FILES := ../../lib/jni/libsocks5.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jrtp
LOCAL_SRC_FILES := ../../lib/jni/libjrtp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := CommBase
#LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__

G=../../
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include ../../jrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include

LOCAL_SRC_FILES := ThreadCallBack.cpp Thread.cpp Timer.cpp ACSocket.cpp LogTrace.cpp Tools.cpp SockStream.cpp IdDef.cpp Mutex.cpp\
RTPEventCallBack.cpp  SockEventCallBack.cpp SockHeaderCallBack.cpp SockHeader.cpp TestSockEvent_rtp.cpp flib.cpp

#LOCAL_STATIC_LIBRARIES := libNetCtrl libCommBase libServBase libSendConfig libtinyxml libjrtp libsocks5 libACE 
#LOCAL_STATIC_LIBRARIES := libsocks5 libjrtp
LOCAL_SHARED_LIBRARIES := socks5 jrtp
#LOCAL_STATIC_LIBRARIES := libjrtp

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := myjni
#LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__

G=../../
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/include ../../jrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include 

LOCAL_SRC_FILES := test.cpp
#LOCAL_STATIC_LIBRARIES := libMyAc
#LOCAL_STATIC_LIBRARIES := HighComm
LOCAL_SHARED_LIBRARIES := CommBase
#LOCAL_STATIC_LIBRARIES := libjrtp
#LOCAL_STATIC_LIBRARIES := libsocks5

include $(BUILD_SHARED_LIBRARY)
