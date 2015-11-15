/**
 * @file Reader.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief The abstract class to read from files
 * @date 2013-11-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "Reader.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

#include <iostream>
dpps::Reader::Reader () {
}

dpps::Reader::~Reader () {
    close () ;
}

void dpps::Reader::close () {
    if (file. is_open ()){
        file. close () ;
        if (file. bad()) {
            std::string reason {"Reader: tmpfile " + filename +
                                " could not be closed"} ;
            throw bad_io (reason. c_str ()) ;
        }
    }
}

void dpps::Reader::analyze_metadata () {
}

std::string dpps::Reader::metadata () const {
    return metadata_string ;
}

void dpps::Reader::set_parametres (const std::vector<bool> &vbool,
                     const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 0) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Abstract Reader::set_parametres",
            0, 0, 0, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
}

void dpps::Reader::append_to_pattern (Pattern &p) {
    bool ok {true} ;
    while (ok) {
        Polyline q ;
        ok = read_polyline (q) ;
        //std::cerr << "final polyline (ok = " << ok << ") " << q.display_string() <<"\n";
        if (ok) {
            p. push_back (q) ;
        }
    }
}
