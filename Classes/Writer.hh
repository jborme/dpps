/**
 * @file Writer.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Header file for base abstract class Writer
 * @date 2013-11-30 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The \c Writer.hh contains an abstract classes use for output of
 * polylines to a file. Several classes can derive from this base abstract
 * class, for the various file formats supported.
 */
#ifndef DPPS_WRITER
#define DPPS_WRITER

#include "Pattern.hh"

namespace dpps {
/** @brief This is the base abstract class for file processing. It will be
 * implemented by derived classes for svg, dxf, pdf, etc.
 *
 * The purpose of Writer class is to set the basic attributes for all
 * file types: one output stream, and common methods: writing polylines.
*/
class Writer {
protected:
    /** @brief the stream where all data is going to be sent to. */
    std::ofstream file ;

    std::string filename ;

    /** @brief A temporary file.
     *
     * It can be used as a buffer for the output. Often, the file header is
     * known only when the file is finished. In this case, the temporary file
     * is used to write the output, then the final file is used to write the
     * header (once it's known) and receive a copy of temporary file's contents.
     *
     * The file is opened in text-mode for output, an will be closed and
     * removed by Writer::close() and Writer::~Writer().
     */
    std::ofstream tmpfile ;
    /** @brief A randomly chosen filename, provided by C functions. It is set
     * in Writer constructor.
     */
    std::string tmpfilename ;


    /** @brief whether the stream is open.
     *
     * It's open by the constructor so set to true by default. It is only
     * changed by an appel to close() */
    bool open {true} ;

    /** @brief whether the stream was opened in append mode (if true)
     * or in overwrite mode.
     *
     * Whether we have to open the file in append mode. This is used in
     * the Writer constructor and, in derived classes, if further files
     * need to be opened in similar fashion.
     */
    bool append {false} ;
public:
    Writer (const std::string &set_filename, const bool append) ;

    /** @brief A struct to pass encapsulate metadata used by Writer derivatives.
    *
    * Classes derived from Writer can use a struct derived  Writer_settings
    * depending on their needs.
    *
    */
    struct Writer_settings {
    public:
    };

    //Writer (const std::string set_filename) ;

    /** @brief writes the header of the file, before any polyline is written.
     *
     * The full mask is passed as an argument, so that Writer derivates
     * can ask the metadata they need for the header.
     */
    virtual void write_header () ;

    /** @brief writes the footer of the file, before any polyline is written.
     *
     * The full mask is passed as an argument, so that Writer derivates
     * can ask the metadata they need for the header.
     */
    virtual void write_footer () ;

    /** @brief Destructor, closes the stream.
     *
     * It is virtual to make sure we always give a chance to invoke the
     * destructor of derived classes.
     * Siddharta Rao, op. cit. Lesson 11 Polymorphism, p. 292.
     */
    virtual ~Writer () ;

    virtual void close () ;

    /** @brief Function to pass metadata to set-up the Writer.
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

    /** @brief writes a polyline to the stream in the specific file format.
     *
     * This method is the one that actually does filetype-specific operations,
     * and needs to be set as pure virtual. To contrast, all the others virtual
     * were set to empty as some filetypes might not require any processing in
     * their steps, but \c write_polyline really requires something to be
     * implemented for the derived class to be of any use.
     */
    virtual void write_Polyline (const Polyline &polyline) = 0 ;

    virtual void write_Pattern (const Pattern &pattern) ;

    virtual void write_Pattern_selected (const Pattern &pattern,
                                         const selection_t selection = 0) ;
} ;
} // namespace dpps
#endif