// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_MEFFILE_H
#define _ELEFITS_MEFFILE_H

#include "EleFits/BintableHdu.h"
#include "EleFits/FitsFile.h"
#include "EleFits/Hdu.h"
#include "EleFits/ImageHdu.h"
#include "EleFits/Strategy.h"

#include <memory>
#include <vector>

namespace Euclid {
namespace Fits {

/// @cond
// Forward declaration for MefFile::filter()
template <typename T>
class HduSelector;
/// @endcond

/**
 * @ingroup file_handlers
 * @brief Multi-Extension FITS file reader-writer.
 * 
 * In addition to `FitsFile`'s methods, this class provides HDU access and creation services.
 * 
 * A `MefFile` can roughly be seen as a sequence of
 * image HDUs and binary table HDUs (see \ref primer).
 * Methods to access HDUs, header units or data units all return constant references,
 * because they are stateless views on the `MefFile` object,
 * which is the only handler modified by write operations.
 * 
 * At creation, a `MefFile` already contains a Primary,
 * which is empty but can be resized and filled.
 * HDUs are accessed either directly by their index, e.g. with `access()` or operator `[]`,
 * or by finding a {type, name, version} triplet with `find()`.
 * Although it should not be an issue for most files (even with hundreds of HDUs),
 * the second option is much slower because it consists in reading each header unit until a match is found.
 * Once an HDU has been accessed, the reference can be safely reused
 * (please don't re-find an already accessed HDU).
 * It is possible to specify the return type of `access()` and `find()` to best fit target usage, e.g.:
 * 
 * \code
 * const auto& p = f.primary(); // ImageHdu
 * const auto& hdu = f[1]; // Hdu
 * const auto& hdu = f.access<Hdu>(1);
 * const auto& hdu = f.access<ImageHdu>(1);
 * const auto& hdu = f.access<BintableHdu>(1);
 * const auto& h = f.access<Header>(1);
 * const auto& du = f.access<ImageRaster>(1);
 * const auto& du = f.access<BintableColumns>(1);
 * \endcode
 * 
 * Creating extensions can be done in three ways:
 * - Header only: A header unit is created, the data unit is empty;
 * - Null data: A header unit is created,
 *   as well as a data unit which is filled with null values
 *   (as defined by the header contents, e.g. with record `BLANK`);
 * - Complete: A header unit is created,
 *   as well as a data unit which is filled with provided values.
 * 
 * `MefFile` also follows a so-called strategy (see \ref strategy),
 * which is made of actions triggered automatically at various moments (e.g. at file closure).
 * One or some of them may be compression actions,
 * which enable internal compression of image extensions (see \ref compression).
 * The strategy can be defined at construction, or with methods `strategy()`.
 * By default, the strategy consists of a `CiteEleFits` action, which can be disabled with `strategy().clear()`.
 * 
 * @note
 * Single Image FITS files can be handled by this class, but `SifFile` is better suited:
 * it is safer and provides shortcuts.
 * 
 * @see \ref handlers
 * @see \ref iterators
 * @see \ref strategy
 * @see \ref compression
 * @see \ref optim
 */
class MefFile : public FitsFile {

public:
  /// @group_construction

  /**
   * @copybrief FitsFile::FitsFile()
   * @param filename The file name
   * @param mode The opening mode
   * @param actions The strategy or list of actions
   */
  template <typename... TActions>
  explicit MefFile(const std::string& filename, FileMode mode, TActions&&... actions);

  /**
   * @copydoc FitsFile::~FitsFile()
   */
  virtual ~MefFile();

  /**
   * @copydoc FitsFile::close()
   */
  void close() override;

  /// @group_properties

  /**
   * @brief Get the number of HDUs.
   * 
   * As opposed to CFITSIO's HDU counting, incomplete HDUs are taken into account:
   * it is initialized by the constructor and then updated at each modification through `MefFile` methods.
   * This way, incomplete HDUs are also taken into account where CFITSIO would exclude them.
   * This means, for example, that the initial number of HDUs in a new file is 1 instead of 0 with CFITSIO.
   */
  long hduCount() const;

