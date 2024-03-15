// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_SEGMENT_H
#define _ELEFITSDATA_SEGMENT_H

#include "Linx/Base/TypeUtils.h" // Index

namespace Fits {

/**
 * @ingroup bintable_data_classes
 * @brief Bounds of a closed index interval.
 * @details
 * The lower and upper bounds are named `front` and `back`
 * to match `Segment` wording.
 * This also avoids confusion when working with table segments,
 * where the lower bound is upward the upper bound.
 */
struct Segment {
  /**
   * @brief Create a segment specified by a lower bound and size.
   */
  static Segment fromSize(Linx::Index front, Linx::Index size)
  {
    return {front, front + size - 1};
  }

  /**
   * @brief Create a maximal segment (from index 0 to max).
   */
  static Segment whole()
  {
    return {0, -1};
  }

  /**
   * @brief The lower bound.
   */
  Linx::Index front;

  /**
   * @brief The upper bound.
   */
  Linx::Index back;

  /**
   * @brief Get the number of elements.
   */
  Linx::Index size() const
  {
    return back - front + 1;
  }
};

/**
 * @relates Segment
 * @brief Check whether two segments are equal.
 */
inline bool operator==(const Segment& lhs, const Segment& rhs)
{
  return lhs.front == rhs.front && lhs.back == rhs.back;
}

/**
 * @relates Segment
 * @brief Check whether two segments are different.
 */
inline bool operator!=(const Segment& lhs, const Segment& rhs)
{
  return lhs.front != rhs.front || lhs.back != rhs.back;
}

/**
 * @relates Segment
 * @brief Add a scalar.
 */
inline Segment& operator+=(Segment& lhs, Linx::Index rhs)
{
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Segment
 * @brief Subtract a scalar.
 */
inline Segment& operator-=(Segment& lhs, Linx::Index rhs)
{
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Segment
 * @brief Add 1.
 */
inline Segment& operator++(Segment& lhs)
{
  lhs += 1;
  return lhs;
}

/**
 * @relates Segment
 * @brief Subtract 1.
 */
inline Segment& operator--(Segment& lhs)
{
  lhs -= 1;
  return lhs;
}

/**
 * @relates Segment
 * @brief Return the current segment and then add 1.
 */
inline Segment operator++(Segment& lhs, int)
{
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @relates Segment
 * @brief Return the current segment and then subtract 1.
 */
inline Segment operator--(Segment& lhs, int)
{
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @relates Segment
 * @brief Identity.
 */
inline Segment operator+(const Segment& rhs)
{
  return rhs;
}

/**
 * @relates Segment
 * @brief Change the sign of each coordinate.
 */
inline Segment operator-(const Segment& rhs)
{
  return {-rhs.front, -rhs.back};
}

/**
 * @relates Segment
 * @brief Add a segment and a scalar.
 */
inline Segment operator+(const Segment& lhs, Linx::Index rhs)
{
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Segment
 * @brief Subtract a segment and a scalar.
 */
inline Segment operator-(const Segment& lhs, Linx::Index rhs)
{
  auto res = lhs;
  res -= rhs;
  return res;
}

} // namespace Fits

#endif // _ELEFITSDATA_SEGMENT_H
