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

#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {
namespace FitsIO {

BintableHdu::BintableHdu(fitsfile*& fptr, std::size_t index) :
    RecordHdu(fptr, index) {}

#ifndef COMPILE_READ_COLUMN
#define COMPILE_READ_COLUMN(T) \
    template VecColumn<T> BintableHdu::read_column(std::string) const;
COMPILE_READ_COLUMN(char)
COMPILE_READ_COLUMN(short)
COMPILE_READ_COLUMN(int)
COMPILE_READ_COLUMN(long)
COMPILE_READ_COLUMN(float)
COMPILE_READ_COLUMN(double)
COMPILE_READ_COLUMN(unsigned char)
COMPILE_READ_COLUMN(unsigned short)
COMPILE_READ_COLUMN(unsigned int)
COMPILE_READ_COLUMN(unsigned long)
#undef COMPILE_READ_COLUMN
#endif

#ifndef COMPILE_WRITE_COLUMN
#define COMPILE_WRITE_COLUMN(T) \
    template void BintableHdu::write_column(const Column<T>&) const;
COMPILE_WRITE_COLUMN(char)
COMPILE_WRITE_COLUMN(short)
COMPILE_WRITE_COLUMN(int)
COMPILE_WRITE_COLUMN(long)
COMPILE_WRITE_COLUMN(float)
COMPILE_WRITE_COLUMN(double)
COMPILE_WRITE_COLUMN(unsigned char)
COMPILE_WRITE_COLUMN(unsigned short)
COMPILE_WRITE_COLUMN(unsigned int)
COMPILE_WRITE_COLUMN(unsigned long)
#undef COMPILE_WRITE_COLUMN
#endif

}
}
