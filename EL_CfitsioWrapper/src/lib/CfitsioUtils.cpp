/**
 * @file src/lib/CfitsioUtils.cpp
 * @date 07/27/19
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

#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Euclid {
namespace Cfitsio {

std::unique_ptr<char[]> to_char_ptr(std::string str) {
	const std::size_t size = str.length();
	std::unique_ptr<char[]> c_str(new char[size + 1]);
	strcpy(c_str.get(), str.c_str());
	return c_str;
}

c_str_array::c_str_array(const std::vector<std::string>& data) :
        c_str_array(data.begin(), data.end()) {
}

c_str_array::c_str_array(const std::initializer_list<std::string>& data) :
        c_str_array(data.begin(), data.end()) {
}

char** c_str_array::data() {
    return c_str_vector.data();
}

}
}
