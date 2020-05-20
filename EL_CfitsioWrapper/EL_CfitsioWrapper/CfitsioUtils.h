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

#ifndef _EL_CFITSIOWRAPPER_CFITSIOUTILS_H
#define _EL_CFITSIOWRAPPER_CFITSIOUTILS_H

#include <cstring>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Convert a string to a unique_ptr<char>.
 * @details Used to work around non-const correctness of CFitsIO.
 */
std::unique_ptr<char[]> to_char_ptr(std::string str);

/**
 * @brief A helper structure to safely convert vector<string> to char**.
 */
struct c_str_array {
  template<typename T>
  c_str_array(const T begin, const T end);
  explicit c_str_array(const std::vector<std::string>& data);
  explicit c_str_array(const std::initializer_list<std::string>& data);
  std::vector<std::unique_ptr<char[]>> smart_ptr_vector;
  std::vector<char*> c_str_vector;
  char** data();
};

/**
 * @brief C++14 index_sequence.
 * @details To be replaced with <utility> implementation.
 * @see https://stackoverflow.com/questions/49669958
 */
namespace std14 {

template <std::size_t ...>
struct index_sequence
 { };

template <std::size_t N, std::size_t ... Next>
struct _index_sequence : public _index_sequence<N-1U, N-1U, Next...>
 { };

template <std::size_t ... Next>
struct _index_sequence<0U, Next ... >
 { using type = index_sequence<Next ... >; };

template <std::size_t N>
using make_index_sequence = typename _index_sequence<N>::type;

}


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
c_str_array::c_str_array(const T begin, const T end) :
        smart_ptr_vector(end - begin),
        c_str_vector(end - begin) {
  for(std::size_t i = 0; i < static_cast<std::size_t>(end - begin); ++i) {  //TODO iterators?
    auto& smart_ptr_i = smart_ptr_vector[i];
    smart_ptr_i = std::unique_ptr<char[]>(new char[(begin + i)->length() + 1]);
    std::strcpy(smart_ptr_i.get(), (begin + i)->c_str());
    c_str_vector[i] = smart_ptr_i.get();
  }
}

}
}

#endif