  /**
   * @brief Read the name of each HDU.
   * 
   * Unnamed HDUs are taken into account: an empty string is returned for them.
   */
  std::vector<std::string> readHduNames();

  /**
   * @brief Read the name and version of each HDU.
   * 
   * When there is no name specified, an empty string is returned.
   * When there is no version specified, 1 is returned.
   */
  std::vector<std::pair<std::string, long>> readHduNamesVersions();

  /**
   * @brief Get the strategy.
   */
  inline const Strategy& strategy() const;

  /**
   * @copybrief strategy()
   */
  inline Strategy& strategy();

  /// @group_elements

  /**
   * @brief Access the HDU at given 0-based index.
   * @tparam T The type of HDU or header or data unit handler, i.e.
   * `ImageHdu`, `ImageRaster`, `BintableHdu`, `BintableColumns`, `Hdu` or `Header`.
   * @return A reference to the HDU reader-writer.
   * 
   * Backward indexing is enabled.
   * The default handler is `Hdu`, in which case the returned HDU can still be cast to another handler with `Hdu::as()`, e.g.:
   * \code
   * const auto &ext = f.access<>(-1); // Same as f[-1]
   * auto raster = ext.as<ImageHdu>().raster().read<float>();
   * 
   * // Shortcut
   * auto raster = f.access<ImageRaster>(-1).read<float>();
   * \endcode
   * @see operator[]
   */
  template <class T = Hdu>
  const T& access(long index);

  /**
   * @brief Shortcut for `access<Hdu>(long)`
   */
  const Hdu& operator[](long index);

  /**
   * @brief Access the first HDU with given name, type and version.
   * @tparam T The type of HDU or header or data unit handler
   * @param name The HDU name
   * @param version The HDU version, or 0 to not check the version
   * 
   * The template parameter is used to disambiguate when two extensions of different types have the same name.
   * For example, in a file with an image extension and a binary table extension both named "EXT",
   * `find<ImageHdu>("EXT")` or `find<ImageRaster>()` return the image extension,
   * while `find<BintableHdu>("EXT")` returns the binary table extension,
   * and `find<Hdu>("EXT")` returns whichever of the two has the smallest index.
   * 
   * In the case where several HDUs have the same type, name and version
   * (which is discouraged by the standard, but not forbidden),
   * method `readHduNames()` should be used to get the indices
   * and then `access(long)` should be called.
   * 
   * @see access(long)
   * @see access(const std::string&)
   */
  template <class T = Hdu>
  const T& find(const std::string& name, long version = 0);

  /**
   * @brief Access the only HDU with given name, type and version.
   * @tparam T The type of HDU or header or data unit handler
   * 
   * Throws an exception if several HDUs with given name exists.
   * 
   * @warning
   * In order to ensure uniqueness of the name, all HDUs are visited,
   * which may have a non-negligible I/O cost.
   * 
   * @see find()
   */
  template <class T = Hdu>
  const T& access(const std::string& name, long version = 0);

  /**
   * @brief Access the Primary HDU.
   * @see access()
   */
  const ImageHdu& primary();

  /**
   * @ingroup iterators
   * @brief Get an iterable object which represents a filtered set of HDUs.
   * @return An iterable object, i.e. one for which `begin()` and `end()` functions are provided.
   * 
   * @par_example
   * \code
   * for (const auto& hdu : f.filter<ImageHdu>(HduCategory::Created)) {
   *   processNewImage(hdu);
   * }
   * \endcode
   * @see `iterators`
   */
  template <typename T = Hdu>
  HduSelector<T> filter(const HduFilter& categories = HduCategory::Any);

  /// @group_modifiers

