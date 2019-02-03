/**
 * @file Pattern.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation file for abstract base class Pattern
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The \c Pattern.cpp file contains implementation for the simple methods
 * which are common to all Pattern types.
 *
 * The pseudorandom generator is the default one (implementation-dependant) as
 * we are not very concerned with the high quality of the random number used
 * for the selection. However the initilization is done with a timestamp, so we
 * guarantee that hey will never be the same at two launches of the function.
 */

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <chrono>
#include <cmath>
#include <array>

#include "common.hh"
#include "Pattern.hh"
#include "Pattern_factory.hh"
#include "bad_parametre.hh"

dpps::Pattern::Pattern () {
    // Used references:
    // M. Josuttis, The C++ Standard Library 2nd Ed., 5.7.3 p. 149 seq.
    // http://www.cplusplus.com/reference/chrono/time_point/time_since_epoch/
    typedef std::chrono::high_resolution_clock high_clock ;
    high_clock::time_point time_point_now {high_clock::now ()} ;
    high_clock::duration duration_now {time_point_now. time_since_epoch ()} ;
    time_seed = duration_now. count () ;
    pseudorandom_generator. seed (time_seed) ;
//    pseudorandom_generator. seed (0) ; // non-random initialization for debug purposes
}

dpps::Pattern::~Pattern () {
}

dpps::Pattern::Pattern (const Pattern &source):
    polylines (source. polylines),
    pseudorandom_generator (source. pseudorandom_generator),
    time_seed (source. time_seed) {
}

dpps::Pattern::Pattern (Pattern &&source):
    polylines (std::move (source. polylines)),
    pseudorandom_generator (
        std::move (source. pseudorandom_generator)),
    time_seed (std::move (source. time_seed)) {
}

dpps::Pattern &dpps::Pattern::operator= (dpps::Pattern &&source) {
    if (this != &source) {
        polylines = std::move (source. polylines) ;
        pseudorandom_generator = source. pseudorandom_generator ;
        time_seed = source. time_seed ;
    }
    return *this ;
}

dpps::Pattern &dpps::Pattern::operator= (const dpps::Pattern &source) {
    polylines = source. polylines ;
    pseudorandom_generator = source. pseudorandom_generator ;
    time_seed = source. time_seed ;
    return *this ;
}

void dpps::Pattern::generate () {}

void dpps::Pattern::set_parametres (
                     const std::vector<bool> &vbool,
                     const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)    ||
        (vint. size () != 0)    ||
        (vdouble. size () != 0) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Pattern::set_parametres",
            0, 0, 0, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
}

// long_unsigned_int dpps::Pattern::size_all () const {
//     return polylines. size () ;
// }

// Selection functions
//////////////////////

void dpps::Pattern::select (
    const enum_logical_action action,
    const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ; ;
    switch (action) {
        case logical_action_all: // 0
            std::for_each (polylines. begin (), polylines. end(),
                [=] (Polyline &p) {
                    p. selected [selection] = true ;
                }) ;
//             for (auto &p : polylines)
//                 p. selected [selection] = true ;
            break ;
        case logical_action_none: // 1
            std::for_each (polylines. begin (), polylines. end(),
                [=] (Polyline &p) {
                    p. selected [selection] = false ;
                }) ;
//             for (auto &p : polylines)
//                 p. selected [selection] = false ;
            break ;
        case logical_action_toggle: // 2
            std::for_each (polylines. begin (), polylines. end(),
                [=] (Polyline &p) {
                    p. selected [selection] = !(p. selected [selection]) ;
                }) ;
//             for (auto &p : polylines)
//                 p. selected [selection] = !(p. selected[selection]) ;
            break ;
        // no need of default:, we did the whole enum.
    }
}

void dpps::Pattern::select_all (const bool status,
                                 const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &p : polylines) {
        if ((selection < 0) || (p. selected[selection])) {
            // if performance issue, consider duplicating the code
            p. select_all (status) ;
        }
    }
}

void dpps::Pattern::toggle_all (const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &p : polylines) {
        if ((selection < 0) || (p. selected[selection])) {
            p. selected. flip () ; // p. toggle_all () ; // we avoid function call
        }
    }
}

void dpps::Pattern::toggle (const selection_t selection_to_toggle, const selection_t selection) {
check_selection_not_negative_not_too_high(selection_to_toggle) ;
check_selection_not_too_high(selection) ;
    for (auto &p : polylines) {
        if ((selection < 0) || (p. selected[selection])) {
            p. selected[selection_to_toggle] = !p. selected[selection_to_toggle] ;
        }
    }
}

void dpps::Pattern::select_window (const double xmin, const double ymin,
                        const double xmax, const double ymax,
                        const bool dots_as_circles,
                        const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    double temp_min_x {0.0} ;
    double temp_min_y {0.0} ;
    double temp_max_x {0.0} ;
    double temp_max_y {0.0} ;
    for (auto &p : polylines) {
        if (dots_as_circles && (p.size() == 1)) {
            temp_min_x = p.vertices[0].x-p.dose;
            temp_min_y = p.vertices[0].y-p.dose;
            temp_max_x = p.vertices[0].x+p.dose;
            temp_max_y = p.vertices[0].y+p.dose;
        } else
            p.limits (temp_min_x, temp_min_y, temp_max_x, temp_max_y) ;
        if ((temp_min_x >= xmin) && (temp_min_x <= xmax) &&
            (temp_min_y >= ymin) && (temp_min_y <= ymax) &&
            (temp_max_x >= xmin) && (temp_max_x <= xmax) &&
            (temp_max_y >= ymin) && (temp_max_y <= ymax)) {
            p. selected [selection] = true ;
        }
    }
}

void dpps::Pattern::select_window (const Vertex &min, const Vertex &max) {
    select_window (min. x, min. y, max. x, max. y) ;
}

void dpps::Pattern::select_window (const Vertex &min, const Vertex &max,
                        const bool dots_as_circles) {
    select_window (min. x, min. y, max. x, max. y, dots_as_circles) ;
}

void dpps::Pattern::select_window (const Vertex &min, const Vertex &max,
                        const bool dots_as_circles,
                        const selection_t selection) {
    select_window (min. x, min. y, max. x, max. y, dots_as_circles, selection) ;
}
void dpps::Pattern::selection_next (const selection_t origin_selection,
                           const selection_t destination_selection,
                           const bool unselect_origin) {
check_selection_not_negative_not_too_high(origin_selection) ;
check_selection_not_negative_not_too_high(destination_selection) ;
    if (polylines. size () == 0)
        return ;
    long_unsigned_int number_selected {size (origin_selection)} ;
    if (number_selected == 0)
        return ;
    long_unsigned_int last_selected {polylines. size ()-1} ;
    // Because number_selected is not zero, we are assured the loop
    // will end before the index gets out of bound.
    while (!polylines[last_selected]. selected[origin_selection])
        last_selected-- ;
    if (unselect_origin)
        select (logical_action_none, origin_selection) ;
    long_unsigned_int max_loop {
        std::min (number_selected,
                  static_cast<long_unsigned_int>(polylines. size ())
                    - 1 - last_selected)} ;
    for (long_unsigned_int i {last_selected + 1} ;
                           i < last_selected + max_loop + 1; i++)
        polylines[i]. selected[destination_selection] = true ;
}

void dpps::Pattern::select_front (const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    if (polylines. size () >= 1)
        polylines. front (). selected[selection] = 1 ;
}

void dpps::Pattern::select_back (const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    if (polylines. size () >= 1)
        polylines. back (). selected[selection] = 1 ;
}

