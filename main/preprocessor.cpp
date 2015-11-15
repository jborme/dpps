/**
 * @file preprocessor.cpp
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Simple preprocessor for the lua files.
 * @date 2014-06-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * */

#include <fstream>
#include <string> // getline is in <string>

#include "configure.h" // created by cmake
#include "common.hh"
#include "preprocessor.hh"
#include "util.hh"

#include "bad_parametre.hh"
#include "bad_io.hh"

std::string preprocess_file (
    const std::string &configuration_filename,
    std::vector<std::string> &replacements) {
    std::ifstream infile ;
    std::ofstream tmpfile ;

    infile. open (configuration_filename, std::ios_base::in) ;
    if (!infile. is_open ()) {
        std::string reason {"preprocess_file: File " + configuration_filename +
                            " could not be open for input"} ;
        throw dpps::bad_io (reason. c_str ()) ;
    }
    std::string tmpfilename {tmpnam (nullptr)} ;
#ifdef FILENAMES_FOR_WINDOWS
    // We escape backslashes so that the user can quote the filenames the normal
    // way with " " and not [[ ]] in the lua programs.
    tmpfilename = ".\\" + tmpfilename ;
    for (auto &i : replacements) {
        dpps::replace_string (i, "\\", "/") ;
        dpps::replace_string (i, "/", "\\\\") ;
    }
#endif
    tmpfile. open (tmpfilename, std::ofstream::out|std::ofstream::trunc) ;
    if (!tmpfile. is_open ()) {
        std::string reason {"preprocess_file: File " + tmpfilename +
                            " could not be open for output"} ;
        throw dpps::bad_io (reason. c_str ()) ;
    }
    std::vector<std::string> expressions ;
    std::vector<std::string> expressions_noaffix ;
    std::vector<std::string> replacements_noaffix ;
    for (long_unsigned_int i {0} ; i < replacements. size () ; i++) {
        expressions. push_back ("#" + std::to_string (i) + "#") ;
        expressions_noaffix. push_back ("#" + std::to_string (i) + "#noaffix#") ;
        replacements_noaffix. push_back (dpps::remove_affix (replacements.at(i))) ;
        for (auto &j : replacements) {
            if (j. find (expressions. back()) != std::string::npos) {
                std::string reason {"preprocess_file: command-line parametre "
                    + std::to_string (i) + ": " + j + " contains special \
sequence not allowed: " + expressions. back()} ;
                throw dpps::bad_parametre (reason. c_str ()) ;
            }
            if (j. find (expressions_noaffix. back()) != std::string::npos) {
                std::string reason {"preprocess_file: command-line parametre "
                    + std::to_string (i) + ": " + j + " contains special \
sequence not allowed: " + expressions. back()} ;
                throw dpps::bad_parametre (reason. c_str ()) ;
            }
        }
    }
    std::string line ;
    bool stay {true} ;
    while (stay) {
        stay = getline (infile, line). good () ;
        // It will return false on the last line.
        // (actually it will say eof()==true which implies good()==false
        // We will process the last line and return.
        for (long_unsigned_int i {0} ; i < replacements. size () ; i++) {
            dpps::replace_string (line, expressions_noaffix. at (i), replacements_noaffix. at (i)) ;
            dpps::replace_string (line, expressions. at (i), replacements. at (i)) ;
        }
        tmpfile << line << "\n";
    }
    infile. close () ;
    tmpfile. close () ;
    return tmpfilename ;
}
