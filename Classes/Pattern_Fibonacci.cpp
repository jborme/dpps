/**
 * @file Pattern_Fibonacci.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that implements a Fibonacci grating
 * @date 2014-01-28 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Some bibliography to read:
 * * C Janot. Quasicrystals, a primer, Oxford University Press.
 * * https://en.wikipedia.org/wiki/Fibonacci_word
 * * R. Dallapiccola et al., Optics Express, Vol. 16 no. 8, pp. 5544-5555 (2008)
 *   http://dx.doi.org/10.1364/OE.16.005544
 *
 */

#include <vector>

#include "Pattern_Fibonacci.hh"
#include "bad_parametre.hh"

dpps::Pattern_Fibonacci::Pattern_Fibonacci (): Pattern () {
}

dpps::Pattern_Fibonacci::Pattern_Fibonacci (const Pattern_Fibonacci &source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_Fibonacci::Pattern_Fibonacci (Pattern_Fibonacci &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_Fibonacci::Pattern_Fibonacci (
    const bool bidimentionnal,
    const long_unsigned_int generation,
    const double x0,
    const double y0,
    const double size) {
        set_all_parametres (bidimentionnal, generation, x0, y0, size) ;
        generate () ;
}

dpps::Pattern_Fibonacci &dpps::Pattern_Fibonacci::operator= (
    dpps::Pattern_Fibonacci &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_Fibonacci &dpps::Pattern_Fibonacci::operator= (
    const dpps::Pattern_Fibonacci &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

void dpps::Pattern_Fibonacci::set_all_parametres (
    const bool bidimentionnal,
    const long_unsigned_int generation,
    const double x0,
    const double y0,
    const double size) {
    pattern_settings. bidimentionnal = bidimentionnal ;
    pattern_settings. generation = generation ;
    pattern_settings. x0 = x0 ;
    pattern_settings. y0 = y0 ;
    pattern_settings. size = size ;
}

void dpps::Pattern_Fibonacci::set_parametres (
                                     const std::vector<bool> &vbool,
                                     const std::vector<long_unsigned_int> &vint,
                                     const std::vector<double> &vdouble,
                                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 1)   ||
        (vint. size () != 1)    ||
        (vdouble. size () != 3) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_Fibonacci::set_parametres",
            1, 2, 3, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    set_all_parametres (vbool[0], vint[0], vdouble[0], vdouble[1], vdouble[2]) ;
}

void dpps::Pattern_Fibonacci::generate () {
    if (pattern_settings. bidimentionnal)
        generate_2d () ;
    else
        generate_1d () ;
}

void dpps::Pattern_Fibonacci::generate_Fibonacci_sequence (
    long_unsigned_int generation, std::vector<char> &result) {
// Note that vector<bool> is a C++11 specialization of vector<> with
// few differences to allow working on individual bits while packing them by
// byte in memory (occupying 8 times less memory). It is much slower to
// use so we decided for a vector<char>.
// To give an example, at iteration 26 there are 196419 elements.
// Generation takes 22 s with vector<bool> and 0.35 s with vector<char>
// (gcc-4.9.0-alpha20140126, -O9)
// See also Nicolai M. Josuttis, the C++ Standard Library 2nd Ed., section 7.3.6
// p. 281, Addison-Wesley.
//
// Counter-intuitively, we measure that reserve () incrases slightly the process
// time. At generation 30 with 1 346 270 elements, program takes
// 17.08 s with reserve, 16.79 without. Still we keep it as difference is small
// and believe that reserve() is somehow the good thing to do.
//
// S -> L, L -> LS | false -> true ; true  -> true,false
// C Janot. Quasicrystals, a primer, Oxford University Press,
// 2nd Ed (1994), p. 23.
// 'A'=='L' == true, 'B'=='S' == false
    result. clear () ;
    switch (generation) {
        case 0:
            break ;
        case 1:
            result. push_back (true_) ;
            break ;
        case 2:
            result. push_back (true_) ;
            result. push_back (false_) ;
            break ;
        default:
            // At gen. 30 there are 1'346'270 elements, and we know the increase
            // rate is the golen ratio.
            long_unsigned_int prevision {static_cast<long_unsigned_int> (
                round (1346270.0 * pow (golden_ratio, 1.0 *
                                        generation - 30.0)))} ;
            // std::cout << prevision << std::endl ;
            result. reserve (prevision) ;
            result. push_back (true_) ;
            result. push_back (false_) ;
            for (long_unsigned_int j = 0 ; j < generation ; j++) {
                for (long_unsigned_int i = result. size () - 1 ; i > 0 ; i--) {
                    if (result [i] == true_) {
                        // L -> LS: we insert S (false_) after position i.
                        // NOTE: gcc allows cbegin() here, but clang does not.
                        result. insert (result. begin () + i + 1, false_) ;
                    }
                    else // S -> L
                        result [i] = true_ ;
                }
            }
            break ;
    }
}

void dpps::Pattern_Fibonacci::generate_1d () {
    Vertex v ;
    Polyline p ;
    p. closed = true ;
    std::vector<char> fibonacci_sequence ;
    generate_Fibonacci_sequence (pattern_settings. generation,
                                 fibonacci_sequence) ;
    long_unsigned_int number {fibonacci_sequence. size ()} ;
    double half_size {(0.5*number) * pattern_settings. size} ;
    double minx {pattern_settings. x0 - half_size} ;
    double miny {pattern_settings. y0 - half_size} ;
    double maxy {pattern_settings. y0 + half_size} ;
    double startx {minx} ;
    //std::cout << "number : " << pattern_settings. generation << " " << number << std::endl ;
    for (long_unsigned_int i = 0 ; i < number ; i++) {
        // https://en.wikipedia.org/wiki/Truth_table
        // https://en.wikipedia.org/wiki/Logical_biconditional
        // std::cout << i << " " ;
        if (fibonacci_sequence. at (i) == true_)  {
            //std::cout << "*" ;
            v. x = startx ;
            v. y = miny ;
            p. push_back (v) ;
            v. y = maxy ;
            p. push_back (v) ;
            v. x = startx + pattern_settings. size * linear_ratio ;
            p. push_back (v) ;
            v. y = miny ;
            p. push_back (v) ;
            polylines. push_back (p) ;
            p. vertices. clear () ; // does not change that p. closed == true ;
        }
        startx += pattern_settings. size ;
    }
}

void dpps::Pattern_Fibonacci::generate_2d () {
    // A simple algorithm to produce the pattern published in:
    // Quasi-periodic distribution of plasmon modes in two-dimensional
    // Fibonacci arrays of metal nanoparticles.
    // Ramona Dallapiccola, Ashwin Gopinath, Francesco Stellacci, and
    // Luca Dal Negro
    // Optics Express, Vol. 16, Issue 8, pp. 5544-5555 (2008)
    // http://dx.doi.org/10.1364/OE.16.005544
    //
    // Actually we don't use their fA/fB method which turns out complex to
    // implement with 2D vectors. But we do use their figure 1b to build
    // a really simple way.
    Vertex v ;
    Polyline p ;
    p. closed = true ;
    std::vector <char> fibonacci_sequence ;
    generate_Fibonacci_sequence (pattern_settings. generation,
                                 fibonacci_sequence) ;
    long_unsigned_int number {fibonacci_sequence. size ()} ;
    double half_size {(0.5*number) * pattern_settings. size} ;
    double minx {pattern_settings. x0 - half_size} ;
    double miny {pattern_settings. y0 - half_size} ;
    double startx {minx} ;
    for (long_unsigned_int i = 0 ; i < number ; i++) {
        double starty {miny} ;
        for (long_unsigned_int j = 0 ; j < number ; j++) {
            // X XNOR Y = Biconditional = X IFF Y, NOT (X XOR Y), X == Y
            // We output a square where fi XNOR fj
            if (fibonacci_sequence [i] == fibonacci_sequence [j])  {
                v. x = startx ;
                v. y = starty ;
                p. push_back (v) ;
                v. x += pattern_settings. size * area_ratio ;
                p. push_back (v) ;
                v. y += pattern_settings. size * area_ratio ;
                p. push_back (v) ;
                v. x = startx ;
                p. push_back (v) ;
                polylines. push_back (p) ;
                p. vertices. clear () ;
            }
            starty += pattern_settings. size ;
        }
        startx += pattern_settings. size ;
    }
}