#!/bin/sh
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This will compile the executable for the specified target.
# gcc and clang are two targets for linux using the respective toolchain.
# mingw uses gcc to cross-compile.
#
# This script does not compile documentation, it is only intended to check
# whether the executable compilation produces warnings with specific toolchains.
#
# NOTE: As I always forget how to see cmake options
# http://cmake.org/cmake/help/v2.8.12/cmake.html#command:option
# cmake -LH
# setting options: -D<option>=<value>

# User settings
prog=dpps
REP=/mnt/stockage/jerome/travail/INL/projets/C/20131117-masques/
makeopts="-j4" #VERBOSE=1
# end of user settings

function help {
    echo -e "Syntax: " $0 " clean\n"
    echo    "   will clean most files produced by the build including cmake cache"
    echo    "   but not the executables (because we want to produce them all)"
    echo    "   (not font_* since I still do not know to rebuild them well)"
    echo -e "        " $0 " <clang|gcc|mingw64|scan-build|all>\n\n"
    echo    "It will clean previous compilation and configure files,"
    echo -e "then compiles for the provided target.\n"
    echo    "Final executable is copied to ${prog}-target, where target is one"
    echo    "of the possible targets given above."
    echo -e "If argument \"all\" is passed, all of the targets are compiled.\n"
    echo    "Description of the targets:"
    echo    "   gcc: compiles with g++, with -mtune=native"
    echo    "        This is for use on one's machine"
    echo    "   clang: compiles with clang++, mostly to check warnings"
    echo    "   mingw32: cross-compiles with mingw as a target format PE32"
    echo    "        using gentoo linux cross-development platform"
    echo    "        This is to distribute to 32bit windows users."
    echo    "   mingw64: cross-compiles with mingw as a target format PE32+"
    echo    "        using gentoo linux cross-development platform"
    echo    "        This is to distribute to 64bit windows users."
    echo    "   scan-build: calls the clang static analyzer."
    echo    "        This will not create an executable, only a bug report."
    echo    "gcc and clang will be compiled using host architecture"
    echo    "(32 or 64 bits), which for the author is 64 bits. mingw64 uses a"
    echo    "64 bits target."
    exit
}

function clean_all {
    for i in ${REP}/bin ${REP}/src ; do
        cd ${i}
        make clean 2> /dev/null
        rm -f CMakeCache.txt CPackConfig.cmake CPackSourceConfig.cmake Makefile cmake_install.cmake configure.h Doxyfile ${prog}-*.bz2
        # ${prog}-* ${prog}-*.exe
        rm -rf CMakeFiles _CPack_Packages
    done
}

function build {
    case $1 in
        "clang")
            cmake_options="-DCMAKE_TOOLCHAIN_FILE=${REP}/src/misc/Toolchain_clang.cmake"
            executable="${prog}"
            ;;
        "gcc")
            cmake_options="-DNATIVE_TUNE=1"
            executable="${prog}"
            ;;
        "scan-build")
            cmake_options="-DCMAKE_TOOLCHAIN_FILE=${REP}/src/misc/Toolchain_clang.cmake"
            executable="${prog}"
            ;;
        "mingw64")
            cmake_options=-DCMAKE_TOOLCHAIN_FILE="${REP}/src/misc/Toolchain_gentoo_crossdev_mingw64.cmake"
            executable="${prog}.exe"
            ;;
        "mingw32")
            cmake_options=-DCMAKE_TOOLCHAIN_FILE="${REP}/src/misc/Toolchain_gentoo_crossdev_mingw32.cmake"
            executable="${prog}.exe"
            ;;
        *)
            help
            ;;
    esac
    echo "Preparing for $1"
    clean_all
    cd ${REP}/bin
    cmake ${cmake_options} ${REP}/src

    case $1 in
        "scan-build")
            scan-build make VERBOSE=1
            ;;
        *)
            make ${makeopts} && cd ${REP}/bin && rename ${prog} ${prog}-$1 ${executable}
            ;;
    esac
}

case $# in
    1) target=$1
    ;;
    *)
    help
    ;;
esac

if [ "${target}" == "clean" ]
then
    clean_all
    exit
fi

if [ "${target}" == "all" ]
then
    # finishing with gcc so that we are left with the build files
    # for my normal build environment
    for i in scan-build clang mingw32 mingw64 gcc ; do
        build ${i}
    done
else
    build "${target}"
fi
