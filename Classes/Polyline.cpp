/**
 * @file Polyline.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation for class Polyline
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <exception>
#include <array>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <limits>
#include <errno.h>
#include <algorithm> // std::reverse

#include <iostream> // debug

#include "configure.h"
#include "common.hh"
#include "Polyline.hh"
#include "bad_math.hh"

dpps::Polyline::Polyline () {
}

dpps::Polyline::~Polyline () {
}

dpps::Polyline::Polyline (const dpps::Vertex &first) {
    vertices. push_back (first) ;
}

dpps::Polyline::Polyline (const dpps::Polyline &p):
    vertices (p. vertices),
    closed (p. closed),
    dose (p. dose),
    reference (p. reference),
    selected (p. selected) {
}

dpps::Polyline::Polyline (Polyline &&p):
    vertices (std::move(p. vertices)),
    closed (p. closed),
    dose (p. dose),
    reference (p. reference),
    selected (p. selected) { // bitset is usually one byte, no need to move
}

dpps::Polyline &dpps::Polyline::operator= (Polyline &&p) {
    if (this != &p) {
        vertices = std::move (p. vertices) ;
        closed = p. closed ;
        dose = p. dose ;
        reference = p. reference ;
        selected = p. selected ;
    }
    return *this ;
}

dpps::Polyline &dpps::Polyline::operator= (const Polyline &p) {
    vertices = p. vertices ;
    closed = p. closed ;
    dose = p. dose ;
    reference = p. reference ;
    selected = p. selected ;
    return *this ;
}

dpps::Polyline::Polyline (const long_unsigned_int number_vertices,
                    const double diametre)  {
    constexpr const double two_pi {M_PI*2.0} ;
#ifdef __GNUC__
#ifndef __clang__
    // gcc allows to calculate those, although sqrt is not
    // constexpr by the standard
    constexpr const double sqrt_3_over_4 {sqrt(3.0)/4.0} ;
    constexpr const double sqrt_2_over_4 {sqrt(2.0)/4.0} ;
#define CONSTEXPR_OK
#endif
#endif
#ifndef CONSTEXPR_OK
    // for other compilers, we provide an approximation.
    constexpr const double sqrt_3_over_4 {0.433012701} ;
    constexpr const double sqrt_2_over_4 {0.35355339} ;
#endif
    switch (number_vertices) {
        case 0:
            break ;
        case 1: // brackets are to limit scope of variables so that compiler
                // does not complain about variables crossing case: instructions
            {
                dpps::Vertex v (0.0, 0.0) ;
                vertices. push_back (v) ;
            }
            break ;
        case 2:
            {
                dpps::Vertex v1 (-diametre / 2.0, 0.0) ;
                dpps::Vertex v2 ( diametre / 2.0, 0.0) ;
                vertices. push_back (v1) ;
                vertices. push_back (v2) ;
            }
            break ;
        case 3:
            // Equilateral triangle (drawn like a Play button, pointing to
            // the right) of side « a »
            // The radius of circumscribed circle is 2/3 of the height of the
            // triangle
            // r = 2/3 · sqrt (3/2) · a
            //      <=> a = d sqrt (3) / 2, where d is the diametre
            // Vertices are (-a sqrt (3)/6, ±a/2) and (a sqrt (3)/3, 0)
            // which gives (-d/4, ±sqrt (3)/4 d) and (d/2, 0)
            {
                dpps::Vertex v1 (-diametre / 4.0, -diametre * sqrt_3_over_4) ;
                dpps::Vertex v2 (-diametre / 4.0,  diametre * sqrt_3_over_4) ;
                dpps::Vertex v3 ( diametre / 2.0,  0.0) ;
                vertices. push_back (v1) ;
                vertices. push_back (v2) ;
                vertices. push_back (v3) ;
            }
            break ;
        case 4:
            {
                double a {diametre * sqrt_2_over_4} ;
                dpps::Vertex v1 (-a, -a) ;
                dpps::Vertex v2 (-a,  a) ;
                dpps::Vertex v3 ( a,  a) ;
                dpps::Vertex v4 ( a, -a) ;
                vertices. push_back (v1) ;
                vertices. push_back (v2) ;
                vertices. push_back (v3) ;
                vertices. push_back (v4) ;
            }
            break ;
        default:
            {
                double radius {diametre / 2.0} ;
                for (long_unsigned_int i = 0 ; i < number_vertices ; i++)
                    vertices. push_back (Vertex (
                        radius * cos (two_pi * i / number_vertices),
                        radius * sin (two_pi * i / number_vertices))) ;
            }
            break ;
    }
}

dpps::Polyline::Polyline (const long_unsigned_int number_vertices,
                    const double major_axis,
                    const double minor_axis,
                    const double angle_x)  {
    constexpr const double two_pi {M_PI*2.0} ;
    switch (number_vertices) {
        case 0:
            break ;
        case 1:
            {
                dpps::Vertex v (0.0, 0.0) ;
                vertices. push_back (v) ;
            }
            break ;
        case 2:
            {
                dpps::Vertex v1 (-major_axis /* * cos (angle_x) */ / 2.0,
                                 -major_axis /* * sin (angle_x) */ / 2.0) ;
                dpps::Vertex v2 ( major_axis /* * cos (angle_x) */ / 2.0,
                                  major_axis /* * sin (angle_x) */ / 2.0) ;
                vertices. push_back (v1) ;
                vertices. push_back (v2) ;
            }
            break ;
        default:
            {
                double semi_major_axis {major_axis / 2.0} ;
                double semi_minor_axis {minor_axis / 2.0} ;
                for (long_unsigned_int i = 0 ; i < number_vertices ; i++)
                    vertices. push_back (Vertex (
                        semi_major_axis *
                            cos (two_pi * i / number_vertices /*- angle_x*/),
                        semi_minor_axis *
                            sin (two_pi * i / number_vertices /*- angle_x*/))) ;
            }
            break ;
    }
    if (fabs (angle_x) > std::numeric_limits<double>::epsilon())
        rotate (Vertex (0.0, 0.0), angle_x) ;
}

//dpps::Polyline::Polyline (const std::vector<dpps::Vertex> &v): vertices (v) {}

dpps::Polyline::Polyline (const std::vector<std::pair <double, double>> &v) {
    // the reference &p is used to avoid copy
    for (auto &p: v)
        vertices. push_back (Vertex (p)) ;
}

dpps::Polyline::Polyline (const std::vector<std::pair <float, float>> &v) {
    // the reference &p is used to avoid copy
    for (auto &p: v)
        vertices. push_back (Vertex (p)) ;
}

