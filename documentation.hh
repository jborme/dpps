/**

@mainpage DPPS Design production and processing system

@section Introduction

Our lab owns a Witec confocal Raman spectroscopy system. The system is equipped
with a motorised stage and aperture which allows to take measurements at
different places of the sample. As a side-effect of the laser measurement, it
can be used as a general purpose lithography system, drawing patterns on samples
with the laser. The density of energy of the laser is high enough to locally
melt amorphous Silicon, which the recrystallizes as it cools down, gaining
piezoresistive properties, enabling us to make strain sensors.

The software API provides scripting commands to operate the motorized stage, but
not a tool to convert regular CAD designs into this format. Hence the need to
write such a tool.

I also needed a general-purpose framework to generate large designs based on
mathematical formulas, so the final projects got a set of mix of these
capabilities.

@section Compilation Compiling DPPS

DPPS is written in C++11. For common compilers, this gives gcc >= 4.7 or
clang >= 3.3. Code was written to cross-compile to PE32+ format using mingw64.

The whole project should compile without a warning in the highest achievable
warning levels of gcc and clang. The compilation of the code generated by
SWIG might produce warnings depending on SWIG version installed on the host
system (code generated by swig 3.0.2 produces none).

@section Conventions Common conventions

In the following, all the floating point quantities are defined as double
precision. Almost all integer constant are defined as long unsigned int in the
general case (counters, sizes), or short unsigned int when it can be assured
that the number of possibilities will always stay very small. The only case
where signed int are used is in the user file (converted by std::stoi). Static
casting is later performed to transform these numbers to unsigned int.

The code makes use of C++11 standard. It has been tested to compile with g++
branches 4.6, 4.7, 4.8, 4.9 (alpha version at this moment). The project was
also tested to compile with clang (llvm) version 3.3.

@section Class_hierarchy Class hierarchy
@subsection Base Base classes

The two base classes are Vertex and Polyline. The two classes define the basic
elements of drawings. However, the user of the main program does not need to
call these classes directly, and will call some Pattern derivative instead.

@subsubsection Vertex Vertex

Vertex is the most basic class available. A Vertex instance contains a x and a
y coordinate defined as double, which are public attributes.

The class vertex provides equality operator, a function display() which will
serialize by echoing x and y to the standard input in the format (x,y). A method
display_string() will return this serialized content as a std::string. A
method quadrant gives an indication of the relative position of the argument
as compared to the instance, divided in four quadrants (1,3,5,7), four
straight angles (8,2,4,6) plus the identity case (0).

@sa Vertex::quadrant()

@subsubsection Polyline Polyline

Polyline is the basic element of which all drawings of this program are done.

The major property of the Polyline class is vertices, a deque of Vertex
elements. The class also gives indication of whether the polyline is to be
considered closed. A double parametre for dose is also defined. The
signification of this parametre depends on the Writer used. A negative value
is the default, it means there is no dose. For example, the witec writer will
interpret a positive value as the duration in millisecond in the case of a
single dot, and a velocity in micrometers per second in the case of a line.

Many methods are provided, they can be divided into constructing, informative
and transforming.

Constructing functions are
- The default constructor, which returns and empty polyline.
- the constructor using arguments, which returns a regular polygon of a specific
number of vertices and circumscribed diametre.
- The functions Polyline::push_back() and
Polyline::push_back_vector() allow to add vertices, either individually or
b copying a vector of them.

Display functions are Polyline::display() and Polyline::display_string(),
which generalize the display of the class Vertex. The format is
[(x,y),(x,y),(x,y),]. The trailing comma is present.

Informative functions are:

- Polyline::size(), which returns the number of vertices.
- Polyline::orientation_at_vertex(), which returns the absolute angle in
radians made by the segment defined by vertices i and i+1, referred to the
x axis.
- Polyline::geometrical_centre() returns a vertex which is the equally weighted
barycentre (calculated as the average of x and y).
- Polyline::minimum_x(), Polyline::maximum_x(), Polyline::minimum_y(),
Polyline::maximum_y() all return the respective minimum or maximum calculated
on all the vertices. Polyline::limits does the same thing but sets all
the four values in the arguments at once, which is slighty faster than
calling four times the minimum... functions. Also methods
Polyline::horizontal_size(), Polyline::vertical_size() provide the difference
between maxima and minima in x and y, respectively.
- Polyline::maximum_distance() and minimum_distance() return the respective
distances between vertices in the polyline. Finally a polyline can be
characterized by area and perimeter, which are calculated by Polyline::area()
and Polyline::perimeter(). Finally Polyline::length() also returns a value,
vut considering the polyline as open (the difference between perimeter and
length is exactly the length between the last vertex and the first one).

Transforming functions are:
- Polyline::translate(), Polyline::scale(), Polyline::rotate(). These functions
work with a Vertex defining the translation or a centre, can use additional
parametres (angle, scaling factor in x and y). The _centred versions
automatically work with respect to the geometrical centre of the polyline.
- Polyline::remove_duplicated_vertices(), remove successive identical vertices.
- Polyline::replace_by() replaces the vertices of a polyline by the vertices of
another polyline passed as argument. Polyline::replace_by_regular_polygon()
allows to replace a polyline by a regular polygon, calling the constructor with
the arguments.

@subsection Abstract Abstract base classes
Several abstract classes give the general framework in which Polylines are
manipulated. A very limited number of pure virtual functions define the
functions which are defined in the derived classes.

@subsubsection Pattern Pattern class
A Pattern is a set of polylines, and represents from the user point of view the
content to be processed physically. The only pure virtual function is
Pattern::generate(), which contains the specific mathematical rules to create
the patterns.

The abstract class provides a number of functions allowing to manipulate the
Polylines alltogether

@subsubsection Writers Writer functions

@subsubsection Pattern Pattern functions

@section Launching Invocation of the program

@section API Available functions
@subsection Example A simple example

@subsection Readers Reader functions

@subsection Select Selecting patterns

@section About About the program

@subsection WhyNot Why not Python?
Although the developer works on linux, the computer where the application will
run uses a a proprietary OS. Let's see how it goes with
another project as an example:

Gwyddion is a software for analysis of scanning probe microscopy images. It is
developed by a small team of dedicated developers in the Czecz institute of
metrology. As for the Windows port, the website noted for long time that the
python bindings are broken and nobody knows why, as core developers use linux,
cross-compile from a Fedora, and have no expertise in Windows. Later the team
switched to native builds, and right now (gwyddion 2.36) it says the python
binding work on 32 bits but not on 64 bits, and the user must install Python
on its own as well.

I am in a similar situation as I cross-compile from linux and have no taste for
Windows. I am in a worse situation as Gwyddion has a dedicated team and needed
literally years to solve Python binding issues on Windows (and have not yet
finished solving them) while I am alone, very part time, and plan to finish a
1.0 that does what I need for my research and be done with this project. Also,
the pyhton interpreter is known to be difficult to install under windows, and
I do not want to have a small C++ application depend on a package so large and
difficult to install.

Finally, I particularly dislike the options of the Python developers to stop
supporting branch 2 of the language. The best option is to support both versions
in a single implementation, just like it is done for standardized languages.
This would allow to switch language version on a file-to-file basis. I do not
want the scripts I write today to stop working one day because the version
I'm writing bindings for stops being supported in some years, and there are
no more installers for the platform.

@subsection WhyLua Why Lua?

Lua is perfectly suited for this application. It was designed to be embedded as
an extention language, it is small enough so that the source code can be
read an modified or forked as necessary, leaving away all problems of installing
large binary packages on target machines, which has proven to be a difficult
tasks for larger languages like Perl and Python.

Lua is portable to any architecture with a C
compiler. The language features are stable, and scripts should continue working
in the forseeable future.

Recently, changes happened in the C API, in our case this would involve at most
maintenance to the 30 lines of code in interpret_lua.cpp where the Lua API
is called, and in case we are not pleased with future evolutions of the
language, we have no need to evolve, as the lua C implementation was designed
to be embedded into the projects that use them.

The purpose of this project was to have a language to build designs by calling
the API with flow control. This does not require much, and I expect that in
practical cases only a small subset of Lua features are going to be needed.

@subsection WhyGUI Why not using a GUI software?

Those who like to design in a graphical software already have plenty of
solutions, like qcad / librecad. These software usually have some programming
interface.

For the lab I need a software to export designs the specific CNC-type machine
that we have (a Confocal Raman system from Witec). This is so specific that
a separate software was justified. Also a small executable is more convenient
to install on the computer controlling the equipment than a large graphical
package. Since this need is critical to us, having a software we fully control
was also better.

@subsection Module Why an executable and not a Lua module?

The executable includes a minimal preprocessor, which is useful to my target
application.

A module might be useful to some, and can be produced by calling -DLUA_MODULE=1
in the CMake invocation. However I cannot provide guarantees as to its work
status. For once, because on my particular system something goes wrong when
then trying to load the module into Lua (I don't know why). And also, mingw
apparently does not support cross-compiling these modules, so they are not
useful to my application.

Surely, one could produce a module for Windows build using a native
compiler of the target system. I have no competence nor interest in Windows, so
I won't spend time on this, but feel free to do it.





Preprocessor examples:
file exports using the program file. Allows to quickly change a parameter, save
again, and do not overwrite by mistake.


geometrical_centre only works for non-self-crossing polygons
fill wilth lines/dashes only works for convex polygons.

The support for piezo stage is limited.
Once, because the author does not own such system.
Second, because the commands available to the programmer do not allow
interaction with the user, while the use of a piezo stage should involve a
calibration of the movements and of the rotation.

Designs with thousands of features. The major time would be disk I/O to write
large masks. Masks with tens of millions of features would take significantly
more time of computation, but no particular efforts were made to benchmark the
program in such cases. Builds pass arguments to gcc to use the openMP parallel
version of the C++ STL algorithms. Operations on patterns that can be done
independently on each Polyline, operations on Polylines that can be done
independently on each Vertex, and sorting operations, benefit from this
optimisation which comes for free with modern compilers. But it is probably
not noticeable in practical cases due to the relatively low number of polylines
in a practical design (in the tens of thousands), where sequential loops doing
are already fast on modern hardware, memory bandwidth is more a limitation
than CPU performance, and I/O will be the most time-consuming operation.

Functions using arrays (like Vertex::get_matrix_coefficients_*)
are not wrapped for lua, because it's not a simple thing for swig and I did not
want to write wrappers myself.

http://www.swig.org/Doc3.0/Lua.html#Lua_typemap_arrays
http://www.swig.org/Doc3.0/Library.html#Library_carrays

Reader_DXF_R12::get_layers_read, which returns a vector of strings, is
supported through a specific function adapter,
comma_separated_string_from_vector() which returns a single string, which is
supported. comma_separated_string_from_vector(reader:get_layers_read())

Note that it appears to be important that the C++ headers and the interface
file have the same declaration, even if the difference does not seem relevant.
For example in one case I had the namespace in the declaration in one file and
not in the other, which lead to run-time error calling the function:

Error in Vertex_get_matrix (arg 1), expected 'dpps::Vertex *' got 'Vertex *'

Calling static methods.
answer=dpps.Vertex.intersects(x, y, z, t)

*/