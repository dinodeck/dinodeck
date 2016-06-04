## Dinodeck is dynamically linking some files

libpng16.16.dylib is referened by libfreetype.6.dylib

So libfreetype needs making static and that might solve that issue.

To test I've just been renaming the files temporarily

    sudo mv /opt/local/lib/libpng16.16.dylib /opt/local/lib/libpng16.16.dylib_

## Detecting Dependancies

This command seems to do the trick

    otool -L dinodeck

## OpenAL console errors

https://github.com/libgdx/libgdx/issues/3572