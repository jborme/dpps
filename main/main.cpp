/**
 * @file main.cpp
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Main program, reads a configuration file and executes commands.
 * @date 2013-12-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * */

#include "configure.h"

#include <cstdlib> // EXIT_SUCCESS
#include <cstdio> // remove()
#include <iostream>
#include <string>

#include "bad_io.hh"
#include "interpret_lua.hh"
#include "preprocessor.hh"

int main (int argc, char** argv) {
    std::string configuration_filename ;
    std::vector<std::string> preprocessor_expressions ;

    if (argc < 2) {
        std::cout
        << "dpps, a design process tool using lua, written starting from "
        << "2013-11-30 by Jérôme Borme <jerome@escreve.me>.\n"
        << "Source code is ditributed under the Mozilla public licence 2.0. "
#ifdef GSL_FOUND
        << "This binary version was compiled with GSL and is therefore "
        << "submitted to the GNU GPL version 3.0 as published by the Free "
        << "software foundation or, at your option, any later version\n."
#endif
        << "Syntax:\n    dpps LUAFILE EXPR1 EXPR2...\n"
        << "    where EXPR1 EXPR2 are preprocessor expressions\n" ;
        return EXIT_SUCCESS ;
    }
    configuration_filename = argv [1] ;
    for (int i = 1 ; i < argc ; i++) {
        preprocessor_expressions. push_back (argv[i]) ;
    }
    std::string temp_file {
        preprocess_file (configuration_filename, preprocessor_expressions)} ;
    read_lua_file (temp_file) ;
    if (remove (temp_file. c_str ()) != 0) {
        std::string reason {"main: file " + temp_file +
                            " could not be removed"} ;
        throw dpps::bad_io (reason. c_str ()) ;
    }
    return EXIT_SUCCESS ;
}