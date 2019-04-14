/**
 * @file Writer_DXF_R12.cpp
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Implementation of class Writer_DXF_R12
 * @date 2013-12-08 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This header derives from a file a generated by a licensed version
 * of Autocad 2013 or whatever version we have. It was taken from a simple
 * file containing a single layer called L1. I attempted to generalize some
 * parametres: viewbox, timers, etc. However I'm not really making use of
 * these parametres in this program so default values will be just fine.
 *
 * The parametre max_handle is a count on how many elements the display
 * software has to allocate. This one really has to be known at the moment
 * of calling the header, which means after generation of the Pattern.
 *
 */

#include "Writer_DXF_R12.hh"
#include "util.hh"
#include "bad_parametre.hh"
#include "bad_io.hh"

#include <ctime>
#include <cstdlib>
#include <iomanip>

dpps::Writer_DXF_R12::~Writer_DXF_R12 () {
    if (open)
        close () ;
}

dpps::Writer_DXF_R12::Writer_DXF_R12 (
    const std::string &set_filename): Writer(set_filename, false) {
    handle = min_handle ;
}

dpps::Writer_DXF_R12::Writer_DXF_R12 (
    const std::string &set_filename,
    const bool append): Writer(set_filename, append) {
    handle = min_handle ;
    // layers_set = false ; // already set to false
}

dpps::Writer_DXF_R12::Writer_DXF_R12 (
    const std::string &set_filename,
    const bool append,
    const bool two_point_line_as_polyline,
    const enum_dot_dxf_export dot_dxf_export,
    const double julian_date,
    const std::string &layer_name_parametre,
    const std::string &layer_colour_parametre):
    Writer_DXF_R12 (set_filename, append) {
    set_all_parametres (/*dot_as_polyline,*/ two_point_line_as_polyline, dot_dxf_export,
                        julian_date, layer_name_parametre, layer_colour_parametre) ;
    }

////////////////////////////////////////////////////////////////////////
// dpps::Writer_DXF_R12:: Implementation of inherited virtual methods //
////////////////////////////////////////////////////////////////////////

static double rounding_zero (const double x) {
    if (fabs(x) < 0.001)
        return 0 ;
    return x ;
}

