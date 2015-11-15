/**
 * @file Writer_witec.hh
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Writer_witec This class contains the processes to control a Witec
 * laser stage in order to draw \c Polylines.
 * @date 2013-12-08 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This class contains the processes to write the format specified by Witec for
 * a laser stage with the lithography package. Specification was communicated to
 * us by e-mail.
 * */

#ifndef DPPS_WRITER_WITEC
#define DPPS_WRITER_WITEC

#include <array>

#include "Vertex.hh"
#include "Writer.hh"
#include "Pattern.hh"

namespace dpps {
typedef enum {
    hysteresis_no_correction,
    hysteresis_quadrant_pxpy,
    hysteresis_quadrant_mxpy,
    hysteresis_quadrant_mxmy,
    hysteresis_quadrant_pxmy,
} enum_hysteresis_quadrant ;

typedef enum {
    stepper_reorder_none,
    stepper_reorder_zup_y_x,
    stepper_reorder_zup_x_y,
    stepper_reorder_zdown_y_x,
    stepper_reorder_zdown_x_y,
} enum_stepper_reorder ;

/** @brief The alignment correction mode.
 *
 * This is intended to correct misalignments for overlay lithography.
 * the user provides a vertices, which are exactly known in the design,
 * and the b vertices, which are measured on the sample using the stage
 * movements. Corrections factors (translation, rotation and scaling) are
 * then computed and applied to each vertex in each polyline.
 *
 * The simplest alignment possible is the translation. It is unavoidable,
 * as a reference position has ot be defined as the zero of the coordinates.
 *
 * alignment_translation   uses one reference point ("the zero")
 *   design is just exposed assuming a (0,0) provided by the user to the
 *   lithography system.
 *
 * alignment_isometry      uses two points: rotation
 *   property it keeps: euclidian distance between points (and all of the below)
 *
 * alignment_similarity    uses two points: rotation and isotropic scaling
 *   property it keeps: angles between lines (and all of the below)
 *
 * alignment_affine        uses three points: two rotations and two scalings
 *   property it keeps: parallelity of lines (and all of the below)
 *
 * alignment_homography    uses four points (eight degrees of freedom)
 *   property it keeps: alignment of three points
 *   It was finally not implemented, because there is no obvious use case
 *   (an overlay lithograpĥy with a two-axis stage can be assumed to differ
 *    from any other by an affine transformation)
 *
 * For all the transformations, the two degrees of freedom of translations
 * are taken into account by always assuming that the (0,0) point from design
 * coincides with the (0,0) point of the stage.
 */

/*typedef enum {
    alignment_none,
    alignment_translation,
    alignment_isometry,
    alignment_similarity,
    alignment_affine
#ifdef GSL_FOUND
    , alignment_homography
#endif
} enum_alignment ;
*/
/** @brief A Writer derivative for Witec laser stage */
class Writer_witec: public Writer {
public:
    /** @brief A struct to encapsulates the metadata needed to write
     * efficiently */
    struct Writer_witec_settings: public Writer_settings {
    public:

        bool calculate_reference_point {true} ;

        enum_hysteresis_quadrant hysteresis_quadrant {
            hysteresis_quadrant_pxpy} ;

        /** @brief Speed of the stage in micrometers per second, used when
         * the polyline dose is set to zero. */
        double moving_speed {10.0} ;

        /** @brief The time of exposure for single dots, in milliseconds, used
         * when the single-vertex polyline dose is set to zero. */
        double single_dot_time {100.0} ;

        /** @brief Estimation of the stage linear speed in Jump mode (as fast
         * as possible). This is used for the evaluation of the exposure
         * time. */
        double jump_speed {30.0} ;

        // on a 3 Mb example, short_names reduced 45 % the size of the
        // resulting file.  However, after xz-9e compression, the difference
        // was 8 %.
        bool short_names {false} ;
        bool absolute_movements {true} ;
        bool finish_at_zero {true} ;
        bool litho_by_shutter {true} ;
        bool litho_by_focus {false} ;
        double defocus_steps {10.0} ;
        bool focus_correction {false} ;

