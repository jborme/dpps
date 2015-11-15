/**
 * @file bad_math.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Exception thrown for math errors (sqrt, atan2)
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_BAD_MATH
#define DPPS_BAD_MATH

namespace dpps {
class bad_math: public std::exception {
protected:
    std::string reason ;

public:
    /** @brief Constructor with a standard char* for a reason.
     *
     * To call it, use for example:
     * @code
     * std::string reason = "There was a problem" ;
     * throw bad_io (reason. c_str ()) ;
     * @endcode
     */
    bad_math (const char *why): reason (why) {} ;

    virtual const char* what () const throw () {
        return reason. c_str () ;
    }
} ;
} // namespace dpps
#endif