void dpps::Writer_DXF_R12::write_final_header () {
  long_unsigned_int number_layers {layer_names. size ()} ;
  if (number_layers == 0) {
        std::string reason {"Writer_DXF_R12::write_final_header, there are no \
layers defined."} ;
        throw bad_parametre (reason. c_str ()) ;
  }

  double timer {0.0} ;
  // Precision 5 is to the second in French Revolutionary time.
  std::string julian_date_string {fixed_number (writer_settings. julian_date, 5, false)} ;
  handle++ ;

  double screen_ratio {2.5} ;
  Vertex centre {(upper_right + lower_left)/2.0} ;
  Vertex size {upper_right - lower_left} ;

  double height {1.05*(size. y > size. x? size.y : size.x/screen_ratio)} ;

 file
<< "  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n  1\nAC1009\n  9\n$INSBASE\n 10\
\n0.0\n 20\n0.0\n 30\n0.0\n  9\n$EXTMIN\n 10\n"
<< rounding_zero (lower_left. x) << "\n 20\n" << rounding_zero (lower_left. y)
<< "\n 30\n0.0\n  9\n$EXTMAX\n 10\n" << rounding_zero (upper_right.x) << "\n 20\n" << rounding_zero (upper_right. y)
<< "\n 30\n0.0\n  9\n$LIMMIN\n 10\n0.0\n 20\n0.0\n  9\n"
<< "$LIMMAX\n 10\n12.0\n 20\n9.0\n  9\n$ORTHOMODE\n 70\n     0\n  9\n$REGENMODE\
\n 70\n     1\n  9\n$FILLMODE\n 70\n     1\n  9\n$QTEXTMODE\n 70\n     0\n  9\n\
$MIRRTEXT\n 70\n     0\n  9\n$DRAGMODE\n 70\n     2\n  9\n$LTSCALE\n 40\n1.0\
\n  9\n"
<< "$OSMODE\n 70\n   167\n  9\n$ATTMODE\n 70\n     1\n  9\n$TEXTSIZE\n 40\n0.2\
\n  9\n$TRACEWID\n 40\n0.05\n  9\n$TEXTSTYLE\n  7\nSTANDARD\n  9\n"
    // Let's start with the first layer.
    << "$CLAYER\n  8\n" << layer_names. at (0)
    << "\n  9\n$CELTYPE\n  6\nBYLAYER\n  9\n$CECOLOR\n 62\n   256\n  9\
\n$DIMSCALE\n"
<< " 40\n1.0\n  9\n$DIMASZ\n 40\n0.18\n  9\n$DIMEXO\n 40\n0.0625\n  9\n$DIMDLI\
\n 40\n0.38\n  9\n$DIMRND\n 40\n0.0\n  9\n$DIMDLE\n 40\n0.0\n  9\n$DIMEXE\n 40\
\n0.18\n  9\n$DIMTP\n 40\n0.0\n  9\n$DIMTM\n 40\n0.0\n  9\n$DIMTXT\n 40\n0.18\
\n  9\n"
<< "$DIMCEN\n 40\n0.09\n  9\n$DIMTSZ\n 40\n0.0\n  9\n$DIMTOL\n 70\n     0\n  9\
\n$DIMLIM\n 70\n     0\n  9\n$DIMTIH\n 70\n     1\n  9\n$DIMTOH\n 70\n     1\
\n  9\n$DIMSE1\n 70\n     0\n  9\n$DIMSE2\n 70\n     0\n  9\n$DIMTAD\n 70\
\n     0\n  9\n"
<< "$DIMZIN\n 70\n     0\n  9\n$DIMBLK\n  1\n\n  9\n$DIMASO\n 70\n     1\n  9\
\n$DIMSHO\n 70\n     1\n  9\n$DIMPOST\n  1\n\n  9\n$DIMAPOST\n  1\n\n  9\
\n$DIMALT\n 70\n     0\n  9\n$DIMALTD\n 70\n     2\n  9\n$DIMALTF\n 40\
\n25.399999999999999\n  9\n"
<< "$DIMLFAC\n 40\n1.0\n  9\n$DIMTOFL\n 70\n     0\n  9\n$DIMTVP\n 40\n0.0\n  9\
\n$DIMTIX\n 70\n     0\n  9\n$DIMSOXD\n 70\n     0\n  9\n$DIMSAH\n 70\n     0\
\n  9\n$DIMBLK1\n  1\n\n  9\n$DIMBLK2\n  1\n\n  9\n$DIMSTYLE\n  2\nSTANDARD\
\n  9\n"
<< "$DIMCLRD\n 70\n     0\n  9\n$DIMCLRE\n 70\n     0\n  9\n$DIMCLRT\n 70\
\n     0\n  9\n$DIMTFAC\n 40\n1.0\n  9\n$DIMGAP\n 40\n0.09\n  9\n$LUNITS\n 70\
\n     2\n  9\n$LUPREC\n 70\n     4\n  9\n$SKETCHINC\n 40\n0.1\n  9\n$FILLETRAD\
\n 40\n0.0\n"
<< "  9\n$AUNITS\n 70\n     0\n  9\n$AUPREC\n 70\n     0\n  9\n$MENU\n  1\n.\
\n  9\n$ELEVATION\n 40\n0.0\n  9\n$PELEVATION\n 40\n0.0\n  9\n$THICKNESS\n 40\
\n0.0\n  9\n$LIMCHECK\n 70\n     0\n  9\n$BLIPMODE\n 70\n     0\n  9\n$CHAMFERA\
\n 40\n0.0\n"
<< "  9\n$CHAMFERB\n 40\n0.0\n  9\n$SKPOLY\n 70\n     0\n  9\n$TDCREATE\n 40\n"
<< julian_date_string << "\n  9\n$TDUPDATE\n 40\n"
<< julian_date_string << "\n  9\n$TDINDWG\n 40\n" << rounding_zero (timer)
<< "\n  9\n$TDUSRTIMER\n 40\n" << rounding_zero (timer)
<< "\n  9\n$USRTIMER\n 70\n     1\n  9\n$ANGBASE\n 50\n"
<< "0.0\n  9\n$ANGDIR\n 70\n     0\n  9\n$PDMODE\n 70\n     0\n  9\n$PDSIZE\
\n 40\n0.0\n  9\n$PLINEWID\n 40\n0.0\n  9\n$COORDS\n 70\n     2\n  9\n$SPLFRAME\
\n 70\n     0\n  9\n$SPLINETYPE\n 70\n     6\n  9\n$SPLINESEGS\n 70\n     8\
\n  9\n$ATTDIA\n"
<< " 70\n     0\n  9\n$ATTREQ\n 70\n     1\n  9\n$HANDLING\n 70\n     1\n  9\
\n$HANDSEED\n  5\n"
<< std::hex << handle
<< "\n  9\n$SURFTAB1\n 70\n     6\n  9\n$SURFTAB2\n 70\n     6\n  9\
\n$SURFTYPE\n 70\n     6\n  9\n$SURFU\n 70\n     6\n  9\n$SURFV\n 70\n"
<< "     6\n  9\n$UCSNAME\n  2\n\n  9\n$UCSORG\n 10\n0.0\n 20\n0.0\n 30\n0.0\
\n  9\n$UCSXDIR\n 10\n1.0\n 20\n0.0\n 30\n0.0\n  9\n$UCSYDIR\n 10\n0.0\n 20\
\n1.0\n 30\n0.0\n  9\n$PUCSNAME\n  2\n\n  9\n$PUCSORG\n 10\n0.0\n 20\n0.0\n 30\
\n0.0\n  9\n"
<< "$PUCSXDIR\n 10\n1.0\n 20\n0.0\n 30\n0.0\n  9\n$PUCSYDIR\n 10\n0.0\n 20\n1.0\
\n 30\n0.0\n  9\n$USERI1\n 70\n     0\n  9\n$USERI2\n 70\n     0\n  9\n$USERI3\
\n 70\n     0\n  9\n$USERI4\n 70\n     0\n  9\n$USERI5\n 70\n     0\n  9\
\n$USERR1\n"
<< " 40\n0.0\n  9\n$USERR2\n 40\n0.0\n  9\n$USERR3\n 40\n0.0\n  9\n$USERR4\
\n 40\n0.0\n  9\n$USERR5\n 40\n0.0\n  9\n$WORLDVIEW\n 70\n     1\n  9\
\n$SHADEDGE\n 70\n     3\n  9\n$SHADEDIF\n 70\n    70\n  9\n$TILEMODE\n 70\
\n     1\n  9\n$MAXACTVP\n"
<< " 70\n    64\n  9\n$PLIMCHECK\n 70\n     0\n  9\n$PEXTMIN\n 10\n0.0\n 20\
\n0.0\n 30\n0.0\n  9\n$PEXTMAX\n 10\n0.0\n 20\n0.0\n 30\n0.0\n  9\n$PLIMMIN\
\n 10\n0.0\n 20\n0.0\n  9\n$PLIMMAX\n 10\n12.0\n 20\n9.0\n  9\n$UNITMODE\n 70\
\n     0\n  9\n"
<< "$VISRETAIN\n 70\n     1\n  9\n$PLINEGEN\n 70\n     0\n  9\n$PSLTSCALE\n 70\
\n     1\n  0\nENDSEC\n  0\nSECTION\n  2\nTABLES\n  0\nTABLE\n  2\nVPORT\n 70\
\n     1\n  0\nVPORT\n  2\n*ACTIVE\n 70\n     0\n 10\n0.0\n 20\n0.0\n 11\n1.0\
\n 21\n1.0\n"
<< " 12\n" << rounding_zero (centre. x) << "\n 22\n" << rounding_zero (centre. y)
<< "\n 13\n0.0\n 23\n0.0\n 14\
\n0.5\n 24\n0.5\n 15\n0.5\n 25\n0.5\n 16\n0.0\n 26\n0.0\n 36\n1.0\n 17\n0.0\
\n 27\n0.0\n 37\n0.0\n 40\n" << rounding_zero (height)
<< "\n 41\n" << rounding_zero (screen_ratio) << "\n 42\n50.0\n 43\n0.0\n 44\n"
<< "0.0\n 50\n0.0\n 51\n0.0\n 71\n     0\n 72\n  1000\n 73\n     1\n 74\
\n     3\n 75\n     0\n 76\n     1\n 77\n     0\n 78\n     0\n  0\nENDTAB\n  0\
\nTABLE\n  2\nLTYPE\n 70\n     2\n  0\nLTYPE\n  2\nCONTINUOUS\n 70\n     0\n  3\
\nSolid line\n 72\n"
<< "    65\n 73\n     0\n 40\n0.0\n  0\nENDTAB\n  0\n"

    // Table of layers
    << "TABLE\n  2\nLAYER\n 70\n     " << number_layers + 1
         << "\n  0\n"
    // Layer 0
    << "LAYER\n  2\n0\n 70\n     0\n 62\n     7\n  6\nCONTINUOUS\n  0\n" ;
    // User layers
    long_unsigned_int i {0} ;
    for (auto name: layer_names) {
        file << "LAYER\n  2\n" << name << "\n 70\n     0\n"
             <<" 62\n     " << layer_colours. at(i)
             << "\n  6\nCONTINUOUS\n  0\n" ;
             i++;
    }
    file << "ENDTAB\n  0\n"

    << "TABLE\n  2\nSTYLE\n 70\n"
         << "     2\n  0\nSTYLE\n  2\nSTANDARD\n 70\n     0\n 40\n0.0\n 41\
\n1.0\n 50\n0.0\n 71\n     0\n 42\n0.2\n  3\ntxt\n  4\n\n  0\nSTYLE\n  2\
\nANNOTATIVE\n 70\n     0\n 40\n0.0\n 41\n1.0\n 50\n0.0\n 71\n     0\n 42\n0.2\
\n  3\ntxt\n  4\n\n  0\n"
<< "ENDTAB\n  0\nTABLE\n  2\nVIEW\n 70\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\
\nUCS\n 70\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nAPPID\n 70\n    10\n  0\
\nAPPID\n  2\nACAD\n 70\n     0\n  0\nAPPID\n  2\nACADANNOPO\n 70\n     0\n  0\
\nAPPID\n  2\n"
<< "ACADANNOTATIVE\n 70\n     0\n  0\nAPPID\n  2\nACAD_DSTYLE_DIMJAG\n 70\
\n     0\n  0\nAPPID\n  2\nACAD_DSTYLE_DIMTALN\n 70\n     0\n  0\nAPPID\n  2\
\nACAD_MLEADERVER\n 70\n     0\n  0\nAPPID\n  2\nACAD_NAV_VCDISPLAY\n 70\
\n     0\n  0\nAPPID\n"
<< "  2\nACAECLAYERSTANDARD\n 70\n     0\n  0\nAPPID\n  2\nACCMTRANSPARENCY\
\n 70\n     0\n  0\nAPPID\n  2\nACAD_EXEMPT_FROM_CAD_STANDARDS\n 70\n     0\
\n  0\nENDTAB\n  0\nTABLE\n  2\nDIMSTYLE\n 70\n     3\n  0\nDIMSTYLE\n  2\
\nSTANDARD\n 70\n     0\n"
<< "  3\n\n  4\n\n  5\n\n  6\n\n  7\n\n 40\n1.0\n 41\n0.18\n 42\n0.0625\n 43\
\n0.38\n 44\n0.18\n 45\n0.0\n 46\n0.0\n 47\n0.0\n 48\n0.0\n140\n0.18\n141\n0.09\
\n142\n0.0\n143\n25.399999999999999\n144\n1.0\n145\n0.0\n146\n1.0\n147\n0.09\
\n 71\n     0\n 72\n"
<< "     0\n 73\n     1\n 74\n     1\n 75\n     0\n 76\n     0\n 77\n     0\
\n 78\n     0\n170\n     0\n171\n     2\n172\n     0\n173\n     0\n174\n     0\
\n175\n     0\n176\n     0\n177\n     0\n178\n     0\n  0\nDIMSTYLE\n  2\
\nANNOTATIVE\n 70\n     0\n"
<< "  3\n\n  4\n\n  5\n\n  6\n\n  7\n\n 40\n0.0\n 41\n0.18\n 42\n0.0625\n 43\
\n0.38\n 44\n0.18\n 45\n0.0\n 46\n0.0\n 47\n0.0\n 48\n0.0\n140\n0.18\n141\n0.09\
\n142\n0.0\n143\n25.399999999999999\n144\n1.0\n145\n0.0\n146\n1.0\n147\n0.09\
\n 71\n     0\n 72\n"
<< "     0\n 73\n     1\n 74\n     1\n 75\n     0\n 76\n     0\n 77\n     0\
\n 78\n     0\n170\n     0\n171\n     2\n172\n     0\n173\n     0\n174\n     0\
\n175\n     0\n176\n     0\n177\n     0\n178\n     0\n  0\nENDTAB\n  0\nENDSEC\
\n  0\nSECTION\n  2\n"
<< "BLOCKS\n  0\nBLOCK\n  8\n0\n  2\n$MODEL_SPACE\n 70\n     0\n 10\n0.0\n 20\
\n0.0\n 30\n0.0\n  3\n$MODEL_SPACE\n  1\n\n  0\nENDBLK\n  5\n21\n  8\n0\n  0\
\nBLOCK\n 67\n     1\n  8\n0\n  2\n$PAPER_SPACE\n 70\n     0\n 10\n0.0\n 20\
\n0.0\n 30\n0.0\n  3\n"
<< "$PAPER_SPACE\n  1\n\n  0\nENDBLK\n  5\n5B\n 67\n     1\n  8\n0\n  0\nENDSEC\
\n  0\nSECTION\n  2\nENTITIES\n  0\n" ;
}

