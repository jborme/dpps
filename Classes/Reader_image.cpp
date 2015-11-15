/**
 * @file Reader_Image.cpp
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief A Reader derivative that converts an opened image to
 * horizontal and vertical lines to polylines. Image opening must be done in a
 * derivative.
 * @date 2013-12-13 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ios>

#include "common.hh"
#include "Reader_image.hh"

#include "bad_parametre.hh"

void dpps::Reader_image::set_all_parametres (
    const bool only_this_colour,
    const enum_image_reader_mode mode,
    const long_unsigned_int red,
    const long_unsigned_int green,
    const long_unsigned_int blue,
    const double size) {
    reader_settings. only_this_colour = only_this_colour ;
    reader_settings. mode = mode ;
    reader_settings. colour[0] = red ;
    reader_settings. colour[1] = green ;
    reader_settings. colour[2] = blue ;
    reader_settings. size = size ;
    maxx = 1.0 * image_size_width * reader_settings. size / 2.0 ;
    maxy = 1.0 * image_size_height * reader_settings. size / 2.0 ;
    minx = -maxx ;
    miny = -maxy ;
}

void dpps::Reader_image::set_parametres (
                     const std::vector<bool> &vbool,
                     const std::vector<long_unsigned_int> &vint,
                     const std::vector<double> &vdouble,
                     const std::vector<std::string> &vstring) {
    if ((vbool. size () != 1)   ||
        (vint. size () != 4)    ||
        (vdouble. size () != 1) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Reader_image::set_parametres",
            1, 4, 1, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint[0] > 2) {
        std::string reason {"Reader_image::set_parametres, mode can \
be zero to import x lines, 1 to import y lines and 2 to import as dots, " +
            std::to_string (vint[0]) + " obtained."} ;
            throw bad_parametre (reason. c_str ()) ;
    }

    if ((vint[1] > 255) || (vint[2] > 255) || (vint[3] > 255)) {
        std::string reason {"Reader_image::set_parametres, red, green and \
blue colours for polyline must be between 0 and 255, got " +
            std::to_string (vint[1]) + ", " +
            std::to_string (vint[2]) + " and " +
            std::to_string (vint[3]) + ", respectively."} ;
            throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (vbool. at (0),
                        static_cast<enum_image_reader_mode> (vint. at (0)),
                        vint. at (1), vint. at (2), vint. at (3),
                        vdouble. at (0)) ;
}

bool dpps::Reader_image::colour_matches (
    const std::array<short unsigned int, 3> &colour) const {
    if ((reader_settings. only_this_colour) &&
        (colour == reader_settings. colour)) {
        return true ;
    }
    if ((!reader_settings. only_this_colour) &&
        (colour != reader_settings. colour)) {
        return true ;
    }
    return false ;
}

bool dpps::Reader_image::get_next_block_x (
    const long_unsigned_int row,
    const long_unsigned_int column,
    long_unsigned_int &block_start_column,
    long_unsigned_int &block_end_column) const {

    block_start_column = column ;
    if ((block_start_column >= image_size_width) ||
        (row >= image_size_height))
        return false ;
    if (!colour_matches (image_pixel (block_start_column, row))) {
        // Since we use <= and then the increment, the last step in this loop
        // will be at image_size_width+1, outside the image range. This is
        // ok because we use the short-circuit && operator and check first
        // if we are within acceptable range.
        while ( (block_start_column < image_size_width) &&
                (!colour_matches (image_pixel (block_start_column, row))) ) {
            block_start_column++ ;
        }
        if (block_start_column >= image_size_width) {
            //std::cout << "returning false" << std::endl ;
            // reached EOL before getting a matching pixel
            // returning with block_start_column > image_size_width
            // so caller can determine with the numbers as well that we did
            // not find a block in this row.
            block_end_column = block_start_column ;
            return false ;
        }
        // If we are here then we found a block in the current rown. At the
        // moment block_start_column is right after the start, so we decrement
        // its value to make it match the real start of the block.
        //block_start_column-- ;
    }
    block_end_column = block_start_column ;
    // Same as above: block_end_column can go beyond image_size_width, but
    // this is ok as we check first the range and then use the && operator.
    while ( (block_end_column < image_size_width) &&
            (colour_matches (image_pixel (block_end_column, row))) ) {
        block_end_column++ ;
    }
    // Same as above:
    // We found the end of the block, but we are at the first non-matching
    // point, so the actual end of the block is one position before.
    block_end_column-- ;
    return true ;
}

bool dpps::Reader_image::get_next_block_y (
    const long_unsigned_int row,
    const long_unsigned_int column,
    long_unsigned_int &block_start_row,
    long_unsigned_int &block_end_row) const {

    block_start_row = row ;
    if ((block_start_row >= image_size_height) ||
        (column >= image_size_width))
        return false ;
    if (!colour_matches (image_pixel (column, block_start_row))) {
        while ( (block_start_row < image_size_height) &&
                (!colour_matches (image_pixel (column, block_start_row))) ) {
            block_start_row++ ;
        }
        if (block_start_row >= image_size_height) {
            block_end_row = block_start_row ;
            return false ;
        }
    }
    block_end_row = block_start_row ;
    while ( (block_end_row < image_size_height) &&
            (colour_matches (image_pixel (column, block_end_row))) ) {
        block_end_row++ ;
    }
    block_end_row-- ;
    return true ;
}

bool dpps::Reader_image::acquire_polyline_x (Polyline &p) {
    bool success {false} ;
    long_unsigned_int begin_col {0} ;
    long_unsigned_int end_col   {0} ;
    Vertex v0, v1 ;
    while (!success) {
        // if the current value is beyond line width, this is because last
        // call to get_next_block did not find more matching pixels on this
        // line.
        if (current_col >= image_size_width) {
            current_col = 0 ;
            current_row++ ;
        }
        // If we now would like to analyse beyond image height, then we
        // finished with x.
        if (current_row >= image_size_height) {
            parsed = true ;
            return false ;
        }
        // last two parametres passed by reference.
        success = get_next_block_x (current_row, current_col,
                                    begin_col, end_col) ;
        current_col = end_col + 1 ;
    }
    if (begin_col == end_col) {
        //switch (reader_settings. action_dot) {
        //    case 0:
        //        p. push_back (Vertex (
        //            minx + (1.0*reader_settings. size * begin_col),
        //            maxy - (1.0*reader_settings. size * current_row))) ;
        //        break ;
        //    case 1:
                p. push_back (Vertex (
                    minx + (1.0*reader_settings. size * begin_col),
                    maxy - (1.0*reader_settings. size * current_row))) ;
                p. push_back (Vertex (
                    minx + (1.0*reader_settings. size * (begin_col+0.5)),
                    p. vertices. back (). y)) ;
        //        break ;
        //   case 2:  // nothing to do
        //   default: // already checked
        //       break ;
        //}
    }
    else {
        p. push_back (Vertex (
            minx + (1.0*reader_settings. size * begin_col),
            maxy - (1.0*reader_settings. size * current_row))) ;
        p. push_back (Vertex (
            minx + (1.0*reader_settings. size * end_col),
            p. vertices. back (). y)) ;
    }
    return true ;
}

bool dpps::Reader_image::acquire_polyline_y (Polyline &p) {
    bool success {false} ;
    long_unsigned_int begin_row {0} ;
    long_unsigned_int end_row   {0} ;
    Vertex v0, v1 ;

    while (!success) {
        if (current_row >= image_size_height) {
            current_row = 0 ;
            current_col++ ;
        }
        if (current_col >= image_size_width) {
            parsed = true ;
            return false ;
        }
        success = get_next_block_y (current_row, current_col,
                                    begin_row, end_row) ;
        current_row = end_row + 1 ;
    }
    if (begin_row == end_row) {
        //switch (reader_settings. action_dot) {
        //    case 0:
        //        p. push_back (Vertex (
        //            minx + (1.0*reader_settings. size * current_col),
        //            maxy - (1.0*reader_settings. size * begin_row))) ;
        //        break ;
        //    case 1:
                p. push_back (Vertex (
                    minx + (1.0*reader_settings. size * current_col),
                    maxy - (1.0*reader_settings. size * begin_row))) ;
                p. push_back (Vertex (
                    p. vertices. back (). x,
                    maxy - (1.0*reader_settings. size * (begin_row+0.5)))) ;
        //        break ;
        //    case 2:
        //    default:
        //        break ;
        //}

    }
    else {
        p. push_back (Vertex (
            minx + (1.0*reader_settings. size * current_col),
            maxy - (1.0*reader_settings. size * begin_row))) ;
        p. push_back (Vertex (
            p. vertices. back (). x,
            maxy - (1.0*reader_settings. size * end_row))) ;
    }
    return true ;
}

bool dpps::Reader_image::acquire_dot (Polyline &p) {
    bool success {false} ;
    while (!success) {
        if (current_col >= image_size_width) {
            current_col = 0 ;
            current_row++ ;
        }
        if (current_row >= image_size_height) {
            parsed = true ;
            return false ;
        }
        success = colour_matches (image_pixel (current_col, current_row)) ;
        current_col++ ;
    }
    p. vertices. clear () ;
    p. push_back (Vertex (
        minx + (1.0*reader_settings. size * current_col),
        maxy - (1.0*reader_settings. size * current_row))) ;
    return true ;
}

bool dpps::Reader_image::read_polyline (Polyline &p) {
    if (parsed)
        return false ;
    if ((image_size_width == 0) || (image_size_height == 0)) {
        parsed = true ;
        return false ;
    }
    bool value {false} ; // initialized just so that compiler does not complain
    p. vertices. clear () ;
    switch (reader_settings. mode) {
        case image_reader_import_x_lines:
            value = acquire_polyline_x (p) ;
            break ;
        case image_reader_import_y_lines:
            value = acquire_polyline_y (p) ;
            break ;
        case image_reader_import_dots:
            value = acquire_dot (p) ;
        default: // already filtered above,
            break ;
    }

    // I also considered doing it by rotating the image, instead of
    // duplicating the code of acquire_polyline and get_next_block
    // Rotate an image in set_parametres
    //if (!reader_settings. parse_x) {
    //    QTransform transform ;
    //    transform. rotate (90) ;
    //    image = image. transformed (transform, Qt::FastTransformation) ;
    //}
    // Then here, rotate the resulting polyline.
    //p. rotate (Vertex (0, 0), M_PI/2.0) ;
    return value ;
}
