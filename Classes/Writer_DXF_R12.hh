/**
 * @file Writer_DXF_R12.hh
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation of class Writer_DXF_R12, a Writer derivative that
 * can write in DXF version R12 revision, supporting only a single layer.
 * @date 2013-12-08 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The limitation to one layer comes from the limited reverse-engineering
 * of the format. Also it does not appear to be a blocker to have just one
 * layer, which is I do not want to spend more time on adding layers. Also
 * several layers make loose genericity as other Writer derivatives do not
 * necessarily support such features (svg has groups but all members should
 * be successive while ekements in layers can appear at any place of the file).
 *
 * For considered uses, one layer is enough, as all produced pattern will
 * be used at the lame lithography level. In the case we want to produce
 * several process levels, they would have to be converted to machine format
 * separately, so there could as well be produced separately (running several
 * times this program with different configuration files) without
 * inconveniences.
 *
 * Limitation to R12 is a positive choice. It is a limitation of the equipment
 * that will read the files produced by this program. Their software does not
 * implement the full DXF format, just a subset, and R12 teh only one that we
 * can be sure will work.
 * */

#ifndef DPPS_WRITER_DXF_R12
#define DPPS_WRITER_DXF_R12

#include <array>

#include "Writer.hh"
#include "Pattern.hh"

namespace dpps {
/** @brief A Writer derivative for DXF R12 with the limitation of one layer.
 *
 * There are some additional protected members, but public interface is just
 * the same as the parent Writer class. Indeed all classes derived from Writer
 * will be used in a general way from the main program without knowledge of
 * what type is the instance (without dynamic cast).
 */
    typedef enum {
        dot_as_dot,
        dot_as_polyline,
        dot_as_circle
    } enum_dot_dxf_export ;
class Writer_DXF_R12: public Writer {
public:
    /** @brief A struct to encapsulates the metadata that goes in the header of
        * the DXF R12 file. */
    struct Writer_DXF_R12_settings: public Writer_settings {
    public:
        enum_dot_dxf_export dot_dxf_export {dot_as_dot} ;
        // bool dot_as_polyline {false} ;
        bool two_point_line_as_polyline {false} ;

        /** @brief The date the file will be stamped with in DXF fields (for
         * both creation and last modification dates), as a Julian date in
         * local time.
         *
         * The date used is the moment set_all_parametres() was called. The
         * date is written to the fifth decimal, which is the precision of
         * about one second. Remember that Julian date does not take into
         * account leap seconds and is not intended for scientifically high
         * accuracy purposes.
         *
         * If user sets a value grater or equal to zero, that value is used.
         * If user sets a value strictly lower than zero, the current Julian
         * date is used. Both situations can be desirable. Stamping the file
         * with current date can be useful to keep track of date of file
         * generation, even when the filesystem metadata are not preserved.
         * Passing a positive value ensures the file will be stamped with
         * the same value when the function is called several times. This allows
         * to produce bit-identical files for testing purposes.
         */
        double julian_date {0.0} ;
        std::string layer_name_parametre {"dpps"} ;
    };
protected:
    //std::array<double, 4> coordinates {{0., 0., 0., 0.}} ;
    bool all_set {false} ;

    Vertex lower_left, upper_right ;

    std::vector<std::string> layer_names ;

    std::vector<int> layer_colours ;
   /** handles have some minimum. I do not know the minimum limit. One file I
    *  tested that contained 3 POLYLINEs had the first polylines and vertices
    *  start at 0x285 == 581.
    *
    * An in-class initializer of a non-static data member requires gcc >= 4.7
    */
    long_unsigned_int handle ;
    //long_unsigned_int max_handle ;

    Writer_DXF_R12_settings writer_settings ;

    void write_dxf_polyline_header (const bool closed,
                                    const long_unsigned_int layer_number) ;

    void write_dxf_vertex (const double x,
                           const double y,
                           const long_unsigned_int layer_number) ;

    void write_dxf_seqend (const long_unsigned_int layer_number) ;
    void write_dxf_point (const double x, const double y,
        const long_unsigned_int layer_number) ;
    void write_dxf_circle (const double x, const double y,
        const double radius,
        const long_unsigned_int layer_number) ;
    void write_dxf_line (
        const double x1, const double y1,
        const double x2, const double y2,
        const long_unsigned_int layer_number) ;
    void write_final_header () ;
public:
    ~Writer_DXF_R12 () ;
    static const long_unsigned_int min_handle {581} ;
    /** @brief The maximum number of digits in DXF numbers. Setting to
      * 9 so that distances up to 1 m can be addressed within 1 nm
      * precision.
      * */
    const int precision {9} ;
    // Implementation of inherited virtual functions.
    // The public interface is the same as the parent class, as we want the
    // main program to handle Writer derivatives generically.

    // Writer_DXF_R12 () ;
    Writer_DXF_R12 (const std::string &set_filename, const bool append) ;
    Writer_DXF_R12 (
        const std::string &set_filename,
        const bool append,
        // const bool dot_as_polyline,
        const bool two_point_line_as_polyline,
        const enum_dot_dxf_export dot_dxf_export,
        const double julian_date,
        const std::string &layer_name_parametre,
        const std::string &layer_colour_parametre) ;
    void write_Pattern (const Pattern &pattern) ;
    void write_Pattern_selected (const Pattern &pattern, const selection_t selection = 0) ;
    void write_footer () ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;
    void write_Polyline (const Polyline &polyline) ;
    void close () ;
    void set_all_parametres (
        //const bool dot_as_polyline,
        const bool two_point_line_as_polyline,
        const enum_dot_dxf_export dot_dxf_export,
        const double julian_date,
        const std::string &layer_name_parametre,
        const std::string &layer_colour_parametre) ;
} ;
} // namespace dpps
#endif
