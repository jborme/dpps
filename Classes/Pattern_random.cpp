/**
 * @file Pattern_random.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of randomly situated elements according to the
 * different real distributions supported by the C++11 standard.
 * @date 2013-02-03 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <limits>

#include "Pattern_random.hh"
#include "bad_parametre.hh"

dpps::Pattern_random::Pattern_random (): Pattern () {
}

dpps::Pattern_random::Pattern_random (const Pattern_random &source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_random::Pattern_random (Pattern_random &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_random &dpps::Pattern_random::operator= (
    dpps::Pattern_random &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_random &dpps::Pattern_random::operator= (
    const dpps::Pattern_random &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

dpps::Pattern_random::Pattern_random (
    const enum_distribution type,
    const long_unsigned_int number,
    const long_unsigned_int max_attempts,
    const double x0,
    const double y0,
    const double lx,
    const double ly,
    const double side,
    const double p1,
    const double p2) {
    set_all_parametres (type, number, max_attempts, x0, y0, lx, ly, side, p1, p2) ;
    generate () ;
}

void dpps::Pattern_random::set_all_parametres (
    const enum_distribution type,
    const long_unsigned_int number,
    const long_unsigned_int max_attempts,
    const double x0,
    const double y0,
    const double lx,
    const double ly,
    const double side,
    const double p1,
    const double p2) {
    pattern_settings. type = type ;
    pattern_settings. number = number ;
    pattern_settings. max_attempts = max_attempts ;
    pattern_settings. x0     = x0 ;
    pattern_settings. y0     = y0 ;
    pattern_settings. lx     = lx ;
    pattern_settings. ly     = ly ;
    pattern_settings. side   = side ;
    pattern_settings. p1     = p1 ;
    pattern_settings. p2     = p2 ;
}

void dpps::Pattern_random::set_parametres (
                            const std::vector<bool> &vbool,
                            const std::vector<long_unsigned_int> &vint,
                            const std::vector<double> &vdouble,
                            const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 3)    ||
        (vdouble. size () != 7) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_random::set_parametres",
            0, 3, 7, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint[0] >= 10) {
        std::string reason {"Pattern_random::set_parametres, parametre called \
type is an enum which is valued 0-9, got " + std::to_string (vint[0])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (static_cast<enum_distribution> (vint[0]),
                        vint[1], vint[2], vdouble[0], vdouble[1], vdouble[2],
                        vdouble[3], vdouble[4], vdouble[5], vdouble[6]) ;
}

void dpps::Pattern_random::generate () {
    // We have do declare them all. We cannot do a switch of if,
    // because distributions would get out of scope.
    // The only other possibility would be to place the switch inside
    // the loop, but it would be very inefficient to call the constructor
    // everytime.
    // So we have to pay for overhead and initialize them all in te beginning.
    // It's probably a negligible amount of time and memory as compared to the
    // loop.

    // signification of parametres depends on the engine. Some engines
    // need only one parametre.
    // For all user-changeable values (p1 and p2), 1.0 would have been the
    // default if we had not specified it, so it is an acceptable value for the
    // user as well.
    const double p1 {pattern_settings. p1} ;
    const double p2 {pattern_settings. p2} ;

    std::uniform_real_distribution<double> d0 (-p1, p1) ; // min, max
    // Normal-type
    std::normal_distribution<double> d1 (0.0, p1) ;    // average = 0, sigma
    std::lognormal_distribution<double> d2 (0.0, p1) ; // average = 0, m
    std::chi_squared_distribution<double> d3 (p1) ;    // n
    std::cauchy_distribution<double> d4 (p1, p2) ;     // a, b
    std::fisher_f_distribution<double> d5 (p1, p2) ;   // m, n
    std::student_t_distribution<double> d6 (p1) ;      // n
    // Poisson-type
    std::exponential_distribution<double> d7 (p1) ;    // lambda
    std::gamma_distribution<double> d8 (p1, p2) ;      // alpha, beta
    std::weibull_distribution<double> d9 (p1, p2) ;    // a, b

    Polyline p ;
    p. closed = true ;
    double x {0.0}, y {0.0} ;
    long_unsigned_int i {0}, attempts {0} ;
    const double s {pattern_settings. side / 2.0} ;
    if (pattern_settings. max_attempts < pattern_settings. number)
        pattern_settings. max_attempts =
            std::numeric_limits<long_unsigned_int>::max () ;
    //std::cout << "hello" << i << " " << pattern_settings. number << " " <<  attempts << " " << pattern_settings. max_attempts << std::endl ;
    while ((i < pattern_settings. number) &&
           (attempts < pattern_settings. max_attempts)) {
        switch (pattern_settings. type) {
            case type_uniform_real_distribution: // 0
                x = d0 (pseudorandom_generator) ;
                y = d0 (pseudorandom_generator) ;
                //std::cout << "alea 0 : " << x << " " << y << std::endl ;
                break ;
            case type_normal_distribution: // 1
                x = d1 (pseudorandom_generator) ;
                y = d1 (pseudorandom_generator) ;
                break ;
            case type_lognormal_distribution: // 2
                x = d2 (pseudorandom_generator) ;
                y = d2 (pseudorandom_generator) ;
                break ;
            case type_chi_squared_distribution: // 3
                x = d3 (pseudorandom_generator) ;
                y = d3 (pseudorandom_generator) ;
                break ;
            case type_cauchy_distribution: // 4
                x = d4 (pseudorandom_generator) ;
                y = d4 (pseudorandom_generator) ;
                break ;
            case type_fisher_f_distribution: // 5
                x = d5 (pseudorandom_generator) ;
                y = d5 (pseudorandom_generator) ;
                break ;
            case type_student_t_distribution: // 6
                x = d6 (pseudorandom_generator) ;
                y = d6 (pseudorandom_generator) ;
                break ;
            case type_exponential_distribution: // 7
                x = d7 (pseudorandom_generator) ;
                y = d7 (pseudorandom_generator) ;
                break ;
            case type_gamma_distribution: // 8
                x = d8 (pseudorandom_generator) ;
                y = d8 (pseudorandom_generator) ;
                break ;
            case type_weibull_distribution: // 9
                x = d9 (pseudorandom_generator) ;
                y = d9 (pseudorandom_generator) ;
                break ;
            default:
                break ;
        }
        // Either we are inside lx, or user set it to negative to disable it.
        if (((fabs (x - pattern_settings. x0) < pattern_settings. lx / 2.0) ||
             (pattern_settings. lx <= 0)) &&
            ((fabs (y - pattern_settings. y0) < pattern_settings. ly / 2.0) ||
             (pattern_settings. ly <= 0))) {
            p. push_back (Vertex (x - s, y - s)) ;
            p. push_back (Vertex (x - s, y + s)) ;
            p. push_back (Vertex (x + s, y + s)) ;
            p. push_back (Vertex (x + s, y - s)) ;
            polylines. push_back (p) ;
            p. vertices. clear () ; // does not change that p. closed == true ;
            i++ ;
        }
        attempts++ ;
    }
}