void dpps::Pattern::select_integer_property (
         const enum_integer_property property,
         const long_unsigned_int parametre,
         const enum_comparison_operator comparison_operator,
         const long_unsigned_int comparison_value,
         const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    // property == 1 is for modulo, we do not calculate modulo 0.
    if ((property == integer_property_number_vertices_modulo) && (parametre == 0))
        return ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            bool result {false} ;
            long_unsigned_int compared {
                p. get_integer_property (property, parametre)} ;
            switch (comparison_operator) {
                case comparison_operator_equal: // 0 ==
                    result = (compared == comparison_value) ;
                    break ;
                case comparison_operator_lower_equal: // 1 <=
                    result = (compared <= comparison_value) ;
                    break ;
                case comparison_operator_greater_equal: // 2 >=
                    result = (compared >= comparison_value) ;
                    break ;
                case comparison_operator_strictly_lower: // 3 <
                    result = (compared < comparison_value) ;
                    break ;
                case comparison_operator_strictly_greater: // 4 >
                    result = (compared > comparison_value) ;
                    break ;
                case comparison_operator_different: // 5 !=
                    result = (compared != comparison_value) ;
                    break ;
                default:
                    break ;
            }
            p. selected[selection] = result ;
            //std::cout << "Polyline selection " << (int) selection << " status " << p.selected[selection]<< " " << std::to_string (p.selected. to_ulong ()) << "\n";
        }) ;
//     for (long_unsigned_int i {0} ; i < polylines. size () ; i++) {
//     }
}

void dpps::Pattern::select_double_property (
                const enum_double_property property,
                const long_unsigned_int parametre,
                const enum_comparison_operator comparison_operator,
                const double comparison_value,
                double tolerance,
                const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
//     double compared {0.0} ;
//     if (tolerance < 0)
//         tolerance = std::numeric_limits<double>::epsilon() ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            bool result {false} ;
            double compared = p. get_double_property (property, parametre) ;
            switch (comparison_operator) {
                case comparison_operator_equal: // 0 ==
                    result = (fabs (compared - comparison_value) <= tolerance) ;
                    break ;
                case comparison_operator_lower_equal: // 1 <=
                    result = (compared <= comparison_value) ;
                    break ;
                case comparison_operator_greater_equal: // 2 >=
                    result = (compared >= comparison_value) ;
                    break ;
                case comparison_operator_strictly_lower: // 3 <
                    result = (compared < comparison_value) ;
                    break ;
                case comparison_operator_strictly_greater: // 4 >
                    result = (compared > comparison_value) ;
                    break ;
                case comparison_operator_different: // 5 !=
                    result = (fabs (compared - comparison_value) > tolerance) ;
                    break ;
                default: // not reachable
                    break ;
    //                 std::string reason {"Pattern::double_property:
    // argument comparison_operator is an enumeration valued between 0 and 5, got " +
    //                     std::to_string_int (comparison_operator)} ;
    //                 throw bad_parametre_value (reason. c_str ()) ;
            }
            p. selected [selection] = result ;
        }) ;

//     for (auto &p : polylines) {
//     }
}

void dpps::Pattern::selection_swap (const selection_t selection_p,
                                    const selection_t selection_q) {
check_selection_not_negative_not_too_high(selection_p) ;
check_selection_not_negative_not_too_high(selection_q) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            long_unsigned_int p_old {p. selected [selection_p]} ;
            p. selected [selection_p] = p. selected [selection_q] ;
            p. selected [selection_q] = p_old ;
        }) ;
//     for (auto &p : polylines) {
//         long_unsigned_int p_old {p. selected [selection_p]} ;
//         p. selected [selection_p] = p. selected [selection_q] ;
//         p. selected [selection_q] = p_old ;
//     }
}

void dpps::Pattern::selection_logical_operation (
                         const selection_t destination_selection,
                         const selection_t selection_p,
                         const selection_t selection_q,
                         const enum_logical_operator logical_operator) {
check_selection_not_negative_not_too_high(selection_p) ;
check_selection_not_negative_not_too_high(selection_q) ;
check_selection_not_negative_not_too_high(destination_selection) ;

    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            p. selected [destination_selection] =
                logical_operation (p. selected [selection_p],
                                p. selected [selection_q],
                                logical_operator) ;
        }) ;
//     for (auto &p : polylines)
//         p. selected [destination_selection] =
//             logical_operation (p. selected [selection_p],
//                                p. selected [selection_q],
//                                logical_operator) ;
}

void dpps::Pattern::select_uniform_pseudorandom (const double fraction,
                                          const selection_t selection) {
check_selection_not_negative_not_too_high(selection) ;
    constexpr const double large_number {10000000.0} ;
    std::uniform_int_distribution<unsigned int>
            pseudorandom_distribution (0, large_number);
    std::for_each (polylines. begin (), polylines. end(),
        [&] (Polyline &p) {
            p. selected [selection] = (
                    (1.0 * pseudorandom_distribution (pseudorandom_generator))
                    / (1.0 * large_number) <= fraction) ;
        }) ;
}

long_unsigned_int dpps::Pattern::size (
    const selection_t selection) const {
check_selection_not_too_high(selection) ;
    long_unsigned_int n {0} ;
    // the reference &p is used to avoid copy
    for (auto &p : polylines)
        if ((selection < 0) || (p. selected[selection]))
            n++ ;
    return n ;
}

// Iterated functions from Polyline
///////////////////////////////////
void dpps::Pattern::symmetry_x (const double y0, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. symmetry_x (y0) ;
        }) ;
//     for (auto &p: polylines)
//         if (p. selected[selection])
//             p. symmetry_x (y0) ;
}

void dpps::Pattern::symmetry_y (const double x0, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. symmetry_y (x0) ;
        }) ;
//     for (auto &p: polylines)
//         if (p. selected[selection])
//             p. symmetry_y (x0) ;
}

void dpps::Pattern::symmetry_x_centred (const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            p. symmetry_x_centred () ;
}

void dpps::Pattern::symmetry_y_centred (const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. symmetry_y_centred () ;
        }) ;
//     for (auto &p: polylines)
//         if (p. selected[selection])
//             p. symmetry_y_centred () ;
}


double dpps::Pattern::area (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. area () ;
    return result ;
}

double dpps::Pattern::algebraic_area (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. algebraic_area () ;
    return result ;
}

double dpps::Pattern::perimeter (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. perimeter () ;
    return result ;
}

double dpps::Pattern::algebraic_perimeter (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. algebraic_perimeter () ;
    return result ;
}

double dpps::Pattern::length (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. length () ;
    return result ;
}

double dpps::Pattern::algebraic_length (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double result {0.0} ;
    for (auto &p: polylines)
        if ((selection < 0) || p. selected[selection])
            result += p. algebraic_length () ;
    return result ;
}

/*long_unsigned_int dpps::Pattern::count (selection_t selection) const {
CHECK_SELECTION(selection)
    long_unsigned_int result {0} ;
    for (auto &p: polylines)
        if (p. selected[selection])
            result ++ ;
    return result ;
}*/

double dpps::Pattern::minimum_x (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double minx {std::numeric_limits<double>::max ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. minimum_x ()) < minx) {
                done = true ;
                minx = temporary_value ;
            }
    }
    if (done)
        return minx ;
    else
        return 0.0 ;
}

double dpps::Pattern::minimum_y (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double miny {std::numeric_limits<double>::max ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. minimum_y ()) < miny) {
                done = true ;
                miny = temporary_value ;
            }
    }
    if (done)
        return miny ;
    else
        return 0.0 ;
}

double dpps::Pattern::maximum_x (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double maxx {std::numeric_limits<double>::lowest ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. maximum_x ()) > maxx) {
                done = true ;
                maxx = temporary_value ;
            }
    }
    if (done)
        return maxx ;
    else
        return 0.0 ;
}

