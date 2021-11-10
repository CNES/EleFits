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

#if defined(_ELEFITSDATA_TESTUTILS_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/TestUtils.h"
#include "ElementsKernel/Unused.h"

#include <algorithm> // generate
#include <chrono> // chrono
#include <random> // default_random_engine, uniform_real_distribution

namespace Euclid {
namespace Fits {
namespace Test {

template <typename T>
T almostMin() {
  return std::numeric_limits<T>::lowest() + std::numeric_limits<T>::epsilon();
}

template <>
std::complex<float> almostMin<std::complex<float>>();

template <>
std::complex<double> almostMin<std::complex<double>>();

template <>
std::string almostMin<std::string>();

template <typename T>
T almostMax() {
  return std::numeric_limits<T>::max() - std::numeric_limits<T>::epsilon();
}

template <>
std::complex<float> almostMax<std::complex<float>>();

template <>
std::complex<double> almostMax<std::complex<double>>();

template <>
std::string almostMax<std::string>();

template <typename T>
T halfMin() {
  return std::numeric_limits<T>::lowest() / 2;
}

template <>
std::complex<float> halfMin<std::complex<float>>();

template <>
std::complex<double> halfMin<std::complex<double>>();

template <>
std::string halfMin<std::string>();

template <typename T>
T halfMax() {
  return std::numeric_limits<T>::max() / 2;
}

template <>
bool halfMax<bool>();

template <>
std::complex<float> halfMax<std::complex<float>>();

template <>
std::complex<double> halfMax<std::complex<double>>();

template <>
std::string halfMax<std::string>();

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
  std::generate(vec.begin(), vec.end(), [&]() {
    return T(distribution(generator));
  });
  return vec;
}

template <>
std::vector<std::complex<float>>
generateRandomVector<std::complex<float>>(long size, std::complex<float> min, std::complex<float> max);

template <>
std::vector<std::complex<double>>
generateRandomVector<std::complex<double>>(long size, std::complex<double> min, std::complex<double> max);

template <>
std::vector<std::string> generateRandomVector<std::string>(long size, std::string min, std::string max);

template <typename T>
bool approx(T test, T ref, ELEMENTS_UNUSED double tol) {
  return test == ref;
}

template <>
bool approx<float>(float test, float ref, double tol);

template <>
bool approx<double>(double test, double ref, double tol);

template <>
bool approx<std::complex<float>>(std::complex<float> test, std::complex<float> ref, double tol);

template <>
bool approx<std::complex<double>>(std::complex<double> test, std::complex<double> ref, double tol);

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