void dpps::Polyline::push_back (const dpps::Vertex &v) {
    vertices. push_back (v) ;
}

void dpps::Polyline::push_back (const double x, const double y) {
    vertices. push_back (Vertex (x, y)) ;
}

void dpps::Polyline::push_back_relative (const dpps::Vertex &v) {
    if (vertices. size () == 0)
        vertices. push_back (v) ;
    else {
        Vertex w = vertices. back () ;
        vertices. push_back (w+v) ;
    }
}

void dpps::Polyline::push_back_relative (const double x, const double y) {
    push_back_relative (Vertex (x, y)) ;
}

void dpps::Polyline::push_back (const dpps::Polyline& p) {
    vertices. insert (vertices. end(),
                      p. vertices. begin (),
                      p. vertices. end()) ;
}

/////////////////////////////////
// const informative functions //
/////////////////////////////////
bool dpps::Polyline::operator== (const Polyline &p) const {
    return equals_to (p) ; // with default precision, which is machine epsilon.
}

bool dpps::Polyline::equals_to (const Polyline &p,
        const double precision) const {
//     if (precision <= 0)
//         precision = std::numeric_limits<double>::epsilon() ;
    if (size () != p. size ())
        return false ;
    if (size () == 0)
        return true ;
    for (long_unsigned_int i = 0 ; i < size () ; i++)
        if (!vertices[i]. equals_to (p. vertices[i], precision))
//             (fabs (vertices[i]. x - p. vertices [i]. x) > precision) ||
//             (fabs (vertices[i]. y - p. vertices [i]. y) > precision))
            return false ;
    return true ;
}

void dpps::Polyline::display () const {
    std::cout << display_string () ;
}

std::string dpps::Polyline::display_string () const {
    std::string result ;
    bool first {true} ;
    if (selected. count () > 0)
        result += "*" + std::to_string (selected. to_ulong ()) ;
    if (reference != 0)
        result += "#" + std::to_string (reference) ;
    if (!closed)
        result += "O" ;
    result += "[" ;
    // the reference &v is used to avoid copy
    for (auto &v : vertices) {
        if (first) {
            first = false ;
        } else {
            result += "," ;
        }
        result += v. display_string () ;
    }
    result += "]" ; //<< std::endl ;
    return result ;
}

long_unsigned_int dpps::Polyline::size () const {
    return vertices. size () ;
}

double dpps::Polyline::orientation_at_vertex (long_unsigned_int n) const {
    double result ;
    if (size () <= 1) return 0.0 ;
    if (n > size () - 1)
        n = size () - 1 ;
    long_unsigned_int o {n+1} ;
    if (o >= size())
        o = 0 ;
    if (n == vertices. size() - 1) {
        result = atan2 (vertices. at (n). y - vertices. at (o). y,
                        vertices. at (n). x - vertices. at (o). x) ;
/*    std::cout << "We characterize " << vertices. at (0). display_string () << " and " << vertices. at (m). display_string () << " and we get " << result << "\n" ;

// This gave error for (0.5;0.5) and (0.5;-0.5) with result=1.5708
// so I desactivate error handling

// It seems gcc/mingw32 cross compilation does not offer this
// although it is in the standard
// (see http://en.cppreference.com/w/cpp/numeric/math/math_errhandling)
#ifndef NO_ERRHANDLING_ERRNO
        if (math_errhandling & MATH_ERRNO) {
           std::string reason {"Domain Error in atan2 with polyline: \
when trying to get the orientation at a vertex. Usually this means this \
polyline has duplicated vertices. If you need to calculate angles of polylines \
that have duplicated vertices, you should first call \
remove_consecutive_duplicated_vertices. Polyline was " + display_string ()} ;
                throw bad_math (reason. c_str ()) ;
            }
#endif
*/
    return result ;
    }
    result = atan2 (vertices. at (n+1). y - vertices. at (n). y,
                    vertices. at (n+1). x - vertices. at (n). x) ;
/*    std::cout << "We characterize " << vertices. at (m+1). display_string () << " and " << vertices. at (m). display_string () << " and we get " << result << "\n" ;
#ifndef NO_ERRHANDLING_ERRNO
    if (math_errhandling & MATH_ERRNO) {
        std::string reason {"Domain Error in atan2 with polyline: \
when trying to get the orientation at a vertex. Usually this means this \
polyline has duplicated vertices. If you need to calculate angles of polylines \
that have duplicated vertices, you should first call \
remove_consecutive_duplicated_vertices. Polyline was " + display_string ()} ;
        throw bad_math (reason. c_str ()) ;
    }
#endif
*/
    return result ;
}

dpps::Vertex dpps::Polyline::geometrical_centre () const {
    // https://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
    // see also: http://paulbourke.net/geometry/polygonmesh/
    //           http://paulbourke.net/geometry/polygonmesh/centroid.pdf
    switch (size ()) {
        case 0:
            return Vertex {0.0, 0.0} ;
        case 1:
            return vertices. front () ;
        case 2:
            return (vertices. front () + vertices. back ()) / 2.0 ;
        case 3:
            return (vertices[0] + vertices[1] + vertices[2]) / 3.0 ;
        default:
        {
            long_unsigned_int s {size ()} ;
            Polyline q {*this} ;
            q. closed = true ;
            if (q.is_self_crossing())
                q. reorder_vertices_by_angle() ;
//             std::cout << "We will calculate the geometrical centre of : " << q.display_string() << "\n" ;
            double cx {0},
                   cy {0},
                   factor {0},
                   six_area {6*q. algebraic_area ()} ;
            q. push_back (q. vertices. front ()) ;
//             std::cout << "6area=" << six_area << "\n" ;
            for (long_unsigned_int i {0} ; i < s ; i++) {
//                 std::cout << "We use vertice_i " << q. vertices[i].display_string() << "\n" ;
//                 std::cout << "We use vertice_i+1 " << q. vertices[i+1].display_string() << "\n" ;
                factor = ((q. vertices[i].x   * q. vertices[i+1].y) -
                          (q. vertices[i+1].x * q. vertices[i].y)) ;
//                 std::cout << "factor=" << factor << "\n" ;
                cx += (q. vertices[i].x + q. vertices[i+1].x) * factor ;
                cy += (q. vertices[i].y + q. vertices[i+1].y) * factor ;
//                 std::cout << "cx=" << cx << "\n" ;
//                 std::cout << "cy=" << cy << "\n" ;
            }
            cx /= six_area ;
            cy /= six_area ;
//             std::cout << "We found : " << cx << " " << cy << "\n" ;
            return Vertex {cx, cy} ;
        }
    }
}

