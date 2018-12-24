/**
 * @file Reader_DXF_R12.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A simple DXF R12 reader
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This reader works with files from DXF R12.
 * In particular, AutoCAD 2012 LT, Librecad 2.0.x, LayoutEditor GPL 20110914
 * and pstoedit 3.61 produce files readable by this software, when saved into
 * DXF R12. qcad 2.0.5 (legacy Qt3) works, but the more recent qcad 3.2 will
 * not, as it uses DXF R15. Files produced by Inkscape (as of 0.48) will not
 * work as teh underlying library Cairo (tested 1.12.14) uses DXF R14 (also,
 * Inkscape seems to break polylines into individual lines, effectively killing
 * possibility of closed areas).
 * Note: pstoedit syntax is: pstoedit file.eps -f dxf > file.dxf
 *
 */

#include <algorithm>
#include <istream>
#include <iostream>

#include "Reader_DXF_R12.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"
#include "util.hh"

dpps::Reader_DXF_R12::Reader_DXF_R12 (const std::string &set_filename) {
    filename = set_filename ;
    file. open (filename, std::ios_base::in) ;
    if (!file. is_open ()) {
        std::string reason {"File " + filename +
                            " could not be open for input"} ;
        throw bad_io (reason. c_str ()) ;
    }
}

dpps::Reader_DXF_R12::Reader_DXF_R12 (
    const std::string &set_filename,
    const bool set_reference_from_layer,
    const bool include_blocks,
    const enum_regex regex_grammar,
    const std::string &layers_to_read): Reader_DXF_R12 (set_filename) {
    set_all_parametres (
        set_reference_from_layer,
        include_blocks,
        regex_grammar, layers_to_read) ;
}

void dpps::Reader_DXF_R12::set_all_parametres (
    const bool set_reference_from_layer,
    const bool include_blocks,
    const enum_regex regex_grammar,
    const std::string &layers_to_read) {

    reader_settings. regex_grammar = regex_grammar ;

    std::regex_constants::syntax_option_type flags ;
    switch (reader_settings. regex_grammar) {
        case all:
            reader_settings. layers_to_read. clear () ;
            break ;
        case verbatim:
            // clear first, in case user called more than once
            // set_all_parametres
            reader_settings. layers_to_read. clear () ;
            reader_settings. layers_to_read. push_back (layers_to_read) ;
            break ;
        case comma_separated_verbatim_list:
            // split_string appends to the vector, so we need to clear first
            reader_settings. layers_to_read. clear () ;
            reader_settings. layers_to_read =
                split_string (layers_to_read, ',', true) ;
            break ;
        case regex_ECMAScript:
            flags = std::regex_constants::ECMAScript ;
            break ;
        case regex_basic:
            flags = std::regex_constants::basic ;
            break ;
        case regex_extended:
            flags = std::regex_constants::extended ;
            break ;
        case regex_awk:
            flags = std::regex_constants::awk ;
            break ;
        case regex_grep:
            flags = std::regex_constants::grep ;
            break ;
        case regex_egrep:
            flags = std::regex_constants::egrep ;
            break;
    }
    if ((reader_settings. regex_grammar != all) &&
        (reader_settings. regex_grammar != verbatim) &&
        (reader_settings. regex_grammar != comma_separated_verbatim_list)) {
        reader_settings. layers_to_read. push_back (layers_to_read) ;
        expression = std::regex (layers_to_read, flags) ;
    }
    reader_settings. set_reference_from_layer = set_reference_from_layer ;
    reader_settings. include_blocks = include_blocks ;

}

