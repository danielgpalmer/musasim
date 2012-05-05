LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libSDL2_net
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libSDL2_net.so

include $(PREBUILT_SHARED_LIBRARY)