void dpps::Writer_DXF_R12::write_footer () {
    tmpfile << "ENDSEC\n  0\nEOF\n" ;
}

void dpps::Writer_DXF_R12::set_all_parametres (
    //const bool dot_as_polyline,
    const bool two_point_line_as_polyline,
    const enum_dot_dxf_export dot_dxf_export,
    const double julian_date,
    const std::string &layer_name_parametre,
    const std::string &layer_colour_parametre) {

    set_julian_date (julian_date) ;
    set_layer_names (layer_name_parametre) ;
    set_layer_colours (layer_colour_parametre) ;
    set_dot_dxf_export (dot_dxf_export) ;
    set_two_point_line_as_polyline (two_point_line_as_polyline) ;

    //writer_settings. dot_as_polyline = dot_as_polyline ;
    layers_set = true ;
}

void dpps::Writer_DXF_R12::set_parametres (
                            const std::vector<bool> &vbool,
                            const std::vector<long_unsigned_int> &vint,
                            const std::vector<double> &vdouble,
                            const std::vector<std::string> &vstring) {
    if ((vbool. size () != 1)   ||
        (vint. size () != 1)    ||
        (vdouble. size () != 1) ||
        (vstring. size () != 2)) {
        throw bad_parametre ("Writer_DXF_R12::set_parametres",
            1, 1, 1, 2,
            vbool. size (), vint. size (), vdouble. size (), vstring. size ()) ;
    }
    if (vint. at (0) > 2) {
            std::string reason {"Writer_DXF_R12::set_all_parametres, \
dot_dxf_export can value 0 (dot_as_dot), 1 (dot_as_polyline), 2 \
(dot_as_circle), but value was " + std::to_string (vint. at (0))} ;
            throw bad_parametre (reason. c_str ()) ;
    }

    set_all_parametres (vbool. at (0),
                        static_cast<enum_dot_dxf_export>(vint. at (0)),
                        vdouble. at (0), vstring. at (0), vstring. at (1)) ;
}

