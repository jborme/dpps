/**
 * @file Typeface.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief An encapsulation of fonts. Well, it's not the class I'm most proud of.
 * @date 2014-02-19 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_TYPEFACE
#define DPPS_TYPEFACE

#include <Reader_DXF_R12.hh>
#include <array>

// #include "fonts_declarations.hh"
#include "Polyline.hh"

namespace dpps {
    // types used after load, with an easier access to Polyline.
    const constexpr short unsigned int maxchars {256} ;
    typedef std::vector<dpps::Polyline> character ;
    typedef std::array<character, maxchars> alphabet_iso8859_1_polyline ;

    typedef enum {
        several,
        cut,
        continuous
    } stroke_type ;

/** @brief A class to encapsulate a typeface */
class Typeface {
    friend class Pattern_text ;
protected:
    std::string font_name ;
    short unsigned int variant {0} ;
    bool loaded {false} ;
    stroke_type stroke {cut} ;

    bool get_next_line_vector (std::istream &scan_file,
                         std::vector <short signed int> &vector_target) ;
    Pattern create_character (std::vector<short signed int> &vararg, Pattern &original, long_unsigned_int &current) ;

    // The protected things below are accessed from Pattern_text
    alphabet_iso8859_1_polyline font ;
    std::array<double, maxchars> top {{}} ;   // make really sure they are initialized at zero
    std::array<double, maxchars> bottom {{}} ;
    std::array<double, maxchars> right {{}} ;
public:
    static const constexpr short unsigned int characters_in_a_typeface {188} ;

    Typeface () ;
    Typeface (const std::string &font_value, const short unsigned int variant_value, const stroke_type stroke_value = cut) ;

    void set_font (const std::string &font_value) ;
    void set_variant (const short unsigned int variant_value) ;
    void set_stroke_type (const stroke_type stroke_value) ;

    std::string get_font () const ;
    short unsigned int get_variant () const ;
    stroke_type get_stroke_type () const ;

    void load () ;

    double ex {1.0} ;
    double em {1.0} ;
    double en {1.0} ;
    character get_character (const unsigned char c) const ;
} ;
} // namespace dpps
#endif
