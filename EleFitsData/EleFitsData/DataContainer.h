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

#ifndef _ELEFITSDATA_DATACONTAINER_H
#define _ELEFITSDATA_DATACONTAINER_H

#include "EleFitsData/ContiguousContainer.h"
#include "EleFitsData/DataUtils.h"
#include "EleFitsData/FitsError.h"
#include "EleFitsData/VectorArithmetic.h"

#include <array>
#include <cstddef> // size_t
#include <initializer_list>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_concepts
 * @requirements{SizedData}
 * @brief Requirements for a `DataContainerHolder`.
 * @details
 * A contiguous data holder is some class which stores or points to
 * some data contiguous in memory as a public or protected member `TContainer m_container`,
 * and implements the following methods:
 * - Default, copy and move constructors;
 * - Constructor from a size;
 * - Constructor from an iterator pair;
 * - Implicit constructor from an initialization list;
 * - `size_type size() const`;
 * - `inline const T* data() const`.
 */
// FIXME ctot(size = 0, data = nullptr) should suffice

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
  /// @group_construction

  /**
   * @brief Default or size-based constructor.
   */
  explicit DataContainerHolder(std::size_t size = 0) : m_container(size) {}

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainerHolder(TIterator begin, TIterator end) : m_container(begin, end) {}

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainerHolder(std::initializer_list<T> values) : DataContainerHolder(values.begin(), values.end()) {}

  /**
   * @brief Forwarding constructor.
   */
  template <typename... Ts>
  DataContainerHolder(Ts&&... args) : m_container(std::forward<Ts>(args)...) {}

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
  TContainer m_container;
};

/**
 * @ingroup data_concepts
 * @brief Raw pointer specialization.
 */
template <typename T>
class DataContainerHolder<T, T*> {

public:
  /// @group_construction

  /**
   * @brief Default or size-based constructor.
   */
  explicit DataContainerHolder(std::size_t size = 0, T* data = nullptr) : m_size(size), m_container(data) {}

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainerHolder(TIterator begin, TIterator end) : DataContainerHolder(std::distance(begin, end), &(*begin)) {}

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainerHolder(std::initializer_list<T> values) : DataContainerHolder(values.begin(), values.end()) {}

  /// @group_properties

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return m_size;
  }

  /// @group_elements

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_container;
  }

  /// @}

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
  /// @group_construction

  /**
   * @brief Default or size-based constructor.
   */
  explicit DataContainerHolder(std::size_t size = N) : m_container {} {
    if (size != N) {
      throw FitsError("Size missmatch in DataContainerHolder<std::array> specialization."); // FIXME clarify
    }
  }

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainerHolder(TIterator begin, TIterator end) : DataContainerHolder(std::distance(begin, end)) {
    std::copy(begin, end, m_container.begin());
  }

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainerHolder(std::initializer_list<T> values) : DataContainerHolder(values.begin(), values.end()) {}

  /// @group_properties

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return N;
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
template <typename T, typename TContainer, typename TDerived> // TODO allow void
class DataContainer :
    public ContiguousContainerMixin<T, TDerived>, // TODO fallback to DataContainer<T, TContainer, void>
    public VectorArithmeticMixin<T, TDerived>, // TODO fallback to DataContainer<T, TContainer, void>
    private DataContainerHolder<T, TContainer> {

public:
  /**
   * @brief The concrete data holder type.
   */
  using Holder = DataContainerHolder<T, TContainer>;

  /// @group_construction

  /**
   * @brief Inherit data holder's constructors.
   */
  using Holder::Holder;

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
  const std::decay_t<TContainer>& container() const {
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
  TContainer& moveTo(TContainer& destination) {
    destination = std::move(this->m_container);
    return destination;
  }

  /// @}
};

} // namespace Fits
} // namespace Euclid

#endif
