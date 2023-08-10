// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Record.h"

namespace Euclid {
namespace Fits {

/*
 * 70 bytes minus opening and closing quotes
 */
constexpr std::size_t max_short_value_length = 68;

template <>
bool Record<std::string>::has_long_string_value() const {
  return value.length() > max_short_value_length;
}

template <>
bool Record<const char*>::has_long_string_value() const {
  return std::strlen(value) > max_short_value_length;
}

template <>
bool Record<VariantValue>::has_long_string_value() const {
  const auto& id = value.type();
  if (id == typeid(std::string)) {
    return boost::any_cast<std::string>(value).length() > max_short_value_length;
  }
  if (id == typeid(const char*)) {
    return std::strlen(boost::any_cast<const char*>(value)) > max_short_value_length;
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