void dpps::Writer_DXF_R12::write_Polyline (const Polyline &polyline) {
    long_unsigned_int layer_number {
        std::min (polyline. reference, layer_names. size ()-1)} ;
    if ((polyline. vertices. size () == 1) &&
        (writer_settings. dot_dxf_export == dot_as_dot))
        write_dxf_point (polyline. vertices. front (). x,
                         polyline. vertices. front (). y,
                         layer_number) ;
    if ((polyline. vertices. size () == 1) &&
         (writer_settings. dot_dxf_export == dot_as_circle))
        write_dxf_circle (polyline. vertices. front (). x,
                         polyline. vertices. front (). y,
                         polyline. dose,
                         layer_number) ;
    if ((polyline. vertices. size () == 2) && !writer_settings. two_point_line_as_polyline)
        write_dxf_line (polyline. vertices. front (). x,
                        polyline. vertices. front (). y,
                        polyline. vertices. back (). x,
                        polyline. vertices. back (). y,
                        layer_number) ;
    if (((polyline. vertices. size () == 1) && writer_settings. dot_dxf_export == dot_as_polyline) ||
        ((polyline. vertices. size () == 2) && writer_settings. two_point_line_as_polyline) ||
         (polyline. vertices. size () > 2)) {
        write_dxf_polyline_header (polyline. closed, layer_number) ;
        // the reference &p is used to avoid copy
        for (auto &i: polyline.vertices)
            write_dxf_vertex (i.x, i.y, layer_number) ;
        write_dxf_seqend (layer_number) ;
    }
}