double dpps::Pattern::maximum_y (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double maxy {std::numeric_limits<double>::lowest ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. maximum_y ()) > maxy) {
                done = true ;
                maxy = temporary_value ;
            }
    }
    if (done)
        return maxy ;
    else
        return 0.0 ;
}

dpps::Vertex dpps::Pattern::lower_left (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double minx {std::numeric_limits<double>::max ()} ;
    double miny {std::numeric_limits<double>::max ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. minimum_x ()) < minx) {
                done = true ;
                minx = temporary_value ;
            }
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. minimum_y ()) < miny)
                miny = temporary_value ;
    }
    if (done)
        return Vertex (minx, miny) ;
    else
        return Vertex (0.0, 0.0) ;
}

dpps::Vertex dpps::Pattern::upper_right (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    double maxx {std::numeric_limits<double>::lowest ()} ;
    double maxy {std::numeric_limits<double>::lowest ()} ;
    bool done {false} ;
    for (auto &p : polylines) {
        double temporary_value ;
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. maximum_x ()) > maxx) {
                done = true ;
                maxx = temporary_value ;
            }
        if ((selection < 0) || p. selected[selection])
            if ((temporary_value = p. maximum_y ()) > maxy)
                maxy = temporary_value ;
    }
    if (done)
        return Vertex (maxx, maxy) ;
    else
        return Vertex (0.0, 0.0) ;
}

double dpps::Pattern::horizontal_size (const selection_t selection) const {
    Vertex ll {lower_left (selection)},
           ur {upper_right (selection)} ;
    return (ur. x - ll. x) ;
}

double dpps::Pattern::vertical_size (const selection_t selection) const {
    Vertex ll {lower_left (selection)},
           ur {upper_right (selection)} ;
    return (ur. y - ll. y) ;
}

long_unsigned_int dpps::Pattern::vertices_size (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    long_unsigned_int count {0} ;
    for (auto &p : polylines) {
        if ((selection < 0) || p. selected[selection])
            count += p. size () ;
    }
    return count ;
}

/*long_unsigned_int dpps::Pattern::vertices_size_all () const {
    long_unsigned_int count {0} ;
//     std::for_each (polylines. begin (), polylines. end(),
//         [&] (Polyline &p) {
//             count += p. size () ;
//         }) ;
    for (auto &p : polylines) {
        count += p. size () ;
    }
    return count ;
}*/

void dpps::Pattern::scale_centred (const double fx, const double fy, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. scale_centred (fx, fy) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. scale_centred (fx, fy) ;
}

void dpps::Pattern::scale_centred (const double f, const selection_t selection) {
    scale_centred (f, f, selection) ;
}

void dpps::Pattern::scale (const Vertex &v, const double fx, const double fy, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. scale (v, fx, fy) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. scale (v, fx, fy) ;
}

void dpps::Pattern::scale (const double x, const double y,
                           const double fx, const double fy, const selection_t selection) {
    scale (Vertex (x, y), fx, fy, selection) ;
}

void dpps::Pattern::scale (const Vertex &v, const double f, const selection_t selection) {
    scale (v, f, f, selection) ;
}

void dpps::Pattern::scale (const double x, const double y, const double f, const selection_t selection) {
    scale (Vertex (x, y), f, f, selection) ;
}

void dpps::Pattern::rotate_centred (const double angle, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. rotate_centred (angle) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. rotate_centred (angle) ;
}

void dpps::Pattern::rotate (const Vertex &v, const double angle, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. rotate (v, angle) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. rotate (v, angle) ;
}

void dpps::Pattern::rotate (const double x, const double y,
                            const double angle,
                            const selection_t selection) {
    rotate (Vertex (x, y), angle, selection) ;
}

void dpps::Pattern::transpose (const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. transpose () ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. transpose () ;
}

void dpps::Pattern::translate (const Vertex &v, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. translate (v) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. translate (v) ;
}

void dpps::Pattern::translate (const double x, const double y, const selection_t selection) {
    translate (Vertex (x, y), selection) ;
}

void dpps::Pattern::remove_consecutive_duplicated_vertices (
    const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. remove_consecutive_duplicated_vertices (precision) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. remove_consecutive_duplicated_vertices (precision) ;
}

void dpps::Pattern::remove_duplicated_vertices (const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection])
                p. remove_duplicated_vertices (precision) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. remove_duplicated_vertices (precision) ;
}

void dpps::Pattern::replace_by (const Polyline &polyline,
                                const bool centre,
                                const bool keep_initial_properties,
                                const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || p. selected[selection]) {
                if (centre)
                    p. replace_by (polyline, keep_initial_properties) ;
                else {
                    if (keep_initial_properties) {
                        Polyline properties ;
                        properties. set_metadata_from (p) ;
                        p = polyline ;
                        p. set_metadata_from (properties) ;
                    } else
                        p = polyline ;
                }
            }
        }) ;
}

void dpps::Pattern::replace_polyline_by_pattern (const Pattern &pat,
                                                 const bool centre,
                                                 const bool keep_initial_properties,
                                                 const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    Vertex pattern_centre (0,0) ;
    if (centre)
        pattern_centre = pat. geometrical_centre_polylines() ;
    for (auto &p : polylines)
        if ((selection < 0) || (p. selected[selection])) {
            Pattern pat_translate (pat) ;
            if (centre) {
                Vertex polyline_centre {p. geometrical_centre ()} ;
                pat_translate. translate (polyline_centre - pattern_centre) ;
            }
            for (auto &r : pat_translate. polylines) {
                if (keep_initial_properties)
                    r. set_metadata_from (p) ;
                result. push_back (r) ;
            }
        } else {
            result. push_back (p) ;
        }
    polylines = result ;
}

void dpps::Pattern::replace_by_regular_polygon (
    const long_unsigned_int number_vertices,
    const double diametre,
    const bool keep_initial_properties,
    const selection_t selection) {
    // We create the polyline and then iterate on Polyline::replace_by()
    // instead of iterating on Polyline::replace_by_regular_polygon() in order
    // to create the polyline just one time and save on expensive cos() and
    // sin() functions.
    Polyline p (number_vertices, diametre) ;
    replace_by (p, selection, keep_initial_properties) ;
}

void dpps::Pattern::explode_in_vertices (const bool keep_initial,
                                         const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    long_unsigned_int current_size {polylines. size ()} ;
    if (current_size == 0)
        return ;
    for (long_unsigned_int i {0} ; i < current_size ; i++) {
        if ((selection < 0) || polylines[i]. selected[selection]) {
        //if (polylines[i]. selected[selection]) {
            Polyline p {polylines[i]} ;
            // p. selected [selection] = false ;
            long_unsigned_int number_vertices {p. size ()} ;
            for (long_unsigned_int j {0} ; j < number_vertices ; j++) {
                p. vertices. clear () ;
                p. vertices. push_back (polylines[i]. vertices[j]) ;
                result. push_back (p) ;
            }
        }
    }
    if (!keep_initial)
        erase (selection) ;
    polylines. insert (polylines. end (), result. begin(), result. end()) ;
}

void dpps::Pattern::explode (const bool keep_initial,
                                         const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    long_unsigned_int current_size {polylines. size ()} ;
    if (current_size == 0)
        return ;
    for (long_unsigned_int i {0} ; i < current_size ; i++) {
        if ((selection < 0) || (polylines[i]. selected[selection])) {
            Pattern q {Pattern_factory::explode (polylines[i])} ;
            result. insert (result. end (),
                            q. polylines. cbegin (),
                            q. polylines. cend ()) ;
        }
    }
    if (!keep_initial)
        erase (selection) ;
    polylines. insert (polylines. end (), result. begin(), result. end()) ;
}

