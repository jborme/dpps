/**
 * @file Pattern_Penrose_tiling.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Pattern derivative that implements P2 and P3 Penrose tilings.
 * @date 2014-01-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_PATTERN_PENROSE_TILING
#define DPPS_PATTERN_PENROSE_TILING

#include "Pattern.hh"

namespace dpps {
    typedef enum {
        start_tile_thin,
        start_tile_large,
        start_tile_two_thin,
        start_tile_two_large
    } enum_start_tile ;

/** @brief Pattern_Penrose_tiling generates a 2D aperiodic pattern of symmetry
 * 5, which can be used to model certain kinds of quasicrystals. One type of
 * Penrose patterns is implemented: P3, which should be enough to get desired
 * topological effects are all the other are mutually locally derivable.
 */
class Pattern_Penrose_tiling: public Pattern {
public:
    /** @brief A derived structure to encapsulate data defining the pattern. */
    struct Pattern_Penrose_tiling_settings: public Pattern_settings {
    public:
        /** @brief The x coordinate of the center of the pattern */
        double x0 {0.0} ;

        /** @brief The y coordinate of the center of the pattern */
        double y0 {0.0} ;

        /** @brief The element size at first generation.
         *
         * In case of a P3 pattern starting with an acute Robinson triangle,
         * this will be the length of the long edge at the first generation.
         * In case of starting with an obbtuse triangle, it will be the length
         * of the short edge. (This way the two tiles at first generation also
         * would combine).
         *
         * Size will decrease by a factor phi (1.61803...) at each generation,
         * so one should take an initial vale large enough. When in need to
         * increase the generation number, one should also scale the size
         * by a factor phi in order to keep the same size at the finest element.
         *
         */
        double size {1.0} ;

        /** @brief The generation number of the pattern. */
        long_unsigned_int generation {10} ;

        //** @brief whether we generate a type P3 (true, default) or a
        // * type P2 (false) */
        //bool P3 {true} ;

        /** @brief what tile to use as the first. Default is one thin tile.
         *
         * The small tile refers to the acute Robinson triangle, the large tile
         * to the obtuse Robinson triangle. The composition of two depends
         * on the pattern.
         * For the P3 pattern, two small tiles make a thin element, two large
         * tiles make a fat element.
         * For the P2 pattern, two large tiles make a kite, and
         * two small tiles make a dart. (P2 is unimplemented yet.)
         */
        enum_start_tile start_tile {start_tile_thin} ;
    } ;

protected:
    Pattern_Penrose_tiling_settings pattern_settings ;

public:
    /** @brief the default constructor */
    Pattern_Penrose_tiling () ;

    /** @brief the copy constructor */
    Pattern_Penrose_tiling (const Pattern_Penrose_tiling &source) ;

    /** @brief the move constructor. */
    Pattern_Penrose_tiling (Pattern_Penrose_tiling &&source) ;

    /** @brief the move assignment constructor. */
    Pattern_Penrose_tiling &operator= (Pattern_Penrose_tiling &&source) ;

    /** @brief the copy assignment constructor. */
    Pattern_Penrose_tiling &operator= (const Pattern_Penrose_tiling &source) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    void generate () ;
    //void generate_P2 () ;
    void generate_P3 () ;
    Pattern_Penrose_tiling (
        const long_unsigned_int generation,
        const enum_start_tile start_tile,
        const double x0,
        const double y0,
        const double size) ;

    void set_all_parametres (
        const long_unsigned_int generation,
        const enum_start_tile start_tile,
        const double x0,
        const double y0,
        const double size) ;
} ;
} // namespace dpps
#endif
