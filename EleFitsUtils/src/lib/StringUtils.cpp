// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsUtils/StringUtils.h"

#include <boost/algorithm/string.hpp>
#include <fstream>

namespace Euclid {
namespace Fits {
namespace String {

std::vector<std::string> split(const std::string& input, const std::string& delimiters) {
  std::vector<std::string> res;
  boost::split(res, input, boost::is_any_of(delimiters));
  return res;
}

std::string trim(const std::string& input, const std::string& characters) {
  if (characters.length() > 0) {
    return boost::trim_copy_if(input, boost::is_any_of(characters));
  }
  return boost::trim_copy(input);
}

std::string read_file(const std::string& filename) {
  std::ifstream ifs(filename);
  return {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
}

std::string read_aux_file(const std::string& filename) {
  return read_file(Elements::getAuxiliaryPath(filename).string());
}

std::unique_ptr<char[]> to_char_ptr(const std::string& str) {
  const long size = str.length();
  auto c_str = std::make_unique<char[]>(size + 1);
  strcpy(c_str.get(), str.c_str());
  return c_str;
}

CStrArray::CStrArray(const std::vector<std::string>& data) : CStrArray(data.begin(), data.end()) {}

CStrArray::CStrArray(const std::initializer_list<std::string>& data) : CStrArray(data.begin(), data.end()) {}

std::size_t CStrArray::size() const {
  return c_str_vector.size();
}

char** CStrArray::data() {
  return c_str_vector.data();
}

} // namespace String
} // namespace Fits
} // namespace Euclid
