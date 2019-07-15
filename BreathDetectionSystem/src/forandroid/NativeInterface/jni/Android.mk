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
LOCAL_SRC_FILES := ../../lib/jni/libCommBase.so
include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := tinyxml
#LOCAL_SRC_FILES := ../../lib/jni/libtinyxml.so
#include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := NetCtrl
LOCAL_SRC_FILES := ../../lib/jni/libNetCtrl.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := FCNetAC
LOCAL_SRC_FILES := ../../lib/jni/libFCNetAC.so
include $(PREBUILT_SHARED_LIBRARY)
#include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := myjni
#LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS += -D__LINUX__
LOCAL_LDLIBS := -llog
#LOCAL_CFLAGS += -D__LINUX__ -D_For_Pad_
#LOCAL_CFLAGS += -D__LINUX__ -D_For_CheZai_
LOCAL_CFLAGS += -D_For_ZhiHuiYangLao_

G=../../
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/include ../../jrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include
LOCAL_C_INCLUDES := ../../CommBase/jni/include $(LOCAL_PATH)/include  ../../FCNetACLib/jni/include 

LOCAL_SRC_FILES := Lock.cpp ClientNativeInterface.cpp
#LOCAL_SHARED_LIBRARIES := socks5 jrtp CommBase tinyxml NetCtrl FCNetAC
LOCAL_SHARED_LIBRARIES := socks5 jrtp CommBase NetCtrl FCNetAC
LOCAL_STATIC_LIBRARIES := libjvm
#LOCAL_STATIC_LIBRARIES := libFCNetAC
#LOCAL_STATIC_LIBRARIES := libMyAc
#LOCAL_STATIC_LIBRARIES := HighComm
#LOCAL_SHARED_LIBRARIES := CommBase
#LOCAL_STATIC_LIBRARIES := libjrtp
#LOCAL_STATIC_LIBRARIES := libsocks5

include $(BUILD_SHARED_LIBRARY)
