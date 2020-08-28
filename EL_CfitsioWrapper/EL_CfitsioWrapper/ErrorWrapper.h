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

#ifndef _EL_CFITSIOWRAPPER_ERRORWRAPPER_H
#define _EL_CFITSIOWRAPPER_ERRORWRAPPER_H

#include <stdexcept>
#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Runtime error printing CFitsIO messages.
 */
class CfitsioError : public std::runtime_error {

public:

    /**
     * @brief Create from CFitsIO error code.
     * @details This generate the associated CFitsIO message with no context.
     */
    explicit CfitsioError(int cfitsio_status);
    
    /**
     * @brief Create from CFitsIO error code and user-given context.
     * @details This generates the associated CFitsIO message with context.
     */
    CfitsioError(int cfitsio_status, const std::string& context);
    
    /**
     * @brief The CFitsIO error code.
     */
    int status;

};

/**
 * @brief Get the error message of an error code.
 */
std::string cfitsio_error_message(int status);

/**
 * @brief Check whether status is OK (=0) and throw an error if not.
 */
void may_throw_cfitsio_error(int status, const std::string& context="");

/**
 * @brief Check whether the file is valid and throw an error if not.
 */
void may_throw_invalid_file_error(fitsfile* fptr);

/**
 * @brief Check whether the file is writable and throw an error if not.
 */
void may_throw_readonly_error(fitsfile* fptr);

}
}

#endif
