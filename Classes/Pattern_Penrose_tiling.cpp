/**
 * @file Pattern_Penrose_tiling.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that implements P2 and P3 Penrose tilings.
 * @date 2014-01-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * https://en.wikipedia.org/wiki/Penrose_tiling
 * https://en.wikipedia.org/wiki/Golden_triangle_%28mathematics%29
 * http://tilings.math.uni-bielefeld.de/substitution_rules/penrose_rhomb
 *
 * In particular we implemented the method explained at:
 * http://tartarus.org/simon/20110412-penrose/penrose.xhtml
 *
 * Construction of the P3 Pattern
 *
 * Suppose we have an large tile. We decompose it into two obtuse Robinson
 * triangles, whose specific orientation is given by vertex indices 1 and 2
 * (index 1 is common to both triangles).
@verbatim
     .              2
   .   .          .   .
 .       .      .       .
.         . -> 1-------- .
 .       .      .       .
   .   .          .   .
     .              2

@endverbatim

 * Now consider we have an obtuse Robinson triangle with points P1, P2 and P3
 * (P1 and P2 define the orientation). We decompose it in two
 * smaller obtuse Robinson triangles and one smaller acute Robinson triangle,
 * defined by points A and B.
@verbatim
                                             .
                                           . .
     P2                 .                 2 c.  .
   .   .     ->       B   .    ==      2   . .   . .
 .       .          .       .        .b .   ..    . a .
P1.........P3      .....A.....      .....1   1      2.....1
@endverbatim
 *
 * On the previous drawing, a and b are two obtuse Robinson triangles and c is
 * an acute Robinson, all three oriented by their vertex number 1 and 2. We now
 * calculate points A and B.
 *
 * In the initial obtuse initial Robinson triangle, the long edge measures 1 by
 * definition. The short edge measures Phi (phi-1 or 1/phi, ~ 0.61).
 * In the triangle a, long edges measures Phi. Short edge measures Phi/phi,
 * or Phi^2. Point A is therefore placed at fraction Phi^2 of vector P3P1. Now
 * in triangle b, long edge equals the short edge of triangle a, which is Phi^2.
 * Short edge of triangle a is therefore Phi^2/phi, which is Phi^3. Point B is
 * therefore placed at fraction Phi^3 of vector P2P1.
 *
 * We therefore conclude:
 * A = P3 + Phi^2 * (P1P3)
 * B = P2 + Phi^2 * (P2P1)
 *
 * The three smaller triangles with their correct orientation are:
 * - (P3, A, P2) (obtuse)
 * - (A,  B, P1) (obtuse)
 * - (A,  B, P2) (acute)
 *
 * We now consider an acute Robinson triangle. We decompose it into one obtuse
 * and one acute Robinson triangle.
@verbatim
           P2.                  .                 .  .
      .      .           .C     .         .2.  2.  c .
.P1          . ->  .            . == .    d  .   .  .
      .      .           .      .         .   .   . .
           P3.                  .             1.   1

@endverbatim
 *
 * We calculate the position of point C. In the initial acute Robinson triangle,
 * the short edge measures 1. The long edge measures phi. In the acute Robinson
 * triangle c, the long edge measures 1 and the short edge Phi. This allows to
 * place point C at fraction Phi on segment P2P1. We write:
 *
 * C = P2 + Phi^2 * (P2P1)
 *
 * The two smaller triangles are given by:
 * - (P3, C, P1) (obtuse)
 * - (P3, C, P2) (acute)
 *
 *
 */

#include <vector>

#include "common.hh"
#include "Pattern_Penrose_tiling.hh"
#include "bad_parametre.hh"

dpps::Pattern_Penrose_tiling::Pattern_Penrose_tiling (): Pattern () {
}

