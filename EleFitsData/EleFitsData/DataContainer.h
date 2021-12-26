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

#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @brief Base class for a Fits data container.
 * @tparam T The value type
 * @tparam TDerived The child class which implements required methods
 * @details
 * This class provides the necessary types and methods
 * to meet the standard `ContiguousContainer` requirements.
 * This is a CRTP implementation which takes as template parameter
 * the derived class to be empowered.
 * The only functions which should be implemented in the derived class are:
 * - `const T* data() const`, and
 * - `std::size_t size() const`.
 */
template <typename T, typename TDerived>
struct ContiguousContainerBase {

  /**
   * @name Standard types definitions
   */
  /// @{

  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = const T*;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  /// @}
  /**
   * @name Raw data access
   */
  /// @{

  inline T* data() {
    return const_cast<T*>(const_cast<const TDerived&>(*this).data());
  }

  /// @}
  /**
   * @name Index-based element access
   */
  /// @{

  inline const T& operator[](size_type index) const {
    return *(static_cast<const TDerived&>(*this).data() + index);
  }

  inline T& operator[](size_type index) {
    return const_cast<T&>(const_cast<const ContiguousContainerBase&>(*this)[index]);
  }

  /// @}
  /**
   * @brief Iterators
   */
  /// @{

  const_iterator begin() const {
    return static_cast<const TDerived&>(*this).data();
  }

  iterator begin() {
    return const_cast<iterator>(const_cast<const ContiguousContainerBase&>(*this).begin());
  }

  iterator cbegin() {
    return const_cast<const ContiguousContainerBase&>(*this).begin();
  }

  const_iterator end() const {
    return begin() + static_cast<const TDerived&>(*this).size();
  }

  iterator end() {
    return const_cast<iterator>(const_cast<const ContiguousContainerBase&>(*this).end());
  }

  iterator cend() {
    return const_cast<const ContiguousContainerBase&>(*this).end();
  }

  /// @}
  /**
   * @name Comparison operators
   */
  /// @{

  virtual bool operator==(const TDerived& rhs) const {
    return (static_cast<const TDerived&>(*this).size() == rhs.size() && std::equal(begin(), end(), rhs.begin()));
  }

  bool operator!=(const TDerived& rhs) const {
    return not(*this == rhs);
  }

  bool emtpy() const {
    return begin() == end();
  }

  /// @}
};

/**
 * @brief A `ContiguousContainerBase` which implements `data()`.
 * @tparam TDerived The derived class which should implement `size()`
 */
template <typename T, typename TContainer, typename TDerived>
class DataContainerBase : public ContiguousContainerBase<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainerBase)
  ELEFITS_COPYABLE(DataContainerBase)
  ELEFITS_MOVABLE(DataContainerBase)

  template <typename... Ts>
  DataContainerBase(Ts&&... args) : m_container(std::forward<Ts>(args)...) {}

  DataContainerBase(TContainer&& values) : m_container(std::forward<TDerived>(values)) {}

  /**
   * @name Raw data access
   */
  /// @{

  inline const T* data() const {
    std::cout << "Generic\n";
    return m_container.data();
  }

  /// @}
  /**
   * @name Container access
   */
  /// @{

  /**
   * @brief Access the container in read-only mode.
   */
  const TContainer& container() const {
    return m_container;
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
  TContainer& moveTo(TContainer& target) const {
    return target = std::move(m_container);
  }

  /// @}

private:
  TContainer m_container;
};

template <typename T, typename TDerived>
class DataContainerBase<T, T*, TDerived> : public ContiguousContainerBase<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainerBase)
  ELEFITS_NON_COPYABLE(DataContainerBase)
  ELEFITS_MOVABLE(DataContainerBase)

  DataContainerBase(T* data = nullptr) : m_data(data) {}

  /**
   * @name Raw data access
   */
  /// @{

  const T* data() const {
    std::cout << "Ptr\n";
    return m_data;
  }

  /// @}

private:
  T* m_data;
};

} // namespace Fits

} // namespace Euclid

#endif
