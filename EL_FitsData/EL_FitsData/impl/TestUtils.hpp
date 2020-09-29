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

#ifdef _EL_FITSDATA_TESTUTILS_IMPL

#include <algorithm>
#include <chrono>
#include <random>

#include "EL_FitsData/TestUtils.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

template <typename T>
T almostMin() {
  return std::numeric_limits<T>::lowest() + std::numeric_limits<T>::epsilon();
}

template <typename T>
T almostMax() {
  return std::numeric_limits<T>::max() - std::numeric_limits<T>::epsilon();
}

template <typename T>
T generateRandomValue(T min, T max) {
  const auto vec = generateRandomVector<T>(1, min, max);
  return vec[0];
}

template <typename T>
std::vector<T> generateRandomVector(long size, T min, T max) {
  const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<long double> distribution(
      static_cast<long double>(min),
      static_cast<long double>(max));
  std::vector<T> vec(size);
  std::generate(vec.begin(), vec.end(), [&]() { return T(distribution(generator)); });
  return vec;
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif