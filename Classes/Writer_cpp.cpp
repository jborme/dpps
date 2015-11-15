/**
 * @file Writer_cpp.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Writer derivative that uses the C++ initialization list, usable
 * to integrate into this program with a provided transformation function.
 * @date 2013-03-14 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The final type is vector<vector<vector<pair<double,double>>>>>
 * The pair represents a (x, y) vertex.
 * A vector of pairs (stroke) represents a polyline.
 * A vector of polyline is called here element, as character in
 * font_declarations.hh.
 *
 * This type was chosen because it is very similar to vector of Vertex and
 * Polylines, allows easy translation to these types, and also allows to be
 * used with initialization lists
 *
 * This function always writes element that contain a single stroke, but the
 * structure was chosen to allow compatibility with the definitions in
 * font_definitions.hh.
 *
 * A typical configuration file could be:
 * @example
Reader_DXF_R12::set_parametres 1 0 0 * input.dxf
Writer_cpp::set_parametres output.cpp
Pattern::write_to
Writer::finish

 * The writer produces a definition of a variable called output.
 * the user willing to use it in a program would create a class derived from
 * Pattern, for example Pattern_my_pattern, and implement generate() as the
 * following:
 * @example
void dpps:Pattern_my_pattern::generate () {
std::vector<std::vector<std::vector<std::pair<double,double>>>> output {{
// copy-paste the initialization list produced by this writer
}} ;
std::vector<dpps::Polyline> my_polylines {
    Polyline_factory::from_initialization_list (output)} ;
    // polylines is in class Pattern
    polylines. insert (polylines. end (),
        my_polylines.begin (), my_polylines. end ()) ;
}

 * This writer is intended to be used to ease integration of external shapes
 * into the C++ code. If the user has a DXF R12 file, this writer
 *
 * */

#include <utility> // std::pair

#include "Writer_cpp.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

dpps::Writer_cpp::Writer_cpp (
    const std::string &set_filename,
    const bool append): Writer (set_filename, append) {}

void dpps::Writer_cpp::write_header () {
    if (first)
        file << "{" ;
    else {
        first = true ;
        file << ",{" ;
    }

    //file << "std::vector<std::vector<std::vector<std::pair<double,
//double>>>> output {{" << std::endl ;
}
void dpps::Writer_cpp::write_footer () {
    file << "}\n" ;
    //file << std::endl << "}} ;" << std::endl ;
}

void dpps::Writer_cpp::set_all_parametres () {}

void dpps::Writer_cpp::set_parametres (
                        const std::vector<bool> &vbool,
                        const std::vector<long_unsigned_int> &vint,
                        const std::vector<double> &vdouble,
                        const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 0) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Writer_cpp::set_parametres",
            0, 0, 0, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    set_all_parametres () ;
}

void dpps::Writer_cpp::write_Polyline (const Polyline &polyline) {
    if (first)
        first = false ;
    else
        file << "," << std::endl ;
    file << "{" ;
    for (long_unsigned_int i = 0 ; i < polyline. vertices. size () ; i++) {
        file << "{" << polyline. vertices[i]. x << ","
                    << polyline. vertices[i]. y << "}" ;
        if (i != polyline. vertices. size () - 1)
            file << "," ;
    }
    file << "}" ;
}
