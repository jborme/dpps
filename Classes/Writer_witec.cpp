/**
 * @file Writer_witec.cpp
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation of class Writer_witec
 * @date 2013-12-08 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <algorithm>
#include <functional>
// N. Josuttis, The C++ Standard Library 2nd Ed. (2012)
// section 15.2.3 « Header files » p. 752 note 6.
// For performance reasons, avoid including iostream if not really needed
// for cin and cout; initializations present in iostream add overheads to
// the file and buffer operations.
#include <iostream>

#include <limits>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <cstdio>

#include "util.hh"
#include "Vertex.hh"
#include "Writer_witec.hh"
#include "Polyline_factory.hh"
#include "Pattern_factory.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

dpps::Writer_witec::Writer_witec (const std::string &set_filename,
    const bool append): Writer (set_filename, append) {}

dpps::Writer_witec::~Writer_witec() {
    if (open) // set in Writer contructor
        close () ;
    // open will be set to false in Writer::close()
}

void dpps::Writer_witec::close () {
    // Ready to close, we actually write the header of the real file.
    std::chrono::milliseconds total (static_cast<long_unsigned_int>(
        round (1000.0*(jump_duration+move_duration+
                       dot_duration+shutter_duration)))) ;
#ifdef FILENAMES_FOR_WINDOWS
    // For users using drag and drop of the lua program onto
    // the executable, the OS will pass the full path. Because display size
    // on the machine is limited, we need to remove the path unless the
    // informations are not visible.
    std::string::size_type pos {filename. find_last_of ("\\")} ;
    if (pos != std::string::npos)
        filename. erase (filename. begin (), filename. begin () + pos + 1) ;
#endif
    file << string_message << "(\"" << filename
         << ": estimated "
         << std::chrono::duration_cast<std::chrono::hours>(total). count()
         << "h"
         << std::chrono::duration_cast<std::chrono::minutes>(
             total % std::chrono::hours(1)). count ()
         << "min (" ;
    if (jump_number > 0) {
        file << jump_number << " jumps" ;
       if ((move_number > 0) || (dot_number > 0))
           file << ", " ;
    }
    if (move_number > 0) {
       file << move_number << " moves" ;
       if (dot_number > 0)
           file << ", " ;
    }
    if (dot_number > 0)
        file << dot_number << " dots" ;
    file << ")\");\n" ;
    // We finally do not use jump_distance, move_distance, dot_distance.

    if (log. is_open ()) {
        log. close () ;
        if (log. bad()) {
            std::string reason {"Writer_witec: log file " + filename +
                                " could not be closed"} ;
            throw bad_io (reason. c_str ()) ;
        }
    }
    // Writign a stream into another. See:
    // * N. Josuttis, The C++ std. lib. 2nd ed (2012),
    //   section 15.14.3 « Using stream buffers directly », page 846-7.
    //   « This is probably the fastest way to copy files by using
    //     C++ I/O streams »
    // * http://stackoverflow.com/questions/3738233/how-to-copy-binary-data-from-one-stream-to-another
    // * http://stackoverflow.com/questions/7250720/ofstreamoperatorstreambuf-is-a-slow-way-to-copy-a-file
    tmpfile. close () ;
    if (tmpfile. bad()) {
        std::string reason {"Writer_witec: tmpfile " + tmpfilename +
                            " could not be closed"} ;
        throw bad_io (reason. c_str ()) ;
    }
    std::ifstream tmpfile_readonly (tmpfilename, std::ios::in);
    file << tmpfile_readonly. rdbuf() ;
    tmpfile_readonly. close () ;
    if (tmpfile. bad()) {
        std::string reason {"Writer_witec: tmpfile_readonly " + tmpfilename +
                            " could not be closed"} ;
        throw bad_io (reason. c_str ()) ;
    }
    Writer::close() ;
}

void dpps::Writer_witec::write_header () {
    file << "/* " + current_date_string () << " */\n" ;
    if (writer_settings. comment_separate) {
        // It can be open already if the user calls several times write_header()
        if (!log. is_open ()) {
            log_filename = remove_affix (filename) + ".log" ;
            if (log_filename == filename)
                log_filename += "file" ;
            if (append)
                log. open (log_filename, std::ios_base::out|std::ios_base::app) ;
            else
                log. open (log_filename, std::ios_base::out|std::ios_base::trunc) ;
            if (!log. is_open ()) {
                std::string reason {"Log file " + filename +
                                    " could not be open for output"} ;
                throw bad_io (reason. c_str ()) ;
            }
        }
    }
}

void dpps::Writer_witec::emit_comment (const std::string value) {
    if (writer_settings. comment_inline)
        tmpfile << "/* " << value << " */\n" ;
    if (writer_settings. comment_separate)
        log << value << "\n" ;
    if (writer_settings. comment_cerr)
        std::cerr << value << "\n" ;
}

void dpps::Writer_witec::perform_focus_change (double value) {
    if (writer_settings. focus_by_piezo) {
        if (fabs (value) < writer_settings. piezo_threshold_z_correction)
            return ;
        std::string comment {"Changing piezo z from " +
            std::to_string (current_piezo_height) + " µm by " +
            std::to_string (value) + " µm "} ;
        if (writer_settings. absolute_movements) {
            tmpfile << string_piezo_move_absolute << "("
                 << fixed_number (current_stepper_position. x, 4, false) << ","
                 << fixed_number (current_stepper_position. y, 4, false) << ","
                 << fixed_number (current_piezo_height+value, 4, false)
                 << ");\n" ;
            current_piezo_height = fixed_number_value (current_piezo_height+value, 4) ;
        } else {
            tmpfile << string_piezo_move_relative << "(0,0,"
                 << fixed_number (value, 4, false)
                 << ");\n" ;
            current_piezo_height = fixed_number_value (value, 4) ;
        }
        comment += "to " + std::to_string (current_piezo_height) ;
        emit_comment (comment) ;
    }
    else {
        if (fabs (value) < writer_settings. stepper_threshold_z_correction)
            return  ;
        // writer_settings. stepper_z_correction_step
        // was checked to be >= movement_unit > 0
        value = std::round (value / writer_settings. stepper_z_correction_step) *
                writer_settings. stepper_z_correction_step ;
        tmpfile << string_stepper_z_move << "("
             << fixed_number (value, 4, false)
             << ");\n" ;
        std::string comment {"Changing stepper z from " +
            std::to_string (current_stepper_height) + " µm by " +
            std::to_string (value) + " µm "} ;
        current_stepper_height += fixed_number_value (value, 4) ;
        comment += "to " + std::to_string (current_stepper_height) ;
        emit_comment (comment) ;
    }
}

