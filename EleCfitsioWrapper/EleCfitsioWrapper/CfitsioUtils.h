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

#ifndef _ELECFITSIOWRAPPER_CFITSIOUTILS_H
#define _ELECFITSIOWRAPPER_CFITSIOUTILS_H

#include <cstring>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Convert a string to a `unique_ptr<char[]>`.
 * @details
 * Used to work around non-const correctness of CFitsIO:
 * when a function expects a `char *` instead of a `const char *`,
 * `string::c_str()` cannot be used.
 * To call some function `f(char *)` with a string `s`, do:
 * \code
 * f(toCharPtr(s).get());
 * \endcode
 */
std::unique_ptr<char[]> toCharPtr(const std::string& str);

/**
 * @brief A helper structure to safely convert `vector<string>` to `char **`.
 * @details
 * To call some function `f(char **)` with a vector of strings `v`, do:
 * \code
 * CStrArray a(v);
 * f(a.data());
 * \endcode
 * @warning
 * The CStrArray should not be destroyed before the user function ends.
 */
struct CStrArray {

  /**
   * @brief Create from begin and end iterators.
   */
  template <typename T>
  CStrArray(const T begin, const T end);

  /**
   * @brief Create from a vector.
   */
  explicit CStrArray(const std::vector<std::string>& data);

  /**
   * @brief Create from an initializer_list.
   */
  explicit CStrArray(const std::initializer_list<std::string>& data);

  /**
   * @brief The number of elements.
   */
  std::size_t size() const;

  /**
   * @brief A vector of smart pointers to `char[]`.
   * @warning
   * Modification makes CStrArray object invalid.
   */
  std::vector<std::unique_ptr<char[]>> smartPtrVector;

  /**
   * @brief A vector of `char*`.
   * @warning
   * Modification makes CStrArray object invalid.
   */
  std::vector<char*> cStrVector;

  /**
   * @brief Get the data as a non-const `char **`.
   */
  char** data();
};

} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_CFITSIOUTILS_IMPL
#include "EleCfitsioWrapper/impl/CfitsioUtils.hpp"
#undef _ELECFITSIOWRAPPER_CFITSIOUTILS_IMPL
/// @endcond

#endif
