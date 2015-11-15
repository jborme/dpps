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

dpps::Typeface::Typeface (
        alphabet_iso8859_1_raw &set_raw_font) {
    load_raw_alphabet (set_raw_font) ;
}

dpps::Typeface::Typeface () {
}


dpps::character dpps::Typeface::get_character (const unsigned char c) const {
    short unsigned int d {static_cast<short unsigned int> (c)} ;
    std::cout << "get_character ; maxchars: " << maxchars << " " << "\n" ;
    if (d >= maxchars) {
        std::string reason {"Typeface::get_character, a provided character was \
out of range allowed by the font: range is 0-" + std::to_string ((long_unsigned_int)(maxchars-1)) +
         "provided value was " + std::to_string ((long_unsigned_int)d) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    return font [static_cast<short unsigned int> (c)] ;
}

void dpps::Typeface::load_raw_alphabet (alphabet_iso8859_1_raw &set_raw_font) {
    raw_font = set_raw_font ;
    double mb {0.0},
           mt {1.0},
           ml {0.0},
           mr {1.0},
           cb,
           ct,
           cl,
           cr ;
    for (short unsigned int i = 0 ; i < maxchars ; i++) {
        std::cout << "caractère " << i << " parmi " << maxchars << std::endl ;
        raw_character raw_letter {raw_font. at(i)} ;
        std::cout << "taille de raw_letter: " << raw_letter. size () << std::endl ;
        character letter ;
        if (raw_letter. size () > 0) {
            mt = std::numeric_limits<double>::lowest () ;
            mr = std::numeric_limits<double>::lowest () ;
            mb = std::numeric_limits<double>::max () ;
            ml = std::numeric_limits<double>::max () ;
            for (long_unsigned_int j = 0 ; j < raw_letter. size () ; j++) {
                Polyline t {raw_letter[j]} ;
                t. closed = true ;
                Vertex ll {t. lower_left ()},
                       ur {t. upper_right ()} ;
                cl = ll. x ;
                cb = ll. y ;
                cr = ur. x ;
                ct = ur. y ;
                // t. limits (cl, cb, cr, ct) ;
                if (ct > mt)
                     mt = ct ;
                if (cb < mb)
                    mb = cb ;
                if (cr > mr)
                    mr = cr ;
                if (cl < ml)
                    ml = cl ;
                // Inkscape where we took the coordinates from
                // uses an inverted vertical axis.
//                 for (auto &v: t. vertices)
//                     v. y = -v. y ;
                letter. push_back (t) ;
                t. display() ;
            }
            top    [i] = mt ;
            bottom [i] = mb ;
            left   [i] = ml ;
            right  [i] = mr ;
        } else {
            top    [i] = 1.0 ;
            bottom [i] = 0.0 ;
            left   [i] = 0.0 ;
            right  [i] = 1.0 ;
        }
        // The raw letter might contain a x shift, which we reset here.
        // This comes from how we copy-pasted the coordinates. This disallows
        // to define a different x origin for the character (for purpose of
        // kerning) but this is simpler and for now it is enough (we do not
        // intend to implement a full-featured typography system).
        // Note that after this step, left[i] is almost useless as always 0.
        if (letter. size () > 0) {
            for (auto &p : letter)
                p. translate (Vertex (-left[i], 0.0)) ;
            right[i] -= left[i] ;
            left [i] = 0.0 ;
            font [i] = letter ;
        }// char(i) <<
        //std::cout << "caractère " << i << " " << " taille " << letter. size () << std::endl ;
    }
    ex = top    [static_cast<short unsigned int> ('x')] -
         bottom [static_cast<short unsigned int> ('x')] ;
    em = right  [static_cast<short unsigned int> ('m')] -
         left   [static_cast<short unsigned int> ('m')] ;
    en = right  [static_cast<short unsigned int> ('n')] -
         left   [static_cast<short unsigned int> ('n')] ;
    //std::cout << " tailles ex " << ex << " em " << em << " en " << en << std::endl ;
}
