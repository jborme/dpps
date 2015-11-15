/**
 * @file Matrix2x2.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A really basic and incomplete 2x2 matrix class
 * @date 2014-07-21 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "Matrix2x2.hh"
#include "bad_parametre.hh"


dpps::Matrix2x2::Matrix2x2 (): coefficients({{0,0,0,0}}) {}

dpps::Matrix2x2::Matrix2x2 (const double a,
        const double b,
        const double c,
        const double d): coefficients ({{a, b, c, d}}) {}


void dpps::Matrix2x2::display () const {
    std::cout << display_string () << "\n" ;
}

std::string dpps::Matrix2x2::display_string () const {
   std::ostringstream result ;
   result << "["
          << coefficients[0] << " " << coefficients[1] << "\n "
          << coefficients[2] << " " << coefficients[3] << "]" ;
    return result. str () ;
}

double dpps::Matrix2x2::get_coefficient (const short unsigned int i) const {
    if (i < length)
        return coefficients[i] ;
    else {
        std::string reason {"Matrix2x2::set_coefficient, out-of bounds, i \
must be between 0 and 3, and value provided is : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) i)} ;
        throw bad_parametre (reason.c_str()) ;
    }
    // unreachable
    return 0.0 ;
}

double dpps::Matrix2x2::get_coefficient (const short unsigned int i, const short unsigned int j) const {
    if ((i < 2) && (j < 2))
        return coefficients[2*i+j] ;
    else {
        std::string reason {"Matrix2x2::set_coefficient, out-of bounds, i and \
j must be between 0 and 2, and values provided are : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) j)} ;
        throw bad_parametre (reason.c_str()) ;
    }
    // unreachable
    return 0.0 ;
}

void dpps::Matrix2x2::set_coefficient (const short unsigned int i, const short unsigned int j, const double value) {
    if ((i < 2) && (j < 2))
        coefficients[2*i+j] = value ;
    else {
        std::string reason {"Matrix2x2::set_coefficient, out-of bounds, i and \
j must be between 0 and 2, and values provided are : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) j)} ;
        throw bad_parametre (reason.c_str()) ;
    }
}

void dpps::Matrix2x2::set_coefficient (const short unsigned int i, const double value) {
    if (i < length)
        coefficients[i] = value ;
    else {
        std::string reason {"Matrix2x2::set_coefficient, out-of bounds, i \
must be between 0 and 3, and value provided is : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) i)} ;
        throw bad_parametre (reason.c_str()) ;
    }
}

bool dpps::Matrix2x2::is_null (double precision) const {
    for (int i{0} ; i < length ; i++)
        if (fabs (coefficients[i]) > precision)
            return false ;
    return true ;
}

bool dpps::Matrix2x2::operator== (const Matrix2x2 &comparison) const {
    return equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
}

bool dpps::Matrix2x2::equals_to (const Matrix2x2 &comparison, double precision) const {
    if (precision <= 0)
        precision = std::numeric_limits<double>::epsilon() ;
    for (int i{0} ; i < length ; i++)
        if (fabs (coefficients[i]-comparison.coefficients[i]) > precision)
            return false ;
    return true ;
}

bool dpps::Matrix2x2::operator!= (const Matrix2x2 &comparison) const {
    return !equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator+ (const Matrix2x2 &add) const {
    return Matrix2x2 (coefficients[0]+add.coefficients[0], coefficients[1]+add.coefficients[1],
                      coefficients[2]+add.coefficients[2], coefficients[3]+add.coefficients[3]) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator+ (const double add) const {
    return Matrix2x2 (coefficients[0]+add, coefficients[1]+add,
                      coefficients[2]+add, coefficients[3]+add) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator- (const Matrix2x2 &subtract) const {
    return Matrix2x2 (coefficients[0]-subtract.coefficients[0], coefficients[1]-subtract.coefficients[1],
                      coefficients[2]-subtract.coefficients[2], coefficients[3]-subtract.coefficients[3]) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator- (const double subtract) const {
    return Matrix2x2 (coefficients[0]-subtract, coefficients[1]-subtract,
                      coefficients[2]-subtract, coefficients[3]-subtract) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator* (const double multiply) const {
    return Matrix2x2 (coefficients[0]*multiply, coefficients[1]*multiply,
                      coefficients[2]*multiply, coefficients[3]*multiply) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator* (const Matrix2x2 &product) const {
    return Matrix2x2(coefficients[0]*product.coefficients[0]+coefficients[1]*product.coefficients[2],
             coefficients[0]*product.coefficients[1]+coefficients[1]*product.coefficients[3],
             coefficients[2]*product.coefficients[0]+coefficients[3]*product.coefficients[2],
             coefficients[2]*product.coefficients[1]+coefficients[3]*product.coefficients[3]) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::operator/ (const double divide) const {
    return Matrix2x2 (coefficients[0]/divide, coefficients[1]/divide,
                      coefficients[2]/divide, coefficients[3]/divide) ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator+= (const Matrix2x2 &add) {
    this->coefficients[0] += add. coefficients[0] ;
    this->coefficients[1] += add. coefficients[1] ;
    this->coefficients[2] += add. coefficients[2] ;
    this->coefficients[3] += add. coefficients[3] ;
    return *this ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator+= (const double &add) {
    this->coefficients[0] += add ;
    this->coefficients[1] += add ;
    this->coefficients[2] += add ;
    this->coefficients[3] += add ;
    return *this ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator-= (const Matrix2x2 &subtract) {
    this->coefficients[0] -= subtract. coefficients[0] ;
    this->coefficients[1] -= subtract. coefficients[1] ;
    this->coefficients[2] -= subtract. coefficients[2] ;
    this->coefficients[3] -= subtract. coefficients[3] ;
    return *this ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator-= (const double &subtract) {
    this->coefficients[0] -= subtract ;
    this->coefficients[1] -= subtract ;
    this->coefficients[2] -= subtract ;
    this->coefficients[3] -= subtract ;
    return *this ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator*= (const double &multiply) {
    this->coefficients[0] *= multiply ;
    this->coefficients[1] *= multiply ;
    this->coefficients[2] *= multiply ;
    this->coefficients[3] *= multiply ;
    return *this ;
}

dpps::Matrix2x2 &dpps::Matrix2x2::operator/= (const double &divide) {
    this->coefficients[0] /= divide ;
    this->coefficients[1] /= divide ;
    this->coefficients[2] /= divide ;
    this->coefficients[3] /= divide ;
    return *this ;
}

double dpps::Matrix2x2::determinant () const {
    return coefficients[0]*coefficients[3]-coefficients[1]*coefficients[3] ;
}

dpps::Matrix2x2 dpps::Matrix2x2::inverse() const {
    double det ;
    if ((det = fabs (determinant ())) > std::numeric_limits<double>::epsilon()) {
        return Matrix2x2(coefficients[3]/det, -coefficients[1]/det,
                        -coefficients[2]/det, coefficients[0]/det) ;
    } else
        return Matrix2x2() ;
}

void dpps::Matrix2x2::invert() {
    *this = inverse() ;
}

dpps::Matrix2x2 dpps::Matrix2x2::rotation (const double angle) {
    double c {cos (angle)} ;
    double s {sin (angle)} ;
    return Matrix2x2 (c, -s, s, c) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::diagonal (const double a, const double b) {
    return Matrix2x2 (a, 0, 0, b) ;
}

dpps::Matrix2x2 dpps::Matrix2x2::identity () {
    return Matrix2x2 (1, 0, 0, 1) ;
}
