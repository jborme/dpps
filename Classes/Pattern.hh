/**
 * @file Pattern.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for abstract base class Pattern
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The \c Pattern.hh contains one class that describes a larger element called
 * Pattern, which agglomerates many Polyline. It registers all the
 * properties of several designs. This is an abstract class as derivatives
 * Line, Penrose, etc. will implement the logic of the generation of the
 * pattern.
 *
 * Commons remaks for selection functions: default selection is 0, and
 * polylines other than the first are not affected in any way. If the pattern
 * is empty then the selection has no effect. It is possible to know how many
 * lines have been selected by calling Pattern::Selected_size().
 *
 * Note that the last selection is used internally (in interpret.cpp),
 * so that you cannot trust it as a user.
 *
 */

#ifndef DPPS_PATTERN
#define DPPS_PATTERN

#include <array>
#include <random>

#include "common.hh"
#include "util.hh"
#include "Polyline.hh"

namespace dpps {
    typedef enum {
        partial_dash_policy_ignore,
        partial_dash_policy_include,
        partial_dash_policy_fit
    } enum_partial_dash_policy ;

    typedef enum {
        type_uniform_real_distribution,
        type_normal_distribution,
        type_lognormal_distribution,
        type_chi_squared_distribution,
        type_cauchy_distribution,
        type_fisher_f_distribution,
        type_student_t_distribution,
        type_exponential_distribution,
        type_gamma_distribution,
        type_weibull_distribution
    } enum_distribution ;

    typedef enum {
        random_property_position_xy,
        random_property_position_x,
        random_property_position_y,
        random_property_rotation_centred,
        random_property_scale_centred_xy,
        random_property_scale_centred_isotropic,
        random_property_scale_centred_x,
        random_property_scale_centred_y,
        random_property_replace_polygon_random_vertice_number
    } enum_random_property ;

    typedef enum {
        comparison_operator_equal,
        comparison_operator_lower_equal,
        comparison_operator_greater_equal,
        comparison_operator_strictly_lower,
        comparison_operator_strictly_greater,
        comparison_operator_different
    } enum_comparison_operator ;

    typedef enum {
        logical_action_all,
        logical_action_none,
        logical_action_toggle
    } enum_logical_action ;

 /** @brief Pattern is an base class for various types of patterns that
 * can be drawn. Users willing to add more patterns can make them derive from
 * this one.
 *
 * The transforming functions implement an iterator around the function in
 * \c Polyline. Additional comments about these functions can be found in the
 * latter class.
 *
 * @Note Except when explicitely otherwise stated, transforming functions
 * always apply to selection number 0. Exception at the moment are :
 *
 * - fill_will_lines() (it should not be an exception but I'm a bit lazy as
 * this one is more complex).
 * - pop_front() and pop_back() which just operate on the first and last
 * polyline.
 *
 * @Note Writer classe print all polylines (not selected ones)
 *
 */
class Pattern {
// friend class Pattern_factory ;
// friend class Writer ;
// friend class Writer_witec ;
// friend class Typeface ;
public:
    //std::vector<enum_double_property> sort_order ;
    /* @brief generated is set to true when generation is complete */
    // bool generated ;

    /** @brief The pattern is a vector of Polylines. */
    std::vector<Polyline> polylines ;

protected:
    /** @brief A struct to pass encapsulate metadata used by Writer derivatives.
    *
    * Classes derived from Writer can use a struct derived  Writer_settings
    * depending on their needs.
    *
    */
    struct Pattern_settings {} ;

    std::default_random_engine pseudorandom_generator ;
    long_unsigned_int time_seed ;

    bool compare_polylines (const Polyline &p1, const Polyline &p2) ;
public:
    Pattern () ;

    /** @brief the copy constructor */
    Pattern (const Pattern &source) ;

    /** @brief the move constructor. */
    Pattern (Pattern &&source) ;

    /** @brief the move assignment constructor. */
    virtual Pattern &operator= (Pattern &&source) ;

    /** @brief the copy assignment constructor. */
    virtual Pattern &operator= (const Pattern &source) ;

