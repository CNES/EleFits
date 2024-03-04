// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_SCALING_H
#define _ELEFITSDATA_SCALING_H

namespace Euclid {
namespace Fits {

/**
 * @brief Helper class for scaling parameters.
 * 
 * A scaling can be:
 * - Disabled (null value);
 * - Provided as an absolute value;
 * - Provided as a relative value, either as a multiplicative factor or its inverse.
 */
class Scaling {
public:

  /**
   * @brief The type of scaling value.
   */
  enum class Type {
    Absolute, ///< Absolute
    Factor, ///< Relative as factor: absolute = reference * value
    Inverse ///< Relative as inverse: absolute = reference / value
  };

  /**
   * @brief Default constructor: make identity.
   */
  inline Scaling();

  /**
   * @brief Constructor.
   */
  inline Scaling(double value, Type type = Type::Absolute);
  // Purposedly not explicit to support implicit cast from double

  /**
   * @brief Check whether the scaling is enabled (i.e. the value is not null).
   */
  inline explicit operator bool() const;

  /**
   * @brief Check whether the scaling is identity (factor or inverse with value 1).
  */
  inline bool is_identity() const;

  /**
   * @brief Get the scaling type.
   */
  inline Type type() const;

  /**
   * @brief Get the scaling value.
   */
  inline double value() const;

  /**
   * @brief Check whether two parameters are equal.
   */
  inline bool operator==(const Scaling& rhs) const;

  /**
   * @brief Check whether two parameters are different.
   */
  inline bool operator!=(const Scaling& rhs) const;

  /**
   * @brief Multiply the scaling by a given factor.
   * 
   * If the scaling is inverse, the value is divided by the factor.
   */
  inline Scaling& operator*=(double value);

  /**
   * @brief Divide the scaling by a given factor.
   * 
   * If the scaling is inverse, the value is multiplied by the factor.
   */
  inline Scaling& operator/=(double value);

  /**
   * @brief New-instance multiplication.
   */
  inline Scaling operator*(double value) const;

  /**
   * @brief New-instance division.
   */
  inline Scaling operator/(double value);

private:

  /**
     * @brief The value type.
     */
  Type m_type;

  /**
     * @brief The scaling value.
     */
  double m_value;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_SCALING_IMPL
#include "EleFitsData/impl/Scaling.hpp"
#undef _ELEFITSDATA_SCALING_IMPL
/// @endcond

#endif