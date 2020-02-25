/**
 * @file src/lib/FitsFile.cpp
 * @date 07/23/19
 * @author user
 *
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

#include "EL_CfitsioWrapper/FileWrapper.h"

#include "EL_FitsFile/FitsFile.h"

namespace Euclid {
namespace FitsIO {

FitsFile::FitsFile(std::string filename, Permission permission) :
    m_filename(filename),
    m_permission(permission),
    m_open(false) {
  open(filename, permission);
}

FitsFile::~FitsFile() {
  close();
}

std::string FitsFile::filename() const {
  return m_filename;
}

void FitsFile::open(std::string filename, Permission permission) {
  switch (permission) {
  case Permission::READ:
    m_fptr = Cfitsio::File::open(filename, Cfitsio::File::OpenPolicy::READ_ONLY);
    break;
  case Permission::EDIT:
    m_fptr = Cfitsio::File::open(filename, Cfitsio::File::OpenPolicy::READ_WRITE);
    break;
  case Permission::CREATE:
    m_fptr = Cfitsio::File::create_and_open(filename, Cfitsio::File::CreatePolicy::CREATE_ONLY);
    break;
  case Permission::OVERWRITE:
    m_fptr = Cfitsio::File::create_and_open(filename, Cfitsio::File::CreatePolicy::OVER_WRITE);
    break;
  case Permission::TEMPORARY:
    m_fptr = Cfitsio::File::create_and_open(filename, Cfitsio::File::CreatePolicy::CREATE_ONLY);
  }
  m_open = true;
}

void FitsFile::close() {
  if(not m_open)
    return;
  switch (m_permission) {
  case Permission::TEMPORARY:
    close_and_delete();
    break;
  default:
    Cfitsio::File::close(m_fptr);
  }
  m_open = false;
}

void FitsFile::close_and_delete() {
  if(not m_open)
    return; //TODO should we delete if not open?
  Cfitsio::File::close_and_delete(m_fptr);
  m_open = false;
}

}
}