        std::vector<Vertex> focus_position ;
        std::vector<double> focus_value ;
        long_unsigned_int take_snapshot_every {0} ;
        double stepper_threshold_z_correction {0.2} ;
        double piezo_threshold_z_correction {0.01} ;
        double stepper_z_correction_step {0.1} ;
        double stepper_xy_relative_step {0.1} ;
        enum_stepper_reorder stepper_reorder {stepper_reorder_none} ;
        double shutter_response_time {0.6} ; // unit: seconds

        bool xy_by_piezo {false} ;
        bool focus_by_piezo {false} ;

        double piezo_x_range {100.0} ;
        double piezo_y_range {100.0} ;

        Vertex field_crossing ;
        double field_size_x {100.0} ;
        double field_size_y {100.0} ;
        bool automatic_field_crossing {true} ;

        bool comment_inline {false} ;
        bool comment_separate {false} ;
        bool comment_cerr {false} ;
        bool correct_focus_inside_polylines {false} ;
        //enum_alignment alignment {alignment_translation} ;
        std::vector<Vertex> a, b ;
} ;

protected:
    /** @brief default constructor is private so that user has to pass
     * filename and append parameters. */
    Writer_witec () ;

    /** @brief variable that describe the nature of the quadrants in the
     * different cases given by hysteresis_quadrant.
     *
     * There are 4 quadrants and 4 equality cases, and method Vertex::quadrant
     * will return a value from 0 to 8 (0 is equality, 1, 3, 5, 7 are the four
     * quadrants, and 8, 2, 4, 6 the respective straight angles at 0, pi/2,
     * pi and 3pi/2).
     *
     * If user selected hysteresis_quadrant as 1 (the usual case), the correct
     * movement direction is +x/+y, which is quadrant 1, and equality cases
     * 8 and 2. Therefore in this case, we will have correct={8, 1, 2}. The
     * opposite quadrant (the one in which we should swap the direction of a
     * segment) is in this case {4, 5, 6}.
     *
     * These will be used if hysteresis is to be corrected (if user sets
     * hysteresis_quadrant to 1, 2, 3 or 4) and will be ignored of
     * hysteresis_quadrant is zero.
     *
     */
    std::array<enum_quadrant, 3>
        correct  {{quadrant_centre, quadrant_centre, quadrant_centre}},
        opposite {{quadrant_centre, quadrant_centre, quadrant_centre}},
        lateral  {{quadrant_centre, quadrant_centre, quadrant_centre}} ;

    Writer_witec_settings writer_settings ;

    /** @brief The absolute position of the stage after each Polyline is
     * written, so that we can always calculate what is the relative movement
     * to the next Polyline.
     */
    Vertex current_stepper_position ;
    Vertex current_piezo_position ;
    Vertex reference_position ;

    double duration {0.0} ;
    double current_linear_velocity {10.0} ;
    double current_dot_dose {100.0} ;

    // set to a 0 so that it is detected as changed at the first movement
    // if user set it to a > 0 value.
    double current_moving_speed {0.0} ;

    //double current_jump_speed {0.0} ;
    bool triggered {false} ;
    bool status_hysteresis_ok {true} ;
    std::vector<Polyline> decompose_polyline_no_hysteresis_paths (
        const Polyline &p) ;
    // Note that current_hysteresis_quadrant is set to none so that
    // at first use when user requested a correction, it is detected as changed.
    enum_hysteresis_quadrant current_hysteresis_quadrant {
            hysteresis_no_correction} ;

    double current_stepper_height {0.0} ;
    double current_piezo_height {0.0} ;
    double local_sample_height {0.0} ;
    bool focused {true} ;

    long_unsigned_int move_number {0} ;
    long_unsigned_int jump_number {0} ;
    long_unsigned_int dot_number {0} ;
    long_unsigned_int shutter_number {0} ;
    double move_distance {0.0} ;
    double jump_distance {0.0} ;
    double move_duration {0.0} ;
    double jump_duration {0.0} ;
    double dot_duration {0.0} ;
    double shutter_duration {0.0} ;
    bool three_point_adjust_already_made {false} ;

