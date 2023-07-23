// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_RASTER_H
#define _ELEFITSDATA_RASTER_H

#include "EleFitsData/DataContainer.h"
#include "EleFitsData/Position.h"
#include "EleFitsData/Region.h"

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_data_classes
 * @brief Loop over supported raster types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EleFitsPrintSupportedTypes to display all supported types
 * @see ELEFITS_FOREACH_RECORD_TYPE
 * @see ELEFITS_FOREACH_COLUMN_TYPE
 */
#define ELEFITS_FOREACH_RASTER_TYPE(MACRO) \
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

/// @cond
// Issue with forward declarations: https://github.com/doxygen/doxygen/issues/8177

// Forward declaration for Raster::subraster()
template <typename T, long N, typename THolder>
class Subraster;

// Forward declaration for PtrRaster and VecRaster
template <typename T, long N, typename THolder>
class Raster;

/// @endcond

/**
 * @ingroup image_data_classes
 * @brief `Raster` which points to some external data (`THolder` = `T*`).
 */
template <typename T, long N = 2>
using PtrRaster = Raster<T, N, DataContainerHolder<T, T*>>;

/**
 * @ingroup image_data_classes
 * @brief `Raster` which owns a data vector (`THolder` = `std::vector<T>`).
 */
template <typename T, long N = 2>
using VecRaster = Raster<T, N, DataContainerHolder<T, std::vector<T>>>;

/**
 * @ingroup image_data_classes
 * @brief Data of a _n_-dimensional image (2D by default).
 * 
 * @tparam T The value type, which can be `const`-qualified for read-only rasters
 * @tparam N The dimension, which can be &ge; 0 for fixed dimension, or -1 for variable dimension
 * @tparam THolder The underlying data container type
 * 
 * @details
 * A raster is a contiguous container for the pixel data of an image.
 * It features access and view services.
 * 
 * There are two ways of defining the dimension of a `Raster`:
 * - When the dimension is knwon at compile-time (safer),
 *   by giving the dimension parameter a positive or null value;
 * - When the dimension is known at run-time only (more flexible),
 *   by assigning `N = -1`.
 * 
 * In the former case, index and size computations are optimized, and the dimension is enforced.
 * For example, it is not possible to read a 3D image HDU as a 2D `Raster` --
 * which is nice, because an exception will be raised early!
 * In contrast, it is possible to read a 2D image HDU as a 3D `Raster` of third axis lenght =1.
 * 
 * In the latter case, the dimension may vary or be deduced from the file,
 * which is also nice sometimes but puts more responsibility on the shoulders of the user code,
 * as it should check that the actual dimension is acceptable.
 * 
 * `Raster` meets the `ContiguousContainer` requirements,
 * by extending `DataContainer` (e.g. is iterable).
 * `Raster` ensures constant-time access to elements, whatever the dimension of the data,
 * through subscipt operator `Raster::operator[]()`.
 * Bound checking and backward indexing (index <0) are enabled in `Raster::at()`.
 * 
 * `Raster` also implements some arithmetic operators by extending `VectorArithmeticMixin`.
 * For example, two rasters can be added, or a raster can be multiplied by a scalar.
 * 
 * The raster data can be viewed region-wise as a `PtrRaster`,
 * provided that the region is contiguous in memory.
 * Reading and writing non contiguous region is possible: see `ImageRaster`.
 * 
 * @tspecialization{PtrRaster}
 * @tspecialization{VecRaster}
 * 
 * @satisfies{ContiguousContainer}
 * @satisfies{VectorArithmetic}
 * 
 * @see `Position` for details on the fixed- and variable-dimension cases.
 * @see `makeRaster()` for creation shortcuts.
 * 
 * @par_example
 * \code
 * Position<2> shape {2, 3};
 * 
 * // Read/write PtrRaster
 * float data[] = {1, 2, 3, 4, 5, 6};
 * PtrRaster<float> ptrRaster(shape, data);
 * 
 * // Read-only PtrRaster
 * const float cData[] = {1, 2, 3, 4, 5, 6};
 * PtrRaster<const float> cPtrRaster(shape, cData);
 * 
 * // Read/write VecRaster
 * std::vector<float> vec(&data[0], &data[6]);
 * VecRaster<float> vecRaster(shape, std::move(vec));
 * 
 * // Read-only VecRaster
 * std::vector<const float> cVec(&data[0], &data[6]);
 * VecRaster<const float> cVecRaster(shape, std::move(cVec));
 * \endcode
 * 
 * @note
 * Why "raster" and not simply image or array?
 * Mostly for disambiguation purpose:
 * "image" refers to the extension type, while "array" has already several meanings in C/C++.
 * `NdArray` would have been an option, but every related class should have been prefixed with `Nd` for homogeneity:
 * `NdPosition`, `NdRegion`, `VecNdArray`...
 * From the cathodic television era, raster also historically carries the concept of contiguous pixels,
 * is very common in the field of Earth observation,
 * and also belongs to the Java library.
 * All in all, `Raster` seems to be a fair compromise.
 */
