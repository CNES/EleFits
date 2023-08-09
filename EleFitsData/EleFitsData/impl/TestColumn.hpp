// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_TESTCOLUMN_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestUtils.h"

#include <algorithm>

namespace Euclid {
namespace Fits {
namespace Test {

template <typename T>
VecColumn<T> RandomTable::generateColumn(const std::string& typeName, long repeatCount, long rowCount) {
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
  return {{keyword, prefixes + suffix[0], repeatCount}, generateRandomVector<T>(repeatCount * rowCount)};
}

template <typename T>
const VecColumn<T>& RandomTable::getColumn() const {
  return std::get<VecColumn<T>>(columns);
}

template <typename T>
VecColumn<T>& RandomTable::getColumn() {
  return const_cast<VecColumn<T>&>(const_cast<const RandomTable*>(this)->getColumn<T>());
}

template <typename T>
RandomScalarColumn<T>::RandomScalarColumn(long size, T min, T max) :
    VecColumn<T>({"SCALAR", "m", 1}, generateRandomVector<T>(size, min, max)) {}

template <>
RandomScalarColumn<std::string>::RandomScalarColumn(long size, std::string min, std::string max) :
    VecColumn<std::string>({"SCALAR", "m", 1}, generateRandomVector<std::string>(size, min, max)) {
  for (const auto& v : vector()) {
    long current_size = static_cast<long>(v.length() + 1); // +1 for '\0'
    if (current_size > info().repeatCount()) {
      reshape(current_size);
    }
  }
}

template <typename T>
RandomVectorColumn<T>::RandomVectorColumn(long repeatCount, long rowCount, T min, T max) :
    VecColumn<T>({"VECTOR", "m", repeatCount}, generateRandomVector<T>(repeatCount * rowCount, min, max)) {}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