dpps::Pattern_Penrose_tiling::Pattern_Penrose_tiling (const Pattern_Penrose_tiling &source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_Penrose_tiling::Pattern_Penrose_tiling (Pattern_Penrose_tiling &&source):
    Pattern (source),
    pattern_settings (source. pattern_settings) {
}

dpps::Pattern_Penrose_tiling &dpps::Pattern_Penrose_tiling::operator= (
    dpps::Pattern_Penrose_tiling &&source) {
    if (this != &source) {
        Pattern::operator=(source) ;
        pattern_settings = source. pattern_settings ;
    }
    return *this ;
}

dpps::Pattern_Penrose_tiling &dpps::Pattern_Penrose_tiling::operator= (
    const dpps::Pattern_Penrose_tiling &source) {
    Pattern::operator=(source) ;
    pattern_settings = source. pattern_settings ;
    return *this ;
}

dpps::Pattern_Penrose_tiling::Pattern_Penrose_tiling (
        const long_unsigned_int generation,
        const enum_start_tile start_tile,
        const double x0,
        const double y0,
        const double size) {
    set_all_parametres (generation, start_tile, x0, y0, size) ;
    generate () ;
}

void dpps::Pattern_Penrose_tiling::set_all_parametres (
    const long_unsigned_int generation,
    enum_start_tile start_tile,
    const double x0,
    const double y0,
    const double size) {
    pattern_settings. x0 = x0 ;
    pattern_settings. y0 = y0 ;
    pattern_settings. size = size ;
    pattern_settings. generation = generation ;
    //pattern_settings. P3 = vbool[0] ;
    pattern_settings. start_tile = start_tile ;
}

void dpps::Pattern_Penrose_tiling::set_parametres (
                                     const std::vector<bool> &vbool,
                                     const std::vector<long_unsigned_int> &vint,
                                     const std::vector<double> &vdouble,
                                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)    ||
        (vint. size () != 2)    ||
        (vdouble. size () != 3) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern_Penrose_tiling::set_parametres",
            0, 2, 3, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint[1] > 3) {
        std::string reason {"Pattern_Penrose_tiling::set_parametre, \
start_tile can value 0 (one small tile), 1 (one large tile), 2 (two small \
tiles), 3 (two large tiles), but value provided is" +
         std::to_string (vint[1])} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (vint[0],
                        static_cast<enum_start_tile> (vint[1]),
                        vdouble[0], vdouble[1], vdouble[2]) ;
}

void dpps::Pattern_Penrose_tiling::generate () {
    //if (pattern_settings. P3)
        generate_P3 () ;
    //else
    //    generate_P2 () ;
}


