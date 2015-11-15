/**
 * @file Vertex.hh
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for classes Vertex, basic element of a Polyline
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DPPS_VERTEX
#define DPPS_VERTEX

#include "common.hh"

#include <utility> // std::pair
#include <limits>
#include <string>
#include <array>

namespace dpps {

    typedef enum {
        quadrant_centre,
        quadrant_pxpy,
        quadrant_py,
        quadrant_mxpy,
        quadrant_mx,
        quadrant_mxmy,
        quadrant_my,
        quadrant_pxmy,
        quadrant_px
    } enum_quadrant ;

    /*typedef enum {
        vertex_turn_clockwise,
        vertex_turn_collinear,
        vertex_turn_counterclockwise
    } vertex_turn ;*/
/** @brief A small class to describe a 2D vertex in a Polyline.
 *
 * All members are public, allowing for simple use. The parameters are just two
 * double, so that memory footprint of instances is kept small.
 */
class Vertex {
public:
    /** @brief The x coordinate of the Vertex */
    double x ;

    /** @brief The y coordinate of the Vertex */
    double y ;

    Vertex () ;

    /** @brief Constructor with the two coordinates as arguments. */
    Vertex (const double x0, const double y0) ;

    /** @brief Constructor with a pair of double arguments. */
    Vertex (const std::pair<double,double> &p) ;

    /** @brief Constructor with a pair of float arguments.
     *
     * Note that the vertex is defined as double internally.
     */
    Vertex (const std::pair<float,float> &p) ;

    /** @brief Equality operator, to machine precision */
    bool operator== (const Vertex &comparison) const ;

    /** @brief Equality to specified precision */
    bool equals_to (const Vertex &comparison,
       double precision = std::numeric_limits<double>::epsilon ()) const ;

    /** @brief Inequality operator */
    bool operator!= (const Vertex &comparison) const ;

    /** @brief Addition operator for two Vertex instances */
    Vertex operator+ (const Vertex &add) const ;

    /** @brief Addition operator for a Vertex and a double */
    Vertex operator+ (const double add) const ;

    /** @brief Subtraction operator for two Vertex instances */
    Vertex operator- (const Vertex &subtract) const ;

    /** @brief Subtraction operator a Vertex and a double */
    Vertex operator- (const double subtract) const ;

    /** @brief Multiplication operator a Vertex and a double */
    Vertex operator* (const double multiply) const ;

    /** @brief Scalar product with another Vertex */
    double operator* (const Vertex &product) const ;

    /** @brief Multiplication operator a Vertex and a double */
    Vertex operator/ (const double divide) const ;

    /** @brief Addition-assignment operator for a Vertex and a double.
     * Also works for integer types due to implicit conversion. */
    Vertex &operator+= (const Vertex &add) ;

    /** @brief Addition-assignment operator for two vertices. */
    Vertex &operator+= (const double &add) ;

    /** @brief Subtraction-assignment operator for a Vertex and a double.
     * Also works for integer types due to implicit conversion. */
    Vertex &operator-= (const Vertex &subtract) ;

    /** @brief Substraction-assignment operator for two vertices. */
    Vertex &operator-= (const double &subtract) ;

    /** @brief Multiplication-assignment operator for a Vertex and a double.
     * Also works for integer types due to implicit conversion. */
    Vertex &operator*= (const double &multiply) ;

    /** @brief Division-assignment operator for a Vertex and a double. Also
     * works for integer types due to implicit conversion. */
    Vertex &operator/= (const double &divide) ;

/** @brief Returns sqrt (x^2+y^2) */
    double norm2 () const ;

    /** @brief Returns x^2+y^2
     *
     * This is provided for cases where it is not necessary to actually extract
     * the root square, such as for comparison of distances.
     */
    double norm2_square () const ;

    /** @brief Displays (x;y) on standard output
     *
     * This function just really sends the result of display_string to the
     * standard output.
     */
    void display () const ;

    /** @brief Returns a string description of the vertex. */
    std::string display_string () const ;

    /** @brief Compares the current Vertex with the Vertex passed in argument
     * and replies on each quadrant the new Vertex is located with respect
     * to the current one.
     *
     * @verbatim
                      py
                 mxpy 2  pxpy
          | w       3 | 1
       ---v--- mx 4---0---8 px (0 == centre)
          |         5 | 7
               mxmy   6  pxmy
                      my
     @endverbatim
     *
     * \retval quadrant_centre
     *              both vertices are equal
     * \retval quadrant_pxpy
     *              the first quadrant, 0 to pi/2, excluding boundaries
     * \retval quadrant_py
     *              pi/2 exactly
     * \retval quadrant_mxpy
     *              the second quadrant, from pi/2 to pi, excluding boundaries
     * \retval quadrant_mx
     *              pi exactly
     * \retval quadrant_mxmy
     *              the third quadrant, from pi to 3pi/2, excluding boundaries
     * \retval quadrant_my
     *              3pi/2 exactly
     * \retval quadrant_pxmy
     *              the fourth quadrant, from 3pi/2 to 2pi, excluding boundaries
     * \retval quadrant_px
     *              0 exactly
     * */
    enum_quadrant quadrant (
        const Vertex &w,
        double precision = std::numeric_limits<double>::epsilon()) const ;

    void round_to (double precision = std::numeric_limits<double>::epsilon()) ;

    /** @brief whether the triplet (u, this, w) form a counterclockwise turn */
    //vertex_turn
    short signed int turn (const Vertex &u, const Vertex &w) const ;

    /** @brief whether segments [uv] and [wz] intersect. */
    static bool intersects (const Vertex &u, const Vertex &v,
                            const Vertex &w, const Vertex &z) ;

    /** @brief Calculates the intersction position of two segments defined
     * by four points (uv) and (wz).
     *
     * @retval -2 if segments are collinear.
     * @retval -1 if segments parallel (but not collinear).
     * @retval 0 if segments intesect.
     * @retval 1 if prolongation of segments intesect.
     *
     * If segments intersect or if they prolongation does, the resulting
     * intersection point is placed in the parameter intersection. If segments
     * are parallel or collinear, the parameter intersection is left untouched.
     *. */
    static short int intersects_at (Vertex &intersection,
                                    const Vertex &u, const Vertex &v,
                                    const Vertex &w, const Vertex &z) ;
     /** @brief angle between three vertices; two vertices passed as argument
     * and the current vertex which is considered the centre of the angle.
     *
     * v. angle (u, w) give the planar angle between points (uvw).
     *
     * Sign convention is that the valeur returned it is the argument of u
     * minus the argument of w  when v is the centre of the coordinates, which
     * is the rotation needed to transform (w-v) into (u-v), with positive
     * rotation is in the trigonometric direction.
     *
     * If u == v or w == v to the specified precision, angle returned is pi/2.
     * Rationale is that we measure angle between (w-v) and (u-v), and null
     * vector is perpendicular to all vectors.
     *
     * This function calls atan2 which might fail (set errno). However we
     * verify that we do not send atan2(0,0) so a failure should not happen.
     */
    double angle (const Vertex &u, const Vertex &w,
       double precision = std::numeric_limits<double>::epsilon ()) const ;
    void rotate (const Vertex &v, const double angle) ;

    bool is_aligned (const Vertex &v, const Vertex &w, double precision) const ;

    static short int segments_intersect (const Vertex &A, const Vertex &B, const Vertex &C, const Vertex &D) ;
} ;
} // namespace dpps

#endif