    // The epsilon for all movements, in units of micrometres
    static const constexpr double movement_unit {0.001} ;

    // The smallest value for the speed, as per documentation of the control
    // software.
    static const constexpr double speed_unit {0.1} ;

    void get_reference_position_from_pattern (const dpps::Pattern &p,
        const selection_t selection) ;
    void update_duration_jump_to (const Vertex &destination) ;//, const double speed) ;
    void update_duration_move_to (const Vertex &destination,
        const double speed) ;
    void update_duration_dot (const double time) ;
    void update_duration_shutter () ;

    void stepper_lithography_on (const Vertex &focus_position,
                                 const bool correct_focus_now) ;
    void lithography_off () ;

    /** @brief sets lithography to on, in case we are using the piezo for
     * xy movements.
     *
     * the difference is that stepper_lithography_on() has to perform focus
     * change before the call to stepper_move_to() so that focus is stable
     * before moving the stage, while piezo_lithography_on() only performs
     * focus changes if they have to be performed by stepper. If they are
     * to be performed by piezo then they are taken care of by the
     * piezo_jump_to() and pizeo_move_to() commands.
     * */
    void piezo_lithography_on (const Vertex &focus_position,
                               const bool correct_focus_now) ;

    //void print_fixed_number (const double value) ;

    void stepper_jump_to (const Vertex &destination) ;
    void stepper_move_to (const Vertex &destination, const double speed) ;
    void piezo_jump_to (const Vertex &destination) ;
    void piezo_move_to (const Vertex &destination, const double speed) ;
    void sleep (const double time) ;
    void check_speed (const double speed) ;

    std::string capture_name ;

    double interpolate_height (const Vertex &position) ;//const ;

    // initial value set to true so that the first image at start is skipped
    //bool just_took_snapshot {true} ;

    void perform_focus_change (double value) ;
    std::string string_speed                 = "MovingSpeed" ;
    std::string string_stepper_z_move        = "MoveZMicroscope" ;
    std::string string_stepper_move_absolute = "MoveAbsoluteSamplePos" ;
    std::string string_stepper_move_relative = "MoveRelativeSamplePos" ;
    std::string string_stepper_jump_absolute = "JumpAbsoluteSamplePos" ;
    std::string string_stepper_jump_relative = "JumpRelativeSamplePos" ;

    std::string string_piezo_move_absolute   = "MoveAbsolute" ;
    std::string string_piezo_move_relative   = "MoveRelative" ;
    std::string string_piezo_jump_absolute   = "JumpAbsolute" ;
    std::string string_piezo_jump_relative   = "JumpRelative" ;
    std::string string_set_origin            = "SetOrigin" ;
    std::string string_stabilize             = "WaitForStablePosition" ;

    std::string string_sleep                 = "Sleep" ;
    std::string string_shutter               = "SetTrigger" ;
    std::string string_message               = "DisplayMessage" ;
    std::string string_snapshot              = "Snapshot" ;


    bool compare_height_x_y (const Polyline &p, const Polyline &q) ; //const ;
    bool compare_height (const Polyline &p, const Polyline &q) ; //const
    void get_extremum_altitude_from_pattern (const Pattern &pattern,
        const selection_t selection) ;
    bool compare_x_y (const Vertex &v, const Vertex &w) const ;
    bool compare_x_y_field (const Polyline &p, const Polyline &q) const ;
    bool polyline_crosses_field_border (const Polyline &p) const ;
    double extremum_altitude ;

    // The position of the extremum altitude calculated on dots and two-vertices
    // elements composing polylines. Sample will be assumed to be focused there
    // at the beginning of the exposure and this will define the zero of
    // altitudes when doing focus correction. However, this might not be the
    // first line to be exposed, if it happens not be the extermity of a
    // polyline. If polylines are small enough, this will make no difference.
    // However, if this polyline is large enough so that significant differences
    // in altitude should be accounted for dueing its exposure, it is possible
    // that non-monotonic vertical movements will be ordered, possibly having an
    // impact on accuracy of vertical mechanical movements. To avoid this issue,
    // user should use small polylines when focus correction is critical.
    Vertex extremum_altitude_position ;
    void write_polyline_piezo (Polyline &p) ;
    void move_piezo (Polyline &p) ;

