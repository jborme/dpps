/**
 * @file Writer_cpp.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Writer derivative that uses the C++ format of this program.
 * @date 2013-03-14 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * */

#ifndef DPPS_WRITER_CPP
#define DPPS_WRITER_CPP

#include "Writer.hh"

namespace dpps {
/** @brief A Writer derivative to print patterns and groups of patterns. It is
 * used to create fonts of type alphabet_iso8859_1_raw in Typeface class).
 *
 * In the case of a font, a character is a pattern, therefore e need a vector or
 * array of patterns (a raw_alphabet). To achieve this, it is possible to call
 * write_Pattern several times. The writer keeps track whether it is the first
 * or a subsequent call of write_Polyline. If a second Pattern is written by
 * the same instance of the Writer_cpp, then a comma is inserted before.
 *
 * To initialize a alphabet_iso8859_1_raw:
 *
 * std::array<std::vector<std::vector<std::pair<double,double>>>, maxchars>
 *
 *
 * Note that the Typeface class will then convert this to
 * alphabet_iso8859_1_polyline:
 * std::array<std::vector<dpps::Polyline>, maxchars>  =
 *
 * {
 *       pair                 pair                      pair
 * { { {0.148112,1.05165},{0.16135,1.04493},{...},{0.11735,1.03696}}, <- stroke
 *    { {0.0967882,0.15403},{0.114746,0.1524},{...},{0.0779081,0.15246}} <- stroke
 *  }, <- raw_character
 *  {{{...},{...}}}, <- raw_character
 *  {{{...},{...}}} <- raw_character
 *  } <- alphabet_iso8859_1_raw
 *
 *
 */
class Writer_cpp: public Writer {
public:
    /** @brief A struct to encapsulates the size metadata */
    struct Writer_cpp_settings: public Writer_settings {
    };
protected:
    Writer_cpp_settings writer_settings ;
    bool first {true} ;
public:
    // Implementation of inherited virtual functions.
    Writer_cpp (const std::string &set_filename, const bool append) ;
    void write_header () ;
    void write_footer () ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;
    void write_Polyline (const Polyline &polyline) ;
    void set_all_parametres () ;
} ;
} // namespace dpps
#endif