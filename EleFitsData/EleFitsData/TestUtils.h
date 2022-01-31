// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_TESTUTILS_H
#define _ELEFITSDATA_TESTUTILS_H

#include <complex>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

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

/**
 * @brief Half the min of the type.
 */
template <typename T>
T halfMin();

/**
 * @brief Half the max of the type.
 */
template <typename T>
T halfMax();

/**
 * @brief Generate a random value of given type.
 */
template <typename T>
T generateRandomValue(T min = halfMin<T>(), T max = halfMax<T>());

/**
 * @brief Generate a random vector of given type and size.
 */
template <typename T>
std::vector<T> generateRandomVector(long size, T min = halfMin<T>(), T max = halfMax<T>());

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
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_TESTUTILS_IMPL
#include "EleFitsData/impl/TestUtils.hpp"
#undef _ELEFITSDATA_TESTUTILS_IMPL
/// @endcond

#endif
