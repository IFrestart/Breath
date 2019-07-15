LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := socks5
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS := -lpthread
LOCAL_CFLAGS += -D__LINUX__
G=../../
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $Gtinyxmllib/jni/include $Gsocks5/jni/include

LOCAL_SRC_FILES := CommFunction.cpp HttpSocks5.cpp TcpSocks5.cpp test.cpp UdpSocks5.cpp

#include $(ACE_ROOT)/include/makeinclude/platform_cygwin32.GNU
#include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
#include $(ACE_ROOT)/include/makeinclude/macros.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.nested.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