template <typename T, long N, typename THolder>
class Raster : public DataContainer<T, THolder, Raster<T, N, THolder>> {
  friend class ImageRaster; // FIXME rm when Subraster is removed

public:
  /**
   * @brief The pixel value type.
   */
  using Value = T;

  /**
   * @brief The dimension template parameter.
   * @details
   * The value of `Raster<T, N, THolder>::Dim` is always `N`, irrespective of its sign.
   * In contrast, `dimension()` provides the actual dimension of the Raster,
   * even in the case of a variable dimension.
   */
  static constexpr long Dim = N;

  /// @group_construction

  ELEFITS_VIRTUAL_DTOR(Raster)
  ELEFITS_COPYABLE(Raster)
  ELEFITS_MOVABLE(Raster)

  /**
   * @brief Constructor.
   * @param shape The raster shape
   */
  explicit Raster(Position<N> shape) :
      DataContainer<T, THolder, Raster<T, N, THolder>>(shapeSize(shape)), m_shape(std::move(shape)) {}

  /**
   * @brief Constructor.
   * @param shape The raster shape
   * @param data The raw data
   */
  template <typename U>
  Raster(Position<N> shape, U* data) :
      DataContainer<T, THolder, Raster<T, N, THolder>>(data, data + shapeSize(shape)), m_shape(std::move(shape)) {}

  /**
   * @brief Constructor.
   * @param shape The raster shape
   * @param args Arguments to be forwarded to the underlying container
   */
  template <typename... Ts>
  Raster(Position<N> shape, Ts&&... args) :
      DataContainer<T, THolder, Raster<T, N, THolder>>(std::forward<Ts>(args)...), m_shape(std::move(shape)) {}

  /// @group_properties

  /**
   * @brief Get the raster shape.
   */
  const Position<N>& shape() const;

  /**
   * @brief Get the raster domain.
   * @details
   * The domain is the region which spans from the first to the last pixel position.
   * 
   * \par_example
   * It can be used to loop over all pixels, e.g.:
   * \code
   * for (auto pos : raster.domain()) {
   *   processPixel(pos, raster[pos]);
   * }
   * \endcode
   */
  Region<N> domain() const;

  /**
   * @brief Get the actual dimension.
   * @details
   * This corresponds to the `N` template parameter in general,
   * or to the current dimension if variable.
   */
  long dimension() const;

  /**
   * @brief Get the length along given axis.
   */
  template <long I>
  long length() const;

  /// @group_elements

  using DataContainer<T, THolder, Raster<T, N, THolder>>::operator[];

  /**
   * @brief Compute the raw index of a given position.
   */
  inline long index(const Position<N>& pos) const;

  /**
   * @brief Access the pixel value at given position.
   */
  inline const T& operator[](const Position<N>& pos) const;

  /**
   * @copybrief operator[]()
   */
  inline T& operator[](const Position<N>& pos);

  /**
   * @copybrief operator[]()
   * @details
   * As opposed to `operator[]()`, negative indices are supported for backward indexing,
   * and bounds are checked.
   * @see operator[]()
   */
  inline const T& at(const Position<N>& pos) const;

  /**
   * @copybrief at()
   */
  inline T& at(const Position<N>& pos);

  /// @group_views

  /**
   * @brief Check whether a region is made of contiguous values in memory.
   * @tparam M The actual region dimension
   * @details
   * A region is contiguous if and only if:
   * - For `i` < `M-1`, `front[i]` = 0 and `back[i]` = -1;
   * - For `i` > `M`, `front[i]` = `back[i]`.
   */
  template <long M = 2>
  bool isContiguous(const Region<N>& region) const;