    /** @brief Virtual destructor, so derived classes can have their own.
     *
     * For simple cases of derivatives there is no need to write a destructor
     * as all instances of Polyline are inside a std::vector,
     * so the Polyline destructor will be called automatically when the
     * vector is destroyed.
     */
    virtual ~Pattern () ;

    /** @brief Function to pass metadata to set-up the Writer.
     *
     * To allow the main program to pass generically the arguments that
     * it might find without making much efforts, the simplest solution
     * is to pass separately a vector of int, double and strings. They
     * might be empty if the Writer derivative does not require these
     * parametres. The meaning of these parametres is entirely dependent
     * on the derived classes.
     */
    virtual void set_parametres (const std::vector<bool> &vbool,
                                 const std::vector<long_unsigned_int> &vint,
                                 const std::vector<double> &vdouble,
                                 const std::vector<std::string> &vstring) ;

    /* @brief Informs whether a pattern has been completely generated
     *  already. */
    //bool is_generated () const ;

    /** @brief Generates the full pattern. Complex mathematics are delegated to
     * the derived classes.
     *
     * This function is where the real work is done. Its task is to fill the
     * std::vector<Polyline> polylines with elements
     */ // and set generated = true when it finishes.
    virtual void generate () ;

    //virtual void write_to (Writer &file) ;

    //virtual void write_selected_to (Writer &file) ;

    // Selection_functions
    //////////////////////

    /* @brief The number of polylines of the pattern, whether selected or not.
     *
     * This is one of the few functions that apply to all pattern whether
     * selected or not. Use selection_size() to take into account the selection.
     */
    //long_unsigned_int size_all () const ;

    /** @brief Selects all polylines, or unselect them, or toggles the selection,
     * depeding on the mode. Default action is to select all.
     *
     * logical_operator = 0 : T selects all polylines in the pattern.
     * logical_operator = 1 : F unselects the polylines.
     * logical_operator = 2 : ~ inverts (toggles) the selection.
     *
     * Other values of logical_operator lead to no action.
     *
     */
    void select (const enum_logical_action action,
                 const selection_t selection) ;

    /** @brief Adds to the current selection a range of polylines in the Pattern
     * with their number. Minimum range value is 1, maximum value is size().
     *
     * Selection number can be selected.
     *
     * The number of polylines selected is normally end-begin+1. When begin and
     * end have the same value, then one polyline is selected. When a number
     * outside the polyline range is used (0 or > size()), the part outside the
     * meaningful range is ignored.
     *
     * To select from the beginning, use begin=1. To select from the end, use
     * select_range (pattern.size()-number, pattern.size())
     *
     * Selection is erased and replaced. To be able to apply logical operations
     * like or (add more polylines to the selection) or and (intersection of
     * two selections), function selection_logical_operation() is to be used.
     *
     * All transforming operations are applied to "the selected polylines" refer
     * to selection 0.
     *
     * */

    void select_integer_property (
         const enum_integer_property property,
         const long_unsigned_int parametre,
         const enum_comparison_operator comparison_operator,
         const long_unsigned_int comparison_value,
         const selection_t selection = 0) ;

    void select_double_property (
                  const enum_double_property property,
                  const long_unsigned_int parametre,
                  const enum_comparison_operator comparison_operator,
                  const double comparison_value,
                  double tolerance,
                  const selection_t selection = 0) ;
    void select_uniform_pseudorandom (const double fraction,
                                      const selection_t selection = 0) ;

    /** @brief Apply a logical operation to selections.
     *
     * This method applies to all polylines, not only members of some
     * selection.
     */
    void selection_logical_operation (
                  const selection_t destination_selection,
                  const selection_t selection_p,
                  const selection_t selection_q,
                  const enum_logical_operator logical_operator) ;

    /** @brief Exchange the selection status of two registers.
     *
     * This method applies to all polylines, not only members of some
     * selection.
     */
    void selection_swap (const selection_t selection_p,
                         const selection_t selection_q) ;

