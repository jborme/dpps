# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# BUG: it always calculates data_vollkorn_normal,
# and then it always compiles Pattern_text.cpp.o and relinks.
# same thing with the swig interface
# I commented:    #add_dependencies (dpps ${FONT})
# so one needs to make data_vollkonrn_normal before to do make all

# vérifier qu'on a bien lua52 parce que sinon ça va foirer.
# http://stackoverflow.com/questions/3249459/for-the-cmake-include-command-what-is-the-difference-between-a-file-and-a-mod

# Note that it won't compile in clang if gcc updated to 4.9 due to some
# incompatibility in libstdc++ of gcc-4.9 and clang.
#
#In file included from /usr/include/png++/pixel_buffer.hpp:35:
#/usr/lib/gcc/x86_64-pc-linux-gnu/4.9.0/include/g++-v4/cstddef:51:11: error: no
#      member named 'max_align_t' in the global namespace
#  using ::max_align_t;
# See similar report in a distro:
# https://bugs.archlinux.org/task/40229

################################
##           Project          ##
################################
cmake_minimum_required (VERSION 2.8.10)

option (USE_CLI "Compiles with TCLAP for full-feature command line support" 1)
option (USE_QT  "Links to Qt4 for a large variety of image format loading" 1)
option (USE_PNG "Links to libpng/png++ image loading support" 1)
option (USE_GSL "Links to GNU GSL to calculate homography coefficients; this changes licence to GPLv3+" 1)
option (NATIVE_TUNE "Passes -mtune=native to the compiler" 0)
option (GCC_OPENMP "Links to the parallel version of gcc libstdc++" 0)
option (MAKE_MODULE "Compiles the lua module, not activated by default because the author cannot test it" 0)
mark_as_advanced (MAKE_MODULE)

# set (LUA_DIR "-I/usr/include/lua"
#              "-I/usr/include/lua5.2"
# )
project (dpps)
set (dpps_VERSION_MAJOR 1)
set (dpps_VERSION_MINOR 0)
set (dpps_VERSION_PATCH 0)

