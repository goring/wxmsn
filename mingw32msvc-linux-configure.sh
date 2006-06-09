#!/bin/bash
export CC=i586-mingw32msvc-gcc 
export LD=i586-mingw32msvc-ld 
export AR=i586-mingw32msvc-ar 
export AS=i586-mingw32msvc-as 
export NM=i586-mingw32msvc-nm 
export STRIP=i586-mingw32msvc-strip 
export RANLIB=i586-mingw32msvc-ranlib 
export DLLTOOL=586-mingw32msvc-dlltool 
export OBJDUMP=i586-mingw32msvc-objdump
export LIBS="-lws2_32 -mwindows"
./configure --with-wx-config=/usr/i586-mingw32msvc/bin/wx-config --disable-openssl --prefix=/usr/i586-mingw32msvc --host=i586-mingw32msvc --target=i586-mingw32msvc --with-libgnutls-prefix=/usr/i586-mingw32msvc
