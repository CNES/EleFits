// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_HDU_H
#define _ELEFITS_HDU_H

#include "EleFits/Header.h"
#include "EleFitsData/DataUtils.h"
#include "EleFitsData/HduCategory.h"
#include "EleFitsData/KeywordCategory.h"
#include "EleFitsData/Record.h"
#include "EleFitsData/RecordVec.h"

#include <fitsio.h>
#include <memory>

namespace Euclid {
namespace Fits {

class MefFile; // necessary for friend class declaration in Hdu

/**
 * @ingroup header_handlers
 * @brief Base class for `ImageHdu` and `BintableHdu`.
 * @details
 * This class provides services common to all HDUs for reading and writing records.
 * Services exclusively applicable to MEF files (e.g. HDU name or type) are directly provided as methods,
 * while generic services are accessed through the `header()` method
 * (refer to the documentation of the `Header` class).
 */
class Hdu {

  // A non-parent MefFile can be wanting to access the fitsfile of the parent MefFile of the hdu
  // FIXME: approach might be changed in the future
  friend class Euclid::Fits::MefFile;

public:
  /// @cond

  /**
   * @brief A token for the passkey idiom.
   * @details
   * Only few classes should be able to create a Hdu.
   * This is enforced by the private Token constructor with restricted set of friend classes.
   */
  class Token {
    friend class MefFile;
    friend class SifFile;
    friend class Hdu;
    friend class ImageHdu;
    friend class BintableHdu;

  private:
    Token() {}
  };

  /**
   * @brief Constructor.
   * @warning
   * You should not try to instantiate a Hdu yourself,
   * but use the dedicated MefFile creation methods.
   * @details
   * The constructor cannot be protected, because unique pointers are created with the make_unique function,
   * and we definitely don't want make_unique to be a friend!
   * We rely on the passkey idiom: Token is protected and therefore accessible only from MefFile (a fiend class)
   * and classes derived from RecrodHdu.
   */
  Hdu(Token,
      fitsfile*& file,
      long index,
      HduCategory type = HduCategory::Image,
      HduCategory status = HduCategory::Untouched);

  /**
   * @brief Dummy constructor, dedicated to iterators.
   */
  Hdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  virtual ~Hdu() = default;

  /**
   * @brief Get the 0-based index of the HDU.
   */
  long index() const;

  /**
   * @brief Get the type of the HDU.
   * @return Either HduCategory::Image or HduCategory::Bintable
   * @details
   * As opposed to readCategory(), the return value of this method can be tested for equality, e.g.:
   * \code
   * if (ext.type() == HduCategory::Image) {
   *   processImage(ext);
   * }
   * \endcode
   */
  HduCategory type() const;

  /**
   * @ingroup iterators
   * @brief Read the category of the HDU.
   * @details
   * This is more specific than the type of the HDU.
   * The category is a bitmask which encodes more properties,
   * e.g. Primary is more specific than Image, and MetadataPrimary is even more specific.
   * The result of this function should not be tested with operator ==, but rather with HduFilter::accepts().
   * Often, the method matches can be used directly.
   * 
   * This is indeed a read operation, because the header should be parsed,
   * e.g. to know whether the data unit is empty or not.
   * @see HduCategory
   * @see matches
   */
  virtual HduCategory readCategory() const;

  /**
   * @ingroup iterators
   * @brief Check whether the HDU matches a given filter.
   * @param filter The list of categories to be tested
   * @warning
   * Like readCategory, this is a read operation.
   */
  bool matches(HduFilter filter) const;

  /**
   * @brief View as an `ImageHdu`, `ImageRaster`, `BintableHdu` or `BintableColumns` (if possible).
   */
  template <typename T>
  const T& as() const;

  /**
   * @brief Access the header unit to read and write records.
   * @see Header
   */
  const Header& header() const;

  /**
   * @brief Read the extension name.
   */
  std::string readName() const;

  /**
   * @brief Read the extension version.
   */
  long readVersion() const;

  /**
   * @brief Read the number of bytes used by the Hdu.
   * @details
   * Total number of bits in the extension data array exclusive of fill that is needed after the data to complete the last record.
   * According to doc: https://archive.stsci.edu/fits/fits_standard/node39.html#s:conf
  */
  long readDataUnitSize() const;

  /**
   * @brief Write or update the extension name.
   */
  void updateName(const std::string& name) const;

  /**
   * @brief Write or update the extension version.
   */
  void updateVersion(long version) const;

  /**
   * @brief Compute the HDU and data checksums and compare them to the values in the header.
   * @throw ChecksumError if checksums values in header are missing or incorrect
   * @details
   * In case of error, a `ChecksumError` is thrown.
   * The checksum records `CHECKSUM` and `DATASUM` can be missing or invalid.
   * To get details on the error, ask the `ChecksumError` object itself:
   * \code
   * try {
   *   hdu.verifyChecksums();
   * } catch (ChecksumError& e) {
   *   if (e.data == ChecksumError::Status::Missing) {
   *     // ...
   *   }
   * }
   * \endcode
   * @see updateChecksums()
   */
  void verifyChecksums() const;

  /**
   * @brief Compute and write (or update) the HDU and data checksums.
   * @details
   * Two checksums are computed: at whole HDU level (keyword `CHECKSUM`), and at data unit level (keyword `DATASUM`).
   * @see verifyChecksums()
   */
  void updateChecksums() const;

protected:
  /**
   * @brief Set the current HDU to this one.
   * @details
   * The status of the HDU is modified to Touched if it was initially Untouched.
   */
  void touchThisHdu() const;

  /**
   * @brief Set the current HDU to this one for writing.
   * @details
   * Edited is added to the status of the HDU.
   */
  void editThisHdu() const;

  /**
   * @brief The parent file handler.
   * @warning
   * This is a reference to a pointer because we want the pointer to be valid
   * even if file is closed and reopened.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The 1-based CFITSIO HDU index.
   * @warning
   * EleFits HDUs are 0-based in the public API and 1-based in the CfitsioWrapper namespace
   * because CFITSIO indices are 1-based.
   */
  long m_cfitsioIndex;

  /**
   * @brief The HDU type.
   */
  HduCategory m_type;

  /**
   * @brief The header unit handler.
   */
  Header m_header;

  /**
   * @brief The HDU status.
   * @details
   * It is mutable because read/write operations change it.
   * Another option to avoid such practice would be to store a record of statuses in MefFile instead.
   */
  mutable HduCategory m_status;

  /**
   * @brief Dummy file handler dedicated to dummy constructor.
   */
  fitsfile* m_dummyFptr = nullptr;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_HDU_IMPL
#include "EleFits/impl/Hdu.hpp"
#undef _ELEFITS_HDU_IMPL
/// @endcond

#endif
