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

#include "EL_FitsData/TestUtils.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

template <>
std::complex<float> almostMin<std::complex<float>>() {
  return { almostMin<float>(), almostMin<float>() };
}

template <>
std::complex<double> almostMin<std::complex<double>>() {
  return { almostMin<double>(), almostMin<double>() };
}

template <>
std::string almostMin<std::string>() {
  return std::to_string(almostMin<int>());
}

template <>
const char *almostMin<const char *>() {
  return almostMin<std::string>().c_str();
}

template <>
std::complex<float> almostMax<std::complex<float>>() {
  return { almostMax<float>(), almostMax<float>() };
}

template <>
std::complex<double> almostMax<std::complex<double>>() {
  return { almostMax<double>(), almostMax<double>() };
}

template <>
std::string almostMax<std::string>() {
  return std::to_string(almostMax<int>());
}

template <>
const char *almostMax<const char *>() {
  return almostMax<std::string>().c_str();
}

template <>
const char *generateRandomValue<const char *>(const char *min, const char *max) {
  return generateRandomValue<std::string>(min, max).c_str();
}

template <>
std::vector<std::complex<float>>
generateRandomVector<std::complex<float>>(long size, std::complex<float> min, std::complex<float> max) {
  const auto reVec = generateRandomVector<float>(size, min.real(), max.real());
  const auto imVec = generateRandomVector<float>(size, min.imag(), max.imag());
  std::vector<std::complex<float>> vec(size);
  for (long i = 0; i < size; ++i) {
    vec[i] = { reVec[i], imVec[i] };
  }
  return vec;
}

template <>
std::vector<std::complex<double>>
generateRandomVector<std::complex<double>>(long size, std::complex<double> min, std::complex<double> max) {
  const auto reVec = generateRandomVector<double>(size, min.real(), max.real());
  const auto imVec = generateRandomVector<double>(size, min.imag(), max.imag());
  std::vector<std::complex<double>> vec(size);
  for (long i = 0; i < size; ++i) {
    vec[i] = { reVec[i], imVec[i] };
  }
  return vec;
}

template <>
std::vector<std::string> generateRandomVector<std::string>(long size, std::string min, std::string max) {
  std::vector<int> intVec = generateRandomVector<int>(size, std::atoi(min.c_str()), std::atoi(max.c_str()));
  std::vector<std::string> vec(size);
  std::transform(intVec.begin(), intVec.end(), vec.begin(), [](int i) {
    return std::to_string(i);
  });
  return vec;
}

template <>
bool approx<float>(float test, float ref, double tol) {
  return approx<double>(test, ref, tol);
}

template <>
bool approx<double>(double test, double ref, double tol) {
  if (tol == 0 || test == 0) {
    return test == ref;
  }
  const double rel = (test - ref) / ref;
  if (rel > 0 && rel > tol) {
    return false;
  }
  if (rel < 0 && -rel > tol) {
    return false;
  }
  return true;
}

template <>
bool approx<std::complex<float>>(std::complex<float> test, std::complex<float> ref, double tol) {
  return approx(test.real(), ref.real(), tol) && approx(test.imag(), ref.imag(), tol);
}

template <>
bool approx<std::complex<double>>(std::complex<double> test, std::complex<double> ref, double tol) {
  return approx(test.real(), ref.real(), tol) && approx(test.imag(), ref.imag(), tol);
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid
