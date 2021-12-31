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

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/FitsError.h"
#include "EleFitsData/VectorArithmetic.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_classes
 * @brief Base class for a Fits data container.
 * @tparam T The value type
 * @tparam TDerived The child class which implements required methods
 * @details
 * This class provides the necessary types and methods
 * to meet the standard `ContiguousContainer` requirements.
 * This is a CRTP implementation which takes as template parameter
 * the derived class to be empowered.
 * The functions which should be implemented in the derived class are:
 * - Default constructor;
 * - Copy and move constructors;
 * - Copy and move assignment operators;
 * - `const T* data() const` and `T* data()`;
 * - `std::size_t size() const`.
 */
template <typename T, typename TDerived>
struct ContiguousContainerMixin {

  /**
   * @brief The value type.
   */
  using value_type = T;

  /**
   * @brief The value reference.
   */
  using reference = T&;

  /**
   * @brief The constant value reference.
   */
  using const_reference = const T&;

  /**
   * @brief The value iterator.
   */
  using iterator = T*;

  /**
   * @brief The constant value iterator.
   */
  using const_iterator = const T*;

  /**
   * @brief The iterator difference type.
   */
  using difference_type = std::ptrdiff_t;

  /**
   * @brief The underlying container size type.
   */
  using size_type = std::size_t;

  /**
   * @brief Access the element with given index.
   */
  inline const T& operator[](size_type index) const {
    return *(static_cast<const TDerived&>(*this).data() + index);
  }

  /**
   * @copydoc operator[]()
   */
  inline T& operator[](size_type index) {
    return const_cast<T&>(const_cast<const ContiguousContainerMixin&>(*this)[index]);
  }

  /**
   * @brief Iterator to the first element.
   */
  const_iterator begin() const {
    return static_cast<const TDerived&>(*this).data();
  }

  /**
   * @copybrief begin()
   */
  iterator begin() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).begin());
  }

  /**
   * @copybrief begin()
   */
  iterator cbegin() {
    return const_cast<const ContiguousContainerMixin&>(*this).begin();
  }

  /**
   * @brief Iterator to one past the last element.
   */
  const_iterator end() const {
    return begin() + static_cast<const TDerived&>(*this).size();
  }

  /**
   * @copybrief end()
   */
  iterator end() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).end());
  }

  /**
   * @copybrief end()
   */
  iterator cend() {
    return const_cast<const ContiguousContainerMixin&>(*this).end();
  }

  /**
   * @brief Check equality.
   */
  virtual bool operator==(const TDerived& rhs) const {
    return (static_cast<const TDerived&>(*this).size() == rhs.size() && std::equal(begin(), end(), rhs.begin()));
  }

  /**
   * @brief Check inequality.
   */
  bool operator!=(const TDerived& rhs) const {
    return not(*this == rhs);
  }

  /**
   * @brief Check whether the container is empty.
   * @details
   * Empty corresponds to `begin() == end()`.
   */
  bool emtpy() const {
    return begin() == end();
  }
};

/**
 * @ingroup data_classes
 * @brief A `ContiguousContainerMixin` which implements `data()`, `size()` and arithmetic operators.
 * @tparam TDerived The derived class
 * @details
 * The class can be specialized for any container, in which case it should provide the following methods:
 * - Default, copy and move constructors;
 * - Constructor from a size;
 * - Constructor from an iterator pair;
 * - Non-explicit constructor from an initialization list;
 * - `const T* data() const` and `T* data()`;
 * - `size_type size() const`.
 */
