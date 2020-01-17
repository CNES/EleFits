/**
 * @file src/lib/ImageHdu.cpp
 * @date 08/30/19
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

#include "EL_FitsFile/ImageHdu.h"

namespace Euclid {
namespace FitsIO {

ImageHdu::ImageHdu(fitsfile* fptr, std::size_t index) :
        RecordHdu(fptr, index) {}

#define COMPILE_READ_RASTER(T, n) \
        template DataRaster<T, n> ImageHdu::read_raster() const;
COMPILE_READ_RASTER(char, 2)
COMPILE_READ_RASTER(int, 2)
COMPILE_READ_RASTER(float, 2)
COMPILE_READ_RASTER(double, 2)
COMPILE_READ_RASTER(char, 3)
COMPILE_READ_RASTER(int, 3)
COMPILE_READ_RASTER(float, 3)
COMPILE_READ_RASTER(double, 3)

#define COMPILE_WRITE_RASTER(T, n) \
        template void ImageHdu::write_raster(const Raster<T, n>&) const;
COMPILE_WRITE_RASTER(char, 2)
COMPILE_WRITE_RASTER(int, 2)
COMPILE_WRITE_RASTER(float, 2)
COMPILE_WRITE_RASTER(double, 2)
COMPILE_WRITE_RASTER(char, 3)
COMPILE_WRITE_RASTER(int, 3)
COMPILE_WRITE_RASTER(float, 3)
COMPILE_WRITE_RASTER(double, 3)

}
}
