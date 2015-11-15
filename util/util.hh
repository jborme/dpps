/**
 * @file util.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Headers for the common functions
 * @date 2014-03-29 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>
#include <string>
#include <algorithm>

#include "common.hh"

#ifndef DPPS_UTIL
#define DPPS_UTIL

namespace dpps {

    typedef enum {
        progression_model_fixed,
        progression_model_linear,
        progression_model_quadratic,
        progression_model_exponential
    } enum_progression_model ;

// https://en.wikipedia.org/wiki/Logical_connective
// https://en.wikipedia.org/wiki/Truth_function#Table_of_binary_truth_functions
    typedef enum {
        logical_operator_false,
        logical_operator_true,
        logical_operator_p,
        logical_operator_not_p,
        logical_operator_q,
        logical_operator_not_q,
        logical_operator_and,
        logical_operator_nand,
        logical_operator_or,
        logical_operator_nor,
        logical_operator_material_nonimplication,
        logical_operator_material_implication,
        logical_operator_converse_nonimplication,
        logical_operator_converse_implication,
        logical_operator_xor,
        logical_operator_xnor
    } enum_logical_operator ;

/** @brief splits a string into a vector of strings according to a delimiter
 *
 * Resulting strings are pushed at the end of the vector passed in argument.
 */
std::vector<std::string> split_string (const std::string &input,
                                       const char delimiter,
                                       const bool ignore_empty) ;
/** @brief Replaces all occurrences of target by replace in text
 *
 * replace must not contain target as a substring, unless the function will
 * loop infinitely and fail. */
void replace_string (std::string &text,
                     const std::string &target,
                     const std::string &replace) ;

//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// Solution from Evan Teran dated 2008-10-20
//
// And this one explains why inline functions are in the header
// http://stackoverflow.com/questions/5057021/why-are-c-inline-functions-in-the-header
inline std::string remove_spaces_left (std::string input) {
    input. erase (input. begin (),
                  std::find_if (input. begin (),
                    input. end (),
                    std::not1 (std::ptr_fun<int, int> (std::isspace)))) ;
    return input ;
}

inline std::string remove_spaces_right (std::string input) {
    input. erase (std::find_if (input. rbegin (), input. rend (),
       std::not1 (std::ptr_fun<int, int> (std::isspace))). base (),
                  input. end ()) ;
    return input ;
}

std::string remove_affix (const std::string &input) ;

void sanitize_EOL (std::string &line) ;

/** @brief returns the string representing a number in a normal representation
 * but, in case it is in decimal form, then the trailing zeros are removed.
 * If then the last character is a point, it can be removed or let shown.
 */
std::string normal_number (const double value,
                          const bool showpoint) ;

/** @brief returns the string representing a number with fixed precision
 * but without extra zeros at the end.
 * If then the last character is a point, it can be removed or let shown.
 *
 * This misses in the C++11 standard, so here we code it.
 */
std::string fixed_number (const double value,
                          const int precision,
                          const bool showpoint) ;
double fixed_number_value (const double value, const int precision) ;

bool logical_operation (const bool p, const bool q,
                         const enum_logical_operator logical_operator) ;

double get_model_value (const enum_progression_model model,
                       const double minimum,
                       const double maximum,
                       const long_unsigned_int number_values,
                       const long_unsigned_int i) ;

std::string comma_separated_string_from_vector (const std::vector<std::string> list) ;

void check_selection_not_negative_not_too_high(selection_t THE_SELECTION) ;
void check_selection_not_too_high(selection_t THE_SELECTION) ;

static constexpr int polyline_max_selection {
    std::numeric_limits<selection_t>::digits} ;

std::string current_date_string () ;

bool remove_from_to (std::string &line, const std::string &from, const std::string &to) ;
long_unsigned_int vector_string_find (const std::vector<std::string> vec, const std::string target) ;

/** @brief downconverts from utf8 to iso8859-1, removing characters which
 * cannot be converted */
std::string utf8toiso8859_1 (const std::string &input) ;

/** @brief returns a string converted from iso8859-1 to utf8. */
std::string iso8859_1toutf8 (const std::string &input) ;

}// namespace dpps

#endif
