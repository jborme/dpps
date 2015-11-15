/**
 * @file Polyline.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for classes Polyline and Vertex
 * @date 2013-12-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The \c Polyline.hh contains one class that describes the basic element
 * of which our designs are made of: the class Polyline.
 *
 */
#ifndef DPPS_POLYLINE
#define DPPS_POLYLINE

#include <vector>
#include <fstream>
#include <bitset>
#include <limits>

#include "common.hh"
#include "Vertex.hh"
#include "Matrix3x3.hh"
#include "util.hh" // dpps_to_string

namespace dpps {
    typedef enum {
        double_property_minimum_x,
        double_property_maximum_x,
        double_property_minimum_y,
        double_property_maximum_y,
        double_property_geometrical_centre_x,
        double_property_geometrical_centre_y,
        double_property_distance_to_centre,
        double_property_horizontal_size,
        double_property_vertical_size,
        double_property_minimum_distance,
        double_property_maximum_distance,
        double_property_orientation_at_vertex,
        double_property_turning_angle_at_vertex,
        double_property_area,
        double_property_perimeter,
        double_property_dose
    } enum_double_property ;

    typedef enum {
        integer_property_position,
        integer_property_number_vertices,
        integer_property_number_vertices_modulo,
        integer_property_closeness,
        integer_property_reference
    } enum_integer_property ;

/** @brief A class to describe the basic element of our designs, the Polyline.
 *
 * Polylines are defined as a vector of x and a vector of y positions.
 * Currently only adding vertices to a polyline is supported.
 *
 * A polyline can be either closed or open. This distinction has consequences
 * in future uses of the design. Usually closed polylines are filled with ink
 * while open polylines are considered as perimiter.
 */
class Polyline {
public:
     /** @brief vector of x/y positions. */
    std::vector<Vertex> vertices ;

    /** @brief Whether the polyline is to be considered a closed (filled)
     * polygon or an open path. */
    bool closed = false ;

    /** @brief A numerical value associated to the instrumental process of the
     * polyline. Signification depends of the writer used.
     *
     * Current versions of dxf and svg writers ignore it, Witec writer uses it.
     *
     * For Witec writer this is the linear speed of the stage (micrometres per
     * second), or the duration of the dot (in milliseconds) if the Polyline
     * has just one dot. Zero means the default value of the writer is used.
     */
    double dose = 0.0 ;

    /** @brief A numerical value associated with the polyline, to set freely.
     * The Pattern_Penrose_tiling uses it to store the type of tile.
     */
    long_unsigned_int reference = 0 ;

    // One day, we have to support layers. We can use just a reference number.
    // long_unsigned_int layer ;

    /** @brief Whether the polyline is currenlty selected or not.
     *
     * Polylines can be part of a selection, which is a flag whether they are
     * selected or not. There are several independent selections, usually
     * 8, counted from 0 to 7.
     *
     * @note the number of selections registers is set in common.h, it is
     * equal to the number of bits of type selection_t, which then later is
     * used to allocate the std::bitset element.
     *
     * Most functions in Pattern class expose a «selection» parameter (an
     * integer). In this case, the Pattern function is applied to the polylines
     * which are selected. Functions in Pattern also accept a negative value
     * for a selection. In this case the function applies to all polylines
     * whether they are selected or not. The negative selection (all polylines)
     * is the default behaviour of the Pattern class.
     *
     * In case the user wants to apply a function to selected polylines in a
     * Pattern, a selection function can be used.
     * For example function call Pattern::select(logical_operation_all, 0)
     * mark all polyline as selected in selection 0. Later calls to Pattern
     * function referring to selection 0 will be applied to all polylines.
     * Calls referring to selection 1, 2 etc. would lead to no polyline being
     * altered as we did not in this example select any polyline in selection 1,
     * 2, etc.
     *
     * The 8 seperate selections and the boolean operations between selection
     * allow to build a complex multi-criteria selection of Polylines.
     *
     * @note: A user who does not care about selection registers can just use
     * selection -1, which is not a real selection and means «All polylines»,
     * or select certain Polylines to selection 0. the other 7 selection were
     * added also because they do not
     * consume memory at all, as allocating a bool or a 8 bit bitset uses
     * exactly one byte in each polyline.
     *
     * All functions which accept a selection parameter perform bound check
     * before reading the bitset.
     *
     * */
    std::bitset <polyline_max_selection> selected ;

