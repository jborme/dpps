/**
 * @file Pattern_hard_sphere_random_packing.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivativethat is empty. This is useful to use with a Reader
 * derivative.
 * @date 2014-02-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>

#include "common.hh"
#include "Pattern_hard_sphere_random_packing.hh"
#include "bad_parametre.hh"

// Included class hard_sphere
/////////////////////////////
dpps::Pattern_hard_sphere_random_packing::hard_sphere::hard_sphere (
    const double x0, const double y0, const bool set_full):
            x(x0), y(y0), full(set_full) {
}

dpps::Pattern_hard_sphere_random_packing::hard_sphere::hard_sphere () {
}

void dpps::Pattern_hard_sphere_random_packing::hard_sphere::display () const {
    std::cout << display_string () << std::endl ;
}

std::string dpps::Pattern_hard_sphere_random_packing::hard_sphere::
    display_string () const {
    std::string result {"(" + std::to_string (x) + ", " +
        std::to_string (y) + ", " + (full?"f":"o") + ")"} ;
    return result ;
}

double dpps::Pattern_hard_sphere_random_packing::hard_sphere::squared_distance (
    const dpps::Pattern_hard_sphere_random_packing::hard_sphere &s) const {
    return ((x - s. x) * (x - s. x) + (y - s. y) * (y - s. y)) ;
}
/////////////////////////////

dpps::Pattern_hard_sphere_random_packing::Pattern_hard_sphere_random_packing (): Pattern () {
}

dpps::Pattern_hard_sphere_random_packing::Pattern_hard_sphere_random_packing (const Pattern_hard_sphere_random_packing &source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_hard_sphere_random_packing::Pattern_hard_sphere_random_packing (Pattern_hard_sphere_random_packing &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_hard_sphere_random_packing &dpps::Pattern_hard_sphere_random_packing::operator= (
    dpps::Pattern_hard_sphere_random_packing &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_hard_sphere_random_packing &dpps::Pattern_hard_sphere_random_packing::operator= (
    const dpps::Pattern_hard_sphere_random_packing &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

dpps::Pattern_hard_sphere_random_packing::Pattern_hard_sphere_random_packing (
    const long_unsigned_int number_total,
    const long_unsigned_int number_random,
    const double x0,
    const double y0,
    const double lx,
    const double ly,
    const double diametre) {
    set_all_parametres (number_total, number_random, x0, y0, lx, ly, diametre) ;
    generate () ;
}

void dpps::Pattern_hard_sphere_random_packing::set_all_parametres (
    const long_unsigned_int number_total,
    const long_unsigned_int number_random,
    const double x0,
    const double y0,
    const double lx,
    const double ly,
    const double diametre) {
    pattern_settings. x0 = x0 ;
    pattern_settings. y0 = y0 ;
    pattern_settings. lx = lx ;
    pattern_settings. ly = ly ;
    pattern_settings. diametre = diametre ;
    if (pattern_settings. diametre <= 0) {
        std::string reason {"Pattern_hard_sphere_random_packing::set_parametres\
: diametre of the atomic element must be strictly positive, value provided was"
+ std::to_string (pattern_settings. diametre)} ;
        throw (reason. c_str ()) ;
    }
    pattern_settings. number_total = number_total ;
    pattern_settings. number_random = number_random ;

    }
void dpps::Pattern_hard_sphere_random_packing::
        set_parametres (const std::vector<bool> &vbool,
                        const std::vector<long_unsigned_int> &vint,
                        const std::vector<double> &vdouble,
                        const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)    ||
        (vint. size () != 2)    ||
        (vdouble. size () != 5) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_hard_sphere_random_packing::\
set_parametres",
            0, 2, 5, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;

    }

    set_all_parametres (vint[0], vint[1], vdouble[0], vdouble[1],
                        vdouble[2], vdouble[3], vdouble[4]) ;
}

void dpps::Pattern_hard_sphere_random_packing::generate () {
    constexpr const static short unsigned int max_angle_iterations {50} ;
    constexpr const static double two_pi {2*M_PI} ;
    constexpr const static long_unsigned_int factor_maximum_trials {3} ;
    if (pattern_settings. number_total == 0)
        return ;
    if (pattern_settings. number_random > pattern_settings. number_total)
        pattern_settings. number_random = pattern_settings. number_total ;
    std::vector<hard_sphere> open_spheres, full_spheres ;
    hard_sphere s ;
    bool finished {false} ;

    // s. diametre = pattern_settings. diametre ;
    if (pattern_settings. number_random == 0)
        open_spheres. push_back (s) ;
    else {
        std::uniform_real_distribution<double> dx (
            pattern_settings. x0 - pattern_settings. lx /2.0,   // min
            pattern_settings. x0 + pattern_settings. lx /2.0) ; // max
        std::uniform_real_distribution<double> dy (
            pattern_settings. y0 - pattern_settings. ly /2.0,
            pattern_settings. y0 + pattern_settings. ly /2.0) ;
        long_unsigned_int i {0} ;
        while (!finished) {
            i++ ;
            hard_sphere possible_place (dx (pseudorandom_generator),
                                        dy (pseudorandom_generator),
                                        true) ;
            bool already_a_sphere_here {false} ;
            for (long_unsigned_int j = 0 ;
                    j < open_spheres. size () ; j++) {
                if (open_spheres[j]. squared_distance (possible_place) <
                    pattern_settings. diametre*pattern_settings. diametre) {
                    already_a_sphere_here = true ;
                    break ;
                }
            }
            if (already_a_sphere_here)
                continue ;
            open_spheres. push_back (possible_place) ;
            finished = ((open_spheres. size () >=
                            pattern_settings. number_random) ||
                        (i > pattern_settings. number_random *
                             factor_maximum_trials)) ;
        }
    }
    finished = (full_spheres. size () + open_spheres. size ()
                >= pattern_settings. number_total) ;
    std::uniform_int_distribution<long_unsigned_int> d_int ;
    std::uniform_real_distribution<double> d_real (0, two_pi) ;
    while (!finished) {
        // To choose an element from open_sheres, we tune the
        // distribution parametres.
        // Nokilai Josuttis, The C++ Standard Library 2nd Ed., §17.1.4 p. 919
        std::uniform_int_distribution<long_unsigned_int>::param_type
                param (0, open_spheres.size () - 1) ;
        long_unsigned_int pos {d_int (pseudorandom_generator, param)} ;
        s = open_spheres. at (pos) ;
        bool already_a_sphere_here {false} ;
        // We choose one angle and see if there is space to add a new sphere
        // at this angle around the one already selected.
        // We repeat many times as finding a suitable angle might not be easy.
        for (short unsigned int i = 0 ; i < max_angle_iterations ; i++) {
            // We try to locate a sphere at angle (from x) in contact with the
            // chose sphere.
            double angle {d_real (pseudorandom_generator)} ;
            double x1 {s. x+(pattern_settings. diametre * cos (angle))} ;
            double y1 {s. y+(pattern_settings. diametre * sin (angle))} ;
            hard_sphere possible_place (x1, y1, true) ;
            // std::cout << "i " ;
            if ((x1 < pattern_settings. x0 - (pattern_settings. lx / 2.0)) ||
                (y1 < pattern_settings. y0 - (pattern_settings. ly / 2.0)) ||
                (x1 > pattern_settings. x0 + (pattern_settings. lx / 2.0)) ||
                (y1 > pattern_settings. y0 + (pattern_settings. ly / 2.0)))
                continue ;
            already_a_sphere_here = false ;
            // We see if there is any open sphere in the close vicinity,
            // exlcuding the one we considered intitially at [pos]

            for (long_unsigned_int j = 0 ; j < open_spheres. size () ; j++) {
                if (j == pos)
                    continue ;
                if (open_spheres[j]. squared_distance (possible_place) <
                    pattern_settings. diametre*pattern_settings. diametre) {
                    already_a_sphere_here = true ;
                    break ;
                }
            }
            if (already_a_sphere_here)
                continue ;
            // Now for closed spheres... we do not to check for equality to pos.
            for (long_unsigned_int j = 0 ; j < full_spheres. size () ; j++) {
                if (full_spheres[j]. squared_distance (possible_place) <
                    pattern_settings. diametre*pattern_settings. diametre) {
                    already_a_sphere_here = true ;
                    break ;
                }
            }
            // We found a sphere, we add it.
            if (already_a_sphere_here)
                continue ;
            // If we are here, (x1, y1) is not too close from another sphere.
            //std::cout << "On ajoute la sphere " ;
            // s. x = x1 ;
            // s. y = y1 ;
            open_spheres. push_back (possible_place) ;
            break ;
        }
        if (already_a_sphere_here) {
            // We got out the for loop without finding a suitable place
            // so classify the selected sphere as full.
            full_spheres. push_back (open_spheres[pos]) ;
            // NOTE: gcc allows cbegin() here, but clang does not.
            open_spheres. erase (open_spheres. begin () + pos) ;
        }
        // End criterion
        if (full_spheres. size () + open_spheres. size ()
            >= pattern_settings. number_total) {
            finished = true ;
        }
        else
            finished = (open_spheres. size () == 0) ;
    }
    // To perform just one iteration, we will join the two vectors.
    // NOTE: gcc allows cbegin() and cend() here, but clang does not.
    full_spheres. insert (full_spheres. end (), open_spheres. begin (),
        open_spheres. end ()) ;
    // the reference &i is used to avoid copy
    Polyline q ;
    q. dose = pattern_settings. diametre / 2.0 ;
    for (auto &i: full_spheres) {
        q. push_back (Vertex (i. x, i. y)) ;
        polylines. push_back (q) ;
        q. vertices. clear () ;
    }
}

