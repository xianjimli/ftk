LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
         $(call all-subdir-java-files)

LOCAL_CERTIFICATE := platform

LOCAL_PACKAGE_NAME := FtkApps

LOCAL_JNI_SHARED_LIBRARIES := libftkapp

include $(BUILD_PACKAGE)

MY_PATH := $(LOCAL_PATH)

include $(MY_PATH)/jni/Android.mk

