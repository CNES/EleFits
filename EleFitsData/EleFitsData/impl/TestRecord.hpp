// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_TESTRECORD_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/TestRecord.h"
#include "EleFitsData/TestUtils.h"

#include <algorithm> // transform
#include <cctype> // toupper

namespace Euclid {
namespace Fits {
namespace Test {

template <typename T>
Record<T> generateRandomRecord(const std::string& typeName) {
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
Record<T> generateRandomRecord(const std::string& k, const std::string& u, const std::string& c) {
  return Record<T>(k, generateRandomValue<T>(), u, c);
}

template <>
const Record<bool>& RandomHeader::getRecord<bool>() const;

template <>
const Record<char>& RandomHeader::getRecord<char>() const;

template <>
const Record<short>& RandomHeader::getRecord<short>() const;

template <>
const Record<int>& RandomHeader::getRecord<int>() const;

template <>
const Record<long>& RandomHeader::getRecord<long>() const;

template <>
const Record<long long>& RandomHeader::getRecord<long long>() const;

template <>
const Record<unsigned char>& RandomHeader::getRecord<unsigned char>() const;

template <>
const Record<unsigned short>& RandomHeader::getRecord<unsigned short>() const;

template <>
const Record<unsigned int>& RandomHeader::getRecord<unsigned int>() const;

template <>
const Record<unsigned long>& RandomHeader::getRecord<unsigned long>() const;

template <>
const Record<unsigned long long>& RandomHeader::getRecord<unsigned long long>() const;

template <>
const Record<float>& RandomHeader::getRecord<float>() const;

template <>
const Record<double>& RandomHeader::getRecord<double>() const;

template <>
const Record<std::complex<float>>& RandomHeader::getRecord<std::complex<float>>() const;

template <>
const Record<std::complex<double>>& RandomHeader::getRecord<std::complex<double>>() const;

template <>
const Record<std::string>& RandomHeader::getRecord<std::string>() const;

template <>
const Record<const char*>& RandomHeader::getRecord<const char*>() const;

template <typename T>
Record<T>& RandomHeader::getRecord() {
  return const_cast<Record<T>&>(const_cast<const RandomHeader*>(this)->getRecord<T>());
}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
