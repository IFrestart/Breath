LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libACE
#LOCAL_SRC_FILES := ../../libACE.a
#include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := socks5
LOCAL_SRC_FILES := ../../lib/jni/libsocks5.so
include $(PREBUILT_SHARED_LIBRARY)
#include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := jrtp
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS := -lpthread
LOCAL_CFLAGS += -D__LINUX__
G=../../
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $Gtinyxmllib/jni/include $Gsocks5/jni/include

LOCAL_SRC_FILES := rtpconnection.cpp rtpdestlist.cpp rtpiptable.cpp rtppacket.cpp rtprtcpmodule.cpp rtpsourcedescription.cpp rtptimeutil.cpp\
rtpcontributingsources.cpp rtperror.cpp rtplocalinfo.cpp rtppacketprocessor.cpp rtpsession.cpp rtpsources.cpp\
rtpdebug.cpp rtphandlers.cpp rtpmcasttable.cpp rtprandom.cpp rtpsourcedata.cpp rtpsrclist.cpp

#LOCAL_STATIC_LIBRARIES := libACE
#LOCAL_STATIC_LIBRARIES := libsocks5
LOCAL_SHARED_LIBRARIES := libsocks5

#include $(ACE_ROOT)/include/makeinclude/platform_cygwin32.GNU
#include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
#include $(ACE_ROOT)/include/makeinclude/macros.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.nested.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