    // Constructors, etc. //
    ////////////////////////

    /** @brief Constructor for an empty polyline */
    Polyline () ;

    /** @brief Constructor for an polyline starting with a first vertex */
    Polyline (const Vertex &first) ;

    /** @brief Explicit copy constructor, so that it can be bound by SWIG
     * and used in bindings.
     *
     * In the Lua binding, the = operator creates a reference to the same
     * memory content.
     * a = dpps.Polyline()
     * b = a
     * a and b refer to the same object in memory, therefore changing a or b
     * also changes the other. In order to make a copy, one needs to call the
     * copy constructor:
     * a = dpps.Polyline()
     * b = dpps.Polyline(a)
     * This way a and b can be altered separately
     */
    Polyline (const Polyline &p) ;

    /** @brief Move constructor */
    Polyline (Polyline &&p) ;

    /** @brief copy assignment operator */
    Polyline &operator= (const Polyline &p) ;

    /** @brief move assignment operator */
    Polyline &operator= (Polyline &&p) ;

    ~Polyline () ;

    /** @brief Constructor for a regular closed polygon with a number of
     * vertices and the diametre of the circumsribed circle */
    Polyline (const long_unsigned_int number_vertices, const double diametre) ;

    /* @brief Constructor using a vector of vertices.
     *
     * Calling this constructor is equivalent to declarig an empty polyline and
     * calling push_back_vector().
     *
    Polyline (const std::vector<dpps::Vertex> &v) ;*/

    Polyline (const long_unsigned_int number_vertices,
                    const double major_axis,
                    const double minor_axis,
                    const double angle_x)  ;

    /** @brief Constructor using a vector of pairs of double
     *
     * This constructor does the same as passing a vector of Vertex and will
     * handle pair to Vertex converion internally.
     */
    Polyline (const std::vector<std::pair<double, double>> &p) ;

    /** @brief Constructor using a vector of pairs of float
     *
     * Polyline is internally defined as double, but sometimes we might want
     * to input floats. For constructors based on fundamental types there
     * are implicit conversions, but for the vector of float we must explicitely
     * write the header.
     *
     * This function is called in the Typeface class.
     */
    Polyline (const std::vector<std::pair<float, float>> &p) ;

    /** @brief returns the vertex at position i, which is the same as
     * vertices[i]. If i is not a valid index, vertex (0,0) is returned,
     * and no error happens.
     * */
    Vertex get_vertex (const long_unsigned_int i) const ;

    /** @brief sets the vertex at position i, which is the same as
     * setting directly vertices[i]. If i is not a valid index, nothing happens.
     * */
    void set_vertex (const long_unsigned_int i, const Vertex &v) ;

    void select (const bool status, const selection_t) ;

    /** @brief Removes all selection registers from the Polyline. */
    void select_all (const bool status) ;

    void toggle (const selection_t) ;

    void toggle_all () ;

    bool get_select (const selection_t) ;

    /** @brief Grows the Polyline by one vertex */
    void push_back (const Vertex &v) ;

    /**  @brief grow a polyline by appending the argument */
    void push_back (const Polyline &p) ;

    /** @brief Grows the Polyline by one vertex in relative coordinates */
    void push_back_relative (const Vertex &v) ;

    /** @brief Grows the Polyline by one vertex, described by x and y */
    void push_back (const double x, const double y) ;

    /** @brief Grows the Polyline by one vertex described by x and y,
     * in relative coordinates */
    void push_back_relative (const double x, const double y) ;

    // const informative functions //
    /////////////////////////////////

    bool operator== (const Polyline &p) const ;

    Polyline operator+ (const Vertex &v) const ;

    Polyline operator- (const Vertex &v) const ;

    Polyline operator* (const double m) const ;

    Polyline operator/ (const double m) const ;

    bool equals_to (const Polyline &p,
        const double precision = std::numeric_limits<double>::epsilon()) const ;