void dpps::Pattern::fill_with_lines (
    const double angle,
    const double spacing,
    const bool keep_initial,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection])) {
            Pattern p {Pattern_factory::fill_with_lines (i, angle, spacing)} ;
            if (keep_initial)
                polylines. push_back (i) ;
            polylines. insert (polylines. end (),
                               p. polylines. begin(),
                               p. polylines. end()) ;

        } else
            result. push_back (i) ;
/*    long_unsigned_int initial_size {polylines. size ()} ;
    for (long_unsigned_int i {0} ; i < initial_size ; i++) {
        Polyline element = *(polylines. begin ()) ;
        polylines. pop_front () ;
        // we remove a polyline from the front, it will be processed and
        // then added to the back.
        if (element. selected[selection]) {
            // If the polyline was selected, we transform it and insert the
            // resulting vector of polylines at the end of the vector.
            // Resulting polylines are selected
            Pattern result {Pattern_factory::fill_with_lines
                                                    (element, angle, spacing)} ;
            polylines. insert (polylines. end (),
                               result. polylines. begin(),
                               result. polylines. end()) ;
            if (keep_initial) {
                // To keep the initial polylines, we just put it back in the end
                polylines. push_back (element) ;
            }
        } else
            // if polyline was not selected, we just put it
            // at the other end of the vector.
            polylines. push_back (element) ;
    }*/
}

void dpps::Pattern::fill_with_lines_first_angle (
    const double spacing,
    const bool keep_initial,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection])) {
            Pattern p {Pattern_factory::fill_with_lines_first_angle (
                                i, spacing)} ;
            if (keep_initial)
                polylines. push_back (i) ;
            polylines. insert (polylines. end (),
                               p. polylines. begin(),
                               p. polylines. end()) ;

        } else
            result. push_back (i) ;
/*    long_unsigned_int initial_size {polylines. size ()} ;
    for (long_unsigned_int i {0} ; i < initial_size ; i++) {
        Polyline element = *(polylines. begin ()) ;
        polylines. pop_front () ;
        if (element. selected[selection]) {
            Pattern result {
                Pattern_factory::fill_with_lines_first_angle (
                    element, spacing)} ;
            polylines. insert (polylines. end (),
                               result. polylines. begin(),
                               result. polylines. end()) ;
            if (keep_initial)
                polylines. push_back (element) ;
        } else
            polylines. push_back (element) ;
    }*/
}

void dpps::Pattern::fill_with_dashes (
            const double angle,
            const double spacing_longitudinal,
            const double spacing_transversal,
            const double duty_cycle,
            const double phase_initial,
            const double phase_increment,
            const enum_partial_dash_policy partial_dash_policy,
            const bool keep_initial,
            const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection])) {
            Pattern p {Pattern_factory::fill_with_dashes
                  (i, angle, spacing_longitudinal, spacing_transversal,
                   duty_cycle, phase_initial, phase_increment,
                   partial_dash_policy)} ;
            if (keep_initial)
                polylines. push_back (i) ;
            polylines. insert (polylines. end (),
                               p. polylines. begin(),
                               p. polylines. end()) ;

        } else
            result. push_back (i) ;
/*    long_unsigned_int initial_size {polylines. size ()} ;
    for (long_unsigned_int i {0} ; i < initial_size ; i++) {
        Polyline element = *(polylines. begin ()) ;
        polylines. pop_front () ;
        if (element. selected[selection]) {
            Pattern result {} ;
            polylines. insert (polylines. end (),
                               result. polylines. begin(),
                               result. polylines. end()) ;
            if (keep_initial)
                polylines. push_back (element) ;
        } else
            polylines. push_back (element) ;
    }*/
}

void dpps::Pattern::fill_with_dashes_first_angle (
           const double spacing_longitudinal,
           const double spacing_transversal,
           const double duty_cycle,
           const double phase_initial,
           const double phase_increment,
           const enum_partial_dash_policy partial_dash_policy,
           const bool keep_initial,
           const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<Polyline> result ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection])) {
            Pattern p {Pattern_factory::fill_with_dashes_first_angle (
                    i, spacing_longitudinal, spacing_transversal,
                    duty_cycle, phase_initial, phase_increment,
                    partial_dash_policy)} ;
            if (keep_initial)
                polylines. push_back (i) ;
            polylines. insert (polylines. end (),
                               p. polylines. begin(),
                               p. polylines. end()) ;

        } else
            result. push_back (i) ;
/*    long_unsigned_int initial_size {polylines. size ()} ;
    for (long_unsigned_int i {0} ; i < initial_size ; i++) {
        Polyline element = *(polylines. begin ()) ;
        polylines. pop_front () ;
        if ((selection < 0) || (element. selected[selection])) {
            Pattern result {
                Pattern_factory::fill_with_dashes_first_angle (
                    element, spacing_longitudinal, spacing_transversal,
                    duty_cycle, phase_initial, phase_increment,
                    partial_dash_policy)} ;
            polylines. insert (polylines. end (),
                               result. polylines. begin(),
                               result. polylines. end()) ;
            if (keep_initial)
                polylines. push_back (element) ;
        } else
            polylines. push_back (element) ;
    }*/
}

void dpps::Pattern::display (const selection_t selection) const {
    long_unsigned_int s_selected{size (selection)} ;
    long_unsigned_int s_total{size (-1)} ;
    std::cout << "{" ;
    long_unsigned_int k{0} ;
    for (long_unsigned_int i{0} ; i < s_total ; i++)
        if ((selection < 0) || (polylines[i]. selected[selection])) {
            polylines[i]. display () ;
            k++ ;
            if (k < s_selected)
                std::cout << ",\n" ;
        }
    std::cout << "}\n" ;
}

// void dpps::Pattern::display_all () const {
//     std::cout << "{" ;
//     for (long_unsigned_int i{0} ; i < polylines. size () ; i++) {
//         polylines[i]. display () ;
//         if (i != polylines. size () - 1)
//             std::cout << ",\n" ;
//     }
//     std::cout << "}\n" ;
// }

std::string dpps::Pattern::display_string (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    std::string result ;
    long_unsigned_int number_selected {size (selection)} ;
    long_unsigned_int j {0} ;
    result += "{" ;
    for (long_unsigned_int i{0} ; i < polylines. size () ; i++)
        if ((selection < 0) || (polylines[i]. selected[selection])) {
            j++ ;
            result += polylines[i]. display_string () ;
            if (j != number_selected)
                result += ",\n" ;
        }
    result += "}\n" ;
    return result ;
}

// std::string dpps::Pattern::display_string_all () const {
//     std::string result ;
//     result += "{" ;
//     for (long_unsigned_int i{0} ; i < polylines. size () ; i++) {
//         result += polylines[i]. display_string () ;
//         if (i != polylines. size () - 1)
//             result += ",\n" ;
//     }
//     result += "}\n" ;
//     return result ;
// }

void dpps::Pattern::duplicate (const selection_t selection) {
check_selection_not_too_high(selection) ;
    long_unsigned_int initial_size {polylines. size ()} ;
    for (long_unsigned_int i {0} ; i < initial_size ; i++)
        if ((selection < 0) || (polylines[i]. selected[selection])) {
            polylines. push_back (polylines[i]) ;
            if (selection >=0)
                polylines[i]. selected[selection] = false ;
        }
}

void dpps::Pattern::reverse_vertices (const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. reverse_vertices () ;
        }) ;
//     for (auto &i : polylines)
//         if (i. selected[selection])
//             i. reverse_vertices () ;
}

void dpps::Pattern::overwrite_selected (const Pattern &p,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    if ((size (selection) == 0) || (size (-1) == 0))
        return ;
    long_unsigned_int j {0} ;
    for (auto &i: polylines)
        if ((selection < 0) || (i. selected[selection])) {
            i = p. polylines [j] ;
            j++ ;
            if (j >= p. size (-1))
                return ;
        }
}

