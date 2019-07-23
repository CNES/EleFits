/**
 * @file src/lib/HduHandler.cpp
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

#include "EL_CFitsIOWrapper/HduWrapper.h"
#include "EL_CFitsIOWrapper/ErrorWrapper.h"

namespace Cfitsio {
namespace HDU {

bool init_primary(fitsfile *fptr) {
    const auto count = read_HDU_count(fptr);
    if(count > 0)
        return false;
    const int default_type = BYTE_IMG;
    long naxes = 0;
    int status = 0;
    fits_create_img(fptr, default_type, naxes, nullptr, &status);
    throw_cfitsio_error(status);
    return true;
}

int read_HDU_count(fitsfile *fptr) {
    int count;
    int status = 0;
    fits_get_num_hdus(fptr, &count, &status);
    throw_cfitsio_error(status);
    return count;
}

}
}

