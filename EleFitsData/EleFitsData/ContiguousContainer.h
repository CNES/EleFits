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

#ifndef _ELEFITSDATA_CONTIGUOUSCONTAINER_H
#define _ELEFITSDATA_CONTIGUOUSCONTAINER_H

#include <algorithm> // equal
#include <ostream>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_concepts
 * @requirements{ContiguousContainer}
 * @brief Standard contiguous container requirements
 * @details
 * A <a href="https://en.cppreference.com/w/cpp/named_req/ContiguousContainer">contiguous container</a>
 * is a standard container whose elements are stored contiguously in memory.
 */

/**
 * @ingroup data_concepts
 * @brief Base class for a Fits data container.
 * @tparam T The value type
 * @tparam TDerived The child class which implements required methods
 * @details
 * This class provides the necessary types and methods
 * to meet the standard `ContiguousContainer` requirements.
 * This is a CRTP implementation, which means it takes as template parameter
 * the derived class to be empowered.
 * 
 * The derived class must satisfy the `SizedData` requirements.
 * @satisfies{ContiguousContainer}
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

  /// @{
  /**
   * @brief Access the element with given index.
   */
  inline const T& operator[](size_type index) const {
    return *(static_cast<const TDerived&>(*this).data() + index);
  }
  inline T& operator[](size_type index) {
    return const_cast<T&>(const_cast<const ContiguousContainerMixin&>(*this)[index]);
  }
  /// @}

  /// @{
  /**
   * @brief Iterator to the first element.
   */

  const_iterator begin() const {
    return static_cast<const TDerived&>(*this).data();
  }
  iterator begin() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).begin());
  }
  iterator cbegin() {
    return const_cast<const ContiguousContainerMixin&>(*this).begin();
  }

  /// @}

  /// @{
  /**
   * @brief Iterator to one past the last element.
   */

  const_iterator end() const {
    return begin() + static_cast<const TDerived&>(*this).size();
  }

  iterator end() {
    return const_cast<iterator>(const_cast<const ContiguousContainerMixin&>(*this).end());
  }

  iterator cend() {
    return const_cast<const ContiguousContainerMixin&>(*this).end();
  }

  /// @}

  /// @{
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

  /// @}

  /// @{
  /**
   * @brief Check whether the container is empty.
   * @details
   * Empty corresponds to `begin() == end()`.
   */

  bool emtpy() const {
    return begin() == end();
  }

  /// @}
};

/**
 * @ingroup data_concepts
 * @brief Insert a `ContiguousContainerMixin` into an output stream.
 * @relates ContiguousContainerMixin
 */
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