    /** @brief Check for hysteresis and calls stepper_jump_to().
     *
     * The purpose of this function is to encapsulate stepper_jump_to() with
     * hysteresis check and possible jumps to the reference position to
     * compensate.
     */
    void go_stepper_to (const Vertex &position) ;

    /** @brief Centre of field (i, j)
     *
     * The field (0,0) is centred on (field_size_x/2, field_size_y/2) and
     * field (i,j) are translated by (i*field_size_x, j*field_size_y).
     */
    Vertex get_field_centre_position (
        const long signed int i,
        const long signed int j) const ;
    void write_polyline_stepper (Polyline &p) ;
    //void emit_warning (const std::string &reason) ;
    /** @brief Writes a comment on the log.
     *
     * If separate_comment is true, then the string is written to the logfile
     * opened in write_header(), ostream log, named by log_filename.
     *
     * If inline_comment is true, the string is output in the main output file
     * within comment characters.
     *
     * Separate_comment and inline_comment can be set true or false
     * independently.
     *
     */
    void emit_comment (const std::string value) ;
    void piezo_stabilize () ;

    /** @brief Get the (i, j) field coordinate of a given Vertex position.
     *
     * A field corresponds to the division of the (xy) plane in rectangles of
     * field_size_x and field_size_y, where one corner is Vertex field_crossing.
     * The fields are taken relatively to field_crossing. Field (0, 0) is
     * the field immediatedly top-right of Vertex field_crossing.
     * This function is called in write_polyline_piezo() to know when the next
     * polyline needs a field change.
     */
    void get_field_number (long signed int &i, long signed int &j,
                           const Vertex &destination) const ;
    /** @brief Where some log information is output, if a separate log has been
     * requested. */
    std::ofstream log ;
    /** @brief Name of the log file, when a separate log has been requested.
     *
     * The name of the log is the radical of the filename plus ".log".
     * If the filename already ends in .log, then affix ".logfile" is used.
     */
    std::string log_filename ;

    /** @brief coordinates of the transformation matrix.
     *
     * alignment_translation -> h[0] and h[1] (translation) are used
     * alignment_isometry    -> the above and h[2] (rotation)
     * alignment_similarity  -> the above and h[3] (scaling factor)
     * alignment_affine      -> the above and h[4] (rot.), h[5] (scaling fact.)
     *
     * The full matrix has 9 coordinates and 8 degrees of freedom.
     * One coordinate is arbitrary. Two for the translation (0,0) -> (0,0)
     * need not being stored.
     */
    //std::array<double, 9> h ;

    //bool alignment_done {false} ;
    /** @brief the function that calculates the transformation matrix. */
    //void calculate_alignment_parameters () ;
public:
    ~Writer_witec() ;
    void close () ;
    // Implementation of inherited virtual functions.
    Writer_witec (const std::string &set_filename, const bool append) ;
    void write_header () ;
    void write_footer () ;
    void set_parametres (const std::vector<bool> &vbool,
                         const std::vector<long_unsigned_int> &vint,
                         const std::vector<double> &vdouble,
                         const std::vector<std::string> &vstring) ;
    void write_Polyline (const Polyline &polyline) ;
    void write_Pattern_selected (const Pattern &pattern,
                                 const selection_t selection = 0) ;
    void write_Pattern (const Pattern &pattern) ;
    // class-specific functions

