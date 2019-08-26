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

#include <algorithm>

#include "EL_CfitsioWrapper/RecordWrapper.h"

namespace Cfitsio {
namespace Record {

std::string read_value(fitsfile* fptr, std::string keyword) {
    std::vector<char> cvalue(FLEN_VALUE);
    int status = 0;
    fits_read_keyword(fptr, keyword.c_str(), &cvalue[0], nullptr, &status);
    may_throw_cfitsio_error(status);
    return std::string(&cvalue[0]);
}    

std::vector<std::string> read_values(fitsfile* fptr, std::vector<std::string> keywords) {
    std::vector<std::string> values(keywords.size());
    std::transform(keywords.begin(), keywords.end(), values.begin(), [fptr](std::string k) { return read_value(fptr, k); });
    return values;
}

template<>
std::string parse_value<std::string>(fitsfile* fptr, std::string keyword) {
    return read_value(fptr, keyword);
}

template<>
void write_value<std::string>(fitsfile* fptr, std::string keyword, std::string value) {
    return write_value<char*>(fptr, keyword, &value[0]);
}

template<>
void update_value<std::string>(fitsfile* fptr, std::string keyword, std::string value) {
    printf("Updating value %s of %s\n", value.c_str(), keyword.c_str());
    return update_value<char*>(fptr, keyword, &value[0]);
}

}
}

