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
 * The functions which should be implemented in the derived class are:
 * - Default constructor;
 * - Copy and move constructors;
 * - Copy and move assignment operators;
 * - `const T* data() const`;
 * - `T* data()`;
 * - `std::size_t size() const`.
 */
template <typename T, typename TDerived>
struct ContiguousContainerBase {

  /**
   * @name Standard types definitions
   */
  /// @{

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

  /// @}
  /**
   * @name Index-based element access
   */
  /// @{

  /**
   * @brief Access the element with given index.
   */
  inline const T& operator[](size_type index) const {
    return *(static_cast<const TDerived&>(*this).data() + index);
  }

  /**
   * @copydoc operator[]
   */
  inline T& operator[](size_type index) {
    return const_cast<T&>(const_cast<const ContiguousContainerBase&>(*this)[index]);
  }

  /// @}
  /**
   * @name Iterators
   */
  /// @{

  /**
   * @brief Iterator to the first element.
   */
  const_iterator begin() const {
    return static_cast<const TDerived&>(*this).data();
  }

  /**
   * @copydoc begin()
   */
  iterator begin() {
    return const_cast<iterator>(const_cast<const ContiguousContainerBase&>(*this).begin());
  }

  /**
   * @copydoc begin()
   */
  iterator cbegin() {
    return const_cast<const ContiguousContainerBase&>(*this).begin();
  }

  /**
   * @brief Iterator to one past the last element.
   */
  const_iterator end() const {
    return begin() + static_cast<const TDerived&>(*this).size();
  }

  /**
   * @copydoc end()
   */
  iterator end() {
    return const_cast<iterator>(const_cast<const ContiguousContainerBase&>(*this).end());
  }

  /**
   * @copydoc end()
   */
  iterator cend() {
    return const_cast<const ContiguousContainerBase&>(*this).end();
  }

  /// @}
  /**
   * @name Comparison operators
   */
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
 * @brief A `ContiguousContainerBase` which implements `data()`.
 * @tparam TDerived The derived class which should implement `size()`
 */
template <typename T, typename TContainer, typename TDerived>
class DataContainerBase : public ContiguousContainerBase<T, TDerived> { // FIXME arithmetic ops

public:
  /**
   * @name Constructors
   */
  /// @{

  ELEFITS_VIRTUAL_DTOR(DataContainerBase)
  ELEFITS_COPYABLE(DataContainerBase)
  ELEFITS_MOVABLE(DataContainerBase)

  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  DataContainerBase(Ts&&... args) : m_container(std::forward<Ts>(args)...) {}

  /// @}
  /**
   * @name Raw data access
   */
  /// @{

  /**
   * @brief Access the raw data.
   */
  inline const T* data() const {
    return m_container.data();
  }

  /**
   * @copydoc data()
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainerBase&>(*this).data());
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
   * @copydoc container()
   * @deprecated Use more generic `container()` instead.
   */
  const typename std::enable_if<std::is_same<TContainer, std::vector<T>>::value, TContainer>::type& vector() const {
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
  TContainer& moveTo(TContainer& destination) const {
    destination = std::move(m_container);
    return destination;
  }

  /// @}

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
class DataContainerBase<T, T*, TDerived> : public ContiguousContainerBase<T, TDerived> {

public:
  ELEFITS_VIRTUAL_DTOR(DataContainerBase)
  ELEFITS_COPYABLE(DataContainerBase)
  ELEFITS_MOVABLE(DataContainerBase)

  /**
   * @brief Constructor.
   */
  DataContainerBase(T* data = nullptr) : m_data(data) {}

  /**
   * @name Raw data access
   */
  /// @{

  const T* data() const {
    return m_data;
  }

  /**
   * @copydoc data()
   */
  inline T* data() {
    return const_cast<T*>(const_cast<const DataContainerBase&>(*this).data());
  }

  /// @}

private:
  /**
   * @brief The data raw pointer.
   */
  T* m_data;
};

template <typename TContainer>
struct ContainerAllocator {
  static TContainer alloc(std::size_t size) {
    return TContainer(size);
  }
};

/**
 * @brief Disable allocation for non-owning `T*` container.
 */
template <typename T>
struct ContainerAllocator<T*> {
  static T* alloc(std::size_t) {
    return nullptr;
  }
};

} // namespace Fits
} // namespace Euclid

#endif
