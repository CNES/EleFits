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
Record<T> generate_random_record(const std::string& type_name) {
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
  std::string comment = prefixes.empty() ? suffix : prefixes + " " + suffix;
  return generate_random_record<T>(keyword, prefixes + suffix[0], comment);
}

template <typename T>
Record<T> generate_random_record(const std::string& k, const std::string& u, const std::string& c) {
  return Record<T>(k, generate_random_value<T>(), u, c);
}

template <>
const Record<bool>& RandomHeader::get_record<bool>() const;

template <>
const Record<char>& RandomHeader::get_record<char>() const;

template <>
const Record<short>& RandomHeader::get_record<short>() const;

template <>
const Record<int>& RandomHeader::get_record<int>() const;

template <>
const Record<long>& RandomHeader::get_record<long>() const;

template <>
const Record<long long>& RandomHeader::get_record<long long>() const;

template <>
const Record<unsigned char>& RandomHeader::get_record<unsigned char>() const;

template <>
const Record<unsigned short>& RandomHeader::get_record<unsigned short>() const;

template <>
const Record<unsigned int>& RandomHeader::get_record<unsigned int>() const;

template <>
const Record<unsigned long>& RandomHeader::get_record<unsigned long>() const;

template <>
const Record<unsigned long long>& RandomHeader::get_record<unsigned long long>() const;

template <>
const Record<float>& RandomHeader::get_record<float>() const;

template <>
const Record<double>& RandomHeader::get_record<double>() const;

template <>
const Record<std::complex<float>>& RandomHeader::get_record<std::complex<float>>() const;

template <>
const Record<std::complex<double>>& RandomHeader::get_record<std::complex<double>>() const;

template <>
const Record<std::string>& RandomHeader::get_record<std::string>() const;

template <>
const Record<const char*>& RandomHeader::get_record<const char*>() const;

template <typename T>
Record<T>& RandomHeader::get_record() {
  return const_cast<Record<T>&>(const_cast<const RandomHeader*>(this)->get_record<T>());
}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
