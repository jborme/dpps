set (CMAKE_C_COMPILER clang)
set (CMAKE_CXX_COMPILER clang++)
set (COMPILER_OPTIONS "-O2 -std=c++11 -Wall -Wextra -pedantic")

# I ran into big trouble with cmake and clang 3.3: linker would not work.
# (although simple Makefile used to work)
# Checked the following messages and tried out their solutions, but no way.
# * http://mhungerford.blogspot.pt/2010/10/cmake-and-clangllvm-fun.html
# * http://stackoverflow.com/questions/10286850/using-llvm-linker-when-using-clang-cmake
# I did not investigate further, all magically worked with llvm/clang 3.4.
