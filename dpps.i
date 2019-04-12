/**
 * @file dpps.i
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief SWIG interface file used to create bindings to Lua
 * @date 2014-04-21 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "configure.h"
////////////////////
// SWIG typedefs  //
////////////////////
// changes must be kept in sync with the #define in common.hh
#ifdef VECTOR_SIZE_LONG_LONG
typedef long long unsigned int long_unsigned_int;
#else
typedef long unsigned int long_unsigned_int;
#endif
typedef short signed int selection_t ;

// We lie to SWIG. We say we have const char *, while we have const std::string
// in the real interface.

// http://lua-users.org/lists/lua-l/2008-02/msg00974.html
// http://lua-users.org/lists/lua-l/2008-02/msg00986.html
// std_string allows to use std::string.
// If we do not include it, we still can use strings by declaring them as
// const char * in the .i file and C++ will do implicit conversions.
// It seems that to get const std::string &, we are still obliged to do this,
// we cannot use the strings directly.
//
// If trying to use std::string without including this std_string.i, it will
// fail at runtime with errors of the kind:
// Error in Reader_DXF_R12::set_all_parametres (arg 4), expected 'string const' got 'string'
//////////////////////////////////////////
// SWIG type definition modules for Lua //
//////////////////////////////////////////
%include "std_string.i"
%include "std_vector.i"

%module dpps
%{
#include "Pattern.hh"

#include "Pattern_text.hh"
#include "Pattern_Fibonacci.hh"
#include "Pattern_Penrose_tiling.hh"
// #include "Pattern_bridge.hh"
#include "Pattern_random.hh"
#include "Pattern_grating.hh"
#include "Pattern_rectangular_lattice.hh"
#include "Pattern_hard_sphere_random_packing.hh"
#include "Polyline_factory.hh"
#include "Pattern_factory.hh"
#include "Reader_DXF_R12.hh"
#include "Reader_image.hh"
#include "Reader_witec.hh"

#ifdef QT_FOUND
#include "Reader_image_Qt.hh"
#endif

#ifdef PNGPP_FOUND
#include "Reader_image_png.hh"
#endif

#include "Writer_DXF_R12.hh"
#include "Writer_SVG.hh"
#include "Writer_cpp.hh"
#include "Writer_witec.hh"

#include "util.hh"
#include "Matrix2x2.hh"
#include "Matrix3x3.hh"

using namespace dpps;
using namespace std;
%}

typedef enum {
    quadrant_centre,
    quadrant_pxpy,
    quadrant_py,
    quadrant_mxpy,
    quadrant_mx,
    quadrant_mxmy,
    quadrant_my,
    quadrant_pxmy,
    quadrant_px
} enum_quadrant ;

class Vertex {
public:
    double x;
    double y;
    Vertex ();
    Vertex (double x0, double y0);
    Vertex (pair<double,double> &p);
    bool operator== (Vertex &comparison);
    bool equals_to (Vertex &comparison, double precision);
    bool operator!= (Vertex &comparison);
    Vertex operator+ (Vertex &add);
    Vertex operator+ (double add);
    Vertex operator- (Vertex &subtract);
    Vertex operator- (double subtract);
    Vertex operator* (double multiply);
    double operator* (Vertex &product);
    Vertex operator/ (double divide);
    double norm2 () const ;
    double norm2_square () const ;
    void display () const ;
    std::string display_string () const ;
    enum_quadrant quadrant (const Vertex &w) const;
    void round_to (double precision = std::numeric_limits<double>::epsilon()) ;
    short signed int turn (const Vertex &u, const Vertex &w) const ;
    static bool intersects (const Vertex &u, const Vertex &v,
                            const Vertex &w, const Vertex &z) ;
    double angle (const Vertex &u, const Vertex &w,
       double precision = std::numeric_limits<double>::epsilon()) const ;
    void rotate (const dpps::Vertex &v, const double angle) ;
    bool is_aligned (const Vertex &v, const Vertex &w, double precision) const ;
};

class Polyline {
public:
    vector<Vertex> vertices;
    bool closed;
    double dose;
    unsigned int reference;
    bitset <polyline_max_selection> selected ;
    Polyline () ;
    ~Polyline () ;
    Polyline (const Vertex &first) ;
    Polyline (const Polyline &p) ;
    Polyline &operator= (const Polyline &p) ;
    Polyline (const long_unsigned_int number_vertices, const double diametre) ;
    Polyline (const long_unsigned_int number_vertices,
                    const double major_axis,
                    const double minor_axis,
                    const double angle_x)  ;
    Vertex get_vertex (const long_unsigned_int i) const ;
    void set_vertex (const long_unsigned_int i, const Vertex &v) ;
    void select (const bool status, const selection_t selection) ;
    void select_all (const bool status) ;
    void toggle (const selection_t) ;
    void toggle_all () ;
    bool get_select (const selection_t) ;
    void push_back (const Vertex &v) ;
    void push_back (const Polyline &p) ;
    void push_back_relative (const Vertex &v) ;
    void push_back (const double x, const double y) ;
    void push_back_relative (const double x, const double y) ;
    bool operator== (const Polyline &p) const ;
    Polyline operator+ (const Vertex &v) const ;
    Polyline operator- (const Vertex &v) const ;
    Polyline operator* (const double m) const ;
    Polyline operator/ (const double m) const ;
    bool equals_to (const Polyline &p,
        double precision = numeric_limits<double>::epsilon()) const ;
    void display () const ;
    std::string display_string () const ;
    long_unsigned_int size () const ;
    double orientation_at_vertex (const long_unsigned_int i) const ;
    Vertex geometrical_centre () const ;
    double minimum_x () const ;
    double maximum_x () const ;
    double minimum_y () const ;
    double maximum_y () const ;
    void limits (double &minx, double &miny, double &maxx, double &maxy) const ;
    double horizontal_size () const ;
    double vertical_size () const ;
    double maximum_distance () const ;
    double minimum_distance () const ;
    double area () const ;
    double algebraic_area () const ;
    double algebraic_length () const ;
    double algebraic_perimeter () const ;
    double perimeter() const ;
    double length () const ;
    void transpose () ;
    void translate (const Vertex &v) ;
    void translate (const double x, const double y) ;
    void scale_centred (const double fx, const double fy) ;
    void scale_centred (const double f) ;
    void scale (const Vertex &v, const double fx, const double fy) ;
    void scale (const double x, const double y, const double fx, const double fy) ;
    void scale (const Vertex &v, const double f) ;
    void scale (const double x, const double y, const double f) ;
    void rotate_centred (const double angle) ;
    void rotate (const Vertex &v, const double angle) ;
    void rotate (const double x, const double y, const double angle) ;
    void symmetry_x (const double y0) ;
    void symmetry_y (const double x0) ;
    void symmetry_x_centred () ;
    void symmetry_y_centred () ;
    void remove_consecutive_duplicated_vertices (
        const double precision = numeric_limits<double>::epsilon()) ;
    void remove_aligned_vertices (
        double precision = std::numeric_limits<double>::epsilon()) ;
    void remove_duplicated_vertices (
        const double precision = numeric_limits<double>::epsilon()) ;
    void replace_by (Polyline polyline, const bool keep_initial_properties = true) ;
    void replace_by_regular_polygon (const long_unsigned_int number_vertices,
                                     const double diametre,
                                     const bool keep_initial_properties = true) ;
    void round_to (const double precision) ;
    void reverse_vertices () ;
    void set_dose (const double dose_value) ;
    void add_to_dose (const double additive_constant) ;
    void multiply_dose (const double multiplication_factor) ;
    void set_reference (const long_unsigned_int reference_value) ;
    void add_to_reference (const long_unsigned_int additive_constant) ;
    void subtract_to_reference (const long_unsigned_int additive_constant) ;
    void set_closure (const bool closure_value) ;
    void toggle_open_close () ;
    void erase () ;
    void erase (const long_unsigned_int start, long_unsigned_int end) ;
    void set_metadata_from (const Polyline &p) ;
    double get_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre) const ;
    long_unsigned_int get_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre) const ;
    void update_double_property (const enum_double_property property,
                                 const long_unsigned_int parametre,
                                 const double value) ;
    //void unselect () ;
//     bool multicriteria_less_than (
//         const Polyline &p,
//         std::vector<dpps::enum_double_property> &sort_order) const ;
    bool is_convex () const ;
    bool is_self_crossing () const ;
    double turning_angle_at_vertex (long_unsigned_int i) const ;
    void apply_matrix (Matrix3x3 matrix) ;
    void apply_homography (Matrix3x3 matrix) ;
    void subdivide (const double distance) ;
    void reorder_vertices_by_angle() ;
    Vertex closest_to (const Vertex &v) const ;
    long_unsigned_int closest_to_index (const Vertex &v) const ;
    long_unsigned_int minimum_x_index () const ;
    long_unsigned_int maximum_x_index () const ;
    long_unsigned_int minimum_y_index () const ;
    long_unsigned_int maximum_y_index () const ;
    void insert (Polyline p, const long_unsigned_int pos, const bool reverse = false) ;
    void cycle (long_unsigned_int val, const bool forward) ;
    void cut_into (Polyline p,
                   const long_unsigned_int pos_this,
                   const long_unsigned_int pos_p,
                   const bool reverse_auto = true) ;
    void cut_into (Polyline p, const bool reverse_auto = true) ;
    void check_closed(const double precision) ;
} ;

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
    integer_property_position,
    integer_property_number_vertices,
    integer_property_number_vertices_modulo,
    integer_property_closeness,
    integer_property_reference
} enum_integer_property ;

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

// https://en.wikipedia.org/wiki/Logical_connective
// https://en.wikipedia.org/wiki/Truth_function#Table_of_binary_truth_functions
typedef enum {
    logical_operator_false,
    logical_operator_true,
    logical_operator_p,
    logical_operator_not_p,
    logical_operator_q,
    logical_operator_not_q,
    logical_operator_and,
    logical_operator_nand,
    logical_operator_or,
    logical_operator_nor,
    logical_operator_material_nonimplication,
    logical_operator_material_implication,
    logical_operator_converse_nonimplication,
    logical_operator_converse_implication,
    logical_operator_xor,
    logical_operator_xnor
} enum_logical_operator ;

typedef enum {
    start_tile_thin,
    start_tile_large,
    start_tile_two_thin,
    start_tile_two_large
} enum_start_tile ;

typedef enum {
    progression_model_fixed,
    progression_model_linear,
    progression_model_quadratic,
    progression_model_exponential
} enum_progression_model ;

class Pattern {
//friend class Pattern_factory ;
//friend class Writer ;
//protected:
    //deque<Polyline> polylines ;
    //default_random_engine pseudorandom_generator ;
    //long_unsigned_int time_seed ;
public:
    Pattern () ;
    Pattern (const Pattern &source) ;
    Pattern &operator= (const Pattern &source) ;
//     virtual ~Pattern () ;
//     virtual void set_parametres (const vector<bool> &vbool,
//                                  const vector<long_unsigned_int> &vint,
//                                  const vector<double> &vdouble,
//                                  const vector<string> &vstring) ;
//     virtual void generate () ;
    //long_unsigned_int size_all () const ;
    void select (const enum_logical_action action,
                 const selection_t selection) ;
    void select_window (const double xmin, const double ymin,
                        const double xmax, const double ymax,
                        const bool dots_as_cricles = true,
                        const selection_t selection = 0) ;
    void select_window (const Vertex &min, const Vertex &max,
                        const bool dots_as_circles = true,
                        const selection_t selection = 0) ;
    void select_all (const bool status) ;
    void toggle (const selection_t) ;
    void toggle_all () ;
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
    void selection_logical_operation (
                  const selection_t destination_selection,
                  const selection_t selection_p,
                  const selection_t selection_q,
                  const enum_logical_operator logical_operator) ;
    void selection_swap (const selection_t selection_p,
                         const selection_t selection_q) ;
    void selection_next (const selection_t origin_selection,
                      const selection_t destination_selection,
                      const bool unselect_origin) ;
    void select_front (const selection_t selection = -1) ;
    void select_back (const selection_t selection = -1) ;
    void symmetry_x (const double y0, const selection_t selection = -1) ;
    void symmetry_y (const double x0, const selection_t selection = -1) ;
    void symmetry_x_centred (const selection_t selection = -1) ;
    void symmetry_y_centred (const selection_t selection = -1) ;
    double area (const selection_t selection = -1) const ;
    double algebraic_area (const selection_t selection = -1) const ;
    double perimeter (const selection_t selection = -1) const ;
    double length (const selection_t selection = -1) const ;
    double algebraic_length (const selection_t selection = -1) const ;
    double algebraic_perimeter (const selection_t selection = -1) const ;
    void display (const selection_t selection = -1) const ;
    std::string display_string (const selection_t selection = -1) const ;
    //void display_all () const ;
    //std::string display_string_all () const ;
    long_unsigned_int vertices_size (const selection_t selection = -1) const ;
    double minimum_x (const selection_t selection = -1) const ;
    double maximum_x (const selection_t selection = -1) const ;
    double minimum_y (const selection_t selection = -1) const ;
    double maximum_y (const selection_t selection = -1) const ;
    double horizontal_size (const selection_t selection = -1) const ;
    double vertical_size (const selection_t selection = -1) const ;
    Vertex geometrical_centre_polylines (const selection_t selection = -1) const ;
    Vertex geometrical_centre_vertices (const selection_t selection = -1) const ;
    long_unsigned_int size (const selection_t selection = -1) const ;

    // due to unaibility of Lua to distinguish between integers and doubles,
    // certains versions have to be ignored in the bindings.
    void scale_centred (const double fx, const double fy, const selection_t selection = -1) ;
    //void scale_centred (const double f, const selection_t selection = -1) ;
    void scale (const Vertex &v, const double fx, const double fy, const selection_t selection = -1) ;
    void scale (const double x, const double y, const double fx, const double fy, const selection_t selection = -1) ;
    //void scale (const Vertex &v, const double f, const selection_t selection = -1) ;
    //void scale (const double x, const double y, const double f, const selection_t selection = -1) ;

    void rotate_centred (const double angle, const selection_t selection = -1) ;
    void rotate (const Vertex &v, const double angle, const selection_t selection = -1) ;
    void rotate (const double x, const double y, const double angle, const selection_t selection = -1) ;
    void transpose (const selection_t selection = -1) ;
    void translate (const Vertex &v, const selection_t selection = -1) ;
    void translate (const double x, const double y, const selection_t selection = -1) ;

    void remove_aligned_vertices (
        double precision = numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void remove_consecutive_duplicated_vertices (
        double precision = numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void remove_duplicated_vertices (
        double precision = numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void remove_consecutive_duplicated_polylines (
        double precision = numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void remove_duplicated_polylines (
        double precision = numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void explode_in_vertices (const bool keep_initial, const selection_t selection = -1) ;
    void replace_by (const Polyline &polyline,
                     const bool translate = true,
                     const bool keep_initial_properties = true,
                     const selection_t selection = -1) ;
    void replace_polyline_by_pattern (const Pattern &p,
                                      const bool translate = true,
                                      const bool keep_initial_properties = true,
                                      const selection_t selection = -1) ;
    void replace_by_regular_polygon (const long_unsigned_int number_vertices,
                                     const double diametre,
                                     const bool keep_initial_properties = true,
                                     const selection_t selection = -1) ;
    void fill_with_lines (const double angle, const double spacing,
                          const bool keep_initial,
                          const selection_t selection = -1) ;
    void fill_with_lines_first_angle (const double spacing,
                                      const bool keep_initial,
                                      const selection_t selection = -1) ;
    void fill_with_dashes (const double angle,
                    const double spacing_longitudinal,
                    const double spacing_transversal,
                    const double duty_cycle,
                    const double phase_initial,
                    const double phase_increment,
                    const enum_partial_dash_policy partial_policy,
                    const bool keep_initial,
                    const selection_t selection = -1) ;
    void fill_with_dashes_first_angle (
                const double spacing_longitudinal,
                const double spacing_transversal,
                const double duty_cycle,
                const double phase_initial,
                const double phase_increment,
                const enum_partial_dash_policy partial_policy,
                const bool keep_initial,
                const selection_t selection = -1) ;
    void duplicate (const selection_t selection = -1) ;
    void reverse_vertices (const selection_t selection = -1) ;
    void round_to (const double precision) ;
    void set_dose (const double dose_value, const selection_t selection = -1) ;
    void set_dose_increasing (const double dose_initial,
                              const double dose_final,
                              const selection_t selection = -1) ;
    void add_to_dose (const double additive_constant, const selection_t selection = -1) ;
    void multiply_dose (const double multiplication_factor, const selection_t selection = -1) ;
    void set_reference (const long_unsigned_int reference_value, const selection_t selection = -1) ;
    void set_reference_increasing (const selection_t selection = -1) ;
    void add_to_reference (const long_unsigned_int additive_constant, const selection_t selection = -1) ;
    void subtract_to_reference (const long_unsigned_int additive_constant, const selection_t selection = -1) ;
    void erase (const selection_t selection = -1) ;
    void set_closure (const bool closure_value, const selection_t selection = -1) ;
    void toggle_open_close (const selection_t selection = -1) ;
    void push_back (const Polyline &p) ;
    //void push_front (const Polyline &p) ;
    Polyline pop_back () ;
    //void pop_front () ;
    void append_from (const Pattern &p, const selection_t selection = -1) ;
    //vector<Polyline> vector_selected (selection_t selection) ;
    Polyline get_polyline (long_unsigned_int number) ;
    void random_update_property (const enum_random_property property,
                  const enum_distribution random_distribution,
                  const double p1,
                  const double p2,
                  const selection_t selection = -1) ;
//     void sort (const std::vector <selection_t> properties,
//                const bool increasing,
//                const selection_t selection = -1) ;

    void shuffle (const selection_t selection = -1) ;

    void reverse_order (const selection_t selection = -1) ;

    void sort1 (const enum_double_property property,
                const bool increasing,
                const selection_t selection = -1) ;
    void sort2 (const enum_double_property property1,
                const enum_double_property property2,
                const bool increasing,
                const selection_t selection = -1) ;
    void sort3 (const enum_double_property property1,
                const enum_double_property property2,
                const enum_double_property property3,
                const bool increasing,
                const selection_t selection = -1) ;
    void set_metadata_from (const Polyline &p, const selection_t selection = -1) ;
    void distribute (
        const enum_progression_model model,
        const enum_double_property property,
        const long_unsigned_int parametre,
        const selection_t selection = -1) ;
    double get_minimum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;

    double get_maximum_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;
    void get_limits_double_property (
                    const enum_double_property property,
                    const long_unsigned_int parametre,
                    double &minimum,
                    double &maximum,
                    const selection_t selection = -1) const ;
    long_unsigned_int get_minimum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;
    long_unsigned_int get_maximum_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    const selection_t selection = -1) const ;
    void get_limits_integer_property (
                    const enum_integer_property property,
                    const long_unsigned_int parametre,
                    long_unsigned_int &minimum,
                    long_unsigned_int &maximum,
                    const selection_t selection = -1) const ;
    void update_double_property (
            const enum_double_property property,
            const long_unsigned_int parametre,
            const double value,
            const selection_t selection = -1) ;
    void limits (double &minx, double &miny, double &maxx, double &maxy, const bool dots_as_circles = false) const ;
    //void unselect (const selection_t selection = -1) ;
    void overwrite_selected (const Pattern &p, const selection_t selection = -1) ;
    bool is_convex (const selection_t selection = -1) const ;
    void join_successive_intersecting_lines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void join_intersecting_lines (
        const double precision = std::numeric_limits<double>::epsilon(),
        const selection_t selection = -1) ;
    void subdivide (const double distance, const selection_t selection = -1) ;
    void reorder_vertices_by_angle(const selection_t selection = -1) ;
    void apply_matrix (Matrix3x3 &matrix, const selection_t selection = -1) ;
    void apply_homography (Matrix3x3 &matrix, const selection_t selection = -1) ;
    void cycle (long_unsigned_int val, const bool forward,
                const selection_t selection = -1) ;
    void cut_into (Polyline p,
                   const long_unsigned_int pos_this,
                   const long_unsigned_int pos_p,
                   const bool reverse_auto = true) ;
    void cut_into (Polyline p, const bool reverse_auto = true) ;
    void check_closed(const double precision = std::numeric_limits<double>::epsilon(), const selection_t selection = -1) ;
} ;


class Pattern_Fibonacci: public Pattern {
public:
    Pattern_Fibonacci () ;
    Pattern_Fibonacci (const Pattern_Fibonacci &source) ;
    Pattern_Fibonacci (
        const bool bidimentionnal,
        const long_unsigned_int generation,
        const double x0,
        const double y0,
        const double size) ;
    Pattern_Fibonacci &operator= (const Pattern_Fibonacci &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const bool bidimensionnal,
        const long_unsigned_int generation,
        const double x0,
        const double y0,
        const double size) ;
} ;

class Pattern_grating: public Pattern {
public:
    Pattern_grating () ;
    Pattern_grating (const Pattern_grating &source) ;
    Pattern_grating::Pattern_grating (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period,
        const double area_ratio) ;
    Pattern_grating &operator= (const Pattern_grating &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period,
        const double area_ratio) ;
} ;

class Pattern_hard_sphere_random_packing: public Pattern {
public:
    Pattern_hard_sphere_random_packing () ;
    Pattern_hard_sphere_random_packing (const Pattern_hard_sphere_random_packing &source) ;
    Pattern_hard_sphere_random_packing (
        const long_unsigned_int number_total,
        const long_unsigned_int number_random,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre) ;
    Pattern_hard_sphere_random_packing &operator= (const Pattern_hard_sphere_random_packing &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const long_unsigned_int number_total,
        const long_unsigned_int number_random,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre) ;
} ;

class Pattern_Penrose_tiling: public Pattern {
public:
    Pattern_Penrose_tiling () ;
    Pattern_Penrose_tiling (const Pattern_Penrose_tiling &source) ;
    Pattern_Penrose_tiling (
        const long_unsigned_int generation,
        const enum_start_tile start_tile,
        const double x0,
        const double y0,
        const double size) ;
    Pattern_Penrose_tiling &operator= (const Pattern_Penrose_tiling &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const long_unsigned_int generation,
        const enum_start_tile start_tile,
        const double x0,
        const double y0,
        const double size) ;
} ;

class Pattern_random: public Pattern {
public:
    Pattern_random () ;
    Pattern_random (const Pattern_random &source) ;
    Pattern_random (
        const enum_distribution type,
        const long_unsigned_int number,
        const long_unsigned_int max_attempts,
        const bool avoid_overlap,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre,
        const double p1,
        const double p2) ;
    Pattern_random &operator= (const Pattern_random &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const enum_distribution type,
        const long_unsigned_int number,
        const long_unsigned_int max_attempts,
        const bool avoid_overlap,
        const double x0,
        const double y0,
        const double lx,
        const double ly,
        const double diametre,
        const double p1,
        const double p2) ;
} ;

class Pattern_rectangular_lattice: public Pattern {
public:
    Pattern_rectangular_lattice () ;
    Pattern_rectangular_lattice (const Pattern_rectangular_lattice &source) ;
    Pattern_rectangular_lattice (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period_x,
        const double period_y,
        const double area_ratio = 0.0) ;
    Pattern_rectangular_lattice &operator= (const Pattern_rectangular_lattice &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void generate () ;
    void set_all_parametres (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const double period_x,
        const double period_y,
        const double area_ratio) ;
} ;

typedef enum {
    text_align_y_centred_baseline,
    text_align_y_centred_height,
    text_align_y_centred_bottom,
    text_align_y_centred_top
} text_align_y ;

typedef enum {
    text_align_x_centred,
    text_align_x_left,
    text_align_x_right
} text_align_x ;

typedef enum {
    several,
    cut,
    continuous
} stroke_type ;

class Pattern_text: public Pattern {
public:
    Pattern_text () ;
    Pattern_text (const Pattern_text &source) ;
    Pattern_text (
        const bool utf8,
        const stroke_type stroke,
        const short unsigned int variant,
        const text_align_x align_x,
        const text_align_y align_y,
        const double x0,
        const double y0,
        const double ex,
        const char *font_name,
        const char *text) ;
    Pattern_text &operator= (const Pattern_text &source) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    void set_text (const std::string &text) ;
    void set_font (const std::string &font_name) ;
    void set_x (const double x) ;
    void set_y (const double y) ;
    void set_ex (const double ex) ;
    void generate () ;
    void set_all_parametres (
        const bool utf8,
        const stroke_type stroke,
        const short unsigned int variant,
        const text_align_x align_x,
        const text_align_y align_y,
        const double x0,
        const double y0,
        const double ex,
        const char *font_name,
        const char *text) ;
} ;

// typedef enum {
//     bridge_layer_1,
//     bridge_layer_2,
// } enum_bridge_layer ;
//
// class Pattern_bridge: public Pattern {
// public:
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
//     void generate () ;
//     void set_all_parametres (
//         const bool geometrical_progression,
//         const enum_bridge_layer layer,
//         const long_unsigned_int n_d,
//         const long_unsigned_int n_A,
//         const long_unsigned_int n_W,
//         const double base_width_d_min,
//         const double base_width_d_max,
//         const double bridge_width_A_min,
//         const double bridge_width_A_max,
//         const double bridge_length_W_min,
//         const double bridge_length_W_max,
//         const double spacing_factor,
//         const double spacing_offset,
//         const double L2_factor) ;
// } ;

class Polyline_factory {
public:
    //static Polyline cut_into (Polyline outer, const Polyline &inner) ;
    static Polyline frame (const double outer_size, const double thickness) ;
    static Polyline frame (const double outer_size_x, const double outer_size_y, const double thickness) ;
    static Polyline cross (const double outer_size, const double thickness) ;
    static Polyline crossing_star_polygon (
        const long_unsigned_int p,
        const long_unsigned_int q,
        const double diametre) ;
    static Polyline noncrossing_star_polygon (
        const bool regular,
        const long_unsigned_int p,
        const long_unsigned_int q,
        const double diametre,
        double ratio) ;
    static Polyline polygonal_ring (const long_unsigned_int outer_vertices,
                           const long_unsigned_int inner_vertices,
                           const double outer_diameter,
                           const double inner_diameter) ;

    static Polyline polygonal_elliptic_ring (
        const long_unsigned_int outer_vertices,
        const long_unsigned_int inner_vertices,
        const double outer_major_axis,
        const double outer_minor_axis,
        const double outer_angle_x,
        const double inner_major_axis,
        const double inner_minor_axis,
        const double inner_angle_x) ;

} ;

class Pattern_factory {
public:
    static Pattern explode (const Polyline &polyline) ;
    static Pattern fill_with_lines (
                            const Polyline &polyline,
                            const double angle,
                            const double spacing) ;
    static Pattern fill_with_lines_first_angle (
                            const Polyline &polyline,
                            const double spacing) ;
    static Pattern from_initialization_list (shape list) ;
    static Pattern fill_with_dashes (
                         const Polyline &polyline,
                         const double angle,
                         const double period_longitudinal,
                         const double spacing_transversal,
                         const double duty_cycle,
                         const double phase_initial,
                         const double phase_increment,
                         const enum_partial_dash_policy partial_dash_policy) ;
    static Pattern fill_with_dashes_first_angle (
                           const Polyline &polyline,
                           const double period_longitudinal,
                           const double spacing_transversal,
                           const double duty_cycle,
                           const double phase_initial,
                           const double phase_increment,
                           const enum_partial_dash_policy partial_dash_policy) ;
    static Pattern Pattern_from_selected (Pattern &p,
                                                selection_t selection = -1) ;
    static Pattern compare (const Pattern &pattern_old,
                const Pattern &pattern_new,
                const selection_t selection_old = -1,
                const selection_t selection_new = -1,
                const selection_t selection_only_old = 0,
                const selection_t selection_only_new = 1,
                const double precision = std::numeric_limits<double>::epsilon(),
                const bool ignore_references = false,
                const bool ignore_dose = false) ;
} ;

class Reader {
public:
    void append_to_pattern (Pattern &p) ;
    //string metadata () const ;
    virtual void analyze_metadata () ;
//     virtual void set_parametres (const vector<bool> &vbool,
//                                  const vector<long_unsigned_int> &vint,
//                                  const vector<double> &vdouble,
//                                  const vector<string> &vstring) ;
    virtual bool read_polyline (Polyline &p) = 0 ;
} ;

// typedef enum {
//     open_polyline_import,
//     open_polyline_ignore,
//     open_polyline_import_as_closed
// } enum_open_polyline_action ;

typedef enum {
    all,
    verbatim,
    comma_separated_verbatim_list,
    regex_ECMAScript,
    regex_basic,
    regex_extended,
    regex_awk,
    regex_grep,
    regex_egrep
} enum_regex ;

class Reader_DXF_R12: public Reader {
public:
    //Reader_DXF_R12 (string set_filename) ;
    Reader_DXF_R12 (const char *set_filename) ;
    Reader_DXF_R12 (
        const char *set_filename,
        const bool set_reference_from_layer,
        const bool include_blocks,
//         const enum_open_polyline_action open_polyline_action,
        const enum_regex regex_grammar,
        const char *layers_to_read) ;

//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    bool read_polyline (Polyline &p) ;
    void set_all_parametres (
        const bool set_reference_from_layer,
        const bool include_blocks,
//         const enum_open_polyline_action open_polyline_action,
        const enum_regex regex_grammar,
        const char *layers_to_read) ;
     std::vector<std::string> get_layers_read () const ;
     void close() ;
};

class Reader_witec: public Reader {
public:
    Reader_witec (const char *set_filename) ;
    Reader_witec (const char *set_filename,
        const bool construction_lines) ;
    bool read_polyline (Polyline &p) ;
    void set_all_parametres (
        const bool construction_lines) ;
     void close() ;
} ;

#if defined (QT_FOUND) || defined (PNGPP_FOUND)
    typedef enum {
        image_reader_import_x_lines,
        image_reader_import_y_lines,
        image_reader_import_dots,
        image_reader_import_x_rectangles,
        image_reader_import_y_rectangles,
        image_reader_import_squares
    } enum_image_reader_mode ;
class Reader_image: public Reader {
public:
    bool read_polyline (Polyline &p) ;
    // pure virtual method lets swig know this class is abstract so no
    // wrappers will be generated, but this declaration can be used for
    // classes that inherit from this one.
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
     void close() ;
} ;
#endif

#ifdef QT_FOUND
class Reader_image_Qt: public Reader_image {
public:
    Reader_image_Qt (
        const char *set_filename,
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    // image_pixel has to appear so that swig knows class is not abstract.
    std::array<short unsigned int, 3> image_pixel (
        const long_unsigned_int column,
        const long_unsigned_int row) const ;
    bool read_polyline (Polyline &p) ;
    Reader_image_Qt (const char *filename) ;
    void append_to_pattern (Pattern &p) ;
    void set_all_parametres (
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;
     void close() ;
} ;
#endif

#ifdef PNGPP_FOUND
class Reader_image_png: public Reader_image {
public:
    Reader_image_png (
        const char *set_filename,
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;
//     void set_parametres (const vector<bool> &vbool,
//                          const vector<long_unsigned_int> &vint,
//                          const vector<double> &vdouble,
//                          const vector<string> &vstring) ;
    // image_pixel has to appear so that swig knows class is not abstract.
    std::array<short unsigned int, 3> image_pixel (
        const long_unsigned_int column,
        const long_unsigned_int row) const ;
    bool read_polyline (Polyline &p) ;
    Reader_image_png (const char *filename) ;
    void append_to_pattern (Pattern &p) ;
    void set_all_parametres (
        const bool only_this_colour,
        const enum_image_reader_mode mode,
        const long_unsigned_int red,
        const long_unsigned_int green,
        const long_unsigned_int blue,
        const double size) ;
     void close() ;
} ;
#endif

class Writer {
public:
    Writer (const char *set_filename, const bool append) ;
    virtual void write_header () ;
    virtual void write_footer () ;
    virtual ~Writer () ;
    void close () ;
//     virtual void set_parametres (const vector<bool> &vbool,
//                                  const vector<long_unsigned_int> &vint,
//                                  const vector<double> &vdouble,
//                                  const vector<string> &vstring) ;
    virtual void write_Polyline (const Polyline &polyline) = 0 ;
    virtual void write_Pattern (Pattern &pattern) ;
    virtual void write_Pattern_selected (const Pattern &pattern,
        const selection_t selection = -1) ;
} ;

typedef enum {
    dot_as_dot,
    dot_as_polyline,
    dot_as_circle
} enum_dot_dxf_export ;

class Writer_DXF_R12: public Writer {
public:
    static const long_unsigned_int min_handle {581} ;
    Writer_DXF_R12 (const char *set_filename) ;
    Writer_DXF_R12 (const char *set_filename, const bool append) ;
    Writer_DXF_R12 (
        const char *set_filename,
        const bool append,
        const bool two_point_line_as_polyline,
        const enum_dot_dxf_export dot_dxf_export,
        const double julian_date,
        const char *layer_name_parametre,
        const char *layer_colour_parametre) ;
    void write_Polyline (const Polyline &polyline) ;
    void set_all_parametres (
        const bool two_point_line_as_polyline,
        const enum_dot_dxf_export dot_dxf_export,
        const double julian_date,
        const char *layer_name_parametre,
        const char *layer_colour_parametre
                            ) ;
    void set_julian_date (const double julian_date) ;
    void set_layer_names (char *layer_name_parametre) ;
    void set_dot_dxf_export (const enum_dot_dxf_export dot_dxf_export) ;
    void set_two_point_line_as_polyline (const bool two_point_line_as_polyline) ;
    void set_layer_colours (char *layer_colour_parametre) ;
} ;

class Writer_SVG: public Writer {
public:
    Writer_SVG (const char *set_filename, const bool append) ;
    Writer_SVG (
        const char *set_filename,
        const bool append,
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const char *style) ;
    void write_Polyline (const Polyline &polyline) ;
    void set_all_parametres (
        const double minx,
        const double miny,
        const double maxx,
        const double maxy,
        const char *style) ;
} ;

class Writer_cpp: public Writer {
public:
    Writer_cpp (const char *set_filename, const bool append) ;
    void write_Polyline (const Polyline &polyline) ;
} ;

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

class Writer_witec: public Writer {
public:
    Writer_witec (const char *set_filename, const bool append) ;
    void write_header () ;
    void write_footer () ;
//     void set_parametres (const vector<bool> &vbool,
//                         const vector<long_unsigned_int> &vint,
//                         const vector<double> &vdouble,
//                         const vector<string> &vstring) ;
    void write_Polyline (const Polyline &polyline) ;
    void write_Pattern_selected (const Pattern &pattern,
       const selection_t selection = -1) ;

    void set_all_parametres (
        const bool finish_at_zero,
        const bool focus_correction,
        const bool litho_by_shutter,
        const bool litho_by_focus,
        const bool xyz_by_piezo,
        const bool focus_by_piezo,
        const bool automatic_field_crossing,
        const bool correct_focus_inside_polylines,
//         const enum_alignment alignment,
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
    void set_additional_parametres(
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
} ;

std::string vector_to_string (const std::vector<std::string> list, const char separator=',') ;

class Matrix2x2 {
public:
    Matrix2x2 () ;
    Matrix2x2 (const double a,
            const double b,
            const double c,
            const double d) ;
    void display () const ;
    std::string display_string () const ;
    double get_coefficient (const short unsigned int i) const ;
    double get_coefficient (const short unsigned int i, const short unsigned j) const ;
    void set_coefficient (const short unsigned int i, const double value) ;
    void set_coefficient (const short unsigned int i, const short unsigned j, const double value) ;
    bool is_null (double precision = std::numeric_limits<double>::epsilon()) const ;
    bool operator== (const Matrix2x2 &comparison) const ;
    bool equals_to (const Matrix2x2 &comparison,
       double precision = std::numeric_limits<double>::epsilon()) const ;
    bool operator!= (const Matrix2x2 &comparison) const ;
    Matrix2x2 operator+ (const Matrix2x2 &add) const ;
    Matrix2x2 operator+ (const double add) const ;
    Matrix2x2 operator- (const Matrix2x2 &subtract) const ;
    Matrix2x2 operator- (const double subtract) const ;
    Matrix2x2 operator* (const double multiply) const ;
    Matrix2x2 operator* (const Matrix2x2 &product) const ;
    Matrix2x2 operator/ (const double divide) const ;
    double determinant () const ;
    Matrix2x2 inverse() const ;
    void invert() ;
    static Matrix2x2 rotation (const double angle) ;
    static Matrix2x2 diagonal (const double a, const double b) ;
    static Matrix2x2 identity () ;
} ;

class Matrix3x3 {
public:
    Matrix3x3 () ;
    Matrix3x3 (const double a, const double b,
            const double c, const double d,
            const double e, const double f,
            const double g, const double h, const double i) ;
    void display () const ;
    std::string display_string () const ;
    double get_coefficient (const short unsigned int i) const ;
    double get_coefficient (const short unsigned int i, const short unsigned int j) const ;
    void set_coefficient (const short unsigned int i, const double value) ;
    void set_coefficient (const short unsigned int i, const short unsigned int j, const double value) ;
    bool is_null (double precision = std::numeric_limits<double>::epsilon()) const ;
    bool operator== (const Matrix3x3 &comparison) const ;
    bool equals_to (const Matrix3x3 &comparison,
       double precision = std::numeric_limits<double>::epsilon()) const ;
    bool operator!= (const Matrix3x3 &comparison) const ;
    Matrix3x3 operator+ (const Matrix3x3 &add) const ;
    Matrix3x3 operator+ (const double add) const ;
    Matrix3x3 operator- (const Matrix3x3 &subtract) const ;
    Matrix3x3 operator- (const double subtract) const ;
    Matrix3x3 operator* (const double multiply) const ;
    Matrix3x3 operator* (const Matrix3x3 &product) const ;
    Matrix3x3 operator/ (const double divide) const ;
    double determinant () const ;
    Matrix3x3 inverse() const ;
    void invert() ;
    static Matrix3x3 rotation (const double angle) ;
    static Matrix3x3 diagonal (const double a, const double b, const double c) ;
    static Matrix3x3 identity () ;
#ifdef GSL_FOUND
static Matrix3x3 homography  (const Vertex &a0, const Vertex &a1, const Vertex &a2, const Vertex &a3,
                              const Vertex &b0, const Vertex &b1, const Vertex &b2, const Vertex &b3,
                              const double precision) ;

#endif
static Matrix3x3 affine (const Vertex &a0, const Vertex &a1, const Vertex &a2,
                         const Vertex &b0, const Vertex &b1, const Vertex &b2,
                         const double precision) ;
static Matrix3x3 similarity  (const Vertex &a0, const Vertex &a1,
                              const Vertex &b0, const Vertex &b1,
                              const double precision) ;
static Matrix3x3 isometry    (const Vertex &a0, const Vertex &a1,
                              const Vertex &b0, const Vertex &b1,
                              const double precision) ;
static Matrix3x3 translation (const Vertex &a0, const Vertex &b0) ;
Vertex apply (const Vertex &v) const ;
Vertex apply_homography (const Vertex &v) const ;
} ;

std::string utf8toiso8859_1 (const char *input) ;
