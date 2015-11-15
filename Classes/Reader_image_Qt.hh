/**
 * @file Reader_image_Qt.hh
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
#ifndef DPPS_READER_IMAGE_QT
#define DPPS_READER_IMAGE_QT

#include <QtGui/QImage>
#include <QtGui/QColor>

#include "Reader_image.hh"

namespace dpps {
/** @brief This class uses the QImage class to give access to an image. The
 * logic of he image to polyline conversion is treated in the base class.
 */
class Reader_image_Qt: public Reader_image {
protected:
    QImage image ;
public:
    Reader_image_Qt (const std::string &filename) ;

    Reader_image_Qt (
        const std::string &set_filename,
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;

// Implementation of virtual methods of the base class
    std::array<short unsigned int, 3> image_pixel (
        const long_unsigned_int column,
        const long_unsigned_int row) const ;

} ;
} // namespace dpps
#endif