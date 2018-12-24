/**
 * @file Reader_image.hh
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
#ifndef DPPS_READER_IMAGE
#define DPPS_READER_IMAGE

#include <array>

#include "Reader.hh"

namespace dpps {
    typedef enum {
        image_reader_import_x_lines,
        image_reader_import_y_lines,
        image_reader_import_dots,
        image_reader_import_x_rectangles,
        image_reader_import_y_rectangles,
        image_reader_import_squares
    } enum_image_reader_mode ;

/** @brief This is the class used to read images and transform straight
 * horizontal and vertical lines to Polyline. It implements the logic of the
 * transformation, but not the image opening, which is let to derivatives,
 * which currently exist for Qt (QImage) and pnglib. Hence the class
 * Reader_image is abstract.
 *
 * To read x and y, use twice.
 * To use small circles instead of polylines, read then transform the pattern
 * using replace_with
 *
 * This reader does not support assigning colours to doses. Doses are
 * double (with unit), while colours are int, so transform is not obvious.
 *
 * The derived constructor must open the image and set the variables
 * image_size_width and image_size_height. The image_pixel implementation image
 * must return the colour of the pixel.
 */
class Reader_image: public Reader {
public:
    /** @brief A struct to pass encapsulate metadata used by Reader_image */
    struct Reader_image_settings: public Reader_settings {
    public:

        /** @brief Behaviour regarding colours: import the specified colour
         * (true), or all but the specified colour (false). Default is true.
         *
         * The default value of import_colour_behaviour and the default values
         * of colours together define the behaviour where only the black colour
         * is imported, all other colours are ignored.
         *
         * This behaviour is more limitative than a complete layer treatment.
         * However this image format was just defined as a help for easy cases
         * (the ability to expose raster designs was specified early in this
         * project). Indeed image format like XCF supports layers, one might
         * also think of MNG, APNG or GIF as ways to emulate layers behaviours.
         * But for complex cases, the user raster images are just not adequate,
         * the user should switch to vector drawing format like SVG and DXF
         * which support a groups and layers, respectively, and can export to
         * image formats in the end if te user wishes so.
         *
         */
        bool only_this_colour {true} ;

        /** @brief action to be taken, import with x, y or as individual dots.
         */
        enum_image_reader_mode mode {image_reader_import_x_lines} ;

        /** @brief The red, green and blue components used for Polyline,
         * from 0 to 255. Default is (0,0, 0) as default colour is black. */
        std::array<short unsigned int, 3> colour {{0,0,0}} ;

        /** @brief The real size of a single image pixel, that will be used to
         * map pixel positions to absolute positions.
         *
         * Default value is (1 µm, 1 µm). Image will be centred.
         */
        double size {1.0} ;
    };

protected:
    std::vector<Polyline> polylines ;
    Reader_image_settings reader_settings ;

    /*QImage image ;
    QSize image_size ;*/
    long_unsigned_int image_size_height ;
    long_unsigned_int image_size_width ;

    void parse_polylines () ;
    long_unsigned_int last_passed_polyline {0} ;
    bool parsed = false ;

    bool parsed_x = false ;
    bool parsed_y = false ;

    // signed because QSize is
    long_unsigned_int current_row {0};
    long_unsigned_int current_col {0};

    double minx, maxx, miny, maxy ;

    bool get_next_block_x (const long_unsigned_int row,
                           const long_unsigned_int column,
                           long_unsigned_int &block_start_column,
                           long_unsigned_int &block_end_column) const ;

    bool get_next_block_y (const long_unsigned_int row,
                           const long_unsigned_int column,
                           long_unsigned_int &block_start_row,
                           long_unsigned_int &block_end_row) const ;

    bool colour_matches (const std::array<short unsigned int,3> &colour) const ;
    bool acquire_polyline_x (Polyline &p) ;
    bool acquire_polyline_y (Polyline &p) ;
    bool acquire_dot (Polyline &p) ;
public:
    //Reader_image (const std::string &filename) ;

    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;

    bool read_polyline (Polyline &p) ;
    virtual std::array<short unsigned int, 3> image_pixel (
        const long_unsigned_int column,
        const long_unsigned_int row) const = 0 ;
    void set_all_parametres (
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;
} ;
} // namespace dpps
#endif
