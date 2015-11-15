/**
 * @file Matrix3x3.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A really basic and incomplete 3x3 matrix class
 * @date 2014-07-21 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <sstream>
#include <iomanip> // needed ?
#include <cmath>

#include "Matrix3x3.hh"
#include "Matrix2x2.hh"
#include "Polyline.hh"
#include "bad_parametre.hh"

extern "C" {
#include <gsl/gsl_linalg.h>
}

#define HAVE_INLINE


dpps::Matrix3x3::Matrix3x3 (): coefficients({{0,0,0,0,0,0,0,0,0}}) {}

dpps::Matrix3x3::Matrix3x3 (const double a,
        const double b, const double c,
        const double d, const double e,
        const double f, const double g,
        const double h, const double i): coefficients ({{a, b, c, d, e, f, g, h, i}}) {}

void dpps::Matrix3x3::display () const {
    std::cout << display_string () << "\n" ;
}

std::string dpps::Matrix3x3::display_string () const {
   std::ostringstream result ;
   result << "["
          << coefficients[0] << " " << coefficients[1] << " " << coefficients[2] << "\n "
          << coefficients[3] << " " << coefficients[4] << " " << coefficients[5] << "\n "
          << coefficients[6] << " " << coefficients[7] << " " << coefficients[8] << "]" ;
    return result. str () ;
}

double dpps::Matrix3x3::get_coefficient (const short unsigned int i) const {
    if (i < length)
        return coefficients[i] ;
    else {
        std::string reason {"Matrix3x3::set_coefficient, out-of bounds, i \
must be between 0 and 8, and value provided is : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) i)} ;
        throw bad_parametre (reason.c_str()) ;
    }
    // unreachable
    return 0.0 ;
}

double dpps::Matrix3x3::get_coefficient (const short unsigned int i, const short unsigned int j) const {
    if ((i < 3) && (j < 3))
        return coefficients[3*i+j] ;
    else {
        std::string reason {"Matrix3x3::set_coefficient, out-of bounds, i and \
j must be between 0 and 3, and values provided are : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) j)} ;
        throw bad_parametre (reason.c_str()) ;
    }
    // unreachable
    return 0.0 ;
}

void dpps::Matrix3x3::set_coefficient (const short unsigned int i, const double value) {
    if (i < length)
        coefficients[i] = value ;
    else {
        std::string reason {"Matrix3x3::set_coefficient, out-of bounds, i \
must be between 0 and 8, and value provided is : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) i)} ;
        throw bad_parametre (reason.c_str()) ;
    }
}

void dpps::Matrix3x3::set_coefficient (const short unsigned int i, const short unsigned j, const double value) {
    if ((i < 3) && (j < 3))
        coefficients[3*i+j] = value ;
    else {
        std::string reason {"Matrix3x3::set_coefficient, out-of bounds, i and \
j must be between 0 and 3, and values provided are : " + std::to_string ((long_unsigned_int) i) +
", " + std::to_string ((long_unsigned_int) j)} ;
        throw bad_parametre (reason.c_str()) ;
    }
}

bool dpps::Matrix3x3::is_null (double precision) const {
    for (int i{0} ; i < length ; i++)
        if (fabs (coefficients[i]) > precision)
            return false ;
    return true ;
}

bool dpps::Matrix3x3::operator== (const Matrix3x3 &comparison) const {
    return equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
}

bool dpps::Matrix3x3::equals_to (const Matrix3x3 &comparison, double precision) const {
    if (precision <= 0)
        precision = std::numeric_limits<double>::epsilon() ;
    for (int i{0} ; i < length ; i++)
        if (fabs (coefficients[i]-comparison.coefficients[i]) > precision)
            return false ;
    return true ;
}

bool dpps::Matrix3x3::operator!= (const Matrix3x3 &comparison) const {
    return !equals_to (comparison, std::numeric_limits<double>::epsilon()) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator+ (const Matrix3x3 &add) const {
    return Matrix3x3 (coefficients[0]+add.coefficients[0], coefficients[1]+add.coefficients[1],
                      coefficients[2]+add.coefficients[2], coefficients[3]+add.coefficients[3],
                      coefficients[4]+add.coefficients[4], coefficients[5]+add.coefficients[5],
                      coefficients[6]+add.coefficients[6], coefficients[7]+add.coefficients[7],
                      coefficients[8]+add.coefficients[8]) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator+ (const double add) const {
    return Matrix3x3 (coefficients[0]+add, coefficients[1]+add, coefficients[2]+add,
                      coefficients[3]+add, coefficients[4]+add, coefficients[5]+add,
                      coefficients[6]+add, coefficients[7]+add, coefficients[8]+add) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator- (const Matrix3x3 &subtract) const {
    return Matrix3x3 (coefficients[0]-subtract.coefficients[0], coefficients[1]-subtract.coefficients[1],
                      coefficients[2]-subtract.coefficients[2], coefficients[3]-subtract.coefficients[3],
                      coefficients[4]-subtract.coefficients[4], coefficients[5]-subtract.coefficients[5],
                      coefficients[6]-subtract.coefficients[6], coefficients[7]-subtract.coefficients[7],
                      coefficients[8]-subtract.coefficients[8]) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator- (const double subtract) const {
    return Matrix3x3 (coefficients[0]-subtract, coefficients[1]-subtract, coefficients[2]-subtract,
                      coefficients[3]-subtract, coefficients[4]-subtract, coefficients[5]-subtract,
                      coefficients[6]-subtract, coefficients[7]-subtract, coefficients[8]-subtract) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator* (const double multiply) const {
    return Matrix3x3 (coefficients[0]*multiply, coefficients[1]*multiply, coefficients[2]*multiply,
                      coefficients[3]*multiply, coefficients[4]*multiply, coefficients[5]*multiply,
                      coefficients[6]*multiply, coefficients[7]*multiply, coefficients[8]*multiply) ;
}


dpps::Matrix3x3 dpps::Matrix3x3::operator* (const Matrix3x3 &product) const {
    return Matrix3x3(coefficients[0]*product.coefficients[0]+coefficients[1]*product.coefficients[3]+coefficients[2]*product.coefficients[6], coefficients[0]*product.coefficients[1]+coefficients[1]*product.coefficients[4]+coefficients[2]*product.coefficients[7], coefficients[0]*product.coefficients[2]+coefficients[1]*product.coefficients[5]+coefficients[2]*product.coefficients[8],
             coefficients[3]*product.coefficients[0]+coefficients[4]*product.coefficients[3]+coefficients[5]*product.coefficients[6], coefficients[3]*product.coefficients[1]+coefficients[4]*product.coefficients[4]+coefficients[5]*product.coefficients[7], coefficients[3]*product.coefficients[2]+coefficients[4]*product.coefficients[5]+coefficients[5]*product.coefficients[8],
             coefficients[6]*product.coefficients[0]+coefficients[7]*product.coefficients[3]+coefficients[8]*product.coefficients[6], coefficients[6]*product.coefficients[1]+coefficients[7]*product.coefficients[4]+coefficients[8]*product.coefficients[7], coefficients[6]*product.coefficients[2]+coefficients[7]*product.coefficients[5]+coefficients[8]*product.coefficients[8]) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::operator/ (const double divide) const {
    return Matrix3x3 (coefficients[0]/divide, coefficients[1]/divide, coefficients[2]/divide,
                      coefficients[3]/divide, coefficients[4]/divide, coefficients[5]/divide,
                      coefficients[6]/divide, coefficients[7]/divide, coefficients[8]/divide) ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator+= (const Matrix3x3 &add) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] += add. coefficients[i] ;
    return *this ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator+= (const double &add) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] += add ;
    return *this ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator-= (const Matrix3x3 &subtract) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] -= subtract. coefficients[i] ;
    return *this ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator-= (const double &subtract) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] -= subtract ;
    return *this ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator*= (const double &multiply) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] *= multiply ;
    return *this ;
}

dpps::Matrix3x3 &dpps::Matrix3x3::operator/= (const double &divide) {
    for (int i{0} ; i < length ; i++)
        this->coefficients[i] /= divide ;
    return *this ;
}

double dpps::Matrix3x3::determinant () const {
    return coefficients[0]*(coefficients[4]*coefficients[8]-coefficients[5]*coefficients[7])
           -coefficients[1]*(coefficients[8]*coefficients[3]-coefficients[5]*coefficients[6])
           +coefficients[2]*(coefficients[3]*coefficients[7]-coefficients[4]*coefficients[6]) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::inverse() const {
    double det ;
    if ((det = fabs (determinant ())) > std::numeric_limits<double>::epsilon()) {
        return Matrix3x3 (coefficients[4]*coefficients[8]-coefficients[5]*coefficients[7], -coefficients[1]*coefficients[8]+coefficients[2]*coefficients[7], coefficients[1]*coefficients[5]-coefficients[2]*coefficients[4],
                 -coefficients[3]*coefficients[8]+coefficients[5]*coefficients[6], coefficients[0]*coefficients[8]-coefficients[2]*coefficients[6], -coefficients[0]*coefficients[5]+coefficients[2]*coefficients[3],
                 coefficients[3]*coefficients[7]-coefficients[4]*coefficients[6], -coefficients[0]*coefficients[7]+coefficients[1]*coefficients[6], coefficients[0]*coefficients[4]-coefficients[1]*coefficients[3])/det ;
    } else
        return Matrix3x3() ;
}

void dpps::Matrix3x3::invert() {
    *this = inverse() ;
}

dpps::Matrix3x3 dpps::Matrix3x3::rotation (const double angle) {
    double c {cos (angle)} ;
    double s {sin (angle)} ;
    return Matrix3x3 (c, -s, 0,
                      s,  c, 0,
                      0,  0, 1) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::diagonal (const double a, const double b, const double c) {
    return Matrix3x3 (a, 0, 0,
                      0, b, 0,
                      0, 0, c) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::identity () {
    return Matrix3x3 (1, 0, 0,
                      0, 1, 0,
                      0, 0, 1) ;
}

/*
#define displaycoeff int k {0} ;\
for (int i = 0; i < 3 ; i++) {\
        for (int j = 0; j < 3 ; j++)\
                std::cout << coefficients[k++] << " " ;\
        std::cout << "\n" ;\
    }\
    std::cout << "\n" ;\
*/


