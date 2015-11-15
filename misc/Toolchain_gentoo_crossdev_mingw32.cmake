set (CMAKE_SYSTEM_NAME Windows)

# To build on gentoo with crossdev --ov-output mingw32
set (CMAKE_C_COMPILER mingw32-gcc)
set (CMAKE_CXX_COMPILER mingw32-g++)
set (CMAKE_RC_COMPILER mingw32-windres)

set (CMAKE_FIND_ROOT_PATH /usr/mingw32)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Local settings in the case of mingw cross-compilation
set (NO_ERRHANDLING_ERRNO 1)
set (FILENAMES_FOR_WINDOWS 1)
# broken above -O0
set (COMPILER_OPTIONS "-O0 -std=c++11 -Wall -Wextra -pedantic")