/**
 * @file Pattern_random.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative of randomly situated elements according to the
 * different real distributions supported by the C++11 standard.
 * @date 2013-02-03 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_RANDOM
#define DPPS_PATTERN_RANDOM

#include "Pattern.hh"

namespace dpps {
/** @brief Pattern_random is a Pattern derivative that generates a random
 * distribution of elements. Several random distributions are supported,
 * the most important of which are the uniform distribution and the normal
 * distribution. Elements are squares of a certain side. The user can call
 * Pattern::replace_polyline() to replace by the element of his choice.
 */
class Pattern_random: public Pattern {
public:
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_random_settings: public Pattern_settings {
    public:
        /** @brief x coordinate of the centre of the pattern. */
        double x0 {0.0} ;

        /** @brief y coordinate of the centre of the pattern. */
        double y0 {0.0} ;

        /** @brief length along x of the maximum extent of the pattern.
         *
         * This parametre applies a cutoff to the distribution. No point will be
         * selected outside this limit. Note that the criterium is applied to
         * the center of the small square elements.
         * Set to a negative value to disable cutoff.
         */
        double lx {100.0} ;

        /** @brief length along y of the maximum extent of the pattern.
         *
         * This parametre applies a cutoff to the distribution. No point will be
         * selected outside this limit. Note that the criterium is applied to
         * the center of the small square elements.
         * Set to a negative value to disable cutoff.
         */
        double ly {100.0} ;

        double p1 {1.0} ;
        double p2 {1.0} ;

        /** @brief The type of distribution. Default is 0 (uniform). */
        enum_distribution type {type_uniform_real_distribution} ;

        /** @brief The number of elements to include in the distribution. */
        long_unsigned_int number {100} ;
        long_unsigned_int max_attempts {100} ;

        /** @brief The side of the square elements to be added. */
        short unsigned int side {1} ;
    } ;

protected:
//     std::uniform_real_distribution<int> d0 ;
//     std::uniform_real_distribution<int> d0 ;

    Pattern_random_settings pattern_settings ;

public:
    /** @brief the default constructor */
    Pattern_random () ;

    /** @brief the copy constructor */
    Pattern_random (const Pattern_random &source) ;

    /** @brief the move constructor. */
    Pattern_random (Pattern_random &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_random &operator= (Pattern_random &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_random &operator= (const Pattern_random &source) ;

    Pattern_random (
        const enum_distribution type,
        const long_unsigned_int number,
        const long_unsigned_int max_attempts,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double side,
        const double p1,
        const double p2) ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;
    void set_all_parametres (
        const enum_distribution type,
        const long_unsigned_int number,
        const long_unsigned_int max_attempts,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double side,
        const double p1,
        const double p2) ;
} ;
} // namespace dpps
#endif
