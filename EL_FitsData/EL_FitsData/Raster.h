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

#include <array>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @brief n-dimensional pixel position or image shape, i.e. set of integer coordinates.
 * @details
 * Alias for std::array<long, n>.
 * Indices are long, so we stick to the convention.
 */
template <long n = 2>
using Position = std::array<long, (std::size_t)n>;

/**
 * @brief Raster of a n-dimensional image (2D by default).
 * @details
 * This is an interface to be implemented with a concrete data container (e.g. std::vector).
 * Some implementations are provided with the library,
 * but others could be useful to interface with client code
 * (e.g. with other external libraries with custom containers).
 * @see \ref data-classes
 */
template <typename T, long n = 2>
class Raster {

public:
  /** @brief Destructor. */
  virtual ~Raster() = default;

  /** @brief Default constructor. */
  Raster() = default;

  /**
   * @brief Create a Raster with given shape.
   */
  explicit Raster(Position<n> rasterShape);

  /**
   * @brief Const pointer to the first data element.
   */
  virtual const T *data() const = 0;

  /**
   * @brief Length along given axis.
   */
  template <long i>
  long length() const;

  /**
   * @brief Number of pixels.
   */
  long size() const;

  /**
   * @brief Raw index of a position.
   */
  long index(const Position<n> &pos) const;

  /**
   * @brief Pixel at given position.
   */
  const T &operator[](const Position<n> &pos) const;

  /**
   * @brief Pixel at given position.
   */
  T &operator[](const Position<n> &pos);

public:
  /**
   * @brief Raster shape, i.e. length along each axis.
   */
  Position<n> shape;
};

/**
 * @brief Raster which references some external pointer data.
 * @see \ref data-classes
 */
template <typename T, long n = 2>
class PtrRaster : public Raster<T, n> {

public:
  /** @brief Destructor. */
  virtual ~PtrRaster() = default;
  /** @brief Copy constructor. */
  PtrRaster(const PtrRaster &) = default;
  /** @brief Move constructor. */
  PtrRaster(PtrRaster &&) = default;
  /** @brief Copy assignment. */
  PtrRaster &operator=(const PtrRaster &) = default;
  /** @brief Move assignment. */
  PtrRaster &operator=(PtrRaster &&) = default;

  /**
   * @brief Create a Raster with given shape and values.
   */
  PtrRaster(Position<n> rasterShape, const T *data);

  /** @copydoc Raster::data */
  const T *data() const override;

private:
  const T *m_data;
};

/**
 * @brief Raster which references some external vector data.
 * @details Use it for temporary rasters.
 * @see \ref data-classes
 */
template <typename T, long n = 2>
class VecRefRaster : public Raster<T, n> {

public:
  /** @brief Destructor. */
  virtual ~VecRefRaster() = default;
  /** @brief Copy constructor. */
  VecRefRaster(const VecRefRaster &) = default;
  /** @brief Move constructor. */
  VecRefRaster(VecRefRaster &&) = default;
  /** @brief Copy assignment. */
  VecRefRaster &operator=(const VecRefRaster &) = default;
  /** @brief Move assignment. */
  VecRefRaster &operator=(VecRefRaster &&) = default;

  /**
   * @brief Create a Raster with given shape and values.
   */
  VecRefRaster(Position<n> rasterShape, const std::vector<T> &vectorRef);

  /** @copydoc Raster::data */
  const T *data() const override;

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<T> &vector() const;

private:
  const std::vector<T> &m_ref;
};

/**
 * @brief Raster which stores internally the data as a vector.
 * @see \ref data-classes
 */
template <typename T, long n = 2>
class VecRaster : public Raster<T, n> {

public:
  /** @brief Destructor. */
  virtual ~VecRaster() = default;
  /** @brief Copy constructor. */
  VecRaster(const VecRaster &) = default;
  /** @brief Move constructor. */
  VecRaster(VecRaster &&) = default;
  /** @brief Copy assignment. */
  VecRaster &operator=(const VecRaster &) = default;
  /** @brief Move assignment. */
  VecRaster &operator=(VecRaster &&) = default;

  /**
   * @brief Create a VecRaster with given shape and values.
   * @details
   * To transfer ownership of the data instead of copying it, use move semantics:
   * \code
   * VecRaster column(shape, std::move(data));
   * \endcode
   */
  VecRaster(Position<n> rasterShape, std::vector<T> vector);

  /**
   * @brief Create a VecRaster with given shape and empty data.
   */
  explicit VecRaster(Position<n> rasterShape);

  /**
   * @brief Create an empty VecRaster.
   */
  VecRaster() = default;

  /** @copydoc Raster::data */
  const T *data() const override;

  /**
   * @brief Non-const pointer to the first data element.
   */
  T *data();

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<T> &vector() const;

  /**
   * @brief Non-const reference to the vector, useful to take ownership through move semantics.
   * \code
   * std::vector<T> v = std::move(raster.vector());
   * \endcode
   */
  std::vector<T> &vector();

private:
  std::vector<T> m_vec;
};

} // namespace FitsIO
} // namespace Euclid

#include "impl/Raster.hpp"

#endif