void dpps::Reader_DXF_R12::set_parametres (
                     const std::vector<bool> &vbool,
                     const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 2)   || // 1
        (vint. size () != 1)    ||
        (vdouble. size () != 0)  ||
        (vstring. size () != 1)) {
        throw bad_parametre ("Reader_DXF_R12::set_parametres",
            2, 1, 0, 1,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint. front () >= 7) {
        std::string reason {"Reader_DXF_R12::set_parametres, \
regex_grammar should be either of 0 to 6 for the five regex grammars \
supported by C++ (ECMAScript, basic, extended, awk, grep, egrep), got " +
        std::to_string (vint. front ()) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }

    set_all_parametres (vbool. at (0),
                        vbool. at (1),
                        static_cast<enum_regex> (vint. at (1)),
                        vstring. at (0)) ;
}


bool dpps::Reader_DXF_R12::find_next_line (std::istream &scan_file, std::string target) {
    std::string line ;
    bool finished {false} ;
    while (!finished) {
        bool test = getline (scan_file, line). good() ;
        sanitize_EOL (line) ;
        line = remove_spaces_left  (line) ;
        line = remove_spaces_right (line) ;
        //if (!scan_file. good ())
        if (!test)
            break ;
        finished = (line == target) ;
    }
    return finished ;
}

long_unsigned_int dpps::Reader_DXF_R12::find_next_line_vector (std::istream &scan_file,
                                   std::vector <std::string> vector_target,
                                   bool &ok) {
    std::string line ;
    bool finished {false} ;
    // BUG gcc: if we remove {0} then we get a -Wmaybe-uninitialized
    // warning (which is ok by itself), but at the wrong place,
    // actually at the switch (found_position) in read_polyline.
    long_unsigned_int i {0} ;
    //std::vector<std::string>::const_iterator i ;
    while (!finished) {
        //std::cerr << "*" ;
        bool test {getline (scan_file, line). good()} ;
        //std::cerr << "read line : " << line << " ##\n ";
        sanitize_EOL (line) ;
        line = remove_spaces_left  (line) ;
        line = remove_spaces_right (line) ;
        //if (!scan_file. good ())
        if (!test) {
            //std::cerr << "getting out !\n";
            break ;
        }
        i = vector_string_find (vector_target, line) ;
        finished = (i != std::numeric_limits<long_unsigned_int>::max()) ;
        //std::cerr << i << " " << std::numeric_limits<long_unsigned_int>::max() << " " << finished << " ";
        //std::cerr << finished << "\n";
//         for (long_unsigned_int j = 0 ; j < vector_target. size () ; j++) {
//             if (line == vector_target[j]) {
//                 finished = true ;
//                 i = j ;
//             }
//         }
    }
    if (finished) {
        //works as well: std::distance (vector_target. begin (), i)
        ok = true ;
        //return (i - vector_target. begin ()) ;
        return i ;
    }
    ok = false ;
    return 0 ;
}

bool dpps::Reader_DXF_R12::get_field_value (std::istream &scan_file,
                             long int &field,
                             std::string &value) {
    // std::cout << "Getting field value\n" ;
    std::string line ;
    bool test {getline (scan_file, line). good ()} ;
    sanitize_EOL (line) ;
    line = remove_spaces_left  (line) ;
    line = remove_spaces_right (line) ;
    //if (!scan_file. good ())
    if (!test)
        return false ;
    // std::cout << "We will convert " << line << std::endl ;
    field = std::stol (line. c_str (), nullptr, 10) ;
    if (field == 0)
        return true ;
    test = getline (scan_file, line). good () ;
    sanitize_EOL (line) ;
    line = remove_spaces_left  (line) ;
    line = remove_spaces_right (line) ;
    //if (!scan_file. good ())
    if (!test)
        return false ;
    value = line ;
    //std::cout << "field " << field << " value " << value << std::endl ;
    return true ;
}
bool dpps::Reader_DXF_R12::manage_layer (std::string &string_value) {
    sanitize_EOL (string_value) ;
    //std::cerr << "we have layer " << string_value << "\n" << std::flush; //***
    string_value = remove_spaces_left  (string_value) ;
    string_value = remove_spaces_right (string_value) ;
    switch (reader_settings. regex_grammar) {
        case all:
            return false ;
        case verbatim:
        case comma_separated_verbatim_list:
            if (std::find (reader_settings. layers_to_read. begin (),
                    reader_settings. layers_to_read. end (),
                    string_value. c_str()) ==
                reader_settings. layers_to_read. end ()) {
                // Layer not in our list, ignore this Polyline
                // look_for_next_polyline = true ;
                //std::cerr << "true 1\n" << std::flush;
                return true ;
            }
            break ;
        default:
        if (!std::regex_match (string_value, expression))
            // look_for_next_polyline = true ;
            //std::cerr << "true 2\n" << std::flush ;
            return true ;
    }
    //std::cerr << "false \n"  << std::flush;

    // N.B. true means "look for next polyline", false we accept this polyline
    return false ;
}

long_unsigned_int dpps::Reader_DXF_R12::manage_reference (
    std::vector<std::string> &layers_read,
    std::string &polyline_layer) {
    auto pos = std::find (layers_read. cbegin (),
                                 layers_read. cend (), polyline_layer) ;
    //std::cout << "manage reference:\n";
    if (pos == layers_read. cend ()) {
        layers_read. push_back (polyline_layer) ;
        if (reader_settings. set_reference_from_layer) {
            //std::cerr <<  "layer " << polyline_layer << " reference " << layers_read. size () - 1 << "\n";
            return layers_read. size () - 1;
        }
    } else {
        if (reader_settings. set_reference_from_layer) {
            //std::cerr <<  "layer " << polyline_layer << " reference " << layers_read. size () - 1 << "\n";
            return distance (layers_read. cbegin (), pos) ;
        }
    }
    //std::cerr <<  "layer " << polyline_layer << " reference will be zero\n";
    return 0 ;
}

bool dpps::Reader_DXF_R12::read_polyline (Polyline &p) {

    std::string string_value ;
    std::string polyline_layer ;
    bool set_x,
         set_y ;
    long int field ;
    double x {0.0} ;
    double y {0.0} ;
    Vertex v ;
    std::string reason ;
    if (!file. good ())
        return false ;
    if (!reader_settings. include_blocks) {
        // We do not understand blocks. Most of the time we will therefore
        // want to skip them, by setting include_blocks to false.
        // However, certain software produce dxf files where the polylines
        // only appear in a large root block. The main part of the file is
        // empty, just calling a block previously defined with all the
        // polylines. In this case we want to include the blocks.
        if (!skipped_header) {
            // Before ENTITIES can be BLOCK sections, but we do not support them.
            // After ENTITIES start the actual polylines.
            skipped_header = find_next_line (file, "ENTITIES") ;
            if (!skipped_header)
                return false ;
        }
    }
    // strangely, the line below would segfault (at least with gcc)
    //std::vector<std::string> sequence {{"VERTEX", "SEQEND"}} ;
    std::vector<std::string> sequence2 ;
    sequence2. push_back (std::string ("VERTEX")) ;
    sequence2. push_back (std::string ("SEQEND")) ;
    std::vector<std::string> sequence1 ;
    sequence1. push_back (std::string ("POLYLINE")) ;
    sequence1. push_back (std::string ("POINT")) ;
    sequence1. push_back (std::string ("LINE")) ;
    sequence1. push_back (std::string ("CIRCLE")) ;
    while (true) {
        bool set_layer {false} ;
        bool set_closed {false} ;
        bool look_for_next_polyline {false} ;
        bool found ;
        //std::cerr << "." ;
        long_unsigned_int pos {find_next_line_vector (file, sequence1, found)} ;
        if (!found)
            return false ;
        if (pos == 0) { // POLYLINE
            while ((!set_layer || !set_closed) && !look_for_next_polyline) {
                found = get_field_value (file, field, string_value) ;
                // std::cerr << "field " << field << " " << string_value << "\n" ;
                if (!found)
                    return false ;
                switch (field) {
                    case 8: // LAYER
                        look_for_next_polyline = manage_layer (string_value) ;
                        set_layer = true ;
                        polyline_layer = string_value ;
                        break ;
                    case 70: // CLOSED
                        p. closed = (std::stol (string_value. c_str (),
                                            nullptr, 10) == 1) ;
                        set_closed = true ;
                        break ;
                    // other fields are ignored.
                    // field could be 10, 20, 30 for x, y, z but it's not clear what
                    // they are for. Could be 5 for the handle, but we won't use it.
                    case 0:
                        // we normally never reach here because we get out as soon
                        // as set_closed and layer_set are true.
                        if (!set_closed) {
                            // Lines without indication will be closed by default.
                            set_closed = true ;
                            p. closed = true ;
                        }
                    default:
                        break ;
                }
            }
            if (look_for_next_polyline)
                continue ;
            // std::cerr << "we are here\n";
            while (true) {
                bool ok {true} ;
                long_unsigned_int found_position {
                    find_next_line_vector (file, sequence2, ok)} ;
                if (!ok)
                    return false ;
                switch (found_position) {
                    case 1:
                        // We found SEQEND, Polyline is finished.
                        //////////////////////////////////////
                        // RETURN TRUE IS HERE FOR POLYLINE //
                        //////////////////////////////////////
                        p. reference = manage_reference (layers_read,
                                                         polyline_layer) ;
                        //std::cerr << "POLY we use ref " << p.reference << "\n" ;
                        return true ;
                    case 0:
                        // if it's 0 and ok==true, then:
                        // VERTEX: good, let's proceed with reading vertex contents
                        break ;
                    default:
                        // Not reachable, as sequence2 has only two components,
                        // returned value can only be 0 or 1.
                        // But compilers like to have default case treated.
                        return false ;
                }

                set_x = false ;
                set_y = false ;
                v. x = 0 ;
                v. y = 0 ;
                while (!(set_x && set_y)) {
                    found = get_field_value (file, field, string_value) ;
                    //std::cerr << "-- field " << field << " " << string_value << "\n" ;
                    if (!found)
                        return false ;
                    switch (field) {
                        case 10: // x coordinate
                            x = std::stod (string_value. c_str (), nullptr) ;
                            set_x = true ;
                            break ;
                        case 20: // y coordinate
                            y = std::stod (string_value. c_str (), nullptr) ;
                            set_y = true ;
                            break ;
                        // ignored cases:
                        //case 0:  // We finished this vertex
                        //case 5:  // HANDLE in hexadecimal. We do nothing with it.
                        //case 8:  // LAYER. We have it from the Polyline, the
                        //         // vertices should belong to the same layer.
                        //case 30: // Z component, always 0 in DXF R12 and useless
                        //         // to our 2D Polyline.
                        default:
                            break ;
                    }
                }
                // At this point we have a vertex at (x, y)
                v. x = x ;
                v. y = y ;
                p. push_back (v) ;
                set_x = false ;
                set_y = false ;
            }
        }
        if (pos == 1) { // POINT
            v. x = 0 ;
            v. y = 0 ;
            while (!look_for_next_polyline) {
                found = get_field_value (file, field, string_value) ;
                if (!found)
                    return false ;
                switch (field) {
                    case 8: // LAYER
                        look_for_next_polyline = manage_layer (string_value) ;
                        polyline_layer = string_value ;
                        break ;
                    case 10: // x coordinate
                        v. x = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 20: // y coordinate
                        v. y = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 0:
                        // We found SEQEND, Polyline is finished.
                        ///////////////////////////////////
                        // RETURN TRUE IS HERE FOR POINT //
                        ///////////////////////////////////
                        p. reference = manage_reference (layers_read, polyline_layer) ;
                        //std::cerr << "POINT we use ref " << p.reference << "\n" ;
                        p. push_back (v) ;
                        return true ;
                    default:
                        break ;
                }
            }
        }
        if (pos == 2) { // LINE
            v. x = 0 ;
            v. y = 0 ;
            Vertex w (0,0) ;
            while (!look_for_next_polyline) {
                found = get_field_value (file, field, string_value) ;
                if (!found)
                    return false ;
                switch (field) {
                    case 8: // LAYER
                        look_for_next_polyline = manage_layer (string_value) ;
                        polyline_layer = string_value ;
                        break ;
                    case 10: // x coordinate, first point
                        v. x = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 20: // y coordinate, first point
                        v. y = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 11: // x coordinate, second point
                        w. x = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 21: // y coordinate, second point
                        w. y = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 0:
                        // We found SEQEND, Polyline is finished.
                        //////////////////////////////////
                        // RETURN TRUE IS HERE FOR LINE //
                        //////////////////////////////////
                        p. reference = manage_reference (layers_read, polyline_layer) ;
                        //std::cerr << "LINE we use ref " << p.reference << "\n" ;
                        p. push_back (v) ;
                        p. push_back (w) ;
                        return true ;
                    default:
                        break ;
                }
            }
        }
        if (pos == 3) { // CIRCLE
            v. x = 0 ;
            v. y = 0 ;
            while (!look_for_next_polyline) {
                found = get_field_value (file, field, string_value) ;
                if (!found)
                    return false ;
                switch (field) {
                    case 8: // LAYER
                        look_for_next_polyline = manage_layer (string_value) ;
                        polyline_layer = string_value ;
                        break ;
                    case 10: // x coordinate, first point
                        v. x = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 20: // y coordinate, first point
                        v. y = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 40: // dose is used to store the circle radius
                        p. dose = std::stod (string_value. c_str (), nullptr) ;
                        break ;
                    case 0:
                        // We found SEQEND, Polyline is finished.
                        ////////////////////////////////////
                        // RETURN TRUE IS HERE FOR CIRCLE //
                        ////////////////////////////////////
                        p. reference = manage_reference (layers_read, polyline_layer) ;
                        //std::cerr << "CIRCLE we use ref " << p.reference << "\n" ;
                        p. push_back (v) ;
                        return true ;
                    default:
                        break ;
                }
            }
        }
    }
    // unreachable
    return false ;
}

std::vector<std::string> dpps::Reader_DXF_R12::get_layers_read () const {
//     std::cout << "here the layers : ";
//     for (auto &s: layers_read)
//         std::cout << s << " ";
    return layers_read ;
}
