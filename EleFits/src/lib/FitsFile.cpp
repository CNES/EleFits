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

void ReadOnlyError::mayThrow(const std::string& prefix, FileMode mode) {
  if (mode == FileMode::Read) {
    throw ReadOnlyError(prefix);
  }
}

FitsFile::FitsFile(const std::string& filename, FileMode permission) :
    m_fptr(nullptr), m_filename(filename), m_permission(permission), m_open(false) {
  openImpl(filename, permission);
}

FitsFile::~FitsFile() {
  closeImpl();
}

std::string FitsFile::filename() const {
  return m_filename;
}

bool FitsFile::isOpen() const {
  return m_open;
}

void FitsFile::reopen() {
  if (not m_open) {
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
  openImpl(filename, permission);
}

void FitsFile::openImpl(const std::string& filename, FileMode permission) {
  if (m_open) {
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
        m_fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
      }
      break;
    case FileMode::Edit:
      m_fptr = Cfitsio::FileAccess::open(filename, Cfitsio::FileAccess::OpenPolicy::ReadWrite);
      break;
    case FileMode::Create:
      m_fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
      break;
    case FileMode::Overwrite:
      m_fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::OverWrite);
      break;
    case FileMode::Temporary:
      m_fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::CreateOnly);
  }
  m_filename = filename;
  m_permission = permission;
  m_open = true; // If this line is reached, no error was raised
}

void FitsFile::close() {
  closeImpl();
}

void FitsFile::closeImpl() {
  if (not m_open) {
    return;
  }
  switch (m_permission) {
    case FileMode::Temporary:
      closeAndDelete();
      break;
    default:
      Cfitsio::FileAccess::close(m_fptr);
  }
  m_open = false;
}

void FitsFile::closeAndDelete() {
  if (not m_open) {
    return; // TODO should we delete if not open?
  }
  Cfitsio::FileAccess::closeAndDelete(m_fptr);
  m_open = false;
}

fitsfile* FitsFile::handoverToCfitsio() {
  auto fptr = m_fptr;
  m_fptr = nullptr;
  m_open = false;
  return fptr;
}

bool fileExists(const std::string& filename) {
  std::ifstream f(filename);
  return f.is_open();
}

} // namespace Fits
} // namespace Euclid
