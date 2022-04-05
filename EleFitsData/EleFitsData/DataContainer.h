// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_DATACONTAINER_H
#define _ELEFITSDATA_DATACONTAINER_H

#include "EleFitsData/ContiguousContainer.h"
#include "EleFitsData/DataUtils.h"
#include "EleFitsData/FitsError.h"
#include "EleFitsData/VectorArithmetic.h"

#include <algorithm> // copy_n
#include <array>
#include <cstddef> // size_t
#include <initializer_list>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_concepts
 * @brief A holder of any contiguous container specified by a size and data pointer.
 * @details
 * The class can be specialized for any container,
 * in which case it should satisfy the `SizedData` requirements.
 * @satisfies{SizedData}
 */
template <typename T, typename TContainer>
class DataContainerHolder {

public:
  /**
   * @brief The concrete container type.
   */
  using Container = TContainer;

  /// @group_construction

  /**
   * @brief Default or size-based constructor.
   */
  template <typename U>
  explicit DataContainerHolder(std::size_t size, U* data) : m_container(size) {
    if (data) {
      std::copy_n(data, size, const_cast<T*>(this->data()));
    }
  }

  /**
   * @brief Forwarding constructor.
   */
  template <typename... TArgs>
  explicit DataContainerHolder(TArgs&&... args) : m_container(std::forward<TArgs>(args)...) {}

  /// @group_properties

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return m_container.size();
  }

  /// @group_elements

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_container.data();
  }

  /// @}

protected:
  /**
   * @brief The underlying container.
   */
  Container m_container;
};

/**
 * @ingroup data_concepts
 * @brief Raw pointer specialization.
 */
template <typename T>
class DataContainerHolder<T, T*> {

public:
  using Container = T*;

  explicit DataContainerHolder(std::size_t size, T* data) : m_size(size), m_container(data) {}

  std::size_t size() const {
    return m_size;
  }

  inline const T* data() const {
    return m_container;
  }

protected:
  /**
   * @brief The number of elements.
   */
  std::size_t m_size;

  /**
   * @brief The raw data pointer.
   */
  T* m_container;
};

/**
 * @ingroup data_concepts
 * @brief `std::array` specialization.
 */
template <typename T, std::size_t N>
class DataContainerHolder<T, std::array<T, N>> {

public:
  using Container = std::array<T, N>;

  explicit DataContainerHolder(std::size_t size, const T* data) : m_container {} {
    if (size != N && size != 0) {
      std::string msg = "Size mismatch in DataContainerHolder<std::array> specialization. ";
      msg += "Got " + std::to_string(size) + ", should be 0 or " + std::to_string(N) + ".";
      throw FitsError(msg);
    }
    if (data) {
      std::copy_n(data, size, m_container.begin());
    }
  }

  std::size_t size() const {
    return N;
  }

  inline const T* data() const {
    return m_container.data();
  }

protected:
  /**
   * @brief The data array.
   */
  std::array<T, N> m_container;
};

/**
 * @ingroup data_classes
 * @brief Mix `ContiguousContainerMixin` and `VectorArithmeticMixin` into a `DataHolder`
 * as a user-defined derived class.
 * @tparam TDerived The derived class
 * @details
 * The class can be specialized for any container which implements the `SizedData` requirements.
 * @satisfies{ContiguousContainer}
 * @satisfies{VectorArithmetic}
 */
template <typename T, typename THolder, typename TDerived> // TODO allow void
class DataContainer :
    public ContiguousContainerMixin<T, TDerived>, // TODO fallback to DataContainer<T, THolder, void>
    public VectorArithmeticMixin<T, TDerived>, // TODO fallback to DataContainer<T, THolder, void>
    public THolder {

public:
  /**
   * @brief The concrete data holder type.
   */
  using Holder = THolder;

  /**
   * @brief The concrete container type.
   */
  using Container = typename Holder::Container;

  /// @group_construction

  /**
   * @brief Construct from a size and non-constant data.
   */
  template <typename TSize = std::size_t, typename std::enable_if_t<std::is_integral<TSize>::value, TSize>* = nullptr>
  DataContainer(TSize s = 0, std::remove_const_t<T>* d = nullptr) : Holder(std::size_t(s), d) {}

  /**
   * @brief Construct from a size and constant data.
   */
  template <typename TSize = std::size_t, typename std::enable_if_t<std::is_integral<TSize>::value, TSize>* = nullptr>
  DataContainer(TSize s, const T* d) : Holder(std::size_t(s), d) {}

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainer(TIterator begin, TIterator end) : DataContainer(std::distance(begin, end), &(*begin)) {}

  /**
   * @brief Value list constructor.
   */
  DataContainer(std::initializer_list<T> values) : DataContainer(values.begin(), values.end()) {}

  /**
   * @brief Forwarding constructor.
   */
  // template <typename... TArgs>
  // DataContainer(TArgs&&... args) : Holder(std::forward<TArgs>(args)...) {}
  template <
      typename TArg0,
      typename... TArgs,
      typename std::enable_if_t<not std::is_integral<TArg0>::value, TArg0>* = nullptr>
  DataContainer(TArg0 arg0, TArgs&&... args) : Holder(std::forward<TArg0>(arg0), std::forward<TArgs>(args)...) {}

  ELEFITS_VIRTUAL_DTOR(DataContainer)
  ELEFITS_COPYABLE(DataContainer)
  ELEFITS_MOVABLE(DataContainer)

  /// @group_properties

  /**
   * @brief Inherit data holder's `size()`.
   */
  using Holder::size;

  /// @group_elements

  /**
   * @brief Inherit data holder's `data()`.
   */
  using Holder::data;

  /**
   * @copybrief data()const
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainer&>(*this).data());
  }

  /**
   * @brief Access the container in read-only mode.
   */
  const std::decay_t<Container>& container() const {
    return this->m_container;
  }

  /// @group_operations

  /**
   * @brief Copy the container values into an `std::vector`.
   * @deprecated Use more generic `container()` instead, which performs no copy.
   */
  std::vector<T> vector() const {
    return {this->begin(), this->end()};
  }

  /// @group_modifiers

  /**
   * @brief Move the container.
   * @details
   * This method is used to take ownership on the data without copying it.
   * Example usage:
   * \code
   * VecRaster<float> raster(...);
   * std::vector<float> data;
   * raster.moveTo(data);
   * // Values have been moved to data without copy.
   * // raster.vector() is in an unspecified state now.
   * \endcode
   * @warning
   * The container is not usable anymore after this call.
   */
  Container& moveTo(Container& destination) {
    destination = std::move(this->m_container);
    return destination;
  }

  /// @}
};

} // namespace Fits
} // namespace Euclid

#endif
