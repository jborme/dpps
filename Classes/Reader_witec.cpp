/**
 * @file Reader_witec.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader derivative that reads the witec files as exported by this
 * program (parser is rather simple so file has to be well-behaved)
 * @date 2014-07-19 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>

#include "util.hh"
#include "Reader_witec.hh"
#include "bad_io.hh"
#include "bad_parametre.hh"

dpps::Reader_witec::Reader_witec (const std::string &set_filename) {
    filename = set_filename ;
    file. open (filename, std::ios_base::in) ;
    if (!file. is_open ()) {
        std::string reason {"File " + filename +
                            " could not be open for input"} ;
        throw bad_io (reason. c_str ()) ;
    }
    current_stepper_pos = Vertex (0,0) ;
    current_piezo_pos = Vertex (0,0) ;
}

dpps::Reader_witec::Reader_witec (const std::string &set_filename,
    const bool construction_lines): Reader_witec (set_filename) {
        reader_settings. construction_lines = construction_lines ;
}

void dpps::Reader_witec::set_all_parametres (
    const bool construction_lines) {
    reader_settings. construction_lines = construction_lines ;
}

void dpps::Reader_witec::set_parametres (
                     const std::vector<bool> &vbool,
                     const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 1)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 0)  ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Reader_witec::set_parametres",
            1, 0, 0, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    set_all_parametres (vbool. at (0)) ;
}

bool dpps::Reader_witec::read_polyline (Polyline &p) {
    std::string line ;
    bool inside_comment {false} ;
    bool look_for_parametre {false} ;
    bool look_for_semicolon {false} ;
    bool get_next_line {true} ;
    bool call_action {false} ;
    bool look_for_on_off {false} ;
    int read_parametres{0} ;
    std::string function ;
    long_unsigned_int i {0} ;
    int j{0} ;
    long_unsigned_int k{0} ;
    std::array<double, 3> parametres ;
    while (true) {
//         std::cout << "beginning: next line=" << get_next_line
//                   << " call_action=" << call_action
//                   << " semicolon=" << look_for_semicolon
//                   << " on_off=" << look_for_on_off
//                   << " inside_comment=" <<inside_comment
//                   << " param=" << look_for_parametre
//                   << ": **" << line
//                   << "\n";
        if (get_next_line) {
            //std::cout << "Getting next line\n" ;
            if (!getline (file, line). good ())
                return false ;
            sanitize_EOL (line) ;
            k++ ;
        } else {
            //std::cout << "Keeping same line\n" ;
            get_next_line = true ;
        }
        if (!file. good ())
            break ;
        if (inside_comment) {
            std::string::size_type pos ;
            if ((pos = line. find ("*/")) != std::string::npos) {
                line. erase (line.begin(), line.begin()+pos+2) ;
                while (remove_from_to (line, "/*", "*/")) ;
                pos = line. find ("/*") ;
                if (pos != std::string::npos)
                    line. erase (line.begin()+pos, line.end()) ;
                    // We will still be in comment next line,
                    // but we have some non-comment text so we can proceed
                else
                    inside_comment = false ;
            } else
                // The comment did not end, so we continue to next line
                continue ;
        } else {
            while (remove_from_to (line, "/*", "*/")) ;
            std::string::size_type pos {line. find ("/*")} ;
            if (pos != std::string::npos) {
                line. erase (line.begin()+pos, line.end()) ;
                inside_comment = true ;
            }
        }
        line = dpps::remove_spaces_left  (line) ;
        line = dpps::remove_spaces_right (line) ;
//         std::cout << "now line: **" << line << "\n" ;
        if (line. empty ())
            continue ;
        // At this point all comments are eliminated
