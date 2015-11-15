/**
 * @file Polyline_factory.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for class Polyline_factory
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The file \c Polyline_factory.hh contains one class with static functions
 * to produce a single Polyline, as function of a arguments.
 */
#ifndef DPPS_POLYLINE_FACTORY
#define DPPS_POLYLINE_FACTORY

#include "Polyline.hh"

namespace dpps {
/** @brief
 * The class \c Polyline_factory hosts functions that return certain kinds
 * of Polyline as function of arguments.
 */
class Polyline_factory {
public:
    //static Polyline cut_into (Polyline outer, const Polyline &inner) ;
    static Polyline frame (const double outer_size, const double thickness) ;
    static Polyline frame (const double outer_size_x, const double outer_size_y, const double thickness) ;
    static Polyline cross (const double outer_size, const double thickness) ;
    static Polyline crossing_star_polygon (
        const long_unsigned_int p,
        const long_unsigned_int q,
        const double diametre) ;
    static Polyline noncrossing_star_polygon (
        const bool regular,
        const long_unsigned_int p,
        const long_unsigned_int q,
        const double diametre,
        double ratio) ;
    static Polyline polygonal_ring (const long_unsigned_int outer_vertices,
                           const long_unsigned_int inner_vertices,
                           const double outer_diameter,
                           const double inner_diameter) ;

    static Polyline polygonal_elliptic_ring (
        const long_unsigned_int outer_vertices,
        const long_unsigned_int inner_vertices,
        const double outer_major_axis,
        const double outer_minor_axis,
        const double outer_angle_x,
        const double inner_major_axis,
        const double inner_minor_axis,
        const double inner_angle_x) ;

} ;
} // namespace dpps
#endif