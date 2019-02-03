/**
 * @file Polyline_factory.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A class that produces polylines.
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>

#include "common.hh"
#include "Polyline_factory.hh"

/*
dpps::Polyline dpps::Polyline_factory::cut_into (Polyline outer,
                                                 const Polyline &inner) {
    outer. vertices. push_back (outer. vertices. front ()) ;
    outer. push_back (inner. vertices. front ()) ;
    Polyline inner_inverted ;
    inner_inverted. vertices. reserve (inner. vertices. size ()) ;
    for (long_unsigned_int i = inner. vertices. size () - 1; i > 0 ; i--) //{
        inner_inverted. vertices. push_back (inner. vertices[i]) ;
    // 0 was not processed because i is unsigned, so we could not
    // use i >= 0 and get out when i == -1
    inner_inverted. vertices. push_back (inner. vertices[0]) ;
    outer. vertices. insert (outer. vertices. end (),
                             inner_inverted. vertices. begin (),
                             inner_inverted. vertices. end ()) ;
    return outer ;
}
*/
dpps::Polyline dpps::Polyline_factory::polygonal_ring (
    const long_unsigned_int outer_vertices,
    const long_unsigned_int inner_vertices,
    const double outer_diameter,
    const double inner_diameter) {

    // Created using :
    // The outer circumference, then the inner one in opposite direction,
    // then repeating the first point of the inner circle, then repeating the
    // first point of the outer
    Polyline outer (outer_vertices, outer_diameter) ;
    if (inner_vertices < 2)
        return outer ;
    Polyline inner (inner_vertices, inner_diameter) ;
    if (outer_vertices < 2)
        return inner ;
    //return Polyline_factory::cut_into (outer, inner) ;
    outer. cut_into (inner, 0) ;
    return outer ;
}

dpps::Polyline dpps::Polyline_factory::polygonal_elliptic_ring (
    const long_unsigned_int outer_vertices,
    const long_unsigned_int inner_vertices,
    const double outer_major_axis,
    const double outer_minor_axis,
    const double outer_angle_x,
    const double inner_major_axis,
    const double inner_minor_axis,
    const double inner_angle_x) {

    Polyline outer (outer_vertices, outer_major_axis,
                    outer_minor_axis, outer_angle_x) ;
    if (inner_vertices < 2)
        return outer ;
    Polyline inner (inner_vertices, inner_major_axis,
                    inner_minor_axis, inner_angle_x) ;
    if (outer_vertices < 2)
        return inner ;
    //return dpps::Polyline_factory::cut_into (outer, inner) ;
    outer. cut_into (inner, 0) ;
    return outer ;
}

dpps::Polyline dpps::Polyline_factory::frame (
    const double outer_size,
    const double thickness) {
    double s {outer_size / 2.0} ;
    Polyline p ;
    p. vertices = {Vertex (-s, -s),
                   Vertex (-s, s),
                   Vertex (s, s),
                   Vertex (s, -s),
                   Vertex (-s+thickness, -s),
                   Vertex (-s+thickness, -s+thickness),
                   Vertex ( s-thickness, -s+thickness),
                   Vertex ( s-thickness,  s-thickness),
                   Vertex (-s+thickness,  s-thickness),
                   Vertex (-s+thickness, -s)} ;
    return p ;
}

dpps::Polyline dpps::Polyline_factory::frame (
    const double outer_size_x,
    const double outer_size_y,
    const double thickness) {
    double sx {outer_size_x / 2.0} ;
    double sy {outer_size_y / 2.0} ;
    Polyline p ;
    p. vertices = {Vertex (-sx, -sy),
                   Vertex (-sx, sy),
                   Vertex (sx, sy),
                   Vertex (sx, -sy),
                   Vertex (-sx+thickness, -sy),
                   Vertex (-sx+thickness, -sy+thickness),
                   Vertex ( sx-thickness, -sy+thickness),
                   Vertex ( sx-thickness,  sy-thickness),
                   Vertex (-sx+thickness,  sy-thickness),
                   Vertex (-sx+thickness, -sy)} ;
    return p ;
}

