/**
 * @file Pattern_hard_sphere_random_packing.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that implements a hard sphere random packing.
 * derivative.
 * @date 2014-02-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_HARD_SPHERE_RANDOM_PACKING
#define DPPS_PATTERN_HARD_SPHERE_RANDOM_PACKING

#include "Pattern.hh"

namespace dpps {
/** @brief Pattern derivative that implements a random packing of hard spheres.
 *
 * This model could implement an initial state for a 2D weakly interacting
 * monoatomic liquid. It is a generic 2D amorphous system that does not take
 * into consideration atomic interactions. However the construction process
 * could be mimicked in a physical system by sending glued balls one against
 * the others.
 *
 * Note that we would like to reproduce things like:
 *
 * Experimental observation of plasmons in a graphene monolayer resting on
 * a two-dimensional subwavelength silicon grating
 * Xiaolong Zhu, Wei Yan, Peter Uhd Jepsen, Ole Hansen,
 * N. Asger Mortensen et al.
 * Appl. Phys. Lett. 102, 131101 (2013)
 * http://dx.doi.org/10.1063/1.4799173

 */
class Pattern_hard_sphere_random_packing: public Pattern {
protected:
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_hard_sphere_random_packing_settings:
            public Pattern_settings {
    public:
        /** @brief the x position of the centre of the pattern */
        double x0 {0.0} ;

        /** @brief the y position of the centre of the pattern */
        double y0 {0.0} ;

        /** @brief the x size of the whole pattern */
        double lx {100.0} ;

        /** @brief the y size of the whole pattern */
        double ly {100.0} ;

        /** @brief the diametre of the monoatomic element */
        double diametre {1.0} ;

        /** @brief the maximum number of elements added */
        long_unsigned_int number_total {1000} ;

        /** @brief the number of elements whose position is randomlt chosen as
         * a first iteration.
         *
         * This correspond to the nucleation centres of the structure. If set to
         * 0, a single nucleation centre is used located at (0,0). If 1 or more,
         * then as an initial step, a uniform random distribution is first used
         * before starting the agglutination growth.
         *
         * Note that the major difference between this random distribution and
         * Pattern_random is that Pattern_amorphous will exclude positions
         * of spheres too close from each other.
         */
        long_unsigned_int number_random {0} ;
    } ;

    /** @brief A class to encapsulate a monoatomic position. It is used during
     * the generation of the pattern */
    class hard_sphere {
    public:
        /** @brief the x center of the sphere */
        double x {0.0} ;

        /** @brief the y center of the sphere */
        double y {0.0} ;

        // @brief the diameter of the sphere */
        //double diametre {1.0} ;

        /** @brief whether the sphere is already fully surrounded by
         * neighbours */
        bool full {false} ;

        /** @brief contructor with three arguments, the centre position defined
         * by x0 and y0 and a boolean describing whether there is more free
         * space around, or not */
        hard_sphere (const double x0, const double y0, const bool set_full) ;

        /** @brief default contructor, sets centre position to (0, 0) and
         * not full. */
        hard_sphere () ;

        /** @brief Displays (x;y;full) on standard output
        *
        * This function just really sends the result of display_string to the
        * standard output.
        */
        void display () const ;

        /** @brief Returns a string description of the hard_sphere. */
        std::string display_string () const ;

        /** @brief Returns the square of the distance to another sphere.
         *
         * If the current sphere has coordinates (x, y), and the one in argument
         * has coordinates (z, t), then it returns (x-z)^2+(y-t)^2. User can
         * get the distance by calculating the square root of this value.
         *
         * The purpose of this function is to be useful for comparisons of
         * distances, a context in which the exact distance is not needed, and
         * therefore its computation cost can be spared.
         *
         */
        double squared_distance (const hard_sphere &s) const ;
    };
protected:
    Pattern_hard_sphere_random_packing_settings pattern_settings ;

public:
    /** @brief the default constructor */
    Pattern_hard_sphere_random_packing () ;

    /** @brief the copy constructor */
    Pattern_hard_sphere_random_packing (const Pattern_hard_sphere_random_packing &source) ;

    /** @brief the move constructor. */
    Pattern_hard_sphere_random_packing (Pattern_hard_sphere_random_packing &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_hard_sphere_random_packing &operator= (Pattern_hard_sphere_random_packing &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_hard_sphere_random_packing &operator= (const Pattern_hard_sphere_random_packing &source) ;

    Pattern_hard_sphere_random_packing (
        const long_unsigned_int number_total,
        const long_unsigned_int number_random,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;
    void set_all_parametres (
        const long_unsigned_int number_total,
        const long_unsigned_int number_random,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre) ;
} ;
} // namespace dpps
#endif
