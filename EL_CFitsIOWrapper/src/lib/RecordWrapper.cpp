/**
 * @file src/lib/RecordHandler.cpp
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
#include "EL_CFitsIOWrapper/RecordWrapper.h"
#include "EL_CFitsIOWrapper/TypeWrapper.h"

namespace Cfitsio {
namespace Record {

template<typename T>
T read_value(fitsfile* fptr, std::string keyword) {
  T value;
  char *ckey = keyword.c_str();
  int status = 0;
  fits_read_key(fptr, cfitsio_type<T>::code, ckey, &value, nullptr, &status);
  throw_cfitsio_error(status);
  return value;
}

}
}
