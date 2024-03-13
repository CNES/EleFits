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
VecColumn<T> RandomTable::generate_column(const std::string& type_name, Linx::Index repeat_count, Linx::Index row_count)
{
  std::vector<std::string> type_chunks;
  std::string chunk;
  std::istringstream chunk_stream(type_name);
  while (std::getline(chunk_stream, chunk, ' ')) {
    type_chunks.push_back(chunk);
  }
  const auto prefix_count = type_chunks.size() - 1;
  std::string suffix = type_chunks[prefix_count];
  std::string prefixes(prefix_count, 0);
  for (std::size_t i = 0; i < prefix_count; ++i) {
    prefixes[i] = type_chunks[i][0];
  }
  std::string keyword = prefixes + suffix;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), [](unsigned char c) {
    return std::toupper(c);
  });
  return VecColumn<T>(
      {keyword, prefixes + suffix[0], repeat_count},
      generate_random_vector<T>(repeat_count * row_count));
}

template <typename T>
const VecColumn<T>& RandomTable::get_column() const
{
  return std::get<VecColumn<T>>(columns);
}

template <typename T>
VecColumn<T>& RandomTable::get_column()
{
  return const_cast<VecColumn<T>&>(const_cast<const RandomTable*>(this)->get_column<T>());
}

template <typename T>
RandomScalarColumn<T>::RandomScalarColumn(Linx::Index size, T min, T max) :
    VecColumn<T>({"SCALAR", "m", 1}, generate_random_vector<T>(size, min, max))
{}

template <>
RandomScalarColumn<std::string>::RandomScalarColumn(Linx::Index size, std::string min, std::string max) :
    VecColumn<std::string>({"SCALAR", "m", 1}, generate_random_vector<std::string>(size, min, max))
{
  for (const auto& v : container()) {
    Linx::Index current_size = static_cast<Linx::Index>(v.length() + 1); // +1 for '\0'
    if (current_size > info().repeat_count()) {
      reshape(current_size);
    }
  }
}

template <typename T>
RandomVectorColumn<T>::RandomVectorColumn(Linx::Index repeat_count, Linx::Index row_count, T min, T max) :
    VecColumn<T>({"VECTOR", "m", repeat_count}, generate_random_vector<T>(repeat_count * row_count, min, max))
{}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