    /** @brief Writes the coordinates of the vertices to the standard output.
     *
     * This really just displays the result of display_string () to the
     * standard output.
     */
    void display () const ;

    /** @brief Returns a string description of the coordinates of the vertices
     * to the standard output. It will also show a decimal representation of
     * the selection value, in case the polyline is member of any selection
     * but the last (which is for internal purposes).
     *
     */
    std::string display_string () const ;

    /** @brief Informative function that returns the number of vertices in the
     * Polyline. Informative. */
    long_unsigned_int size () const ;

    /** @brief Informative function that returns the angle between the vertex
     * i and vertex i+1.
     *
     * If i+1 is beyond end, first is used. If i is beyond end, the modulus
     * by number of vertices is used.
     *
     * This function catches exceptions of atan2. A domain_error can happen if
     * the two vertices (positions i and i+1) are identical. In this case, a
     * message is printed on the standard output including a call to display()
     * and the angle returned is 0.0. We could make it return NaN but that would
     * complexify the caller function, the user should just fix the Polyline
     * that was created by calling remove_duplicated_vertices () on it.
     *
     */
    double orientation_at_vertex (long_unsigned_int i) const ;

    /** @brief Informative function that returns a Vertex corresponding to
     * the geometrical centre (centre of mass) of teh polyline.
     *
     * If the polyline is self-crossing then the calculation is performed on
     * a non-self crossing version of the same vertices. See
     * reorder_vertices_by_angle()
     */
    Vertex geometrical_centre () const ;

    /** @brief The signed smallest x of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    double minimum_x () const ;

    /** @brief The signed largest x of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    double maximum_x () const ;

    /** @brief The signed smallest y of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    double minimum_y () const ;

    /** @brief The signed largest y of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    double maximum_y () const ;

    /** @brief The signed smallest and largest vertices.
     *
     * This function calculates the four limits at the same time, which is
     * more efficient than calling the four other functions, in particular when
     * called on a large number of polylines.
     *
     * If the polyline is empty, the values of the variables are not changed.
     */
    void limits (double &minx, double &miny, double &maxx, double &maxy) const ;

    Vertex lower_left () const ;

    Vertex upper_right () const ;

    /** @brief The difference between maximum_x() and minimum_x().
     *
     * It could run faster if we made one loop instead of calling the two
     * functions, but it's probably not the most intensive function in terms
     * of execution time. */
    double horizontal_size () const ;

    /** @brief The difference between maximum_y() and minimum_y()
     *
     * It could run faster if we made one loop instead of calling the two
     * functions, but it's probably not the most intensive function in terms
     * of execution time. */
    double vertical_size () const ;

    /** @brief The maximum distance between all combinations of vertices.
     *
     * This function runs in O(vertices. size ()^2)
     */
    double maximum_distance () const ;

    /** @brief The minimum distance between all combinations of vertices.
     *
     * This function runs in O(vertices. size ()^2) */
    double minimum_distance () const ;

    /** @brief Returns the area of the closed polygon described by the Polyline.
     *
     * The value is the same whether the Polyline is set to closed or not.
     * If the polyline is self-crossing then the calculation is performed on
     * a non-self crossing version of the same vertices. See
     * reorder_vertices_by_angle()
     */
    double area () const ;

    /** @brief Returns the signed area of the closed polygon described by the
     * Polyline.
     *
     * The sign depends on the orientation, and is positive for rotation in the
     * direct (trigonometric) direction.
     *
     * If the polyline is self-crossing then the calculation is performed on
     * a non-self crossing version of the same vertices. See
     * reorder_vertices_by_angle()
     */
    double algebraic_area () const ;

    /** @brief Returns the sign of a polygon considered a closed area element,
     * which is 1 if it is rotating in the trigonometric direction, -1 if
     * rotating in the opposite direction. If area is zero then value returned
     * is zero.
     */
    short signed int orientation () const ;

    /** @brief Signed version of length() function, which returns the product
     * of the length by the orientation (hence zero if area is zero). */
    double algebraic_length () const ;

    /** @brief Signed version of perimeter() function, which returns the product
     * of the length by the orientation (hence zero if area is zero). */
    double algebraic_perimeter () const ;

