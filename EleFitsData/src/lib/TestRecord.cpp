// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRecord.h"

namespace Euclid {
namespace Fits {
namespace Test {

constexpr long RandomHeader::record_count;

RandomHeader::RandomHeader() :
    b {generate_random_record<bool>("bool")}, c {generate_random_record<char>("char")},
    s {generate_random_record<short>("short")}, i {generate_random_record<int>("int")},
    l {generate_random_record<short>("long")}, ll {generate_random_record<long long>("long long")},
    uc {generate_random_record<unsigned char>("unsigned char")},
    us {generate_random_record<unsigned short>("unsigned short")},
    ui {generate_random_record<unsigned int>("unsigned int")}, ul {generate_random_record<unsigned short>("unsigned long")},
    ull {generate_random_record<unsigned long long>("unsigned long long")}, f {generate_random_record<float>("float")},
    d {generate_random_record<double>("double")}, cf {generate_random_record<std::complex<float>>("complex float")},
    cd {generate_random_record<std::complex<double>>("complex double")},
    str {generate_random_record<std::string>("string")} {}

template <>
const Record<bool>& RandomHeader::get_record<bool>() const {
  return b;
}

template <>
const Record<char>& RandomHeader::get_record<char>() const {
  return c;
}

template <>
const Record<short>& RandomHeader::get_record<short>() const {
  return s;
}

template <>
const Record<int>& RandomHeader::get_record<int>() const {
  return i;
}

template <>
const Record<long>& RandomHeader::get_record<long>() const {
  return l;
}

template <>
const Record<long long>& RandomHeader::get_record<long long>() const {
  return ll;
}

template <>
const Record<unsigned char>& RandomHeader::get_record<unsigned char>() const {
  return uc;
}

template <>
const Record<unsigned short>& RandomHeader::get_record<unsigned short>() const {
  return us;
}

template <>
const Record<unsigned int>& RandomHeader::get_record<unsigned int>() const {
  return ui;
}

template <>
const Record<unsigned long>& RandomHeader::get_record<unsigned long>() const {
  return ul;
}

template <>
const Record<unsigned long long>& RandomHeader::get_record<unsigned long long>() const {
  return ull;
}

template <>
const Record<float>& RandomHeader::get_record<float>() const {
  return f;
}

template <>
const Record<double>& RandomHeader::get_record<double>() const {
  return d;
}

template <>
const Record<std::complex<float>>& RandomHeader::get_record<std::complex<float>>() const {
  return cf;
}

template <>
const Record<std::complex<double>>& RandomHeader::get_record<std::complex<double>>() const {
  return cd;
}

template <>
const Record<std::string>& RandomHeader::get_record<std::string>() const {
  return str;
}

RecordSeq RandomHeader::all_record() const {
  return RecordSeq(b, c, s, i, l, ll, uc, us, ui, ul, ull, f, d, cf, cd, str);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