void dpps::Pattern::remove_consecutive_duplicated_polylines (
    const double precision,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    if (size (selection) <= 1) return ;
    std::vector<dpps::Polyline> result ;
    result. push_back (polylines[0]) ;
    for (long_unsigned_int i {1} ; i < polylines. size () ; i++) {
        bool selected {(selection < 0) || (polylines[i]. selected[selection])} ;
        if (!selected || !polylines[i]. equals_to (
                result. at (result. size ()-1), precision))
            result. push_back (polylines[i]) ;
    }
    polylines = result ;

//     Pattern s {dpps::Pattern_factory::Pattern_from_selected (*this, selection)} ;
//     std::cout << "before algorithm :\n" ;
//    s. display () ;
//     if (s. size_all () <= 1) return ;
    // See M. Josuttis, section 10.2.2 Function adapters and binders
    // This will call param_1.equals_to (param_2, precision)
    // where param_1 and param_2 are the parameters passed by
    // std::unique to the function object returned by std::bind()
//     std::unique (s. polylines. begin (),
//                  s. polylines. end (),
//                  std::bind (&Polyline::equals_to,
//                             std::placeholders::_1,
//                             std::placeholders::_2,
//                             precision)) ;
//     std::cout << "\nafter algorithm :\n" ;
//     s. display () ;
//     overwrite_selected (s, selection) ;

    // unlike for vertices in a closed polyline, we do not need to check for
    // equality case between the first and the last polyline, as a Pattern
    // has no notion of being closed.
}

// Note that this function is duplicated in Polyline
void dpps::Pattern::remove_duplicated_polylines (const double precision,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::vector<dpps::Polyline> result ;
    result. push_back (polylines. front()) ;
    for (long_unsigned_int i {1} ; i < polylines. size () ; i++) {
        bool duplicated {false} ;
        for (long_unsigned_int j {0} ; j < result. size () ; j++) {
            if (polylines[i]. equals_to (result[j], precision)) {
                duplicated = true ;
                break ;
            }
        }
        if (!duplicated)
            result. push_back (polylines[i]) ;
    }
    polylines = result ;
}

void dpps::Pattern::set_dose (const double dose_value,
                              const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. dose = dose_value ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. dose = dose_value ;
}

void dpps::Pattern::set_dose_increasing (const double dose_initial,
                                         const double dose_final,
                                         const selection_t selection) {
check_selection_not_too_high(selection) ;
    long_unsigned_int total {0} ;
    for (auto &p : polylines)
        if ((selection < 0) || (p. selected[selection]))
            total++ ;
    if (total == 0)
        return ;
    if (total == 1) {
        set_dose (dose_initial, selection) ;
        return ;
    }
    long_unsigned_int count {0} ;
    double epsilon {(dose_final-dose_initial)/(total-1)} ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. dose = dose_initial + epsilon*count ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection]) {
//             p. dose = dose_initial + epsilon*count ;
//             count++ ;
//         }
}

void dpps::Pattern::add_to_dose (const double additive_constant,
                                 const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. dose += additive_constant ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. dose += additive_constant ;
}

void dpps::Pattern::multiply_dose (const double multiplication_factor,
                                   const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. dose *= multiplication_factor ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. dose *= multiplication_factor ;
}

void dpps::Pattern::erase (const selection_t selection) {
    check_selection_not_too_high(selection) ;
    if (selection < 0) {
        polylines. clear () ;
        return ;
    }
    long_unsigned_int number = size (selection) ;
    if (number == 0)
        return ;
    // get all except this selection
    select (logical_action_toggle, selection) ;
    Pattern q {dpps::Pattern_factory::Pattern_from_selected (*this, selection)} ;
    select (logical_action_none, selection) ;

    polylines. clear () ;
    polylines = std::move (q. polylines);

// Algorithm below is exponential.
//     (For example we define a square lattice, select the dots outside a circle
//     ... /projets/brasil/2014/20141215_Sinnecker/20141217_Sinnecker_rect.lua)
//     long_unsigned_int number = size (selection) ;
//     // std::cout << "number " << number << "\n" ;
//     for (long_unsigned_int i {0} ; i < number ; i++) {
//         //if (i% 1000 == 0)
//         //    std::cout << i << std::endl ;
//         for (long_unsigned_int i {0} ; i < polylines. size () ; i++) {
//             if ((selection < 0) || (polylines. at (i). selected [selection])) {
//                 // NOTE: gcc allows cbegin() here, but clang does not.
//                 polylines. erase (polylines. begin () + i) ;
//                 break ;
//             }
//         }
//     }
}

void dpps::Pattern::set_reference (const long_unsigned_int set_reference_value,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. reference = set_reference_value ;
        }) ;

//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. reference = set_reference_value ;
}

void dpps::Pattern::set_reference_increasing (const selection_t selection) {
check_selection_not_too_high(selection) ;
    long_unsigned_int i {0} ;
    for (auto &p : polylines)
        if ((selection < 0) || (p. selected[selection])) {
            p. reference = i ;
            i++ ;
        }
}

void dpps::Pattern::add_to_reference (const long_unsigned_int additive_constant,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. reference += additive_constant ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. reference += additive_constant ;
}

void dpps::Pattern::subtract_to_reference (
    const long_unsigned_int additive_constant,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection])) {
                if (p. reference >= additive_constant)
                    p. reference -= additive_constant ;
                else
                    p. reference = 0 ;
            }
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection]) {
//             if (p. reference >= additive_constant)
//                 p. reference -= additive_constant ;
//             else
//                 p. reference = 0 ;
//         }
}

void dpps::Pattern::set_closure (const bool closure_value,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. closed = closure_value ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. closed = closure_value ;
}

void dpps::Pattern::toggle_open_close (const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. closed = !p. closed ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection])
//             p. closed = !p. closed ;
}

void dpps::Pattern::push_back (const Polyline &p) {
    polylines. push_back (p) ;
}

dpps::Polyline dpps::Pattern::pop_back () {
    Polyline result ;
    if (polylines. size () >= 1) {
        result = polylines[polylines. size()-1] ;
        polylines. pop_back () ;
    }
    return result ;
}

void dpps::Pattern::append_from (const Pattern &pattern,
                                 const selection_t selection) {
check_selection_not_too_high(selection) ;
    if (selection < 0)
        polylines. insert (polylines. end (),
                       pattern. polylines. begin (),
                       pattern. polylines. end ()) ;
    else
        for (auto p : pattern. polylines)
            if (p. selected[selection])
                polylines. push_back (p) ;
}

