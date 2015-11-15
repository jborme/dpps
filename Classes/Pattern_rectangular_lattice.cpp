/**
 * @file Pattern_rectangular_lattice.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of long lines like a diffraction grating.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>
#include <limits>

#include "Pattern_rectangular_lattice.hh"
#include "bad_parametre.hh"

dpps::Pattern_rectangular_lattice::Pattern_rectangular_lattice (): Pattern () {
}

dpps::Pattern_rectangular_lattice::Pattern_rectangular_lattice (const Pattern_rectangular_lattice &source):
    Pattern (source),
    side_x (source. side_x),
    side_y (source. side_y),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_rectangular_lattice::Pattern_rectangular_lattice (Pattern_rectangular_lattice &&source):
    Pattern (source),
    side_x (source. side_x),
    side_y (source. side_y),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_rectangular_lattice &dpps::Pattern_rectangular_lattice::operator= (
    dpps::Pattern_rectangular_lattice &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        side_x = source. side_x ;
        side_y = source. side_y ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_rectangular_lattice &dpps::Pattern_rectangular_lattice::operator= (
    const dpps::Pattern_rectangular_lattice &source) {
    Pattern::operator=(source) ;
    side_x = source. side_x ;
    side_y = source. side_y ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

dpps::Pattern_rectangular_lattice::Pattern_rectangular_lattice (
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const double period_x,
    const double period_y,
    const double area_ratio) {
    set_all_parametres (minx, miny, maxx, maxy, period_x, period_y, area_ratio) ;
    generate () ;
}

void dpps::Pattern_rectangular_lattice::set_all_parametres (
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const double period_x,
    const double period_y,
    const double area_ratio) {
    pattern_settings. coordinates[0] = minx ;
    pattern_settings. coordinates[1] = miny ;
    pattern_settings. coordinates[2] = maxx ;
    pattern_settings. coordinates[3] = maxy ;
    pattern_settings. period_x       = period_x ;
    pattern_settings. period_y       = period_y ;
    pattern_settings. area_ratio     = area_ratio ;
    // solving the following system for side_x and side_y:
    // { (side_x side_y) / (period_x period_y) = area_ratio
    // { side_x/period_x = size_y/period_y
    side_x = pattern_settings. period_x * sqrt (pattern_settings. area_ratio) ;
    side_y = pattern_settings. period_y * sqrt (pattern_settings. area_ratio) ;
}

void dpps::Pattern_rectangular_lattice::set_parametres (
                    const std::vector<bool> &vbool,
                    const std::vector<long_unsigned_int> &vint,
                    const std::vector<double> &vdouble,
                    const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 7) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_rectangular_lattice::set_parametres", \
            0, 0, 7, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    set_all_parametres (vdouble[0], vdouble[1], vdouble[2], vdouble[3],
                        vdouble[4], vdouble[5], vdouble[6]) ;
}

void dpps::Pattern_rectangular_lattice::generate () {
    double minx {pattern_settings. coordinates[0]} ;
    double miny {pattern_settings. coordinates[1]} ;
    double maxx {pattern_settings. coordinates[2]} ;
    double maxy {pattern_settings. coordinates[3]} ;
    if (maxx <= minx) throw bad_parametre
       ("Pattern_rectangular_lattice::generate, maximum_x is smaller or equal than \
the minimum_x.") ;
    if (maxy <= miny) throw bad_parametre
       ("Pattern_rectangular_lattice::generate, maximum_y is smaller or equal than \
the minimum_y.") ;
    Vertex v ;
    Polyline p ;
    p. closed = true ;
    double startx {minx} ;
    while (startx < maxx) {
        double starty {miny} ;
        while (starty < maxy) {
            if (fabs (pattern_settings. area_ratio) < std::numeric_limits<double>::epsilon ()) {
                // If zero was passed, then we create a vertex lattice,
                // instead of a lattice of rectangulars.
                v. x = startx ;
                v. y = starty ;
                p. push_back (v) ;
            } else {
                // Vertex and Polyline are passed as parametre and inserted in
                // vectors by copy, so we can always push_back the same variable.
                v. x = startx ;
                v. y = starty ;
                p. push_back (v) ;
                v. x += side_x ;
                p. push_back (v) ;
                v. y += side_y ;
                p. push_back (v) ;
                v. x = startx ;
                p. push_back (v) ;
            }
            polylines. push_back (p) ;

            // preparing for next iteration
            p. vertices. clear () ; // does not change that p. closed == true ;
            starty += pattern_settings. period_y ;
        }
        startx += pattern_settings. period_x ;
    }
}