  /**
   * @brief Shortcut for `strategy().append()`.
   */
  template <typename... TActions>
  void strategy(TActions&&... actions);

  /**
  * @brief Append a copy of a given HDU.
  * 
  * If the copied HDU is a Primary, it is appended as an image extension.
  * 
  * Strategy (e.g. compression or checksums verification) is applied.
  * 
  * @warning
  * The source and destination `MefFile`s must be different.
  */
  template <typename T = Hdu>
  const T& append(const T& hdu);

  /**
   * @brief Append a new image extension with empty data unit (`NAXIS = 0`).
   */
  template <typename T = unsigned char>
  const ImageHdu& appendImageHeader(const std::string& name = "", const RecordSeq& records = {});

  /**
   * @brief Append a new image extension and fill the data unit with null values.
   * @tparam T The value type
   * @tparam N The dimension parameter
   * 
   * The null value is defined as follows:
   * - For integer images, if the `BLANK` record is among the provided ones,
   *   then its value is used unscaled (`BZERO` is not taken into account);
   *   Otherwise, the data unit is filled with zeros.
   * - For real images, the data unit is filled with NaNs.
   * 
   * @par_example
   * \code
   * f.appendNullImage<char, 3>("CUBE", {{"BLANK", 255}}, {28, 06, 1989});
   * \endcode
   */
  template <typename T, long N = 2>
  const ImageHdu& appendNullImage(const std::string& name, const RecordSeq& records, const Position<N>& shape);

  /**
   * @brief Append and write a new image extension.
   * @param name The extension name (or an empty string to not write any)
   * @param records The sequence of records to be written in addition to structural records
   * @param raster The data
   */
  template <typename TRaster>
  const ImageHdu& appendImage(const std::string& name, const RecordSeq& records, const TRaster& raster);

  /**
   * @brief Append a binary table extension with empty data unit (0 rows, and possibly 0 columns).
   */
  template <typename... TInfos>
  const BintableHdu&
  appendBintableHeader(const std::string& name = "", const RecordSeq& records = {}, const TInfos&... infos);

  /**
   * @brief Append a binary table extension and fill it with null values.
   * 
   * The null value is defined as follows:
   * - For integer columns, if the `TNULLn` record is among the provided ones,
   *   then its value is used unscaled (`TZEROn` is not taken into account);
   *   Otherwise, the data unit is filled with zeros.
   * - For real columns, the data unit is filled with NaNs.
   */
  template <typename... TInfos>
  const BintableHdu&
  appendNullBintable(const std::string& name, const RecordSeq& records, long rowCount, const TInfos&... infos);

  /**
   * @brief Append and write a new binary table extension.
   */
  template <typename... TColumns>
  const BintableHdu& appendBintable(const std::string& name, const RecordSeq& records, const TColumns&... columns);

  /**
   * @brief Append and write a new binary table extension.
   */
  template <typename TColumns, std::size_t Size = std::tuple_size<TColumns>::value> // FIXME rm Size => enable_if
  const BintableHdu& appendBintable(const std::string& name, const RecordSeq& records, const TColumns& columns);

  /**
   * @brief Remove the HDU with given index.
   */
  inline void remove(long index);

  /// @}

protected:
  /**
   * @copydoc FitsFile::open()
   */
  void open(const std::string& filename, FileMode permission) override;

  /**
   * @brief Vector of `Hdu`s (castable to `ImageHdu` or `BintableHdu`).
   * @warning
   * m_hdus is 0-based while Cfitsio HDUs are 1-based.
   */
  std::vector<std::unique_ptr<Hdu>> m_hdus;

private:
  /**
   * @brief Non virtual implementation of `open()`.
   */
  void open_impl(const std::string& filename, FileMode permission);

  /**
   * @brief Non virtual implementation of `close()`.
   */
  void close_impl();

  /**
   * @brief The strategy.
   */
  Strategy m_strategy;
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
