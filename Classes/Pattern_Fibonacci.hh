/**
 * @file Pattern_Fibonacci.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that implements a Fibonacci grating
 * @date 2014-01-28 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_FIBONACCI
#define DPPS_PATTERN_FIBONACCI

#include "Pattern.hh"

namespace dpps {
/** @brief Pattern_Fibonacci_grating is a Pattern derivative that generates a 1D
 * Fibonacci grating defined on a fixed notwork, where L corresponds to a dot,
 * and S to the absence of a dot.
 */
class Pattern_Fibonacci: public Pattern {
public:
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_Fibonacci_settings: public Pattern_settings {
    public:
        /** @brief The x coordinate of the center of the pattern */
        double x0 {0.0} ;

        /** @brief The y coordinate of the center of the pattern */
        double y0 {0.0} ;

        /** @brief The distance from one element to the next. */
        double size {1.0} ;

        /** @brief The generation number of the Fibonacci pattern. */
        long_unsigned_int generation {10} ;

        /** @brief whether we generate a bidimensional grid (true) or a
         * monodimensional grating of lines (false, default) */
        bool bidimentionnal {false} ;
    } ;

protected:
    Pattern_Fibonacci_settings pattern_settings ;
    static constexpr const double linear_ratio {0.5} ;
#ifdef __GNUC__
#ifndef __clang__
    // gcc allows to calculate those, although sqrt is not
    // constexpr by the standard
    static constexpr const double area_ratio {sqrt (0.5)/2.0} ;
    static constexpr const double golden_ratio {(1.0+sqrt(5.0))/2.0} ;
#define CONSTEXPR_OK
#endif
#endif
#ifndef CONSTEXPR_OK
    // for other compilers, we provide an approximation.
    static constexpr const double area_ratio {0.35355339} ;
    static constexpr const double golden_ratio {1.618033989} ;
#endif

    const char true_ = 'T' ;
    const char false_ = 'F' ;

    /** @brief Returns a vector containing at least n elements of
     * a Fibonacci sequence. The boolean answer uses true for L, false for S
     */
    void generate_Fibonacci_sequence (long_unsigned_int n,
                                      std::vector<char> &result) ;
    //void generate_Fibonacci_array (long_unsigned_int n,
    //                               std::vector<std::vector <bool>> &result) ;
//     void modify_row_A (std::vector <bool> &result) ;
//     void modify_row_B (std::vector <bool> &result) ;
//     void modify_column_A (std::vector <bool> &result) ;
//     void modify_column_B (std::vector <bool> &result) ;

public:
    /** @brief the default constructor */
    Pattern_Fibonacci () ;

    /** @brief the copy constructor */
    Pattern_Fibonacci (const Pattern_Fibonacci &source) ;

    /** @brief the move constructor. */
    Pattern_Fibonacci (Pattern_Fibonacci &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_Fibonacci &operator= (Pattern_Fibonacci &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_Fibonacci &operator= (const Pattern_Fibonacci &source) ;

    Pattern_Fibonacci (
        const bool bidimentionnal,
        const long_unsigned_int generation,
        const double x0,
        const double y0,
        const double size) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;
    void generate_1d () ;
    void generate_2d () ;

    void set_all_parametres (
        const bool bidimentionnal,
        const long_unsigned_int generation,
        const double x0,
        const double y0,
        const double size) ;
} ;
} // namespace dpps
#endif
