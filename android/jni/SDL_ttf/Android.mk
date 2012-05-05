LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libSDL2_ttf
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libSDL2_ttf.so

include $(PREBUILT_SHARED_LIBRARY)