///////////////////////////////////////////
// dpps::Writer_DXF_R12:: setter methods //
///////////////////////////////////////////

void dpps::Writer_DXF_R12::set_julian_date (const double julian_date) {
    if (julian_date >= 0)
        writer_settings. julian_date = julian_date ;
    else {
        time_t epoch ;
        time_t now ;
        struct tm epoch_info ;
        epoch_info. tm_year = 70 ; // 1970
        epoch_info. tm_mon  = 0 ;  // January
        epoch_info. tm_mday = 1 ;  // 1st
        epoch_info. tm_hour = 0 ;  // 0 h
        epoch_info. tm_min  = 0 ;  // 0 min
        epoch_info. tm_sec  = 0 ;  // 0 s
        epoch = mktime (&epoch_info) ;
        now   = time (nullptr) ;
        writer_settings. julian_date = 2440587.5 + difftime (now, epoch) / 86400.0 ;
    }
}

void dpps::Writer_DXF_R12::set_layer_names (const std::string &layer_name_parametre) {
    if (layer_name_parametre. empty())
        // the default value
        layer_names. push_back (writer_settings. layer_name_parametre) ;
    else
        layer_names = split_string (layer_name_parametre, ',', true) ;
    if (layer_names. size () == 0) {
        std::string reason {"Writer_DXF_R12::set_all_parametres, layer name \
set to null"} ;
        throw bad_parametre (reason. c_str ()) ;
    }

    for (auto &s : layer_names) {
        s = remove_spaces_left (remove_spaces_right (s)) ;
        if (s. empty ()) {
            std::string reason {"Writer_DXF_R12::set_all_parametres, layer \
cannot be empty nor contain only spaces"} ;
            throw bad_parametre (reason. c_str ()) ;
        }
        if (s == "0") {
            std::string reason {"Writer_DXF_R12::set_all_parametres, a layer \
cannot be named \"0\", which is a reserved named"} ;
            throw bad_parametre (reason. c_str ()) ;
        }
    }
    layers_set = true ;
}

