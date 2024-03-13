// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_SIFFILE_H
#define _ELEFITS_SIFFILE_H

#include "EleFits/FitsFile.h"
#include "EleFits/ImageHdu.h"
#include "Linx/Base/TypeUtils.h"

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

  /// @group_construction

  /**
   * @copydoc FitsFile::FitsFile
   */
  SifFile(const std::string& filename, FileMode permission = FileMode::Read);

  LINX_NON_COPYABLE(SifFile)
  LINX_NON_MOVABLE(SifFile)

  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~SifFile() = default;

  /// @group_elements

  /**
   * @brief Access the header unit.
   */
  const Header& header() const;

  /**
   * @brief Access the data unit.
   */
  const ImageRaster& raster() const;

  /// @group_operations

  /**
   * @brief Write both the records and the raster (resize the data unit if empty).
   */
  template <typename TRaster>
  void write(const RecordSeq& records, const TRaster& raster);

  /**
   * @copydoc Hdu::verify_checksums()
   */
  void verify_checksums() const;

  /**
   * @copydoc Hdu::update_checksums()
   */
  void update_checksums() const;

  /// @}

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
