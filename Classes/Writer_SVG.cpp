/**
 * @file Writer_SVG.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Writer derivative to write a simple SVG file.
 * @date 2013-12-09 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The file \c SVG_witer.cp contains one derived class to write simple SVG
 * files implementing Polyline elemnt as <Path> with commands M (absolute
 * coordinates) and Z (for closed lines).
 * */

#include "Writer_SVG.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

dpps::Writer_SVG::Writer_SVG (
    const std::string &set_filename,
    const bool append): Writer (set_filename, append) {}

dpps::Writer_SVG::Writer_SVG (
    const std::string &set_filename,
    const bool append,
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const std::string &style):
    Writer_SVG (set_filename, append) {
    set_all_parametres (minx, miny, maxx, maxy, style) ;
    }

void dpps::Writer_SVG::write_header () {
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
         << std::endl << "<svg xmlns:svg=\"http://www.w3.org/2000/svg\""
         << std::endl << "xmlns=\"http://www.w3.org/2000/svg\""
         << std::endl << "version=\"1.1\"" << std::endl << "viewBox=\""
         << writer_settings. view_box [0] << ' '
         << writer_settings. view_box [1] << ' '
         << writer_settings. view_box [2] << ' '
         << writer_settings. view_box [3] << '\"' << std::endl
         << "id=\"svg2\">" << std::endl
         << "<g id = \"Design\" style=\""
         << writer_settings. style << "\">" << std::endl ;
}
void dpps::Writer_SVG::write_footer () {
    file << "</g>" << std::endl << "</svg>" ;
}

void dpps::Writer_SVG::set_all_parametres (
    const double minx,
    const double miny,
    const double maxx,
    const double maxy,
    const std::string &style) {
    //writer_settings. use_CAD_convention = vbool[0] ;
    writer_settings. view_box = {{minx, miny, maxx, maxy}} ;
    writer_settings. style = style ;
}

void dpps::Writer_SVG::set_parametres (
                        const std::vector<bool> &vbool,
                        const std::vector<long_unsigned_int> &vint,
                        const std::vector<double> &vdouble,
                        const std::vector<std::string> &vstring) {
    if ((vbool. size () != 0)   ||
        (vint. size () != 0)    ||
        (vdouble. size () != 4) ||
        (vstring. size () != 1)) {
        throw bad_parametre ("Writer_SVG::set_parametres",
            0, 0, 4, 1,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    set_all_parametres (vdouble. at (0), vdouble. at (1), vdouble. at (2),
        vdouble. at (3), vstring. at (0)) ;
}

void dpps::Writer_SVG::write_Polyline (const Polyline &polyline) {
    file << "<path d=\"M " ;
    // the reference &v is used to avoid copy
    for (auto &v : polyline. vertices)
        file << v. x << "," << v. y << " " ;
    file << (polyline. closed? "Z":"") << "\" id=\"p"
         << path_id << "\"/>" << std::endl ;
    path_id++ ;
}
