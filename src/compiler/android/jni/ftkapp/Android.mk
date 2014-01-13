LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libftkapp

LOCAL_CFLAGS :=	\
	-I$(LOCAL_PATH)/../../../../../src \
	-I$(LOCAL_PATH)/../../../../../src/im \
	-I$(LOCAL_PATH)/../../../../../src/backend/android \
	-I$(LOCAL_PATH)/../../../../../src/os/android\
	-I$(EXT_INCLUDE_DIR)/skia/include \
	-I$(EXT_INCLUDE_DIR)/skia/include/core \
	-I$(EXT_INCLUDE_DIR)/skia/include/images \
    -I$(EXT_INCLUDE_DIR)/system/core/include \
    -I$(EXT_INCLUDE_DIR)/frameworks/base/include 

LOCAL_SRC_FILES := \
	ftk_jni.cpp \
	../../../..//demos/demo_file_browser.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../ftkapp  $(JNI_H_INCLUDE)

LOCAL_STATIC_LIBRARIES := libftk libcutils
LOCAL_SHARED_LIBRARIES := libskia
LOCAL_LDLIBS := -lGLESv1_CM -llog -lgcc -L$(EXT_LIBS_DIR) -lskia -lz
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
