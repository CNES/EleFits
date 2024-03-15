// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/SifFile.h"

#include "EleFits/ImageHdu.h"

namespace Fits {

SifFile::SifFile(const std::string& filename, FileMode permission) :
    FitsFile(filename, permission), m_hdu(ImageHdu::Token {}, m_fptr, 0), m_header(m_hdu.header()),
    m_raster(m_hdu.raster())
{}

const Header& SifFile::header() const
{
  return m_header;
}

const ImageRaster& SifFile::raster() const
{
  return m_raster;
}

void SifFile::verify_checksums() const
{
  m_hdu.verify_checksums();
}

void SifFile::update_checksums() const
{
  m_hdu.update_checksums();
}

} // namespace Fits
