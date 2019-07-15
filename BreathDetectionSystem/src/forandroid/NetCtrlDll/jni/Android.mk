LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libACE
#LOCAL_SRC_FILES := ../../libACE.a
#include $(PREBUILT_STATIC_LIBRARY)
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
#LOCAL_MODULE    := libSendConfig
#LOCAL_SRC_FILES := ../../lib/jni/libSendConfig.a
#include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := NetCtrl
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS := -lpthread
#LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -D__LINUX__ -DVER_FOR_ANDROID_
G=../../
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/include $GCommBase/jni/include $GSendConfig/jni/include\
$Gjrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include

LOCAL_SRC_FILES := AllState.cpp BaseState.cpp NetCtrlCall.cpp PacketManager.cpp TransServSockEvent.cpp AuthServSockEvent.cpp NetCtrlDll.cpp 

#LOCAL_STATIC_LIBRARIES := libSendConfig
LOCAL_SHARED_LIBRARIES := socks5 jrtp CommBase

#include $(ACE_ROOT)/include/makeinclude/platform_cygwin32.GNU
#include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
#include $(ACE_ROOT)/include/makeinclude/macros.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.nested.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
