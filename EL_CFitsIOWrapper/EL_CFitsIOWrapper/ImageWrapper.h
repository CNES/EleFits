/**
 * @file EL_CFitsIOWrapper/ImageWrapper.h
 * @date 07/25/19
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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include <array>
#include <cfitsio/fitsio.h>
#include <string>
#include <vector>

#include "EL_CFitsIOWrapper/ErrorWrapper.h"
#include "EL_CFitsIOWrapper/TypeWrapper.h"


namespace Cfitsio {
namespace Image {

template<typename T>
struct Raster2D {
    using size_type = long;
    //TODO Would be nice to have std::vector<T>::size_type
    // but not compliant with CFitsIO which uses long for subscripts
    std::vector<T> data;
    std::array<size_type, 2> shape;
    const T& operator()(size_type x, size_type y) const;
    T& operator()(size_type x, size_type y);
};

template<typename T>
Raster2D<T> read_raster_2d(fitsfile* fptr);

template<typename T>
const T& Raster2D<T>::operator()(size_type x, size_type y) const {
    return data[x + y*shape[2]];
}

template<typename T>
T& Raster2D<T>::operator()(size_type x, size_type y) {
    return const_cast<T&>(const_cast<const Raster2D*>(this)->operator()(x, y));
}

template<typename T>
Raster2D<T> read_raster_2d(fitsfile* fptr) {
    Raster2D<T> raster;
    long &rows = raster.shape[1];
    long &cols = raster.shape[0];
    int status = 0;
    fits_get_img_size(fptr, 2, &raster.shape[0], &status);
    throw_cfitsio_error(status);
    raster.data.resize(rows*cols);
    fits_read_img(fptr, TypeCode<T>::for_image(), 1, rows*cols, nullptr, raster.data.data(), nullptr, &status);
    // Number 1 is a 1-base offset (so we read the whole raster here)
    throw_cfitsio_error(status);
    return raster;
}

}
}


#endif
