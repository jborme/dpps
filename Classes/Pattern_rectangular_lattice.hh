/**
 * @file Pattern_rectangular_lattice.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of long lines like a diffraction grating.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_SQUARE_LATTICE
#define DPPS_PATTERN_SQUARE_LATTICE

#include "Pattern.hh"

namespace dpps {
/** @brief Pattern_rectangular_lattice is a Pattern derivative that generates a
 * 2D diffraction grating composed of rectangulars.
 *
 * One can choose the period of the pattern and the area percentage covered
 * by the inside of the rectangulars.
 */
class Pattern_rectangular_lattice: public Pattern {
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_rectangular_lattice_settings: public Pattern_settings {
    public:
        /** @brief Limit coordinates of the design, in order (xmin, ymin,
         * xmax, ymax). */
        std::array<double, 4> coordinates {{-10.0, -10.0, 10.0,  10.0}} ;

        /** @brief the distance from one rectangular to the next in x */
        double period_x = 1.0 ;
        /** @brief the distance from one rectangular to the next in y */
        double period_y = 1.0 ;

        /** @brief the amount of surface that is covered by the inside of the
         * structure.
         *
         * The value is nominally from 0 to 1 but is constrained. Values outside
         * this range have little practical use but lead to valid designs.
         * Values larger than 1 or smaller than -1 will cause overlapping
         * structures. Negative values will cause the structures to be drawn to
         * the left and to the top instead of to the right and to the bottom.
         */
        double area_ratio = 0.5 ;
    } ;
protected:
    double side_x {0.0} ;
    double side_y {0.0} ;
    Pattern_rectangular_lattice_settings pattern_settings ;

public:
    /** @brief the default constructor */
    Pattern_rectangular_lattice () ;

    /** @brief the copy constructor */
    Pattern_rectangular_lattice (const Pattern_rectangular_lattice &source) ;

    /** @brief the move constructor. */
    Pattern_rectangular_lattice (Pattern_rectangular_lattice &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_rectangular_lattice &operator= (Pattern_rectangular_lattice &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_rectangular_lattice &operator= (const Pattern_rectangular_lattice &source) ;

    Pattern_rectangular_lattice (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period_x,
        const double period_y,
        const double area_ratio = 0.0) ;
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
        const double period_x,
        const double period_y,
        const double area_ratio) ;
} ;
} // namespace dpps
#endif
