/**
 * @file fonts_declarations.hh
 * @author  Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief The types used in fonts here.
 * @date 2014-02-19 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DPPS_FONTS_DECLARATIONS
#define DPPS_FONTS_DECLARATIONS

#include <vector>
#include <array>
#include "Polyline.hh"

namespace dpps {

constexpr short unsigned int maxchars {256} ;
// types used for the raw definition, they allow initialization lists
// typedef std::vector<std::pair<float,float>> stroke ;
// typedef std::vector<stroke> raw_character ; //dpps::Polyline
// typedef std::array<raw_character, maxchars> alphabet_iso8859_1_raw ;
//
// // types used after load, with an easier access to Polyline.
// typedef std::vector<dpps::Polyline> character ;
// typedef std::array<character, maxchars> alphabet_iso8859_1_polyline ;

} // namespace dpps
#endif
