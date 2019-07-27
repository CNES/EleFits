/**
 * @file EL_CFitsIOWrapper/CfitsioUtils.h
 * @date 07/27/19
 * @author user
 *
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

#ifndef _EL_CFITSIOWRAPPER_CFITSIOUTILS_H
#define _EL_CFITSIOWRAPPER_CFITSIOUTILS_H

#include <string>

namespace Cfitsio {

/**
 * @brief Convert a string to a char*.
 * 
 * Used to work around non-const correctness of CFitsIO.
 */
char* to_char_ptr(std::string str);

inline char* to_char_ptr(std::string str) {
    return const_cast<char*>(str.c_str());
}

}


#endif