//         std::cout << ":" + line + "\n" ;
        if (look_for_semicolon) {
//             std::cout << "looking for semicolon... " ;
            std::string::size_type pos = line. find (";") ;
            if (pos == std::string::npos) {
                look_for_semicolon = true ;
//                 std::cout << "none\n" ;
            }
            else {
                line. erase (line.begin (), line.begin()+pos+1) ;
//                 std::cout << "erased. New line is **" << line << "**\n" ;
                look_for_semicolon = false ;
            }
            // and then we proceed to analyse
        }
        if (line. empty () && !call_action)
            continue ;

        if (look_for_parametre) {
//              std::cout << "Looking for parametres, " << read_parametres << " to be read out of " << j << "\n" ;
            // j was set at previous iteration.
            std::string::size_type pos ;
            if (read_parametres < j-1) {
//                 std::cout << "Looking for comma" << "\n" ;
                pos = line. find (",") ;
            }
            if ((read_parametres >= j-1) || j == 0) {
//                 std::cout << "Looking for closing bracket" << "\n" ;
                pos = line. find (")") ;
            }
            std::string number = line.substr (0, pos) ;
            parametres[read_parametres] = std::stod (number. c_str(), nullptr) ;
//              std::cout << "parametre string " << number << " and value " << parametres[read_parametres] << "\n" ;
            line.erase (line.begin(), line.begin()+pos+1) ;
            read_parametres++ ;
            if (read_parametres == j) {
//                  std::cout << "read_parametres == j !" << "\n" ;
                look_for_parametre = false ;
                look_for_semicolon = true ;
                get_next_line = false ;
                call_action = true ;
                continue ;
            } else {
//                  std::cout << "We continue !" << "\n" ;
                look_for_semicolon = false ;
                get_next_line = false ;
                continue ;
            }
        }

        if (look_for_on_off) {
//              std::cout << "Looking for closing bracket" << "\n" ;
            std::string::size_type pos {line. find (")")} ;
            std::string on_off = line.substr (0, pos) ;
            line.erase (line.begin(), line.begin()+pos+1) ;
            std::string::size_type pos1 {on_off. find ("on")} ;
            std::string::size_type pos2 {on_off. find ("off")} ;
            if ((pos1 != std::string::npos) && (pos2 != std::string::npos)) {
                std::string reason {filename + ":" + std::to_string (k) +
                    " SetTrigger found both on and off ?"} ;
                throw bad_io (reason. c_str ()) ;
            }
            if ((pos1 == std::string::npos) && (pos2 == std::string::npos)) {
                std::string reason {filename + ":" + std::to_string (k) +
                    " SetTrigger was not set on nor off."} ;
                throw bad_io (reason. c_str ()) ;
            }
            if ((pos1 != std::string::npos) && (pos2 == std::string::npos))
                parametres[0] = 1 ;
            if ((pos1 == std::string::npos) && (pos2 != std::string::npos))
                parametres[0] = 0 ;
            call_action = true ;
            look_for_on_off = false ;
            look_for_semicolon = true ;
            get_next_line = false ;
            read_parametres = 1 ;
            continue ;
        }
        if (call_action) {
//             std::cout << function << " (" << i << ") (" ;
//             for (int k{0} ; k < read_parametres ; k++)
//                 std::cout << parametres[k] << " " ;
//             std::cout << ")\n" ;
            Vertex param (parametres[0], parametres[1]) ;
            switch (i) {
                case 6: case 7: // setTrigger
                    if (parametres[0] == 0) { // we were exposing
                        current_polyline. reference = 0 ;
                        p = current_polyline ;
                        current_polyline = Polyline () ;
//                         std::cout << "We return Polyline " << p. display_string () << "\n" ;
                        return true ;
                    } else {
                        if (reader_settings. construction_lines) {
                            current_polyline. reference = 1 ;
                            p = current_polyline ;
                            current_polyline = Polyline () ;
//                             std::cout << "We return construction line " << p. display_string () << "\n" ;
                            return true ;
                        } else {
                            current_polyline = Polyline () ;
                            continue ;
                        }
                    }
                    break ;
                case 22: case 23: // setOrigin
                    current_piezo_pos = Vertex (0,0) ;
                    break ;
                case 34: case 35: // SetOriginSamplePos
                    current_stepper_pos = Vertex (0,0) ;
                    break ;
                case 24: case 25: // MoveRelative
                case 28: case 29: // JumpRelative
                    if (current_polyline. size () == 0)
                        current_polyline. push_back (current_stepper_pos+current_piezo_pos) ;
                    current_polyline. push_back_relative (param) ;
                    current_piezo_pos += param ;
                    break ;
                case 26: case 27: // MoveAbsolute
                case 30: case 31: // JumpAbsolute
                {
                    if (current_polyline. size () == 0)
                        current_polyline. push_back (current_stepper_pos+current_piezo_pos) ;
                    current_polyline. push_back_relative (param - current_piezo_pos) ;
                    current_piezo_pos = param ;
                    break ;
                }
                case 36: case 37: // MoveRelativeSamplePos
                case 40: case 41: // JumpRelativeSamplePos
                    if (current_polyline. size () == 0)
                        current_polyline. push_back (current_stepper_pos+current_piezo_pos) ;
                    current_polyline. push_back_relative (param) ;
                    current_stepper_pos += param ;
//                     std::cout << "At this point, current_polyline= " << current_polyline. display_string () << "\n" ;
                    break ;
                case 38: case 39: // MoveAbsoluteSamplePos
                case 42: case 43: // JumpAbsoluteSamplePos
                {
//                     std::cout << "stepper is at " << current_stepper_pos. display_string() << " and we must move absolute to "
//                     << param.display_string() << " so we calculate the relative movement : " << (param - current_stepper_pos). display_string()
//                     << "\n" ;
//
                    if (current_polyline. size () == 0)
                        current_polyline. push_back (current_stepper_pos+current_piezo_pos) ;
                    current_polyline. push_back_relative (param - current_stepper_pos) ;
//                     std::cout << "polyline status is " << current_polyline. display_string() << "\n" ;
                    current_stepper_pos = param ;
                    break ;
                }
                case 4: case 5: // MoveZMicroscope
                default:
                    // Function is known but not supported.
                    // We Will just look for final ; and interpret after that
                    // if ; is forgotten then we will skip until there is one.
                    break ;
            }
            j = 0 ;
            read_parametres = 0 ;
            call_action = false ;
            look_for_semicolon = false ;
            get_next_line = false ;
            continue ;
        }
        // below we look for the name of a function
        // at the beginning of the line.
        //std::cout << "Passing by function analysis\n" ;
        std::string::size_type pos {line. find ("(")} ;
        if (pos == std::string::npos) {
            std::string reason {filename + ":" + std::to_string (k) +
                " function expected but no opening bracket found."} ;
            throw bad_io (reason. c_str ()) ;
        }
        function = line.substr(0, pos) ;
        function = dpps::remove_spaces_right (function) ;
        line. erase (line.begin (), line.begin()+pos+1) ;

        i = vector_string_find (allowed_functions, function) ;
        j = parametre_number [i] ;
