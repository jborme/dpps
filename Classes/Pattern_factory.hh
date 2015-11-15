/**
 * @file Pattern_factory.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for class Pattern_factory
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_FACTORY
#define DPPS_PATTERN_FACTORY

#include "Polyline.hh"
#include "Pattern.hh"

namespace dpps {
typedef std::vector<std::vector<std::vector<std::pair<double,double>>>> shape ;

class Pattern_factory {
public:

    /* * @brief Replaces the current Polyline a regular polygon whose
     * circonscribed circle is the circle tangent to the first two edges of the
     * polyline.
     *
     * The parametre gives the number of vertices o the resulting polygon,
     * which works as an approximation to the circle.
     *
     * If the Polyline describes a tangential (or circumscribed) polygon,
     * then the circle mentioned is the incircle of the Polyline. However no
     * check is done whether the Polyline is actually inscrible.
     *
     * The latter property would not difficult to implement however, see:
     * https://en.wikipedia.org/wiki/Pitot_theorem
     * */
     //static vector Polyline smallest_tangent_circle_polygon (
     //    long_unsigned_int edges) ;

    // Functions returning a vector of polylines //
    ///////////////////////////////////////////////
    /** @brief Function returning a vector of Polyline, each of them with two
     * vertices, corresponding to the successive segments of the current
     * Polyline */
    static dpps::Pattern explode (const Polyline &polyline) ;

    /** @brief Function which converts a closed Polyline into a vector of
     * two-vertices Polyline that can be drawn along a line.
     *
     * The algorithm will only produce meaningful results if the Polyline
     * describes a convex shape. Concave shapes should be first fractured
     * in convex elements. Given the complexity of the general fracturing
     * problem and the large number of different algorithm which are
     * optimized for different kinds of lithography, fracturing was not
     * implemented in this package. It is let to the user to make a design
     * already fractured the way which optimizes better for the application.
     *
     * If the line is open then it is not altered
     */
    static dpps::Pattern fill_with_lines (
                            const Polyline &polyline,
                            const double angle,
                            const double spacing) ;

    /** @brief Function which converts a closed Polyline into a vector of
     * two-vertices Polyline that can be drawn along a line. The angle
     * of fill_with_lines of taken to be equal to the first direction of each
     * Polyline.
     */
    static dpps::Pattern fill_with_lines_first_angle (
                            const Polyline &polyline,
                            const double spacing) ;

    // designed for C++ code, not for Lua bindings.
    static dpps::Pattern from_initialization_list (shape list) ;

    static dpps::Pattern fill_with_dashes (
                         const Polyline &polyline,
                         const double angle,
                         const double period_longitudinal,
                         const double spacing_transversal,
                         const double duty_cycle,
                         const double phase_initial,
                         const double phase_increment,
                         const enum_partial_dash_policy partial_dash_policy) ;
    static dpps::Pattern fill_with_dashes_first_angle (
                            const Polyline &polyline,
                            const double period_longitudinal,
                            const double spacing_transversal,
                            const double duty_cycle,
                            const double phase_initial,
                            const double phase_increment,
                            const enum_partial_dash_policy partial_dash_policy) ;

    static dpps::Pattern Pattern_from_selected (const Pattern &p, const selection_t selection = 0) ;
} ;
} // namespace dpps
#endif