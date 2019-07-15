LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libACE
#LOCAL_SRC_FILES := ../../libACE.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libCommBase
#LOCAL_SRC_FILES := ../../lib/jni/libCommBase.a
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libServBase
#LOCAL_SRC_FILES := ../../lib/jni/libServBase.a
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
#LOCAL_MODULE    := tinyxml
#LOCAL_SRC_FILES := ../../lib/jni/libtinyxml.so
#include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := NetCtrl
LOCAL_SRC_FILES := ../../lib/jni/libNetCtrl.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := FCNetAC
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS := -lpthread
#LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -D__LINUX__ -DVER_FOR_ANDROID_
G=../../
#LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/include $GServBase/jni/include  $GCommBase/jni/include $GSendConfig/jni/include\
#$Gjrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include $GNetCtrlDll/jni/include
LOCAL_C_INCLUDES := $(ACE_ROOT) $(LOCAL_PATH)/include $GCommBase/jni/include $GServBase/jni/include $GSendConfig/jni/include\
$Gjrtplib-2.7/jni/ $Gtinyxmllib/jni/include $Gsocks5/jni/include $GNetCtrlDll/jni/include

#LOCAL_SRC_FILES := Event.cpp FCNetACLib.cpp NetACClient.cpp WaitForObject.cpp FileGetPara.cpp
LOCAL_SRC_FILES := Event.cpp FCNetACLib.cpp NetACClient.cpp WaitForObject.cpp

#LOCAL_STATIC_LIBRARIES := libACE libCommBase libServBase libNetCtrl
#LOCAL_STATIC_LIBRARIES := libNetCtrl
#LOCAL_SHARED_LIBRARIES := socks5 jrtp CommBase tinyxml NetCtrl
LOCAL_SHARED_LIBRARIES := socks5 jrtp CommBase NetCtrl

#include $(ACE_ROOT)/include/makeinclude/platform_cygwin32.GNU
#include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
#include $(ACE_ROOT)/include/makeinclude/macros.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.nested.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
