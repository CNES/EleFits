/**
 * @file src/lib/ErrorHandler.cpp
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

#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"

namespace Cfitsio {

void may_throw_cfitsio_error(int status) {
    if(status == 0)
        return;
    char msg[FLEN_ERRMSG];
    fits_get_errstatus(status, msg);
    throw CfitsioError("CFitsIO error " + std::to_string(status) + ": " + msg);
}

void may_throw_readonly_error(fitsfile* fptr) {
    if(not File::is_writable(fptr))
        may_throw_cfitsio_error(READONLY_FILE);
}

void may_throw_invalid_file_error(fitsfile* fptr) {
    if(not fptr)
        may_throw_cfitsio_error(BAD_FILEPTR);
}

}