################################
##           CPack            ##
################################
# The CPack directives have to be placed before the
# set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
# unless the configuration of CPack goes to the binary direction
# and the targets for package are not added to the Makefile (which is created
# in the source directory).
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A tool used at INL to generate DXF designs from a C++ class, manipulate them and to convert designs into Witec CNC code.")
set (CPACK_PACKAGE_VENDOR "Jérôme Borme")
set (CPACK_PACKAGE_CONTACT "Jérôme Borme <jerome@escreve.me>")
set (CPACK_PACKAGE_VERSION_MAJOR ${dpps_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${dpps_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${dpps_VERSION_PATCH})
set (VERSION ${dpps_VERSION_MAJOR}.${dpps_VERSION_MINOR}.${dpps_VERSION_PATCH})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
"/_CPack_Packages/"
"/CMakeFiles/"
"cmake_install.cmake"
"CPackConfig.cmake"
"CMakeCache.txt"
"Makefile.txt"
"CPackSourceConfig.cmake"
"install_manifest.txt"
"dpps-*"
".*.kate-swp"
)
include (CPack)

#set (lua_module_name dpps_lua)
set (wrap_spec "dpps.i")
set (wrap_cxx_file "dppsLUA_wrap.cxx")

################################
##        Directories         ##
################################
set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# The PROJECT_BINARY_DIR is used by CPack for the binary package
set (PROJECT_BINARY_DIR ${CMAKE_BINARY_DIR})

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

################################
##        Dependencies        ##
################################

##      Dependencies: lua     ##
################################
# We are using FindLua52 from luadist github repository
include (FindLua52)
if (LUA_FOUND)
    set (link_to "${link_to}"
        "-llua"
    )
    add_definitions (${LUA_DIR})
    # add_definitions ("-I /usr/include/lua5.2")
    message ("-- Found lua" ${LUA_DIR})
else ()
    message (FATAL_ERROR "-- Looking for lua - not found.")
endif ()

include (FindDoxygen)
include (FindLATEX)

##     Dependencies: tclap    ##
################################
# if (USE_CLI)
#     find_path (TCLAP_INCLUDE_DIR tclap/CmdLine.h
#         PATHS /usr/include /usr/local/include)
#     if (TCLAP_INCLUDE_DIR)
#         message ("-- Found tclap: " ${TCLAP_INCLUDE_DIR})
#         set (TCLAP_FOUND TRUE)
#     else ()
#         message ("-- Looking for tclap - not found.")
#         set (TCLAP_FOUND FALSE)
#     endif ()
# else ()
#     set (TCLAP_FOUND FALSE)
# endif ()

##      Dependencies: Qt      ##
################################
# This is enough for QtCore and QtGui
# https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
#if (DEFINED CROSSDEV_MINGW)
#else ()
#endif ()
if (USE_QT)
    find_package (Qt4)
    if (${QT_FOUND})
        include (${QT_USE_FILE})
        add_definitions (${QT_DEFINITIONS})
        set (link_to "${link_to}"
            "${QT_LIBRARIES}")
    endif ()
else ()
    set (QT_FOUND FALSE)
endif ()

##    Dependencies: pnglib    ##
################################
if (USE_PNG)
    include (FindPNG)
    if (PNG_FOUND)
        add_definitions (${PNG_DEFINITIONS})
        set (link_to "${link_to}"
            ${PNG_LIBRARIES}
        )
        find_path (PNGPP_INCLUDE_DIR png.hpp
            PATHS /usr/include/png++ /usr/local/include/png++ /usr/include /usr/local/include)
        if (PNGPP_INCLUDE_DIR)
            set (PNGPP_FOUND TRUE)
            include_directories (${PNGPP_INCLUDE_DIR})
            message ("-- Found png++: " ${PNGPP_INCLUDE_DIR})
        else ()
            message ("-- Looking for png++ - not found")
            set (PNGPP_FOUND FALSE)
        endif ()
    else ()
        set (PNGPP_FOUND FALSE)
    endif ()
else ()
    set (PNGPP_FOUND FALSE)
endif ()

##      Dependencies: gsl     ##
################################
if (USE_GSL)
    find_path (TCLAP_INCLUDE_DIR gsl/gsl_version.h
        PATHS /usr/include /usr/local/include)
    if (TCLAP_INCLUDE_DIR)
        message ("-- Found gsl: " ${TCLAP_INCLUDE_DIR})
        set (GSL_FOUND TRUE)
        set (link_to "${link_to}"
            "-lgsl"
        )
    else ()
        message ("-- Looking for gsl - not found.")
        set (GSL_FOUND FALSE)
    endif ()
else ()
    set (TCLAP_FOUND FALSE)
endif ()

##     Dependencies: SWIG     ##
################################

include (FindSWIG_cross)
set_source_files_properties (dpps.i PROPERTIES CPLUSPLUS ON)
# I give up on using parameters,
# http://stackoverflow.com/questions/17043088/cmake-build-calling-swig-with-multiple-arguments
# I pass them on the custom target below.
#
#
#set_source_files_properties (dpps.i PROPERTIES SWIG_FLAGS "-includeall") #-Wall -Wextra
#set_property (SOURCE dpps.i PROPERTY SWIG_FLAGS "-includeall") #"-Wall" "-Wextra"
#set(CMAKE_SWIG_FLAGS "-includeall")
include (${SWIG_USE_FILE})
if (${MAKE_MODULE})
    swig_add_module (${lua_module_name} lua dpps.i)
    swig_link_libraries (dpps_lua ${LUA_LIBRARIES})
endif()
set_source_files_properties (${wrap_cxx_file} GENERATED)
add_custom_target (lua_wrap
    swig -c++ -lua -Wall -Wextra -includeall -o ${wrap_cxx_file} ${CMAKE_CURRENT_SOURCE_DIR}/${wrap_spec}
#-Wall -Wextra
    DEPENDS ${wrap_spec}
)

################################
##        Main settings       ##
################################

configure_file ("${PROJECT_SOURCE_DIR}/configure.h.in"
                "${PROJECT_BINARY_DIR}/configure.h")

# add the binary directory to the -I option of the compiler
# so that generated files are visible.
include_directories (
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/Classes"
    "${PROJECT_SOURCE_DIR}/main"
    "${PROJECT_SOURCE_DIR}/util"
    "${PROJECT_SOURCE_DIR}/font_data"
)

set (SOURCES "${SOURCES}"
    "Classes/Pattern.cpp"
    "Classes/Pattern_factory.cpp"
    "Classes/Pattern_Fibonacci.cpp"
    "Classes/Pattern_grating.cpp"
    "Classes/Pattern_hard_sphere_random_packing.cpp"
    "Classes/Pattern_Penrose_tiling.cpp"
    "Classes/Pattern_random.cpp"
    "Classes/Pattern_rectangular_lattice.cpp"
    "Classes/Pattern_text.cpp"
    "Classes/Polyline.cpp"
    "Classes/Polyline_factory.cpp"
    "Classes/Reader.cpp"
    "Classes/Reader_DXF_R12.cpp"
    "Classes/Reader_image.cpp"
    "Classes/Reader_witec.cpp"
    "Classes/Typeface.cpp"
    "Classes/Vertex.cpp"
    "Classes/Writer.cpp"
    "Classes/Writer_cpp.cpp"
    "Classes/Writer_DXF_R12.cpp"
    "Classes/Writer_SVG.cpp"
    "Classes/Writer_witec.cpp"
    "main/interpret_lua.cpp"
    "main/main.cpp"
    "main/preprocessor.cpp"
    "util/common.cpp"
    "util/util.cpp")

if (${PNGPP_FOUND})
    set (SOURCES "${SOURCES}"
         "Classes/Reader_image_png.cpp")
endif ()
if (${QT_FOUND})
    set (SOURCES "${SOURCES}"
         "Classes/Reader_image_Qt.cpp")
endif ()
if (${GSL_FOUND})
    set (SOURCES "${SOURCES}"
         "util/Matrix2x2.cpp"
         "util/Matrix3x3.cpp")
endif ()
set (SOURCES "${SOURCES}"
    "${wrap_cxx_file}")

add_executable (dpps ${SOURCES})
add_dependencies (dpps lua_wrap)

################################
##        Documentation       ##
################################
# This facultative target to get html and latex reference manual
if (DOXYGEN_FOUND)
    # idea adapted from:
    # https://majewsky.wordpress.com/2010/08/14/tip-of-the-day-cmake-and-doxygen/
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/doxy.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (htmldoc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    # The findLatex package does not provide LATEX_FOUND
    # There are more advanced packages that define this variable
    # https://github.com/fhoefling/halmd/blob/master/cmake/FindLatex.cmake
    # but we do not need this. All we need to know is if PDFLATEX_COMPILER was
    # defined.
    if (PDFLATEX_COMPILER)
        add_custom_target (doc
            DEPENDS htmldoc
        )
    else ()
        add_custom_target (doc
            make
            DEPENDS htmldoc
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex
        )
    endif ()
endif ()

################################
##       Local generator      ##
################################
# target to produce a C++ header from our SVG output of a font.
#set (FONT_NAMES
#    "data_vollkorn_normal"
#    #"data_vollkorn_italic.in"
#)
#foreach (FONT ${FONT_NAMES})
#    set_source_files_properties (${FONT}.hh GENERATED)
#    add_custom_target (${FONT}
#        # There seem to be no way to put a dollar sign
#        #http://www.cmake.org/pipermail/cmake/2008-January/019242.html
#        #cat ${CMAKE_CURRENT_SOURCE_DIR}/${FONT}.in | sed -e "s/L/},{/g" -e "s/^M/{{{/g" -e "s/M/}},{{/g" -e "/^{/ s/${DOLLAR_LITERAL}/}}},/" -e "$ s/,$//" > ${CMAKE_CURRENT_BINARY_DIR}/${FONT}.hh
#        sh -c "process_data_inkscape.sh ${CMAKE_CURRENT_SOURCE_DIR}/${FONT}.in"  > ${CMAKE_CURRENT_BINARY_DIR}/${FONT}.hh
#        OUTPUT ${FONT}.hh
#        DEPENDS ${FONT}.in
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#    )
#    #add_dependencies (dpps ${FONT})
#endforeach ()

################################
##      Compiler settings     ##
################################
if (COMPILER_OPTIONS)
    # set in toolchain file (clang, mingw32, mingw64)
    set (CC_OPTIONS "${COMPILER_OPTIONS}")
else()
    # gcc linux build // -O0 -ggdb3
    set (CC_OPTIONS "-O2 -std=c++11 -Wall -Wextra -pedantic")
endif()

if (GCC_OPENMP)
# https://gcc.gnu.org/onlinedocs/gcc-4.9.0/libstdc%2B%2B/manual/manual/parallel_mode_using.html#parallel_mode.using.prereq_flags
    set (CC_OPTIONS "${CC_OPTIONS} -D_GLIBCXX_PARALLEL")
    set (link_to "${link_to}"
        "-fopenmp")
endif()
target_link_libraries (dpps ${link_to})

if (NATIVE_TUNE)
    set (CC_OPTIONS "${CC_OPTIONS} -mtune=native")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
        message (FATAL_ERROR "gcc 4.7 or higher is required to build " ${CMAKE_PROJECT_NAME})
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message (FATAL_ERROR "clang 3.3 misses certain functions we use (string::pop_back), update to clang 3.4")
    endif ()
else ()
    message (WARNING "Compilation of " ${CMAKE_PROJECT_NAME} " was tested only with gcc and clang.")
endif ()

set_target_properties (dpps PROPERTIES COMPILE_FLAGS ${CC_OPTIONS})

################################
##        Installation        ##
################################
install (TARGETS dpps
         RUNTIME DESTINATION bin)
install (DIRECTORY ${PROJECT_BINARY_DIR}/html DESTINATION doc/dpps)
install (FILES ${PROJECT_BINARY_DIR}/latex/refman.pdf DESTINATION doc/dpps)
