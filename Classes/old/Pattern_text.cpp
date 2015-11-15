/**
 * @file Pattern_text.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that hat writes a text in a Polyline approximation
 * of fonts.
 * @date 2014-02-17 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ${date} uses C localtime format, which is, as far as I understand, the format
 * defined in RFC 733/822/2822/5322 series.
 */

#include <limits>
#include <chrono>

#include <iostream> // debug only

#include "util.hh"
#include "Pattern_text.hh"
//#include "font_Vollkorn.hh"
//#include "font_Source_Sans_Pro.hh"
//#include "font_Source_Code_Pro.hh"
#include "bad_parametre.hh"

dpps::Pattern_text::Pattern_text (): Pattern () {
}

dpps::Pattern_text::Pattern_text (const Pattern_text &source):
    Pattern (source),
    pattern_settings (source. pattern_settings),
    font (source. font) {
}

dpps::Pattern_text::Pattern_text (Pattern_text &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings),
    font (std::move (source.font)) {
}

dpps::Pattern_text &dpps::Pattern_text::operator= (
    dpps::Pattern_text &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
        font = std::move (source.font) ;
    }
    return *this ;
}

dpps::Pattern_text &dpps::Pattern_text::operator= (
    const dpps::Pattern_text &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    font = source.font ;
    return *this ;
}

dpps::Pattern_text::Pattern_text (
    const bool utf8,
    const bool continuous_stroke,
    const text_font font,
    const short unsigned int variant,
    const text_align_x align_x,
    const text_align_y align_y,
    const double x0,
    const double y0,
    const double ex,
    const std::string &text) {
    set_all_parametres (utf8, continuous_stroke, font, variant, align_x, align_y, x0, y0, ex, text) ;
    generate () ;
}

