// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
T almost_min()
{
  return std::numeric_limits<T>::lowest() + std::numeric_limits<T>::epsilon();
}

template <>
std::complex<float> almost_min<std::complex<float>>();

template <>
std::complex<double> almost_min<std::complex<double>>();

template <>
std::string almost_min<std::string>();

template <typename T>
T almost_max()
{
  return std::numeric_limits<T>::max() - std::numeric_limits<T>::epsilon();
}

template <>
std::complex<float> almost_max<std::complex<float>>();

template <>
std::complex<double> almost_max<std::complex<double>>();

template <>
std::string almost_max<std::string>();

template <typename T>
T half_min()
{
  return std::numeric_limits<T>::lowest() / 2;
}

template <>
std::complex<float> half_min<std::complex<float>>();

template <>
std::complex<double> half_min<std::complex<double>>();

template <>
std::string half_min<std::string>();

template <typename T>
T half_max()
{
  return std::numeric_limits<T>::max() / 2;
}

template <>
bool half_max<bool>();

template <>
std::complex<float> half_max<std::complex<float>>();

template <>
std::complex<double> half_max<std::complex<double>>();

template <>
std::string half_max<std::string>();

template <typename T>
T generate_random_value(T min, T max)
{
  const auto vec = generate_random_vector<T>(1, min, max);
  return vec[0];
}

template <typename T>
std::vector<T> generate_random_vector(Linx::Index size, T min, T max)
{
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
generate_random_vector<std::complex<float>>(Linx::Index size, std::complex<float> min, std::complex<float> max);

template <>
std::vector<std::complex<double>>
generate_random_vector<std::complex<double>>(Linx::Index size, std::complex<double> min, std::complex<double> max);

template <>
std::vector<std::string> generate_random_vector<std::string>(Linx::Index size, std::string min, std::string max);

template <typename T>
bool approx(T test, T ref, ELEMENTS_UNUSED double tol)
{
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
