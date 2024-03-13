// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/Hdu.h"

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"

namespace Euclid {
namespace Fits {

Hdu::Hdu(Token, fitsfile*& fptr, Linx::Index index, HduCategory type, HduCategory status) :
    m_fptr(fptr), m_cfitsio_index(index + 1), m_type(type),
    m_header(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        }),
    m_status(status)
{}

Hdu::Hdu() : Hdu(Token(), m_dummy_fptr, 0, HduCategory::Image, HduCategory::Untouched) {}

Linx::Index Hdu::index() const
{
  return m_cfitsio_index - 1;
}

HduCategory Hdu::type() const
{
  return m_type;
}

HduCategory Hdu::category() const
{
  touch();
  HduCategory cat = m_type & m_status;
  if (m_cfitsio_index == 1) {
    cat &= HduCategory::Primary;
  } else {
    cat &= HduCategory::Ext;
  }
  return cat;
}

const Header& Hdu::header() const
{
  return m_header;
}

bool Hdu::matches(HduFilter filter) const
{
  return filter.accepts(category());
}

std::string Hdu::read_name() const
{
  touch();
  return Cfitsio::HduAccess::current_name(m_fptr);
}

long Hdu::read_version() const
{
  touch();
  return Cfitsio::HduAccess::current_version(m_fptr);
}

std::size_t Hdu::size_in_file() const
{
  touch();
  return Cfitsio::HduAccess::current_size(m_fptr);
}

void Hdu::update_name(const std::string& name) const
{
  edit();
  Cfitsio::HduAccess::update_name(m_fptr, name);
}

void Hdu::update_version(long version) const
{
  edit();
  Cfitsio::HduAccess::update_version(m_fptr, version);
}

void Hdu::verify_checksums() const
{
  touch();
  int status = 0;
  int datastatus;
  int hdustatus;
  fits_verify_chksum(m_fptr, &datastatus, &hdustatus, &status);
  ChecksumError::may_throw(ChecksumError::Status(hdustatus), ChecksumError::Status(datastatus));
  // TODO wrap in EleCfitsioWrapper
}

void Hdu::update_checksums() const
{
  edit();
  int status = 0;
  fits_write_chksum(m_fptr, &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot write checksums.");
  // TODO wrap in EleCfitsioWrapper
}

void Hdu::touch() const
{
  Cfitsio::HduAccess::goto_index(m_fptr, m_cfitsio_index);
  if (m_status == HduCategory::Untouched) {
    m_status = HduCategory::Touched;
  }
}

void Hdu::edit() const
{
  touch();
  m_status &= HduCategory::Edited;
}

template <>
const Header& Hdu::as() const
{
  return as<Hdu>().header();
}

} // namespace Fits
} // namespace Euclid
