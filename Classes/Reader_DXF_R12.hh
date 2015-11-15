/**
 * @file Reader_DXF_R12.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader derivative that reads POLYLINE elements from DXF R12 files.
 * @date 2013-12-10 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DPPS_READER_DXF_R12
#define DPPS_READER_DXF_R12

#include <regex>
#include "Reader.hh"

namespace dpps {
//     typedef enum {
//         open_polyline_import,
//         open_polyline_ignore,
//         open_polyline_import_as_closed
//     } enum_open_polyline_action ;

    typedef enum {
        all,
        verbatim,
        comma_separated_verbatim_list,
        regex_ECMAScript,
        regex_basic,
        regex_extended,
        regex_awk,
        regex_grep,
        regex_egrep
    } enum_regex ;

/** @brief This is the derivative class that implements DXF R12 parsing */
class Reader_DXF_R12: public Reader {

public:
    /** @brief A struct to pass encapsulate metadata used by Reader_DXF_R12 */
    struct Reader_DXF_R12_settings: public Reader_settings {
    public:
        /* @brief What to do with open polylines. Default is to import them and
         * process them as they are.
         *
         * 0 : import open polylines
         * 1 : ignore open polylines
         * 2 : import open polylines and treat them as closed
         */
//         enum_open_polyline_action open_polyline_action {open_polyline_import} ;

        /** @brief Case-sensitive vector if layer identifiers to be parsed for
         * POLYLINE elements.
         *
         * Value {"*"} (a vector containing just one string, that string
         * containing just the asterisk), which means all layers are parsed for
         * POLYLINEs.
         *
         * Default value is {"*"}
         */
        std::vector<std::string> layers_to_read {} ;

        /* @brief Boolean value that defines whether the layer names are
         * the representation of a dose (a double value) to apply to the
         * Polylines of the layer (true) or just names.
         *
         * If this option is used then layers in layers_to_read are expected
         * to have a name which can be converted to a double. If the name does
         * not represent a valid double number, it will be converted to 0.0.
         *
         * The interpretation of the dose depends on the writer. For example,
         * for witec_writer, the only one currently implementing doses, the dose
         * for polylines containing a single point is an exposure time in
         * milliseconds and and the dose for polylines containing a least two
         * points is a linear speed in micrometres per second. A polyline with
         * dose 0 will be ignored.
         *
         * Default value is false (just names).
         */
        enum_regex regex_grammar {regex_ECMAScript} ;

        bool set_reference_from_layer {true} ;
        bool include_blocks {false} ;
    };

protected:
    Reader_DXF_R12_settings reader_settings ;
    std::regex expression ;
    bool skipped_header {false} ;
    std::vector<std::string> layers_read ;

    /** @brief normalizes the string, manages the layer array and returns
     * true if we need to skip to next, or false if we keep this polyline.
     *
     * This is a convenience function to de-duplicate code that exists
     * for POLYLINE, LINE and POINT. Could be done witha C macro as well but a
     * function is a bit cleaner and easier to debug.
     */
    bool manage_layer (std::string &string_value) ;
    bool find_next_line (std::istream &scan_file, std::string target) ;
    long_unsigned_int find_next_line_vector (std::istream &scan_file,
                                   std::vector <std::string> vector_target,
                                   bool &ok) ;
    bool get_field_value (std::istream &scan_file,
                             long int &field,
                             std::string &value) ;
    long_unsigned_int manage_reference (
        std::vector<std::string> &layers_read,
        std::string &polyline_layer) ;
public:
    //Reader_DXF_R12 () ;
    Reader_DXF_R12 (const std::string &set_filename) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    // Complex to implement and not so useful to my practical uses
    // so I just do not implement metadata parsing.
    // void analyze_metadata () ;

    /** @brief A very simple DXF parser that will capture polylines from
     * specific layers */
    bool read_polyline (Polyline &p) ;

    Reader_DXF_R12 (
        const std::string &set_filename,
        const bool set_reference_from_layer,
        const bool include_blocks,
        // const enum_open_polyline_action open_polyline_action,
        const enum_regex regex_grammar,
        const std::string &layers_to_read) ;
    void set_all_parametres (
        const bool set_reference_from_layer,
        const bool include_blocks,
        //const enum_open_polyline_action open_polyline_action,
        const enum_regex regex_grammar,
        const std::string &layers_to_read) ;
    std::vector<std::string> get_layers_read () const ;
} ;
} // namespace dpps
#endif