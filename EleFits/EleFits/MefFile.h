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

#ifndef _ELEFITS_MEFFILE_H
#define _ELEFITS_MEFFILE_H

#include "EleFits/BintableHdu.h"
#include "EleFits/FitsFile.h"
#include "EleFits/Hdu.h"
#include "EleFits/ImageHdu.h"

#include <memory>
#include <vector>

namespace Euclid {
namespace Fits {

// Forward declaration for MefFile::select()
template <typename THdu>
class HduSelector;

/**
 * @ingroup file_handlers
 * @brief Multi-Extension Fits file reader-writer.
 * @details
 * Provide HDU access/create services.
 * Single Image Fits files can be handled by this class, but SifFile is better suited:
 * it is safer and provides shortcuts.
 * @see \ref handlers
 */
class MefFile : public FitsFile {

public:
  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~MefFile() = default;

  /**
   * @copydoc FitsFile::FitsFile
   */
  MefFile(const std::string& filename, FileMode permission);

  /**
   * @brief Get the number of HDUs.
   * @details
   * As opposed to CFitsIO HDU counting, the number is not read by this function:
   * it is initialized by the constructor and then updated at each modification through MefFile methods.
   * This way, incomplete HDUs are also taken into account where CFitsIO would exclude them.
   * This means, for example, that the initial number of HDUs in a new file is 1 instead of 0.
   */
  long hduCount() const;

  /**
   * @brief Read the name of each HDU.
   * @details
   * Unnamed HDUs are taken into account: an empty string is returned for them.
   */
  std::vector<std::string> readHduNames();

  /**
   * @brief Read the name and version of each HDU.
   * @details
   * When there is no name specified, an empty string is returned.
   * When there is no version specified, 1 is returned.
   */
  std::vector<std::pair<std::string, long>> readHduNamesVersions();

  /**
   * @brief Access the HDU at given 0-based index.
   * @tparam T The type of HDU: ImageHdu, BintableHdu, or Hdu to just handle metadata.
   * @return A reference to the HDU reader-writer.
   * @details
   * The type can be ImageHdu, BintableHdu or unspecified (i.e. base class Hdu, the metadata reader-writer.).
   * In the latter case, if needs be, the returned HDU can still be cast to an ImageHdu or BintableHdu:
   * \code
   * const auto &ext = f.access<>(1);
   * ext.as<ImageHdu>().readRaster<float>();
   * \endcode
   * @see operator[]
   */
  template <class T = Hdu>
  const T& access(long index);

  /**
   * @brief Shortcut for access<Hdu>(long)
   */
  const Hdu& operator[](long index);

  /**
   * @brief Access the first HDU with given name, type and version.
   * @tparam T The type of HDU, or Hdu to not check the type
   * @param name The HDU name
   * @param version The HDU version, or 0 to not check the version
   * @details
   * The template parameter is used to disambiguate when two extensions of different types have the same name,
   * if set to ImageHdu or BintableHdu.
   * For example, in a file with an image extension and a binary table extension both named "EXT",
   * `accessFirst<ImageHdu>("EXT")` returns the image extension,
   * while `accessFirst<BintableHdu>("EXT")` returns the binary table extension,
   * and `accessFirst<Hdu>("EXT")` returns whichever of the two has the smallest index.
   * 
   * In the case where several HDUs of same type have the same name
   * (which is discouraged by the standard, but not forbidden),
   * method `readHduNames()` should be used to get the indices
   * and then `access(long)` should be called.
   * @see access(long)
   * @see access(const std::string&)
   */
  template <class T = Hdu>
  const T& accessFirst(const std::string& name, long version = 0);

  /**
   * @brief Access the only HDU with given name, type and version.
   * @details
   * Throws an exception if several HDUs with given name exists.
   * @warning
   * In order to ensure uniqueness of the name, all HDUs are visited,
   * which may have a non-negligible I/O cost.
   * @see accessFirst
   */
  template <class T = Hdu>
  const T& access(const std::string& name, long version = 0);

  /**
   * @brief Access the Primary HDU.
   * @see deprecated, to be replaced with primary()
   */
  template <class T = Hdu>
  const T& accessPrimary();

  /**
   * @brief Access the Primary HDU.
   * @see access()
   */
  const ImageHdu& primary();

  /**
   * @ingroup iterators
   * @brief Select a filtered set of HDUs.
   * @return An iterable object, i.e. one for which begin and end functions are provided.
   */
  template <typename THdu = Hdu>
  HduSelector<THdu> select(const HduFilter& filter = HduCategory::Any);

  /**
   * @brief Append a new Hdu (as an empty ImageHdu) with given name.
   * @return A reference to the new Hdu.
   */
  const Hdu& initRecordExt(const std::string& name);

  /**
   * @brief Append a new ImageHdu with given name and shape.
   * @details
   * To not only initialize the HDU but also write data, use assignImageExt instead.
   */
  template <typename T, long N>
  const ImageHdu& initImageExt(const std::string& name, const Position<N>& shape);

  /**
   * @brief Append an ImageHdu with given name and data.
   * @return A reference to the new ImageHdu.
   */
  template <typename TRaster>
  const ImageHdu& assignImageExt(const std::string& name, const TRaster& raster);

  /**
   * @brief Append a BintableHdu with given name and columns info.
   * @details
   * To not only initialize the HDU but also write data, use assignBintableExt instead.
   */
  template <typename... TInfos>
  const BintableHdu& initBintableExt(const std::string& name, const TInfos&... header);

  /**
   * @brief Append a BintableHdu with given name and data.
   * @return A reference to the new BintableHdu.
   * @warning
   * All columns should have the same number of rows.
   */
  template <typename... TColumns>
  const BintableHdu& assignBintableExt(const std::string& name, const TColumns&... columns);

  /**
   * @brief The index of the Primary HDU.
   * @details
   * The HDU indices are now 0-based while they used to be 1-based.
   * This constant makes migration smoother.
   * It can be used to loop safely over the HDUs:
   * \code
   * for (long i = MefFile::primaryIndex; i < MefFile::primaryIndex + f.hduCount(); ++i) {
   *   const auto &ext = f.access<>(i);
   *   // do something with ext
   * }
   * \endcode
   */
  static constexpr long primaryIndex = 0;

protected:
  /**
   * @brief Append an extension.
   * @return A reference to the new HDU of type T.
   */
  template <class T = Hdu>
  const T& appendExt(T extension);

  /**
   * @brief Append a BintableHdu with given name and data.
   * @return A reference to the new BintableHdu.
   * @tparam TTuple A tuple of columns
   * @tparam Size The number of columns
   * @warning
   * All columns should have the same number of rows.
   */
  template <typename TTuple, std::size_t Size = std::tuple_size<TTuple>::value>
  const BintableHdu& assignBintableExt(const std::string& name, const TTuple& columns);

  /**
   * @brief Vector of `Hdu`s (castable to `ImageHdu` or `BintableHdu`).
   * @warning
   * m_hdus is 0-based while Cfitsio HDUs are 1-based.
   */
  std::vector<std::unique_ptr<Hdu>> m_hdus;
};

} // namespace Fits
} // namespace Euclid

#include "EleFits/HduIterator.h"

/// @cond INTERNAL
#define _ELEFITS_MEFFILE_IMPL
#include "EleFits/impl/MefFile.hpp"
#undef _ELEFITS_MEFFILE_IMPL
/// @endcond

#endif