void dpps::Pattern::random_update_property (
                  const enum_random_property property,
                  const enum_distribution random_distribution,
                  const double p1,
                  const double p2,
                  const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::uniform_real_distribution<double> d0 (p1, p2) ; // min, max
    std::uniform_real_distribution<double> d_angle (0, 2*M_PI) ;
    // Normal-type
    std::normal_distribution<double> d1 (p1, p2) ;    // average = p1, sigma
    std::lognormal_distribution<double> d2 (p1, p2) ; // average = p1, m
    std::chi_squared_distribution<double> d3 (p1) ;    // n
    std::cauchy_distribution<double> d4 (p1, p2) ;     // a, b
    std::fisher_f_distribution<double> d5 (p1, p2) ;   // m, n
    std::student_t_distribution<double> d6 (p1) ;      // n
    // Poisson-type
    std::exponential_distribution<double> d7 (p1) ;    // lambda
    std::gamma_distribution<double> d8 (p1, p2) ;      // alpha, beta
    std::weibull_distribution<double> d9 (p1, p2) ;    // a, b

    short unsigned int number_repeat {1} ;
    constexpr const short unsigned int maxproperties {2} ;
    // Here we decide how many numbers we need to acquire depending on the
    // property we want to change.
    // If an added property must acquire many more parametres, do not forget
    // to increase maxproperties accordingly.
    switch (property) {
        case random_property_position_xy:
        case random_property_scale_centred_xy:
            number_repeat = 2 ;
            break ;
        default:
            break ;
    }
    std::array<double, maxproperties> x ;
    constexpr const double two_pi {2*M_PI} ;

    std::for_each (polylines. begin (), polylines. end(),
        [&] (Polyline &p) {
        Polyline q ;
    //for (auto &p : polylines) {
        if ((selection < 0) || (p. selected[selection])) {
            // First we acquire as many pseudo-random numbers as needed.
            for (short unsigned int i {0} ; i < number_repeat ; i++) {
                switch (random_distribution) {
                    case type_uniform_real_distribution: // 0
                        x[i] = d0 (pseudorandom_generator) ;
                        break ;
                    case type_normal_distribution: // 1
                        x[i] = d1 (pseudorandom_generator) ;
                        break ;
                    case type_lognormal_distribution: // 2
                        x[i] = d2 (pseudorandom_generator) ;
                        break ;
                    case type_chi_squared_distribution: // 3
                        x[i] = d3 (pseudorandom_generator) ;
                        break ;
                    case type_cauchy_distribution: // 4
                        x[i] = d4 (pseudorandom_generator) ;
                        break ;
                    case type_fisher_f_distribution: // 5
                        x[i] = d5 (pseudorandom_generator) ;
                        break ;
                    case type_student_t_distribution: // 6
                        x[i] = d6 (pseudorandom_generator) ;
                        break ;
                    case type_exponential_distribution: // 7
                        x[i] = d7 (pseudorandom_generator) ;
                        break ;
                    case type_gamma_distribution: // 8
                        x[i] = d8 (pseudorandom_generator) ;
                        break ;
                    case type_weibull_distribution: // 9
                        x[i] = d9 (pseudorandom_generator) ;
                        break ;
                    default:
                        break ;
                }
            }
            // Then we use these numbers to set a property.
            // Make sure that the property in question acquires enough numbers when
            // adding new properties.
            long_unsigned_int n ;
            double area,
                target_size {1.0} ;
            switch (property) {
                case random_property_position_xy: // 0 bivariate
                { // grouping because of the variable declaration.
                    double angle {d_angle (pseudorandom_generator)} ;
                    p. translate (Vertex (x[0]*cos (angle), x[1]*sin (angle))) ;
                    break ;
                }
                case random_property_position_x: // 1
                    p. translate (Vertex (x[0], 0.0)) ;
                    break ;
                case random_property_position_y: // 2
                    p. translate (Vertex (0.0, x[0])) ;
                    break ;
                case random_property_rotation_centred: // 3
                    p. rotate_centred (x[0]) ;
                    break ;
                case random_property_scale_centred_xy: // 4
                    p. scale_centred (x[0], x[1]) ;
                    break ;
                case random_property_scale_centred_isotropic: // 5
                    p. scale_centred (x[0]) ;
                    break ;
                case random_property_scale_centred_x: // 6
                    p. scale_centred (x[0], 1.0) ;
                    break ;
                case random_property_scale_centred_y: // 7
                    p. scale_centred (1.0, x[0]) ;
                    break ;
                case random_property_replace_polygon_random_vertice_number: // 8
                    n = static_cast<long_unsigned_int> (
                        std::nearbyint (fabs (x[0]))) ;
                    // Area of a regular polygon of n vertices and radius R
                    // (here R = 1/2)
                    // http://www.mathwords.com/a/area_regular_polygon.htm
                    // area = (n/2)*R^2*sin(2pi/n) where
                    area = (1.0 * n) / 8.0 * sin (two_pi / n) ;
                    switch (n) {
                        case 0:
                        case 1:
                            // target_size has no meaning
                            break ;
                        case 2:
                            target_size = p. maximum_distance () ;
                            break ;
                        default:
                            target_size = sqrt (p. area () / area) ;
                            break ;
                    }
                    q. replace_by_regular_polygon (n, target_size, true) ;
                    //FIXME: true ????
                    p. replace_by (q, true) ;
                    //FIXME: true ????
                    break ;
                default:
                    break ;
            }
        }
    //}
    }) ;

}

void dpps::Pattern::round_to (const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &p) {
            if ((selection < 0) || (p. selected[selection]))
                p. round_to (precision) ;
        }) ;
//     for (auto &p : polylines)
//         if (p. selected[selection]) {
//             p. round_to (precision) ;
//         }
}

dpps::Vertex dpps::Pattern::geometrical_centre_polylines (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    Vertex v (0.0, 0.0) ;
    long_unsigned_int i {0} ;
    for (auto p: polylines)
        if ((selection < 0) || (p. selected[selection])) {
            v = v + p. geometrical_centre () ;
            i++ ;
        }
    return (v/i) ;
}

dpps::Vertex dpps::Pattern::geometrical_centre_vertices (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    Vertex v (0.0, 0.0) ;
    long_unsigned_int i {0} ;
//     std::for_each (polylines. begin (), polylines. end(),
//         [&] (Polyline &p) {
//             if (p. selected[selection]) {
//                 v = v + ((p. geometrical_centre ()) * p. size ()) ;
//                 i++ ;
//             }
//         }) ;
    for (auto p: polylines)
        if ((selection < 0) || (p. selected[selection])) {
            if (p. size() > 0) {
                v = v + ((p. geometrical_centre ()) * p. size ()) ;
                i += p.size() ;
            }
        }
    return (v/i) ;
}


dpps::Polyline dpps::Pattern::get_polyline (long_unsigned_int number) {
    if (number < polylines. size ())
        return polylines. at (number) ;
    else {
        std::string reason {"Pattern::get_polyline, asking for a polyline \
beyond end of the vector, as vector size is " +
        std::to_string ((long_unsigned_int) polylines. size ()) + " and polyline required is " +
        std::to_string (number) + "." } ;
        throw bad_parametre (reason. c_str ()) ;
    }
}

void dpps::Pattern::sort1 (
    const enum_double_property property,
    const bool increasing,
    const selection_t selection) {
    std::vector<enum_double_property> p ;
    p. push_back (property) ;
    sort (p, increasing, selection) ;
}

void dpps::Pattern::sort2 (
    const enum_double_property property1,
    const enum_double_property property2,
    const bool increasing,
    const selection_t selection) {
    std::vector<enum_double_property> p ;
    p. push_back (property1) ;
    p. push_back (property2) ;
    sort (p, increasing, selection) ;
}

void dpps::Pattern::sort3 (
    const enum_double_property property1,
    const enum_double_property property2,
    const enum_double_property property3,
    const bool increasing,
    const selection_t selection) {
    std::vector<enum_double_property> property ;
    property. push_back (property1) ;
    property. push_back (property2) ;
    property. push_back (property3) ;
    sort (property, increasing, selection) ;
}