    /**
     * Note that if xz_by_piezo is used, reorder is ignored
     */
    void set_all_parametres (
        const bool finish_at_zero,
        const bool focus_correction,
        const bool litho_by_shutter,
        const bool litho_by_focus,
        const bool xy_by_piezo,
        const bool focus_by_piezo,
        const bool automatic_field_crossing,
        const bool correct_focus_inside_polylines,
        //const enum_alignment alignment,
        const enum_stepper_reorder reorder,
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
        const Vertex field_crossing) ;
    void set_additional_parametres (
        const bool short_names,
        const bool absolute_movements,
        const bool comment_separate,
        const bool comment_inline,
        const bool comment_cerr,
        const double stepper_xy_relative_step,
        const double piezo_x_range,
        const double piezo_y_range) ;
    void add_focus_point (const Vertex &position, const double value) ;
    void clear_focus_points () ;

#ifdef GSL_FOUND
    /** @brief The functions to call to set the points used to align the sample.
     *
     * The version with 8 vertices must be called for alignment_homography.
     * Any call of this function will call previous alignment point definitions.
     *
     * * If user sets alignment_none, none of these will be used
     * * If user sets alignment_translation, none of these will be used
     * * If user sets alignment_isometry, the first two points are used
     * * If user sets alignment_similarity, the first two points are used
     * * If user sets alignment_affine, the first three points are used
     *
     * New alignment parameters will be calculated and applied
     * of write_Polyline(), write_Pattern() and write_Pattern_selected()
     *
     * Points a and be are passed by the user to correct for alignments.
     *
     * a2 ---- a3      b2 ---- b3
     * |       |       |       |
     * |       |  ->   |       |
     * a0 ---- a1      b0 ---- b1
     *
     * All a points (from the design) and all b points (measured) must
     * be different, and no three points are allowed to be aligned.
     *
     * Differences and alignment are evaluated to the precision of
     * movement_unit.
     */
//     void set_alignment_points (const Vertex &a0, const Vertex &a1, const Vertex &a2, const Vertex &a3,
//                                const Vertex &b0, const Vertex &b1, const Vertex &b2, const Vertex &b3) ;
#endif

    /** @brief The functions to call to set the points used to align the sample.
     *
     * The version with 6 vertices must be called for alignment_affine
     *
     * * If user sets alignment_none, none of these will be used
     * * If user sets alignment_translation, none of these will be used
     * * If user sets alignment_isometry, the first two points are used
     * * If user sets alignment_similarity, the first two points are used
     *
     * New alignment parameters will be calculated and applied
     * of write_Polyline(), write_Pattern() and write_Pattern_selected()
     *
     * Points a and be are passed by the user to correct for alignments.
     *
     * a2              b2
     * |               |
     * |          ->   |
     * a0 ---- a1      b0 ---- b1
     *
     * All a points (from the design) and all b points (measured) must
     * be different, and are not allowed to be aligned.
     *
     * Differences and alignment are evaluated to the precision of
     * movement_unit.
     **/
//     void set_alignment_points (const Vertex &a0, const Vertex &a1, const Vertex &a2,
//                                const Vertex &b0, const Vertex &b1, const Vertex &b2) ;

    /** @brief The functions to call to set the points used to align the sample.
     *
     * The version with 4 vertices must be called for alignment_isometry or
     * alignment_similarity
     *
     * * If user sets alignment_none, none of these will be used
     * * If user sets alignment_translation, none of these will be used
     *
     * New alignment parameters will be calculated and applied
     * of write_Polyline(), write_Pattern() and write_Pattern_selected()
     *
     * a0 ---- a1 ->   b0 ---- b1
     *
     * a points (from the design) and b points (measured) must
     * be different.
     *
     * Differences and alignment are evaluated to the precision of
     * movement_unit.
     **/
//     void set_alignment_points (const Vertex &a0, const Vertex &a1,
//                                const Vertex &b0, const Vertex &b1) ;

    /** @brief The functions to call to set the points used to align the sample.
     *
     * The version with 2 vertices must be called for alignment_translation
     *
     * * If user sets alignment_none, the point is not used.
     **/
//     void set_alignment_points (const Vertex &a0,
//                                const Vertex &b0) ;

    /** @brief The functions to call to set the points used to align the sample.
     *
     * The version with no vertices clears previous definitions of absence of
     * focus points. It is not compulsory to call it, as setting the parameter
     * alignment in set_all_parametres() is enough to have all possible defined
     * points being ignored.
     *
     * However, just in case the user wants to make sure the definitions are
     * cleared, this function is here.
     **/
//     void set_alignment_points () ;
} ;
} // namespace dpps
#endif