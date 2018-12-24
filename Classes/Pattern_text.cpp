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
    const stroke_type stroke,
    const short unsigned int variant,
    const text_align_x align_x,
    const text_align_y align_y,
    const double x0,
    const double y0,
    const double ex,
    const std::string &font_name,
    const std::string &text) {
    set_all_parametres (utf8, stroke, variant, align_x, align_y, x0, y0, ex, font_name, text) ;
    generate () ;
}

void dpps::Pattern_text::set_all_parametres (
    const bool utf8,
    const stroke_type stroke,
    const short unsigned int variant,
    const text_align_x align_x,
    const text_align_y align_y,
    const double x0,
    const double y0,
    const double ex,
    const std::string &font_name,
    const std::string &text) {
    pattern_settings. stroke = stroke ;
    pattern_settings. x0 = x0 ;
    pattern_settings. y0 = y0 ;
    pattern_settings. ex = ex ;
    set_text (text) ;
    set_font (font_name) ;
    pattern_settings. variant = variant ;
    pattern_settings. align_x = align_x ;
    pattern_settings. align_y = align_y ;
    pattern_settings. utf8 = utf8 ;
    if (pattern_settings. ex <= 0) {
        std::string reason {"The height size of ex cannot be zero or negative, \
value " + std::to_string (pattern_settings. ex) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if ((font_name == "Vollkorn") && (variant > 3)) {
        std::string reason {"Font Vollkorn has four variants: normal \
(0), italic (1), bold (2) and bold italic (3), but value " +
        std::to_string (pattern_settings. variant) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if ((font_name == "Source Sans Pro") && (variant > 11)) {
        std::string reason {"Font Source Sans Pro has twelve variants: \
Ultra light (0), Light (1), Normal (2), Semi-Bold (3), Bold (4), Heavy (5), \
Ultra light Italic (6), Light Italic (7), Italic (8), Semi-Bold Italic (9), \
Bold Italic (10), Heavy Italic (11), but value " +
        std::to_string (pattern_settings. variant) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if ((font_name == "Source Code Pro") && (variant > 7)) {
        std::string reason {"Font Source Code Pro has eight variants: \
Ultra light (0), Light (1), Medium (2), Bold (3), Heavy (4), Italic (5), \
Bold Italic (6), but value " +
        std::to_string (pattern_settings. variant) + " was provided"} ;
        throw bad_parametre (reason. c_str ()) ;
    } ;
}

void dpps::Pattern_text::set_text (const std::string &text) {
    pattern_settings. text = text ;
}

void dpps::Pattern_text::set_font (const std::string &font_name) {
    pattern_settings. font_name =  font_name ;
    pattern_settings. font_changed = true ;
}

void dpps::Pattern_text::set_x (const double x) {
    pattern_settings. x0 =  x ;
}

void dpps::Pattern_text::set_y (const double y) {
    pattern_settings. y0 =  y ;
}

void dpps::Pattern_text::set_ex (const double ex) {
    pattern_settings. ex =  ex ;
}

void dpps::Pattern_text::set_parametres (
        const std::vector<bool> &vbool,
        const std::vector<long_unsigned_int> &vint,
        const std::vector<double> &vdouble,
        const std::vector<std::string> &vstring) {
    if ((vbool. size () != 1)   ||
        (vint. size () != 4)    ||
        (vdouble. size () != 3) ||
        (vstring. size () != 2)) {
        throw bad_parametre ("Pattern_text::set_parametres",
            2, 3, 3, 2,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint[0] > 2) {
        std::string reason {"Pattern_text::set_parametres, paramatre stroke \
is an enum valued 0-2, got " + std::to_string (vint[1])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if (vint[2] > 3) {
        std::string reason {"Pattern_text::set_parametres, paramatre align_x \
is an enum valued 0-3, got " + std::to_string (vint[1])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if (vint[3] > 3) {
        std::string reason {"Pattern_text::set_parametres, paramatre align_y \
is an enum valued 0-3, got " + std::to_string (vint[2])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (vbool[0],
                        static_cast<stroke_type> (vint[0]),
                        vint[1],
                        static_cast<text_align_x> (vint[2]),
                        static_cast<text_align_y> (vint[3]),
                        vdouble[0], vdouble[1], vdouble[2],
                        vstring[0], vstring[1]) ;
}

void dpps::Pattern_text::generate () {
    if (pattern_settings. text. empty ())
        return ;
    replace_string (pattern_settings. text, "${date}", current_date_string()) ;
    replace_string (pattern_settings. text, "${a-z}", "abcdefghijklmnopqrstuvwxyz") ;
    replace_string (pattern_settings. text, "${A-Z}", "ABCDEFGHIJKLMNOPQRSTUVWXYZ") ;
    replace_string (pattern_settings. text, "${0-9}", "0123456789") ;
    std::string::size_type pos {pattern_settings. text. find ("${all}")} ;
    if (pos != std::string::npos) {
        std::string alphabet ;
        for (unsigned short int i = 33 ; i < 127 ; i++)
            alphabet. push_back (char(i)) ;
        for (unsigned short int i = 161 ; i < 256 ; i++) {
            if (i == 173)
                i++ ;
            alphabet. push_back (char(i)) ;
        }
        replace_string (pattern_settings. text, "${all}", alphabet) ;
    }
    // replace_string (pattern_settings. text, "${lorem_ipsum}", "Nemo enim ipsam voluptatem, quia voluptas sit, aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos, qui ratione voluptatem sequi nesciunt, neque porro quisquam est, qui dolorem ipsum, quia dolor sit, amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt, ut labore et dolore magnam aliquam quaerat voluptatem.") ;

//    replace_string (pattern_settings. text, "${space}", " ") ;
//    replace_string (pattern_settings. text, "${comma}", ",") ;
    std::string copy ;
    if (pattern_settings. utf8)
        copy = utf8toiso8859_1 (pattern_settings. text) ;
    else
        copy = pattern_settings. text ;

    if (pattern_settings. font_changed || (!font. get_loaded ())) {
        font. set_font (pattern_settings. font_name) ;
        font. set_variant (pattern_settings. variant) ;
        font. set_stroke_type (pattern_settings. stroke) ;
        font. load () ;
    }

    double bottom {std::numeric_limits<double>::max()} ;
    double top {std::numeric_limits<double>::lowest()} ;
    double width {0.0} ;
    double caret {0.0} ;
    double ratio {pattern_settings. ex / font. ex} ;
    Pattern left_aligned_text ;
    // We first build a pattern with the text left-aligned.
    // std::cout << "copy: " << copy << "\n" ;
    for (long_unsigned_int i = 0 ; i < copy. size () ; i++) {
        // we want c in 0-255
        unsigned char c = copy [i] ;
        // Since c is 0-255, we avoid integer promotion problems when converting
        short unsigned int ic = static_cast<short unsigned int> (c) ;
        character letter {font. get_character (c)} ;
        if ((c == ' ') || (static_cast<short unsigned int>(c) == 128)) {
            // if we have a space or a non-breaking space, we just move the caret.
            caret += ratio * font. en ;
            continue ;
        }
        if (letter. size () == 0)
            continue ;
//             {            std::string reason {"on character number " + std::to_string (i) +
//             "of the argument, which is character number " +
//             std::to_string(static_cast<short unsigned int> (c)) +
//             "(" + std::to_string(c) + "), we got an empty Polyline from
// Typeface, indicating that the font is not complete, which is inconsistent
// with not having other exception before this one. This definitely should not
// have happened and indicates an internal bug."} ;
//             throw bad_parametre (reason. c_str ()) ;
//         }
        if (font. top [ic] > top)
            top = font. top [ic] ;
        if (font. bottom [ic] < bottom)
            bottom = font. bottom [ic] ;
        double left_letter {std::numeric_limits<double>::max()} ;
        for (auto p: letter) {
            double x0 {p. minimum_x ()} ;
            if (x0 < left_letter)
                left_letter = x0 ;
        }
        for (auto p: letter) {
            p = ((p + Vertex (-left_letter, 0)) * ratio) + Vertex (caret, 0.0) ;
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
            break ;
        //default: // unreachable
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

/*short unsigned int dpps::Pattern_text::choose_variant (const std::string &font_name, const bool italic, const short unsigned int thickness) {
    short unsigned int result {0} ;
    if (font_name == "Vollkorn") {
            if (!italic && thickness == 0)
                return 0 ;
            if (italic && thickness == 0)
                return 1 ;
            if (!italic && thickness > 0)
                return 2 ;
            if (italic && thickness > 0)
                return 3 ;
    } else {
        if (font_name == "Source Sans Pro") {
            if (italic)
                result = 6 ;
            result += (thickness <= 6? thickness:6) ;
        } else {
        if (font_name == "Source_Code_Pro") {
            if (italic) {
                result = 6 + (thickness == 0? 0:1) ;
            } else {
                result = (thickness < 6? thickness:6) ;
            }
        }
    }
    return 0 ;
}*/