    /** @brief Select the same amount of
     * polylines that are currently present in the origin_selection, and adds
     * them to the destination_selection. The third parametre decides whether
     * the initially selected polylines will be unselect or not at the end
     *
     * The polylines are stored in an ordered container. This function will
     * count how many are selected, take note of the index of the last selected,
     * then possibly unselect them (if specified), and select a contiguous
     * amount of polylines, starting just after the last that was selected
     * before. In case the number of lines to select goes beyond the limits,
     * then the selection process stops at the end of the end of the polyline
     * vector (it does not loop).
     *
     * @Note Like the other functions two prefixed with selection_ (
     * selection_swap() and selection_logical_operation()), selection_next()
     * uses two selection arguments.
     */
    void selection_next (const selection_t origin_selection,
                      const selection_t destination_selection,
                      const bool unselect_origin) ;

    /** @brief Adds the first polyline of the vector to the specified selection.
     *
     * This function is intended to be used with select_next(), allowing then
     * to select all polylines consecutively.
     *
     * @note Pattern::select_front() is equivalent to
     * Pattern::select_integer_property (0, 0, 0, 0, 0)
     * */
    void select_front (const selection_t selection = -1) ;

    /** @brief Adds the last polyline of the vector to the specified selection.
     * */
    void select_back (const selection_t selection = -1) ;

    // Iterated functions from Polyline, applying to polyline currently selected
    // (use Pattern::select_all() if it should be applied to all polylines.)
    ////////////////////////////////////////////////////////////////////////////

    /** @brief Symmetry with an x axis at a certain y height. */
    void symmetry_x (const double y0, const selection_t selection = -1) ;

    /** @brief Symmetry with an y axis at a certain x height. */
    void symmetry_y (const double x0, const selection_t selection = -1) ;

    /** @brief Symmetry with an x axis of each Polyline at its average y
     * ordinate. */
    void symmetry_x_centred (const selection_t selection = -1) ;

    /** @brief Symmetry with an y axis of each Polyline at its average x
     * abscissae. */
    void symmetry_y_centred (const selection_t selection = -1) ;

    /** @brief Returns the sum of areas of the selected polylines, calling
     * Polyline::area() */
    double area (const selection_t selection = -1) const ;

    /** @brief Returns the sum of signed areas of the selected polylines,
     * calling Polyline::algebraic_area() */
    double algebraic_area (const selection_t selection = -1) const ;

    /** @brief Returns the sum of the perimiters of the selected polylines,
     * calling Polyline::perimiter() */
    double perimeter (const selection_t selection = -1) const ;

    /** @brief Returns the sum of the length of the selected polylines, calling
     * Polyline::length() */
    double length (const selection_t selection = -1) const ;

    /** @brief Signed version of length() function */
    double algebraic_length (const selection_t selection = -1) const ;

    /** @brief Signed version of perimeter() function */
    double algebraic_perimeter (const selection_t selection = -1) const ;

    /* @brief Returns the number of selected polylines.
     *
     * See selection_size !
     *
    long_unsigned_int count (selection_t selection = -1) const ;*/

    /** @brief Displays the selected polyline in the pattern to stdout */
    void display (const selection_t selection = -1) const ;

    std::string display_string (const selection_t selection = -1) const ;

    /* @brief Displays all the polyline in the patter, independently of their
     * selection flags.
     *
     * This is the only method that applies independently
     * of selection, as it purpose is to allow the user to manually check
     * which lines are selected or not, mostly for verification (debugging)
     * of the programmed recipes.
     */
    //void display_all () const ;

    //std::string display_string_all () const ;

    /** @brief The total number of vertices of the selected polylines */
    long_unsigned_int vertices_size (const selection_t selection = -1) const ;

    /* @brief The total number of vertices of all the polylines */
    //long_unsigned_int vertices_size_all () const ;

    /** @brief The signed smallest x of the vertices of the selected
     * polylines.
     *
     * Like all similar functions, if there are no polylines then 0.0 is
     * returned.
     */
    double minimum_x (const selection_t selection = -1) const ;

    /** @brief The signed largest x of the vertices of the selected
     * polylines.
     *
     * Like all similar functions, if there are no polylines then 0.0 is
     * returned.*/
    double maximum_x (const selection_t selection = -1) const ;

