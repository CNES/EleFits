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

#include "EL_FitsData/TestRecord.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

constexpr long RandomHeader::recordCount;

RandomHeader::RandomHeader() :
    b { generateRandomRecord<bool>("bool") },
    c { generateRandomRecord<char>("char") },
    s { generateRandomRecord<short>("short") },
    i { generateRandomRecord<int>("int") },
    l { generateRandomRecord<short>("long") },
    ll { generateRandomRecord<long long>("long long") },
    uc { generateRandomRecord<unsigned char>("unsigned char") },
    us { generateRandomRecord<unsigned short>("unsigned short") },
    ui { generateRandomRecord<unsigned int>("unsigned int") },
    ul { generateRandomRecord<unsigned short>("unsigned long") },
    ull { generateRandomRecord<unsigned long long>("unsigned long long") },
    f { generateRandomRecord<float>("float") },
    d { generateRandomRecord<double>("double") },
    cf { generateRandomRecord<std::complex<float>>("complex float") },
    cd { generateRandomRecord<std::complex<double>>("complex double") },
    str { generateRandomRecord<std::string>("string") },
    cstr { generateRandomRecord<const char *>("C string") } {
}

RecordVector<boost::any> RandomHeader::allRecords() const {
  return { b, c, s, i, l, ll, uc, us, ui, ul, ull, f, d, cf, cd, str, cstr };
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid