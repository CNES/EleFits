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

#ifndef _EL_FITSDATA_RASTER_H
#define _EL_FITSDATA_RASTER_H

#include "EL_FitsData/Position.h"
#include "EL_FitsData/Region.h"

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief Loop over supported raster types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EL_FitsIO_PrintSupportedTypes to display all supported types
 * @see EL_FITSIO_FOREACH_RECORD_TYPE
 * @see EL_FITSIO_FOREACH_COLUMN_TYPE
 */
#define EL_FITSIO_FOREACH_RASTER_TYPE(MACRO) \
  MACRO(char, char) \
  MACRO(std::int16_t, int16) \
  MACRO(std::int32_t, int32) \
  MACRO(std::int64_t, int64) \
  MACRO(float, float) \
  MACRO(double, double) \
  MACRO(unsigned char, uchar) \
  MACRO(std::uint16_t, uint16) \
  MACRO(std::uint32_t, uint32) \
  MACRO(std::uint64_t, uint64)

// Forward declaration for Raster::subraster()
template <typename T, long n>
class Subraster;

/**
 * @ingroup image_data_classes
 * @brief Raster of a _n_-dimensional image (2D by default).
 * @tparam The value type, which can be `const`-qualified for read-only rasters
 * @tparam The dimension, which can be >= 0 for fixed dimension, or -1 for variable dimension
 * @details
 * A raster is a contiguous container for the pixel data of an image.
 * It features access and view services.
 * @see Position for details on the fixed- and variable-dimension cases.
 */
template <typename T, long n = 2>
class Raster {

public:
  /**
   * @brief The pixel value type.
   */
  using Value = T;

  /**
   * @brief The dimension template parameter.
   * @details
   * The value of `Raster<T, n>::Dim` is always `n`, irrespective of its sign.
   * In contrast, dimension() provides the actual dimension of the Raster,
   * even in the case of a variable dimension.
   */
  static constexpr long Dim = n;

  /**
   * @name Constructors, destructor.
   */
  /// @{

  /**
   * @brief Destructor.
   */
  virtual ~Raster() = default;

  /**
   * @brief Copy constructor.
   */
  Raster(const Raster<T, n>& rhs) = default;

  /**
   * @brief Move constructor.
   */
  Raster(Raster<T, n>&& rhs) = default;

  /**
   * @brief Copy assignment.
   */
  Raster<T, n>& operator=(const Raster<T, n>& rhs) = default;

  /**
   * @brief Move assignment.
   */
  Raster<T, n>& operator=(Raster<T, n>&& rhs) = default;

  /**
   * @brief Create an empty raster.
   */
  Raster() = default;

  /**
   * @brief Create a raster with given shape and data.
   */
  Raster(Position<n> rasterShape, T* data);

  /// \}
  /**
   * @name Properties.
   */
  /// \{

  // /**
  //  * @brief Get the raster shape.
  //  */
  // const Position<n>& shape() const; // FIXME for 4.0

  /**
   * @brief Get raster domain.
   * @details
   * The domain is the region which spans from the first to the last pixel position.
   * It can be used to loop over all pixels, e.g.:
   * \code
   * for (auto pos : raster.domain()) {
   *   processPixel(pos, raster[pos]);
   * }
   * \endcode
   */
  Region<n> domain() const;

  /**
   * @brief Dimension.
   * @details
   * This corresponds to the `n` template parameter in general,
   * or to the current dimension if variable.
   */
  long dimension() const;

  /**
   * @brief Number of pixels.
   */
  long size() const;

  /**
   * @brief Length along given axis.
   */
  template <long i>
  long length() const;

  /// @}
  /**
   * @name Element access.
   */
  /// @{

  /**
   * @brief Const pointer to the first data element.
   */
  const T* data() const;

  /**
   * @brief Pointer to the first data element.
   * @details
   * If the Raster is read-only, returns `nullptr`.
   */
  T* data();

  /**
   * @brief Raw index of a position.
   */
  long index(const Position<n>& pos) const;

  /**
   * @brief Pixel at given position.
   */
  const T& operator[](const Position<n>& pos) const;

  /**
   * @brief Pixel at given position.
   */
  T& operator[](const Position<n>& pos);

  /**
   * @brief Access the value at given position.
   * @details
   * As opposed to `operator[]()`, negative indices are supported for backward indexing,
   * and bounds are checked.
   * @see operator[]()
   */
  const T& at(const Position<n>& pos) const;

  /**
   * @copydoc at()
   */
  T& at(const Position<n>& pos);

  /// @}
  /**
   * @brief Views.
   */
  /// @{

  /**
   * @brief Create a subraster from given region.
   */
  const Subraster<T, n> subraster(const Region<n>& region) const;

  /**
   * @copydoc subraster().
   */
  Subraster<T, n> subraster(const Region<n>& region);

  // FIXME implement Raster<T, n> slice(const Region<n>& region)