    /** @brief The signed smallest y of the vertices of the selected
     * polylines.
     *
     * Like all similar functions, if there are no polylines then 0.0 is
     * returned.*/
    double minimum_y (const selection_t selection = -1) const ;

    /** @brief The signed largest y of the vertices of the selected
     * polylines. */
    double maximum_y (const selection_t selection = -1) const ;

    Vertex lower_left (const selection_t selection = -1) const ;

    Vertex upper_right (const selection_t selection = -1) const ;

    /** @brief The difference between the maximum x and the minimum x of the
     * selected polylines.
     *
     * Like all similar functions, if there are no polylines then 0.0 is
     * returned.*/
    double horizontal_size (const selection_t selection = -1) const ;

    /** @brief The difference between the maximum y and the minimum y of the
     * selected polylines.
     *
     * Like all similar functions, if there are no polylines then 0.0 is
     * returned.*/
    double vertical_size (const selection_t selection = -1) const ;

    /** @brief Returns the geometrical centre as calculated by the average of
     * the geometrical centre of the selected polylines.
     *
     * In this function, all polylines are equally ponderated. If the polylines
     * have different number of vertices, the vertices are not equally
     * ponderated.
     */
    Vertex geometrical_centre_polylines (const selection_t selection = -1) const ;

    /** @brief Returns the geometrical centre as calculated by the average of
     * the geometrical centre of all the vertices of the selected polylines.
     *
     * In this function, all vertices are equally ponderated.
     */
    Vertex geometrical_centre_vertices (const selection_t selection = -1) const ;

    /** @brief The number of polylines in the specified selection.
     *
     * @Note The number of all polylines is given by pattern. polylines. size().
     */
    long_unsigned_int size (const selection_t selection = -1) const ;

    /** @brief Modifying function that scales each Polyline with respect to its
     *  geometrical centre. Scaling factors can be different along x and y. */
    void scale_centred (const double fx, const double fy, const selection_t selection = -1) ;

    /** @brief Modifying function that scales each Polyline with respect to its
     *  geometrical centre, using the same scaling factor in x and y. */
    void scale_centred (const double f, const selection_t selection = -1) ;

    /** @brief Modifying function that scales each Polyline with respect to
     * the position (v.x, v.y). Scaling factors can be different on x and y. */
    void scale (const Vertex &v, const double fx, const double fy, const selection_t selection = -1) ;

    /** @brief Version of the scale function where the Vertex is given by
     * x and y*/
    void scale (const double x, const double y,
                const double fx, const double fy,
                const selection_t selection = -1) ;

    /** @brief Modifying function that scales each Polyline with respect to
     * the position (v.x0, v.y). Scaling factors can be different on x and y. */
    void scale (const Vertex &v, const double f, const selection_t selection = -1) ;

    /** @brief Version of the scale function where the vertex is given by x
     * and y */
    void scale (const double x, const double y, const double f,
                const selection_t selection = -1) ;

    /** @brief Modifying function that rotates each Polyline with respect to its
     *  geometrical centre. */
    void rotate_centred (const double angle, const selection_t selection = -1) ;

    /** @brief Modifying function that rotates each Polyline with respect to
     * the position (v.x, v.y). */
    void rotate (const Vertex &v, const double angle, const selection_t selection = -1) ;

    /** @brief Version of the rotate function where the vertex is given by
     * x and y*/
    void rotate (const double x, const double y, const double angle, const selection_t selection = -1) ;

    /** @brief Modifying function that transposes the selected elements of the
     * pattern. */
    void transpose (const selection_t selection = -1) ;

    /** @brief Modifying function that translates the polyline by (v.x, v.y). */
    void translate (const Vertex &v, const selection_t selection = -1) ;

    /** @brief Version of the translate function where the Vertex is given by
     * x and y. */
    void translate (const double x, const double y, const selection_t selection = -1) ;

