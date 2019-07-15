LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libMyAc
LOCAL_SRC_FILES := libMyAc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
#LOCAL_C_INCLUDES += system/core/include/cutils
LOCAL_C_INCLUDES += ../../armbuild/jni/include
#LOCAL_SHARED_LIBRARIES := libcutil
LOCAL_MODULE    := myjni
LOCAL_CFLAGS    := -Werror
#LOCAL_CFLAGS    := -Werror -llog -lpthread
LOCAL_SRC_FILES := test.cpp
#LOCAL_LDLIBS := -ldl -lpthread
#LOCAL_LDLIBS := -ldl
LOCAL_STATIC_LIBRARIES := libMyAc
#LOCAL_STATIC_LIBRARIES := libCommBase libFCNetAC

include $(BUILD_SHARED_LIBRARY)