void dpps::Writer_witec::piezo_lithography_on (const Vertex &focus_position,
                                               const bool correct_focus_now) {
    // Boolean configuration values:
    // * We are here when xy_by_piezo is true
    // * We check litho_by_shutter and litho_by_focus (which can't be both true)
    // * We check focus_correction just in the case that focus is not done
    //   by piezo. If done by piezo, piezo_move_to() or piezo_jump_to() take
    //   care of this step.
    // * We do not check focus_by_piezo, this is done in perform_focus_change()
    double height_correction_value {0.0} ;
    if (writer_settings. focus_correction &&
            correct_focus_now &&
            !writer_settings. focus_by_piezo) {
        double ih {interpolate_height (focus_position)} ;
        double cur {current_stepper_height + current_piezo_height} ;
        height_correction_value = ih - cur ;
        std::string comment {"Considering focusing by piezo at position " +
            focus_position. display_string () + " from current altitude " +
            std::to_string (cur) + " µm by " +
            std::to_string (height_correction_value)
            + " µm, towards interpolated altitude " + std::to_string (ih)
            + " µm"} ;
        emit_comment (comment) ;
    }
    if (writer_settings. litho_by_focus && !focused) {
        std::string comment {"Focusing for lithography on, by " +
            std::to_string (-writer_settings. defocus_steps) + " µm"} ;
        emit_comment (comment) ;
        perform_focus_change (height_correction_value
                            - writer_settings. defocus_steps) ;
        focused = true ;
    }
    if (writer_settings. litho_by_shutter) {
        if (writer_settings. focus_correction &&
                !writer_settings. focus_by_piezo)
            perform_focus_change (height_correction_value) ;
        if (!triggered) {
            tmpfile << string_shutter << "(on);\n" ;
            update_duration_shutter () ;
            triggered = true ;
        }
    }
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::stepper_lithography_on (const Vertex &focus_position,
                                                 const bool correct_focus_now) {
    // Boolean configuration values:
    // * We are here when xy_by_piezo is false
    // * We check litho_by_shutter and litho_by_focus (which can't be both true)
    // * We do not check focus_by_piezo, this is done in perform_focus_change()
    double height_correction_value {0.0} ;
    if (writer_settings. focus_correction && correct_focus_now) {
        double ih {interpolate_height (focus_position)} ;
        double cur {current_stepper_height + current_piezo_height} ;
        height_correction_value = ih - cur ;
        std::string comment {"Considering focusing by stepper at position " +
            focus_position. display_string () + " from current altitude " +
            std::to_string (cur) + " µm by " +
            std::to_string (height_correction_value)
            + " µm, towards interpolated altitude " + std::to_string (ih)
            + " µm"} ;
        emit_comment (comment) ;
    }
    if (writer_settings. litho_by_focus && !focused) {
        perform_focus_change (height_correction_value -
                writer_settings. defocus_steps) ;
        focused = true ;
    }
    if (writer_settings. litho_by_shutter) {
        if (writer_settings. focus_correction)
            perform_focus_change (height_correction_value) ;
        if (!triggered) {
            tmpfile << string_shutter << "(on);\n" ;
            update_duration_shutter () ;
            triggered = true ;
        }
    }
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::lithography_off () {
    if (writer_settings. litho_by_shutter && triggered) {
        tmpfile << string_shutter << "(off);\n" ;
        update_duration_shutter () ;
        triggered = false ;
    }
    if (writer_settings. litho_by_focus && focused) {
        std::string comment {"Unfocusing to set lithography off, by " +
            std::to_string (writer_settings. defocus_steps) + " µm"} ;
        emit_comment (comment) ;
        perform_focus_change (writer_settings. defocus_steps) ;
        focused = false ;
    }
}

void dpps::Writer_witec::write_footer () {
    lithography_off () ;
    if (writer_settings. xy_by_piezo) {
        if (writer_settings. absolute_movements) {
            tmpfile << string_piezo_jump_absolute << "(0,0,0);\n" ;
            current_piezo_position. x = 0.0 ;
            current_piezo_position. y = 0.0 ;
            current_piezo_height = 0.0 ;
        } else {
            tmpfile << string_piezo_jump_relative << "("
                << fixed_number (-current_piezo_position. x, 4, false)
                << ","
                << fixed_number (-current_piezo_position. y, 4, false)
                << ","
                << fixed_number (-current_piezo_height, 4, false)
                << ");\n" ;
            current_piezo_position. x -= fixed_number_value (current_piezo_position. x, 4) ;
            current_piezo_position. y -= fixed_number_value (current_piezo_position. y, 4) ;
            current_piezo_height -= fixed_number_value(current_piezo_height, 4) ;
        }
    }
    if (writer_settings. finish_at_zero)
        stepper_jump_to(Vertex (0, 0)) ;
}


void dpps::Writer_witec::set_additional_parametres (
    const bool short_names,
    const bool absolute_movements,
    const bool comment_separate,
    const bool comment_inline,
    const bool comment_cerr,
    const double stepper_xy_relative_step,
    const double piezo_x_range,
    const double piezo_y_range) {
    writer_settings. absolute_movements = absolute_movements ;
    if (short_names) {
        string_speed                 = "ms" ;
        string_stepper_z_move        = "mz" ;
        string_stepper_move_absolute = "ma_sp" ;
        string_stepper_move_relative = "mr_sp" ;
        string_stepper_jump_absolute = "ja_sp" ;
        string_stepper_jump_relative = "jr_sp" ;

        string_piezo_move_absolute   = "ma" ;
        string_piezo_move_relative   = "mr" ;
        string_piezo_jump_absolute   = "ja" ;
        string_piezo_jump_relative   = "jr" ;
        string_set_origin            = "so" ;
        string_stabilize             = "wpos" ;

        string_sleep                 = "sl" ;
        string_shutter               = "st" ;
        string_message               = "dm" ;
        string_snapshot              = "snap" ;
    } else {
        string_speed                 = "MovingSpeed" ;
        string_stepper_z_move        = "MoveZMicroscope" ;
        string_stepper_move_absolute = "MoveAbsoluteSamplePos" ;
        string_stepper_move_relative = "MoveRelativeSamplePos" ;
        string_stepper_jump_absolute = "JumpAbsoluteSamplePos" ;
        string_stepper_jump_relative = "JumpRelativeSamplePos" ;

        string_piezo_move_absolute   = "MoveAbsolute" ;
        string_piezo_move_relative   = "MoveRelative" ;
        string_piezo_jump_absolute   = "JumpAbsolute" ;
        string_piezo_jump_relative   = "JumpRelative" ;
        string_set_origin            = "SetOrigin" ;
        string_stabilize             = "WaitForStablePosition" ;

        string_sleep                 = "Sleep" ;
        string_shutter               = "SetTrigger" ;
        string_message               = "DisplayMessage" ;
        string_snapshot              = "Snapshot" ;
    }
    writer_settings. stepper_xy_relative_step = fabs (stepper_xy_relative_step) ;
    if (writer_settings. stepper_xy_relative_step < movement_unit)
        writer_settings. stepper_xy_relative_step = movement_unit ;
    writer_settings. piezo_x_range = fabs (piezo_x_range) ;
    writer_settings. piezo_y_range = fabs (piezo_y_range) ;
    writer_settings. comment_separate = comment_separate ;
    writer_settings. comment_inline = comment_inline ;
    writer_settings. comment_cerr = comment_cerr ;
}

void dpps::Writer_witec::piezo_stabilize () {
    tmpfile << string_stabilize<< "();\n" ;
}

void dpps::Writer_witec::set_all_parametres (
        const bool finish_at_zero,
        const bool focus_correction,
        const bool litho_by_shutter,
        const bool litho_by_focus,
        const bool xy_by_piezo,
        const bool focus_by_piezo,
        const bool automatic_field_crossing,
        const bool correct_focus_inside_polylines,
        //const enum_alignment alignment,
        const enum_stepper_reorder stepper_reorder,
        const enum_hysteresis_quadrant hysteresis_quadrant,
        const long_unsigned_int take_snapshot_every,
        const double moving_speed,
        const double single_dot_time,
        const double jump_speed,
        const double shutter_response_time,
        const double defocus_steps,
        const double stepper_threshold_z_correction,
        const double stepper_z_correction_step,
        const double piezo_threshold_z_correction,
        const double field_size_x,
        const double field_size_y,
        const Vertex field_crossing) {
    writer_settings. finish_at_zero = finish_at_zero ;
    writer_settings. hysteresis_quadrant = hysteresis_quadrant ;
    writer_settings. moving_speed = moving_speed ;
    writer_settings. single_dot_time = single_dot_time ;
     /*
                      py
                 mxpy 2  pxpy
          | w       3 | 1
       ---v--- mx 4---0---8 px
          |         5 | 7
               mxmy   6  pxmy
                      my

       User can specify hysteresis_quadrant to 1, 2, 3, 4, which correspond
       to the locations 1, 3, 5, 7 in our quadrant nomenclature.
       In the example of hysteresis_quadrant == 1, user intends to set the
       top-left quadrant as correct. Therefore the correct movements are
       8, 1, 2 (+x, +x+y, +y), the opposite movements are 4, 5, 6 (-x, -x-y, -y)
       and the lateral ones are 3, 7 (-x+y, +x-y).
       The notion of correct is arbitrary, what is important is to make all
       movements in always the same direction to avoid hysteresis. Assuming
       we define +x+y as the correct direction, then we decompose the path
       in sequences of +x+y. Then also in sequences of opposite movements, which
       just need to be done in opposite order. The « lateral » movements (+x-y,
       -x+y) are the ones fo which there is no clear solution, we are always
       creating some kind of hysteresis.
     */

    switch (writer_settings. hysteresis_quadrant) {
        case hysteresis_quadrant_pxpy:
            correct  = {{quadrant_px,   quadrant_pxpy, quadrant_py}} ;
            opposite = {{quadrant_mx,   quadrant_mxmy, quadrant_my}} ;
            // there are only two remaining, still we need an array of 3
            lateral  = {{quadrant_mxpy, quadrant_pxmy, quadrant_pxmy}} ;
            break ;
        case hysteresis_quadrant_mxpy:
            correct  = {{quadrant_py,   quadrant_mxpy, quadrant_mx}} ;
            opposite = {{quadrant_my,   quadrant_pxmy, quadrant_px}} ;
            lateral  = {{quadrant_pxpy, quadrant_mxmy, quadrant_mxmy}} ;
            break ;
        case hysteresis_quadrant_mxmy:
            correct  = {{quadrant_mx,   quadrant_mxmy, quadrant_my}} ;
            opposite = {{quadrant_px,   quadrant_pxpy, quadrant_py}} ;
            lateral  = {{quadrant_mxpy, quadrant_pxmy, quadrant_pxmy}} ;
            break ;
        case hysteresis_quadrant_pxmy:
            correct  = {{quadrant_my,   quadrant_pxmy, quadrant_px}} ;
            opposite = {{quadrant_py,   quadrant_mxpy, quadrant_mx}} ;
            lateral  = {{quadrant_pxpy, quadrant_mxmy, quadrant_mxmy}} ;
            break ;
        case hysteresis_no_correction:
        default:
            break ;
    }
    capture_name = filename ;
    std::string::size_type pos {capture_name. find_last_of ('.')} ;
    if (pos != std::string::npos)
        capture_name = capture_name. substr (0, pos) ;
    else
        capture_name = filename ;
    capture_name += "_image" ;

    writer_settings. focus_correction = focus_correction ;
    writer_settings. litho_by_shutter = litho_by_shutter ;
    writer_settings. litho_by_focus = litho_by_focus ;
    writer_settings. defocus_steps = defocus_steps ;
    writer_settings. jump_speed = jump_speed ;
    writer_settings. take_snapshot_every = take_snapshot_every ;

    writer_settings. stepper_threshold_z_correction = fabs (stepper_threshold_z_correction) ;
    if (writer_settings. stepper_threshold_z_correction < movement_unit)
        writer_settings. stepper_threshold_z_correction = movement_unit ;

    writer_settings. stepper_z_correction_step = fabs (stepper_z_correction_step) ;
    if (writer_settings. stepper_z_correction_step < movement_unit)
        writer_settings. stepper_z_correction_step = movement_unit ;

    writer_settings. piezo_threshold_z_correction = fabs (piezo_threshold_z_correction) ;
    if (writer_settings. piezo_threshold_z_correction < movement_unit)
        writer_settings. piezo_threshold_z_correction = movement_unit ;

    if (litho_by_shutter && litho_by_focus) {
        std::string reason {"Writer_witec::set_all_parametres, it is not \
possible to use lithography by focus and shutter simulatenously."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    writer_settings. stepper_reorder = stepper_reorder ;
    writer_settings. shutter_response_time = shutter_response_time ;
    writer_settings. xy_by_piezo = xy_by_piezo ;
    writer_settings. focus_by_piezo = focus_by_piezo ;

    writer_settings. field_size_x = field_size_x ;
    writer_settings. field_size_y = field_size_y ;

    if (field_size_x > writer_settings. piezo_x_range) {
        std::string reason {"Writer_witec::set_all_parametres, field_size_x \
passed to " + std::to_string (field_size_x) + ", which is larger than piezo \
x range, set to " + std::to_string (writer_settings. piezo_x_range) + ". Call \
set_advanced_parametres() to set piezo_x_range."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if (field_size_y > writer_settings. piezo_y_range) {
        std::string reason {"Writer_witec::set_all_parametres, field_size_y \
passed to " + std::to_string (field_size_y) + ", which is larger than piezo \
y range, set to " + std::to_string (writer_settings. piezo_y_range) + ". Call \
set_advanced_parametres() to set piezo_y_range."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    writer_settings. field_crossing = field_crossing ;
    writer_settings. automatic_field_crossing = automatic_field_crossing ;
    writer_settings. correct_focus_inside_polylines =
                     correct_focus_inside_polylines ;

    if (writer_settings. xy_by_piezo)
        writer_settings. stepper_reorder = stepper_reorder_none ;
    //writer_settings. alignment = alignment ;
}

void dpps::Writer_witec::add_focus_point (const Vertex &position,
                                          const double value) {
    for (auto i: writer_settings. focus_position)
        if (i. equals_to (position, movement_unit)) {
            std::string reason {"Writer_witec::add_focus_point, the added \
point has same position of an already existing point within movement \
accuracy"} ;
            throw bad_parametre (reason. c_str ()) ;
        }
    writer_settings. focus_position. push_back (position) ;
    writer_settings. focus_value. push_back (value) ;

}

void dpps::Writer_witec::clear_focus_points () {
    writer_settings. focus_position. clear () ;
    writer_settings. focus_value. clear () ;
}

void dpps::Writer_witec::set_parametres (
                            const std::vector<bool> &vbool,
                            const std::vector<long_unsigned_int> &vint,
                            const std::vector<double> &vdouble,
                            const std::vector<std::string> &vstring) {
    if ((vbool. size () != 13)   ||
        (vint. size () != 3)    ||
        (vdouble. size () != 15) ||
        (vstring. size () != 0)) {
        throw bad_parametre ("Writer_witec::set_parametres",
            12, 3, 15, 0,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
/*    if (vint. at (0) > 3) {
        std::string reason {"Writec_witec::set_parametres: alignment \
can value 0 (no alignment) or: 1 for two point, 2 for 3 point, 3 for 4 point \
alignment, but value provided was" +
        std::to_string (vint. at (0)) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }*/
    if (vint. at (0) > 4) {
        std::string reason {"Writec_witec::set_parametres: correct_hysteresis \
can value 0 (no correction) or 1 to 4 for the four quadrants in trigonometric \
order, but value provided was" +
        std::to_string (vint. at (1)) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    if (vint. at (1) > 2) {
        std::string reason {"Writec_witec::set_parametres: stepper_reorder \
can value 0 (no reorder), 1 for up or 2 for down, but value provided was" +
        std::to_string (vint. at (2)) + "."} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    set_all_parametres (
        vbool. at (0), vbool. at (1), vbool. at (2), vbool. at (3),
        vbool. at (4), vbool. at (5), vbool. at (6), vbool. at (7),
        //static_cast<enum_alignment> (vint. at (0)),
        static_cast<enum_stepper_reorder> (vint. at (0)),
        static_cast<enum_hysteresis_quadrant>(vint. at (1)),
        vint. at (2),
        vdouble. at (0), vdouble. at (1), vdouble. at (2),
        vdouble. at (3), vdouble. at (4), vdouble. at (5), vdouble. at (6),
        vdouble. at (7), vdouble. at (8), vdouble. at (9),
        Vertex(vdouble. at (10), vdouble. at (11))) ;
    set_additional_parametres (vbool. at (8), vbool. at (9), vbool. at (10),
        vbool. at (11), vbool. at (12), vdouble. at (12),
        vdouble. at (13), vdouble. at (14)) ;
}

static long_unsigned_int next_contiguous (
    const dpps::Polyline &p,
    long_unsigned_int pos,
    const std::array<dpps::enum_quadrant, 3> &quadrants) {

    if (p. vertices. size () <= 1)
        return pos ;
    if (pos == p. vertices. size () - 1)
        return pos ;

    dpps::enum_quadrant next_quadrant ;
    do {
        if (pos == p. size () - 1)
            // We reached the last point, so we cannot test what happens at
            // pos+1. Actually if we are here it is because we reached the point
            // with correct quadrant, so we will let it do the pos++ and exit.
            next_quadrant = dpps::quadrant_centre ;
        else
            next_quadrant = p. vertices[pos]. quadrant (p. vertices[pos+1]) ;
        pos++ ;
    } while (
                ((next_quadrant == dpps::quadrant_centre) ||
                 (next_quadrant == quadrants[0]) ||
                 (next_quadrant == quadrants[1]) ||
                 (next_quadrant == quadrants[2]))
               && (pos <= p. size () - 1)
            ) ;
    return (pos-1) ;
}

std::vector<dpps::Polyline> dpps::Writer_witec::
    decompose_polyline_no_hysteresis_paths (const dpps::Polyline &p) {
    std::vector<dpps::Polyline> result ;
    // the sub-polylines will be based on the enclosing one to keep
    // the parametres (dose), except they must be open.
    dpps::Polyline q {p} ;
    q. closed = false ;

    long_unsigned_int current_pos {0} ;
    long_unsigned_int new_pos {0} ;
    while (new_pos < p. size ()-1) {
        new_pos = next_contiguous  (p, current_pos, correct) ;
        if (new_pos != current_pos) {
            q. vertices. clear () ;
            q. vertices. insert (q. vertices. end (),
                                p. vertices. begin () + current_pos,
                                p. vertices. begin () + new_pos + 1) ;
            result. push_back (q) ;
            current_pos = new_pos ;
        }
        new_pos = next_contiguous (p, current_pos, opposite) ;
        if (new_pos != current_pos) {
            q. vertices. clear () ;
            q. vertices. insert (q. vertices. end (),
                                p. vertices. begin () + current_pos,
                                p. vertices. begin () + new_pos + 1) ;
            // For the opposite direction we want to draw them in reverse
            // order so that they actually will be in the correct direction.
            std::reverse (q. vertices. begin (), q. vertices. end ()) ;
            result. push_back (q) ;
            current_pos = new_pos ;
        }
        new_pos = next_contiguous (p, current_pos, lateral) ;
        if (new_pos != current_pos) {
            q. vertices. clear () ;
            q. vertices. insert (q. vertices. end (),
                                p. vertices. begin () + current_pos,
                                p. vertices. begin () + new_pos + 1) ;
            result. push_back (q) ;
            current_pos = new_pos ;
        }
    }
    return result ;
}

void dpps::Writer_witec::get_reference_position_from_pattern (
        const dpps::Pattern &pattern,
        const selection_t selection) {
    if (writer_settings. calculate_reference_point &&
        (writer_settings. hysteresis_quadrant != hysteresis_no_correction)) {
        Vertex ll {pattern. lower_left (selection)},
               ur {pattern. upper_right (selection)} ;
        double minx {ll. x},
               miny {ll. y},
               maxx {ur. x},
               maxy {ur. y};

        switch (writer_settings. hysteresis_quadrant) {
            case hysteresis_quadrant_pxpy:
                reference_position. x = minx ;
                reference_position. y = miny ;
                break ;
            case hysteresis_quadrant_mxpy:
                reference_position. x = maxx ;
                reference_position. y = miny ;
                break ;
            case hysteresis_quadrant_mxmy:
                reference_position. x = maxx ;
                reference_position. y = maxy ;
                break ;
            case hysteresis_quadrant_pxmy:
                reference_position. x = minx ;
                reference_position. y = maxy ;
                break ;
            case hysteresis_no_correction:
            default:
                // already checked
                break ;
        }
    }
}

void dpps::Writer_witec::get_field_number (long signed int &i,
    long signed int &j, const Vertex &destination) const {
    double x = (destination. x - writer_settings. field_crossing. x) /
                    writer_settings. field_size_x ;
    i = static_cast<long signed int>(floor (x)) ;
    //std::string comment {"pos x = " + std::to_string (x) + ", i=" + std::to_string (i)} ;
    //emit_comment (comment) ;
    double y = (destination. y - writer_settings. field_crossing. y) /
                    writer_settings. field_size_y ;
    //comment = "pos y = " + std::to_string (x) + ", j = " + std::to_string (i) ;
    //emit_comment (comment) ;
    j = static_cast<long signed int>(floor (y)) ;
}

dpps::Vertex dpps::Writer_witec::get_field_centre_position (
    const long signed int i,
    const long signed int j) const {
    return Vertex (
        writer_settings. field_crossing. x
                + (1.0*i + 0.5)*writer_settings. field_size_x,
        writer_settings. field_crossing. y
                + (1.0*j + 0.5)*writer_settings. field_size_y) ;
}

bool dpps::Writer_witec::polyline_crosses_field_border (
    const Polyline &p) const {
    double minx {p. minimum_x () - writer_settings. field_crossing. x} ;
    double miny {p. minimum_y () - writer_settings. field_crossing. y} ;
    double maxx {p. maximum_x () - writer_settings. field_crossing. x} ;
    double maxy {p. maximum_y () - writer_settings. field_crossing. y} ;
    Vertex gc = p. geometrical_centre () ;
    double sx = writer_settings. field_size_x *
                floor (gc. x / writer_settings. field_size_x) ;
    double sy = writer_settings. field_size_y *
                floor (gc. y / writer_settings. field_size_y) ;
    minx -= sx ;
    miny -= sy ;
    maxx -= sx ;
    maxy -= sy ;
    // now minx, etc. are in the first field where gc.x and gc.y are positive.
    return ((minx < 0) || (maxx > writer_settings. field_size_x) ||
            (miny < 0) || (maxy > writer_settings. field_size_y)) ;
}

void dpps::Writer_witec::write_Polyline (const Polyline &polyline) {
    if (polyline. size () == 0)
        return ;
    if (writer_settings. focus_correction && polyline. is_self_crossing ()) {
        std::string comment {"WARNING: when focus correction is activated, \
self-crossing polylines are not advised as geometrical_centre() might not \
give the expected result. Self-crossing polyline: " +
        polyline. display_string()} ;
        emit_comment (comment) ;
    }

    if ((writer_settings. take_snapshot_every > 0) &&
        ((move_number + dot_number) %
            writer_settings. take_snapshot_every == 0) /*&& !just_took_snapshot*/) {
        // Note that second argument is set to 1 for colour.
        tmpfile << string_snapshot << "(\"" << capture_name << "\", 1);\n" ;
        //just_took_snapshot = true ;
    }

    // The API we designed (write_Polyline is inherited from base class Writer)
    // imposes passing by const reference but we need to alter it, so we'll
    // make a copy.
    dpps::Polyline p {polyline} ;
/*    if (!alignment_done)
        calculate_alignment_parameters () ;
    switch (writer_settings. alignment) {
        case alignment_none:
            break ;
        case alignment_translation:
        case alignment_isometry:
        case alignment_similarity:
        case alignment_affine:
            p. matrix_transform (h) ;
#ifdef GSL_FOUND
             break ;
         case alignment_homography:
             p. homography_transform (h) ;
#endif
    }*/
    // For the Witec, dose 0 makes no sense (neither the dot time dose, nor the
    // linear speed dose) so we decide to use the default value in this case.
    if (fabs (p. dose) <= std::numeric_limits<double>::epsilon()) {
        if (p. size () == 1)
            p. dose = writer_settings. single_dot_time ;
        else
            p. dose = writer_settings. moving_speed ;
    }
    // To follow the complete outline of closed polyline, we must finish
    // at their first vertex.
    if ((p. closed) && (p. size () >= 3)) {
        // some imported designs might have the end point already duplicated.
        if (p. vertices. front () != p. vertices. back ())
            p. vertices. push_back (p. vertices. front ()) ;
    }

    if (writer_settings. xy_by_piezo) {
        if ((p. horizontal_size () <= writer_settings. field_size_x) &&
            (p. vertical_size () <= writer_settings. field_size_y))
            write_polyline_piezo (p) ;
        else {
            // It is a bit safer to first move the piezo to zero, although
            // in practice it does not really matters since we are going to
            // do the next move_to right after.
            piezo_jump_to (Vertex (0,0)) ;
            write_polyline_stepper (p) ;
        }
    }
    else
        write_polyline_stepper (p) ;
}

void dpps::Writer_witec::write_polyline_piezo (Polyline &p) {
//    bool warning_emitted {false} ;
//    double polyline_size ;
//    // It polyline is too large, we do not draw it.
//     if ((polyline_size = p. horizontal_size ()) > writer_settings. field_size_x) {
//         std::string reason {"cannot write a polyline with the piezo stage
// because it's larger than the specified x field range. Range is set to " +
//     std::to_string (writer_settings. field_size_x) + " µm, polyline x size is "
//     + std::to_string (polyline_size) + " µm and polyline has coordinates: "
//     + p. display_string()} ;
//         emit_comment (reason) ;
//         warning_emitted = true ;
//     }
//     if ((polyline_size = p. horizontal_size ()) > writer_settings. field_size_y) {
//         std::string reason {"cannot write a polyline with the piezo stage
// because it's larger than the specified y field range. Range is set to " +
//     std::to_string (writer_settings. field_size_y) + " µm, polyline y size is "
//     + std::to_string (polyline_size) + " µm and polyline has coordinates: "
//     + p. display_string()} ;
//         emit_comment (reason) ;
//         warning_emitted = true ;
//     }
//     if (warning_emitted)
//         return ;
//
//     // If polyline crosses field borders, we need to move there by stage and
//     // only then expose it by piezo.
//     double minx {p. minimum_x () - writer_settings. field_crossing. x} ;
//     double miny {p. minimum_y () - writer_settings. field_crossing. y} ;
//     double maxx {p. maximum_x () - writer_settings. field_crossing. x} ;
//     double maxy {p. maximum_y () - writer_settings. field_crossing. y} ;
//     Vertex gc = p. geometrical_centre () ;
//     double sx = writer_settings. field_size_x *
//                 floor (gc. x / writer_settings. field_size_x) ;
//     double sy = writer_settings. field_size_y *
//                 floor (gc. y / writer_settings. field_size_y) ;
//     minx -= sx ;
//     miny -= sy ;
//     maxx -= sx ;
//     maxy -= sy ;
//     // now minx, etc. are in the first field where gc.x and gc.y are positive.
//     if ((minx < 0) || (maxx > writer_settings. field_size_x) ||
//         (miny < 0) || (maxy > writer_settings. field_size_y)) {
    std::string comment {"Starting piezo lithography of polyline " +
        p. display_string ()} ;
    emit_comment (comment) ;
    if (polyline_crosses_field_border (p)) {
        lithography_off () ;
        emit_comment ("Polyline crosses a field border : " +
                p. display_string ()) ;
        piezo_jump_to (Vertex (0,0)) ;
        go_stepper_to (p. geometrical_centre ()) ;
    } else {
        // do we need to change field
        long signed int i1 ;
        long signed int j1 ;
        long signed int i2 ;
        long signed int j2 ;
        get_field_number (i1, j1, current_stepper_position) ;
        get_field_number (i2, j2, p. geometrical_centre ()) ;
        //std::string comment {"This polyline uses field (" + std::to_string_int (i1)
        //    + ", " + std::to_string_int (i2) + ")"} ;
        //emit_comment (comment) ;
        if ((i1 != i2) || (j1 != j2)) {
            emit_comment ("Going to field (" + std::to_string (i2) + ", "
                + std::to_string (j2) + ")") ;
            piezo_jump_to (Vertex (0,0)) ;
            go_stepper_to (get_field_centre_position(i2, j2)) ;
        }
    }

    // Now we expose the polyline with the piezo.
    // Polyline which are closed were already appended their final dot
    if (p. size () == 1) {
        piezo_jump_to (p. vertices [0]-current_stepper_position) ;
        piezo_stabilize () ;
        piezo_lithography_on (p. vertices [0]-current_stepper_position, true) ;
        sleep (p. dose) ;
        lithography_off () ;
        return ;
    }
    piezo_jump_to (p. vertices[0]-current_stepper_position) ;
    piezo_stabilize () ;
    for (long_unsigned_int i = 1 ; i < p. vertices. size () ; i++) {
        piezo_lithography_on ((current_piezo_position + p. vertices[i]) /2.0,
            writer_settings. correct_focus_inside_polylines || (i == 1)) ;
        piezo_move_to(p. vertices[i]-current_stepper_position, p. dose) ;
    }
    lithography_off() ;
}

void dpps::Writer_witec::go_stepper_to (const Vertex &position) {
    if (position == current_stepper_position)
        return ;
    if ((current_hysteresis_quadrant != writer_settings. hysteresis_quadrant) &&
        (writer_settings. hysteresis_quadrant == hysteresis_no_correction)) {
        current_hysteresis_quadrant = writer_settings. hysteresis_quadrant ;
        status_hysteresis_ok = true ;
    }
    if ((current_hysteresis_quadrant != writer_settings. hysteresis_quadrant) &&
        (writer_settings. hysteresis_quadrant != hysteresis_no_correction)) {
        current_hysteresis_quadrant = writer_settings. hysteresis_quadrant ;
        stepper_jump_to(reference_position) ;
        status_hysteresis_ok = true ;
    }
    if (writer_settings. hysteresis_quadrant != hysteresis_no_correction) {
        enum_quadrant next_quadrant {current_stepper_position. quadrant (position)} ;
        if ( ((next_quadrant != quadrant_centre) &&
                (next_quadrant != correct[0]) &&
                (next_quadrant != correct[1]) &&
                (next_quadrant != correct[2])) ||
                (!status_hysteresis_ok)) {
            lithography_off () ;
            stepper_jump_to(reference_position) ;
            status_hysteresis_ok = true ;
        }
    }
    stepper_jump_to (position) ;
}

void dpps::Writer_witec::write_polyline_stepper (Polyline &p) {
    if (p. size () == 1) {
        // Single dots
        std::string comment {"Will do single dot at " + p. display_string () +
            " for duration " + std::to_string (p. dose) + " ms"} ;
        emit_comment (comment) ;
        go_stepper_to (p. vertices [0]) ;
        stepper_lithography_on (p. vertices [0], true) ;
        sleep (p. dose) ;
        lithography_off () ;
        return ;
    } else {
        std::string comment {"Will write polyline " + p. display_string ()} ;
        emit_comment (comment) ;
    }

    std::vector<Polyline> vector_polylines ;
    // We know there are no single dots below.
    if (writer_settings. hysteresis_quadrant ==
                            hysteresis_no_correction)
        vector_polylines. push_back (p) ;
    else
        vector_polylines = decompose_polyline_no_hysteresis_paths (p) ;
    for (long_unsigned_int i = 0 ; i < vector_polylines. size () ; i++) {
        Polyline q {vector_polylines[i]} ;
        // Everytime we get to a new polyline, we check if there is a need
        // to compensate hysteresis. The cases are:
        // - if we need to move, or
        // - if we know we need to compensate for hysteresis
        go_stepper_to (q. vertices [0]) ;
        // Now we can do this whole polyline without thinking of hysteresis
        // problems.
        // Skipping the first dot, we already moved there.
        for (long_unsigned_int i = 1 ; i < q. vertices. size () ; i++) {
            stepper_lithography_on ((current_stepper_position + q. vertices[i])/2.0,
                writer_settings. correct_focus_inside_polylines || (i == 1)) ;
            stepper_move_to(q. vertices[i], p. dose) ;
            if ((writer_settings. hysteresis_quadrant !=
                                    hysteresis_no_correction) &&
                 status_hysteresis_ok) {
                enum_quadrant next_quadrant {
                    current_stepper_position. quadrant (q. vertices[i])} ;
                if ((next_quadrant != quadrant_centre) &&
                    (next_quadrant != correct[0]) &&
                    (next_quadrant != correct[1]) &&
                    (next_quadrant != correct[2])) {
                    // We moved in an incorrect direction, so we will need
                    // to compensate hysteresis at next polyline.
                    // We do not compensate now, as we already decomposed in
                    // sub-polylines as good as possible.
                    status_hysteresis_ok = false ;
                }
            }
        }
    }
    lithography_off () ;
}

bool dpps::Writer_witec::compare_height (
    const Polyline &p,
    const Polyline &q) { // const
    double fz {interpolate_height (p. geometrical_centre ())} ;
    double gz {interpolate_height (q. geometrical_centre ())} ;
    if ( ((writer_settings. stepper_reorder == stepper_reorder_zup_x_y) ||
          (writer_settings. stepper_reorder == stepper_reorder_zup_y_x)) &&
          (fz < gz))
        return true ;
    if ( ((writer_settings. stepper_reorder == stepper_reorder_zdown_x_y) ||
          (writer_settings. stepper_reorder == stepper_reorder_zdown_y_x)) &&
          (fz > gz))
        return true ;
    return false ;
}

bool dpps::Writer_witec::compare_x_y (const Vertex &v, const Vertex &w) const {
    switch (writer_settings. hysteresis_quadrant) {
        case hysteresis_no_correction:
        case hysteresis_quadrant_pxpy:
            return ((v. x < w. x) || ((v. x >= w. x) && (v. y < w. y))) ;
        case hysteresis_quadrant_mxpy:
            return ((v. x > w. x) || ((v. x <= w. x) && (v. y < w. y))) ;
        case hysteresis_quadrant_pxmy:
            return ((v. x < w. x) || ((v. x >= w. x) && (v. y > w. y))) ;
        case hysteresis_quadrant_mxmy:
            return ((v. x > w. x) || ((v. x <= w. x) && (v. y > w. y))) ;
        default:
            return false ;
    }
}

bool dpps::Writer_witec::compare_x_y_field (const Polyline &p,
                                            const Polyline &q) const {
    long signed int ip ;
    long signed int jp ;
    long signed int iq ;
    long signed int jq ;
    Vertex pgc {p. geometrical_centre()} ;
    Vertex qgc {q. geometrical_centre()} ;
    get_field_number (ip, jp, pgc) ;
    get_field_number (iq, jq, qgc) ;
    double phz {p. horizontal_size ()} ;
    double qhz {q. horizontal_size ()} ;
    double pvz {p. vertical_size ()} ;
    double qvz {q. vertical_size ()} ;

    // To sort by incresing order, this function must work like: return (p < q)
    // We then order by (smallest to largest)
    // 1  polylines that fit within the same field
    // 2. polylines that each fit in one field, but the fields are different
    // 3. polylines that cross borders of a field
    // 4. polylines that are larger than the field.
    // When comparing polylines from differen categories, category 1 is smaller
    // than category 2, etc.
    // When comparing two polylines from same category, positions of geometrical
    // centres are compared
    // We return a value by first exluding the "large" options. This means we
    // first compare for the larger options (4 then 3 then 2 then 1).
    // When a polyline is larger, function returns false. More tests are done
    // to exclude other "larger" possibilities and only in the end we can
    // perform the test to compare centres of polylines that both fit in the
    // same write field.

    // 4. If exacly one polyline is larger the field, this one is largest.
    if ( ((phz >  writer_settings. field_size_x) || (pvz >  writer_settings. field_size_y)) &&
         ((qhz <= writer_settings. field_size_x) || (qvz <= writer_settings. field_size_y)))
        return false ;
    if ( ((qhz >  writer_settings. field_size_x) || (qvz >  writer_settings. field_size_y)) &&
         ((phz <= writer_settings. field_size_x) || (pvz <= writer_settings. field_size_y)))
        return true ;

    // When the two polylines are larger than the field, they are classified by centre
    if ( ((qhz > writer_settings. field_size_x) || (qvz > writer_settings. field_size_y)) &&
         ((phz > writer_settings. field_size_x) || (pvz > writer_settings. field_size_y)))
        return compare_x_y (pgc, qgc) ;

    // 3. when one crosses a border, it is larger than one that does not cross.
    bool pcrosses {polyline_crosses_field_border (p)} ;
    bool qcrosses {polyline_crosses_field_border (q)} ;
    if ((pcrosses && !qcrosses) || (!pcrosses && qcrosses))
        // if q crosses, p does not, hence p is smaller and we return true
        return qcrosses ;
    if (pcrosses && qcrosses)
        return compare_x_y (pgc, qgc) ;

    // 2. we know they fit inside a field, we test if they fit in different fields,
    // and in this case we compare the positions of the field centres.
    if ((ip != iq) || (jp != jq))
        return compare_x_y (get_field_centre_position (ip, jp),
                            get_field_centre_position (iq, jq)) ;

    // 1. we know they fit inside the same field, we compare geometrical centres
    return compare_x_y (pgc, qgc) ;
}

bool dpps::Writer_witec::compare_height_x_y (
    const Polyline &p,
    const Polyline &q) { // const
    Vertex f {p. geometrical_centre ()} ;
    Vertex g {q. geometrical_centre ()} ;
    double fz {interpolate_height (f)} ;
    double gz {interpolate_height (g)} ;

    // We want to compare vertical movement to the precision of z movements,
    // with reference to the reference_altitude (normally the lowest point),
    // because sample will be assumed to be initially focused at this place.
    // This is true in the hypothesis that the polyline located at the extremum
    // position is small enough so that the
    fz = round ((fz - extremum_altitude)/ writer_settings. stepper_z_correction_step)
            * writer_settings. stepper_z_correction_step ;
    gz = round ((gz - extremum_altitude)/ writer_settings. stepper_z_correction_step)
            * writer_settings. stepper_z_correction_step ;

    // The first comparison criterion is the interpolated z height.
    if ( ((writer_settings. stepper_reorder == stepper_reorder_zup_x_y) ||
          (writer_settings. stepper_reorder == stepper_reorder_zup_y_x)) &&
          (fabs (fz - gz) > movement_unit)) {
        return (fz < gz) ;
    }

    if ( ((writer_settings. stepper_reorder == stepper_reorder_zdown_x_y) ||
          (writer_settings. stepper_reorder == stepper_reorder_zdown_y_x)) &&
          (fabs (fz - gz) > movement_unit)) {
        return (fz > gz) ;
    }

    // If we are here, we know fz and gz are equal within stepper_z_correction_step,
    // so we'll check the other criteria.

    // If the second criterion is y, we first compare y within
    // mechanical precision then compare x with full double precision.
    if ((writer_settings. stepper_reorder == stepper_reorder_zup_y_x) ||
        (writer_settings. stepper_reorder == stepper_reorder_zdown_y_x)) {
        if ( (writer_settings. hysteresis_quadrant == hysteresis_no_correction ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxpy ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxpy) &&
            (fabs (f. y - g. y) > movement_unit)) {
            return (f. y < g. y) ;
        }
        if ( (writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxmy ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxmy) &&
             (fabs (f. y - g. y) > movement_unit)) {
            return (f. y > g. y) ;
        }

        if (writer_settings. hysteresis_quadrant == hysteresis_no_correction ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxpy ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxmy) {
            return (f. x < g. x) ;
        }
        if (writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxpy ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxmy) {
            return (f. x > g. x) ;
        }
//         return false ;
    }

    // If we are here we know the second reorder criterion is x, but for
    // symmetry we'll check it anyway.

    if ((writer_settings. stepper_reorder == stepper_reorder_zup_x_y) ||
        (writer_settings. stepper_reorder == stepper_reorder_zdown_x_y)) {
        if ( (writer_settings. hysteresis_quadrant == hysteresis_no_correction ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxpy ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxmy) &&
             (fabs (f. x - g. x) > movement_unit)) {
            return (f. x < g. x) ;
        }
        if ( (writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxpy ||
              writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxmy) &&
              (fabs (f. x - g. x) > movement_unit)) {
            return (f. x > g. x) ;
        }

        if (writer_settings. hysteresis_quadrant == hysteresis_no_correction ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxpy ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxpy) {
            return (f. y < g. y) ;
        }
        if (writer_settings. hysteresis_quadrant == hysteresis_quadrant_pxmy ||
            writer_settings. hysteresis_quadrant == hysteresis_quadrant_mxmy) {
            return (f. y > g. y) ;
        }
//         return false ;
    }
    return false ;
}

void dpps::Writer_witec::get_extremum_altitude_from_pattern (
    const Pattern &pattern,
    const selection_t selection) {
    Pattern q {Pattern_factory::Pattern_from_selected (pattern, selection)} ;
    if (writer_settings. correct_focus_inside_polylines)
        q. explode (selection) ;
    Pattern r ;
    for (auto i: writer_settings. focus_position)
        r. push_back (Polyline (i)) ;
    q. append_from (r) ;

    std::partial_sort (q. polylines. begin (),
                       q. polylines. begin () + 1,
                       q. polylines. end (),
                       std::bind (&Writer_witec::compare_height, this,
                            std::placeholders::_1, std::placeholders::_2)) ;
    extremum_altitude_position = q. polylines. front (). geometrical_centre () ;
    extremum_altitude = interpolate_height (extremum_altitude_position) ;

}

void dpps::Writer_witec::write_Pattern_selected (const Pattern &pattern,
                                                 const selection_t selection) {
    if (pattern. size (selection) == 0)
        return ;
    if (writer_settings. hysteresis_quadrant != hysteresis_no_correction)
        get_reference_position_from_pattern (pattern, selection) ;
    if (writer_settings. focus_correction) {
        get_extremum_altitude_from_pattern (pattern, selection) ;
        if (writer_settings. stepper_reorder == stepper_reorder_zdown_x_y ||
            writer_settings. stepper_reorder == stepper_reorder_zdown_y_x)
            tmpfile << "/* Maximum " ;
        else
            tmpfile << "/* Minimum " ;
        tmpfile << " z value of sample found at "
                << extremum_altitude_position. display_string ()
                << " (altitude " << extremum_altitude
                << " µm) please go there and focus,"
                << " then go back to (0,0) */\n" ;
        // We assume user follows our instruction and focused there,
        // we set the current height accordingly
        current_stepper_height = extremum_altitude ;
        if (writer_settings. xy_by_piezo || writer_settings. focus_by_piezo) {
            tmpfile << string_set_origin << "(0,0,0);\n" ;
            current_piezo_height = 0.0 ;
        }
        if (writer_settings. stepper_reorder == stepper_reorder_none) {
            if (writer_settings. xy_by_piezo) {
                Pattern q {Pattern_factory::Pattern_from_selected (
                    pattern, selection)} ;
                // See N. Josuttis (2012), section 11.9.1 pages 597-598
                std::sort (q. polylines. begin (), q. polylines. end (),
                    std::bind (&Writer_witec::compare_x_y_field, this,
                        std::placeholders::_1, std::placeholders::_2)) ;
                Writer::write_Pattern_selected (q, selection) ;
            } else
                Writer::write_Pattern_selected (pattern, selection) ;
        } else {
            Pattern q {Pattern_factory::Pattern_from_selected (
                pattern, selection)} ;
            std::sort (q. polylines. begin (), q. polylines. end (),
                std::bind (&Writer_witec::compare_height_x_y, this,
                    std::placeholders::_1, std::placeholders::_2)) ;
            Writer::write_Pattern_selected (q, selection) ;
        }
    } else
        Writer::write_Pattern_selected (pattern, selection) ;
}

void dpps::Writer_witec::write_Pattern (const Pattern &pattern) {
    Pattern q {pattern} ;
    q. select (logical_action_all, 0) ;
    write_Pattern_selected (q, 0) ;
}

void dpps::Writer_witec::update_duration_shutter () {
    shutter_number++ ;
    shutter_duration += writer_settings. shutter_response_time ;
}
void dpps::Writer_witec::update_duration_jump_to(const Vertex &destination) {
    double distance {(destination - current_stepper_position). norm2 ()} ;
    jump_number++ ;
    jump_distance += distance ;
    jump_duration += distance / writer_settings. jump_speed ;
}

void dpps::Writer_witec::update_duration_move_to(const Vertex &destination,
                                                  const double speed) {
    double distance {(destination - current_stepper_position). norm2 ()} ;
    move_number++ ;
    move_distance += distance ;
    move_duration += distance / speed ;
}

void dpps::Writer_witec::update_duration_dot (const double time) {
    dot_number++ ;
    dot_duration += time * 0.001 ; // time is in milliseconds
}

void dpps::Writer_witec::sleep (const double time) {
    tmpfile << string_sleep << "("
         << fixed_number (time, 4, false)
         << ");\n" ;
    update_duration_dot (time) ;
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::check_speed (const double speed) {
    if ((fabs (speed - current_moving_speed) > speed_unit) && (speed > 0)) {
        // issue command if speed changed significantly ;
        // negative speed means no commands are issued.
        current_moving_speed = speed ;
        tmpfile << string_speed << "("
             << fixed_number (writer_settings. moving_speed, 4, false)
             << ");\n" ;
    }
}

void dpps::Writer_witec::piezo_move_to(const Vertex &destination,
    const double speed) {
    double dest_height ;
    double rel_height ;
    Vertex dest {destination} ;
    if (fabs (dest.x) < movement_unit)
        dest.x = 0 ;
    if (fabs (dest.y) < movement_unit)
        dest.y = 0 ;
    check_speed (speed) ;
    update_duration_move_to (dest, speed) ;
    if (writer_settings. focus_correction && writer_settings. focus_by_piezo) {
        // When we are here we are focused (because lithography_on() was called)
        // so we correct by the difference in height between the start and end
        // of the movement.
        rel_height = interpolate_height (destination)
                     - interpolate_height (current_piezo_position) ;
        dest_height = current_piezo_height + rel_height ;
    } else
        dest_height = current_piezo_height ;
    if (writer_settings. absolute_movements) {
        tmpfile << string_piezo_move_absolute << "("
             << fixed_number (dest. x, 4, false)
             << ","
             << fixed_number (dest. y, 4, false)
             << ","
             << fixed_number (dest_height, 4, false)
             << ");\n" ;
        current_piezo_position. x = fixed_number_value(dest. x, 4) ;
        current_piezo_position. y = fixed_number_value(dest. y, 4) ;
        current_piezo_height = fixed_number_value(dest_height, 4) ;
    } else {
        Vertex rel {dest. x - current_piezo_position. x,
                    dest. y - current_piezo_position. y} ;
        rel_height = dest_height - current_piezo_height ;
        tmpfile << string_piezo_move_relative << "("
             << fixed_number (rel. x, 4, false)
             << ","
             << fixed_number (rel. y, 4, false)
             << ","
             << fixed_number (rel_height, 4, false)
             << ");\n" ;
        current_piezo_position. x += fixed_number_value(rel. x, 4) ;
        current_piezo_position. y += fixed_number_value(rel. y, 4) ;
        current_piezo_height += fixed_number_value(rel_height, 4) ;
    }
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::piezo_jump_to(const Vertex &destination) {
    double dest_height {current_piezo_height} ;
    double rel_height {0.0} ;
    Vertex dest {destination} ;
    if (fabs (dest.x) < movement_unit)
        dest.x = 0 ;
    if (fabs (dest.y) < movement_unit)
        dest.y = 0 ;
    update_duration_jump_to (dest) ;
    if (writer_settings. focus_correction && writer_settings. focus_by_piezo) {
        // We keep focus status by changing only relative values.
        rel_height = interpolate_height (destination)
                     - interpolate_height (current_piezo_position) ;
        dest_height = current_piezo_height + rel_height ;
    }
    if (writer_settings. absolute_movements) {
        tmpfile << string_piezo_jump_absolute << "("
             << fixed_number (dest. x, 4, false)
             << ","
             << fixed_number (dest. y, 4, false)
             << ","
             << fixed_number (dest_height, 4, false)
             << ");\n" ;
        current_piezo_position. x = fixed_number_value (dest. x, 4) ;
        current_piezo_position. y = fixed_number_value (dest. y, 4) ;
        current_piezo_height = fixed_number_value(dest_height, 4) ;
    } else {
        Vertex rel {dest. x - current_piezo_position. x,
                    dest. y - current_piezo_position. y} ;
        tmpfile << string_piezo_jump_relative << "("
             << fixed_number (rel. x, 4, false)
             << ","
             << fixed_number (rel. y, 4, false)
             << ","
             << fixed_number (rel_height, 4, false)
             << ");\n" ;
        current_piezo_position. x += fixed_number_value (rel. x, 4) ;
        current_piezo_position. y += fixed_number_value (rel. y, 4) ;
        current_piezo_height += fixed_number_value(rel_height, 4) ;
    }
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::stepper_jump_to(const Vertex &destination) {
    Vertex dest {destination} ;
    if (fabs (dest.x) < movement_unit)
        dest.x = 0 ;
    if (fabs (dest.y) < movement_unit)
        dest.y = 0 ;
    update_duration_jump_to (dest) ;
    if (writer_settings. absolute_movements) {
        tmpfile << string_stepper_jump_absolute << "("
             << fixed_number (dest. x, 4, false)
             << ","
             << fixed_number (dest. y, 4, false)
             << ");\n" ;
        current_stepper_position. x = fixed_number_value (dest. x, 4) ;
        current_stepper_position. y = fixed_number_value (dest. y, 4) ;
    } else {
        Vertex rel {dest. x - current_stepper_position. x,
                    dest. y - current_stepper_position. y} ;
        rel. x = writer_settings. stepper_xy_relative_step *
                    round (rel. x / writer_settings. stepper_xy_relative_step) ;
        rel. y = writer_settings. stepper_xy_relative_step *
                    round (rel. y / writer_settings. stepper_xy_relative_step) ;
        tmpfile << string_stepper_jump_relative << "("
             << fixed_number (rel. x, 4, false)
             << ","
             << fixed_number (rel. y, 4, false)
             << ");\n" ;
        current_stepper_position. x += fixed_number_value (rel. x, 4) ;
        current_stepper_position. y += fixed_number_value (rel. y, 4) ;
    }
    //just_took_snapshot = false ;
}

void dpps::Writer_witec::stepper_move_to(const Vertex &destination,
    const double speed) {
    Vertex dest {destination} ;
    if (fabs (dest.x) < movement_unit)
        dest.x = 0 ;
    if (fabs (dest.y) < movement_unit)
        dest.y = 0 ;
    check_speed (speed) ;
    update_duration_move_to (dest, speed) ;
    if (writer_settings. absolute_movements) {
        tmpfile << string_stepper_move_absolute << "("
             << fixed_number (dest. x, 4, false)
             << ","
             << fixed_number (dest. y, 4, false)
             << ");\n" ;
        current_stepper_position = dest ;
    } else {
        Vertex rel {dest. x - current_stepper_position. x,
                    dest. y - current_stepper_position. y} ;
        rel. x = writer_settings. stepper_xy_relative_step *
                    round (rel. x / writer_settings. stepper_xy_relative_step) ;
        rel. y = writer_settings. stepper_xy_relative_step *
                    round (rel. y / writer_settings. stepper_xy_relative_step) ;
        tmpfile << string_stepper_move_relative << "("
             << fixed_number (rel. x, 4, false)
             << ","
             << fixed_number (rel. y, 4, false)
             << ");\n" ;
        current_stepper_position = current_stepper_position + rel ;
    }
    //just_took_snapshot = false ;
}

/*
void dpps::Writer_witec::set_alignment_points () {
    writer_settings. a. clear () ;
    writer_settings. b. clear () ;
}

void dpps::Writer_witec::set_alignment_points (
    const Vertex &a0, const Vertex &b0) {
    writer_settings. a. clear () ;
    writer_settings. b. clear () ;
    writer_settings. a. push_back (a0) ;
    writer_settings. b. push_back (b0) ;
    alignment_done = false ;
}

void dpps::Writer_witec::set_alignment_points (
    const Vertex &a0, const Vertex &a1,
    const Vertex &b0, const Vertex &b1) {
    writer_settings. a. clear () ;
    writer_settings. b. clear () ;
    writer_settings. a. push_back (a0) ;
    writer_settings. a. push_back (a1) ;
    writer_settings. b. push_back (b0) ;
    writer_settings. b. push_back (b1) ;
    alignment_done = false ;
}

void dpps::Writer_witec::set_alignment_points (
    const Vertex &a0, const Vertex &a1, const Vertex &a2,
    const Vertex &b0, const Vertex &b1, const Vertex &b2) {
    writer_settings. a. clear () ;
    writer_settings. b. clear () ;
    writer_settings. a. push_back (a0) ;
    writer_settings. a. push_back (a1) ;
    writer_settings. a. push_back (a2) ;
    writer_settings. b. push_back (b0) ;
    writer_settings. b. push_back (b1) ;
    writer_settings. b. push_back (b2) ;
    alignment_done = false ;
}

#ifdef GSL_FOUND
void dpps::Writer_witec::set_alignment_points (
    const Vertex &a0, const Vertex &a1, const Vertex &a2, const Vertex &a3,
    const Vertex &b0, const Vertex &b1, const Vertex &b2, const Vertex &b3) {
    writer_settings. a. clear () ;
    writer_settings. b. clear () ;
    writer_settings. a. push_back (a0) ;
    writer_settings. a. push_back (a1) ;
    writer_settings. a. push_back (a2) ;
    writer_settings. a. push_back (a3) ;
    writer_settings. b. push_back (b0) ;
    writer_settings. b. push_back (b1) ;
    writer_settings. b. push_back (b2) ;
    writer_settings. b. push_back (b3) ;
    alignment_done = false ;
}
#endif


void dpps::Writer_witec::calculate_alignment_parameters () {
    bool result {true} ;
    switch (writer_settings. alignment) {
        case alignment_none:
            result = true ;
            break ;
        case alignment_translation:
            if (writer_settings. a. size () < 1) {
                std::string reason {"Writer_witec::calculate_alignment_parameters, \
alignment_translation uses one points to correct one translation, but none was \
provided"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            result = Vertex::get_matrix_coefficients_translation (
                h,
                writer_settings. a[0],
                writer_settings. b[0]) ;
            break ;
        case alignment_isometry:
            if (writer_settings. a. size () < 2) {
                std::string reason {"Writer_witec::calculate_alignment_parameters, \
alignment_isometry uses two points to correct one rotation, but " +
std::to_string ((long_unsigned_int)writer_settings. a. size ()) + " were provided"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            result = Vertex::get_matrix_coefficients_isometry (
                h,
                writer_settings. a[0], writer_settings. a[1],
                writer_settings. b[0], writer_settings. b[1],
                movement_unit) ;
            break ;
        case alignment_similarity:
            if (writer_settings. a. size () < 2) {
                std::string reason {"Writer_witec::calculate_alignment_parameters, \
alignment_similarity uses two points to correct one rotation and one scaling \
factor, but " +
std::to_string ((long_unsigned_int)writer_settings. a. size ()) + " were provided"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            result = Vertex::get_matrix_coefficients_similarity (
                h,
                writer_settings. a[0], writer_settings. a[1],
                writer_settings. b[0], writer_settings. b[1],
                movement_unit) ;
            break ;
        case alignment_affine:
            if (writer_settings. a. size () < 3) {
                std::string reason {"Writer_witec::calculate_alignment_parameters, \
alignment_affine uses three points \
to correct two rotations and two scaling factors, but " +
std::to_string ((long_unsigned_int)writer_settings. a. size ()) + " were provided"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            result = Vertex::get_matrix_coefficients_affine (
                h,
                writer_settings. a[0], writer_settings. a[1],
                writer_settings. a[2],
                writer_settings. b[0], writer_settings. b[1],
                writer_settings. b[2],
                movement_unit) ;
            break ;
#ifdef GSL_FOUND
        case alignment_homography:
            if (writer_settings. a. size () < 4) {
                std::string reason {"Writer_witec::calculate_alignment_parameters, \
alignment_homography uses four points \
to correct the full transformation matrix, but " +
std::to_string ((long_unsigned_int)writer_settings. a. size ()) + " were provided"} ;
                throw bad_parametre (reason. c_str ()) ;
            }
            result = Vertex::get_matrix_coefficients_homography (
                h,
                writer_settings. a[0], writer_settings. a[1],
                writer_settings. a[2], writer_settings. a[3],
                writer_settings. b[0], writer_settings. b[1],
                writer_settings. b[2], writer_settings. b[3],
                movement_unit) ;
#endif
    }
    if (!result) {
        std::string reason {"Writer_witec::calculate_alignment_parameters, \
points used were detected to be too close one from each other or aligned, to \
the precision of the movement, and this does not allow alignment correction"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
}
*/
double dpps::Writer_witec::interpolate_height (const Vertex &position) { // const
    Vertex p0, p1, p2 ;
    double f0{0.0},
           f1{0.0},
           f2{0.0} ;
    // initializing to avoid a warning ; but actually it will always either
    // return before they are used, or pass through initialization in all cases:
    // focus_position. size () == 2, == 3, >= 3.
    // NOTE: we get a gcc warning in -O2 but not in -O0
    if (writer_settings. focus_position. size () == 0)
        return 0.0 ;
    if (writer_settings. focus_position. size () == 1)
        return writer_settings. focus_value. front () ;
    if (writer_settings. focus_position. size () == 2) {
        p0 = writer_settings. focus_position[0] ;
        p1 = writer_settings. focus_position[1] ;
        f0 = writer_settings. focus_value[0] ;
        f1 = writer_settings. focus_value[1] ;

        // To define a plane we need three points, but user just gave p0 and p1
        // We choose p2 the third so that :
        // (1). (H p2) is perpendicular to (p1 p2)
        // (2). (H p2) lies in the horizontal plane
        // with H is the middle point in between p0 and p1.
        //
        // If p2 has coordinates (pqr) and u=p1-p0 (vector from p0 to p1),
        // we have
        // from condition (2): r = 0
        // from condition (1): ux.p + uy.q = 0
        // if uy != 0, We can choose p = 1 and then q = -Ux/Uy.
        // if uy = 0, we choose p = 0 and q = 1.
        Vertex u {p1-p0} ;
        if (fabs (u. y) < movement_unit)
            p2 = Vertex (0, 1) ;
        else
            p2 = Vertex (1, -u.x/u.y) ;
        f2 = (f0+f1) / 2.0 ;
    }
    if (writer_settings. focus_position. size () == 3) {
        p0 = writer_settings. focus_position[0] ;
        p1 = writer_settings. focus_position[1] ;
        p2 = writer_settings. focus_position[2] ;
        f0 = writer_settings. focus_value[0] ;
        f1 = writer_settings. focus_value[1] ;
        f2 = writer_settings. focus_value[2] ;
    }
    if (writer_settings. focus_position. size () > 3) {
        double min_dist {std::numeric_limits<double>::max ()} ;
        double d ;
        // Initializing to avoid a gcc warning.
        // NOTE: we get a gcc warning in -O2 but not in -O0
        long_unsigned_int i0{0}, i1{0}, i2{0} ;
        for (long_unsigned_int i {0} ; i < writer_settings. focus_position. size () ; i++) {
            d = (writer_settings. focus_position[i] - position). norm2_square () ;
            if (d < min_dist) {
                i0 = i ;
                min_dist = d ;
            }
        }
        min_dist = std::numeric_limits<double>::max () ;
        for (long_unsigned_int i {0} ; i < writer_settings. focus_position. size () ; i++) {
            if (i == i0)
                continue ;
            d = (writer_settings. focus_position[i] - position). norm2_square () ;
            if (d < min_dist) {
                i1 = i ;
                min_dist = d ;
            }
        }
        min_dist = std::numeric_limits<double>::max () ;
        for (long_unsigned_int i {0} ; i < writer_settings. focus_position. size () ; i++) {
            if ((i == i0) || (i == i1))
                continue ;
            if (writer_settings. focus_position[i]. is_aligned (
                    writer_settings. focus_position[i0],
                    writer_settings. focus_position[i1], movement_unit))
                continue ;
            d = (writer_settings. focus_position[i] - position). norm2_square () ;
            if (d < min_dist) {
                i2 = i ;
                min_dist = d ;
            }
        }
        p0 = writer_settings. focus_position[i0] ;
        p1 = writer_settings. focus_position[i1] ;
        p2 = writer_settings. focus_position[i2] ;
        f0 = writer_settings. focus_value[i0] ;
        f1 = writer_settings. focus_value[i1] ;
        f2 = writer_settings. focus_value[i2] ;

//             std::string comment {"Position " + position. display_string()
//                 + ", three closest focus points: "
//                 + p0. display_string() + "(" + std::to_string (f0) + " µm), "
//                 + p1. display_string() + "(" + std::to_string (f1) + " µm), "
//                 + p2. display_string() + "(" + std::to_string (f2) + " µm)."} ;
//             emit_comment (comment) ;
    }
    // We know that p0, p1 and p2 are different, this was checked at
    // call of add_focus_point().

    // We have three points defining a plane: P0, P1, P2.
    // We need to know the height az of point A (ax, ay, az) to be coplanar.
    //
    // A is current_stepper_position, this function should return the value of az.
    //
    // https://en.wikipedia.org/wiki/Coplanarity#Properties
    // Coplanarity criterion can be written
    // (P2-P0).[(P1-P0)×(A-P2)]=0
    // Let call u=P2-P0, v=P1-P0.
    // u.[v x (A-P2)] = 0
    // We develop the scalar and vectorial products and solve for Az.
    //    solve( vx*( uy*(az-p2z)-uz*(ay-p2y))+
    //          vy*(-ux*(az-p2z)+uz*(ax-p2x))+
    //          vz*( ux*(ay-p2y)-uy*(ax-p2x)) = 0, az)
    // -> az = ( ((p2x-ax)*uy+(ay-p2y)*ux)*vz+
    //          ((ax-p2x)*uz+p2z*ux)*vy+
    //          ((p2y-ay)*uz-p2z*uy)*vx   ) /(ux*vy-uy*vx)
#define ux  (p2.x - p0.x)
#define uy  (p2.y - p0.y)
#define uz  (f2   - f0  )
#define vx  (p1.x - p0.x)
#define vy  (p1.y - p0.y)
#define vz  (f1   - f0  )
#define p2x (p2.x)
#define p2y (p2.y)
#define p2z (f2  )
#define ax  (position.x)
#define ay  (position.y)

//     std::string comment {
//         " ux " + std::to_string (ux) +
//         " uy " + std::to_string (uy) +
//         " uz " + std::to_string (uz) +
//         " vx " + std::to_string (vx) +
//         " vy " + std::to_string (vy) +
//         " vz " + std::to_string (vz) +
//         " p2x " + std::to_string (p2x) +
//         " p2y " + std::to_string (p2y) +
//         " p2z " + std::to_string (p2z) +
//         " ax " + std::to_string (ax) +
//         " ay " + std::to_string (ay) +
//         " denom " + std::to_string (ux*vy-uy*vx)
//
//     } ;
//     emit_comment (comment) ;

    // Note that the non-null nature of the divisor is checked
    // in set_advanced_parametres()
    return ( ((p2x-ax)*uy+(ay-p2y)*ux)*vz+
            ((ax-p2x)*uz+p2z*ux)*vy+
            ((p2y-ay)*uz-p2z*uy)*vx      )
           /(ux*vy-uy*vx) ;
//     comment = "Heigh: " + std::to_string (result) ;
//     emit_comment (comment) ;
//     return result ;

#undef ux
#undef uy
#undef uz
#undef vx
#undef vy
#undef vz
#undef p2x
#undef p2y
#undef p2z
#undef ax
#undef ay
}
