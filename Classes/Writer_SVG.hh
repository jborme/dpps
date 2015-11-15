/**
 * @file Writer_SVG.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Writer derivative to write a simple SVG file.
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * */

#ifndef DPPS_WRITER_SVG
#define DPPS_WRITER_SVG

#include <array>

#include "Writer.hh"


namespace dpps {
/** @brief A Writer derivative for SVG visualization
 *
 * Polyines are grouped by Pattern. Layers are not implemented, because of
 * the different point of vue as compared to other writers. DXF stores layer
 * as a part of each polyline, while SVG stores it as group of paths. Writing
 * it by groups requires to first order the polyline by their layer. This is
 * possible by defining Pattern as a std::list of polyline instead of a
 * std::deque or std::vector, then implementing comparison operators to allow
 * STL ordering algorithm to be applied. The call to the reordering would have
 * to be done as a Design method (calling Pattern) before calling the Writer
 * to write the polylines of the design. Note that the choice of using Writer
 * classes that support only the writing of polylines (and not group elements
 * or layer tags) is not ver well suited to this objective.
 */
class Writer_SVG: public Writer {
public:
    /** @brief A struct to encapsulates the size metadata */
    struct Writer_SVG_settings: public Writer_settings {
    public:
        /** @brief SVG style applied to the group of the paths. A default
         * value is provided where the structures are drawn in 1 px black lines,
         * not filled.
         */
        std::string style = "fill:none;stroke:#000000;stroke-width:1px;\
stroke-linecap:butt;stroke-linejoin:bevel;stroke-opacity:1" ;

        /** @brief The view box coordinates describe the visible area of the
         * SVG drawing in the order (xmin, ymin, xmax, ymax).
         */
        std::array<double, 4> view_box {{0., 0., 0., 0.}} ;

        /* @brief Whether to use the CAD convention (origin at lower-left,
         * Y increasing to the top) or the Image convention (origin at top-left,
         * Y increasing to the bottom). */
        // Not used right now. To use it, we could need to know what is maxy.
        // Which is not possible here because we just have access to one
        // polyline at a time, not the full Pattern.
        // So to use CAD convention, one has to select polyline, scale with -1
        // in y with respect to the centre, then write.
        //bool use_CAD_convention = true ;

        /*Writer_SVG_settings (
            const std::string set_style = "fill:none;stroke:#000000;
            stroke-width:1px;stroke-linecap:butt;stroke-linejoin:bevel;
            stroke-opacity:1",
            const std::array<double, 4> set_view_box = {0., 0., 0., 0.},
            const bool set_use_CAD_convention = true) ;*/
    };
protected:
    Writer_SVG_settings writer_settings ;
    long_unsigned_int path_id {0} ;
public:
    // Implementation of inherited virtual functions.
    //Writer_SVG () ;
    Writer_SVG (const std::string &set_filename, const bool append) ;
    Writer_SVG (
        const std::string &set_filename,
        const bool append,
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const std::string &style) ;
    void write_header () ;
    void write_footer () ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;
    void write_Polyline (const Polyline &polyline) ;
    void set_all_parametres (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const std::string &style) ;
} ;
} // namespace dpps
#endif