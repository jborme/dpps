/**
 * @file Reader_image_png.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader_image derivative that uses libpng to read the images from a
 * file.
 * @date 2014-04-14 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DPPS_READER_IMAGE_PNG
#define DPPS_READER_IMAGE_PNG

#include <cstring> // for strerror, used in png++/png.hpp
#include <png++/png.hpp>

#include "Reader_image.hh"

namespace dpps {
/** @brief This class uses libpng to give access to an image. The
 * logic of he image to polyline conversion is treated in the base class.
 */
class Reader_image_png: public Reader_image {
protected:
    png::image<png::rgb_pixel> image ;
public:
    Reader_image_png (const std::string &filename) ;
    Reader_image_png (
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