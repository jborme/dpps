/**
 * @file interpret_lua.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A primitive interpreter for lua.
 * @date 2014-04-29 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include <iostream>
#include <string>
#include <fstream>

#include "common.hh"
#include "bad_io.hh"
#include "bad_parametre.hh"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

// The wrapped module is provided in dppsLUA_wrap.cxx,
// created by swig on lua/dpps.i
extern int luaopen_dpps(lua_State* L);
}

void send_error (lua_State *L) {
    std::string reason {lua_tostring (L, -1)} ;
    lua_pop (L, 1);
    lua_close (L) ;
    throw dpps::bad_parametre (reason. c_str ()) ;
}

void read_lua_file (
    const std::string &configuration_filename) {

    lua_State *L = luaL_newstate () ;
    luaL_openlibs (L) ;

    luaL_requiref (L, "dpps", luaopen_dpps, 1) ;
    lua_pop (L, 1) ;

    std::ifstream configuration_file ;
    // We use double backets [[ ]] as string separator so that
    // names with the backslash work. This is useful for certain OSes.
    std::string line {"dofile([[" + configuration_filename + "]])"} ;

    int code = luaL_loadstring (L, line. c_str ()) ;
    if (code)
        send_error (L) ;
    code = lua_pcall (L, 0, 0, 0) ;
    if (code)
        send_error (L) ;
    lua_close (L) ;
}
