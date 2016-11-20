/**
 * @file util.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Common functions
 * @date 2014-03-29 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <string> // getline
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <math.h> // exp
#include "util.hh"
#include "bad_parametre.hh"

std::string dpps::remove_affix (const std::string &input) {
    std::string::size_type pos ;
    pos = input. find_last_of ('.') ;
    if (pos == std::string::npos)
        return input ;
    else {
        std::string result {input} ;
        result. erase (pos) ;
        return result ;
    }
}

std::vector<std::string> dpps::split_string (const std::string &input,
                                             const char delimiter,
                                             const bool ignore_empty) {
    std::stringstream stream_input (input) ;
    std::vector<std::string> result ;
    std::string value ;
    bool test {true};
    while (test) {
        test = std::getline (stream_input, value, delimiter). good() ;
        if ((!ignore_empty) || (!value. empty ()))
            result. push_back (value) ;
    }
    return result ;
}

void dpps::replace_string (std::string &text,
                     const std::string &target,
                     const std::string &replace) {
    std::string::size_type pos ;
    while ((pos = text. find (target)) != std::string::npos) {
        text. erase (pos, target. length ()) ;
        text. insert (pos, replace) ;
    }
//     std::string::size_type pos {0}, pos_previous {0} ;
//     bool stay{true} ;
//     bool first {true} ;
//     std::cout << "text= " << text << ", target = " << target << ", replace = " << replace << "\n" ;
//     while (stay) {
//         pos_previous = pos ;
//         pos = text. find (target) ;
//         std::cout << "previous = " << pos_previous << ", pos = " << pos << "\n" ;
//         stay = (pos != std::string::npos) && (first || (pos > pos_previous + replace. size())) ;
//         if (stay) {
//             first = false ;
// //         std::cout << "We found : **" << target << "** at pos " << pos << "\n" ;
//             text. erase (pos, target. length ()) ;
// //         std::cout << "after erase : " << text << "\n" ;
//             text. insert (pos, replace) ;
//             std::cout << "after insert : " << text << "\n" ;
//         }
//     }
}

void dpps::sanitize_EOL (std::string &line) {
    if (line. length () == 0)
        return ;
    if (static_cast<short unsigned int>(line. back ()) < 32)
        line. pop_back () ;
}

std::string dpps::normal_number (const double value,
                                 const bool showpoint) {
    std::ostringstream converted ;
    converted << value ;
    std::string result {converted. str ()} ;
    // We only remove extra zeros when it is using decimal representation,
    // (no exponent, so only 0-9.-+[space]) and there actually is a point
    if (result. find_first_not_of ("0123456789.-+ ") == std::string::npos) {
        if (result. find ('.') != std::string::npos) {
            while (result. back () == '0')
                result. pop_back () ;
            if ((!showpoint) && (result. back () == '.'))
                result. pop_back () ;
        }
    }
    return result ;
}

std::string dpps::fixed_number (const double value,
                                const int precision,
                                const bool showpoint) {
    std::ostringstream converted ;
    converted << std::fixed << std::setprecision (precision) << value ;
    std::string result {converted. str ()} ;
    if (result. find ('.') != std::string::npos) {
        while (result. back () == '0')
            result. pop_back () ;
        if ((!showpoint) && (result. back () == '.'))
            result. pop_back () ;
    }
    return result ;
}

double dpps::fixed_number_value (const double value, const int precision) {
    std::string number {fixed_number(value, precision, false)} ;
    double result ;
    std::istringstream (number) >> result ;
    return result ;
}

bool dpps::logical_operation (const bool p, const bool q,
                         const enum_logical_operator logical_operator) {
    switch (logical_operator) {
        // Using order from below
        // http://stackoverflow.com/questions/7578068/besides-and-or-not-whats-the-point-of-the-other-logical-operators-in-programmi
        // Names p and q come from:
        // http://en.wikipedia.org/wiki/Truth_table
        // http://en.wikipedia.org/wiki/Logical_connective
        case logical_operator_false:
            // 0 FALSE = Contradiction = 0, null, NOT TRUE
            return false ;
        case logical_operator_true:
            // 1 TRUE = Tautology = 1, NOT FALSE
            return true ;
        case logical_operator_p:
            // 2 X = Proposition X = X
            return p ;
        case logical_operator_not_p:
            return !p ;
        case logical_operator_q:
            // 4 Y = Proposition Y = Y
            return q ;
        case logical_operator_not_q:
            // 5 NOT Y = Negation of Y = !Y
            return !q ;
        case logical_operator_and:
            // 6 X AND Y = Conjunction = NOT (X NAND Y)
            return (p && q) ;
        case logical_operator_nand:
            // 7 X NAND Y = Alternative Denial = NOT (X AND Y), !X OR !Y
            return !(p && q) ;
        case logical_operator_or:
            // 8 X OR Y = Disjunction = NOT (!X AND !Y)
            return (p || q) ;
        case logical_operator_nor:
            // 9 X NOR Y = Joint Denial = NOT (X OR Y), !X AND !Y
            return !(p || q) ;
        case logical_operator_material_nonimplication:
            // 10 X /\supset Y = Material Nonimplication = X AND !Y,
            //                   NOT(!X OR Y), (X XOR Y) AND X, ???
            return (p && !q) ;
        case logical_operator_material_implication:
            // 11 X \supset Y = Material Implication = !X OR Y, NOT(X AND !Y),
            //                                (X XNOR Y) OR X, ???
            return ((!p) || q) ;
        case logical_operator_converse_nonimplication:
            // 12 X /\subset Y = Converse Nonimplication = !X AND Y,
            //                   NOT(X OR !Y), (X XOR Y) AND Y, ???
            return ((!p) && q) ;
        case logical_operator_converse_implication:
            // 13 X \subset Y = Converse Implication = X OR !Y, NOT(!X AND Y),
            //                                (X XNOR Y) OR Y, ???
            return (p || (!q)) ;
        case logical_operator_xor:
            // 14 X XOR Y = Exclusive disjunction = NOT (X IFF Y),
            //                                   NOT (X XNOR Y), X != Y
            return (p != q) ;
        case logical_operator_xnor:
            // 15 X XNOR Y = Biconditional = X IFF Y, NOT (X XOR Y), X == Y
            return (p == q) ;
        default: // not reachable
            return false ;
    }
}

double dpps::get_model_value (const enum_progression_model model,
                       const double minimum,
                       const double maximum,
                       const long_unsigned_int number_values,
                       const long_unsigned_int i) {
    if (i >= number_values)
        return maximum ;
    if (number_values <= 1)
        return minimum ;
    // integers are always positive so no need to check for i < 0
    // We have two points : (0, min) ; (n-1, max)
    switch (model) {
        case progression_model_fixed:
            return minimum ;
            break ;
        case progression_model_linear:
            // we know number_values > 1
            return minimum + i * (maximum - minimum) / (number_values - 1) ;
            break ;
        case progression_model_quadratic:
            // y = a * i^2 + c
            // min=c ; max=a*(n-1)^2+c => a = (max-min)/(n-1)^2
            return minimum + i * i * (maximum - minimum) /
                            ((number_values - 1) * (number_values - 1)) ;
            break ;
        case progression_model_exponential:
            // y = a * exp (i) + c
            // min = a + c ; max = a * e^(n-1)+c
            // a = (maximum - minimum) / (e^(n-1) - 1)
            return (maximum - minimum) / (exp (number_values - 1) - 1)
                            * (exp (i) - 1) + minimum ;
            break ;
        default: // unreachable
            break ;
    }
    // unreachable
    return 0.0 ;
}

std::string dpps::comma_separated_string_from_vector (const std::vector<std::string> list) {
    std::string result ;
    for (long_unsigned_int i{0} ; i < list. size () ; i++) {
        result += list[i] ;
        if (i < list. size () - 1)
            result += "," ;
    }
    return result ;
}

void dpps::check_selection_not_negative_not_too_high(selection_t selection) {
    if ((selection < 0) || (selection >= polyline_max_selection)) {
        std::string reason {"Selection values " +
std::to_string ((long_unsigned_int)selection)
+ " but at this place it is not allowed to be negative or greater than the \
compile-time maximum, " +
        std::to_string ((long_unsigned_int)polyline_max_selection) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
}

void dpps::check_selection_not_too_high(selection_t selection) {
    if (selection >= polyline_max_selection) {
        std::string reason {"Selection values " +
std::to_string ((long_unsigned_int)selection)
+ " but at this place it is not allowed to be greater than the \
compile-time maximum, " + std::to_string ((long_unsigned_int)polyline_max_selection) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
}

std::string dpps::current_date_string () {
    std::chrono::system_clock::time_point tp {
        std::chrono::system_clock::now ()} ;
    std::time_t t {std::chrono::system_clock::to_time_t (tp)} ;
    std::string cur_date_string {std::ctime (&t)} ;
    cur_date_string. pop_back () ;
    return cur_date_string ;
}

bool dpps::remove_from_to (std::string &line, const std::string &from, const std::string &to) {
    std::string::size_type pos1 {line. find (from)} ;
    std::string::size_type pos2 {line. find (to)} ;
    if ((pos1 != std::string::npos) &&
        (pos2 != std::string::npos) &&
        (pos1 < pos2)) {
        line. erase (line.begin()+pos1, line.begin()+pos2+to.size()) ;
//         std::cout << "pos are " << pos1 << " and " << pos2 << "\n" ;
//         std::cout << "We erased part and now have : " << line << "\n" ;
        return true ;
    }
    return false ;
}

long_unsigned_int dpps::vector_string_find (const std::vector<std::string> vec, const std::string target) {
    for (long_unsigned_int j = 0 ; j < vec. size () ; j++)
        if (target == vec[j])
            return j ;
    return std::numeric_limits<long_unsigned_int>::max() ;
}

std::string dpps::iso8859_1toutf8 (const std::string &input) {
    std::string a ;
    for (long_unsigned_int i = 0 ; i < input. size () ; i++) {
        unsigned char ic = static_cast<unsigned char> (input [i]) ;
        if (ic <= 160)
            a += input[i] ;
        if ((ic >= 161) && (ic <= 191)) {
            a += char (194) ;
            a += input[i] ;
        }
        if (ic >= 192) {
            a += char (195) ;
            a += char (ic-64) ;
        }
    }
    return a ;
}

std::string dpps::utf8toiso8859_1 (const std::string &input) {
    std::string a ;
    for (long_unsigned_int i = 0 ; i < input. size () ; i++) {
        unsigned char ic = static_cast<unsigned char> (input[i]);
//         std::cout << "On lit le caractère numéro " << (int) ic << " " << ic << "\n" ;
        if (ic <= 160)
            a += input[i] ;
        if (ic == 194) {
            if (i == input. size()-1) {
                std::string reason {"Stray control character 0xc2 encountered \
at last byte of character string, which is not allowed by the UTF-8 encoding."} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            i++ ;
            a += input[i] ;
//             std::cout << "On ajoute " << (int) ic << " " << input[i] << "\n" ;
        }
        if (ic == 195) {
            if (i == input. size()-1) {
                std::string reason {"Stray control character 0xc2 encountered \
at last byte of character string, which is not allowed by the UTF-8 encoding."} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            i++ ;
            a += input[i] - 64 ;
//           std::cout << "On ajoute " << (int) (ic-64) << " " << input[i]-64 << "\n" ;
        }
        if (ic > 195) {
            std::string reason {"Stray control character (decimal) " +
                std::to_string ((long_unsigned_int) ic) + " encountered, not \
recognized. Only codepoints up to U+00FF are supported."} ;
            throw bad_parametre (reason. c_str ()) ;
        }
    }
    return a ;
}
