// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestUtils.h"

namespace Euclid {
namespace Fits {
namespace Test {

template <>
std::complex<float> almost_min<std::complex<float>>()
{
  return {almost_min<float>(), almost_min<float>()};
}

template <>
std::complex<double> almost_min<std::complex<double>>()
{
  return {almost_min<double>(), almost_min<double>()};
}

template <>
std::string almost_min<std::string>()
{
  return std::to_string(almost_min<int>());
}

template <>
std::complex<float> almost_max<std::complex<float>>()
{
  return {almost_max<float>(), almost_max<float>()};
}

template <>
std::complex<double> almost_max<std::complex<double>>()
{
  return {almost_max<double>(), almost_max<double>()};
}

template <>
std::string almost_max<std::string>()
{
  return std::to_string(almost_max<int>());
}

template <>
std::complex<float> half_min<std::complex<float>>()
{
  return {half_min<float>(), half_min<float>()};
}

template <>
std::complex<double> half_min<std::complex<double>>()
{
  return {half_min<double>(), half_min<double>()};
}

template <>
std::string half_min<std::string>()
{
  return std::to_string(half_min<int>());
}

template <>
bool half_max<bool>()
{
  return true;
}

template <>
std::complex<float> half_max<std::complex<float>>()
{
  return {half_max<float>(), half_max<float>()};
}

template <>
std::complex<double> half_max<std::complex<double>>()
{
  return {half_max<double>(), half_max<double>()};
}

template <>
std::string half_max<std::string>()
{
  return std::to_string(half_max<int>());
}

template <>
std::vector<std::complex<float>>
generate_random_vector<std::complex<float>>(Linx::Index size, std::complex<float> min, std::complex<float> max)
{
  const auto re_vec = generate_random_vector<float>(size, min.real(), max.real());
  const auto im_vec = generate_random_vector<float>(size, min.imag(), max.imag());
  std::vector<std::complex<float>> vec(size);
  for (Linx::Index i = 0; i < size; ++i) {
    vec[i] = {re_vec[i], im_vec[i]};
  }
  return vec;
}

template <>
std::vector<std::complex<double>>
generate_random_vector<std::complex<double>>(Linx::Index size, std::complex<double> min, std::complex<double> max)
{
  const auto re_vec = generate_random_vector<double>(size, min.real(), max.real());
  const auto im_vec = generate_random_vector<double>(size, min.imag(), max.imag());
  std::vector<std::complex<double>> vec(size);
  for (Linx::Index i = 0; i < size; ++i) {
    vec[i] = {re_vec[i], im_vec[i]};
  }
  return vec;
}

template <>
std::vector<std::string> generate_random_vector<std::string>(Linx::Index size, std::string min, std::string max)
{
  std::vector<int> int_vec = generate_random_vector<int>(size, std::atoi(min.c_str()), std::atoi(max.c_str()));
  std::vector<std::string> vec(size);
  std::transform(int_vec.begin(), int_vec.end(), vec.begin(), [](int i) {
    return std::to_string(i);
  });
  return vec;
}

template <>
bool approx<float>(float test, float ref, double tol)
{
  return approx<double>(test, ref, tol);
}

template <>
bool approx<double>(double test, double ref, double tol)
{
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
bool approx<std::complex<float>>(std::complex<float> test, std::complex<float> ref, double tol)
{
  return approx(test.real(), ref.real(), tol) && approx(test.imag(), ref.imag(), tol);
}

template <>
bool approx<std::complex<double>>(std::complex<double> test, std::complex<double> ref, double tol)
{
  return approx(test.real(), ref.real(), tol) && approx(test.imag(), ref.imag(), tol);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
