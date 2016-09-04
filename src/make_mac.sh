# Obviously renaming files is a horrible way to do this
# but it's the only way I could static linking working correctly
# I'm eager to be corrected with the "correct" way to do this!

if [ -e "/opt/local/lib/libSDL-1.2.0.dylib" ]
then
  mv /opt/local/lib/libSDL-1.2.0.dylib /opt/local/lib/libSDL-1.2.0.dylib_
fi

if [ -e "/opt/local/lib/libfreetype.6.dylib" ]
then
  mv /opt/local/lib/libfreetype.6.dylib /opt/local/lib/libfreetype.6.dylib_
fi

if [ -e "/opt/local/lib/libz.1.dylib" ]
then
  mv /opt/local/lib/libz.1.dylib /opt/local/lib/libz.1.dylib_
fi

if [ -e "/opt/local/lib/libz.dylib" ]
then
  mv /opt/local/lib/libz.dylib /opt/local/lib/libz.dylib_
fi

if [ -e "/opt/local/lib/libbz2.dylib" ]
then
  mv /opt/local/lib/libbz2.dylib /opt/local/lib/libbz2.dylib_
fi

if [ -e "/opt/local/lib/libbz.1.0.dylib" ]
then
  mv /opt/local/lib/libbz2.1.0.dylib /opt/local/lib/libbz.1.0.dylib_
fi

if [ -e "/usr/local/lib/libSDL-1.2.0.dylib" ]
then
  mv /usr/local/lib/libSDL-1.2.0.dylib /usr/local/lib/libSDL-1.2.0.dylib_
fi

make clean

if [ -e "./dinodeck" ]
then
  rm ./dinodeck
fi

if [ -e "./dinodeck_mac" ]
then
  rm ./dinodeck_mac
fi

make PLATFORM_DD=MAC

mv ./dinodeck ./dinodeck_mac

install_name_tool -add_rpath "@executable_path/" dinodeck_mac

if [ -e "/opt/local/lib/libSDL-1.2.0.dylib_" ]
then
  mv /opt/local/lib/libSDL-1.2.0.dylib_ /opt/local/lib/libSDL-1.2.0.dylib
fi

if [ -e "/opt/local/lib/libfreetype.6.dylib_" ]
then
  mv /opt/local/lib/libfreetype.6.dylib_ /opt/local/lib/libfreetype.6.dylib
fi

if [ -e "/opt/local/lib/libz.1.dylib_" ]
then
  mv /opt/local/lib/libz.1.dylib_ /opt/local/lib/libz.1.dylib
fi

if [ -e "/opt/local/lib/libz.dylib_" ]
then
  mv /opt/local/lib/libz.dylib_ /opt/local/lib/libz.dylib
fi

if [ -e "/opt/local/lib/libbz2.dylib_" ]
then
  mv /opt/local/lib/libbz2.dylib_ /opt/local/lib/libbz2.dylib
fi

if [ -e "/opt/local/lib/libbz.1.0.dylib_" ]
then
  mv /opt/local/lib/libbz2.1.0.dylib_ /opt/local/lib/libbz.1.0.dylib
fi

if [ -e "/usr/local/lib/libSDL-1.2.0.dylib_" ]
then
  mv /usr/local/lib/libSDL-1.2.0.dylib_ /usr/local/lib/libSDL-1.2.0.dylib
fi

    # /opt/local/lib/libX11.6.dylib (compatibility version 10.0.0, current version 10.0.0)
    # /opt/local/lib/libXext.6.dylib (compatibility version 11.0.0, current version 11.0.0)
    # /opt/local/lib/libXrandr.2.dylib (compatibility version 5.0.0, current version 5.0.0)
    # /opt/local/lib/libXrender.1.dylib (compatibility version 5.0.0, current version 5.0.0)
    # /opt/local/lib/libopenal.1.dylib (compatibility version 1.0.0, current version 1.15.1)
    # /opt/local/lib/libbz2.1.0.dylib (compatibility version 1.0.0, current version 1.0.6)
    # /usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 120.1.0)
    # /usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1226.10.1)
    # /usr/lib/libobjc.A.dylib (compatibility version 1.0.0, current version 228.0.0)
