
#Godpatterns JNI make file
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := freetype
LOCAL_SRC_FILES :=  freetype/libfreetype.a
LOCAL_EXPORT_C_INCLUDES :=  $(LOCAL_PATH)/freetype/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_MODULE := FTGLES
#LOCAL_CFLAGS := -I$(LOCAL_PATH)/../freetype/include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ftgles/src \
$(LOCAL_PATH)/ftgles/src/iGLU-1.0.0/include

LOCAL_SRC_FILES := \
../../../lib/ftgles/src/FTBuffer.cpp \
../../../lib/ftgles/src/FTCharmap.cpp \
../../../lib/ftgles/src/FTContour.cpp \
../../../lib/ftgles/src/FTFace.cpp \
../../../lib/ftgles/src/FTFont/FTBitmapFont.cpp \
../../../lib/ftgles/src/FTFont/FTBufferFont.cpp \
../../../lib/ftgles/src/FTFont/FTFont.cpp \
../../../lib/ftgles/src/FTFont/FTFontGlue.cpp \
../../../lib/ftgles/src/FTFont/FTOutlineFont.cpp \
../../../lib/ftgles/src/FTFont/FTPixmapFont.cpp \
../../../lib/ftgles/src/FTFont/FTPolygonFont.cpp \
../../../lib/ftgles/src/FTFont/FTTextureFont.cpp \
../../../lib/ftgles/src/FTGL/ftglesGlue.cpp \
../../../lib/ftgles/src/FTGlyph/FTBitmapGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTBufferGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTGlyphGlue.cpp \
../../../lib/ftgles/src/FTGlyph/FTOutlineGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTPixmapGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTPolygonGlyph.cpp \
../../../lib/ftgles/src/FTGlyph/FTTextureGlyph.cpp \
../../../lib/ftgles/src/FTGlyphContainer.cpp \
../../../lib/ftgles/src/FTLayout/FTLayout.cpp \
../../../lib/ftgles/src/FTLayout/FTLayoutGlue.cpp \
../../../lib/ftgles/src/FTLayout/FTSimpleLayout.cpp \
../../../lib/ftgles/src/FTLibrary.cpp \
../../../lib/ftgles/src/FTPoint.cpp \
../../../lib/ftgles/src/FTSize.cpp \
../../../lib/ftgles/src/FTVectoriser.cpp \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/dict.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/geom.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/memalloc.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/mesh.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/normal.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/priorityq.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/render.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/sweep.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/tess.c \
../../../lib/ftgles/src/iGLU-1.0.0/libtess/tessmono.c \
../../../lib/ftgles/src/iGLU-1.0.0/libutil/error.c \
../../../lib/ftgles/src/iGLU-1.0.0/libutil/glue.c \
../../../lib/ftgles/src/iGLU-1.0.0/libutil/project.c \
../../../lib/ftgles/src/iGLU-1.0.0/libutil/registry.c

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DFTGL_LIBRARY_STATIC
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/freetype
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/ftgles/src

LOCAL_STATIC_LIBRARIES := freetype

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

include $(CLEAR_VARS)

LOCAL_MODULE    := lua
LOCAL_SRC_FILES :=  luajit/libluajit.a
LOCAL_EXPORT_C_INCLUDES :=  $(LOCAL_PATH)/luajit/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := soil
LOCAL_SRC_FILES :=  soil/libsoil.a
LOCAL_EXPORT_C_INCLUDES :=  $(LOCAL_PATH)/soil/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_MODULE    := godpatterns
LOCAL_STATIC_LIBRARIES := lua soil FTGLES
LOCAL_CFLAGS    := -Werror -DFTGL_LIBRARY_STATIC
LOCAL_SRC_FILES := \
    DDLog_Android.cpp \
    ../../reflect/Field.cpp \
    ../../reflect/Reflect.cpp \
    ../../GraphicsPipeline.cpp \
    ../../LuaState.cpp \
    ../../Game.cpp \
    ../../input/Button.cpp \
    ../../input/Touch.cpp \
    ../../input/Mouse.cpp \
    ../../input/Keyboard.cpp \
    ../../Vector.cpp \
    ../../Matrix.cpp \
    ../../Sprite.cpp \
    ../../System.cpp \
    ../../Renderer.cpp \
    ../../SaveGame.cpp \
    ../../Texture.cpp \
    ../../TextureManager.cpp \
    ../../Http.cpp \
    ../../HttpPostData.cpp \
    ../../DDTime.cpp \
    DDAudio_Android.cpp \
    ../../DDRestful_Common.cpp \
    DDRestful_Android.cpp \
    ../../Sound.cpp \
    ../../SoundStream.cpp \
    ../../Asset.cpp \
    ../../AssetStore.cpp \
    ../../ManifestAssetStore.cpp \
    ../../Dinodeck.cpp \
    ../../FormatText.cpp \
    AndroidWrapper.cpp \
    DDFile_Android.cpp \
    godpatterns_android.cpp \
    DDLuaCallbacks.cpp \
    ../../ScoreLoop/ScoreLoop.cpp \


LOCAL_LDLIBS    := -llog -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)