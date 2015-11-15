/**
 * @file Pattern_grating.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of long lines like a diffraction grating.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "Pattern_grating.hh"
#include "bad_parametre.hh"

dpps::Pattern_grating::Pattern_grating (): Pattern () {
}

dpps::Pattern_grating::Pattern_grating (const Pattern_grating &source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_grating::Pattern_grating (Pattern_grating &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_grating::Pattern_grating (
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const double period,
    const double area_ratio) {
    set_all_parametres (minx, miny, maxx, maxy, period, area_ratio) ;
    generate () ;
}

dpps::Pattern_grating &dpps::Pattern_grating::operator= (
    dpps::Pattern_grating &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_grating &dpps::Pattern_grating::operator= (
    const dpps::Pattern_grating &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

void dpps::Pattern_grating::set_all_parametres (
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const double period,
    const double area_ratio) {
    pattern_settings. coordinates[0] = minx ;
    pattern_settings. coordinates[1] = miny ;
    pattern_settings. coordinates[2] = maxx ;
    pattern_settings. coordinates[3] = maxy ;
    pattern_settings. period         = period ;
    pattern_settings. area_ratio     = area_ratio ;
}
void dpps::Pattern_grating::set_parametres (
                                    const std::vector<bool> &vbool,
                                    const std::vector<long_unsigned_int> &vint,
                                    const std::vector<double> &vdouble,
                                    const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 6) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_grating::set_parametres",
            0, 0, 6, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;

    }
    set_all_parametres (vdouble[0], vdouble[1], vdouble[2],
                        vdouble[3], vdouble[4], vdouble[5]) ;
}

void dpps::Pattern_grating::generate () {
    double minx {pattern_settings. coordinates[0]} ;
    double miny {pattern_settings. coordinates[1]} ;
    double maxx {pattern_settings. coordinates[2]} ;
    double maxy {pattern_settings. coordinates[3]} ;
    if (maxx <= minx) throw bad_parametre
       ("Pattern_grating::generate, maximum_x is smaller or equal than the \
minimum_x.") ;
    if (maxy <= miny) throw bad_parametre
       ("Pattern_grating::generate, maximum_y is smaller or equal than the \
minimum_y.") ;
    double startx {minx} ;
    Vertex v ;
    Polyline p ;
    p. closed = true ;
    while (startx < maxx) {
        // Vertex and Polyline are passed as parametre and inserted in vectors
        // by copy, so we can always push_back the same variable.
        v. x = startx ;
        v. y = miny ;
        p. push_back (v) ;
        // v. x = startx ;
        v. y = maxy ;
        p. push_back (v) ;
        v. x = startx + pattern_settings. period *
                        pattern_settings. area_ratio ;
        //v. y = maxy ;
        p. push_back (v) ;
        // v. x = startx + pattern_settings. period *
        // pattern_settings. area_ratio ;
        v. y = miny ;
        p. push_back (v) ;
        polylines. push_back (p) ;

        // preparing for next iteration
        p. vertices. clear () ; // does not change that p. closed == true ;
        startx += pattern_settings. period ;
    } ;

}