template <typename T, typename TContainer, typename TDerived>
class DataContainer : public ContiguousContainerMixin<T, TDerived>, public VectorArithmeticMixin<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainer)
  ELEFITS_COPYABLE(DataContainer)
  ELEFITS_MOVABLE(DataContainer)

  /**
   * @brief Default and size-based constructor.
   */
  explicit DataContainer(std::size_t size = 0) : m_container(size) {}

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainer(TIterator begin, TIterator end) : m_container(begin, end) {}

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainer(std::initializer_list<T> values) : m_container(values) {}

  /**
   * @brief Forwarding constructor.
   */
  template <typename... Ts>
  DataContainer(Ts&&... args) : m_container(std::forward<Ts>(args)...) {}

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_container.data();
  }

  /**
   * @copybrief data()
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainer&>(*this).data());
  }

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return m_container.size();
  }

  /**
   * @brief Access the container in read-only mode.
   */
  const TContainer& container() const {
    return m_container;
  }

  /**
   * @brief Copy the container values into an `std::vector`.
   * @deprecated Use more generic `container()` instead, which performs no copy.
   */
  std::vector<T> vector() const {
    return {this->begin(), this->end()};
  }

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
    destination = std::move(m_container);
    return destination;
  }

private:
  /**
   * @brief The data container.
   */
  TContainer m_container;
};

/**
 * @brief Raw pointer specialization.
 */
template <typename T, typename TDerived>
class DataContainer<T, T*, TDerived> :
    public ContiguousContainerMixin<T, TDerived>,
    public VectorArithmeticMixin<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainer)
  ELEFITS_COPYABLE(DataContainer)
  ELEFITS_MOVABLE(DataContainer)

  /**
   * @brief Default and size-based constructor.
   */
  explicit DataContainer(std::size_t size = 0, T* data = nullptr) : m_size(size), m_data(data) {}

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainer(TIterator begin, TIterator end) : m_size(std::distance(begin, end)), m_data(&(*begin)) {}

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainer(std::initializer_list<T> values) : DataContainer(values.begin(), values.end()) {}

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_data;
  }

  /**
   * @copybrief data() const
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainer&>(*this).data());
  }

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return m_size;
  }

private:
  /**
   * @brief The number of elements.
   */
  std::size_t m_size;

  /**
   * @brief The data raw pointer.
   */
  T* m_data;
};

/**
 * @brief `std::array` specialization.
 */
template <typename T, typename TDerived, std::size_t N>
class DataContainer<T, std::array<T, N>, TDerived> :
    public ContiguousContainerMixin<T, TDerived>,
    public VectorArithmeticMixin<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainer)
  ELEFITS_COPYABLE(DataContainer)
  ELEFITS_MOVABLE(DataContainer)

  /**
   * @brief Default and size-based constructor.
   */
  explicit DataContainer(std::size_t size = N) : m_container {} {
    // FIXME assert size == N
    if (size != N) {
      throw FitsError("Size missmatch in DataContainer<std::array> specialization."); // FIXME clarify
    }
  }

  /**
   * @brief Iterator-based constructor.
   */
  template <typename TIterator>
  DataContainer(TIterator begin, TIterator end) : DataContainer(std::distance(begin, end)) {
    std::copy(begin, end, m_container.begin());
  }

  /**
   * @brief Initialization list-based constructor.
   */
  DataContainer(std::initializer_list<T> values) : DataContainer(values.begin(), values.end()) {}

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_container.data();
  }

  /**
   * @copybrief data() const
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainer&>(*this).data());
  }

  /**
   * @brief Get the number of elements.
   */
  std::size_t size() const {
    return N;
  }

  /**
   * @brief Access the container in read-only mode.
   */
  const std::array<T, N>& container() const {
    return m_container;
  }

private:
  /**
   * @brief The data array.
   */
  std::array<T, N> m_container;
}; // FIXME avoid duplication

template <typename T, typename TDerived>
std::ostream& operator<<(std::ostream& os, const ContiguousContainerMixin<T, TDerived>& container) {
  os << "[";
  if (not container.emtpy()) {
    const auto size = static_cast<const TDerived&>(container).size();
    std::size_t i = 0;
    os << container[i];
    if (size > 7) {
      for (++i; i < 3; ++i) {
        os << ", " << container[i];
      }
      i = size - 3;
      os << " ... " << container[i];
    }
    for (++i; i < size; ++i) {
      os << ", " << container[i];
    }
  }
  os << "]";
  return os;
}

} // namespace Fits
} // namespace Euclid

#endif