  /// @}

public:
  /**
   * @brief Raster shape, i.e. length along each axis.
   * @deprecated Use method `shape()` instead
   */
  Position<n> shape;

protected:
  /**
   * @brief The data, possibly constant if `T` is `const`-qualified.
   */
  T* m_data;
};

/**
 * @brief Alias to Raster for backward compatibility.
 * @deprecated Use `Raster` instead.
 */
template <typename T, long n = 2>
using PtrRaster = Raster<T, n>;

/**
 * @ingroup image_data_classes
 * @brief `Raster` which references some external `std::vector` data.
 * @deprecated Doesn't bring much added value. Use `Raster` instead.
 */
template <typename T, long n = 2>
class VecRefRaster : public Raster<T, n> {

public:
  /**
   * @brief Destructor.
   */
  virtual ~VecRefRaster() = default;

  /**
   * @brief Copy constructor.
   */
  VecRefRaster(const VecRefRaster&) = default;

  /**
   * @brief Move constructor.
   */
  VecRefRaster(VecRefRaster&&) = default;

  /**
   * @brief Copy assignment.
   */
  VecRefRaster& operator=(const VecRefRaster&) = default;

  /**
   * @brief Move assignment.
   */
  VecRefRaster& operator=(VecRefRaster&&) = default;

  /**
   * @brief Create a raster with given shape and values.
   */
  VecRefRaster(Position<n> shape, std::vector<std::decay_t<T>>& vecRef);

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<T>& vector() const;

  // FIXME implement non-const vector() => implement ReadOnlyError

private:
  const std::vector<T>* m_cVecPtr;
  std::vector<T>* m_vecPtr;
};

/**
 * @ingroup image_data_classes
 * @brief `Raster` which stores internally the data as a vector.
 */
template <typename T, long n = 2>
class VecRaster : public Raster<T, n> {

public:
  /**
   * @brief Destructor.
   */
  virtual ~VecRaster() = default;

  /**
   * @brief Copy constructor.
   */
  VecRaster(const VecRaster&) = default;

  /**
   * @brief Move constructor.
   */
  VecRaster(VecRaster&&) = default;

  /**
   * @brief Copy assignment.
   */
  VecRaster& operator=(const VecRaster&) = default;

  /**
   * @brief Move assignment.
   */
  VecRaster& operator=(VecRaster&&) = default;

  /**
   * @brief Create a raster with given shape and values.
   * @details
   * To transfer ownership of the data instead of copying it, use move semantics:
   * \code
   * VecRaster column(shape, std::move(data));
   * \endcode
   */
  VecRaster(Position<n> shape, std::vector<std::decay_t<T>> vec);

  /**
   * @brief Create a VecRaster with given shape and empty data.
   */
  explicit VecRaster(Position<n> shape);

  /**
   * @brief Create an empty VecRaster.
   */
  VecRaster() = default;

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<std::decay_t<T>>& vector() const;

  /**
   * @brief Non-const reference to the vector.
   * @details
   * This is especially useful to take ownership through move semantics.
   * \code
   * std::vector<T> v = std::move(raster.vector());
   * \endcode
   */
  std::vector<std::decay_t<T>>& vector(); // FIXME avoid this! E.g. provide move(std::vector) instead

private:
  std::vector<std::decay_t<T>> m_vec;
};

/**
 * @brief Shortcut to create a raster from a shape and data without specifying the pixel type.
 * @tparam n The raster dimension (2 by default)
 * @tparam T The pixel type, should not be specified (deduced from `data`)
 * @details
 * The pixel type is deduced from the `data` parameter.
 * The dimension cannot be deduced from a brace-enclosed list:
 * for example, a shape of `{ width, height, depth }` can be seen as a `Position<3>` or a `Position<-1>`.
 * Like for all raster-related classes and services, the default value is 2.
 * 
 * When using a pre-existing shape, the dimension can be skipped.
 * 
 * Example usages:
 * \code
 * auto raster = makeRaster({ width, height }, data);
 * auto raster = makeRaster<3>({ width, height, depth }, data);
 * auto raster = makeRaster<-1>({ width, height, depth }, data);
 * 
 * Position<3> shape { width, height, depth };
 * auto raster = makeRaster(shape, data);
 * \endcode
 */
template <long n = 2, typename T>
Raster<T, n> makeRaster(const Position<n>& shape, T* data) {
  return PtrRaster<T, n>(shape, data);
}

/**
 * @copydoc makeRaster
 */
template <long n = 2, typename T>
VecRaster<T, n> makeRaster(const Position<n>& shape, std::vector<T>&& data) {
  return VecRaster<T, n>(shape, std::move(data));
}

} // namespace FitsIO
} // namespace Euclid

#include "EL_FitsData/PixelIterator.h"
#include "EL_FitsData/PositionIterator.h"
#include "EL_FitsData/Subraster.h"

/// @cond INTERNAL
#define _EL_FITSDATA_RASTER_IMPL
#include "EL_FitsData/impl/Raster.hpp"
#undef _EL_FITSDATA_RASTER_IMPL
/// @endcond

#endif