    /** @brief Returns the perimiter of the closed polygon described by the
     * Polyline.
     *
     * The value is the same whether the Polyline is set to closed or not.
     * @sa length()
     */
    double perimeter() const ;

    /** @brief Returns the length of the open line described by the Polyline.
     *
     * The value is the same whether the Polyline is set to closed or not.
     * @sa perimeter()
     */
    double length () const ;

    // Functions transforming the Polyline //
    /////////////////////////////////////////
    /** @brief Transposing, which is swapping x and y coordinates */
    void transpose () ;

    /** @brief Modifying function that translates all vertices by constant
     * (v.x, v.y) value */
    void translate (const Vertex &v) ;

    /** @brief translate function with the vertex defined by x and y */
    void translate (const double x, const double y) ;

    /** @brief Modifying function that scales the Polyline with respect to its
     *  geometrical centre. Scaling factors can be different along x and y. */
    void scale_centred (const double fx, const double fy) ;

    /** @brief Modifying function that scales the Polyline with respect to its
     *  geometrical centre, using the same scaling factor in x and y. */
    void scale_centred (const double f) ;

    /** @brief Modifying function that scales the Polyline with respect to
     * the position (v.x, v.y). Scaling factors can be different on x and y. */
    void scale (const Vertex &v, const double fx, const double fy) ;

    /** @brief Scale function with the vertex defined by x and y. */
    void scale (const double x, const double y,
                const double fx, const double fy) ;

    /** @brief Modifying function that scales the Polyline with respect to
     * the position (v.x0, v.y), with the same factor in x and y (isotropic
     * scale).
     */
    void scale (const Vertex &v, const double f) ;

    /** @brief Isotropic scale function with vertex defined as x and y */
    void scale (const double x, const double y, const double f) ;

    /** @brief Modifying function that rotates the Polyline with respect to its
     *  geometrical centre. */
    void rotate_centred (const double angle) ;

    /** @brief Modifying function that rotates the Polyline with respect to
     * the position (v.x, v.y). */
    void rotate (const Vertex &v, const double angle) ;

    /** @brief Rotate function with the vertex defined as x and y */
    void rotate (const double x, const double y, const double angle) ;

    /** @brief Symmetry with an x axis at a certain y height. */
    void symmetry_x (const double y0) ;

    /** @brief Symmetry with an y axis at a certain x height. */
    void symmetry_y (const double x0) ;

    /** @brief Symmetry with an x axis at Polyline average y ordinate. */
    void symmetry_x_centred () ;

    /** @brief Symmetry with an y axis at Polyline average x abscissae. */
    void symmetry_y_centred () ;

    /** @brief Modifying function that removes consecutive duplicated vertices
     *
     * It will remove one of two consecutive vertices which are identical in
     * both x and y. It will not remove identical vertices that are not
     * consecutive.
     *
     * If the Polyline is closed, then the first and the last are considered
     * consecutive.
     * */
    void remove_consecutive_duplicated_vertices (
        double precision = std::numeric_limits<double>::epsilon ()) ;

    /** @brief Modifying function that removes consecutive aligned vertices
     *
     * This allows to simplify polygons that have extraneous points which are
     * aligned. Tolerance is given in radians. Note that duplicated vertices
     * are not removed (because they are detected as orthogonal, not
     * as aligned).
     * */
    void remove_aligned_vertices (
        double precision = std::numeric_limits<double>::epsilon ()) ;

    /** @brief Modifying function that removes duplicated vertices
     *
     * This function will remove all duplicated vertices, to the specified
     * precision. It needs to compare all vertices two by two so it has O(n^2)
     * complexity, with n the number of vertices.
     * */
    void remove_duplicated_vertices (
        double precision = std::numeric_limits<double>::epsilon ()) ;

    /** @brief Replaces the current polyline with another one at the same place,
     * translating the new one so that the geometrical centre of the new
     * is at the same place as the geometrical centre of the former.
     *
     * If keep_initial_properties is set to true the properties of *this (dose,
     * closed, reference, selection) are preserved, otherwise the properties
     * of the argument are used.
     * */
    void replace_by (Polyline polyline, const bool keep_initial_properties = true) ;

