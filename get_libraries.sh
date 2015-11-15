#!/bin/sh
# This scripts collects some libraries and copies them into a distribution
# directory. It works only on the author's machine since directories are
# hardlinked. However everyone can read it to understand what are the files
# and where they should be located on a gentoo/crossdev setup with similar
# parameters than the author's. Because lua does not compile with crossdev
# (for whatever reason) then we compiled it manually. Lua Makefile needs
# some indications about the correct compiler. (and then it is "make mingw")

REP="/mnt/stockage/jerome/travail/INL/projets/C/20131117-masques/"

REP_BIN="${REP}/bin/"

REP_LUA32="${REP}/embedded/lua-5.2.3_mingw32/src/"
REP_MINGW32="/usr/mingw32/usr/bin/"

REP_LUA64="${REP}/embedded/lua-5.2.3_mingw64/src/"
REP_MINGW64="/usr/x86_64-w64-mingw32/usr/bin/"

cd ${REP_BIN}/distrib/w64

cp ${REP_BIN}/dpps-mingw64.exe .
cp ${REP_LUA64}/lua52.dll .
cp ${REP_MINGW64}/zlib1.dll .
cp ${REP_MINGW64}/libpng16-16.dll .
cp ${REP_MINGW64}/libgsl-0.dll .
cp ${REP_MINGW64}/libgslcblas-0.dll .

# mingw-w64 from sourceforge.
# C:\Program Files\mingw-w64\x86_64-4.9.1-posix-seh-rt_v3-rev0\mingw64\x86_64-w64-mingw32\lib
# files which are needed:
# - libgcc_s_seh-1.dll
# - libwinpthreads-1.dll
# - libstdc++-6.dll

# C:\Program Files (x86)\mingw-w64\i686-4.9.1-posix-sjlj-rt_v3-rev0\mingw32\i686-w64-mingw32\lib
# files which are needed:
# - libgcc_s_sjlj-1.dll
# - libwinpthreads-1.dll
# - libstdc++-6.dll

cd ${REP_BIN}/distrib/w32

cp ${REP_BIN}/dpps-mingw32.exe .
cp ${REP_LUA32}/lua52.dll .
cp ${REP_MINGW32}/zlib1.dll .
cp ${REP_MINGW32}/libpng16-16.dll .
cp ${REP_MINGW32}/libgsl-0.dll .
cp ${REP_MINGW32}/libgslcblas-0.dll .