double dpps::Polyline::minimum_x () const {
    double minx {std::numeric_limits<double>::max ()} ;
    for (auto &v : vertices)
        if (v. x < minx)
            minx = v. x ;
    return minx ;
}

double dpps::Polyline::maximum_x () const {
    double maxx {std::numeric_limits<double>::lowest ()} ;
    for (auto &v : vertices)
        if (v. x > maxx)
            maxx = v. x ;
    return maxx ;
}

double dpps::Polyline::minimum_y () const {
    double miny {std::numeric_limits<double>::max ()} ;
    for (auto &v : vertices)
        if (v. y < miny)
            miny = v. y ;
    return miny ;
}

double dpps::Polyline::maximum_y () const {
    double maxy {std::numeric_limits<double>::lowest ()} ;
    for (auto &v : vertices)
        if (v. y > maxy)
            maxy = v. y ;
    return maxy ;
}

void dpps::Polyline::limits (double &minx, double &miny,
                       double &maxx, double &maxy) const {
    if (size() == 0)
        return ;
    minx = std::numeric_limits<double>::max () ;
    maxx = std::numeric_limits<double>::lowest () ;
    miny = std::numeric_limits<double>::max () ;
    maxy = std::numeric_limits<double>::lowest () ;
    for (auto &v : vertices) {
        if (v. x < minx)
            minx = v. x ;
        if (v. x > maxx)
            maxx = v. x ;
        if (v. y < miny)
            miny = v. y ;
        if (v. y > maxy)
            maxy = v. y ;
    }
}

dpps::Vertex dpps::Polyline::lower_left () const {
    double minx {std::numeric_limits<double>::max ()} ;
    double miny {std::numeric_limits<double>::max ()} ;
    for (auto &v : vertices) {
        if (v. x < minx)
            minx = v. x ;
        if (v. y < miny)
            miny = v. y ;
    }
    return Vertex (minx, miny) ;
}

dpps::Vertex dpps::Polyline::upper_right () const {
    double maxx {std::numeric_limits<double>::lowest ()} ;
    double maxy {std::numeric_limits<double>::lowest ()} ;
    for (auto &v : vertices) {
        if (v. x > maxx)
            maxx = v. x ;
        if (v. y > maxy)
            maxy = v. y ;
    }
    return Vertex (maxx, maxy) ;
}

double dpps::Polyline::horizontal_size () const {
    return maximum_x () - minimum_x () ;
}

double dpps::Polyline::vertical_size () const {
    return maximum_y () - minimum_y () ;
}

double dpps::Polyline::maximum_distance () const {
    double maxd2 {std::numeric_limits<double>::lowest ()} ;
    double d2, dx, dy ;
    for (long_unsigned_int i = 0 ; i < vertices. size () ; i++)
        for (long_unsigned_int j = 0 ; j < vertices. size () ; j++) {
            if (i != j) {
                dx = vertices. at (i). x - vertices. at (j). x ;
                dy = vertices. at (i). y - vertices. at (j). y ;
                d2 = dx*dx + dy*dy ;
                if (d2 > maxd2)
                    maxd2 = d2 ;
            }
        }
    return sqrt (maxd2) ;
}

double dpps::Polyline::minimum_distance () const {
    double mind2 {std::numeric_limits<double>::lowest ()} ;
    double d2, dx, dy ;
    for (long_unsigned_int i = 0 ; i < vertices. size () ; i++)
        for (long_unsigned_int j = 0 ; j < vertices. size () ; j++) {
            if (i != j) {
                dx = vertices. at (i). x - vertices. at (j). x ;
                dy = vertices. at (i). y - vertices. at (j). y ;
                d2 = dx*dx + dy*dy ;
                if (d2 < mind2)
                    mind2 = d2 ;
            }
        }
    return sqrt (mind2) ;
}

double dpps::Polyline::perimeter () const {
    if (vertices. size () <= 1)
        return 0.0 ;
    double s {length ()} ;
    double dx {vertices. at (0). x - vertices. at (vertices. size ()-1). x} ;
    double dy {vertices. at (0). y - vertices. at (vertices. size ()-1). y} ;
    s += sqrt (dx*dx + dy*dy) ;
    return s ;
}

double dpps::Polyline::length () const {
    if (vertices. size () <= 1)
        return 0.0 ;
    double s {0.0},
           dx, dy ;
    for (long_unsigned_int i = 0 ; i < vertices. size () - 1 ; i++) {
        dx = vertices. at (i+1). x - vertices. at (i). x ;
        dy = vertices. at (i+1). y - vertices. at (i). y ;
        s += sqrt (dx*dx + dy*dy) ;
    }
    return s ;
}

short signed int dpps::Polyline::orientation () const {
    double area {algebraic_area ()} ;
    if (fabs (area) <= std::numeric_limits<double>::epsilon())
        return 0 ;
    if (area > 0)
        return 1 ;
    return -1 ;
}

double dpps::Polyline::algebraic_length () const {
    return (length () * orientation ()) ;
}

double dpps::Polyline::algebraic_perimeter () const {
    return (perimeter () * orientation ()) ;
}

// http://www.wikihow.com/Calculate-the-Area-of-a-Polygon
double dpps::Polyline::algebraic_area () const {
    Polyline q {*this} ;
    //std::cout << "We calculate area of " << q. display_string () << "\n" ;
//    if (is_self_crossing())  {
        //q. reorder_vertices_by_angle() ;
//         std::cout << "Now in order : " << q. display_string () << "\n" ;
//    }

    if (q. vertices. size () <= 2)
        return 0.0 ;
    double s {0.0},
           t {0.0} ;
    for (long_unsigned_int i = 0 ; i < q. vertices. size () - 1 ; i++) {
        s += q. vertices. at (i). x * q. vertices. at (i+1). y ;
    }
    s += q. vertices. at (q. vertices. size () - 1). x * q. vertices. at (0). y ;
    for (long_unsigned_int i = 0 ; i < q. vertices. size () - 1 ; i++) {
        t += q. vertices. at (i). y * q. vertices. at (i+1). x ;
    }
    t += q. vertices. at (q. vertices. size () - 1). y * q. vertices. at (0). x ;
    return ((s-t)/2.0) ;
}

double dpps::Polyline::area () const {
    return fabs (algebraic_area ()) ;
}
/////////////////////////////////////////
// Functions transforming the Polyline //
/////////////////////////////////////////

dpps::Polyline dpps::Polyline::operator+ (const Vertex &v) const {
    Polyline q (*this) ;
    q. translate (v) ;
    return q ;
}

dpps::Polyline dpps::Polyline::operator- (const Vertex &v) const {
    Polyline q (*this) ;
    dpps::Vertex w {-v. x, -v. y} ;
    q. translate (w) ;
    return q ;
}

