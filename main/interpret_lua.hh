/**
 * @file interpret_lua.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief The lua calling interface.
 * @date 2014-04-29 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#ifndef DPPS_INTERPRET_LUA
#define DPPS_INTERPRET_LUA

void read_lua_file (const std::string &configuration_filename) ;

#endif