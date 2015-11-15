/**
 * @file Pattern_text.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that writes a text in a Polyline approximation of
 * fonts.
 * @date 2014-02-17 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_TEXT
#define DPPS_PATTERN_TEXT

#include "Pattern.hh"
#include "Typeface.hh"

//#include "fonts_declarations.hh"

namespace dpps {
    typedef enum {
        text_align_y_centred_baseline,
        text_align_y_centred_height,
        text_align_y_centred_bottom,
        text_align_y_centred_top
    } text_align_y ;

    typedef enum {
        text_align_x_centred,
        text_align_x_left,
        text_align_x_right
    } text_align_x ;

/** @brief Pattern derivative that adds text in format of a Polyline
 * approximation.
 *
 * Approximation is calculated by straigntening the Beziers.
 * To keep it simple, we limit ourselves to 256 characters of latin-1. It would
 * be great to support unicode, but that would probably be best done in a
 * separate class, which would then open arbitrary fonts by calling some
 * library. This class is really intended to be simple.
 *
 * This is not a full feature typography software. In particular it does not
 * implement paremeters pair kernings. Characters are just placed one next to
 * the others just like it would happen in letterpress printing with movable
 * types types sitting one next to the other on the composing stick.
 *
 * The characters are added one by one into a temporary polyline vector. The
 * characters which are not implemented (for lack of time or just control
 * characters like codes 0-31) are ignored. The polyline is then adjusted
 * for the centering and position and added to the pattern.
 *
 * Does not handle (yet) multiline. Carriage return is ignored like other
 * control characters.
 *
 */
class Pattern_text: public Pattern {
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_text_settings: public Pattern_settings {
    public:
        /** @brief Whether the text is passed in utf-8 (if true) or iso-8859-1
         * (if false) */
        bool utf8 {true} ;

        /** @brief Whether the shapes with holes (like a o) are drawn with
         * a single stroke (without raising the pen from the paper) (if true)
         * or if all shapes are just drawn as they are (if false). Set to true
         * to make sure that the inner side is properly defined for exposure
         * systems based on areas (optical or electronic lithography) when
         * wanting to fill the inner side of the exposure. Set to false if
         * the desired exposure is based on lines, not areas (like the Witec
         * system).
         */
        bool continuous_stroke {false} ;

        /** @brief the x position of the aligned point. Meaning depends on the
         * alignment mode along x defined by align_x */
        double x0 {0.0} ;

        /** @brief the y position of the aligned point. Meaning depends on the
         * alignment mode along x defined by align_y */
        double y0 {0.0} ;

        /** @brief the text to type */
        std::string text {} ;

        /** @brief the number of the font. Currently unused as just one font
         * is implemented.
         */
        text_font font {Vollkorn} ;

        /** @brief the variant of the typeface. The limits depend on the
         * typeface. Volkorn has 4 variants, Source Sans Pro has 12, Source
         * Code pro has 8. The class provides ...
         *
         */
        short unsigned int variant {0} ;

        /** @brief the alignment along x. Values can be 0 for centre, 1 for
         * left, 2 for right.
         */
        text_align_x align_x {text_align_x_centred} ;

        /** @brief the alignment along x. Values can be 0 for baseline,
         * 1 to centre around the height of the whole text height, 2 for
         * bottom, 3 for top.
         */
        text_align_y align_y {text_align_y_centred_baseline} ;

        /** @brief the size of the font expressed in height of the x.
         */
        double ex {10.0} ;
    } ;
protected:
    Pattern_text_settings pattern_settings ;
    Typeface font ;

    /** @brief we will use a space of 0.2 em */
    double space_size_em {0.2} ;
public:
    /** @brief the default constructor */
    Pattern_text () ;

    /** @brief the copy constructor */
    Pattern_text (const Pattern_text &source) ;

    /** @brief the move constructor. */
    Pattern_text (Pattern_text &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_text &operator= (Pattern_text &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_text &operator= (const Pattern_text &source) ;

    Pattern_text (
        const bool utf8,
        const bool continuous_stroke,
        const text_font font,
        const short unsigned int variant,
        const text_align_x align_x,
        const text_align_y align_y,
        const double x0,
        const double y0,
        const double ex,
        const std::string &text) ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;
    void set_all_parametres (
        const bool utf8,
        const bool continuous_stroke,
        const text_font font,
        const short unsigned int variant,
        const text_align_x align_x,
        const text_align_y align_y,
        const double x0,
        const double y0,
        const double ex,
        const std::string &text) ;

    /** @brief returns a valid variant number for the passed font in roman or
     * italic, using the thickness as an indication.
     *
     * Source Sans pro has 6 levels of thickness (0 to 5) in roman and italic,
     * Source Code Pro has 6 levels of thickness (0 to 5) in roman, two in
     * italic. Vollkorn has two levels of thickness (0 and 1) in roman and
     * italic.
     *
     * If thickness is too large the maximum is used. A valid variant number is
     * always returned, it then can be used to pass to set_all_parametres()
     */
    short unsigned int get_variant (const text_font font, const bool italic, const short unsigned int thickness) ;
} ;
} // namespace dpps
#endif
