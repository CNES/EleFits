/**
 * @file EL_FitsData/Raster.h
 * @date 10/21/19
 * @author user
 *
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
#include <functional>
#include <numeric>
#include <vector>


namespace Euclid {
namespace FitsIO {

/**
 * @brief Type for a coordinate along one axis.
 * @details
 * Would be nice to use std::array<T, n>::std::size_t
 * but not compliant with CFitsIO which uses long for subscripts
 */
using coord_type = long;

/**
 * @brief Type for a position or shape, i.e. set of coordinates.
 */
template<std::size_t n>
using pos_type = std::array<coord_type, n>;


/**
 * @brief Raster of a n-dimensional image (2D by default).
 */
template<typename T, std::size_t n=2>
class Raster {

public:

  /** @brief Destructor. */
  virtual ~Raster() = default;

  /** @brief Default constructor. */
  Raster() = default;

  /**
   * @brief Create a Raster with given shape.
   */
  Raster(pos_type<n> input_shape);

  /**
   * @brief Const pointer to the first data element.
   */
  virtual const T* data() const = 0;

  /**
   * @brief Length along given axis.
   */
  template<std::size_t i>
  long length() const;

  /**
   * @brief Number of pixels.
   */
  std::size_t size() const;

  /**
   * @brief Raw index of a position.
   */
  std::size_t index(const pos_type<n>& pos) const;

  /**
   * @brief Pixel at given position.
   */
  const T& operator[](const pos_type<n>& pos) const;

  /**
   * @brief Pixel at given position.
   */
  T& operator[](const pos_type<n>& pos);

public:

  /**
   * @brief Raster shape, i.e. length along each axis.
   */
  pos_type<n> shape;

};


/**
 * @brief Raster which references some external pointer data.
 */
template<typename T, std::size_t n=2>
class PtrRaster : public Raster<T, n> {

public:

  /** @brief Destructor. */
  virtual ~PtrRaster() = default;
  /** @brief Copy constructor. */
  PtrRaster(const PtrRaster&) = default;
  /** @brief Move constructor. */
  PtrRaster(PtrRaster&&) = default;
  /** @brief Copy assignment. */
  PtrRaster& operator=(const PtrRaster&) = default;
  /** @brief Move assignment. */
  PtrRaster& operator=(PtrRaster&&) = default;

  /**
   * @brief Create a Raster with given shape and values.
   */
  PtrRaster(pos_type<n> shape, const T* data);

  virtual const T* data() const;

private:

  const T* m_data;

};


/**
 * @brief Raster which references some external vector data.
 * @details Use it if for temporary rasters.
 */
template<typename T, std::size_t n=2>
class VecRefRaster : public Raster<T, n> {

public:

  /** @brief Destructor. */
  virtual ~VecRefRaster() = default;
  /** @brief Copy constructor. */
  VecRefRaster(const VecRefRaster&) = default;
  /** @brief Move constructor. */
  VecRefRaster(VecRefRaster&&) = default;
  /** @brief Copy assignment. */
  VecRefRaster& operator=(const VecRefRaster&) = default;
  /** @brief Move assignment. */
  VecRefRaster& operator=(VecRefRaster&&) = default;

  /**
   * @brief Create a Raster with given shape and values.
   */
  VecRefRaster(pos_type<n> shape, const std::vector<T>& data);

  virtual const T* data() const;

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<T>& vector() const;

private:

  const std::vector<T>& m_vec_ref;

};


/**
 * @brief Raster which stores internally the data as a vector.
 */
template<typename T, std::size_t n=2>
class VecRaster : public Raster<T, n> {

public:

  /** @brief Destructor. */
  virtual ~VecRaster() = default;
  /** @brief Copy constructor. */
  VecRaster(const VecRaster&) = default;
  /** @brief Move constructor. */
  VecRaster(VecRaster&&) = default;
  /** @brief Copy assignment. */
  VecRaster& operator=(const VecRaster&) = default;
  /** @brief Move assignment. */
  VecRaster& operator=(VecRaster&&) = default;

