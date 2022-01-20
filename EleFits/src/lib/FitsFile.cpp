/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

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
  open(filename, permission);
}

FitsFile::~FitsFile() {
  close();
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

bool fileExists(const std::string& filename) {
  std::ifstream f(filename);
  return f.is_open();
}

} // namespace Fits
} // namespace Euclid
