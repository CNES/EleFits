// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFile.h"

#include "EleCfitsioWrapper/FileWrapper.h"
#include "EleFitsData/FitsError.h"
#include "ElementsKernel/Project.h"

#include <fstream>

namespace Euclid {
namespace Fits {

std::string version() {
  return Elements::Project::versionString();
}

ReadOnlyError::ReadOnlyError(const std::string& prefix) : FitsError(prefix + ": Trying to write a read-only file.") {}

void ReadOnlyError::may_throw(const std::string& prefix, FileMode mode) {
  if (mode == FileMode::Read) {
    throw ReadOnlyError(prefix);
  }
}

FitsFile::FitsFile(const std::string& filename, FileMode permission) :
    m_fptr(nullptr), m_filename(filename), m_permission(permission) {
  open_impl(filename, permission);
}

FitsFile::~FitsFile() {
  close_impl();
}

std::string FitsFile::filename() const {
  return m_filename;
}

bool FitsFile::is_open() const {
  return m_fptr;
}

void FitsFile::reopen() {
  if (not m_fptr) {
    switch (m_permission) {
      case FileMode::Create:
      case FileMode::Overwrite:
        open(m_filename, FileMode::Edit);
        break;
      case FileMode::Temporary:
        throw FitsError("Cannot reopen closed temporary file.");
        break;
      default:
        open(m_filename, m_permission);
        break;
    }
  }
}

void FitsFile::open(const std::string& filename, FileMode permission) {
  open_impl(filename, permission);
}

void FitsFile::open_impl(const std::string& filename, FileMode permission) {
  if (m_fptr) {
    throw FitsError("Cannot open file '" + filename + "' because '" + m_filename + "' is still open.");
  }
  switch (permission) {
    case FileMode::Read:
      m_fptr = Cfitsio::FileAccess::open(filename, Cfitsio::FileAccess::OpenPolicy::ReadOnly);
      break;
    case FileMode::Write:
      if (fileExists(filename)) {
        m_fptr = Cfitsio::FileAccess::open(filename, Cfitsio::FileAccess::OpenPolicy::ReadWrite);
      } else {
        m_fptr = Cfitsio::FileAccess::create_open(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
      }
      break;
    case FileMode::Edit:
      m_fptr = Cfitsio::FileAccess::open(filename, Cfitsio::FileAccess::OpenPolicy::ReadWrite);
      break;
    case FileMode::Create:
      m_fptr = Cfitsio::FileAccess::create_open(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
      break;
    case FileMode::Overwrite:
      m_fptr = Cfitsio::FileAccess::create_open(filename, Cfitsio::FileAccess::CreatePolicy::OverWrite);
      break;
    case FileMode::Temporary:
      m_fptr = Cfitsio::FileAccess::create_open(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
  }
  m_filename = filename;
  m_permission = permission;
}

void FitsFile::close() {
  close_impl();
}

void FitsFile::close_impl() {
  if (not m_fptr) {
    return;
  }
  switch (m_permission) {
    case FileMode::Temporary:
      close_remove();
      break;
    default:
      Cfitsio::FileAccess::close(m_fptr);
  }
}

void FitsFile::close_remove() {
  if (not m_fptr) {
    return; // TODO should we delete if not open?
  }
  Cfitsio::FileAccess::close_delete(m_fptr);
}

fitsfile* FitsFile::handover_to_cfitsio() {
  auto fptr = m_fptr;
  m_fptr = nullptr;
  return fptr;
}

bool fileExists(const std::string& filename) {
  std::ifstream f(filename);
  return f.is_open();
}

} // namespace Fits
} // namespace Euclid
