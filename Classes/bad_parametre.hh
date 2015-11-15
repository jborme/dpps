/**
 * @file bad_parametre.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Exception trown when main program passed settings parametres with an
 * unexpected length of vector arguments.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Vector arguments are used to pass values to Writer and Pattern derivatives.
 * They are read from user input, which is why there can be problems. Each
 * derivative class defines a series of parametres that it needs. In order to
 * parse these arguments from configuration file and pass them to the class in a
 * generic way, the arguments are grouped by nature: integers, double and
 * strings, and placed in three vectors and passed to the classes. When the
 * length of the vector is different to what the set_parametres() function
 * expects, the exception is thrown and captured by the main program.
 *
 * Note: an alternative way to vectors would be to have set_parametres() as
 * variadic function, but:
 *
 * * vectors are easier to process than C variadic functions (at list with
 *   vectors, things of similar nature are grouped together, while with variadic
 *   there are any more mistakes possible with mixing argument types). Also C++
 *   defines ways to interact with vectors, while argument packs must be
 *   processed manually in recursive functions (with a way to parse their
 *   nature), and this is more error-prone.
 * * C++ variadic functions use templates, which cannot be made virtual, which
 *   exclude them for the use in abstract classes Pattern and Writer.
 * * we would still need to throw exceptions if arguments are not as expected,
 *   just the methods to check the arguments would be more convoluted.
 */

#include "util.hh" // to_string

#include "common.hh"
#include "configure.h"

#ifndef DPPS_BAD_PARAMETER
#define DPPS_BAD_PARAMETER

namespace dpps {
class bad_parametre: public std::exception {
protected:
    std::string reason ;

public:
    /** @brief Constructor with a standard char* for a reason.
     *
     * To call it, use for example:
     * @code
     * std::string reason = "There was a problem" ;
     * throw bad_parametre_number (reason. c_str ()) ;
     * @endcode
     */
    bad_parametre (const char *why): reason (why) {} ;

    /** @brief Constructor with specific arguments that correspond to actual
     * uses in classes Writer and derivatives, Pattern and derivatives. It
     * takes as parametre the name of the function which raised the exception,
     * the number of expected arguments in the three categories (int, double,
     * string), and the number of actual arguments received.
     */
    bad_parametre (std::string name,
                          long_unsigned_int expected_bool,
                          long_unsigned_int expected_int,
                          long_unsigned_int expected_double,
                          long_unsigned_int expected_string,
                          long_unsigned_int obtained_bool,
                          long_unsigned_int obtained_int,
                          long_unsigned_int obtained_double,
                          long_unsigned_int obtained_string): reason (name) {
        reason += " expected " + \
                  std::to_string ((long_unsigned_int)expected_bool)   + " booleans, " + \
                  std::to_string ((long_unsigned_int)expected_int)    + " integers, " + \
                  std::to_string ((long_unsigned_int)expected_double) + " double and " + \
                  std::to_string ((long_unsigned_int)expected_string) + " strings, obtained " + \
                  std::to_string ((long_unsigned_int)obtained_bool)   + ", " + \
                  std::to_string ((long_unsigned_int)obtained_int)    + ", " + \
                  std::to_string ((long_unsigned_int)obtained_double) + " and " + \
                  std::to_string ((long_unsigned_int)obtained_string) + ", respectively." ;
        }

    virtual const char* what () const throw () {
        return reason. c_str () ;
    }
} ;
} // namespace dpps
#endif