/**
 * @file EL_FitsData/FixtureRandom.h
 * @date 07/31/19
 * @author user
 *
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

#ifndef _EL_FITSDATA_FIXTURERANDOM_H
#define _EL_FITSDATA_FIXTURERANDOM_H

#include <algorithm>
#include <complex>
#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "ElementsKernel/Temporary.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Test-related classes and functions.
 */
namespace Test {

template<typename T>
T generate_random_value();

template<typename T>
std::vector<T> generate_random_vector(std::size_t size);

template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(std::size_t size);

template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(std::size_t size);

template<>
std::vector<std::string> generate_random_vector<std::string>(std::size_t size);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
T generate_random_value() {
  auto vec = generate_random_vector<T>(1);
  return vec[0];
}

template<typename T>
std::vector<T> generate_random_vector(std::size_t size) {
  const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  const double min = std::numeric_limits<T>::min();
  const double max = std::numeric_limits<T>::max();
  std::uniform_real_distribution<double> distribution(min, max);
  std::vector<T> vec(size);
  for(auto&& val : vec)
    val = T(distribution(generator));
  return vec;
}

template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(std::size_t size) {
  const auto re_im_vec = generate_random_vector<float>(size * 2);
  std::vector<std::complex<float>> vec(size);
  const auto im_begin = re_im_vec.begin() + size;
  std::transform(re_im_vec.begin(), im_begin, im_begin, vec.begin(),
      [](float re, float im) { return std::complex<float> {re, im}; });
  return vec;
}

template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(std::size_t size) {
  const auto re_vec = generate_random_vector<double>(size);
  const auto im_vec = generate_random_vector<double>(size);
  std::vector<std::complex<double>> vec(size);
  std::transform(re_vec.begin(), re_vec.end(), im_vec.begin(), vec.begin(),
      [](double re, double im) { return std::complex<double> {re, im}; });
  return vec;
}

template<>
std::vector<std::string> generate_random_vector<std::string>(std::size_t size) {
  std::vector<int> int_vec = generate_random_vector<int>(size);
  std::vector<std::string> vec(size);
  std::transform(int_vec.begin(), int_vec.end(), vec.begin(),
      [](int i) { return std::to_string(i); } );
  return vec;
}

}
}
}

#endif