  /**
   * @brief Create a VecRaster with given shape and values.
   * @details
   * To transfer ownership of the data instead of copying it, use move semantics:
   * @code VecRaster column(shape, std::move(data)); @endcode
   */
  VecRaster(pos_type<n> shape, std::vector<T> data);

  /**
   * @brief Create a VecRaster with given shape and empty data.
   */
  VecRaster(pos_type<n> input_shape);

  /**
   * @brief Create an empty VecRaster.
   */
  VecRaster() = default;

  virtual const T* data() const;

  /**
   * @brief Non-const pointer to the first data element.
   */
  T* data();

  /**
   * @brief Const reference to the vector.
   */
  const std::vector<T>& vector() const;

  /**
   * @brief Non-const reference to the vector, useful to take ownership through move semantics.
   * @code std::vector<T> v = std::move(raster.vector()); @endcode
   */
  std::vector<T>& vector();

private:

  std::vector<T> m_vec;

};


///////////////
// INTERNAL //
/////////////


/// @cond INTERNAL
namespace internal {

template<std::size_t i>
struct Index {
  template<std::size_t n>
  static std::size_t offset(const pos_type<n>& shape, const pos_type<n>& pos);
};

template<std::size_t i>
template<std::size_t n>
inline std::size_t Index<i>::offset(const pos_type<n>& shape, const pos_type<n>& pos) {
  return std::get<n-1-i>(pos) + std::get<n-1-i>(shape) * Index<i-1>::offset(shape, pos);
}

template<>
template<std::size_t n>
inline std::size_t Index<0>::offset(const pos_type<n>& shape, const pos_type<n>& pos) {
  return std::get<n-1>(pos);
}

}
/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
Raster<T, n>::Raster(pos_type<n> input_shape) :
    shape(input_shape) {
}

template<typename T, std::size_t n>
template<std::size_t i>
inline long Raster<T, n>::length() const {
  return std::get<i>(shape);
}

template<typename T, std::size_t n>
inline std::size_t Raster<T, n>::size() const {
  return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<std::size_t>());
}

template<typename T, std::size_t n>
inline std::size_t Raster<T, n>::index(const pos_type<n>& pos) const {
  return internal::Index<n-1>::offset(shape, pos);
}

template<typename T, std::size_t n>
inline const T& Raster<T, n>::operator[](const pos_type<n>& pos) const {
  return data()[index(pos)];
}

template<typename T, std::size_t n>
inline T& Raster<T, n>::operator[](const pos_type<n>& pos) {
  return const_cast<T&>(const_cast<const Raster*>(this)->operator[](pos));
}


template<typename T, std::size_t n>
PtrRaster<T, n>::PtrRaster(pos_type<n> shape, const T* data) :
    Raster<T, n>(shape),
    m_data(data) {
}

template<typename T, std::size_t n>
const T* PtrRaster<T, n>::data() const {
  return m_data;
}


template<typename T, std::size_t n>
VecRefRaster<T, n>::VecRefRaster(pos_type<n> shape, const std::vector<T>& data) :
    Raster<T, n>(shape),
    m_vec_ref(data) {
}

template<typename T, std::size_t n>
const T* VecRefRaster<T, n>::data() const {
  return m_vec_ref.data();
}

template<typename T, std::size_t n>
const std::vector<T>& VecRefRaster<T, n>::vector() const {
  return m_vec_ref;
}


template<typename T, std::size_t n>
VecRaster<T, n>::VecRaster(pos_type<n> shape, std::vector<T> data) :
    Raster<T, n>(shape),
    m_vec(data) {
}

template<typename T, std::size_t n>
VecRaster<T, n>::VecRaster(pos_type<n> input_shape) :
    Raster<T, n>(input_shape),
    m_vec(this->size()) {
}

template<typename T, std::size_t n>
const T* VecRaster<T, n>::data() const {
  return m_vec.data();
}

template<typename T, std::size_t n>
T* VecRaster<T, n>::data() {
  return const_cast<T*>(const_cast<const VecRaster*>(this)->data());
}

template<typename T, std::size_t n>
const std::vector<T>& VecRaster<T, n>::vector() const {
  return m_vec;
}

template<typename T, std::size_t n>
std::vector<T>& VecRaster<T, n>::vector() {
  return m_vec;
}

}
}

#endif