void dpps::Pattern_Penrose_tiling::generate_P3 () {
    // Following deflation method from:
    // http://tartarus.org/simon/20110412-penrose/penrose.xhtml
    //
    // We build a pattern based on triangles. It would be possible from later
    // identify reflected tiles and join them, in a O(n^2) algorithm. but
    // actually we don't really care. If all we want is to draw inside the small
    // or the large tiles, we can just use the triangle pattern.

    // height of the obtuse Robinson triangle, where base is 1.
#ifdef __GNUC__
#ifndef __clang__
    // gcc allows to calculate those, although sqrt is not
    // constexpr by the standard
    constexpr const double h_obtuse_gnomon {tan (M_PI/5.0)/2.0} ;
    constexpr const double h_acute_gnomon  {tan (M_PI/2.5)/2.0} ;
    constexpr const double Phi {(sqrt(5.0) - 1.0)/2.0} ;
#define CONSTEXPR_OK
#endif
#endif
#ifndef CONSTEXPR_OK
    // for other compilers, we provide an approximation.
    constexpr const double h_obtuse_gnomon {0.363271264} ;
    constexpr const double h_acute_gnomon  {1.538841769} ;
    constexpr const double Phi {0.618033988} ;
#endif
    constexpr const double Phi2 {Phi * Phi} ;
    constexpr const double phi {1.0/Phi} ; // == Phi+1
    // constexpr const double Phi3 {Phi2 * Phi} ;
    // constexpr const double two_thirds {2.0 / 3.0} ;
    Vertex v ;
    Polyline p ;
    p. closed = true ;

    if (pattern_settings. generation == 0)
        return ;
    // One gnomon. By convention we'll set the obtuse to reference 1,
    // and the acute to 0 (the default). That's just to be
    // able to find them later.
    // Size is the length of the longest edge, which is the base of the obtuse.

    // The order of the points is important for the non-periodicity.
    // First point is always the one before the 1 arrow referred on the cited
    // webpage, second point is the one after the 1 arrow. And that's enough
    // to give an orientation to the figure.
    // NOTE: do not change reference before calling the function
    // that restructures triangles into tiles.

    double x0 {pattern_settings. x0} ;
    double y0 {pattern_settings. y0} ;

    if ((pattern_settings. start_tile == start_tile_large) ||
        (pattern_settings. start_tile == start_tile_two_large)) {
        p. reference = 1 ; // obtuse Robinson triangle
        v. x = x0 -0.5 * pattern_settings. size * phi ;
        v. y = y0 ;
        p. push_back (v) ;
        v. x = x0 ;
        v. y = y0+ h_obtuse_gnomon * pattern_settings. size * phi ;
        p. push_back (v) ;
        v. x = x0 + 0.5 * pattern_settings. size * phi ;
        v. y = y0 ;
        p. push_back (v) ;
        polylines. push_back (p) ;
        if (pattern_settings. start_tile == start_tile_two_large) {
            p. vertices. clear () ;
            v. x = x0 -0.5 * pattern_settings. size * phi ;
            v. y = y0 ;
            p. push_back (v) ;
            v. x = x0 ;
            v. y = y0 -h_obtuse_gnomon * pattern_settings. size * phi ;
            p. push_back (v) ;
            v. x = x0 + 0.5 * pattern_settings. size * phi ;
            v. y = y0 ;
            p. push_back (v) ;
            polylines. push_back (p) ;
        }
    } else { // start_tile_thin or start_tile_two_thin
        //std::cout << "We have 0 or 2" << std::endl ;
        p. reference = 0 ; // acute Robinson triangle
        v. x = x0 -h_acute_gnomon * pattern_settings. size * Phi ;
        v. y = y0 ;
        p. push_back (v) ;
        v. x = x0 ;
        v. y = y0 + 0.5 * Phi * pattern_settings. size ;
        p. push_back (v) ;
        v. x = x0 ;
        v. y = y0 - 0.5 * Phi * pattern_settings. size ;
        p. push_back (v) ;
        polylines. push_back (p) ;
        if (pattern_settings. start_tile == start_tile_two_thin) {
            p. vertices. clear () ;
            v. x = x0 + h_acute_gnomon * pattern_settings. size * Phi ;
            v. y = y0 ;
            p. push_back (v) ;
            v. x = x0 ;
            v. y = y0 + 0.5 * Phi * pattern_settings. size ;
            p. push_back (v) ;
            v. x = x0 ;
            v. y = y0 - 0.5 * Phi * pattern_settings. size ;
            p. push_back (v) ;
            polylines. push_back (p) ;
        }
    }
//     if (pattern_settings. generation == 1)
//         return ;
    for (long_unsigned_int gen = 1 ;
         gen < pattern_settings. generation ; gen++) {
        long_unsigned_int current_size {polylines. size ()} ;
        // here we do the most classical loop so we make sure no stange border
        // effects when we change the number of elements.
//         std::cout << "initial size is " << polylines. size ()<< std::endl ;
        for (long unsigned i = 0 ; i < current_size ; i++) {
            if (polylines[i]. reference == 1) { // obtuse Robinson triangle
                // std::cout << "obtuse" <<std::endl ;
                // We have an obtuse Robinson triangle.
                Vertex P3P1 = polylines[i]. vertices[1-1] -
                              polylines[i]. vertices[3-1] ;
                Vertex P2P1 = polylines[i]. vertices[1-1] -
                              polylines[i]. vertices[2-1] ;
                //std::cout << "norm (P2P1) =" << P2P1. norm2 () << std::endl ;
                //std::cout << "norm (P3P1) =" << P3P1. norm2 () << std::endl ;
                // std::cout << "vertex 3" ;
                //polylines[i]. vertices[3-1]. display () ;
                Vertex A = polylines[i]. vertices[3-1] + (P3P1 * Phi2) ;
                //std::cout << "vertex A = " ;
                //A. display () ;
                //std::cout << std::endl ;
                Vertex B = polylines[i]. vertices[2-1] + (P2P1 * Phi2) ;
                //std::cout << "vertex B = " ;
                //B. display () ;
                //std::cout << std::endl ;
                //Vertex test = polylines[i]. vertices[2-1]-B ;
                //std::cout << "norm test = " << test. norm2 () << std::endl ;
                //std::cout << "Phi 1 2 3 = " << Phi << " " << Phi2 << " " << Phi3 << std::endl ;

                p. vertices. clear () ;
                p. push_back (polylines[i]. vertices[3-1]) ;
                p. push_back (A) ;
                p. push_back (polylines[i]. vertices[2-1]) ;
                p. reference = 1 ;
                polylines. push_back (p) ;
                p. vertices. clear () ;
                p. push_back (A) ;
                p. push_back (B) ;
                p. push_back (polylines[i]. vertices[1-1]) ;
                // p. reference = 1 ; // obtuse ; still true, no need to do it.
                polylines. push_back (p) ;
                p. vertices. clear () ;
                p. push_back (A) ;
                p. push_back (B) ;
                p. push_back (polylines[i]. vertices[2-1]) ;
                p. reference = 0 ; // acute
                polylines. push_back (p) ;
            } else { // acute Robinson triangle
                // std::cout << "acute" <<std::endl ;
                Vertex P2P1 = polylines[i]. vertices[1-1] -
                              polylines[i]. vertices[2-1] ;
                Vertex C = polylines[i]. vertices[2-1] + (P2P1 * Phi2) ;
                p. vertices. clear () ;
                p. push_back (polylines[i]. vertices[3-1]) ;
                p. push_back (C) ;
                p. push_back (polylines[i]. vertices[1-1]) ;
                p. reference = 1 ;
                polylines. push_back (p) ;
                p. vertices. clear () ;
                p. push_back (polylines[i]. vertices[3-1]) ;
                p. push_back (C) ;
                p. push_back (polylines[i]. vertices[2-1]) ;
                p. reference = 0 ;
                polylines. push_back (p) ;
            }
        }
//         std::cout << "after gen " << gen << "size is " << polylines. size ()<< std::endl ;
        // NOTE: that gcc allows cbegin() here, but clang does not.
        polylines. erase (polylines. begin (),
                          polylines. begin () + current_size) ;
//         std::cout << "after erase, size is " << polylines. size ()<< std::endl ;
    }
}

//void dpps::Pattern_Penrose_tiling::generate_P2 () {
//}