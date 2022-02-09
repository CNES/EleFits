// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_SIFFILE_H
#define _ELEFITS_SIFFILE_H

#include "EleFits/FitsFile.h"
#include "EleFits/ImageHdu.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup file_handlers
 * @brief Single image FITS file handler.
 * @details
 * Through constructor and destructor, `SifFile` handles creation, opening, closing and removing of the SIF-file.
 * Other methods give access to the header unit and image data unit.
 * Note that the data unit is empty at creation, and must be resized to write data.
 * @see \ref handlers
 */
class SifFile : public FitsFile {

public:
  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~SifFile() = default;

  /**
   * @copydoc FitsFile::FitsFile
   */
  SifFile(const std::string& filename, FileMode permission);

  /**
   * @brief Access the header unit.
   */
  const Header& header() const;

  /**
   * @brief Access the data unit.
   */
  const ImageRaster& raster() const;

  /**
   * @brief Write both the records and the raster (resize the data unit if empty).
   */
  template <typename TRaster>
  void writeAll(const RecordSeq& records, const TRaster& raster);

  /**
   * @brief Read the raster.
   * @details
   * This is a shortcut for:
   * - Accessing the data unit;
   * - Reading the raster.
   * 
   * Anything more complex (e.g. region-wise reading) can be done via `raster()`.
   */
  template <typename T, long N = 2>
  VecRaster<T, N> readRaster() const;

  /**
   * @brief Write the raster (resize the data unit if empty).
   * @details
   * This is a shortcut for:
   * - Accessing the data unit;
   * - Resizing it;
   * - Writing the raster.
   * 
   * Anything more complex (e.g. region-wise writing) can be done via `raster()`.
   */
  template <typename TRaster>
  void writeRaster(const TRaster& raster) const;

  /**
   * @copydoc Hdu::verifyChecksums()
   */
  void verifyChecksums() const;

  /**
   * @copydoc Hdu::updateChecksums()
   */
  void updateChecksums() const;

private:
  /**
   * @brief The Primary (and only) HDU
   */
  ImageHdu m_hdu;

  /**
   * @brief The header unit.
   */
  const Header& m_header;

  /**
   * @brief The data unit.
   */
  const ImageRaster& m_raster;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_SIFFILE_IMPL
#include "EleFits/impl/SifFile.hpp"
#undef _ELEFITS_SIFFILE_IMPL
/// @endcond

#endif