void dpps::Pattern::sort (
    const std::vector <enum_double_property> &properties,
    const bool increasing,
    const selection_t selection) {
    //NOTE: double_property_dose: keep in sync with the maximum value
    // of the enum.

check_selection_not_too_high(selection) ;
    if (properties. size () == 0)
        return ;
    if (properties. size () > double_property_dose+1) {
        std::string reason {"Pattern::sort, the number of sorting criteria \
exceeds the number of available possibilities"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    for (long_unsigned_int i {0} ; i < properties. size () ; i++)
        for (long_unsigned_int j {i+1} ; j < properties. size () ; j++) {
            if (properties[i] == properties[j]) {
                std::string reason {"Pattern::sort, you provided twice the \
same criterion as sorting parametre"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
        }
    Pattern p {Pattern_factory::Pattern_from_selected (*this, selection)} ;
    if (p. size (-1) == 0)
        return ;
    std::stable_sort (
        p. polylines. begin (),
        p. polylines. end (),
        // this will call param_1. multicriteria_less_than(param_2, properties)
        // where param_1 and param_2 are the two arguments passed by stable_sort
        // to the function returned by std::bind.
        // See M. Josuttis (2011), section 10.2.2 Function adapters and binders,
        // pages 491-492, Calling Member Functions.
        std::bind (&Polyline::multicriteria_less_than,
            std::placeholders::_1,
            std::placeholders::_2,
            properties,
            increasing)) ;
     overwrite_selected (p, selection) ;
}

void dpps::Pattern::shuffle (const selection_t selection) {
check_selection_not_too_high(selection) ;
    Pattern p {Pattern_factory::Pattern_from_selected (*this, selection)} ;
    if (size (-1) == 0)
        return ;
    std::shuffle (p. polylines. begin (),
               p. polylines. end (),
               std::default_random_engine(time_seed)) ;
    long_unsigned_int j {0} ;

//     std::for_each (polylines. begin (), polylines. end(),
//         [&] (Polyline &i) {
//             if (i. selected[selection]) {
//                 i = p. polylines [j] ;
//                 j++ ;
//             }
//         }) ;

    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection])) {
            i = p. polylines [j] ;
            j++ ;
        }
}

/*
// overload for swig lua bindings
void dpps::Pattern::sort (
    const std::vector <short unsigned int> properties,
    const selection_t selection) {
    std::vector <enum_double_property> properties_enum ;
    for (auto i: properties)
        properties_enum. push_back (
            static_cast<enum_double_property> (i)) ;
    sort (properties_enum, selection) ;
}
*/

void dpps::Pattern::set_metadata_from (const Polyline &p, const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &i) {
            if ((selection < 0) || (i. selected[selection]))
                i. set_metadata_from (p) ;
        }) ;
}

void dpps::Pattern::distribute (
    const enum_progression_model model,
    const enum_double_property property,
    const long_unsigned_int parametre,
    const selection_t selection) {
check_selection_not_too_high(selection) ;
    double minimum {0.0},
           maximum {0.0} ;
    long_unsigned_int i {0} ;
    long_unsigned_int number_selected {size (selection)} ;
    get_limits_double_property (property, 0, minimum, maximum, selection) ;

//     std::for_each (polylines. begin (), polylines. end(),
//         [&] (Polyline &p) {
//             if (p. selected[selection]) {
//                 double value {dpps::get_model_value (
//                     model, minimum, maximum, number_selected, i)} ;
//                 p. update_double_property (property, parametre, value) ;
//                 i++ ;
//             }
//         }) ;

    for (auto &p: polylines)
        if ((selection < 0) || (p. selected[selection])) {
            double value {dpps::get_model_value (
                model, minimum, maximum, number_selected, i)} ;
            p. update_double_property (property, parametre, value) ;
            i++ ;
        }
}

void dpps::Pattern::limits (
                double &minimum_x,
                double &minimum_y,
                double &maximum_x,
                double &maximum_y,
                const bool dots_as_circles,
                const selection_t selection) const {
check_selection_not_too_high(selection) ;
    if (size (selection) > 0) {
        minimum_x = std::numeric_limits<double>::max () ;
        minimum_y = std::numeric_limits<double>::max () ;
        maximum_x = std::numeric_limits<double>::lowest () ;
        maximum_y = std::numeric_limits<double>::lowest () ;
        for (auto &p : polylines) {
            if ((selection < 0) || (p. selected[selection])) {
                double temp_min_x, temp_min_y, temp_max_x, temp_max_y ;
                if (dots_as_circles && (p.size() == 1)) {
                    temp_min_x = p.vertices[0].x-p.dose;
                    temp_min_y = p.vertices[0].y-p.dose;
                    temp_max_x = p.vertices[0].x+p.dose;
                    temp_max_y = p.vertices[0].y+p.dose;
                } else
                    p.limits (temp_min_x, temp_min_y, temp_max_x, temp_max_y) ;
                if (temp_min_x < minimum_x)
                    minimum_x = temp_min_x ;
                if (temp_min_y < minimum_y)
                    minimum_y = temp_min_y ;
                if (temp_max_x > maximum_x)
                    maximum_x = temp_max_x ;
                if (temp_max_y > maximum_y)
                    maximum_y = temp_max_y ;
            }
        }
    }
}

void dpps::Pattern::get_limits_double_property (
                const enum_double_property property,
                const long_unsigned_int parametre,
                double &minimum,
                double &maximum,
                const selection_t selection) const {
check_selection_not_too_high(selection) ;
    if (size (selection) > 0) {
        minimum = std::numeric_limits<double>::max () ;
        maximum = std::numeric_limits<double>::lowest () ;
        for (auto &p : polylines) {
            double temporary_value ;
            if (((selection < 0) || (p. selected[selection])) &&
                ((temporary_value = p. get_double_property (property, parametre))) < minimum)
                minimum = temporary_value ;
            if (((selection < 0) || (p. selected[selection])) &&
                ((temporary_value = p. get_double_property (property, parametre))) > maximum)
                maximum = temporary_value ;
        }
    }
}

void dpps::Pattern::get_limits_integer_property (
                const enum_integer_property property,
                const long_unsigned_int parametre,
                long_unsigned_int &minimum,
                long_unsigned_int &maximum,
                const selection_t selection) const {
check_selection_not_too_high(selection) ;
    if (size (selection) > 0) {
        minimum = std::numeric_limits<long_unsigned_int>::max () ;
        maximum = std::numeric_limits<long_unsigned_int>::lowest () ;
        for (auto &p : polylines) {
            long_unsigned_int temporary_value ;
            if (((selection < 0) || (p. selected[selection])) &&
                ((temporary_value = p. get_integer_property (property, parametre))) < minimum)
                minimum = temporary_value ;
            if (((selection < 0) || (p. selected[selection])) &&
                ((temporary_value = p. get_integer_property (property, parametre))) > maximum)
                maximum = temporary_value ;
        }
    }
}

double dpps::Pattern::get_minimum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection) const {
    double minimum {0.0},
           maximum {0.0};
    get_limits_double_property (property, parametre, minimum, maximum, selection) ;
    return minimum ;
}

double dpps::Pattern::get_maximum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection) const {
    double minimum {0.0},
           maximum {0.0};
    get_limits_double_property (property, parametre, minimum, maximum, selection) ;
    return maximum ;
}

long_unsigned_int dpps::Pattern::get_minimum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection) const {
    long_unsigned_int minimum {0},
                      maximum {0};
    get_limits_integer_property (property, parametre, minimum, maximum, selection) ;
    return minimum ;
}

long_unsigned_int dpps::Pattern::get_maximum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection) const {
    long_unsigned_int minimum {0},
                      maximum {0};
    get_limits_integer_property (property, parametre, minimum, maximum, selection) ;
    return maximum ;
}

void dpps::Pattern::update_double_property (
        const enum_double_property property,
        const long_unsigned_int parametre,
        const double value,
        const selection_t selection) {
check_selection_not_too_high(selection) ;
    std::for_each (polylines. begin (), polylines. end(),
        [=] (Polyline &i) {
            if ((selection < 0) || i. selected[selection])
            i. update_double_property (property, parametre, value) ;
        }) ;
}

void dpps::Pattern::reverse_order (const selection_t selection) {
check_selection_not_too_high(selection) ;
    Pattern p {Pattern_factory::Pattern_from_selected (*this)} ;
    std::reverse (p. polylines. begin (), p. polylines. end ()) ;
    // replace polylines of a pattern by the new ones
    overwrite_selected (p, selection) ;
}