    /** @brief Replaces each polyline by a regular polygon, calling replace_by()
     * and the Polyline constructor with two arguments.
     *
     * If keep_initial_properties is set to true the properties of *this (dose,
     * closed, reference, selection) are preserved, otherwise the properties
     * of the argument are used.
     */
    void replace_by_regular_polygon (const long_unsigned_int number_vertices,
                                     const double diametre,
                                     const bool keep_initial_properties = true) ;

    /** @brief Rounds the vertices of the polyline to a certain precision.
     *
     * This performs a sort of clean-up of decimals, for example when keeping
     * precision is not useful for an export to a writer.
     */
    void round_to (
        double precision = std::numeric_limits<double>::epsilon ()) ;

    /** @brief Reverses the order of the vertices.
     */
    void reverse_vertices () ;

    // The functions below were added only to provide a similar API to
    // the one availabel in Pattern. However, reference, dose and closure
    // are public, so it is not compulsory to use these functions.
    /** @brief Changes the dose of the polyline by setting a new value. */
    void set_dose (const double dose_value) ;

    /** @brief Change the dose of the polyline by adding a constant
     * value (positive or negative) to the dose.
     *
     * Negative values and negative resulting values are accepted. */
    void add_to_dose (const double additive_constant) ;

    /** @brief Changes the dose of the polyline by multiplying the
     * dose by a constant value (positive or negative).
     *
     * Negative values and negative resulting values are accepted. */
    void multiply_dose (const double multiplication_factor) ;

    /** @brief Changes the reference property of the polyline by
     * setting a new value (positive integer) */
    void set_reference (const long_unsigned_int reference_value) ;

    /** @brief Add a number to the reference of the polyline. */
    void add_to_reference (const long_unsigned_int additive_constant) ;

    /** @brief Retract a number to the reference of the polyline.
     *
     * If the subtracted number is larger than the current value, new value
     * is set to 0 (no underflow will occurr).
     */
    void subtract_to_reference (const long_unsigned_int additive_constant) ;

    /** @brief Set the selected polylines to closed (true) or open (false). */
    void set_closure (const bool closure_value) ;

    /** @brief Toggle the status of closure of selected polylines. */
    void toggle_open_close () ;

    /** @brief Clears the vertices of the polyline, keeping other properties */
    void erase () ;

    /** @brief Clears a range of vertices.
     *
     * If first boundary is out of range, nothing is done. If second boundary is
     * out of range, it is understod as erasing until the last vertex. */
    void erase (const long_unsigned_int start, long_unsigned_int end) ;

    /** @brief Sets the metadata of the Polyline (dose, reference, closure,
     * selection registers) to the values of another Polyline. */
    void set_metadata_from (const Polyline &p) ;

