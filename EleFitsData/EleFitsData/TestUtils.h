// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_TESTUTILS_H
#define _ELEFITSDATA_TESTUTILS_H

#include "Linx/Base/TypeUtils.h" // Index

#include <complex>
#include <string>
#include <vector>

namespace Fits {

/**
 * @brief Test-related classes and functions.
 */
namespace Test {

/**
 * @brief Value very close to the min of the type.
 */
template <typename T>
T almost_min();

/**
 * @brief Value very close to the max of the type.
 */
template <typename T>
T almost_max();

/**
 * @brief Half the min of the type.
 */
template <typename T>
T half_min();

/**
 * @brief Half the max of the type.
 */
template <typename T>
T half_max();

/**
 * @brief Generate a random value of given type.
 */
template <typename T>
T generate_random_value(T min = half_min<T>(), T max = half_max<T>());

/**
 * @brief Generate a random vector of given type and size.
 */
template <typename T>
std::vector<T> generate_random_vector(Linx::Index size, T min = half_min<T>(), T max = half_max<T>());

/**
 * @brief Check whether a test value is approximately equal to a reference value.
 * @details
 * Floating point values are compared as: |test - ref| / ref < tol
 * Complex values are tested component-wise.
 * Other types are tested for equality.
 */
template <typename T>
bool approx(T test, T ref, double tol = 0.01);

} // namespace Test
} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_TESTUTILS_IMPL
#include "EleFitsData/impl/TestUtils.hpp"
#undef _ELEFITSDATA_TESTUTILS_IMPL
/// @endcond

#endif