  /**
   * @brief Create a slice from a given region.
   * @tparam M The dimension of the slice (cannot be -1)
   * @see isContiguous()
   * @see section()
   */
  template <long M = 2>
  const PtrRaster<const T, M> slice(const Region<N>& region) const;

  /**
   * @copybrief slice()
   */
  template <long M = 2>
  PtrRaster<T, M> slice(const Region<N>& region);

  /**
   * @brief Create a section between given indices.
   * @param front The section front index along the last axis
   * @param back The section back index along the last axis
   * @details
   * A section is a maximal slice of dimension `N` or `N`-1.
   * For example, a 3D section of a 3D raster of shape (x, y, z)
   * is a 3D raster of shape (x, y, t) where `t` < `z`,
   * while a 2D section of it is a 2D raster of shape (x, y).
   * 
   * If needed, `section()` can be applied recursively,
   * e.g. to get the x-line at `z` = 4 and `y` = 2:
   * \code
   * auto line = raster.section(4).section(2);
   * \endcode
   * 
   * @see slice()
   */
  const PtrRaster<const T, N> section(long front, long back) const;

  /**
   * @copybrief section(long,long)const
   */
  PtrRaster<T, N> section(long front, long back);

  /**
   * @brief Create a section at given.
   */
  const PtrRaster<const T, N == -1 ? -1 : N - 1> section(long index) const;

  /**
   * @copybrief section(long)const
   */
  PtrRaster<T, N == -1 ? -1 : N - 1> section(long index);

  /// @}

private:
  /**
   * @brief Create a subraster from given region.
   * @details
   * A subraster is a view of the raster data contained in a region.
   * As opposed to a slice or a section, a subraster is not necessarily contiguous in memory.
   * @warning Should be removed.
   * @see isContiguous()
   * @see slice()
   * @see section()
   */
  const Subraster<T, N, THolder> subraster(const Region<N>& region) const; // FIXME rm?

  /**
   * @copydetails subraster().
   */
  Subraster<T, N, THolder> subraster(const Region<N>& region); // FIXME rm?

  /**
   * @brief Raster shape, i.e. length along each axis.
   */
  Position<N> m_shape;
};

/**
 * @relates Raster
 * @brief Shortcut to create a raster from a shape and data without specifying the template parameters.
 * @tparam T The pixel type, should not be specified (automatically deduced)
 * @tparam Longs The axes lengths, should not be specified (automatically deduced)
 * @param data The raster values, which can be either a pointer (or C array) or a vector
 * @param shape The shape as a comma-separated list of `long`s
 * @details
 * \par_example
 * \code
 * Given:
 * - long width, height, depth: The axes lengths;
 * - float* ptr: The pixel values as a pointer;
 * - std::vector<float> vec: The pixel values as a vector;
 * 
 * auto ptrRaster2D = makeRaster(ptr, width, height);
 * auto ptrRaster3D = makeRaster(ptr, width, height, depth);
 * auto vecRaster2D = makeRaster(vec, width, height); // The vector is copied
 * auto vecRaster3D = makeRaster(std::move(vec), width, height, depth); // The vector is moved
 * \endcode
 */
template <typename TContainer, typename... Longs>
Raster<
    typename TContainer::value_type,
    sizeof...(Longs),
    DataContainerHolder<typename TContainer::value_type, TContainer>>
makeRaster(TContainer&& data, Longs... shape) {
  return {{shape...}, std::forward<TContainer>(data)};
}

/**
 * @relates Raster
 * @copybrief makeRaster()
 */
template <typename T, typename... Longs>
PtrRaster<T, sizeof...(Longs)> makeRaster(T* data, Longs... shape) { // FIXME can we merge somehow?
  return {{shape...}, data};
}

/**
 * @relates Raster
 * @brief Get the `BITPIX` value of a given raster.
 */
template <typename TRaster>
constexpr long bitpix(const TRaster&) { // FIXME move to ImageHdu?
  return bitpix<std::decay_t<typename TRaster::Value>>();
}

} // namespace Fits
} // namespace Euclid

#include "EleFitsData/PositionIterator.h"
#include "EleFitsData/Subraster.h"

/// @cond INTERNAL
#define _ELEFITSDATA_RASTER_IMPL
#include "EleFitsData/impl/Raster.hpp"
#undef _ELEFITSDATA_RASTER_IMPL
/// @endcond

#endif
