/**
 * @file Writer.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation of virtual class Writer
 * @date 2013-11-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "configure.h"

#include <cstdio> // remove

#include "Writer.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"
#include <iostream> // for debug

#include "bad_parametre.hh"

dpps::Writer::Writer (const std::string &set_filename,
                      const bool set_append):
    filename (set_filename), append (set_append) {
    if (set_append)
        file. open (filename, std::ios_base::out|std::ios_base::app) ;
    else
        file. open (filename, std::ios_base::out|std::ios_base::trunc) ;
    if (!file. is_open ()) {
        std::string reason {"File " + filename +
                            " could not be open for output"} ;
        throw bad_io (reason. c_str ()) ;
    }
    // tmpnam returns a pointer to an internal static buffer.
    // I prefer to pipe it to an explicit adapter to make sure memory
    // is properly handled.
    tmpfilename = std::string (tmpnam (nullptr)) ;
#ifdef FILENAMES_FOR_WINDOWS
    tmpfilename = ".\\" + tmpfilename ;
#endif
    tmpfile. open (tmpfilename, std::ofstream::out|std::ofstream::trunc) ;
    if (!tmpfile. is_open ()) {
        std::string reason {"Writer: file " + tmpfilename +
                            " could not be open for output"} ;
        throw bad_io (reason. c_str ()) ;
    }
}

void dpps::Writer::write_header () {
}

void dpps::Writer::write_footer () {
}

dpps::Writer::~Writer () {
    if (open)
        close () ;
}

void dpps::Writer::close () {
    if (file. is_open ()) {
        file. close () ;
        if (file. bad()) {
            std::string reason {"Writer: file " + filename +
                                " could not be closed"} ;
            throw bad_io (reason. c_str ()) ;
        }
    }
    if (tmpfile. is_open ()) {
        tmpfile. close () ;
        if (tmpfile. bad()) {
            std::string reason {"Writer: tmpfile " + tmpfilename +
                                " could not be closed"} ;
            throw bad_io (reason. c_str ()) ;
        }
    }
    if (remove (tmpfilename. c_str ()) != 0) {
        std::string reason {"Writer: file " + tmpfilename +
                            " could not be removed"} ;
        throw bad_io (reason. c_str ()) ;
    }
    open = false ;
}

void dpps::Writer::set_parametres (const std::vector<bool> &vbool,
                                   const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 0) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Base Writer::set_parametres",
            0, 0, 0, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
}

void dpps::Writer::write_Pattern (const Pattern &pattern) {
    write_header () ;
    for (auto p: pattern. polylines)
        write_Polyline (p) ;
    write_footer () ;
}

void dpps::Writer::write_Pattern_selected (const Pattern &pattern,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    write_header () ;
    for (auto p: pattern. polylines)
        if ((selection < 0) || (p. selected[selection]))
            write_Polyline (p) ;
    write_footer () ;
}