dpps::Polyline dpps::Polyline::operator* (const double m) const {
    Polyline q (*this) ;
    std::for_each (q. vertices. begin (), q. vertices. end (),
        [=] (Vertex &v) {
            v. x *= m ;
            v. y *= m ;
        }) ;
//     for (auto &i : q. vertices) {
//         i = i * m ;
//     }
    return q ;
}

dpps::Polyline dpps::Polyline::operator/ (const double m) const {
    Polyline q (*this) ;
    std::for_each (q. vertices. begin (), q. vertices. end (),
        [=] (Vertex &v) {
            v. x /= m ;
            v. y /= m ;
        }) ;
//     for (auto &i : q. vertices) {
//         i = i / m ;
//     }
    return q ;
}

void dpps::Polyline::transpose () {
    std::for_each (vertices. begin (), vertices. end (),
        [] (Vertex &v) {
            double temp = v. x ;
            v. x = v. y ;
            v. y = temp ;
        }) ;
//     for (auto &i : vertices) {
//         double temp = i. x ;
//         i. x = i. y ;
//         i. y = temp ;
//     }
}

void dpps::Polyline::translate (const dpps::Vertex &v) {
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &w) {
            w. x += v. x ;
            w. y += v. y ;
        }) ;
//     for (auto &i : vertices) {
//         i. x += v. x ;
//         i. y += v. y ;
//     }
}

void dpps::Polyline::translate (const double x, const double y) {
    translate (Vertex (x, y)) ;
}

void dpps::Polyline::scale_centred (const double fx, const double fy) {
    scale (geometrical_centre (), fx, fy) ;
}

void dpps::Polyline::scale_centred (const double f) {
    scale_centred (f, f) ;
}

void dpps::Polyline::scale (const dpps::Vertex &v, const double fx,
                            const double fy) {
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &i) {
            i. x = v. x + fx * (i. x - v. x) ;
            i. y = v. y + fy * (i. y - v. y) ;
        }) ;
//     for (auto &i : vertices) {
//         i. x = v. x + fx * (i. x - v. x) ;
//         i. y = v. y + fy * (i. y - v. y) ;
//     }
}
void dpps::Polyline::scale (const double x, const double y,
                            const double fx, const double fy) {
    scale (Vertex (x, y), fx, fy) ;
}

void dpps::Polyline::scale (const dpps::Vertex &v, const double f) {
    scale (v, f, f) ;
}

void dpps::Polyline::scale (const double x, const double y, const double f) {
    scale (Vertex (x, y), f, f) ;
}

void dpps::Polyline::rotate_centred (const double angle) {
    rotate (geometrical_centre (), angle) ;
}

void dpps::Polyline::symmetry_x (const double y0) {
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &v) {
            v. y = 2*y0 - v. y ;
        }) ;
//     for (auto &v : vertices)
//         v. y = 2*y0 - v. y ;
}

void dpps::Polyline::symmetry_y (const double x0) {
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &v) {
            v. x = 2*x0 - v. x ;
        }) ;
//     for (auto &v : vertices)
//         v. x = 2*x0 - v. x ;
}

void dpps::Polyline::symmetry_x_centred () {
    symmetry_x (geometrical_centre (). y) ;
}

void dpps::Polyline::symmetry_y_centred () {
    symmetry_y (geometrical_centre (). x) ;
}

void dpps::Polyline::rotate (const dpps::Vertex &v, const double angle) {
    double cos_angle {cos (angle)},
           sin_angle {sin (angle)} ;
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &i) {
            double x1 {i. x - v. x} ;
            double y1 {i. y - v. y} ;
            i. x = x1 * cos_angle - y1 * sin_angle ;
            i. y = x1 * sin_angle + y1 * cos_angle ;
            i. x += v. x ;
            i. y += v. y ;
        }) ;
//     for (auto &i : vertices) {
//     }
}

void dpps::Polyline::rotate (const double x, const double y,
                             const double angle) {
    rotate (Vertex (x, y), angle) ;
}

void dpps::Polyline::remove_consecutive_duplicated_vertices (
    double precision) {
//     if (precision <= 0)
//         precision = std::numeric_limits<double>::epsilon() ;
    if (size () <= 1)
        return ;
    std::vector<Vertex> result ;
    result. push_back (vertices[0]) ;
    for (long_unsigned_int i {1} ; i < vertices. size () ; i++) {
        if (!vertices[i]. equals_to (
                result. at (result. size ()-1), precision))
            result. push_back (vertices[i]) ;
    }
    vertices = std::move (result) ;
//     std::unique (vertices. begin (),
//                  vertices. end (),
//                  std::bind (&Vertex::equals_to,
//                             std::placeholders::_1,
//                             std::placeholders::_2,
//                             precision)) ;
    //if (closed)
        //std::cout << "Comparing:" << vertices. front (). display_string() << " and " << vertices. back (). display_string() << "\n";
    if (closed &&
        (vertices. front (). equals_to (vertices. back (), precision)))
        vertices. pop_back () ;
}

// Note that this function is almost duplicated in Pattern
void dpps::Polyline::remove_duplicated_vertices (double precision) {
//     if (precision <= 0)
//         precision = std::numeric_limits<double>::epsilon() ;
    std::vector<dpps::Vertex> result ;
    result. push_back (vertices. at (0)) ;
    for (long unsigned i = 1 ; i < vertices. size () ; i++) {
        bool duplicated {false} ;
        for (long unsigned j = 0 ; j < result. size () ; j++) {
            if (vertices[i]. equals_to (result[j], precision)) {
                duplicated = true ;
                break ;
            }
        }
        if (!duplicated)
            result. push_back (vertices[i]) ;
    }
    vertices = std::move(result) ;
}

void dpps::Polyline::replace_by (Polyline polyline, const bool keep_initial_properties) {
    dpps::Vertex centre {geometrical_centre ()} ;
    polyline. translate (centre) ;
    vertices = polyline. vertices ;
    if (!keep_initial_properties)
        set_metadata_from (polyline) ;
}

void dpps::Polyline::replace_by_regular_polygon (
    const long_unsigned_int number_vertices,
    const double diametre,
    const bool keep_initial_properties) {
    Polyline polygon (number_vertices, diametre) ;
    replace_by (polygon, keep_initial_properties) ;
    if (!keep_initial_properties)
        set_metadata_from (polygon) ;
}

void dpps::Polyline::round_to (const double precision) {
//     if (precision <= 0)
//         precision = std::numeric_limits<double>::epsilon() ;
    std::for_each (vertices. begin (), vertices. end (),
        [=] (Vertex &v) {
            v. round_to (precision) ;
        }) ;
//     for (auto &v : vertices)
//         v. round_to (precision) ;
}

