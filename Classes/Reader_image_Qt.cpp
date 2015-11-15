/**
 * @file Reader_image_Qt.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader_image derivative that uses QImage to read the images from a
 * file.
 * @date 2013-12-13 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "common.hh"
#include "Reader_image_Qt.hh"

dpps::Reader_image_Qt::Reader_image_Qt (const std::string &filename) {
    image. load (QString (filename. c_str ())) ;
    QSize image_size = image. size () ;
    image_size_height = image_size. height () ;
    image_size_width  = image_size. width  () ;
}

dpps::Reader_image_Qt::Reader_image_Qt (
    const std::string &set_filename,
    const bool only_this_colour,
    const enum_image_reader_mode mode,
    const long_unsigned_int red,
    const long_unsigned_int green,
    const long_unsigned_int blue,
    const double size): Reader_image_Qt (set_filename) {
    set_all_parametres (only_this_colour, mode, red, green, blue, size) ;
}

std::array<short unsigned int, 3> dpps::Reader_image_Qt::image_pixel (
        const long_unsigned_int column,
        const long_unsigned_int row) const {
    QColor colour {image. pixel (column, row)} ;
    std::array<short unsigned int, 3> components {{
        static_cast<short unsigned int>(colour. red ()),
        static_cast<short unsigned int>(colour. green ()),
        static_cast<short unsigned int>(colour. blue ())}} ;
    return components ;

}


