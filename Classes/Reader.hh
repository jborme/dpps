/**
 * @file Reader.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Headers of the abstract class for reading files
 * @date 2013-11-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The \c Reader.hh contains an abstract classes use for input from file.
 * Several classes can derive from this base abstract class, for the various
 * file formats supported.
 */
#ifndef DPPS_READER
#define DPPS_READER

#include "Polyline.hh"
#include "Pattern.hh"

namespace dpps {
/** @brief This is the base abstract class for file input. It will be
 * implemented by derived classes for dxf, svg, raster image formats, etc.
 *
 * The purpose of Reader class is to set the basic attributes for all
 * file types: one input stream, and common methods: reading a file and feeding
 * polylines to the main program.
 *
 * Note that the constructor is let to the derived classes. This is because
 * QImage and DXF have very different ways to open.
 */
class Reader {
protected:
    Reader () ;
    /** @brief the stream where all data is going to be read from. */
    std::ifstream file ;
    std::string filename ;
    std::string metadata_string {""} ;
public:
    /** @brief A struct to pass encapsulate metadata used by Reader derivatives.
    *
    * Classes derived from Reader can use a struct derived  Reader_settings
    * depending on their needs.
    *
    */
    struct Reader_settings {
    public:
    };

    void append_to_pattern (Pattern &p) ;

    //Reader (const std::string set_filename) ;

    /** @brief Destructor, closes the stream.
     *
     * It is virtual to make sure we always give a chance to invoke the
     * destructor of derived classes.
     * Siddharta Rao, op. cit. Lesson 11 Polymorphism, p. 292.
     */
    virtual ~Reader () ;

    /** @brief Returns a string with some plain text metadata.
     *
     * The complete way would be a std::multiset but the metadata is not going
     * to be used outside this class so it is not worth doing it. The reason for
     * not using the metadata outside the class is that data would have to be
     * processed in a way that depends on the derivative class. This is not
     * desired. For simplicity the main program should not know anything about
     * the intrinsics of the derivative classes and just call virtual functions.
     * So the way that makes sense here is to just serialize the metadata to a
     * string that will be shown to the user for information.
     */
    std::string metadata () const ;

    virtual void analyze_metadata () ;

    /** @brief Function to pass metadata to set-up the Reader.
     *
     * To allow the main program to pass generically the arguments that
     * the user might pass in the configrue file, without making much efforts,
     * the simplest solution is to pass separately a vector of int, double and
     * strings. They might be empty if the Reader derivative does not require
     * these parametres. The meaning of these parametres is entirely dependent
     * on the derived classes.
     */
    virtual void set_parametres (const std::vector<bool> &vbool,
                                 const std::vector<long_unsigned_int> &vint,
                                 const std::vector<double> &vdouble,
                                 const std::vector<std::string> &vstring) ;

    /** @brief reads the file and returns the next polyline.
     *
     * If the end of file is reached then an exception is thrown.
     *
     * This method is the one that actually does filetype-specific operations,
     * and needs to be set as pure virtual. To contrast, all the others virtual
     * were set to empty as some filetypes might not require any processing in
     * their steps, but \c write_polyline really requires something to be
     * implemented for the derived class to be of any use.
     */
    virtual bool read_polyline (Polyline &p) = 0 ;

    virtual void close () ;
} ;
} // namespace dpps
#endif