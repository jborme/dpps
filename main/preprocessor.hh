/**
 * @file preprocessor.hh
 * @author Jérôme Borme <jerome.borme@inl.int>
 * @version 1.0
 * @brief Simple preprocessor for the Lua files
 * @date 2014-06-01 Jérôme Borme: Initial version
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * */

#include <string>
#include <vector>

#ifndef DPPS_PREPROCESSOR
#define DPPS_PREPROCESSOR

/** @brief A simple preprocessor for the command-line
 *
 * The replacements parametre contains string characters passed on the
 * command-line. The first is the name of the lua file (compulsory), the second
 * and following are the ones that the user might have passed in addition.
 *
 * The preprocessor will replace occurrences of special sequences by the
 * content of these parameters. #0# will be replaced by the lua file name,
 * #1# will be replaced by the first extra parameter, etc.
 *
 * Sequences of format #0#noaffix# (where 0 can also be 1, 2, 3...) will
 * be replaced by the value of the parameter passed, but without the last
 * affix. This can be useful to use teh radical of a name as a parameter,
 * in particular the name of te lua file #0#.
 *
 * If the lua file refers to a sequence which is not defined (for example the
 * lua file contains #99# but there is not 99 additional parameters), then
 * the sequence is untouched.
 *
 * All occurrences are preprocessed, whether they happen in normal text,
 * in string characters, in comments, etc. Replacement strings passed on the
 * command line are not allowed to contain sequences in substrings. Note that
 * since special sequences start with a # sign, they are not even easy to pass
 * as argument as the shell might interpret the # sign as the beginning of a
 * comment.
 */
std::string preprocess_file (
    const std::string &configuration_filename,
    std::vector<std::string> &replacements) ;

#endif