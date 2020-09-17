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

#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Euclid {
namespace Cfitsio {

/// @cond INTERNAL

/**
 * @brief C++14 index_sequence.
 * @details To be replaced with <utility> implementation.
 * @see https://stackoverflow.com/questions/49669958
 */
namespace std14 {

template <std::size_t...>
struct index_sequence {};

template <std::size_t N, std::size_t... Next>
struct _index_sequence : public _index_sequence<N - 1U, N - 1U, Next...> {};

template <std::size_t... Next>
struct _index_sequence<0U, Next...> {
  using type = index_sequence<Next...>;
};

template <std::size_t N>
using make_index_sequence = typename _index_sequence<N>::type;

} // namespace std14

/// @endcond

template <typename T>
CStrArray::CStrArray(const T begin, const T end) : smartPtrVector(end - begin), cStrVector(end - begin) {
  for (long i = 0; i < static_cast<long>(end - begin); ++i) { // TODO iterators?
    auto &smart_ptr_i = smartPtrVector[i];
    smart_ptr_i = std::unique_ptr<char[]>(new char[(begin + i)->length() + 1]);
    std::strcpy(smart_ptr_i.get(), (begin + i)->c_str());
    cStrVector[i] = smart_ptr_i.get();
  }
}

} // namespace Cfitsio
} // namespace Euclid