void dpps::Polyline::reverse_vertices () {
    std::reverse (vertices. begin (), vertices. end ()) ;
}

void dpps::Polyline::set_dose (const double dose_value) {
    dose = dose_value ;
}

void dpps::Polyline::add_to_dose (const double additive_constant) {
    dose += additive_constant ;
}

void dpps::Polyline::multiply_dose (const double multiplication_factor) {
    dose *= multiplication_factor ;
}

void dpps::Polyline::set_reference (const long_unsigned_int reference_value) {
    reference = reference_value ;
}

void dpps::Polyline::add_to_reference (const long_unsigned_int additive_constant) {
    reference += additive_constant ;
}

void dpps::Polyline::subtract_to_reference (const long_unsigned_int additive_constant) {
    if (additive_constant >= reference)
        reference = 0 ;
    else
        reference -= additive_constant ;
}

void dpps::Polyline::set_closure (const bool closure_value) {
    closed = closure_value ;
}

void dpps::Polyline::toggle_open_close () {
    closed = !closed ;
}

void dpps::Polyline::erase () {
    vertices. clear () ;
}

void dpps::Polyline::erase (const long_unsigned_int start, long_unsigned_int end) {
    if (start >= vertices. size ())
        return ;
    if (end >= vertices. size ())
        vertices. erase (vertices. begin ()+start, vertices. end()) ;
    else
        vertices. erase (vertices. begin ()+start, vertices. begin()+end) ;
}

void dpps::Polyline::select (const bool status, const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    selected[selection] = status ;
}

void dpps::Polyline::select_all (const bool status) {
    if (status)
        for (int i{0} ; i < polyline_max_selection ; i++)
            selected[i] = 1 ;
    else
        selected = 0 ;
}

void dpps::Polyline::toggle_all () {
    for (int i{0} ; i < polyline_max_selection ; i++)
        selected[i] = !selected[i] ;
}

void dpps::Polyline::toggle (const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    selected[selection] = ! selected[selection] ;
}

bool dpps::Polyline::get_select (const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    return selected [selection] ;
}

void dpps::Polyline::set_metadata_from (const Polyline &p) {
    dose = p. dose ;
    reference = p. reference ;
    closed = p. closed ;
    selected = p. selected ;
}

double dpps::Polyline::get_double_property (
                const enum_double_property property,
                const long_unsigned_int parametre) const {
    switch (property) {
        case double_property_minimum_x: // 0
            return minimum_x () ;
        case double_property_maximum_x: // 1
            return maximum_x () ;
        case double_property_minimum_y: // 2
            return minimum_y () ;
        case double_property_maximum_y: // 3
            return maximum_y () ;
        case double_property_geometrical_centre_x: // 4
            return geometrical_centre (). x ;
        case double_property_geometrical_centre_y: // 5
            return geometrical_centre (). y ;
        case double_property_distance_to_centre: // 6
            return geometrical_centre (). norm2() ;
        case double_property_horizontal_size: // 7
            return horizontal_size () ;
        case double_property_vertical_size: // 8
            return vertical_size () ;
        case double_property_minimum_distance: // 9
            return minimum_distance () ;
        case double_property_maximum_distance: // 10
            return maximum_distance () ;
        case double_property_orientation_at_vertex: // 11
            // safe to pass a parametre larger than p. size ()
            return orientation_at_vertex  (parametre) ;
        case double_property_turning_angle_at_vertex: // 12
            // safe to pass a parametre larger than p. size ()
            return turning_angle_at_vertex (parametre) ;
        case double_property_area: // 13
            return area () ;
        case double_property_perimeter: // 14
            return perimeter () ;
        case double_property_dose: // 15
            return dose ;
    }
    // not reachable
    return 0.0 ;
}

long_unsigned_int dpps::Polyline::get_integer_property (
                const enum_integer_property property,
                const long_unsigned_int parametre) const {
    switch (property) {
        case integer_property_position:
            return 0 ;
        case integer_property_number_vertices:
            return size () ;
        case integer_property_number_vertices_modulo:
            return size () % parametre ;
        case integer_property_closeness: // 1 if closed, 0 if open.
            return (closed ? 1 : 0) ;
        case integer_property_reference:
            //std::cout << "This polyline has reference " << reference << "\n";
            return reference ;
    }
    // not reachable
    return 0 ;
}

void dpps::Polyline::update_double_property (
        const enum_double_property property,
        const long_unsigned_int parametre,
        const double value) {
    double current {get_double_property (property, parametre)} ;
    Vertex g (0,0);

    switch (property) {
        case double_property_minimum_x:
        case double_property_maximum_x:
        case double_property_geometrical_centre_x:
            translate (Vertex (value - current, 0.0)) ;
            break ;
        case double_property_minimum_y:
        case double_property_maximum_y:
        case double_property_geometrical_centre_y:
            translate (0.0, value - current) ;
            break ;
        case double_property_distance_to_centre:
            if (fabs (current) < std::numeric_limits<double>::epsilon()) {
                break ; // It was on zero, We want to change distance...
            }           // I do not what to do here, let's do nothing.
            g = geometrical_centre () ;
            if (fabs (value) < std::numeric_limits<double>::epsilon()) {
                translate (-g. x, -g. y) ; // center on top of zero
                break ;
            }
            translate (g * (value/current));
            break ;
        case double_property_vertical_size:
            if (fabs (value) > std::numeric_limits<double>::epsilon())
                scale_centred (1.0, value/current) ;
            break ;
        case double_property_horizontal_size:
            if (fabs (value) > std::numeric_limits<double>::epsilon())
                scale_centred (value/current, 1.0) ;
            break ;
        case double_property_orientation_at_vertex:
            rotate_centred (value - current) ;
            break ;
        case double_property_turning_angle_at_vertex:
            rotate_centred (value - current) ;
            break ;
        case double_property_area:
            if (fabs (value) > std::numeric_limits<double>::epsilon()) {
                double ratio {sqrt (value/current)} ;
                scale_centred (ratio, ratio) ;
            }
            break ;
        case double_property_minimum_distance:
        case double_property_maximum_distance:
        case double_property_perimeter:
            if (fabs (value) > std::numeric_limits<double>::epsilon()) {
                double ratio {value/current} ;
                scale_centred (ratio, ratio) ;
            }
            break ;
        case double_property_dose:
            dose = value ;
            break ;
        //default: // Not reachable
        //    break ;
    }
}

