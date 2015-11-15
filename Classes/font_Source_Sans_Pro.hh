/**
 * @file font_Vollkorn_normal.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Data relative to the Polyline approximation of the Vollkorn normal
 * font from designer Friedrich Althausen.
 * @date 2014-02-21 Jérôme Borme: Initial version
 * @copyright
 * This Font Software is licensed under the SIL Open Font License, Version 1.1.
 * This license is available with a FAQ at: http://scripts.sil.org/OFL
 *
 * The data was produced using kcharselect, inkscape and librecad and, of
 * course, the original fonts. All software and data are under free software
 * licence.
 *
 * Workflow to polygonize a font :
 *
 * * the ISO-8859-1 character list is first produced using kcharselect
 * (any other tool like libreoffice or the glyph function in inkscape would
 * work as well).
 *
 * * Write lines with all the glyphs in ISO-8859-in Inkscape, in the desired
 *   typefaces.
 *   For Vollkorn font correct size is 132. It seems that this gives 1 in in
 *   height and DXF export gets height 1 in dxf units.
 *
 * * Optionally, adjust the page size so that this very long line fits. It is
 *   best to put the lines on the lower left corner which is the zero of
 *   inkscape. Optionally, set each line to zero on x.
 *
 * * Select a line, then Object/Object to Path
 *
 * * Select the line, then right-click Ungroup. All elements are now selected,
 *   keep them like this.
 *
 * * Extensions / Modify path / Flatten Beziers, choose 0.1 (the smallest I
 *   could), then Apply.
 *
 * * File / Save a copy as, choose DXF R12 as a format. Seems to me the
 *   question window about postscript is not relevant.
 *
 * * Use the check.lua file on dpps. It will make sure polylines are closed
 *   by removing
 *   and close the polylines. This might be doable in a graphical CAD software
 *   as well, but there is this easy way in dpps so we can use it for that.
 *
 * * Open the exported file into librecad. Erase the lines of typeface that we
 *   do not want, the select the line that we want to process, then move it
 *   so that the lowest point of X letter is at zero in y. Save it to a
 *   significant name describing the font, in DXF R12 format.
 *
 * Many thanks to the designer who wrote the font in the first place.
 *
 * More information about the font is located at:
 * https://www.google.com/fonts/specimen/Vollkorn
 *
 * The following links are the official ones, but they appear to be down at the
 * time of writing.
 * http://friedrichalthausen.de/vollkorn
 * http://vollkorn-typeface.com
 *
 * http://www.w3.org/TR/SVG/paths.html
 * https://fr.wikipedia.org/wiki/Latin1
 */

#ifndef DPPS_FONT_SOURCE_SANS_PRO
#define DPPS_FONT_SOURCE_SANS_PRO
#include "fonts_declarations.hh"

namespace dpps {
alphabet_iso8859_1_raw Source_Sans_Pro_Ultra_Light_0 {{
   //#include "data_Source-Sans-Pro-Ultra-Light_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Light_0 {{
   //#include "data_Source-Sans-Pro-Light_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Normal_0 {{
   //#include "data_Source-Sans-Pro-Normal_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Semi_Bold_0 {{
   //#include "data_Source-Sans-Pro-Semi-Bold_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Bold_0 {{
   //#include "data_Source-Sans-Pro-Bold_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Heavy_0 {{
   //#include "data_Source-Sans-Pro-Heavy_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Ultra_Light_Italic_0 {{
   //#include "data_Source-Sans-Pro-Ultra-Light-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Light_Italic_0 {{
   //#include "data_Source-Sans-Pro-Light-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Italic_0 {{
   //#include "data_Source-Sans-Pro-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Semi_Bold_Italic_0 {{
   //#include "data_Source-Sans-Pro-Semi-Bold-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Bold_Italic_0 {{
   //#include "data_Source-Sans-Pro-Bold-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Heavy_Italic_0 {{
   //#include "data_Source-Sans-Pro-Heavy-Italic_0.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Ultra_Light_1 {{
   //#include "data_Source-Sans-Pro-Ultra-Light_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Light_1 {{
   //#include "data_Source-Sans-Pro-Light_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Normal_1 {{
   //#include "data_Source-Sans-Pro-Normal_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Semi_Bold_1 {{
   //#include "data_Source-Sans-Pro-Semi-Bold_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Bold_1 {{
   //#include "data_Source-Sans-Pro-Bold_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Heavy_1 {{
   //#include "data_Source-Sans-Pro-Heavy_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Ultra_Light_Italic_1 {{
   //#include "data_Source-Sans-Pro-Ultra-Light-Italic_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Light_Italic_1 {{
   //#include "data_Source-Sans-Pro-Light-Italic_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Italic_1 {{
   //#include "data_Source-Sans-Pro-Italic_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Semi_Bold_Italic_1 {{
   //#include "data_Source-Sans-Pro-Semi-Bold-Italic_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Bold_Italic_1 {{
   //#include "data_Source-Sans-Pro-Bold-Italic_1.txt"
}} ;
alphabet_iso8859_1_raw Source_Sans_Pro_Heavy_Italic_1 {{
   //#include "data_Source-Sans-Pro-Heavy-Italic_1.txt"
}} ;
} // namespace dpps

#endif

