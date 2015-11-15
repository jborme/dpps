/**
 * @file Vertex.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation for class Vertex
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <cmath>
#include <errno.h>
#include <limits>

#include "Polyline.hh"
#include "Vertex.hh"
#include "util.hh"
#include "Matrix2x2.hh"
#include "bad_math.hh"

//////////////////
// class Vertex //
//////////////////
dpps::Vertex::Vertex (): x (0.0), y (0.0) {
}

dpps::Vertex::Vertex (const double x0, const double y0): x (x0), y (y0) {
}

dpps::Vertex::Vertex (const std::pair<double, double> &p):
        x (p. first), y (p. second) {
}

dpps::Vertex::Vertex (const std::pair<float, float> &p):
        x (p. first), y (p. second) {
}

bool dpps::Vertex::operator== (const Vertex &comparison) const {
    return equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
}

bool dpps::Vertex::equals_to (const Vertex &comparison,
                              double precision) const {
    if (precision <= 0)
        precision = std::numeric_limits<double>::epsilon() ;
    return ((fabs (x-comparison.x) <= precision) &&
            (fabs (y-comparison.y) <= precision)) ;
}

bool dpps::Vertex::operator!= (const Vertex &comparison) const {
    return !equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
//     return ((fabs (x-comparison.x) > std::numeric_limits<double>::epsilon()) ||
//             (fabs (y-comparison.y) > std::numeric_limits<double>::epsilon())) ;
}

dpps::Vertex dpps::Vertex::operator+ (const Vertex &add) const {
    return dpps::Vertex (x + add. x, y + add. y) ;
}

dpps::Vertex dpps::Vertex::operator+ (const double add) const {
    return dpps::Vertex (x + add, y + add) ;
}

dpps::Vertex dpps::Vertex::operator- (const Vertex &subtract) const {
    return dpps::Vertex (x - subtract. x, y - subtract. y) ;
}

dpps::Vertex dpps::Vertex::operator- (const double subtract) const {
    return dpps::Vertex (x - subtract, y - subtract) ;
}

dpps::Vertex dpps::Vertex::operator* (const double multiply) const {
    return dpps::Vertex (x * multiply, y * multiply) ;
}

double dpps::Vertex::operator* (const Vertex &product) const {
    return (x * product. x) + (y * product. y) ;
}

dpps::Vertex dpps::Vertex::operator/ (const double divide) const {
    return dpps::Vertex (x / divide, y / divide ) ;
}

dpps::Vertex &dpps::Vertex::operator+= (const Vertex &add) {
    this->x += add. x ;
    this->y += add. y ;
    return *this ;
}

dpps::Vertex &dpps::Vertex::operator+= (const double &add) {
    this->x += add ;
    this->y += add ;
    return *this ;
}

dpps::Vertex &dpps::Vertex::operator-= (const Vertex &subtract) {
    this->x -= subtract.x ;
    this->y -= subtract.y ;
    return *this ;
}

dpps::Vertex &dpps::Vertex::operator-= (const double &subtract) {
    this->x -= subtract ;
    this->y -= subtract ;
    return *this ;
}

dpps::Vertex &dpps::Vertex::operator*= (const double &multiply) {
    this->x *= multiply ;
    this->y *= multiply ;
    return *this ;
}

dpps::Vertex &dpps::Vertex::operator/= (const double &divide) {
    this->x /= divide;
    this->y /= divide ;
    return *this ;
}

double dpps::Vertex::norm2 () const {
    return sqrt (x*x + y*y) ;
}

double dpps::Vertex::norm2_square () const {
    return x*x + y*y ;
}

void dpps::Vertex::display() const {
    std::cout << display_string () ;
}

std::string dpps::Vertex::display_string () const {
    return "(" + normal_number (x, false) + ";" + normal_number (y, false) + ")" ;
}

dpps::enum_quadrant dpps::Vertex::quadrant (const Vertex &w,
    double precision) const {
// v is the current *this vector. w is compared to v.
//                       py
//                  mxpy 2  pxpy
//           | w       3 | 1
//        ---v--- mx 4---0---8 px
//           |         5 | 7
//                mxmy   6  pxmy
//                       my
    double diffx {w. x - x} ;
    double diffy {w. y - y} ;
    if (precision < std::numeric_limits<double>::epsilon())
        precision = std::numeric_limits<double>::epsilon() ;
    if (diffx > precision && diffy > precision )
        return quadrant_pxpy ;
    if (diffx < -precision  && diffy > precision)
        return quadrant_mxpy ;
    if (diffx < -precision && diffy < -precision)
        return quadrant_mxmy ;
    if (diffx > precision && diffy < -precision)
        return quadrant_pxmy ;
    if (fabs (diffx) <= precision && diffy > precision)
        return quadrant_py ;
    if (fabs (diffx) <= precision && diffy < -precision)
        return quadrant_my ;
    if (diffx < -precision && fabs (diffy) <= precision)
        return quadrant_mx ;
    if (diffx > precision && fabs (diffy) <= precision)
        return quadrant_px ;
    return quadrant_centre ;
}

void dpps::Vertex::round_to (double precision) {
    if (precision <= std::numeric_limits<double>::epsilon())
        // nothing to do if precision is machine precision, or less, or negative
        return ;
    x = round (x / precision) * precision ;
    y = round (y / precision) * precision ;
}


short signed int dpps::Vertex::turn (const Vertex &u, const Vertex &w) const {
    // http://algs4.cs.princeton.edu/91primitives/
    // For three points a, b, c :
    //      |ux uy 1|
    //  det |vx vy 1| = 2*signed_area_triangle(u,v,w)
    //      |wx wy 1|
    //  positive is counterclockwise (trigonometric direction), etc.
    // we have v = this

    double criterion {(x - u. x) * (w. y - u. y) - (w. x - u. x) * (y - u. y)} ;
//     std::cout << "We test for points " << display_string () << "->" << u.display_string() << "->" << w.display_string() << ": " << criterion << "\n" ;
    if (criterion > std::numeric_limits<double>::epsilon())
        return 1 ;//vertex_turn_counterclockwise ;
    if (criterion < -std::numeric_limits<double>::epsilon())
        return -1 ; //vertex_turn_clockwise ;
    return 0 ; //vertex_turn_collinear ;
}

bool dpps::Vertex::intersects (const Vertex &u, const Vertex &v,
                       const Vertex &w, const Vertex &z) {
    // http://algs4.cs.princeton.edu/91primitives/
    short signed int uvw {v. turn (u, w)} ;
    short signed int uvz {v. turn (u, z)} ;

    short signed int uzw {z. turn (w, u)} ;
    short signed int wzv {z. turn (w, v)} ;

//     bool result {true};
    if (uvw * uvz > 0)
        return false ; // result = false ; //
    if (uzw * wzv > 0)
        return false ; //result = false ; //
    return true ;
//     result = true ;
//     std::cout << "Intersects : (" << u.display_string() << " <-> " << v.display_string()
//                 << ") AND (" << w.display_string() << " <-> " << z.display_string() << ") : " << result << "\n";

//     return result ;
}

short int dpps::Vertex::intersects_at (Vertex &intersection,
                                    const Vertex &u, const Vertex &v,
                                    const Vertex &w, const Vertex &z) {
    // http://algs4.cs.princeton.edu/91primitives/
    double r {(u.y-w.y)*(z.x-w.x)-(u.x-w.x)*(z.y-w.y)} ;
    double s {(u.y-w.y)*(v.x-u.x)-(u.x-w.x)*(v.y-u.y)} ;
    double denom {(v.x-u.x)*(z.y-w.y)-(v.y-u.y)*(z.x-w.x)} ;
    if (fabs(denom) < std::numeric_limits<double>::epsilon()) {
        if (fabs(r) < std::numeric_limits<double>::epsilon())
            return -2 ; // collinear
        else
            return -1 ; // parallel
    }
    intersection.x = u.x + (v.x-u.x)*r ;
    intersection.y = u.y + (v.y-u.y)*r ;
    if ((r <= 0) && (r <= 1) && (s <= 0) && (s <= 1))
        return 0 ;
    return 1 ;
}

double dpps::Vertex::angle (const Vertex &u, const Vertex &w,
                            double precision) const {
    if (precision < 0)
        precision = std::numeric_limits<double>::epsilon () ;
    if (equals_to (u, precision) || equals_to (w, precision))
       return (M_PI/2.0) ;
    return atan2 (u. y - y, u. x - x) - atan2 (w. y - y, w. x - x) ;
}

void dpps::Vertex::rotate (const dpps::Vertex &v, const double angle) {
    double cos_angle {cos (angle)},
           sin_angle {sin (angle)} ;
    double x1 {x - v. x} ;
    double y1 {y - v. y} ;
    x = x1 * cos_angle - y1 * sin_angle ;
    y = x1 * sin_angle + y1 * cos_angle ;
    x += v. x ;
    y += v. y ;
}

bool dpps::Vertex::is_aligned (const Vertex &v, const Vertex &w,
                               double precision) const {
    Vertex a {v-*this} ;
    Vertex b {w-*this} ;
    if (precision < 0)
        precision = std::numeric_limits<double>::epsilon () ;
    return fabs (a.x*b.y - a.y*b.x) <= precision ;
}


short int dpps::Vertex::segments_intersect (const Vertex &A, const Vertex &B, const Vertex &C, const Vertex &D) {
// http://www.faqs.org/faqs/graphics/algorithms-faq/ probleme 1.03
// they cite:
//    Joseph O'Rourke, Computational Geometry in C (Second Edition) Cambridge University Press 1998, pp. 249-51
//    David Kirk (ed.), Graphics Gems III, Academic Press 1992  pp. 199-202 "Faster Line Segment Intersection,"

// If needed to gain speed :
// - ask to provide AB so sqrt is not computed. AB can be kept in an array for all useful AB combinations.
//    (gains speed only in case of many segments parallel to x directions)
// - do not compute numer / denom, make comparisons of signs (limited gain but systematic)

// /mnt/stockage/jerome/travail/MPI/20110406-multiple-scatter/src/

double
  numer_r     = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y),
  numer_s     = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y),
  denominator = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x),
  r, s,
  AB, ABAC, ABAD ; // ||AB|| and scalar products AB.AC, AB.AD

  //printf ("   Intersect de A (%g %g) B (%g %g) C (%g %g) D (%g %g)\n", Ax*1E10, Ay*1E10, Bx*1E10, By*1E10, Cx*1E10, Cy*1E10, Dx*1E10, Dy*1E10) ;
  //printf ("   denom %g num_r %g num_s %g\n", denominator, numer_r, numer_s) ;
  if (fabs(denominator) <= std::numeric_limits<double>::epsilon()) { // In case denominator is 0, two segments are parallel.
//     printf ("   - denom == 0\n") ;
    if (fabs(numer_r) <= std::numeric_limits<double>::epsilon()) {       // If also numerator is zero, then two segments are aligned
//       printf ("   - num_r == 0\n") ;
      AB   = sqrt ((B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y)) ;          // costly but this whole if/if is not frequent
      ABAC = ((C.x-A.x)*(B.x-A.x)+(C.y-A.y)*(B.y-A.y)) ;
      ABAD = ((D.x-A.x)*(B.x-A.x)+(D.y-A.y)*(B.y-A.y)) ;
      if (((ABAC > AB) && (ABAD > AB)) || ((ABAC < 0) && (ABAD < 0))) {
//         printf ("   --> 0 !\n") ;

        return 0 ; // Both C and D are either fully after B, or fully before A --> no contact
      }
      else {
//         printf ("   --> 1 ou 2 !\n") ;
        // equality, even though we could test it in the beginning of the whole function
        if ((ABAC == AB) || (ABAD == AB) || (ABAC == 0) || (ABAD == 0)) return 2 ; // some of the points are identical
        else return 1 ;                                        // Intersection somewhere in the middle
      }
    } else {
//       printf ("   // not aligned --> 0 !\n") ;
      return 0 ; // Parallel but not aligned
    }
  }
  else {                                                       // Segments are not parallel
    r = numer_r / denominator ;
    s = numer_s / denominator ;
//     printf ("   secants r=%g s=%g || %d %d %d %d\n", r, s, r<0, r>1, s<0, s>1) ;
    if ((r < 0) || (r > 1) || (s < 0) || (s > 1)) {
//       printf ("   sécants ailleurs --> 0\n") ;
      return 0 ;
    }
    else {
      if ((r == 0) || (r == 1) || (s == 0) || (s == 1)) {
//         printf ("   sécants à une extrémité --> 2\n") ;
        return 2 ; // Intersection in extremities. Also, testable in the beginning.
      }
      else {
//         printf ("   sécants à l'intérieur --> 1\n") ;
        return 1 ;
      }
    }
  }
return 0;
}
