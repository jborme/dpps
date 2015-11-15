/**
 * @file Pattern_factory.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A class that produces polylines.
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <limits>
#include <cmath>

#include "Pattern_factory.hh"

dpps::Pattern dpps::Pattern_factory::explode (
    const Polyline &polyline) {
    Pattern result ;
    Polyline line ;
    if (polyline. size () == 0)
        return result ;
    if (polyline. size () == 1) {
        result. push_back (polyline) ;
        return result ;
    }
    // This loop stops one step before the end
    // We do not use references because of the specific for syntax
    // so we make a Polyline copy everytime.
    for (auto i = polyline. vertices. cbegin () ;
         i < polyline. vertices. cend () - 1 ;
         ++i) {
        line. vertices. clear () ;
        line. push_back (*i) ;
        line. push_back (*(i+1)) ;
        result. push_back (line) ;
    }
    return result ;
}

dpps::Pattern dpps::Pattern_factory::fill_with_lines (
                                            const Polyline &polyline,
                                            const double angle,
                                            const double spacing) {
    //if (!polyline. is_convex ())

    Pattern result ;
    Polyline copied_polyline (polyline) ;
    if (! polyline. closed) {
        result. push_back (copied_polyline) ;
        return result ;
    }
    // the starting point must be the most to the top
    // in the referential of the polyline.
    Vertex polyline_centre {polyline. geometrical_centre ()} ;

    copied_polyline. rotate_centred (-angle) ;
    Vertex ll {copied_polyline. lower_left ()},
           ur {copied_polyline. upper_right ()} ;
    double minx {ll. x},
           miny {ll. y},
           maxx {ur. x},
           maxy {ur. y};
    double current_x ;
    // We place the first polyline located at one spacing below the maximum.
    double current_y {maxy - spacing} ;
    // If we do not pass this condition right from the start, it means the
    // structure is smaller than the vertical spacing. It will not be exposed.
    Vertex v, w1, w2 ;
    // The copy makes sure we get all properties of the initial polyline (dose,
    // reference, selection), even if more properties are later added.
    Polyline current {copied_polyline} ;
    current. vertices. clear () ;
    while (current_y >= miny) {
        minx = std::numeric_limits<double>::max () ;
        maxx = std::numeric_limits<double>::lowest () ;
        for (long_unsigned_int i = 0 ;
             i < copied_polyline. vertices. size () ;
             i++) {
            // We want to know where the vector (w1 -> w2) crosses current_y
            w1 = copied_polyline. vertices. at (i) ;
            w2 = copied_polyline. vertices. at ((i+1) %
                                         (copied_polyline. vertices. size ())) ;
            // if both w1 and w2 are either too high or too low, no intersection
            // for this pair of vertices.
            if ((w1. y > current_y) && (w2. y > current_y)) {
                continue ;
            }
            if ((w1. y < current_y) && (w2. y < current_y)) {
                continue ;
            }
            // In the equality case, we take both ends
            if ((fabs (w1. y - current_y) <
                           std::numeric_limits<double>::epsilon()) &&
                (fabs (w2. y - current_y) <
                           std::numeric_limits<double>::epsilon())) {
                if (w1. x < minx)
                    minx = w1. x ;
                if (w1. x > maxx)
                    maxx = w1. x ;
                if (w1. y < miny)
                    miny = w1. y ;
                if (w1. y > maxy)
                    maxy = w1. y ;
                continue ;
            }
            // At this point there must be a unique intersection between the
            // current_y y-level and the (w1, w2) line segment.
            // The division should always be safe, as equality case in the
            // denominator should have been caught in either of the three if
            // above.
            current_x = w1. x +
                       (w2. x - w1. x) / (w2. y - w1. y) * (current_y - w1. y) ;
            if (current_x < minx)
                minx = current_x ;
            if (current_x > maxx)
                maxx = current_x ;
        }
        // NOTE: that gcc allows cbegin() and cend() here, but clang does not.
        current. vertices. erase (current. vertices. begin (),
                                  current. vertices. end ()) ;
        //current. dose = polyline. dose ;
        v. y = current_y ;
        v. x = minx ;
        current. push_back (v) ;
        v. x = maxx ;
        current. push_back (v) ;
        // We calculated everything in the referential rotated by -angle, so
        // we rotate back.

        current. rotate (polyline_centre, angle) ;
        result. push_back (current) ;
        current_y -= spacing ;
    }
    return result ;
}

dpps::Pattern dpps::Pattern_factory::
            fill_with_lines_first_angle (const Polyline &polyline,
                                         const double spacing) {
    return fill_with_lines (polyline, polyline. orientation_at_vertex (0),
                            spacing) ;
}

dpps::Pattern dpps::Pattern_factory::fill_with_dashes_first_angle (
                    const Polyline &polyline,
                    const double period_longitudinal,
                    const double spacing_transversal,
                    const double duty_cycle,
                    const double phase_initial,
                    const double phase_increment,
                    const enum_partial_dash_policy partial_dash_policy) {
    return fill_with_dashes (polyline, polyline. orientation_at_vertex (0),
                            period_longitudinal, spacing_transversal,
                            duty_cycle, phase_initial, phase_increment,
                            partial_dash_policy) ;
}

static dpps::Pattern decompose_line_in_dashes (
    const dpps::Vertex &v1, /**< @brief the first point of the line */
    const dpps::Vertex &v2, /**< @brief the second point of the line */
    const double period,   /**< @brief the longitudinal period of the dashes */
    const dpps::Vertex &v0,
                /**< @brief one point (not necessarily on the line) through
                 * which the pattern passes */
    const double duty_cycle, /**< @brief the duty cycle of the dash pattern */
    const double phase, /**< @brief the phase delay, normally 0..1 */
    /** @brief policy for partial lines, 0 for not including them,
      *  1 for including, 2 for truncating */
    const short unsigned int partial_dash_policy) {

    dpps::Pattern result ;
    dpps::Polyline p ;
    if (v1 == v2) {
        p. push_back (v1) ;
        result. push_back (p) ;
        return result ;
    }
    // At this point we know v1 != v2

    dpps::Vertex h ;
    dpps::Vertex v3 {v2-v1} ;
    dpps::Vertex u {v3 / v3. norm2 ()} ;
    // u is the unit vector of the line
    bool is_left {false} ;
    if (v0 == v1)
        // One might think in this case is_left and is_right give the same
        // result in this case. Actually, we pass here when the user passed v0
        // equal to v1. If for next line we are given the same v0 point (but v1
        // is one line down), then we would have to go through the calculation
        // of scalar1 and scalar2 below, and in this case is_right would be
        // found. So to ensure consistence of choice between the fisrt line and
        // the following, we set is_right here as well. (If the consistence is
        // not preserved, the first line might have one more dot on the left).
        h = v1 ;
    else {
        if (v2 == v0)
            h = v2 ;
            // is_left = false ; // is already false
        else {
            // https://fr.wikipedia.org/wiki/Projection_orthogonale
            // we calculate the projection of v0 onto (v1 v2)
            // v1 is the reference point and u the unit vector of (v1 v2)
            double d {(v0. x - v1. x) * u. x + (v0. y - v1. y) * u. y} ;
            h = v1 + u * d ;
        }
    }
    h = h - (u * (phase * period)) ;
    is_left = (v3*(h-v1) <= 0) ;
    dpps::Vertex v_start {h} ;
    dpps::Vertex u0 {u * period} ;

    if (is_left) {
        while (v3*(v_start-v1) < 0)
            v_start = v_start + u0 ;
        // v_start contains the first position at the right of v1
    } else { // is_right
        while (v3*(v_start-v1) > 0)
            v_start = v_start - u0 ;
        // v_start contains the first position at the left of v1
        v_start = v_start + u0 ;
        // now v_start contains the first position at the right of v1
    }
    if (fabs (duty_cycle) > std::numeric_limits<double>::epsilon()) {
        // If we have dashes not dots, there is the question of their
        // intersection with the polyline.
        dpps::Vertex end_of_dash {v_start - (u0 *(1-duty_cycle))} ;
        if (v3*(end_of_dash-v1) >= 0) {
            // end of previous dash is at the right of polyline, therefore
            // the dash intercepts the polyline.
            switch (partial_dash_policy) {
                case dpps::partial_dash_policy_ignore:  // ignore dash, nothing to do
                    break;
                case dpps::partial_dash_policy_include:
                    // p is empty at this point but does not cost much to
                    // to clear it anyway, and if at a later point we add
                    // some more things above this line, we are already
                    // sure there will be no problem.
                    p. vertices. clear () ;
                    p. vertices. push_back (v_start-u0) ;
                    p. vertices. push_back (end_of_dash) ;
                    result. push_back (p) ;
                    break ;
                case dpps::partial_dash_policy_fit:
                    p. vertices. clear () ;
                    p. vertices. push_back (v1) ;
                    p. vertices. push_back (end_of_dash) ;
                    result. push_back (p) ;
                    break ;
                default:
                    break ;
            }
        }
    }
    while (v3*(v2-v_start) >= 0) {
        // v2 is at the left of v_start
        dpps::Vertex v_end ;
        bool ignore {false} ;
        if (v3*(v2-(v_start + (u0*duty_cycle)))<=0) {
            // the end of the dash is at the right
            switch (partial_dash_policy) {
                case dpps::partial_dash_policy_ignore:
                    ignore = true ;
                    break ;
                case dpps::partial_dash_policy_include:
                    v_end = v_start+(u * (duty_cycle * period)) ;
                    break ;
                case dpps::partial_dash_policy_fit:
                    v_end = v2 ;
                    break ;
                default:
                    break ;
            }
        } else
            v_end = v_start+(u * (duty_cycle * period)) ;
        if (!ignore) {
            p. vertices. clear () ;
            p. vertices. push_back (v_start) ;
            if (fabs (duty_cycle) > std::numeric_limits<double>::epsilon()) {
                // duty_cycle == 0 <=> single point, here we have a dash
                p. vertices. push_back (v_end) ;
            }
            result. push_back (p) ;
        }
        v_start = v_start + u0 ;
    }
    return result ;
}

