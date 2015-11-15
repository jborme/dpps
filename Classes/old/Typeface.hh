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

#include <array>

#include "fonts_declarations.hh"
#include "Polyline.hh"

namespace dpps {
/** @brief A class to encapsulate a typeface */
class Typeface {
public:
    alphabet_iso8859_1_raw      raw_font ;
    alphabet_iso8859_1_polyline font ;

    std::array<double, maxchars> top ;
    std::array<double, maxchars> bottom ;
    std::array<double, maxchars> left ;
    std::array<double, maxchars> right ;
    // std::array<double, 256> baseline ;

    Typeface () ;
    Typeface (alphabet_iso8859_1_raw &set_raw_font) ;

    void load_raw_alphabet (alphabet_iso8859_1_raw &set_raw_font) ;
    double ex {1.0} ;
    double em {1.0} ;
    double en {1.0} ;
    character get_character (const unsigned char c) const ;
} ;
} // namespace dpps
#endif