    double get_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre) const ;
    long_unsigned_int get_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre) const ;

    /** @brief Transforms the polyline in a way to match the set value
     * of a double property.
     *
     * If property is
     * * double_property_minimum_x
     * * double_property_maximum_x
     * * double_property_geometrical_centre_x
     * then a translation along x is done with value such that the property in
     * question will have the new value.
     *
     * If property is
     * * double_property_minimum_y
     * * double_property_maximum_y
     * * double_property_geometrical_centre_y
     * then a translation along y is done.
     *
     * If property is double_property_vertical_size, then a scale along y is
     * done (anisotropic scale).
     *
     * If property is double_property_horizontal_size, then a scale along x is
     * done (anisotropic scale).
     *
     * If property is double_property_orientation_at_vertex, then a centred rotation
     * is done
     *
     * If property is
     * * double_property_area
     * * double_property_minimum_distance,
     * * double_property_maximum_distance
     * * double_property_perimeter
     * Then an isotropic scaling is done. Note that the scaling ratio for
     * double_property_area and for the remaining three is not the same.
     *
     * If property is double_property_dose, then the dose is changed to the new
     * value.
     *
     * In the case of the scaling operations, no operation is performed if the
     * initial value of the parameter is zero (less than machine epsilon in
     * absolute value), in order to avoid dividing by zero.
     */
    void update_double_property (const enum_double_property property,
                                 const long_unsigned_int parametre,
                                 const double value) ;
    bool multicriteria_less_than (
        const Polyline &p,
        std::vector<dpps::enum_double_property> &sort_order,
        const bool increasing
    ) const ;

    /** @brief Returns true if the polyline defines a convex polygon.
     *
     * Polylines with 0, 1, 2 or 3 vertices will always return true.
     */
    bool is_convex () const ;

    /** @brief Returns true if the closed polygon described by the polyline
     * is self-crossing.
     *
     * If polyline is open, self-crossing only considers the open polyline
     * from vertex 0 to size()-1. If polyline is closed, the final line from
     * last vertex to the first one is considered as well.
     *
     * @note Duplicate vertices, different lines are joining at the same place
     * do not count as intersection. Only line segments crossing strictly inside
     * (not at their extremities) will cause true.
     *
     * Polylines with 0, 1, 2 or 3 vertices will always return false.
     */
    bool is_self_crossing () const ;

    /** @brief Returns the angle created by vertices (i-1, i, i+1)
     *
     * If i = 0, then angle is between (last, 0, 1). if i == last, then the
     * angle is between (last, 0, 1). This behaviour does not depend on the
     * polyline being open or closed.
     *
     * If i is beyond end, the modulus by number of vertices is used.
     */
    double turning_angle_at_vertex (long_unsigned_int i) const ;

#ifdef GSL_FOUND
    void apply_matrix (Matrix3x3 &matrix) ;

    void apply_homography (Matrix3x3 &matrix) ;
