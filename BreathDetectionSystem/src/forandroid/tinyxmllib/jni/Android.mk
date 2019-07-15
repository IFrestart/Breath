LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE    := libACE
#LOCAL_SRC_FILES := ../../libACE.a
#include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := tinyxml
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS := -lpthread
LOCAL_CFLAGS += -D__LINUX__
G=../../
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include 

LOCAL_SRC_FILES := tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp

#LOCAL_STATIC_LIBRARIES := libACE

#include $(ACE_ROOT)/include/makeinclude/platform_cygwin32.GNU
#include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
#include $(ACE_ROOT)/include/makeinclude/macros.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.nested.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
#include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
