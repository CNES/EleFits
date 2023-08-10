// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"

#include "EleCfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Fits {

void MefFile::open(const std::string& filename, FileMode permission) {
  open_impl(filename, permission);
}

void MefFile::open_impl(const std::string& filename, FileMode permission) {
  FitsFile::open(filename, permission);
  for (const auto& hdu : *this) {
    m_strategy.opened(hdu);
  }
}

void MefFile::close() {
  close_impl();
}

void MefFile::close_impl() {
  if (not m_fptr) {
    return;
  }
  for (const auto& hdu : *this) {
    m_strategy.closing(hdu);
  }
  FitsFile::close();
}

MefFile::~MefFile() {
  close_impl();
}

long MefFile::hdu_count() const {
  return m_hdus.size();
}

std::vector<std::string> MefFile::read_hdu_names() {
  const long count = hdu_count();
  std::vector<std::string> names(count);
  for (long i = 0; i < count; ++i) {
    names[i] = access<>(i).readName();
  }
  return names;
}

std::vector<std::pair<std::string, long>> MefFile::read_hdu_names_versions() {
  const long count = hdu_count();
  std::vector<std::pair<std::string, long>> namesVersions(count);
  for (long i = 0; i < count; ++i) {
    const auto& hdu = access<>(i);
    const auto n = hdu.readName();
    const auto v = hdu.readVersion();
    namesVersions[i] = std::make_pair(n, v);
  }
  return namesVersions;
}

const Hdu& MefFile::operator[](long index) {
  return access<Hdu>(index);
}

const ImageHdu& MefFile::primary() {
  return access<ImageHdu>(0);
}

} // namespace Fits
} // namespace Euclid
