/**
 * @file common.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Workaround for mingw32
 * @date 2014-07-17 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * mingw32 lacks certain definitions, as explained at:
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52015
 */

#include <sstream>
#include <iomanip>

#include "common.hh"

#ifdef _GLIBCXX_HAVE_BROKEN_VSWPRINTF
std::string std::to_string (long_unsigned_int value) {
    std::ostringstream converted ;
    converted << value ;
    return converted. str () ;
}
std::string std::to_string (long signed int value) {
    std::ostringstream converted ;
    converted << value ;
    return converted. str () ;
}
std::string std::to_string (double value) {
    std::ostringstream converted ;
    converted << value ;
    return converted. str () ;
}

std::string std::to_string (bool value) {
    std::ostringstream converted ;
    converted << value ;
    return converted. str () ;
}
#endif

