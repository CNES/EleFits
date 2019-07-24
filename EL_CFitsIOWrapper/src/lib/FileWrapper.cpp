/**
 * @file src/lib/FileHandler.cpp
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

#include "EL_CFitsIOWrapper/ErrorWrapper.h"
#include "EL_CFitsIOWrapper/FileWrapper.h"
#include "EL_CFitsIOWrapper/HduWrapper.h"

namespace Cfitsio {
namespace File {

fitsfile* create_and_open(std::string filename, bool overwrite) {
    std::string cfitsio_name = filename;
    if(overwrite)
        cfitsio_name.insert (0, 1, '!'); // CFitsIO convention
    fitsfile *fptr;
    int status = 0;
    fits_create_file(&fptr, cfitsio_name.c_str(), &status);
    throw_cfitsio_error(status);
    HDU::init_primary(fptr);
    return fptr;
}

fitsfile* open(std::string filename) {
    fitsfile *fptr;
    int status = 0;
    fits_open_file(&fptr, filename.c_str(), READWRITE, &status);
    throw_cfitsio_error(status);
    HDU::init_primary(fptr);
    return fptr;
}

void close(fitsfile *fptr) {
    if(not fptr)
        return;
    int status = 0;
    fits_close_file(fptr, &status);
    throw_cfitsio_error(status);
    fptr = nullptr;
}

void close_and_delete(fitsfile *fptr) {
    if(not fptr)
        return;
    int status = 0;
    fits_delete_file(fptr, &status);
    throw_cfitsio_error(status);
    fptr = nullptr;
}

}
}