#endif
    /** @brief Subdivides the polyline by inserting vertices at every distance
     * as specified by the parameter, starting counting at each original vertex.
     *
     * If two successive points exist, no intermediate points are added,
     * however the repeated points are not removed either.
     *
     * If distance is negative or positive smaller than machine epsilon,
     * nothing is done.
     */
    void subdivide (const double distance) ;

    /** @brief Reorder the vertices in growing order of orientation.
     *
     * This is useful as a technique to remove self-crosses in a Polyline.
     * It rearranges the order of vertices in order of angle as counted from
     * the vertex that is the average of the vertices.
     *
     * First an average of the vertices is calculated, hoping that this vertex
     * lies inside the polyline (this is always the case if polyline is convex).
     * Then the angle made between the central point, the x axis and each vertex
     * is computed and used to classify points in growing order of angle. In
     * case of equality of angle, the point closest to reference point is added
     * first.
     *
     * While this effectively removes the crossings, the shape of the polyline
     * is changed, and the shape depends on the selection of the reference
     * point. The refercen point is the average of vertices, because this is
     * what can be computed easily at this point. One would think the mass
     * centre could be used but this is not possible here as the algorithm
     * in geometrical_centre() requires no crossings, and actually
     * reorder_vertices_by_angle() is called inside geometrical_centre().
     *
     * This function is called by algebraic_area(), area(),
     * geometrical_centre(), only in the case the polyline is
     * self-crossing. This is because the algorithm I know only work for
     * non-self-crossing polylines and return wrong results in case it is
     * self-crossing (like null area), so it's either a complete failure or
     * something slightly less wrong. Note that the user can always check
     * with is_crossing() whether the polyline is self-crossing or not before
     * calling functions which depend on the polyline not being self-crossing
     * (which will call reorder_vertices_by_angle).
     */
    void reorder_vertices_by_angle() ;

    /** @brief returns the index of the poyline vertex closest to a given
     * vertex.
     *
     * After the vertex index is returned, the user can use this value
     * in get_vertex and set_vertex, or by accessing vertices directly.
     *
     * If polyline is empty, long_unsigned_int::max() is returned. This
     * is not a valid point to access vertices, however get_vertex and
     * set_vertex() will just ignore values.
     */
    Vertex closest_to (const Vertex &v) const ;

    long_unsigned_int closest_to_index (const Vertex &v) const ;
    /** @brief The signed smallest x of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    long_unsigned_int minimum_x_index () const ;

    /** @brief The signed largest x of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    long_unsigned_int maximum_x_index () const ;

    /** @brief The signed smallest y of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    long_unsigned_int minimum_y_index () const ;

    /** @brief The signed largest y of the vertices.
     *
     * This function runs in O (vertices. size ()) */
    long_unsigned_int maximum_y_index () const ;

    void insert (Polyline p, const long_unsigned_int pos, const bool reverse = false) ;

    /** @brief cuts the argument into the current Polyline, where the two
     * positions are manually indicated.
     *
     * The cut_into function series can be used to create ring-shaped objects
     * from two Polylines. It inserts the argument Polyline and takes care of
     * duplicating beginning and ending points so that there is no visible
     * space in the insertion.
     *
     * * with two position argument
     * * with zero position argument
     *
     * With two position arguments, it will cut at a specific place inside *this
     * (insert before the position indicated, 0 inserting at the beginning, 1
     * inserting before the second vertex, etc.) and insert the argument
     * polyline also at a certain place.
     *
     * Without argument, it will calculate the distances and select the
     * combination of arguments that has the lowest distance.
     *
     * When reverse_auto is set to true (which is the default), the function
     * chooses if the argument Polyline needs to have its order reverted before
     * insertion. The invertion of order assures that the rotation direction
     * of the inserted polyline is opposite (compared to *this) at the point of
     * insertion. If this is not checked, the "inside" an "outside" of the ring
     * might not be defined correctly. If reverse_auto is set to false, then
     * no reversal is done. if the user wishes to reverse the argument polyline,
     * the function reverse() can be called previously.
     *
     * It does not much sense to use cut_into to insert polylines of zero, one
     * or two vertices, or to insert into a *this Polyline which has zero or one
     * vertex. All cases are covered, however the point duplication is only
     * done when the two conditions are met: there are at least 3 points in
     * the inserted vertices and *this has at least two vertices.
     * */
    void cut_into (Polyline p,
                   const long_unsigned_int pos_this,
                   const long_unsigned_int pos_p,
                   const bool reverse_auto = true) ;

    /** @brief cuts the argument into the current Polyline, at the location
     * that is of least distance.
     *
     * See the other overload of the function for the details. Due to the
     * lookup of distances this function works in O(this->size * p->size)
     */
    void cut_into (Polyline p, const bool reverse_auto = true) ;

    /** @brief performs a cycle (french permutation circulaire) or rotation
     * on the elements of the vectors. The first argument indicates the number
     * of times the cycle will be done. The second argument indicates the
     * direction of the cycle (it has to be a separate argument since we use
     * unsigned integers). By default the rotation is so that if is forward,
     * which is the first vertex is pushed to the second, third... place, while
     * the last vertex is cycled to the first place.
     *
     * @Note This calls the C++ STL std::rotate algorithm. The name rotate however
     * is ambiguous since we use the word rotation for the planar rotation
     * of the vector.
     *
     */
    void cycle (long_unsigned_int val, const bool forward) ;

    /** @brief Check if the polyline has a last vertex identical to the first,
     * (which is sometimes used as a visual way to close a polyline),
     * to the specified precision, and if the case, then removes the last
     * vertex and sets the polyline to closed.
     */
    void check_closed(const double precision = std::numeric_limits<double>::epsilon ()) ;

    /** @brief Returns whether a Vertex is contained inside the closed polygon
     * defined by the Polyline.
     *
     * The polyline is always assumed closed, although its closure status is
     * not checked nor altered.
     *
     * Caller must provide limits so that they do not need to be calculated at
     * every call, which is useful in case this function is called many times
     * on the same polyline.
     * */
    // probably need to add an optioanl bool for inclusive/exclusive behaviour.
    bool contains (const Vertex &v, double xmin, double ymin, double xmax, double ymax) const;

    /** @brief Returns whether a Vertex is contained inside the closed polygon
     * defined by the Polyline.
     *
     * The polyline is always assumed closed, although its closure status is
     * not checked nor altered.
     *
     * Polyline limits are calculated inside the function. This version is to be
     * used when only a limited number of calls are made on the same polyline,
     * so the calculation of its limits will not take a lot of time.
     * */
    bool contains (const Vertex &v) const;
} ;
} // namespace dpps
#endif