void dpps::Writer_DXF_R12::set_dot_dxf_export (const enum_dot_dxf_export dot_dxf_export) {
    writer_settings. dot_dxf_export = dot_dxf_export ;
}

void dpps::Writer_DXF_R12::set_two_point_line_as_polyline (const bool two_point_line_as_polyline) {
    writer_settings. two_point_line_as_polyline = two_point_line_as_polyline ;
}

void dpps::Writer_DXF_R12::set_layer_colours (const std::string &layer_colour_parametre) {
    if (!layers_set) {
            std::string reason {"Writer_DXF_R12::set_all_parametres, colours \
can only be set after layers are set, call set_layers first()"} ;
            throw bad_parametre (reason. c_str ()) ;
    }
    if (layer_colour_parametre. empty()) {
        long_unsigned_int i {0} ;
        for (auto name: layer_names) {
            layer_colours. push_back (i) ;
            i++ ;
        }
    } else {
        std::vector<std::string> vector_colour_string {split_string (layer_colour_parametre, ',', true)} ;
        for (auto colour: vector_colour_string)
            layer_colours. push_back (std::strtol(colour. c_str(), nullptr, 10)) ;
    }
    if (layer_colours. size() < layer_names. size ()) {
        std::string reason {"Writer_DXF_R12::set_all_parametres, layer colours \
are not enough for all layers"} ;
        throw bad_parametre (reason. c_str ()) ;
    }
    colours_set = true ;
}

///////////////////////////////////////////////////
// dpps::Writer_DXF_R12:: Class-specific methods //
///////////////////////////////////////////////////

void dpps::Writer_DXF_R12::write_dxf_circle (const double x,
     const double y,
     const double radius,
     const long_unsigned_int layer_number) {
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
    // there is a 0 only in the end.
    tmpfile << "CIRCLE\n  5\n" << std::hex << handle
         << "\n  8\n" << name
         << std::setprecision (precision)
         << "\n 10\n" << rounding_zero (x) << "\n 20\n" << rounding_zero (y) << "\n 30\n0.0"
         << "\n 40\n" << rounding_zero (radius) << "\n  0\n" ;
    handle++ ;
}

void dpps::Writer_DXF_R12::write_dxf_point (const double x,
     const double y,
     const long_unsigned_int layer_number) {
    // there is a 0 only in the end.
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
    tmpfile << "POINT\n  5\n" << std::hex << handle
         << "\n  8\n" << name
         << std::setprecision (precision)
         << "\n 10\n" << rounding_zero (x) << "\n 20\n" << rounding_zero (y) << "\n 30\n0.0\n  0\n" ;
    handle++ ;
}

void dpps::Writer_DXF_R12::write_dxf_line (
    const double x1, const double y1,
    const double x2, const double y2,
    const long_unsigned_int layer_number) {
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
    tmpfile << "LINE\n  5\n" << std::hex << handle
         << "\n  8\n" << name
         << std::setprecision (precision)
         << "\n 10\n" << rounding_zero (x1) << "\n 20\n" << rounding_zero (y1) << "\n 30\n0.0"
         << "\n 11\n" << rounding_zero (x2) << "\n 21\n" << rounding_zero (y2) << "\n 31\n0.0\n  0\n" ;
    handle++ ;
}

void dpps::Writer_DXF_R12::write_dxf_polyline_header (
    const bool closed, const long_unsigned_int layer_number) {
  // adds the header for a closed (70 1) polyline.
  // user must then add vertices and finish the line with a seqend.
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
  tmpfile << "POLYLINE\n  5\n" << std::hex << handle
       << "\n  8\n" << name
       << "\n 66\n     1\n 10\n0.0\n 20\n0.0\n 30\n0.0\n 70\n     "
       << (closed ? '1':'0') << "\n  0\n" ;
  handle++ ;
}

void dpps::Writer_DXF_R12::write_dxf_vertex (
    const double x, const double y,
    const long_unsigned_int layer_number) {
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
  tmpfile << "VERTEX\n  5\n" << std::hex << handle
       << "\n  8\n" << name
       << std::setprecision (precision)
       << "\n 10\n" << rounding_zero (x) << "\n 20\n" << rounding_zero (y) << "\n 30\n0.0\n  0\n" ;
  handle++ ;
}

void dpps::Writer_DXF_R12::write_dxf_seqend (
    const long_unsigned_int layer_number) {
    std::string name {"0"};
    if (layer_number < layer_names. size())
        name = layer_names. at (layer_number) ;
  tmpfile << "SEQEND\n  5\n" << std::hex << handle ;
  tmpfile << "\n  8\n" << name
       << "\n  0\n" ;
  handle++ ;
}

void dpps::Writer_DXF_R12::write_Pattern_selected (const Pattern &pattern,
                                                 const selection_t selection) {
    lower_left = pattern. lower_left(selection) * 1.05 ;
    upper_right = pattern. upper_right(selection) * 1.05 ;

    if (!layers_set)
        set_layer_names (default_layer_name) ;
    if (!colours_set)
        set_layer_colours ("") ;

    // handleseed in the header will be equal to max_handle
    // handles in the file will range from min_handle = (dec)581 = (hex)245
    // to (max_handle - 1)
    //max_handle = min_handle + pattern. vertices_size (selection) +
    //             + (2 * pattern. size (selection)) ;
    Writer::write_Pattern_selected (pattern) ;
}

void dpps::Writer_DXF_R12::write_Pattern (const Pattern &pattern) {
    lower_left = pattern. lower_left(-1) * 1.05 ;
    upper_right = pattern. upper_right(-1) * 1.05 ;
    //max_handle = min_handle + pattern. vertices_size (-1) +
    //             + (2 * pattern. size (-1)) ;
    if (!layers_set)
        set_layer_names (default_layer_name) ;
    if (!colours_set)
        set_layer_colours ("") ;
    Writer::write_Pattern (pattern) ;
}

void dpps::Writer_DXF_R12::close () {
    tmpfile. close () ;
    if (tmpfile. bad()) {
        std::string reason {"Writer: tmpfile " + tmpfilename +
                            " could not be closed"} ;
        throw bad_io (reason. c_str ()) ;
    }
    if (layers_set) { // We can only go there if user set the layers.
        // in case user just invoked the two-argument constructor and
        // the instance is destroyed, there is no point in writing the final
        // file, and we cannot call write_final_header() as we don't have a
        // layer list.
        write_final_header () ;
        std::ifstream tmpfile_readonly (tmpfilename, std::ios::in) ;
        file << tmpfile_readonly. rdbuf() ;
        tmpfile_readonly. close () ;
        if (tmpfile_readonly. bad()) {
            std::string reason {"Writer: tmpfile_readonly " + tmpfilename +
                                " could not be closed"} ;
            throw bad_io (reason. c_str ()) ;
        }
    }
    Writer::close() ;
}
