// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Record.h"

namespace Euclid {
namespace Fits {

/*
 * 70 bytes minus opening and closing quotes
 */
constexpr std::size_t maxShortValueLength = 68;

template <>
bool Record<std::string>::hasLongStringValue() const {
  return value.length() > maxShortValueLength;
}

template <>
bool Record<const char*>::hasLongStringValue() const {
  return std::strlen(value) > maxShortValueLength;
}

template <>
bool Record<VariantValue>::hasLongStringValue() const {
  const auto& id = value.type();
  if (id == typeid(std::string)) {
    return boost::any_cast<std::string>(value).length() > maxShortValueLength;
  }
  if (id == typeid(const char*)) {
    return std::strlen(boost::any_cast<const char*>(value)) > maxShortValueLength;
  }
  return false;
}

#ifndef COMPILE_RECORD_CLASS
#define COMPILE_RECORD_CLASS(type, unused) template struct Record<type>;
ELEFITS_FOREACH_RECORD_TYPE(COMPILE_RECORD_CLASS)
#undef COMPILE_RECORD_CLASS
#endif

} // namespace Fits
} // namespace Euclid
