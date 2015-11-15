/**
 * @file Matrix2x2.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A really basic and incomplete 2x2 matrix class
 * @date 2014-07-21 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_MATRIX2x2
#define DPPS_MATRIX2x2

#include <array>
#include <limits>
#include <string>
#include "common.hh"


namespace dpps {
class Matrix2x2 {
protected:
    static const constexpr int length {4} ;
    std::array<double, length> coefficients ;
public:
    Matrix2x2 () ;
    Matrix2x2 (const double a,
            const double b,
            const double c,
            const double d) ;
    void display () const ;
    std::string display_string () const ;
    double get_coefficient (const short unsigned int i) const ;
    double get_coefficient (const short unsigned int i, const short unsigned j) const ;
    void set_coefficient (const short unsigned int i, const double value) ;
    void set_coefficient (const short unsigned int i, const short unsigned j, const double value) ;
    /** @brief Returns true if all elements are smaller (in absolute value)
     * than precision. */
    bool is_null (double precision = std::numeric_limits<double>::epsilon ()) const ;
    /** @brief Equality operator, to machine precision */
    bool operator== (const Matrix2x2 &comparison) const ;

    /** @brief Equality to specified precision */
    bool equals_to (const Matrix2x2 &comparison,
       double precision = std::numeric_limits<double>::epsilon ()) const ;

    /** @brief Inequality operator */
    bool operator!= (const Matrix2x2 &comparison) const ;

    /** @brief Addition operator for two Matrix2x2 instances */
    Matrix2x2 operator+ (const Matrix2x2 &add) const ;

    /** @brief Addition operator for a Matrix2x2 and a double */
    Matrix2x2 operator+ (const double add) const ;

    /** @brief Subtraction operator for two Matrix2x2 instances */
    Matrix2x2 operator- (const Matrix2x2 &subtract) const ;

    /** @brief Subtraction operator a Matrix2x2 and a double */
    Matrix2x2 operator- (const double subtract) const ;

    /** @brief Multiplication operator a Matrix2x2 and a double */
    Matrix2x2 operator* (const double multiply) const ;

    /** @brief Non-commutative matrix multiplication */
    Matrix2x2 operator* (const Matrix2x2 &product) const ;

    /** @brief Multiplication operator a Matrix2x2 and a double */
    Matrix2x2 operator/ (const double divide) const ;

    /** @brief Addition-assignment operator for a Matrix2x2 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix2x2 &operator+= (const Matrix2x2 &add) ;

    /** @brief Addition-assignment operator for two vertices. */
    Matrix2x2 &operator+= (const double &add) ;

    /** @brief Subtraction-assignment operator for a Matrix2x2 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix2x2 &operator-= (const Matrix2x2 &subtract) ;

    /** @brief Substraction-assignment operator for two vertices. */
    Matrix2x2 &operator-= (const double &subtract) ;

    /** @brief Multiplication-assignment operator for a Matrix2x2 and a double.
     * Also works for integer types due to implicit conversion. */
    Matrix2x2 &operator*= (const double &multiply) ;

    /** @brief Division-assignment operator for a Matrix2x2 and a double. Also
     * works for integer types due to implicit conversion. */
    Matrix2x2 &operator/= (const double &divide) ;

    double determinant () const ;
    Matrix2x2 inverse() const ;
    void invert() ;

    static Matrix2x2 rotation (const double angle) ;
    static Matrix2x2 diagonal (const double a, const double b) ;
    static Matrix2x2 identity () ;

} ;
} // namespace dpps
#endif