dpps::Polyline dpps::Polyline_factory::cross (
    const double outer_size,
    const double thickness) {
    Polyline p ;
    double t {thickness / 2.0} ;
    double s {outer_size / 2.0} ;
    p. vertices = {Vertex (-t, -t),
                   Vertex (-s, -t),
                   Vertex (-s , t),
                   Vertex (-t, t),
                   Vertex (-t, s),
                   Vertex (t, s),
                   Vertex (t, t),
                   Vertex (s, t),
                   Vertex (s, -t),
                   Vertex (t, -t),
                   Vertex (t, -s),
                   Vertex (-t, -s)} ;
    p. set_closure (true) ;
    return p ;
}

dpps::Polyline dpps::Polyline_factory::noncrossing_star_polygon (
    const bool self_intersecting,
    const long_unsigned_int p,
    const long_unsigned_int q,
    const double diametre,
    double ratio) {
// https://en.wikipedia.org/wiki/Star_polygon
// and document star.svg.
// Start is given by Schläfli symbol p/q (on the typical 5 star, p=5, q=2).
// We have alpha = pi*(p-2q)/p (1)
//
// In the triangle with R, R and S of length,
// 2alpha+2*(alpha/2)+2*beta = pi => beta=(pi-3alpha)/2 (2)
//
// In half of that triangle (S/2, R, V)
// sin (alpha)=S/(2R) (3)
// cos (alpha)=V/R (4)
//
// Say V=T+U (5)
//
// In the rectangle triangle given by (U, S/2, angle beta),
// U=(S/2)*tan beta (6)
//
// We want T/R
//
// (5) => T=V-U (7)
// We substitute (4) and (6) in (7)
// T = (Rcos alpha)-(S/2)*tan beta (8)
//
// We substitute (2) and (3) in (8)
//
// T = R cos alpha - R sin alpha*tan ((pi-3alpha)/2) (9)
//
// Conclusion :
//
// T/R = cos (alpha) - sin (alpha)*tan ((pi-3alpha)/2) (10)
// with alpha = pi*(p-2q)/p (1)
    // We only check that it will actually lead to a result. p=0, q=0 and
    // q == p are not going to give a polygon. However, q > p is going to work
    // just like q = q mod p. Also, we could check that q and p are coprimes
    // but this is not required, even if they are not then it leads to a
    // polygon (just it has fewer than p vertices).
    // For information, it is possible to know whether two numbers are prime
    // using the Euclidean algorithm:
    // https://en.wikipedia.org/wiki/Euclidean_algorithm
    Polyline polyline ;
    if ((p == 0) || (q == 0) || (q == p))
        return polyline ;
    if (q == 1)
        return Polyline (p, diametre) ;

    if (self_intersecting) {
        double alpha {M_PI*(p-2.0*q)/p} ;
        ratio = cos (alpha) - (sin (alpha)*tan ((M_PI-3.0*alpha)/2.0)) ;
    }
    double radius {diametre / 2.0} ;
    double radius_x_ratio {radius*ratio} ;
    double two_pi_over_2p {2.0*M_PI/(2.0*p)} ;
    for (long_unsigned_int i = 0 ; i < 2*p ; i++) {
        if (i % 2 == 0)
            polyline. vertices. push_back (
                Vertex (radius * cos (two_pi_over_2p * i),
                        radius * sin (two_pi_over_2p * i))) ;
        else
            polyline. vertices. push_back (
                Vertex (radius_x_ratio * cos (two_pi_over_2p * i),
                        radius_x_ratio * sin (two_pi_over_2p * i))) ;
    }

    return polyline ;
}
dpps::Polyline dpps::Polyline_factory::crossing_star_polygon (
    const long_unsigned_int p,
    const long_unsigned_int q,
    const double diametre) {
    Polyline polyline ;
    static constexpr const double two_pi {2.0*M_PI} ;
    // We just exclude the most incorrect cases. See note above
    // for more details.
    if ((p == 0) || (q == 0) || (q == p))
        return polyline ;
    if (q == 1)
        return Polyline (p, diametre) ;
    if (p <= 4)
        return Polyline (4, diametre) ;
    double radius {diametre / 2.0} ;
    double j {0} ;
    for (long_unsigned_int i = 0 ; i < p ; i++) {
        polyline. vertices. push_back (Vertex (
            radius * cos (two_pi * j / p),
            radius * sin (two_pi * j / p))) ;
        j += q ;
    }
    return polyline ;
}