// bool dpps::Vertex::get_matrix_coefficients_affine (std::array<double, 9> &coefficients,
//                                          const Vertex &a0, const Vertex &a1, const Vertex &a2,
//                                          const Vertex &b0, const Vertex &b1, const Vertex &b2,
//                                          const double precision) {
dpps::Matrix3x3 dpps::Matrix3x3::affine (const Vertex &a0, const Vertex &a1, const Vertex &a2,
                                         const Vertex &b0, const Vertex &b1, const Vertex &b2,
                                         const double precision) {
    if (a0. equals_to (a1, precision) || b0. equals_to (b1, precision) ||
        a0. equals_to (a2, precision) || b0. equals_to (b2, precision) ||
        a1. equals_to (a2, precision) || b1. equals_to (b2, precision) ||
        a0. is_aligned (a1, a2, precision))
        return Matrix3x3() ;
    Vertex A1 {a1-a0} ;
    Vertex B1 {b1-b0} ;

    Vertex A2 {a2-a0} ;
    Vertex B2 {b2-b0} ;
    double normA1 {A1. norm2()} ;
    double normB1 {B1. norm2()} ;
    double normA2 {A2. norm2()} ;
    double normB2 {B2. norm2()} ;
    double l1 {normB1/normA1} ;
    double l2 {normB2/normA2} ;

    A1 = A1 / normA1 ;
    A2 = A2 / normA2 ;
    B1 = B1 / normB1 ;
    B2 = B2 / normB2 ;
    double costheta {A1.x*B1.x + A1.y*B1.y} ;
    double cosphi   {A2.x*B2.x + A2.y*B2.y} ;
    double sintheta {sqrt(1.0-costheta*costheta)} ;
    double sinphi   {sqrt(1.0-cosphi*cosphi)} ;

//     std::array<double, 4> D {{l1, 0, 0, l2}} ;
//     std::array<double, 4> Rtheta {{costheta, -sintheta, sintheta, costheta}} ;
//     std::array<double, 4> Rphi {{cosphi, -sinphi, sinphi, cosphi}} ;
//     std::array<double, 4> Rminusphi {{cosphi, sinphi, -sinphi, cosphi}} ;
//     std::array<double, 4> A = matrix2x2_multiply(Rtheta, matrix2x2_multiply(Rminusphi, matrix2x2_multiply(D, Rphi))) ;
    Matrix2x2 D (l1, 0, 0, l2) ;
    Matrix2x2 Rtheta (costheta, -sintheta, sintheta, costheta) ;
    Matrix2x2 Rphi (cosphi, -sinphi, sinphi, cosphi) ;
    Matrix2x2 Rminusphi (cosphi, sinphi, -sinphi, cosphi) ;
    Matrix2x2 A = Rtheta*(Rminusphi*(D*Rphi)) ;

    return Matrix3x3 (A.get_coefficient(0), A.get_coefficient(1), b0. x - a0. x,
                      A.get_coefficient(2), A.get_coefficient(3), b0. y - a0. y,
                      0,    0,   1) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::similarity (
                                         const Vertex &a0, const Vertex &a1,
                                         const Vertex &b0, const Vertex &b1,
                                         const double precision) {
    if (a0. equals_to (a1, precision) || b0. equals_to (b1, precision))
        return Matrix3x3 () ;
    Vertex a {a1-a0} ;
    Vertex b {b1-b0} ;
    double norma {a. norm2()} ;
    double normb {b. norm2()} ;
    double l {normb/norma} ;
    a = a / norma ;
    b = b / normb ;
    double c {a.x*b.x + a.y*b.y} ;
    double s {sqrt(1.0-c*c)} ;
    return Matrix3x3 (l*c, -l*s, b0. x - a0. x,
                     l*s,  l*c, b0. y - a0. y,
                     0,    0,   1) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::isometry (const Vertex &a0, const Vertex &a1,
                                         const Vertex &b0, const Vertex &b1,
                                         const double precision) {
    if (a0. equals_to (a1, precision) || b0. equals_to (b1, precision))
        return Matrix3x3() ;

    Vertex a {a1-a0} ;
    Vertex b {b1-b0} ;
    a = a / a. norm2() ;
    b = b / b. norm2() ;
    double c {a.x*b.x + a.y*b.y} ;
    double s {sqrt(1.0-c*c)} ;
    return Matrix3x3 (c, -s, b0. x - c*a0. x + s*a0. y,
                     s,  c, b0. y - c*a0. y - s*a0. x,
                     0, 0, 1) ;
}

dpps::Matrix3x3 dpps::Matrix3x3::translation (
                                          const Vertex &a0, const Vertex &b0) {
    return Matrix3x3 (1, 0, b0. x - a0. x,
                     0, 1, b0. y - a0. y,
                     0, 0, 1) ;
}

#ifdef GSL_FOUND

//macros used only for debug. Can be commented
#define SQUARE_MATRIX_DISPLAY(A,N) std::cout << "[" ; \
for (int i=0; i < N ; i++) {\
    for (int j=0; j < N ; j++) \
        std::cout << gsl_matrix_get (A, i, j) << " " ;\
    if (i == N-1)\
        std::cout << "]\n" ;\
    else \
        std::cout << "\n" ;\
    }

#define VECTOR_DISPLAY(A,N) std::cout << "[" ; \
for (int i=0; i < N ; i++) \
    std::cout << gsl_vector_get (A, i) << " " ;\
std::cout << "]\n" ;


dpps::Matrix3x3 dpps::Matrix3x3::homography (const Vertex &a0, const Vertex &a1, const Vertex &a2, const Vertex &a3,
                                         const Vertex &b0, const Vertex &b1, const Vertex &b2, const Vertex &b3,
                                         const double precision) {
    if (a0. equals_to (a1, precision) || b0. equals_to (b1, precision) ||
        a0. equals_to (a2, precision) || b0. equals_to (b2, precision) ||
        a0. equals_to (a3, precision) || b0. equals_to (b3, precision) ||
        a1. equals_to (a2, precision) || b1. equals_to (b2, precision) ||
        a1. equals_to (a3, precision) || b1. equals_to (b3, precision) ||
        a2. equals_to (a3, precision) || b2. equals_to (b3, precision) ||
        a0. is_aligned (a1, a2, precision) ||
        a0. is_aligned (a1, a3, precision) ||
        a0. is_aligned (a2, a3, precision) ||
        a1. is_aligned (a2, a3, precision)) {

        return Matrix3x3 () ;
    }
    std::array<Vertex, 8> eight_point {{a0, a1, a2, a3, b0, b1, b2, b3}} ;

/*
 *               X                     H        U
 * |x0 y0 1  0  0 0 -x0.u0 -y0.u0|   |h0|     |u0 h8|
 * | 0  0 0 x0 y0 1 -x0.v0 -y0.v0|   |h1|     |v0 h8|
 * |.1..1.............1..1...1..1|   |h2|     |.1...|
 * |.........1..1.....1..1...1..1| . |h3|  =  |.1...|
 * |.2..2.............2..2...2..2|   |h4|     |.2...|
 * |.........2..2.....2..2...2..2|   |h5|     |.2...|
 * |.3..3.............3..3...3..3|   |h6|     |.3...|
 * |.........3..3.....3..3...3..3|   |h7|     |.3...|
*/
#ifdef __GNUC__
#ifndef __clang__
    // gcc allows to calculate those, although sqrt is not
    // constexpr by the standard
    constexpr const double sqrt_2 {sqrt(2.0)} ;
#define CONSTEXPR_OK
#endif
#endif
#ifndef CONSTEXPR_OK
    // for other compilers, we provide an approximation.
    constexpr const double sqrt_2 {1.414213562} ;
#endif

    // Normalization proposed by Hartley:
    // a and b vectors must be centred around zero with average distance to
    // centre sqrt(2).

    Polyline a ;
    a. push_back (eight_point[0]) ;
    a. push_back (eight_point[1]) ;
    a. push_back (eight_point[2]) ;
    a. push_back (eight_point[3]) ;
// std::cout << "a:\n" ;
// a. display() ; std::cout << "\n" ;
    Vertex centre_a {a. geometrical_centre()} ;
// std::cout << "centre_a:\n" ;
// centre_a. display() ; std::cout << "\n" ;
    double average_a {((a.vertices[0]-centre_a).norm2()+(a.vertices[1]-centre_a).norm2()+
                     (a.vertices[2]-centre_a).norm2()+(a.vertices[3]-centre_a).norm2())/4.0} ;
    double lambda_a {sqrt_2 / average_a} ;
// std::cout << "average_a: " << average_a << " lambda_a: " << lambda_a << "\n" ;
    Matrix3x3 normalization_a (lambda_a, 0, -lambda_a*centre_a.x,
                               0, lambda_a, -lambda_a*centre_a.y,
                               0, 0, 1) ;

// std::cout << "normalization_a:\n" ;
// normalization_a. display() ;
    a. apply_matrix(normalization_a) ;
// std::cout << "normalized sequence of points a:\n" ;
// a. display () ; std::cout << "\n" ;
    Polyline b ;
    b. push_back (eight_point[4]) ;
    b. push_back (eight_point[5]) ;
    b. push_back (eight_point[6]) ;
    b. push_back (eight_point[7]) ;
// std::cout << "b:\n" ;
// b. display() ; std::cout << "\n" ;
    Vertex centre_b {b. geometrical_centre()} ;
// std::cout << "centre_b:\n" ;
// centre_b. display() ; std::cout << "\n" ;
    double average_b {((b.vertices[0]-centre_b).norm2()+(b.vertices[1]-centre_b).norm2()+
                     (b.vertices[2]-centre_b).norm2()+(b.vertices[3]-centre_b).norm2())/4.0} ;
    double lambda_b {sqrt_2 / average_b} ;
// std::cout << "average_b: " << average_b << " lambda_b: " << lambda_b << "\n" ;
    Matrix3x3 normalization_b (lambda_b, 0, -lambda_b*centre_b.x,
                               0, lambda_b, -lambda_b*centre_b.y,
                               0, 0, 1) ;
// std::cout << "normalization_b:\n" ;
// normalization_b. display () ;
    b. apply_matrix(normalization_b) ;
// std::cout << "normalized sequence of points b:\n" ;
// b. display () ; std::cout << "\n" ;

// Beginning of the GSL section
///////////////////////////////
    gsl_matrix *X = gsl_matrix_alloc (8, 8) ;
    gsl_vector *U = gsl_vector_alloc (8);
    for (long_unsigned_int i{0} ; i < 4 ; i++) {
        gsl_matrix_set (X, 2*i,   0, a.vertices[i].x) ;
        gsl_matrix_set (X, 2*i,   1, a.vertices[i].y) ;
        gsl_matrix_set (X, 2*i,   2, 1) ;
        gsl_matrix_set (X, 2*i,   3, 0) ;
        gsl_matrix_set (X, 2*i,   4, 0) ;
        gsl_matrix_set (X, 2*i,   5, 0) ;
        gsl_matrix_set (X, 2*i,   6, -a.vertices[i].x*b.vertices[i].x) ; // -x0.u0
        gsl_matrix_set (X, 2*i,   7, -a.vertices[i].y*b.vertices[i].x) ; // -y0.u0
        gsl_matrix_set (X, 2*i+1, 0, 0) ;
        gsl_matrix_set (X, 2*i+1, 1, 0) ;
        gsl_matrix_set (X, 2*i+1, 2, 0) ;
        gsl_matrix_set (X, 2*i+1, 3, a.vertices[i].x) ;
        gsl_matrix_set (X, 2*i+1, 4, a.vertices[i].y) ;
        gsl_matrix_set (X, 2*i+1, 5, 1) ;
        gsl_matrix_set (X, 2*i+1, 6, -a.vertices[i].x*b.vertices[i].y) ; // -x0.v0
        gsl_matrix_set (X, 2*i+1, 7, -a.vertices[i].y*b.vertices[i].y) ; // -y0.v0
        gsl_vector_set (U, 2*i,   b.vertices[i].x) ; // u0
        gsl_vector_set (U, 2*i+1, b.vertices[i].y) ; // v0
    }
// std::cout << "Matrix X filled with equation parameters:\n" ;
// SQUARE_MATRIX_DISPLAY(X, 8)
// std::cout << "Vector U filled with equation parameters:\n" ;
// VECTOR_DISPLAY(U, 8)

    gsl_vector *H = gsl_vector_alloc (8);
    gsl_permutation *p = gsl_permutation_alloc (8) ;
    int s ;
    gsl_linalg_LU_decomp (X, p, &s) ;
    gsl_linalg_LU_solve  (X, p, U, H) ;
// std::cout << "Vector H after problem resolution:\n" ;
// VECTOR_DISPLAY(H, 8)

    Matrix3x3 coefficients ;
    for (long_unsigned_int i{0} ; i < 8 ; i++)
        coefficients.set_coefficient (i, gsl_vector_get(H, i)) ;

// std::cout << "copied coefficients:\n" ;
// coefficients. display () ;
    //coefficients[8] = 1 ;
    coefficients.set_coefficient (8, 1) ;
// std::cout << "last coefficient set to 1:\n" ;
// coefficients. display () ;

    gsl_matrix_free (X) ;
    gsl_vector_free (U) ;
    gsl_vector_free (H) ;
    gsl_permutation_free (p) ;

// End of the GSL section
/////////////////////////

//    std::array<double, 9> result ;
    // I find the gsl_blas multiplications routines difficult to read, and they
    // need manual allocation of temporary variables. So for 3x3 matrices we
    // just can use hand-written multiplications and inversion functions.
//     Matrix3x3 ancien_result = matrix3x3_multiply(matrix3x3_inverse (normalization_b),
//                                 matrix3x3_multiply(coefficients,
//                                                    normalization_a)) ;
//     std::cout << "Ancien résultat :\n" ;
//     ancien_result. display () ;

//     std::cout << "B^-1 :\n" ;
//     Matrix3x3 Z1 = matrix3x3_inverse (normalization_b) ;
//     Matrix3x3 Z2 = normalization_b. inverse () ;
//     Z1. display() ;
//     Z2. display() ;
    Matrix3x3 result {(normalization_b.inverse())*(coefficients*normalization_a)} ;
//     result. display () ;
    return result ;
}
#endif

dpps::Vertex dpps::Matrix3x3::apply (const Vertex &v) const {
    double x1 {coefficients[0]*v.x + coefficients[1]*v.y + coefficients[2]} ;
    double y {coefficients[3]*v.x + coefficients[4]*v.y + coefficients[5]} ;
    return Vertex (x1, y) ;
}

dpps::Vertex dpps::Matrix3x3::apply_homography (const Vertex &v) const {
    double denominator {v.x*coefficients[6] + v.y*coefficients[7] + coefficients[8]} ;
    double x {(v.x*coefficients[0] + v.y*coefficients[1] + coefficients[2]) / denominator} ;
    double y {(v.x*coefficients[3] + v.y*coefficients[4] + coefficients[5]) / denominator} ;
    return Vertex (x, y) ;
}

// dpps::Matrix3x3 dpps::Matrix3x3::matrix3x3_multiply (const Matrix3x3 &a, const Matrix3x3 &b) {
//     return Matrix3x3(a.coefficients[0]*b.coefficients[0]+a.coefficients[1]*b.coefficients[3]+a.coefficients[2]*b.coefficients[6], a.coefficients[0]*b.coefficients[1]+a.coefficients[1]*b.coefficients[4]+a.coefficients[2]*b.coefficients[7], a.coefficients[0]*b.coefficients[2]+a.coefficients[1]*b.coefficients[5]+a.coefficients[2]*b.coefficients[8],
//              a.coefficients[3]*b.coefficients[0]+a.coefficients[4]*b.coefficients[3]+a.coefficients[5]*b.coefficients[6], a.coefficients[3]*b.coefficients[1]+a.coefficients[4]*b.coefficients[4]+a.coefficients[5]*b.coefficients[7], a.coefficients[3]*b.coefficients[2]+a.coefficients[4]*b.coefficients[5]+a.coefficients[5]*b.coefficients[8],
//              a.coefficients[6]*b.coefficients[0]+a.coefficients[7]*b.coefficients[3]+a.coefficients[8]*b.coefficients[6], a.coefficients[6]*b.coefficients[1]+a.coefficients[7]*b.coefficients[4]+a.coefficients[8]*b.coefficients[7], a.coefficients[6]*b.coefficients[2]+a.coefficients[7]*b.coefficients[5]+a.coefficients[8]*b.coefficients[8]) ;
// }
//
//
// dpps::Matrix3x3 dpps::Matrix3x3::matrix3x3_inverse (const Matrix3x3 &a) {
//     double det ;
//     if ((det = fabs (a. determinant ())) > std::numeric_limits<double>::epsilon()) {
//         return Matrix3x3(( a.coefficients[4]*a.coefficients[8]-a.coefficients[5]*a.coefficients[7])/det, (-a.coefficients[1]*a.coefficients[8]+a.coefficients[2]*a.coefficients[7])/det, ( a.coefficients[1]*a.coefficients[5]-a.coefficients[2]*a.coefficients[4])/det,
//                  (-a.coefficients[3]*a.coefficients[8]+a.coefficients[5]*a.coefficients[6])/det, ( a.coefficients[0]*a.coefficients[8]-a.coefficients[2]*a.coefficients[6])/det, (-a.coefficients[0]*a.coefficients[5]+a.coefficients[2]*a.coefficients[3])/det,
//                  ( a.coefficients[3]*a.coefficients[7]-a.coefficients[4]*a.coefficients[6])/det, (-a.coefficients[0]*a.coefficients[7]+a.coefficients[1]*a.coefficients[6])/det, ( a.coefficients[0]*a.coefficients[4]-a.coefficients[1]*a.coefficients[3])/det) ;
//     } else
//         return Matrix3x3() ;
// }
