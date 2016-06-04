LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := freetype
LOCAL_SRC_FILES :=  freetype/libfreetype.a
LOCAL_EXPORT_C_INCLUDES :=  $(LOCAL_PATH)/freetype/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_MODULE := FTGLES
#LOCAL_CFLAGS := -I$(LOCAL_PATH)/../freetype/include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
$(LOCAL_PATH)/src/iGLU-1.0.0/include

LOCAL_SRC_FILES := \
src/FTBuffer.cpp \
src/FTCharmap.cpp \
src/FTContour.cpp \
src/FTFace.cpp \
src/FTFont/FTBitmapFont.cpp \
src/FTFont/FTBufferFont.cpp \
src/FTFont/FTFont.cpp \
src/FTFont/FTFontGlue.cpp \
src/FTFont/FTOutlineFont.cpp \
src/FTFont/FTPixmapFont.cpp \
src/FTFont/FTPolygonFont.cpp \
src/FTFont/FTTextureFont.cpp \
src/FTGL/ftglesGlue.cpp \
src/FTGlyph/FTBitmapGlyph.cpp \
src/FTGlyph/FTBufferGlyph.cpp \
src/FTGlyph/FTGlyph.cpp \
src/FTGlyph/FTGlyphGlue.cpp \
src/FTGlyph/FTOutlineGlyph.cpp \
src/FTGlyph/FTPixmapGlyph.cpp \
src/FTGlyph/FTPolygonGlyph.cpp \
src/FTGlyph/FTTextureGlyph.cpp \
src/FTGlyphContainer.cpp \
src/FTLayout/FTLayout.cpp \
src/FTLayout/FTLayoutGlue.cpp \
src/FTLayout/FTSimpleLayout.cpp \
src/FTLibrary.cpp \
src/FTPoint.cpp \
src/FTSize.cpp \
src/FTVectoriser.cpp \
src/iGLU-1.0.0/libtess/dict.c \
src/iGLU-1.0.0/libtess/geom.c \
src/iGLU-1.0.0/libtess/memalloc.c \
src/iGLU-1.0.0/libtess/mesh.c \
src/iGLU-1.0.0/libtess/normal.c \
src/iGLU-1.0.0/libtess/priorityq.c \
src/iGLU-1.0.0/libtess/render.c \
src/iGLU-1.0.0/libtess/sweep.c \
src/iGLU-1.0.0/libtess/tess.c \
src/iGLU-1.0.0/libtess/tessmono.c \
src/iGLU-1.0.0/libutil/error.c \
src/iGLU-1.0.0/libutil/glue.c \
src/iGLU-1.0.0/libutil/project.c \
src/iGLU-1.0.0/libutil/registry.c

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DFTGL_LIBRARY_STATIC
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/freetype
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/src

LOCAL_STATIC_LIBRARIES := freetype

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := godpatterns
LOCAL_STATIC_LIBRARIES := FTGLES
LOCAL_SRC_FILES := godpatterns_android.cpp
LOCAL_LDLIBS    := -llog -lGLESv1_CM
include $(BUILD_SHARED_LIBRARY)