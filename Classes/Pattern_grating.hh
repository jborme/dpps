/**
 * @file Pattern_grating.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of long lines like a diffraction grating.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PATTERN_GRATING
#define PATTERN_GRATING

#include "Pattern.hh"

namespace dpps {
/** @brief Pattern_grating is a Pattern derivative that generates a diffraction
 * grating with a certain covered area.
 *
 * The grating is made of long and thin rectangles whose long direction is along
 * the y axis.
 */
class Pattern_grating: public Pattern {
public:
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_grating_settings: public Pattern_settings {
    public:
        /** @brief Limit coordinates of the design, in order (xmin, ymin,
         * xmax, ymax). */
        std::array<double, 4> coordinates ;

        /** @brief the distance from one line to the next. */
        double period {1.0} ;

        /** @brief the amount of surface that is covered by the inside of the
         * structure.
         *
         * The value is nominally from 0 to 1 but is constrained. Values outside
         * this range have little practical use but lead to valid designs.
         * Values larger than 1 or smaller than -1 will cause overlapping
         * structures. Negative values will cause the structures to be drawn to
         * the left instead of to the right.
         */
        double area_ratio {0.5} ;
    } ;

protected:
    Pattern_grating_settings pattern_settings ;

public:
    /** @brief the default constructor */
    Pattern_grating () ;

    Pattern_grating (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period,
        const double area_ratio) ;

    /** @brief the copy constructor */
    Pattern_grating (const Pattern_grating &source) ;

    /** @brief the move constructor. */
    Pattern_grating (Pattern_grating &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_grating &operator= (Pattern_grating &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_grating &operator= (const Pattern_grating &source) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;

    void set_all_parametres (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period,
        const double area_ratio) ;
} ;
} // namespace dpps
#endif