    void remove_aligned_vertices (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

    /** @brief Modifying function that removes consecutive duplicated vertices in
     * each polyline
     *
     * It will remove one of two consecutive vertices which are identical in
     * both x and y. It will not remove identical vertices that are not
     * consecutive.
     * */
    void remove_consecutive_duplicated_vertices (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

    /** @brief Modifying function that removes duplicated vertices in
     * each polyline
     *
     * It will remove one of two consecutive vertices which are identical in
     * both x and y. It will not remove identical vertices that are not
     * consecutive.
     * */
    void remove_duplicated_vertices (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

    /** @brief Modifying function that removes consecutive duplicated polylines.
     *
     * Duplicated polylines are evaluated to a specific precision. By default,
     * this precision is the machine default for the double type. This function
     * only removes consecutive duplicated and is O(n) in commplexity, where n
     * is the number of polylines in the pattern.
     * */
    void remove_consecutive_duplicated_polylines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

    /** @brief Modifying function that removes consecutive duplicated polylines.
     *
     * Duplicated polylines are evaluated to a specific precision. By default,
     * this precision is the machine default for the double type. This function
     * removes all duplicated polylines and is O(n^2) in commplexity, where n
     * is the number of polylines in the pattern.
     * */
    void remove_duplicated_polylines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

     /** @brief Replaces each selected polyline by a series of polyline
     * containing each just one Vertex.
     *
     * All newly created polylines keep the selection property of their parent.
     * This function can be then used with replace_by() and
     * replace_by_regular_polygon(), which then enable to replace each Vertex
     * with a given Polyline.
     * */
    void explode_in_vertices (const bool keep_initial,
        const selection_t selection = -1) ;

     /** @brief Replaces each selected polyline by a series of polyline
     * containing each just two vertices.
     *
     * All newly created polylines keep the selection property of their parent.
     * */
    void explode (const bool keep_initial, const selection_t selection = -1) ;

    /** @brief Replaces each polyline with another one at the same place.
     *
     * If translate is set to true, then the pattern is translated so that the
     * the new one so that the geometrical centre of the new is at the same
     * place as the geometrical centre of the former.
     *
     * if keep_initial_properties is set to true, then only vertices are
     * copied, all other properties of the former polylines
     * (selection, reference, dose) are kept. User should keep in mind that the
     * dose might have a different meaning in the final application for
     * vertices in open and closed shapes, in case shapes of different closures
     * are replaced.
     * */
    void replace_by (const Polyline &polyline, const bool centre = true, const bool keep_initial_properties = true, const selection_t selection = -1) ;

    /** @brief Replaces each polyline by a set of polylines provided as a
     * Pattern.
     *
     * If translate is set to true, then the pattern is translated so that the
     * geometrical centre of the inserted pattern
     * (geometrical_centre_polylines()) is identical to the geometrical centre
     * of the replaced Polyline. This is be done by calling
     * Polyline::replace_by(). If translate is set to false, the argument
     * polyline is just copied on top of the previous polyline.
     *
     * Only vertices are copies, all other properties of the former polylines
     * (selection, reference, dose) are kept. user should keep in mind that the
     * dose might have a different meaning in the final application for
     * vertices than what it has for closed shape, though.
     * */
    void replace_polyline_by_pattern (const Pattern &p, const bool centre = true, const bool keep_initial_properties = true, const selection_t selection = -1) ;

    /** @brief Replaces each polyline by a regular polygon, calling replace_by()
     * and the Polyline constructor with two arguments.
     *
     * The reguar polygon is centred on the geometrical centre of each polyline
     * that is substitutes.
     *
     * Only vertices are copies, all other properties of the former polylines
     * (selection, reference, dose) are kept.
     */
     void replace_by_regular_polygon (const long_unsigned_int number_vertices,
                                      const double diametre,
                                      const bool keep_initial_properties = true,
                                      const selection_t selection = -1) ;

    /** @brief Function which converts each selected closed Polyline into a
     * vector of two-vertices Polyline that can be drawn along a line.
     */
    void fill_with_lines (const double angle, const double spacing,
                          const bool keep_initial,
                          const selection_t selection = -1) ;

    /** @brief Function which converts each selected closed Polyline into a
     * vector of two-vertices Polyline that can be drawn along a line. The angle
     * of lines of taken to be equal to the first direction of each
     * Polyline.
     *
     * The properties of the old polyline (selection, reference, dose) are
     * preserved in the new polylines.
     *
     * This function calculates the angle and calls fill_will_lines() with
     * this value.
     *
     * @sa fill_will_lines()
     */
    void fill_with_lines_first_angle (const double spacing,
                                      const bool keep_initial,
                                      const selection_t selection = -1) ;

    /** @brief Function which converts each selected closed Polyline into a
     * a vector of single dots, filling the area of the polyline (which is
     * removed) with a regular pattern of dots.
     *
     * This function calls fill_with_lines()
     */
    void fill_with_dashes (const double angle,
                    const double spacing_longitudinal,
                    const double spacing_transversal,
                    const double duty_cycle,
                    const double phase_initial,
                    const double phase_increment,
                    const enum_partial_dash_policy partial_policy,
                    const bool keep_initial,
                    const selection_t selection = -1) ;

    /** @brief Variant of fill_with_dash which uses the first angle as the
     * longitudinal direction.
     *
     * This function calls fill_with_lines_first_angle()
     */
    void fill_with_dashes_first_angle (
                const double spacing_longitudinal,
                const double spacing_transversal,
                const double duty_cycle,
                const double phase_initial,
                const double phase_increment,
                const enum_partial_dash_policy partial_policy,
                const bool keep_initial,
                const selection_t selection = -1) ;

    /** @brief Duplicates the selected polylines. The initial polyline are
     * unselected from selection 0, the newly created are selected in 0.
     *
     * All the other properties are kept unchanged (including selections other
     * than zero, reference, openness, dose).
     */
    void duplicate (const selection_t selection = -1) ;

    /** @brief Reverses the order of the vertices in the selected polylines.
     *
     * This changes the sign of algebraic_area and also will affect the order
     * in which Writer_witec draws the lines inside polylines.
     */
    void reverse_vertices (const selection_t selection = -1) ;

    /** @brief Rounds all vertices of all polylines to a certain precision */
    void round_to (const double precision, const selection_t selection = -1) ;

    // Iterated functions which do not correspond to a function call in
    // Polyline but rather change the value of inside the Polyline directly
    ///////////////////////////////////////////////////////////////////////

    /** @brief Change the dose of the selected polylines by setting a
     * new value */
    void set_dose (const double dose_value, const selection_t selection = -1) ;

    void set_dose_increasing (const double dose_initial,
                              const double dose_final,
                              const selection_t selection = -1) ;

    /** @brief Change the dose of the selected polylines by adding a constant
     * value (positive or negative) to the dose of each.
     *
     * Negative values and negative resulting values are accepted. */
    void add_to_dose (const double additive_constant,
        const selection_t selection = -1) ;

    /** @brief Change the dose of the selected polylines by multiplying their
     * dose by a constant value (positive or negative).
     *
     * Negative values and negative resulting values are accepted. */
    void multiply_dose (const double multiplication_factor,
        const selection_t selection = -1) ;

    /** @brief Change the reference property of the selected polylines by
     * setting a new value (positive integer) */
    void set_reference (const long_unsigned_int reference_value,
        const selection_t selection = -1) ;

    /** @brief Set the reference of the selected plylines to values increasing
     * in numbers starting at 0 */
    void set_reference_increasing (const selection_t selection = -1) ;

    /** @brief Add a number to the reference of the selected polylines. */
    void add_to_reference (const long_unsigned_int additive_constant,
        const selection_t selection = -1) ;

    /** @brief Retract a number to the reference of the selected polylines.
     *
     * If the subtracted number is larger than the current value, new value
     * is set to 0 (no underflow will occurr).
     */
    void subtract_to_reference (const long_unsigned_int additive_constant,
        const selection_t selection = -1) ;

    /** @brief Erases the selected polylines.
     *
     * This function can cost time if selected polylines are located very far
     * from the end of the vector. This is because it calls std::vector::erase()
     * on each selected Polyline, whose complexity is linear with the distance
     * between the element to be erased and the end of the vector.
     */
    void erase (const selection_t selection = -1) ;

    /** @brief Set the selected polylines to closed (true) or open (false). */
    void set_closure (const bool closure_value,
        const selection_t selection = -1) ;

    /** @brief Toggle the status of closure of selected polylines. */
    void toggle_open_close (const selection_t selection = -1) ;

    /** @brief Appends one polyline to the pattern. */
    void push_back (const Polyline &p) ;

    /* @brief Prepends one polyline to the pattern.
    void push_front (const Polyline &p) ;*/

    /** @brief Returns the last polyline and deletes it, whether selected
     * or not. */
    Polyline pop_back () ;

    /* @brief Deletes the first polyline (whether selected or not).
    void pop_front () ;*/

    /* @brief Appends the current instance to the pattern passed in
     * argument.
    void append_to (Pattern &p) const ;*/

    /** @brief Appends the pattern passed as argument to the current
     * instance. */
    void append_from (const Pattern &p, const selection_t selection = -1) ;

    /* @brief Returns a pattern with only the selected polylines.
     *
     * See Pattern_factory::Pattern_from_selected
    Pattern pattern_selected (selection_t selection = -1) ; */

    /** @brief Returns the polyline at the specific number */
    Polyline get_polyline (long_unsigned_int number) ;

    /** @brief Applies pseudo-random operations, being able to alter position,
     * angle, etc. according to several random distributions.
     *
     * Note that property 0 is bivariate, while all other are monovariate.
     */
    void random_update_property (const enum_random_property property,
                  const enum_distribution random_distribution,
                  const double p1,
                  const double p2,
                  const selection_t selection = -1) ;

    //std::vector<long_unsigned_int> vector_references () const ;

    /** @brief Classifies the order of storage (in memory) of polylines
     * in increasing order of one or a several criteria.
     *
     * Order of polylines with respect to criteria not mentioned as
     * sorting key os left unchanged.
     * */
    void sort (const std::vector <enum_double_property> &properties,
               const bool increasing,
               const selection_t selection = -1) ;

    /** @brief reverses the order of storage of selected polylines
     *
     * This is designed to called after sort(), in case decreasing
     * order is required instead of increasing.
     */
    void reverse_order (const selection_t selection = -1) ;

    /** @brief Convenience function that calls sort() with one criterion */
    void sort1 (const enum_double_property property,
                const bool increasing,
                const selection_t selection = -1) ;

    /** @brief Convenience function that calls sort() with two criteria */
    void sort2 (const enum_double_property property1,
                const enum_double_property property2,
                const bool increasing,
                const selection_t selection = -1) ;

    /** @brief Convenience function that calls sort() with three criteria */
    void sort3 (const enum_double_property property1,
                const enum_double_property property2,
                const enum_double_property property3,
                const bool increasing,
                const selection_t selection = -1) ;

    void shuffle (const selection_t selection = -1) ;
    /** @brief Sets selected Polylines of the pattern to use the metadata (dose,
     * reference, closure, selection registers) from a specific polyline. */
    void set_metadata_from (const Polyline &p, const selection_t selection = -1) ;

    /** @brief Distribute a property of the selected polylines between
     * extrema values, according to a model.
     *
     * An example of use case would be to distribute the centre of each
     * polylines in a linear model between the minimum and maximum, in a way to
     * equalize their positions. The user must make sure the extrema values
     * of the property among the polylines is correctly set.
     *
     * The polylines are updated in the order they are present in the
     * vector. If polylines are distinguishable (they do not have the same shape,
     * or their order is important), user should consider calling
     * sort() (and reverse_order()) or shuffle() before distribute(), in order
     * to define in which order the polylines will be distributed.
     */
    void distribute (
        const enum_progression_model model,
        const enum_double_property property,
        const long_unsigned_int parametre,
        const selection_t selection = -1) ;

    /** @brief Returns the extremal points of the selected polylines (and their
     * vertices).
     *
     * If dots_as_circles is set to true, single-vertex polylines are considered
     * circles of radius set by the dose (situation happens with the DXF R12
     * reader).
     *
     * If no polylines are selected, minimum and maximum are left unchanged. */
    void limits (
                    double &minimum_x,
                    double &minimum_y,
                    double &maximum_x,
                    double &maximum_y,
                    const bool dots_as_circles = false,
                    const selection_t selection = -1) const ;
    /** @brief Returns the minimum value of the property among the selected
     * polylines.
     *
     * If no polylines are selected, 0.0 is returned.
     * This is a convenience function that calls get_limits_double_property()
     * and returns the minimum.
     */
    double get_minimum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;

    /** @brief Returns the maximum value of the property among the selected
     * polylines.
     *
     * If no polylines are selected, 0.0 is returned.
     * This is a convenience function that calls get_limits_double_property()
     * and returns the maximum.
     */
    double get_maximum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;

    /** @brief Sets the minimum and maximum value of the property among the
     * selected polylines.
     *
     * If no polylines are selected, minimum and maximum are left unchanged. */
    void get_limits_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    double &minimum,
                    double &maximum,
                    const selection_t selection = -1) const ;
    /** @brief Returns the maximum value of the property among the selected
     * polylines.
     *
     * If no polylines are selected, 0.0 is returned.
     * This is a convenience function that calls get_limits_integer_property()
     * and returns the minimum.
     */
    long_unsigned_int get_minimum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;
    /** @brief Sets the minimum and maximum value of the property among the
     * selected polylines.
     *
     * If no polylines are selected, minimum and maximum are left unchanged.
     * This is a convenience function that calls get_limits_integer_property()
     * and returns the maximum.
     */
    long_unsigned_int get_maximum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;

    /** @brief Sets the minimum and maximum value of the property among the
     * selected polylines.
     *
     * If no polylines are selected, minimum and maximum are left unchanged. */
    void get_limits_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    long_unsigned_int &minimum,
                    long_unsigned_int &maximum,
                    const selection_t selection = -1) const ;

    /** @brief Transforms the selected polylines in a way to match the set value
     * of a double property.
     *
     * This function could have been called align(), however certain
     * the properties it can set (suc as the dose) are not a localization,
     * therefore a more general name was chosen.
     */
    void update_double_property (
            const enum_double_property property,
            const long_unsigned_int parametre,
            const double value,
            const selection_t selection = -1) ;

    /* @brief Polylines which are selected in selection will be unselected
     * from all selection registers. */
    //void unselect (const selection_t selection = -1) ;

    void overwrite_selected (const Pattern &p, const selection_t selection = -1) ;

    /** @brief returns true if all selected polylines in the pattern are
     * convex, false if at least one is not convex. */
    bool is_convex (const selection_t selection = -1) const ;

    void join_successive_intersecting_lines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void join_intersecting_lines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;

    void subdivide (const double distance, const selection_t selection = -1) ;
    void reorder_vertices_by_angle(const selection_t selection = -1) ;
#ifdef GSL_FOUND
    void apply_matrix (Matrix3x3 &matrix, const selection_t selection = -1) ;
    void apply_homography (Matrix3x3 &matrix, const selection_t selection = -1) ;
#endif
    /** @brief performs a cycle (french permutation circulaire) or rotation
     * on the vertices of each selected polyline. */
    void cycle (long_unsigned_int val, const bool forward = true,
                const selection_t selection = -1) ;

    /** @brief performs a cut_into with the argument polyline to each selected
     * polyline in the pattern */
    void cut_into (Polyline p,
                   const long_unsigned_int pos_this,
                   const long_unsigned_int pos_p,
                   const bool reverse_auto = true,
                   const selection_t selection = -1) ;
    /** @brief performs a cut_into with the argument polyline to each selected
     * polyline in the pattern */
    void cut_into (Polyline p, const bool reverse_auto = true,
        const selection_t selection = -1) ;

    /** @brief Check if selected polylines have a last vertex identical to the
     * first (which is sometimes used as a visual way to close a polyline),
     * to the specified precision, and if the case, then removes the last
     * vertex and sets the polyline to closed.
     */
    void check_closed(const double precision = std::numeric_limits<double>::epsilon (), const selection_t selection = -1) ;

} ;
} //namespace dpps
#endif