dpps::Pattern dpps::Pattern_factory::fill_with_dashes (
                    const Polyline &polyline,
                    const double angle,
                    const double period_longitudinal,
                    const double spacing_transversal,
                    const double duty_cycle,
                    const double phase_initial,
                    const double phase_increment,
                    const enum_partial_dash_policy partial_dash_policy) {
    constexpr const double pi_over_two {M_PI / 2.0} ;
    double phase {phase_initial} ;

    Pattern result ;
    if (polyline. size () == 0)
        return result ;
    if (polyline. size () == 1) {
        result. push_back (polyline) ;
        return result ;
    }
    if (polyline. size () == 2) {
        result = decompose_line_in_dashes (polyline. vertices[0],
                    polyline. vertices[1], period_longitudinal,
                    polyline. vertices[0], duty_cycle,
                    phase, partial_dash_policy) ;
        std::for_each (result. polylines. begin (), result. polylines. end (),
            [&] (Polyline &p) {
                p. set_metadata_from (polyline) ;
            }) ;
//         for (auto &i: result. polylines)
//             i. set_metadata_from (polyline) ;
        return result ;
    }
//     Vertex ll {polyline. lower_left ()},
//            ur {polyline. upper_right ()} ;
//     double minx {ll. x},
//            miny {ll. y},
//            maxx {ur. x},
//            maxy {ur. y};
    Pattern lines {fill_with_lines (
        polyline, angle, spacing_transversal)} ;
    // v0: all dash will be referred to the first vertex of the first line
    Vertex v0 {lines. polylines[0]. vertices[0]} ;
    // v1: the direction in which the lines are spaced.
    Vertex v1 {cos (angle - pi_over_two)*spacing_transversal,
                     sin (angle - pi_over_two)*spacing_transversal} ;
    long_unsigned_int i {0} ;
    for (auto &l : lines. polylines) {
        Vertex v {v0 - (v1 * (1.0*i))} ;
        Pattern m {
            decompose_line_in_dashes (l. vertices[0], l. vertices[1],
                period_longitudinal, v, duty_cycle, phase,
                partial_dash_policy)} ;
        result. polylines. insert (result. polylines. end (),
                                   m. polylines. begin (),
                                   m. polylines. end ()) ;
        phase = fmod (phase + phase_increment, 1.0) ;
        i++ ;
    }
    for (auto &i: result. polylines)
        i. set_metadata_from (polyline) ;
    return result ;
}

dpps::Pattern dpps::Pattern_factory::from_initialization_list (
    shape list) {
//typedef std::vector<std::vector<std::vector<std::pair<double,double>>>> shape
    // We will use the analogy of the data types in Shape.
    Pattern result ;
    Polyline p ;
    Vertex v ;
    // shape is a vector of letters
    for (auto &i : list)
        // i is an individual letter
        for (auto &j : i) {
            // j is a stroke of a letter
            for (auto k : j) {
                // k is each pair
                Vertex v (k. first, k. second) ;
                p. push_back (v) ;
            }
            result. push_back (p) ;
            p. vertices. clear () ;
        }
    return result ;
}

dpps::Pattern dpps::Pattern_factory::Pattern_from_selected (Pattern const &p, const selection_t selection) {
check_selection_not_too_high(selection) ;
    Pattern result ;
    for (auto &i : p. polylines)
        if ((selection < 0) || (i. selected[selection]))
            result. push_back (i) ;
    return result ;
}