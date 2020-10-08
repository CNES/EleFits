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

#ifdef _EL_FITSDATA_TESTCOLUMN_IMPL

#include <algorithm>

#include "EL_FitsData/TestUtils.h"

#include "EL_FitsData/TestColumn.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

#define GENERATE_COLUMN(type, name) getColumn<type>() = generateColumn<type>(#name, repeatCount, rowCount);

RandomTable::RandomTable(long repeatCount, long rowCount) : columns {} {
  EL_FITSIO_FOREACH_COLUMN_TYPE(GENERATE_COLUMN)
}

template <typename T>
VecColumn<T> RandomTable::generateColumn(const std::string &typeName, long repeatCount, long rowCount) {
  std::vector<std::string> typeChunks;
  std::string chunk;
  std::istringstream chunkStream(typeName);
  while (std::getline(chunkStream, chunk, ' ')) {
    typeChunks.push_back(chunk);
  }
  const auto prefixCount = typeChunks.size() - 1;
  std::string suffix = typeChunks[prefixCount];
  std::string prefixes(prefixCount, 0);
  for (std::size_t i = 0; i < prefixCount; ++i) {
    prefixes[i] = typeChunks[i][0];
  }
  std::string keyword = prefixes + suffix;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), [](unsigned char c) {
    return std::toupper(c);
  });
  return { { keyword, prefixes + suffix[0], repeatCount }, generateRandomVector<T>(repeatCount * rowCount) };
}

/* With C++14
template <typename T>
const VecColumn<T> &RandomTable::getColumn() const {
  return std::get<VecColumn<T>>(columns);
}
*/

template <>
const VecColumn<char> &RandomTable::getColumn() const {
  return std::get<0>(columns);
}

template <>
const VecColumn<std::int16_t> &RandomTable::getColumn() const {
  return std::get<1>(columns);
}

template <>
const VecColumn<std::int32_t> &RandomTable::getColumn() const {
  return std::get<2>(columns);
}

template <>
const VecColumn<std::int64_t> &RandomTable::getColumn() const {
  return std::get<3>(columns);
}

template <>
const VecColumn<float> &RandomTable::getColumn() const {
  return std::get<4>(columns);
}

template <>
const VecColumn<double> &RandomTable::getColumn() const {
  return std::get<5>(columns);
}

template <>
const VecColumn<std::complex<float>> &RandomTable::getColumn() const {
  return std::get<6>(columns);
}

template <>
const VecColumn<std::complex<double>> &RandomTable::getColumn() const {
  return std::get<7>(columns);
}

template <>
const VecColumn<std::string> &RandomTable::getColumn() const {
  return std::get<8>(columns);
}

template <>
const VecColumn<unsigned char> &RandomTable::getColumn() const {
  return std::get<9>(columns);
}

template <>
const VecColumn<std::uint16_t> &RandomTable::getColumn() const {
  return std::get<10>(columns);
}

template <>
const VecColumn<std::uint32_t> &RandomTable::getColumn() const {
  return std::get<11>(columns);
}

template <>
const VecColumn<std::uint64_t> &RandomTable::getColumn() const {
  return std::get<12>(columns);
}

template <typename T>
VecColumn<T> &RandomTable::getColumn() {
  return const_cast<VecColumn<T> &>(const_cast<const RandomTable *>(this)->getColumn<T>());
}

template <typename T>
RandomScalarColumn<T>::RandomScalarColumn(long size, T min, T max) :
    VecColumn<T>({ "SCALAR", "m", 1 }, generateRandomVector<T>(size, min, max)) {
}

template <>
RandomScalarColumn<std::string>::RandomScalarColumn(long size, std::string min, std::string max) :
    VecColumn<std::string>({ "SCALAR", "m", 1 }, generateRandomVector<std::string>(size, min, max)) {
  for (const auto &v : vector()) {
    long currentSize = static_cast<long>(v.length() + 1); // +1 for '\0'
    if (currentSize > info.repeatCount) {
      info.repeatCount = currentSize;
    }
  }
}

template <typename T>
RandomVectorColumn<T>::RandomVectorColumn(long repeatCount, long size, T min, T max) :
    VecColumn<T>({ "VECTOR", "m", repeatCount }, generateRandomVector<T>(repeatCount * size, min, max)) {
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