bool dpps::Polyline::multicriteria_less_than (
    const Polyline &p,
    std::vector<dpps::enum_double_property> &sort_order,
    const bool increasing
) const {
//     std::cout << "p1 :\n" ;
//     p1. display() ;
//     std::cout << "p2 :\n" ;
//     p2. display() ;
    std::array<dpps::Polyline, 2> polys {{*this, p}} ;
    std::array<double, 2> res ;
    for (auto property : sort_order) {
        for (int i = 0 ; i < 2 ; i++) {
            switch (property) {
                case dpps::double_property_minimum_x:
                    res[i] = polys[i]. minimum_x () ;
                    break ;
                case dpps::double_property_maximum_x:
                    res[i] = polys[i]. maximum_x () ;
                    break ;
                case dpps::double_property_minimum_y:
                    res[i] = polys[i]. minimum_y () ;
                    break ;
                case dpps::double_property_maximum_y:
                    res[i] = polys[i]. maximum_y () ;
                    break ;
                case dpps::double_property_geometrical_centre_x:
                    res[i] = polys[i]. geometrical_centre (). x ;
                    break ;
                case dpps::double_property_geometrical_centre_y:
                    res[i] = polys[i]. geometrical_centre (). y ;
                    break ;
                case dpps::double_property_distance_to_centre:
                    res[i] = polys[i]. geometrical_centre (). norm2() ;
                    break ;
                case dpps::double_property_horizontal_size:
                    res[i] = polys[i]. horizontal_size () ;
                    break ;
                case dpps::double_property_vertical_size:
                    res[i] = polys[i]. vertical_size () ;
                    break ;
                case dpps::double_property_minimum_distance:
                    res[i] = polys[i]. minimum_distance () ;
                    break ;
                case dpps::double_property_maximum_distance:
                    res[i] = polys[i]. maximum_distance () ;
                    break ;
                case dpps::double_property_orientation_at_vertex:
                    res[i] = polys[i]. orientation_at_vertex (0) ;
                    break ;
                case dpps::double_property_turning_angle_at_vertex:
                    res[i] = polys[i]. turning_angle_at_vertex (0) ;
                    break ;
                case dpps::double_property_area:
                    res[i] = polys[i]. area () ;
                    break ;
                case dpps::double_property_perimeter:
                    res[i] = polys[i]. perimeter () ;
                    break ;
                case dpps::double_property_dose:
                    res[i] = polys[i]. dose ;
                //default:
                    break ;
            }
        }
        if (fabs (res[0] - res[1]) > std::numeric_limits<double>::epsilon()) {
           if (increasing)
                return (res[0] < res[1]) ;
           else
               return (res[0] > res[1]) ;
        }
    }
    return false ;
}

bool dpps::Polyline::is_convex () const {
    // http://stackoverflow.com/questions/471962/how-do-determine-if-a-polygon-is-complex-convex-nonconvex
    if (size () <= 3)
        return true ;
    long_unsigned_int s {size ()} ;
    Polyline q ;
    q. vertices = vertices ;
    q. push_back (vertices[0]) ;
    q. push_back (vertices[1]) ;
    Vertex d1 ;
    Vertex d2 ;

    double product_i ;
    double product_0 {0.0} ;
    for (long_unsigned_int i{0} ; i < s ; i++) {
        d1.x = vertices[i+1].x - vertices[i].x ;
        d1.y = vertices[i+1].y - vertices[i].y ;
        d2.x = vertices[i+2].x - vertices[i+1].x ;
        d2.y = vertices[i+2].y - vertices[i+1].y ;
        product_i = (d1.x*d2.y) - (d1.y*d2.x) ;
        if (i == 0)
            product_0 = product_i ;
        if ((fabs (product_i) > std::numeric_limits<double>::epsilon()) &&
            (product_i * product_0 < 0))
            return false ;
    }
    return true ;
}

bool dpps::Polyline::is_self_crossing () const {
    // We'll use the simple O(n^2) approach determining if segments have
    // intersection. We need to know whether segments intersect, but
    // we do not need to know where.
    // There is a simpler solution than computing the position of the
    // intersection. I found this mentioned here:
    // http://math.stackexchange.com/questions/80798/detecting-polygon-self-intersection
    // So Vertex::intersect() was implemented by comparison of determinants,
    // following http://algs4.cs.princeton.edu/91primitives/
    if (size() <= 3)
        return false ;
    Polyline q{*this} ;
    q. remove_consecutive_duplicated_vertices() ;
    if (closed)
        q. push_back (vertices. front()) ;
    long_unsigned_int s1{q.size ()-2} ;
    long_unsigned_int s2{q.size ()-1} ;
    for (long_unsigned_int i {0} ; i < s1 ; i++) {
        for (long_unsigned_int j {i+2} ; j < s2 ; j++) {
            bool intersect = Vertex::intersects (
                q.vertices[i], q.vertices[i+1], q.vertices[j], q.vertices[j+1]) ;
           if ((!q. vertices[i+1].   equals_to (q. vertices[j])) &&
                (!q. vertices[i].   equals_to (q. vertices[j+1])) &&
                (!q. vertices[i+1]. equals_to (q. vertices[j])) &&
                (!q. vertices[i+1]. equals_to (q. vertices[j+1])) &&
                intersect)
                return true ;
        }
    }
    return false ;
}

double dpps::Polyline::turning_angle_at_vertex (long_unsigned_int i) const {
    if (size () <= 2)
        return 0.0 ;
    if (i > size () - 1)
        i = size () - 1 ;
    Vertex u ;
    Vertex v ;
    Vertex w ;
    if (i == 0) {
        u = vertices. back () ;
        v = vertices[0] ;
        w = vertices[1] ;
    } else {
        if (i == size() - 1) {
            u = vertices[size()-2] ;
            v = vertices. back () ;
            w = vertices[0] ;
        } else {
            u = vertices[i-1] ;
            v = vertices[i];
            w = vertices[i+1] ;
        }
    }
    return v. angle (u, w) ;
}

void dpps::Polyline::remove_aligned_vertices (double precision) {
    if (size () < 3)
        return ;
    if (precision < 0)
        precision = std::numeric_limits<double>::epsilon() ;
    long_unsigned_int i {0} ;
    while (i < size ()) {
        //std::cout << "Now testing for vertices at i=" << i << ": " << u. display_string() << ", " << v. display_string() << ", " << w. display_string() << "\n" ;
        double a {turning_angle_at_vertex (i)} ;
        if ((fabs (a) < precision) ||
            (fabs (a - M_PI) < precision) ||
            (fabs (a + M_PI) < precision)) {
            //std::cout << "removing i=" << i << "\n" ;
            vertices. erase (vertices. begin () + i,
                             vertices. begin () + i + 1) ;
        } else
            i++ ;
    }
}

#ifdef GSL_FOUND
void dpps::Polyline::apply_matrix (Matrix3x3 &matrix) {
    for (auto &i: vertices)
        i = matrix. apply (i) ;
}

void dpps::Polyline::apply_homography (Matrix3x3 &matrix) {
    for (auto &i: vertices)
        i = matrix.apply_homography(i) ;
}
#endif

void dpps::Polyline::subdivide (const double distance) {
    if (size () <= 1)
        return ;
    if (distance < std::numeric_limits<double>::epsilon())
        return ;
    long_unsigned_int s{size()} ;
    Polyline result ;
    for (long_unsigned_int i{0} ; i < s ; i++) {
        // std::cout << "i=" << i << " " <<closed << "\n" ;
        long_unsigned_int next {i+1} ;
        if (next == s) {
            if (closed)
                next = 0 ;
            else
                break ;
        }
        // std::cout << "next=" << next <<"\n" ;
        result. push_back (vertices[i]) ;
        double local_length {(vertices[next] - vertices[i]). norm2()} ;
        long_unsigned_int number = static_cast<long_unsigned_int> (floor (
            local_length/distance)) ;
        if (number > 0) {
            double step {local_length / number} ;
            for (long_unsigned_int k{0} ; k < number ; k++)
                result. push_back (vertices[i]+(vertices[next]-vertices[i])*(k+1)*step) ;
        }
    }
    vertices = std::move (result. vertices) ;
}

void dpps::Polyline::reorder_vertices_by_angle() {
    if (vertices. size() <= 3)
        return ;
    std::vector<long_unsigned_int> v ;
    Vertex average ;
    for (long_unsigned_int i{0} ; i < vertices. size () ; i++)
        average += vertices[i] ;
    average /= vertices. size () ;
    for (long_unsigned_int i{0} ; i < vertices. size () ; i++)
        v. push_back (i) ;
    Polyline q ;
    for (long_unsigned_int i{0} ; i < vertices. size () ; i++)
        q. push_back (vertices[i]-average) ;
    Vertex v0 (0,0) ;
    Vertex v1 (1,0) ;
//     std::cout << "reorder : polyline = " << q. display_string () << "\n" ;
    std::sort(v. begin (), v. end (),
        [&](const long_unsigned_int &i, const long_unsigned_int &j) -> bool {
            double ai {v0.angle(v1, q.vertices[i])} ;
                //q.orientation_at_vertex(i)} ;
            //double aj {q.orientation_at_vertex(j)} ;
            double aj {v0.angle(v1, q.vertices[j])} ;
            bool r ;
            if (fabs (ai-aj) < std::numeric_limits<double>::epsilon()) {
                r =(q.vertices[i].norm2() < q.vertices[j].norm2()) ;
            } else
                r = (ai < aj) ;
//             std::cout << "vertex: " << i << "=" << ai << ", " << j << "=" << aj << " --> " << r << "\n" ;
            return r ;
    }) ;
    std::vector<Vertex> result ;
    for (long_unsigned_int i{0} ; i < vertices. size() ; i++)
        result. push_back (vertices[v[i]]) ;
    vertices = std::move (result) ;
}

dpps::Vertex dpps::Polyline::closest_to (const Vertex &v) const {
    if (vertices. size() == 0)
        return Vertex (0,0) ;
    return vertices[closest_to_index (v)] ;
}

long_unsigned_int dpps::Polyline::closest_to_index (const Vertex &v) const {
    if (vertices. size() == 0)
        return std::numeric_limits<long_unsigned_int>::max() ;
    Polyline p (*this - v) ;
    double min_norm {std::numeric_limits<double>::max ()} ;
    long_unsigned_int i {0} ;
    for (auto &v : p.vertices)
        if (v. norm2_square() < min_norm) {
            min_norm = v. x ;
            i++ ;
        }
    return i-1 ;
}

dpps::Vertex dpps::Polyline::get_vertex (const long_unsigned_int i) const {
    if (i < vertices. size())
        return vertices[i] ;
    else
        return Vertex (0,0) ;
}

void dpps::Polyline::set_vertex (const long_unsigned_int i, const Vertex &v) {
    if (i < vertices. size())
        vertices[i] = v ;
}

long_unsigned_int dpps::Polyline::minimum_x_index () const {
    if (vertices. size () == 0)
        return 0 ;
    double minx {std::numeric_limits<double>::max ()} ;
    long_unsigned_int i {0} ;
    for (auto &v : vertices)
        if (v. x < minx) {
            minx = v. x ;
            i++ ;
        }
    return i-1 ;
}

long_unsigned_int dpps::Polyline::maximum_x_index () const {
    if (vertices. size () == 0)
        return 0 ;
    double maxx {std::numeric_limits<double>::lowest ()} ;
    long_unsigned_int i {0} ;
    for (auto &v : vertices)
        if (v. x > maxx) {
            maxx = v. x ;
            i++ ;
        }
    return i-1 ;
}

long_unsigned_int dpps::Polyline::minimum_y_index () const {
    if (vertices. size () == 0)
        return 0 ;
    double miny {std::numeric_limits<double>::max ()} ;
    long_unsigned_int i {0} ;
    for (auto &v : vertices)
        if (v. y < miny) {
            miny = v. y ;
            i++ ;
        }
    return i-1 ;
}

long_unsigned_int dpps::Polyline::maximum_y_index () const {
    if (vertices. size () == 0)
        return 0 ;
    double maxy {std::numeric_limits<double>::lowest ()} ;
    long_unsigned_int i {0} ;
    for (auto &v : vertices)
        if (v. y > maxy) {
            maxy = v. y ;
            i++ ;
        }
    return i-1 ;
}


void dpps::Polyline::insert (Polyline p, const long_unsigned_int pos, const bool reverse) {
    auto iter = vertices. begin () ;
    if (pos >= size())
        iter = vertices. end() ;
    else
        iter += pos ;
    if (reverse)
        std::reverse (p. vertices. begin(), p. vertices. end()) ;
    vertices. insert (iter,
                      p. vertices. begin (),
                      p. vertices. end ()) ;
}

