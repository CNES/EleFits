/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _ELEFITSDATA_SEGMENT_H
#define _ELEFITSDATA_SEGMENT_H

namespace Euclid {
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
  static Segment fromSize(long front, long size) {
    return {front, front + size - 1};
  }

  /**
   * @brief Create a maximal segment (from index 0 to max).
   */
  static Segment whole() {
    return {0, -1};
  }

  /**
   * @brief The lower bound.
   */
  long front;

  /**
   * @brief The upper bound.
   */
  long back;

  /**
   * @brief Get the number of elements.
   */
  long size() const {
    return back - front + 1;
  }
};

/**
 * @relates Segment
 * @brief Check whether two segments are equal.
 */
inline bool operator==(const Segment& lhs, const Segment& rhs) {
  return lhs.front == rhs.front && lhs.back == rhs.back;
}

/**
 * @relates Segment
 * @brief Check whether two segments are different.
 */
inline bool operator!=(const Segment& lhs, const Segment& rhs) {
  return lhs.front != rhs.front || lhs.back != rhs.back;
}

/**
 * @relates Segment
 * @brief Add a scalar.
 */
inline Segment& operator+=(Segment& lhs, long rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Segment
 * @brief Subtract a scalar.
 */
inline Segment& operator-=(Segment& lhs, long rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Segment
 * @brief Add 1.
 */
inline Segment& operator++(Segment& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @relates Segment
 * @brief Subtract 1.
 */
inline Segment& operator--(Segment& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @relates Segment
 * @brief Return the current segment and then add 1.
 */
inline Segment operator++(Segment& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @relates Segment
 * @brief Return the current segment and then subtract 1.
 */
inline Segment operator--(Segment& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @relates Segment
 * @brief Identity.
 */
inline Segment operator+(const Segment& rhs) {
  return rhs;
}

/**
 * @relates Segment
 * @brief Change the sign of each coordinate.
 */
inline Segment operator-(const Segment& rhs) {
  return {-rhs.front, -rhs.back};
}

/**
 * @relates Segment
 * @brief Add a segment and a scalar.
 */
inline Segment operator+(const Segment& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Segment
 * @brief Subtract a segment and a scalar.
 */
inline Segment operator-(const Segment& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

} // namespace Fits
} // namespace Euclid

#endif // _ELEFITSDATA_SEGMENT_H
