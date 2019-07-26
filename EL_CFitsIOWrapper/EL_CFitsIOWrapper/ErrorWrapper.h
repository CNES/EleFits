/**
 * @file EL_CFitsIOWrapper/ErrorWrapper.h
 * @date 07/23/19
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

#ifndef _EL_CFITSIOWRAPPER_ERRORWRAPPER_H
#define _EL_CFITSIOWRAPPER_ERRORWRAPPER_H

#include <stdexcept>
#include <cfitsio/fitsio.h>
#include <string>

namespace Cfitsio {

class CfitsioError : public std::runtime_error {
public:
    CfitsioError(std::string message) :
        std::runtime_error(message) {}
};

/**
 * Check whether status is OK (=0) and throw an error if not.
 */
void may_throw_cfitsio_error(int status);

/**
 * Check whether the file is valid and throw an error if not.
 */
void may_throw_invalid_file_error(fitsfile* fptr);

/**
 * Check whether the file is writable and throw an error if not.
 */
void may_throw_readonly_error(fitsfile* fptr);

}

#endif