bool dpps::Pattern::is_convex (const selection_t selection) const {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if (((selection < 0) || (i. selected[selection])) &&
            !i. is_convex ())
            return false ;
     return true ;
}


void dpps::Pattern::remove_aligned_vertices (const double precision,
                                             const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. remove_aligned_vertices (precision) ;
}

void dpps::Pattern::join_successive_intersecting_lines (
        const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    Pattern result ;
    Polyline current ;
    for (long_unsigned_int i{0} ; i < polylines. size ()-1 ; i++) {
        // std::cout << "We are at " << i << " and polyline is " << polylines[i]. display_string () << "\n" ;
        bool selected_i {(selection < 0) || (polylines[i]. selected[selection])} ;
        bool selected_iplus1 {
            // It is always safe to refer to i+1 in this loop, since
            // i < polylines. size ()-1
            ((selection < 0) || (polylines[i+1]. selected[selection]))} ;
        // std::cout << "selected i " << selected_i << " selected next " << selected_iplus1 << "\n" ;
        if (selected_i &&
            selected_iplus1 &&
            polylines[i]. vertices. size() >= 2 &&
            polylines[i+1]. vertices. size() >= 2 &&
            polylines[i]. vertices. back(). equals_to (
                polylines[i+1]. vertices. front(), precision)) {
            if (current. vertices. size () == 0)
                current = polylines[i] ;
            current. vertices. insert (current. vertices. end (),
                polylines[i+1]. vertices. cbegin ()+1,
                polylines[i+1]. vertices. cend ()) ;
            // std::cout << "Adding to polyline, current = " << current. display_string() << "\n" ;
        } else {
            // std::cout << "not okay\n" ;
            if (current. vertices. size () != 0) {
                // std::cout << "We had a polyline " << current. display_string() << "\n" ;
                if (current. vertices. back(). equals_to (
                        current. vertices. front (), precision)) {
                    current. closed = true ;
                    current. vertices. pop_back () ;
                    //erase (current. vertices. end () - 1) ;
                    // std::cout << "closing polyline " << current. display_string() << "\n" ;
                }
                result. push_back (current) ;
                current. vertices. clear () ;
            } else
                result. push_back (polylines[i]) ;
            // When i is at the one before the last, and we cannot join them,
            // then we need to push also the last, otherwise it would ignored,
            // as the for loop never passes through the last
            if (i == polylines. size () - 2)
                result. push_back (polylines[i+1]) ;
        }
    }
    polylines = std::move (result. polylines) ;
}

void dpps::Pattern::join_intersecting_lines (
    const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    Pattern result ;
    Polyline current ;
    //std::cout << "Polylines are: " << display_string (-1) << "\n" ;

    while (polylines. size () > 0) {
        current = polylines. back () ;
        polylines. pop_back () ;
        //std::cout << "Main loop starting with polyline " << current.display_string () << "\n";
        //std::cout << "Polylines to be processed are: \n" << display_string (-1) << "\n" ;
        //std::cout << "Result: \n" << result. display_string (-1) << "\n" ;
        bool selected {(selection < 0) || (current. selected[selection])} ;
        if ((!selected) || current. vertices. size () < 2 || polylines. size () == 0) {
            //std::cout << "No selected, go to next\n" ;
            result. push_back (current) ;
            continue ;
        }
        bool something_was_done {true} ;
        while (something_was_done) {
            //std::cout << "Checking loop\n" ;
            if (polylines. size () == 0) {
                //std::cout << "going away\n" ;
                break ;
            }
            long_unsigned_int i {0} ;
            something_was_done = false ;
            while (!something_was_done && i < polylines. size ()) {
                //std::cout << "Action loop at i=" << i << " probing " << polylines[i]. display_string () << "\n" ;
                //std::cout << "Polylines are: " << display_string (-1) << "\n" ;
                bool selected_i {(selection < 0) || (polylines[i]. selected[selection])} ;
                if (selected_i && (polylines[i]. vertices. size () >= 2)) {
                    if (current. vertices. front (). equals_to (polylines[i]. vertices. front (), precision)) {
                        //std::cout << "case 1 front=front, current= " << current. display_string() << ", probe= " << polylines[i]. display_string () << "\n" ;
                        current. reverse_vertices () ;
                        current. vertices. insert (
                            current. vertices. end (),
                            polylines[i]. vertices. cbegin () + 1,
                            polylines[i]. vertices. cend ()) ;
                        //std::cout << "Now current: " << current. display_string () << "\n" ;
                        something_was_done = true ;
                    } else {
                        if (current. vertices. front (). equals_to (polylines[i]. vertices. back (), precision)) {
                            //std::cout << "case 2 front=back, current= " << current. display_string() << ", probe= " << polylines[i]. display_string () << "\n" ;
                            current. vertices. insert (
                                current. vertices. begin (),
                                polylines[i]. vertices. cbegin (),
                                polylines[i]. vertices. cend () - 1) ;
                            //std::cout << "Now current: " << current. display_string () << "\n" ;
                            something_was_done = true ;
                        } else {
                            if (current. vertices. back (). equals_to (polylines[i]. vertices. back (), precision)) {
                                //std::cout << "case 3 back=back, current= " << current. display_string() << ", probe= " << polylines[i]. display_string () << "\n" ;
                                polylines[i]. reverse_vertices () ;
                                current. vertices. insert (
                                    current. vertices. end (),
                                    polylines[i]. vertices. cbegin () + 1,
                                    polylines[i]. vertices. cend ()) ;
                                //std::cout << "Now current: " << current. display_string () << "\n" ;
                                something_was_done = true ;
                            } else {
                                if (current. vertices. back (). equals_to (polylines[i]. vertices. front (), precision)) {
                                    //std::cout << "case 4 back=front, current= " << current. display_string() << ", probe= " << polylines[i]. display_string () << "\n" ;
                                    current. vertices. insert (
                                        current. vertices. end (),
                                        polylines[i]. vertices. cbegin () + 1,
                                        polylines[i]. vertices. cend ()) ;
                                    //std::cout << "Now current: " << current. display_string () << "\n" ;
                                    something_was_done = true ;
                                }
                            }
                        }
                    }
               }
               if (something_was_done)
                   polylines. erase (polylines. begin () + i) ;
               i++ ;
            }
        }
        result. push_back (current) ;
    }
    polylines = result. polylines ;
}

void dpps::Pattern::subdivide (const double distance, const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. subdivide (distance) ;
}

void dpps::Pattern::reorder_vertices_by_angle(const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. reorder_vertices_by_angle() ;
}

#ifdef GSL_FOUND
void dpps::Pattern::apply_matrix (Matrix3x3 &matrix, const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. apply_matrix(matrix) ;
}

void dpps::Pattern::apply_homography (Matrix3x3 &matrix, const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. apply_homography(matrix) ;
}
#endif

void dpps::Pattern::cycle (long_unsigned_int val,
                           const bool forward, const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. cycle (val, forward) ;
}

void dpps::Pattern::cut_into (Polyline p,
                   const long_unsigned_int pos_this,
                   const long_unsigned_int pos_p,
                   const bool reverse_auto,
                   const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. cut_into (p, pos_this, pos_p, reverse_auto) ;
}

void dpps::Pattern::cut_into (Polyline p,
                   const bool reverse_auto,
                   const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. cut_into (p, reverse_auto) ;
}

void dpps::Pattern::check_closed(const double precision, const selection_t selection) {
check_selection_not_too_high(selection) ;
    for (auto &i : polylines)
        if ((selection < 0) || (i. selected[selection]))
            i. check_closed (precision) ;
}
