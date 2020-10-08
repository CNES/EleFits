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

#ifdef _EL_FITSDATA_TESTRECORD_IMPL

#include <algorithm> // transform
#include <cctype> // toupper

#include "EL_FitsData/TestUtils.h"
#include "EL_FitsData/TestRecord.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

template <typename T>
Record<T> generateRandomRecord(const std::string &typeName) {
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
  std::string comment = prefixes.empty() ? suffix : prefixes + " " + suffix;
  return generateRandomRecord<T>(keyword, prefixes + suffix[0], comment);
}

template <typename T>
Record<T> generateRandomRecord(const std::string &k, const std::string &u, const std::string &c) {
  return Record<T>(k, generateRandomValue<T>(), u, c);
}

template <>
const Record<bool> &RandomHeader::getRecord<bool>() const;

template <>
const Record<char> &RandomHeader::getRecord<char>() const;

template <>
const Record<short> &RandomHeader::getRecord<short>() const;

template <>
const Record<int> &RandomHeader::getRecord<int>() const;

template <>
const Record<long> &RandomHeader::getRecord<long>() const;

template <>
const Record<long long> &RandomHeader::getRecord<long long>() const;

template <>
const Record<unsigned char> &RandomHeader::getRecord<unsigned char>() const;

template <>
const Record<unsigned short> &RandomHeader::getRecord<unsigned short>() const;

template <>
const Record<unsigned int> &RandomHeader::getRecord<unsigned int>() const;

template <>
const Record<unsigned long> &RandomHeader::getRecord<unsigned long>() const;

template <>
const Record<unsigned long long> &RandomHeader::getRecord<unsigned long long>() const;

template <>
const Record<float> &RandomHeader::getRecord<float>() const;

template <>
const Record<double> &RandomHeader::getRecord<double>() const;

template <>
const Record<std::complex<float>> &RandomHeader::getRecord<std::complex<float>>() const;

template <>
const Record<std::complex<double>> &RandomHeader::getRecord<std::complex<double>>() const;

template <>
const Record<std::string> &RandomHeader::getRecord<std::string>() const;

template <>
const Record<const char *> &RandomHeader::getRecord<const char *>() const;

template <typename T>
Record<T> &RandomHeader::getRecord() {
  return const_cast<Record<T> &>(const_cast<const RandomHeader *>(this)->getRecord<T>());
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
