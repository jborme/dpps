#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#
# This script processes coordinates obtained by copying glyphs as
# SVG paths from Inkscape (flattened Beziers)
#
#
# Typically a glyph is represented as:
# Extract of character 39 '
# M 83.786388,-15.135171 L [...] 83.216148,-14.860611 L 83.786388,-15.135171
# We want to convert it to
# {{{ {83.786388,-15.135171},{83.216148,-14.860611},{83.786388,-15.135171} }}}
#
# In the case of characters with several parts
# Extract of character 33 !
# M 4.8516155,-15.134607 L [...] 4.4080855,-14.923387 L 4.8516055,-15.134587
# [...] M 4.1124055,-2.2091474 L [...] 3.5949755,-2.1194474 L 4.1124155,-2.20924
# We want to convert it to
# {{ {{4.8516155,-15.134607},{4.4080855,-14.923387},{4.8516055,-15.134587}},
#    {{4.1124055,-2.20914 L [...] 3.59497,-2.1194474 L 4.1124155,-2.2092474}} }}
#
# We can:
# * replace L by },{
# * replace ^M by {{{{
# * replace M by }},{{
# * add }}}} at the end of the line
# We can also strip comment lines if they confuse doxygen latex mode,
# or replace them in the code by plain names.

case $# in
    1) input=$1
    ;;
    0) echo "Helper script to process font data collected from SVG"
       echo "in a particular format to include in C++ code."
       echo "It needs the name of data file, like vollkorn-normal.in"
    ;;
esac
# http://stackoverflow.com/questions/965053/extract-filename-and-extension-in-bash
# https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html
#filename=$(basename "${input}")
#output="${filename%.*}"

cat ${input} | sed -e "s/L/},{/g" -e "s/^M/{{{/g" -e "s/M/}},{{/g" -e "/^{/ s/$/}}},/" -e "$ s/,$//"
#> ${output}.hh
