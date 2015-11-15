/**
 * @file Reader_witec.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader derivative that reads the witec files as exported by this
 * program (parser is rather simple so file has to be well-behaved)
 * @date 2014-07-19 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Parser is limited and in particular it is not validating.
 *
 * Comments are C-style, they can appear several times in a line, and they
 * can be multi-line. They can even separate numbers and function names.
 *
 * Format is :
 * function(NUMBER,NUMBER);
 *
 * Spaces are allowed before and after the ufnction name, the opening bracket,
 * the comma, the closing bracket, the semicolon.
 * The parser works by looking to the next delimiter it needs.
 *
 * It looks to a function name by isolating what is before the opening bracket.
 * Therefore the opening bracket must be on the same line as the function name.
 * Parameters are isolated by looking to the next comma, or to the next
 * closing bracket. Then the next semicolon is looked for. Any additional text
 * before the semicolon will therefore be ignored. If a semicolon is forgotten
 * then the next lines are scanned until one is found. Strange results
 * can come out if the file is not valid.
 *
 */
#ifndef DPPS_READER_WITEC
#define DPPS_READER_WITEC

#include "Polyline.hh"
#include "Reader.hh"

namespace dpps {
/** @brief This is the derivative class that implements witec parsing */
class Reader_witec: public Reader {
public:
    /** @brief A struct to pass encapsulate metadata used by Reader_witec */
    struct Reader_witec_settings: public Reader_settings {
    public:
        bool construction_lines {false} ;
    };

protected:
    std::vector<std::string> allowed_functions {{
        "MovingSpeed", "ms",               // 0 1
        "Sleep", "sl",                     // 2 3
        "MoveZMicroscope", "mz",           // 4 5 supported
        "SetTrigger", "st",                // 6 7 supported
        "DisplayMessage", "dm",            // 8 9
        "SetPoint", "sp",                  // 10 11
        "Snapshot", "snap",                // 12 13
        "PushTransformation", "pusht",     // 14 15
        "PopTransformation", "popt",       // 16 17
        "Rotate", "rot",                   // 18 19
        "Scale", "sc",                     // 20 21
        "SetOrigin", "so",                 // 22 23 supported
        "MoveRelative", "mr",              // 24 25 supported
        "MoveAbsolute", "ma",              // 26 27 supported
        "JumpRelative", "jr",              // 28 29 supported
        "JumpAbsolute", "ja",              // 30 31 supported
        "WaitforStablePosition", "wpos",    // 32 33
        "SetOriginSamplePos", "so_sp",     // 34 35 supported
        "MoveRelativeSamplePos", "mr_sp",  // 36 37 supported
        "MoveAbsoluteSamplePos", "ma_sp",  // 38 39 supported
        "JumpRelativeSamplePos", "jr_sp",  // 40 41 supported
        "JumpAbsoluteSamplePos", "ja_sp"}} ; // 42 43 supported
    std::vector<int> parametre_number {{
        1, 1, // 0 1
        1, 1, // 2 3
        1, 1, // 4 5 supported
        1, 1, // 6 7 supported // 3 according to the doc but doe snt make much sense
        0, 0, //1, 1, // 8 9
        1, 1, // 10 11
        2, 2, // 12 13
        0, 0, // 14 15
        0, 0, // 16 17
        3, 3, // 18 18
        3, 3, // 20 21
        3, 3, // 22 23 supported
        3, 3, // 24 25 supported
        3, 3, // 26 27 supported
        3, 3, // 28 29 supported
        3, 3, // 30 31 supported
        0, 0, // 32 33
        0, 0, // 34 35 supported
        2, 2, // 36 37 supported
        2, 2, // 38 39 supported
        2, 2, // 40 41 supported
        2, 2}}; // 42 43 supported ;
    Reader_witec_settings reader_settings ;
   bool firstpolyline {true} ;
    Vertex current_stepper_pos ; //(0,0) ;
    Vertex current_piezo_pos ; //(0,0) ;
    Polyline current_polyline ;
public:
    Reader_witec (const std::string &set_filename) ;
    Reader_witec (const std::string &set_filename,
        const bool construction_lines) ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;
    void set_all_parametres (
        const bool construction_lines) ;

    /** @brief A very simple parser */
    bool read_polyline (Polyline &p) ;
} ;
} // namespace dpps
#endif