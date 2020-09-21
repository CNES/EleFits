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

#ifndef _EL_FITSDATA_FITSDATAFIXTURE_H
#define _EL_FITSDATA_FITSDATAFIXTURE_H

#include <algorithm>
#include <chrono>
#include <complex>
#include <random>
#include <string>
#include <vector>

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/Column.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Test-related classes and functions.
 */
namespace Test {

/**
 * @brief Value very close to the min of the type.
 */
template <typename T>
T almostMin();

/**
 * @brief Value very close to the max of the type.
 */
template <typename T>
T almostMax();

template <typename T>
T almostMin() {
  return std::numeric_limits<T>::lowest() + std::numeric_limits<T>::epsilon();
}

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

template <typename T>
T almostMax() {
  return std::numeric_limits<T>::max() - std::numeric_limits<T>::epsilon();
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

/**
 * @brief A 2D image Raster of floats.
 */
class SmallRaster : public VecRaster<float> {

public:
  /**
   * @brief Generate a SmallRaster with given width and height.
   */
  SmallRaster(long width = 3, long height = 2);

  /** @brief Destructor. */
  virtual ~SmallRaster() = default;

  /**
   * @brief Check whether the Raster is approximately equal to another Raster.
   * @details
   * Test each pixel as: (other - this) / this < tol
   */
  bool approx(const Raster<float> &other, float tol = 0.1F) const;

  /**
   * @brief Raster width.
   */
  long width;

  /**
   * @brief Raster height.
   */
  long height;
};

/**
 * @brief A small set of columns with various types.
 */
class SmallTable {

public:
  /**
   * @brief Type of the NUM column.
   */
  using Num = int;

  /**
   * @brief Type of the RADEC column.
   */
  using Radec = std::complex<float>;

  /**
   * @brief Type of the NAME column.
   */
  using Name = std::string;

  /**
   * @brief Type of the DIST_MAG column.
   */
  using DistMag = double;

  /**
   * @brief Generate the columns.
   */
  SmallTable();

  /**
   * @brief HDU name.
   */
  std::string extname;

  /**
   * @brief Values of the NUM column.
   */
  std::vector<Num> nums;

  /**
   * @brief Values of the RADEC column.
   */
  std::vector<Radec> radecs;

  /**
   * @brief Values of the NAME column.
   */
  std::vector<Name> names;

  /**
   * @brief Values of the DIST_MAG column.
   */
  std::vector<DistMag> distsMags;

  /**
   * @brief NUM column.
   */
  VecRefColumn<Num> numCol;

  /**
   * @brief RADEC column.
   */
  VecRefColumn<Radec> radecCol;

  /**
   * @brief NAME column.
   */
  VecRefColumn<Name> nameCol;

  /**
   * @brief DIST_MAG column.
   */
  VecRefColumn<DistMag> distMagCol;
};

/**
 * @brief A random Raster of given type and shape.
 */
template <typename T, long n>
class RandomRaster : public VecRaster<T, n> {

public:
  /**
   * @brief Generate a Raster with given shape.
   */
  explicit RandomRaster(Position<n> rasterShape, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomRaster() = default;
};

/**
 * @brief A random scalar Column of given type.
 */
template <typename T>
class RandomScalarColumn : public VecColumn<T> {

public:
  /**
   * @brief Generate a Column of given size.
   */
  explicit RandomScalarColumn(long size = 3, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomScalarColumn() = default;
};

/**
 * @brief A small string column.
 */
class SmallStringColumn : public VecColumn<std::string> {

public:
  /**
   * @brief Generate a Column of given size.
   */
  SmallStringColumn(long size = 3);

  /** @brief Destructor. */
  virtual ~SmallStringColumn() = default;
};

/**
 * @brief A small vector column of given type.
 */
template <typename T>
class SmallVectorColumn : public VecColumn<std::vector<T>> {

public:
  /**
   * @brief Generate a Column.
   */
  SmallVectorColumn();

  /** @brief Destructor. */
  virtual ~SmallVectorColumn() = default;
};

/**
 * @brief Generate a random value of given type.
 */
template <typename T>
T generateRandomValue(T min = almostMin<T>(), T max = almostMax<T>());

/**
 * @brief Generate a random vector of given type and size.
 */
template <typename T>
std::vector<T> generateRandomVector(long size, T min = almostMin<T>(), T max = almostMax<T>());

/**
 * @brief Specialization of generateRandomVector for complex<float>.
 */
template <>
std::vector<std::complex<float>>
generateRandomVector<std::complex<float>>(long size, std::complex<float> min, std::complex<float> max);

/**
 * @brief Specialization of generateRandomVector for complex<double>.
 */
template <>
std::vector<std::complex<double>>
generateRandomVector<std::complex<double>>(long size, std::complex<double> min, std::complex<double> max);

/**
 * @brief Specialization of generateRandomVector for string.
 */
template <>
std::vector<std::string> generateRandomVector<std::string>(long size, std::string min, std::string max);

/////////////////////
// IMPLEMENTATION //
///////////////////

template <typename T, long n>
RandomRaster<T, n>::RandomRaster(Position<n> rasterShape, T min, T max) : VecRaster<T, n>(rasterShape) {
  this->vector() = generateRandomVector<T>(this->size(), min, max);
}

template <typename T>
RandomScalarColumn<T>::RandomScalarColumn(long size, T min, T max) :
    VecColumn<T>({ "SCALAR", "m", 1 }, generateRandomVector<T>(size, min, max)) {
}

template <>
RandomScalarColumn<std::string>::RandomScalarColumn(long size, std::string min, std::string max) :
    VecColumn<std::string>({ "SCALAR", "m", 1 }, generateRandomVector<std::string>(size, min, max)) {
  for (const auto &v : vector()) {
    long currentSize = static_cast<long>(v.length() + 1); // +1 for '\0'
    if (currentSize > info.repeat) {
      info.repeat = currentSize;
    }
  }
}

template <typename T>
SmallVectorColumn<T>::SmallVectorColumn() :
    VecColumn<std::vector<T>>({ "VECTOR", "m2", 2 }, { { T(0.), T(1.) }, { T(2.), T(3.) }, { T(4.), T(5.) } }) {
}

template <typename T>
T generateRandomValue(T min, T max) {
  auto vec = generateRandomVector<T>(1, min, max);
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
  std::transform(intVec.begin(), intVec.end(), vec.begin(), [](int i) { return std::to_string(i); });
  return vec;
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