void dpps::Pattern_text::set_all_parametres (
    const bool utf8,
    const bool continuous_stroke,
    const text_font font,
    const short unsigned int variant,
    const text_align_x align_x,
    const text_align_y align_y,
    const double x0,
    const double y0,
    const double ex,
    const std::string &text) {
    pattern_settings. continuous_stroke = continuous_stroke ;
    pattern_settings. x0 = x0 ;
    pattern_settings. y0 = y0 ;
    pattern_settings. ex = ex ;
    pattern_settings. text = text ;
    pattern_settings. font =  font ;
    pattern_settings. variant = variant ;
    pattern_settings. align_x = align_x ;
    pattern_settings. align_y = align_y ;
    pattern_settings. utf8 = utf8 ;
    if (pattern_settings. ex <= 0) {
        std::string reason {"The height size of ex cannot be zero or negative, \
value " + std::to_string (pattern_settings. ex) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    switch (pattern_settings. font) {
        case Vollkorn:
            if (variant > 3) {
                std::string reason {"Font Vollkorn has four variants: normal \
(0), italic (1), bold (2) and bold italic (3), but value " +
                std::to_string (pattern_settings. variant) + " was provided"} ;
                throw bad_parametre (reason. c_str ()) ;}
        break ;
        case Source_Sans_Pro:
            if (variant > 11) {
                std::string reason {"Font Source Sans Pro has twelve variants: \
Ultra light (0), Light (1), Normal (2), Semi-Bold (3), Bold (4), Heavy (5), \
Ultra light Italic (6), Light Italic (7), Italic (8), Semi-Bold Italic (9), \
Bold Italic (10), Heavy Italic (11), but value " +
                std::to_string (pattern_settings. variant) + " was provided"} ;
                throw bad_parametre (reason. c_str ()) ;}
            break ;
        case Source_Code_Pro:
            if (variant > 7) {
                std::string reason {"Font Source Code Pro has eight variants: \
Ultra light (0), Light (1), Medium (2), Bold (3), Heavy (4), Italic (5), \
Bold Italic (6), but value " +
                std::to_string (pattern_settings. variant) + " was provided"} ;
            break ;
            }
        //default: not reachable
    }
}

void dpps::Pattern_text::set_parametres (
        const std::vector<bool> &vbool,
        const std::vector<long_unsigned_int> &vint,
        const std::vector<double> &vdouble,
        const std::vector<std::string> &vstring) {
    if ((vbool. size () != 2)   ||
        (vint. size () != 4)    ||
        (vdouble. size () != 3) ||
        (vstring. size () != 1)) {
        throw bad_parametre ("Pattern_text::set_parametres",
            2, 4, 3, 1,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint[0] > 2) {
        std::string reason {"Currently three fonts are implemented: Vollkorn \
(0), Source Sans Pro (1), Source Code Pro (2), but " +
        std::to_string (vint[0]) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    // vint[1] will be checked in set_all_parametres()
    if (vint[2] > 3) {
        std::string reason {"Pattern_text::set_parametres, paramatre align_x \
is an enum valued 0-3, got " + std::to_string (vint[2])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if (vint[3] > 3) {
        std::string reason {"Pattern_text::set_parametres, paramatre align_y \
is an enum valued 0-3, got " + std::to_string (vint[3])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (vbool[0], vbool[1],
                        static_cast<text_font> (vint[0]),
                        vint[1],
                        static_cast<text_align_x> (vint[2]),
                        static_cast<text_align_y> (vint[3]),
                        vdouble[0], vdouble[1], vdouble[2], vstring[0]) ;
}

void dpps::Pattern_text::generate () {
    if (pattern_settings. text. empty ())
        return ;
    replace_string (pattern_settings. text, "${date}", current_date_string()) ;
//    replace_string (pattern_settings. text, "${space}", " ") ;
//    replace_string (pattern_settings. text, "${comma}", ",") ;
    std::string copy ;
    if (pattern_settings. utf8)
        copy = utf8toiso8859_1 (pattern_settings. text) ;
    else
        copy = pattern_settings. text ;

    switch (pattern_settings. font) {
        case Vollkorn:
            switch (pattern_settings. variant) {
                case 0:
                    if (pattern_settings. continuous_stroke) {
                        std::cout << "Loading vollkorn normal 0\n" ;
                        font. load_raw_alphabet (Vollkorn_Normal_0) ;
                    }
                    else
                        font. load_raw_alphabet (Vollkorn_Normal_1) ;
                    break ;
                case 1:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Vollkorn_Bold_0) ;
                    else
                        font. load_raw_alphabet (Vollkorn_Bold_1) ;
                    break ;
                case 2:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Vollkorn_Italic_0) ;
                    else
                        font. load_raw_alphabet (Vollkorn_Italic_1) ;
                    break ;
                case 3:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Vollkorn_Bold_Italic_0) ;
                    else
                        font. load_raw_alphabet (Vollkorn_Bold_Italic_0) ;
                    break ;
                default: // already checked in set_all_parametres()
                    break ;
            }
            break ;
        case Source_Sans_Pro:
            switch (pattern_settings. variant) {
                case 0:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Ultra_Light_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Ultra_Light_1) ;
                    break ;
                case 1:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Light_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Light_1) ;
                    break ;
                case 2:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Normal_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Normal_1) ;
                    break ;
                case 3:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Semi_Bold_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Semi_Bold_1) ;
                    break ;
                case 4:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Bold_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Bold_1) ;
                    break ;
                case 5:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Heavy_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Heavy_1) ;
                    break ;
                case 6:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Ultra_Light_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Ultra_Light_Italic_1) ;
                    break ;
                case 7:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Light_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Light_Italic_1) ;
                    break ;
                case 8:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Italic_1) ;
                    break ;
                case 9:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Semi_Bold_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Semi_Bold_Italic_1) ;
                    break ;
                case 10:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Bold_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Bold_Italic_1) ;
                    break ;
                case 11:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Sans_Pro_Heavy_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Sans_Pro_Heavy_Italic_1) ;
                    break ;
                default: // already checked in set_all_parametres()
                    break ;
            }
            break ;
        case Source_Code_Pro:
            switch (pattern_settings. variant) {
                case 0:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Ultra_Light_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Ultra_Light_1) ;
                    break ;
                case 1:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Light_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Light_1) ;
                    break ;
                case 2:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Medium_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Medium_1) ;
                    break ;
                case 3:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Semi_Bold_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Semi_Bold_1) ;
                    break ;
                case 4:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Bold_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Bold_1) ;
                    break ;
                case 5:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Heavy_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Heavy_1) ;
                    break ;
                case 6:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Italic_1) ;
                    break ;
                case 7:
                    if (pattern_settings. continuous_stroke)
                        font. load_raw_alphabet (Source_Code_Pro_Bold_Italic_0) ;
                    else
                        font. load_raw_alphabet (Source_Code_Pro_Bold_Italic_1) ;
                    break ;
                default: // already checked in set_all_parametres()
                    break ;
            }
            break ;
        // default: // unreachable
    }
    double bottom {std::numeric_limits<double>::max()} ;
    double top {std::numeric_limits<double>::lowest()} ;
    double width {0.0} ;
    double caret {0.0} ;
    double ratio {pattern_settings. ex / font. ex} ;
    Pattern left_aligned_text ;
    // We first build a pattern with the text left-aligned.
    std::cout << "copy: " << copy << "\n" ;
    for (long_unsigned_int i = 0 ; i < copy. size () ; i++) {
        // we want c in 0-255
        unsigned char c = copy [i] ;
        // Since c is 0-255, we avoid integer promotion problems when converting
        short unsigned int ic = static_cast<short unsigned int> (c) ;
        character letter {font. get_character (c)} ;
        if (letter. size () == 0)
            continue ; // should not happen but let's prevent bugs.
        if (((c == ' ') || (static_cast<short unsigned int>(c) == 128)) &&
            (letter[0]. size () == 1)) {
            // if we have a space or a non-breaking space and we the font
            // does not provide a drawing (which is the usual case), then
            // just move the caret.
            caret += ratio * font. en ;
            continue ;
            }
        if (letter[0]. size () == 1)
            // Note that one single dot is our definition to emptyness. It's not
            // optimum, it was intended to avoid some complain of the compilers
            // when trying to define an empty vector of an empty vector of an
            // empty pair.
            continue ;
        if (font. top [ic] > top)
            top = font. top [ic] ;
        if (font. bottom [ic] < bottom)
            bottom = font. bottom [ic] ;
        // the reference &p is used to avoid copy
        for (auto p: letter) {
            p = p * ratio ;
            p = p + Vertex (caret, 0.0) ;
            left_aligned_text. push_back (p) ;
        }
        width = caret + ratio * font. right [ic] ;
        caret = width + (ratio * font. em * space_size_em) ;
    }
    // Now we align this text
    double shift_x {0.0},
           shift_y {0.0} ;
    // left_aligned_text is a Pattern, so most operations
    // (minimum_y, maximum_y, translate) translate will only
    // have effect if we select its polylines.
    const selection_t sel {0} ;
    left_aligned_text. select (logical_action_all, sel) ;
    switch (pattern_settings. align_x) {
        case text_align_x_centred: // 0
            shift_x = -width / 2.0 ;
            break ;
        case text_align_x_right: // 2
            shift_x = -width ;
            break ;
        case text_align_x_left: // 1 left-aligned, keep shift_x at 0.0
        //default: // unreachable
            break ;
    }
    switch (pattern_settings. align_y) {
        case text_align_y_centred_height: // 1
            shift_y = - (left_aligned_text. maximum_y (sel) +
                         left_aligned_text. minimum_y (sel)) / 2.0 ;
            break ;
        case text_align_y_centred_bottom: // 2
            shift_y = - left_aligned_text. minimum_y (sel) ;
            break ;
        case text_align_y_centred_top: // 3
            shift_y = - left_aligned_text. maximum_y (sel) ;
            break ;
        case text_align_y_centred_baseline: // 0 baseline, keep shift_y to 0.0
        //default: // unreachable
            break ;
    }
    left_aligned_text. translate (Vertex (pattern_settings. x0 + shift_x,
                                          pattern_settings. y0 + shift_y), sel) ;
    left_aligned_text. select (logical_action_none, sel) ;
    append_from (left_aligned_text) ;
}

short unsigned int dpps::Pattern_text::get_variant (const text_font font, const bool italic, const short unsigned int thickness) {
    short unsigned int result {0} ;
    switch (font) {
        case Vollkorn:
            if (!italic && thickness == 0)
                return 0 ;
            if (italic && thickness == 0)
                return 1 ;
            if (!italic && thickness > 0)
                return 2 ;
            if (italic && thickness > 0)
                return 3 ;
        break ;
        case Source_Sans_Pro:
            if (italic)
                result = 6 ;
            result += (thickness <= 6? thickness:6) ;
            break ;
        case Source_Code_Pro:
            if (italic) {
                result = 6 + (thickness == 0? 0:1) ;
            } else {
                result = (thickness < 6? thickness:6) ;
            }
            break ;
        //default: not reachable
    }
    // not reachable
    return 0 ;
}
