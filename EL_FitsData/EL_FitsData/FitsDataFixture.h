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
 * @brief A 2D image Raster of floats.
 */
class SmallRaster : public VecRaster<float> {

public:

  /**
   * @brief Generate a SmallRaster with given width and height.
   */
  SmallRaster(long width=3, long height=2);
  
  /** @brief Destructor. */
  virtual ~SmallRaster() = default;
  
  /**
   * @brief Check whether the Raster is approximately equal to another Raster.
   * @details
   * Test each pixel as: (other - this) / this < tol
   */
  bool approx(const Raster<float>& other, float tol=0.1) const;
  
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
  using num_t = int;

  /**
   * @brief Type of the RADEC column.
   */
  using radec_t = std::complex<float>;

  /**
   * @brief Type of the NAME column.
   */
  using name_t = std::string;

  /**
   * @brief Type of the DIST_MAG column.
   */
  using dist_mag_t = double;

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
  std::vector<num_t> nums;

  /**
   * @brief Values of the RADEC column.
   */
  std::vector<radec_t> radecs;

  /**
   * @brief Values of the NAME column.
   */
  std::vector<name_t> names;

  /**
   * @brief Values of the DIST_MAG column.
   */
  std::vector<dist_mag_t> dists_mags;
  
  /**
   * @brief NUM column.
   */
  VecRefColumn<num_t> num_col;

  /**
   * @brief RADEC column.
   */
  VecRefColumn<radec_t> radec_col;

  /**
   * @brief NAME column.
   */
  VecRefColumn<name_t> name_col;

  /**
   * @brief DIST_MAG column.
   */
  VecRefColumn<dist_mag_t> dist_mag_col;

};

/**
 * @brief A random Raster of given type and shape.
 */
template<typename T, long n>
class RandomRaster : public VecRaster<T, n> {

public:

  /**
   * @brief Generate a Raster with given shape.
   */
  explicit RandomRaster(pos_type<n> input_shape);

  /** @brief Destructor. */
  virtual ~RandomRaster() = default;

};


/**
 * @brief A random scalar Column of given type.
 */
template<typename T>
class RandomScalarColumn : public VecColumn<T> {

public:

  /**
   * @brief Generate a Column of given size.
   */
  explicit RandomScalarColumn(long size=3);

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
  SmallStringColumn(long size=3);
  
  /** @brief Destructor. */
  virtual ~SmallStringColumn() = default;

};

/**
 * @brief A small vector column of given type.
 */
template<typename T>
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
template<typename T>
T generate_random_value();

/**
 * @brief Generate a random vector of given type and size.
 */
template<typename T>
std::vector<T> generate_random_vector(long size);

/**
 * @brief Specialization of generate_random_vector for complex<float>.
 */
template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(long size);

/**
 * @brief Specialization of generate_random_vector for complex<double>.
 */
template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(long size);

/**
 * @brief Specialization of generate_random_vector for string.
 */
template<>
std::vector<std::string> generate_random_vector<std::string>(long size);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, long n>
RandomRaster<T, n>::RandomRaster(pos_type<n> input_shape) :
    VecRaster<T, n>(input_shape) {
  this->vector() = generate_random_vector<T>(this->size());
}


template<typename T>
RandomScalarColumn<T>::RandomScalarColumn(long size) :
    VecColumn<T>({"SCALAR", "m", 1}, generate_random_vector<T>(size)) {
}

template<>
RandomScalarColumn<std::string>::RandomScalarColumn(long size) :
    VecColumn<std::string>({ "SCALAR", "m", 1}, generate_random_vector<std::string>(size)) {
  for(const auto& v : vector()) {
    long current_size = static_cast<long>(v.length() + 1); // +1 for '\0'
    if(current_size > info.repeat)
      info.repeat = current_size;
  }
}

template<typename T>
SmallVectorColumn<T>::SmallVectorColumn() :
    VecColumn<std::vector<T>>(
        {"VECTOR", "m2", 2},
        { { T(0.), T(1.) }, { T(2.), T(3.) }, { T(4.), T(5.) } }
    ) {
}

template<typename T>
T generate_random_value() {
  auto vec = generate_random_vector<T>(1);
  return vec[0];
}

template<typename T>
std::vector<T> generate_random_vector(long size) {
  const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  const double min = std::numeric_limits<T>::min();
  const double max = std::numeric_limits<T>::max();
  std::uniform_real_distribution<double> distribution(min, max);
  std::vector<T> vec(size);
  std::generate(vec.begin(), vec.end(),
      [&]() { return T(distribution(generator)); });
  return vec;
}

template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(long size) {
  const auto re_im_vec = generate_random_vector<float>(size * 2);
  std::vector<std::complex<float>> vec(size);
  const auto im_begin = re_im_vec.begin() + size;
  std::transform(re_im_vec.begin(), im_begin, im_begin, vec.begin(),
      [](float re, float im) { return std::complex<float> {re, im}; });
  return vec;
}

template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(long size) {
  const auto re_vec = generate_random_vector<double>(size);
  const auto im_vec = generate_random_vector<double>(size);
  std::vector<std::complex<double>> vec(size);
  std::transform(re_vec.begin(), re_vec.end(), im_vec.begin(), vec.begin(),
      [](double re, double im) { return std::complex<double> {re, im}; });
  return vec;
}

template<>
std::vector<std::string> generate_random_vector<std::string>(long size) {
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
