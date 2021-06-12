/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _EL_FITSUTILS_STRINGUTILS_H
#define _EL_FITSUTILS_STRINGUTILS_H

#include "ElementsKernel/Auxiliary.h"

#include <string>
#include <vector>

namespace Euclid {
namespace FitsIO {
namespace String {

/**
 * @brief Split a string given a set of character delimiters.
 * @param input The input string
 * @param delimiters The set of delimiters
 */
std::vector<std::string> split(const std::string& input, const std::string& delimiters = "\n");

/**
 * @brief Read a text file.
 * @param filename The path to the file
 */
std::string readFile(const std::string& filename);

/**
 * @brief Read a text file from the auxiliary directory.
 * @param filename The path to the file, relative to the auxiliary directory
 */
std::string readAuxFile(const std::string& filename);

} // namespace String
} // namespace FitsIO
} // namespace Euclid

#endif