//          std::cout << "Identified function " << function << ", number " << i << ", parametres " << j << "\n" ;

        if (j == 0) {
//             std::cout << "No args, calling for action\n" ;
            call_action = true ;
            get_next_line = false ;
        } else {
//             std::cout << "Will be looking for params\n" ;
            look_for_parametre = true ;
            get_next_line = false ;
        }
        if (i == 6) {
            look_for_on_off = true ;
            look_for_parametre = false ;
            get_next_line = false ;
//             std::cout << "Will look for on/off\n" ;
            continue ;
        }
        if ((i != 4) && (i != 5) && (i != 6) && (i != 7) && (i != 22) && (i != 23) &&
            (i != 24) && (i != 25) && (i != 26) && (i != 27) && (i != 28) && (i != 29) &&
            (i != 30) && (i != 31) && (i != 34) && (i != 35) && (i != 36) && (i != 37) &&
            (i != 38) && (i != 39) && (i != 40) && (i != 41) && (i != 42) && (i != 43)) {
            // these are the unsupported functions. We just skip them
            //std::cout << "Function ignored, skipping to semicolon\n" ;
            look_for_parametre = false ;
            look_for_semicolon = true ;
            get_next_line = false ;
            continue ;
        }
        //std::cout << "getting next line ? "<< get_next_line << "\n";
        //std::cout << "Function " << function << ", arguments " << line << "\n" ;
        //pos = allowed_functions. find ()
        //vector_string_find ()
    }

    return false ;
}