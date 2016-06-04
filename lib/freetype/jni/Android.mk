LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/builds \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES:= \
    src/base/ftbbox.c \
    src/base/ftbitmap.c \
    src/base/ftglyph.c \
    src/base/ftstroke.c \
    src/base/ftxf86.c \
    src/base/ftbase.c \
    src/base/ftsystem.c \
    src/base/ftinit.c \
    src/base/ftgasp.c \
    src/raster/raster.c \
    src/sfnt/sfnt.c \
    src/smooth/smooth.c \
    src/autofit/autofit.c \
    src/truetype/truetype.c \
    src/cff/cff.c \
    src/psnames/psnames.c \
    src/pshinter/pshinter.c \
    src/type1/type1.c \
    src/cid/type1cid.c \
    src/pfr/pfr.c \
    src/type42/type42.c \
    src/winfonts/winfnt.c \
    src/pcf/pcf.c \
    src/psaux/psaux.c \
    src/bdf/bdf.c \
    src/gzip/ftgzip.c \
    src/lzw/ftlzw.c

LOCAL_CFLAGS += -DFT2_BUILD_LIBRARY
LOCAL_MODULE := freetype
LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_MODULE    := godpatterns
LOCAL_CFLAGS    := -Werror
LOCAL_STATIC_LIBRARIES := freetype
LOCAL_SRC_FILES := godpatterns_android.cpp
LOCAL_LDLIBS    := -llog -lGLESv2

include $(BUILD_SHARED_LIBRARY)
