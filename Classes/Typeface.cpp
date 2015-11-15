/**
 * @file Typeface.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief An encapsulation of fonts. Well, it's not the class I'm most proud of.
 * @date 2014-02-17 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <limits>

#include <iostream> // debug only
#include "Typeface.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

dpps::Typeface::Typeface (
        const std::string &font_value,
        const short unsigned int variant_value,
        const stroke_type stroke_value):
        font_name(font_value), variant(variant_value), stroke (stroke_value) {
    load() ;
}

dpps::Typeface::Typeface () {
    loaded = false ;
}

void dpps::Typeface::set_font (const std::string &font_value) {
    if (font_name != font_value) {
        font_name = font_value ;
        loaded = false ;
    }
}

void dpps::Typeface::set_variant (const short unsigned int variant_value) {
    if (variant != variant_value) {
        variant = variant_value ;
        loaded = false ;
    }
}

void dpps::Typeface::set_stroke_type (const stroke_type stroke_value) {
    if (stroke != stroke_value) {
        stroke = stroke_value ;
        loaded = false ;
    }
}

std::string dpps::Typeface::get_font () const {
    return font_name ;
}

short unsigned int dpps::Typeface::get_variant () const {
    return variant ;
}

dpps::stroke_type dpps::Typeface::get_stroke_type () const {
    return stroke ;
}

dpps::character dpps::Typeface::get_character (const unsigned char c) const {
    short unsigned int d {static_cast<short unsigned int> (c)} ;
    //std::cout << "get_character ; c= : " << c << ", d=" << d << "\n" ;
    if (d >= maxchars) {
        std::string reason {"Typeface::get_character, a provided character was \
out of range allowed by the font: range is 0-" + std::to_string ((long_unsigned_int)(maxchars-1)) +
         "provided value was " + std::to_string ((long_unsigned_int)d) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    return font [static_cast<short unsigned int> (c)] ;
}

bool dpps::Typeface::get_next_line_vector (std::istream &scan_file,
                              std::vector <short signed int> &vector_target) {
    std::string line ;
    std::vector<std::string> string_result ;
    bool finished {false} ;
    while (!finished) {
        if (!getline (scan_file, line). good())
            return false ;
        sanitize_EOL (line) ;
        if (line. find ("#") != std::string::npos) {
            string_result = split_string (line, '#', false) ;
            line = string_result[0] ;
        }
        line = remove_spaces_left  (line) ;
        line = remove_spaces_right (line) ;
        if (line. size () != 0) {
            vector_target. clear () ;
            string_result = split_string (line, ',', true) ;
            for (auto &i : string_result)
                vector_target. push_back (std::stod(i. c_str(), nullptr)) ;
            return true ;
        }
    }
    return finished ;
}

dpps::Pattern dpps::Typeface::create_character (std::vector<short signed int> &vararg, Pattern &original, long_unsigned_int &current) {
    // ported from lua where vararg is ...
    // and loop is written: for j, v in ipairs{...} do
    // main algorithm difference is in lua j starts with 1, in C++ j starts
    // with 0 so parity test is swapped.
    Pattern pattern_result ;
    Polyline c ;
    long_unsigned_int k {0} ;
    long_unsigned_int num {0} ;
    // std::cout << "create character" << "\n" ;
    for (long_unsigned_int j {0} ; j < vararg. size () ; j++) {
        short signed int v {vararg[j]} ;
        // std::cout << "j = " << j << ", v = " << v << "\n" ;
        if (j % 2 == 0)
            num = v ;
        else {
            if (v > 0)
                pattern_result. push_back(original. get_polyline (current + num)) ;
            else {
                c = pattern_result. pop_back () ;
                Polyline cut {original. get_polyline (current + num)} ;
                c. cut_into (cut, true) ;
                pattern_result. push_back (c) ;
            }
            k++ ;
        }
    }
    pattern_result. translate (-pattern_result. minimum_x(), 0) ;
    current += k ;
    return pattern_result ;
}

void dpps::Typeface::load () {
    if (loaded)
        return ;
    loaded = true ;
#ifdef FILENAMES_FOR_WINDOWS
    std::string font_name_complete {".\\font_data\\" + font_name + ".dxf"} ;
#else
    std::string font_name_complete {"./font_data/" + font_name + ".dxf"} ;
#endif
    Reader_DXF_R12 reader (font_name_complete, false, false, verbatim, "L1") ;
    Pattern raw_alphabet ;
    reader. append_to_pattern (raw_alphabet) ;
    //raw_alphabet. display() ;
    reader. close () ;

    std::ifstream composition ;
#ifdef FILENAMES_FOR_WINDOWS
    std::string composition_name_complete {".\\font_data\\" + font_name + "-composition.txt"} ;
#else
    std::string composition_name_complete {"./font_data/" + font_name + "-composition.txt"} ;
#endif
    composition. open (composition_name_complete, std::ios_base::in) ;
    if (!composition. is_open ()) {
        std::string reason {"File " + composition_name_complete +
                            " could not be open for input"} ;
        throw bad_io (reason. c_str ()) ;
    }

    long_unsigned_int current {0} ;
    long_unsigned_int k {0} ;
    for (short unsigned int j = 0 ; j < variant + 1; j++) {
        for (short unsigned int i = 0 ; i < characters_in_a_typeface ; i++) {
            // i describes the linear count of characters.
            // We will then place the character at k=i+32, see below.
            std::vector<short signed int> vararg ;
            if (!get_next_line_vector (composition, vararg)) {
                std::string reason {"Composition file " +
                    composition_name_complete +
                    " does not have enough lines for this variant."} ;
                throw bad_io (reason. c_str ()) ;
            }
            //for (auto &l : vararg)
            //    std::cout << l << ", " ;
            //std::cout << "\n" ;

            // i is the character as read from the dxf font.
            // k is where we are going to place the character in the table,
            //   which follows the iso-8859-1 codepoints.
            if (i < 94)
                k = i + 33 ; // i == 0 is the first char, which is !, goes to 33
                // (we ignore codepoints k=0 to 31 and 32 which is the space)
            else {
                if (i < 106)
                    k = i + 67 ; // i == 94 is ¡, which goes to 161
                    // (we ignore codepints k=127 to 159, non printable, and 160 which is nbsp)
                else
                    k = i + 68 ; // i == 106 is (R), which goes to 174
                    // (we ignore codepoint k == 173, which is conditional hyphen, non printable)
            }
            Pattern c {create_character (vararg, raw_alphabet, current)} ;
            //c. display(-1) ;
            font[k] = c. polylines ;
            // We reset the x shift to zero.
            // We do not do approach or kerning, for sake of simplicity.
            Vertex bl {c. lower_left()} ;
            c. translate (-bl.x, 0.0) ;
            Vertex tr {c. upper_right()} ;
            top[k] = tr. y ;
            bottom[k] = bl. y ;
            //left[k] = 0.0 ;
            right[k] = tr. x ;
        }
    }
    composition. close () ;
    ex = top    [static_cast<short unsigned int> ('x')] -
         bottom [static_cast<short unsigned int> ('x')] ;
    em = right  [static_cast<short unsigned int> ('m')] ;
         //left   [static_cast<short unsigned int> ('m')] ;
    en = right  [static_cast<short unsigned int> ('n')] ;
         //left   [static_cast<short unsigned int> ('n')] ;
    //std::cout << " tailles ex " << ex << " em " << em << " en " << en << std::endl ;
    //throw ("We succeeded the load") ;
}
