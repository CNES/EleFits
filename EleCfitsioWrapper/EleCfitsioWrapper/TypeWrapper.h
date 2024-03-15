// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_TYPEWRAPPER_H
#define _ELECFITSIOWRAPPER_TYPEWRAPPER_H

#include "Linx/Base/TypeUtils.h" // Index

#include <complex>
#include <fitsio.h>
#include <string>
#include <vector>

namespace Cfitsio {

/**
 * @brief Type traits to convert C++ types to CFITSIO type codes.
 * @details
 * Used to read and write:
 * * Records,
 * * Images,
 * * Binary tables (ASCII table not supported).
 */
template <typename T>
struct TypeCode {
  /**
   * @brief Get the type code for a record.
   */
  inline static int for_record();

  /**
   * @brief Get the type code for a binary table.
   */
  inline static int for_bintable();

  /**
   * @brief Get the TFORM value to handle binary table columns.
   */
  inline static std::string tform(Linx::Index repeat_count);

  /**
   * @brief Get the type code for an image.
   */
  inline static int for_image();

  /**
   * @brief Get the BITPIX value to handle images.
   */
  inline static int bitpix();
};

} // namespace Cfitsio

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_TYPEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/TypeWrapper.hpp"
#undef _ELECFITSIOWRAPPER_TYPEWRAPPER_IMPL
/// @endcond

#endif