void dpps::Polyline::cut_into (Polyline p,
                               long_unsigned_int pos_this,
                               long_unsigned_int pos_p,
                               const bool reverse_auto) {
    if (pos_this > size ())
        pos_this = size() ;
    if (p. vertices. size () == 0)
        return ;
    if (size () == 0) {
        vertices = p. vertices ;
        return ;
    }
    p. cycle (pos_p, false) ;
    // We are now sure that neither size is 0.
    if ((size () == 1) || (size () == 2)) {
        if (p. vertices. size () == 1) {
            insert (p, pos_this, false) ;
            return ;
        }
        // We are now sure that p has at least 2 vertices
        // We duplicate first vertex of p
        p. vertices. push_back (p. vertices. front ()) ;
        // We duplicate vertex in *this where we want to insert and insert p
        // between the two.
        vertices. insert (vertices. begin() + pos_this, vertices. begin()+pos_this, vertices. begin()+pos_this) ;
        // and insert the target right inside.
        vertices. insert (vertices. begin() + pos_this, p. vertices. begin(), p. vertices. end()) ;
        return ;
    }
    // We are now sure that *this has at least 3 vertices.
    if ((p. size () == 1) || (p. size () == 2)) {
        vertices. insert (vertices. begin() + pos_this, p. vertices. begin(), p. vertices. end()) ;
        return ;
    }
    // We are now sure that both *this and p have at least 3 vertices.
    bool reverse {false} ;
    if (reverse_auto) {
        // We must join the dots in a way to not cross the lines.
        // We will go from
        Vertex vertex_before ;
        if (pos_this == 0)
            vertex_before = vertices. back() ;
        else
            vertex_before = vertices[pos_this-1] ;

        // std::cout << "this = " << display_string() << "\n" ;
        // std::cout << "p = " << p. display_string() << "\n" ;
        // std::cout << "uv (this " << pos_this << ", p 0) = (" << vertices[pos_this]. display_string () << ", " << p. vertices. front(). display_string() << ")\n" ;
        // std::cout << "wz (this " << pos_this-1 << ", p " << p. vertices. size()-2 << ") = (" << vertex_before. display_string () << ", " << p. vertices[p. vertices. size()-2]. display_string() << ")\n" ;
        reverse = Vertex::intersects( // tests whether [uv] intersects [wz]
            vertices[pos_this],   p. vertices. front(), // uv
            vertex_before, p. vertices[p. vertices. size()-2]) ; //wz
        // std::cout << reverse << "\n" ;
        /*
        Vertex dir1, dir2 ;
        if ((pos_this == 0) || (pos_this == size()))
            dir1 = vertices. front () - vertices. back () ;
        else
            dir1 = vertices[pos_this] - vertex_before ;
        dir2 = p. vertices[1] - p. vertices[0] ;
        reverse = (dir1*dir2 > 0) ;
        */
    }
    p. push_back (p. vertices. front()) ;
    vertices. insert (vertices. begin () + pos_this, vertices. begin () + pos_this, vertices. begin () + pos_this + 1) ;
    pos_this++ ;
    insert (p, pos_this, reverse) ;
    // std::cout << "final = " << display_string() << "\n" ;
}

void dpps::Polyline::cut_into (Polyline p, const bool reverse_auto) {
    if (p. vertices. size () == 0)
        return ;
    if (size () == 0) {
        vertices = p. vertices ;
        return ;
    }
    // We are now sure that neither size is 0.
    // We look for the coordinates of the minimum distance.
    long_unsigned_int pos_this {0} ;
    long_unsigned_int pos_p {0} ;
    double min_distance_square {std::numeric_limits<double>::max()} ;
    for (long_unsigned_int i {0} ; i < vertices. size () ; i++)
        for (long_unsigned_int j {0} ; j < p. vertices. size () ; j++) {
            double dist = (vertices.at(i)-p.vertices.at(j)).norm2_square() ;
//             std::cout << "We are at i = " << i << ", j = " << j
//             << " this = " << vertices.at(i).display_string () << " p = " << p.vertices.at(j). display_string ()
//             << " difference = " << (vertices.at(i)-p.vertices.at(j)). display_string() << " distance = " << dist << "\n" ;
            if (dist < min_distance_square) {
//                 std::cout << "We set it for i = " << i << " and j = " << j << "\n" ;
                min_distance_square = dist ;
                pos_this = i ;
                pos_p = j ;
                /*if (reverse_auto) {
                    if (pos_this == 0)
                        pos_this = 1;
                    if (pos_p == 0)
                        pos_p = 1;
                }*/
            }
        }
    // std::cout << "Smallest distance : " << min_distance_square << "\n" ;
    // std::cout << "pos_this = " << pos_this << " = " << vertices[pos_this].display_string() << "\n" ;
    // std::cout << "pos_p = " << pos_p << " = " << p.vertices[pos_p].display_string() << "\n" ;
    cut_into (p, pos_this, pos_p, reverse_auto) ;
}

void dpps::Polyline::cycle (long_unsigned_int val, const bool forward) {
    // The cycle is equivalent through modulus of vertice size.
    val = val % vertices. size () ;
    if (val == 0)
        return ;
    if (forward)
        std::rotate (vertices. rbegin (), vertices. rbegin () + val, vertices. rend ()) ;
    else
        std::rotate (vertices. begin (), vertices. begin () + val, vertices. end ()) ;
}

void dpps::Polyline::check_closed(const double precision) {
    if (vertices. size() <= 1)
        return ;
    if (vertices. front(). equals_to (vertices. back(), precision)) {
        vertices. pop_back() ;
        closed = true ;
    }
}

bool dpps::Polyline::contains (const Vertex &v, const double minx, const double miny, const double maxx, const double maxy) const {
  // Decides if the point (x, y) is outside (0) or inside (1) of a polygon
  // defined by the list of mx and my (l elements) given in order. Polygon is closed between
  // the last and the first point.
  // https://secure.wikimedia.org/wikipedia/en/wiki/Point_in_polygon#Ray_casting_algorithm
  //
  long_unsigned_int i ;
  long_unsigned_int c {0} ;
  long_unsigned_int l {size()};
  if ((v. x < minx) || (v. y < miny) || (v. x > maxx) || (v. y > maxy))
      return false ;
  // We consider the consecutive segments
  for (i = 0 ; i < l ; i++) {
    if (i == l-1) {
      c += (Vertex::segments_intersect (Vertex(vertices[i].x, vertices[i].y), Vertex(vertices[0].x, vertices[0].y), Vertex(minx, v.y), Vertex(v.x, v.y)) >= 1) ;
    }
    else  {
      c += (Vertex::segments_intersect (Vertex(vertices[i].x, vertices[i].y), Vertex(vertices[i+1].x, vertices[i+1].y), Vertex(minx, v.y), Vertex(v.x, v.y)) >= 1) ;
    }
  }
  return (bool) (c % 2) ;
}

bool dpps::Polyline::contains (const Vertex &v) const {
    double minx, miny, maxx, maxy ;
    limits (minx, miny, maxx, maxy) ;
    return contains (v, minx, miny, maxx, maxy);
}
