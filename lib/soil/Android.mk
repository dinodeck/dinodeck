LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := soil
LOCAL_SRC_FILES := soil.a
LOCAL_CFLAGS    := -O2 -s -Wall
LOCAL_SRC_FILES := \
  src/image_helper.c \
  src/stb_image_aug.c  \
  src/image_DXT.c \
  src/SOIL.c \
LOCAL_LDLIBS    := -llog -lGLESv2

include $(BUILD_STATIC_LIBRARY)
