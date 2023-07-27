// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"

#include "EleCfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Fits {

void MefFile::open(const std::string& filename, FileMode permission) {
  openImpl(filename, permission);
}

void MefFile::openImpl(const std::string& filename, FileMode permission) {
  FitsFile::open(filename, permission);
  for (const auto& hdu : *this) {
    m_strategy.afterOpening(hdu);
  }
}

void MefFile::close() {
  closeImpl();
}

void MefFile::closeImpl() {
  for (const auto& hdu : *this) {
    m_strategy.beforeClosing(hdu);
  }
  FitsFile::close();
}

MefFile::~MefFile() {
  closeImpl();
}

MefFile::MefFile(const std::string& filename, FileMode permission) :
    FitsFile(filename, permission),
    m_hdus(std::max(1L, Cfitsio::HduAccess::count(m_fptr))), // 1 for create, count() for open
    m_strategy() {
  if (m_permission != FileMode::Read) {
    strategy(CiteEleFits()); // FIXME document
  }
  openImpl(m_filename, m_permission)
}

long MefFile::hduCount() const {
  return m_hdus.size();
}

std::vector<std::string> MefFile::readHduNames() {
  const long count = hduCount();
  std::vector<std::string> names(count);
  for (long i = 0; i < count; ++i) {
    names[i] = access<>(i).readName();
  }
  return names;
}

std::vector<std::pair<std::string, long>> MefFile::readHduNamesVersions() {
  const long count = hduCount();
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
