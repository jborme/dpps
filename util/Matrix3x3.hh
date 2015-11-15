/**
 * @file Matrix3x3.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A really basic and incomplete 3x3 matrix class
 * @date 2014-07-21 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#ifndef DPPS_MATRIX3x3
#define DPPS_MATRIX3x3

#include <array>
#include <limits>
#include <string>
#include "Vertex.hh"


namespace dpps {
class Matrix3x3 {
protected:
    static const constexpr int length {9} ;
    std::array<double, length> coefficients ;
public:
    Matrix3x3 () ;
    Matrix3x3 (const double a, const double b,
            const double c, const double d,
            const double e, const double f,
            const double g, const double h, const double i) ;
    void display () const ;
    std::string display_string () const ;
    double get_coefficient (const short unsigned int i) const ;
    double get_coefficient (const short unsigned int i, const short unsigned int j) const ;
    void set_coefficient (const short unsigned int i, const double value) ;
    void set_coefficient (const short unsigned int i, const short unsigned int j, const double value) ;

    /** @brief Returns true if all elements are smaller (in absolute value)
     * than precision. */
    bool is_null (double precision = std::numeric_limits<double>::epsilon ()) const ;

    /** @brief Equality operator, to machine precision */
    bool operator== (const Matrix3x3 &comparison) const ;

    /** @brief Equality to specified precision */
    bool equals_to (const Matrix3x3 &comparison,
       double precision = std::numeric_limits<double>::epsilon ()) const ;

    /** @brief Inequality operator */
    bool operator!= (const Matrix3x3 &comparison) const ;

    /** @brief Addition operator for two Matrix3x3 instances */
    Matrix3x3 operator+ (const Matrix3x3 &add) const ;

    /** @brief Addition operator for a Matrix3x3 and a double */
    Matrix3x3 operator+ (const double add) const ;

    /** @brief Subtraction operator for two Matrix3x3 instances */
    Matrix3x3 operator- (const Matrix3x3 &subtract) const ;

    /** @brief Subtraction operator a Matrix3x3 and a double */
    Matrix3x3 operator- (const double subtract) const ;

    /** @brief Multiplication operator a Matrix3x3 and a double */
    Matrix3x3 operator* (const double multiply) const ;

    /** @brief Non-commutative matrix multiplication */
    Matrix3x3 operator* (const Matrix3x3 &product) const ;

    /** @brief Multiplication operator a Matrix3x3 and a double */
    Matrix3x3 operator/ (const double divide) const ;

    /** @brief Addition-assignment operator for a Matrix3x3 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix3x3 &operator+= (const Matrix3x3 &add) ;

    /** @brief Addition-assignment operator for two vertices. */
    Matrix3x3 &operator+= (const double &add) ;

    /** @brief Subtraction-assignment operator for a Matrix3x3 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix3x3 &operator-= (const Matrix3x3 &subtract) ;

    /** @brief Substraction-assignment operator for two vertices. */
    Matrix3x3 &operator-= (const double &subtract) ;

    /** @brief Multiplication-assignment operator for a Matrix3x3 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix3x3 &operator*= (const double &multiply) ;

    /** @brief Division-assignment operator for a Matrix3x3 and a double. Also
     * works for integer types due to implicit conversion. */
    Matrix3x3 &operator/= (const double &divide) ;

    double determinant () const ;
    Matrix3x3 inverse() const ;
    void invert() ;

    static Matrix3x3 rotation (const double angle) ;
    static Matrix3x3 diagonal (const double a, const double b, const double c) ;
    static Matrix3x3 identity () ;

#ifdef GSL_FOUND
/** @brief Calculates the coefficients of the homography transforming
 * the points (ai) into points (bi).
 *
 * @example
    std::array<double, 9> coefficients ;
    dpps::Vertex a0 (0,0) ;
    dpps::Vertex a1 (10,0) ;
    dpps::Vertex a2 (10,10) ;
    dpps::Vertex a3 (0,10) ;
    dpps::Vertex b0 (0,0) ;
    dpps::Vertex b1 (11,0) ;
    dpps::Vertex b2 (11,10) ;
    dpps::Vertex b3 (0,10) ;
    dpps::Vertex::get_matrix_coefficients_homography (coefficients, a0, a1, a2, a3, b0, b1, b2, b3, 0.001) ;

    In this case the transformation from a to b is actually an affine
    transformation, but we can calculate it as a homography so that we can
    easily verify it works by just looking at the coefficients.

    coefficient will value [1.1 0 0 0 1 0 0 0 1]
    which corresponds to the matrix (row major):
    [1.1 0 0
     0   1 0
     0   0 1]

     now if we calculate a0.homography_transform(coefficients), the resulting
     point should be equal to b0, and so on.

    This actually is a similarity matrix that changes the scale by a factor of
    1.1 on the x axis (and does not change anything else).

   Coefficients are calculated by a direct linear method on a problem of rank 8,
   using the linear solver of GSL library.

   Preconditioning is done following to the famous paper from Harley, by
   transforming the points so their centre is at (0,0) and their average
   distance to centre is sqrt(2).

   It seems that people usually solve the homogeneous problem of 9 equations.
   Since the rank is 8, one of the singular values of the matrix is zero.
   Because solver is approximate, certain singular values of the matrix
   nuymerically solved muight not be exactly one or exact zero as they should.
   Then one needs to decompose in singular values, set certain values to their
   known theoretical value and recompose the matrix.

   In our algorithm, we use the non-homogeneous problem of 8 equations.
   We therefore do not need to check that the singular values of the 3x3 matrix
   since we already fixed by design the additional degree of freedom that the
   solver had. (Well, that's what I think.)
 * */

static Matrix3x3 homography  (const dpps::Vertex &a0, const dpps::Vertex &a1, const dpps::Vertex &a2, const dpps::Vertex &a3,
                              const dpps::Vertex &b0, const dpps::Vertex &b1, const dpps::Vertex &b2, const dpps::Vertex &b3,
                              const double precision) ;

#endif
     /** For context see
     * * https://en.wikipedia.org/wiki/Kabsch_algorithm
     * * https://en.wikipedia.org/wiki/Procrustes_superimposition
     * * https://en.wikipedia.org/wiki/Eight-point_algorithm
     * * http://stackoverflow.com/questions/6157886/square-to-trapezoid
     * * https://www.cs.ubc.ca/grads/resources/thesis/May09/Dubrofsky_Elan.pdf
     **/

static Matrix3x3 affine (const Vertex &a0, const Vertex &a1, const Vertex &a2,
                         const Vertex &b0, const Vertex &b1, const Vertex &b2,
                         const double precision) ;
static Matrix3x3 similarity  (const Vertex &a0, const Vertex &a1,
                              const Vertex &b0, const Vertex &b1,
                              const double precision) ;
static Matrix3x3 isometry    (const Vertex &a0, const Vertex &a1,
                              const Vertex &b0, const Vertex &b1,
                              const double precision) ;
static Matrix3x3 translation (const Vertex &a0, const Vertex &b0) ;


#ifdef GSL_FOUND
Vertex apply (const Vertex &v) const ;

Vertex apply_homography (const Vertex &v) const ;
#endif

// static Matrix3x3 matrix3x3_multiply (const Matrix3x3 &a, const Matrix3x3 &b) ;
// static Matrix3x3 matrix3x3_inverse (const Matrix3x3 &a) ;
} ;
} // namespace dpps
#endif

