// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRecord.h"

namespace Euclid {
namespace Fits {
namespace Test {

constexpr long RandomHeader::recordCount;

RandomHeader::RandomHeader() :
    b {generateRandomRecord<bool>("bool")}, c {generateRandomRecord<char>("char")},
    s {generateRandomRecord<short>("short")}, i {generateRandomRecord<int>("int")},
    l {generateRandomRecord<short>("long")}, ll {generateRandomRecord<long long>("long long")},
    uc {generateRandomRecord<unsigned char>("unsigned char")},
    us {generateRandomRecord<unsigned short>("unsigned short")},
    ui {generateRandomRecord<unsigned int>("unsigned int")}, ul {generateRandomRecord<unsigned short>("unsigned long")},
    ull {generateRandomRecord<unsigned long long>("unsigned long long")}, f {generateRandomRecord<float>("float")},
    d {generateRandomRecord<double>("double")}, cf {generateRandomRecord<std::complex<float>>("complex float")},
    cd {generateRandomRecord<std::complex<double>>("complex double")},
    str {generateRandomRecord<std::string>("string")} {}

template <>
const Record<bool>& RandomHeader::getRecord<bool>() const {
  return b;
}

template <>
const Record<char>& RandomHeader::getRecord<char>() const {
  return c;
}

template <>
const Record<short>& RandomHeader::getRecord<short>() const {
  return s;
}

template <>
const Record<int>& RandomHeader::getRecord<int>() const {
  return i;
}

template <>
const Record<long>& RandomHeader::getRecord<long>() const {
  return l;
}

template <>
const Record<long long>& RandomHeader::getRecord<long long>() const {
  return ll;
}

template <>
const Record<unsigned char>& RandomHeader::getRecord<unsigned char>() const {
  return uc;
}

template <>
const Record<unsigned short>& RandomHeader::getRecord<unsigned short>() const {
  return us;
}

template <>
const Record<unsigned int>& RandomHeader::getRecord<unsigned int>() const {
  return ui;
}

template <>
const Record<unsigned long>& RandomHeader::getRecord<unsigned long>() const {
  return ul;
}

template <>
const Record<unsigned long long>& RandomHeader::getRecord<unsigned long long>() const {
  return ull;
}

template <>
const Record<float>& RandomHeader::getRecord<float>() const {
  return f;
}

template <>
const Record<double>& RandomHeader::getRecord<double>() const {
  return d;
}

template <>
const Record<std::complex<float>>& RandomHeader::getRecord<std::complex<float>>() const {
  return cf;
}

template <>
const Record<std::complex<double>>& RandomHeader::getRecord<std::complex<double>>() const {
  return cd;
}

template <>
const Record<std::string>& RandomHeader::getRecord<std::string>() const {
  return str;
}

RecordSeq RandomHeader::allRecords() const {
  return RecordSeq(b, c, s, i, l, ll, uc, us, ui, ul, ull, f, d, cf, cd, str);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
