#g++
CC=g++
CFLAGS=-c -Wall -Dmain=SDL_main
LDFLAGS=-L/usr/local/lib -lSDLmain -lSDL
SOURCES= \
	../lib/mongoose/mongoose.c \
	./audio/Wave.cpp \
	./input/Button.cpp \
	DDFile_Windows.cpp \
	LuaState.cpp \
	./reflect/Field.cpp \
    ./reflect/Reflect.cpp \
	./input/Keyboard.cpp \
	./FrameBuffer.cpp \
	Vector.cpp \
	./input/Mouse.cpp \
	./input/Touch.cpp \
	TextureManager.cpp \
	Texture.cpp \
	System.cpp \
	Sprite.cpp \
	DDAudio_Windows.cpp \
	Sound.cpp \
    SoundStream.cpp \
    SaveGame.cpp \
    Matrix.cpp \
    DDRestful_Common.cpp \
    DDRestful_Windows.cpp \
    Http.cpp \
    HttpPostData.cpp \
	Main.cpp \
	Dinodeck.cpp \
	DDLog_Windows.cpp \
	DDTime.cpp \
	WebServer.cpp \
	Character.cpp \
    FormatText.cpp \
	GraphicsPipeline.cpp \
	Game.cpp \
	Asset.cpp \
	ManifestAssetStore.cpp \
	AssetStore.cpp \
	Renderer.cpp \
	util/Lerp.cpp

ifeq (${PLATFORM_DD},WINDOWS)

  SOURCES:=../lib/glee/glee.c $(SOURCES)

  CFLAGS+= \
  -I../lib/LuaJIT/src \
  -I../lib/physfs  \
  -I../lib/soil/src \
  -I../lib/ftgl/src \
  -I../lib/freetype/include \
  -I../lib/mongoose \
  -I../lib/openal-soft/include \
  -I../lib/glee

  LDFLAGS:=-Wl,-Bstatic -L/usr/local/lib -lmingw32 $(LDFLAGS)

  LDFLAGS+=-liconv -lm -luser32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
  LDFLAGS+=../lib/soil/lib/libSOIL.a
  LDFLAGS+=../lib/ftgl/src/libftgl_static.a
  LDFLAGS+=../lib/freetype/src/libfreetype.a
  LDFLAGS+=-lopengl32 -lws2_32
  LDFLAGS+=../lib/LuaJIT/src/libluajit.a
  LDFLAGS+=../lib/physfs/libphysfs.a
  #LDFLAGS+=../lib/openal-soft/lib/Win32/libOpenAL32.dll.a
  LDFLAGS+=../lib/openal-soft/build/libOpenAL32.dll.a
endif

ifeq (${PLATFORM_DD},MAC)
	# This is linking to Carbon, which is deprecated I think
	SDLLIBS= -Bstatic `/usr/local/bin/sdl-config --static-libs`

	CFLAGS+= \
	-I../lib/LuaJIT/src \
	-I../lib/physfs/ \
	-I../lib/soil/src \
	-I../lib/freetype/include

	CFLAGS+= -I/opt/local/include -I/opt/local/include/luajit-2.0
	CFLAGS+= -I/opt/X11/include/freetype2
   	CFLAGS+= -I../lib/ftgl/src
	CFLAGS+= -I../lib/mongoose
	CFLAGS+= -I../lib/openal-soft/include

# https://www.eyrie.org/~eagle/notes/rpath.html


	LDFLAGS+= -framework IOKit
	LDFLAGS+= -framework CoreAudio
	# Do I need this?
	LDFLAGS+= `llvm-config --ldflags --system-libs --libs all`
	LDFLAGS+=-lz -pagezero_size 10000 -image_base 100000000
	LDFLAGS+=../lib/LuaJIT/src/libluajit.a
	LDFLAGS+= $(SDLLIBS)
	LDFLAGS+= -L/opt/local/lib -framework OpenGL -framework Cocoa


	LDFLAGS+=/opt/local/lib/libphysfs.a
	LDFLAGS+=/opt/local/lib/libSOIL.a
	LDFLAGS+=/opt/local/lib/libftgl.a
	# LDFLAGS+=/opt/local/lib/libopenal.dylib
	# LDFLAGS+=../lib/openal-soft/build/libcommon.a
	LDFLAGS+=./libopenal.dylib
	 # -install_name @rpath/t/libblah.dylib -o libblah.dylib
	# LDFLAGS+= -L/opt/X11/lib -lfreetype -lbz2
	LDFLAGS+= -L/opt/X11/lib

	LDFLAGS+=/opt/local/lib/libpng16.a
	LDFLAGS+=/opt/local/lib/libfreetype.a -lfreetype
	# Is libz required at all?
	LDFLAGS+=/opt/local/lib/libz.a -lz
	LDFLAGS+=/opt/local/lib/libbz2.a -lbz2
endif


OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dinodeck

default: mingw

mingw: $(SOURCES) $(EXECUTABLE)

clean:
	rm *.o *.exe

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $? $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@