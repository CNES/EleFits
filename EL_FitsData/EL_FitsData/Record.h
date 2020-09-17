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

#ifndef _EL_FITSDATA_RECORD_H
#define _EL_FITSDATA_RECORD_H

#include <boost/any.hpp>
#include <complex>
#include <string>
#include <tuple>
#include <type_traits> // enable_if

namespace Euclid {
namespace FitsIO {

/**
 * @brief Keyword-value pair with optional unit and comment.
 * @details
 * Record is meant to be used to read and write Fits headers.
 * The Record in the Fits file is rendered as:
 * \verbatim keyword = value / [unit] comment \endverbatim
 * if the unit is provided, or:
 * \verbatim keyword = value / comment \endverbatim
 * otherwise.
 * @see \ref data-classes
 */
template <typename T>
struct Record {

  /**
   * @brief Assign a record.
   * @param k The keyword
   * @param v The value
   * @param u The unit
   * @param c The comment
   */
  Record(const std::string &k = "", T v = T(), const std::string &u = "", const std::string &c = "");

  /**
   * @brief Create a Record from a Record of another type.
   * @details
   * This constructor can be used to homogeneize types, for example to create a
   * \c vector<Record<any>> from various \c Record<T>'s with different \c T's.
   * @warning
   * Source type TOther must be castable to destination type T.
   * Valid casts are:
   * - scalar number -> scalar number
   * - complex number -> complex number
   * - \c any -> scalar number if the underlying value type is a scalar number
   * - \c any -> complex number if the value type is a complex number
   * - \c any -> \c string if the value type is a \c string
   * - scalar number -> \c any
   * - complex number -> \c any
   * - \c string -> \c any
   */
  template <typename TOther>
  Record(const Record<TOther> &other);

  /**
   * @brief Slice the record as its value.
   * @details
   * A Record is often read for using its value only;
   * This shortcut is merely a const version of the value member.
   * @code
   * SifFile f("filename.fits");
   * const auto& h = f.header();
   * // Immediately cast to int:
   * int value = h.parseRecord<int>("KEYWORD");
   * // Same as:
   * int value = h.parseRecord<int>("KEYWORD").value;
   * @endcode
   */
  operator T() const;

  /**
   * @brief The keyword.
   */
  std::string keyword;

  /**
   * @brief The value.
   */
  T value;

  /**
   * @brief The unit.
   */
  std::string unit;

  /**
   * @brief The comment without the unit.
   */
  std::string comment;
};

/////////////////
/// INTERNAL ///
///////////////

namespace Internal {

/**
 * @brief Valid only if TFrom and TTo are different.
 */
template <typename TFrom, typename TTo>
using ifDifferent = typename std::enable_if<!std::is_same<TFrom, TTo>::value>::type;

/**
 * @brief Valid only if TTo is a number (not a complex, not a string, and not an any).
 */
template <typename TTo>
using ifScalar = typename std::enable_if<std::is_arithmetic<TTo>::value>::type;

/**
 * @brief Helper class to cast TFrom to TTo.
 * @details
 * Valid casts are:
 * - scalar -> scalar
 * - complex -> complex
 * - any -> scalar/complex/string according to underlying value
 * - anything -> any
 */
template <typename TFrom, typename TTo, class TValid = void>
struct CasterImpl {
  inline static TTo cast(TFrom value);
};

/**
 * @brief Not a cast.
 */
template <typename TFrom>
struct CasterImpl<TFrom, TFrom, void> {
  inline static TFrom cast(TFrom value);
};

/**
 * @brief Cast complex<TFrom> to complex<TTo>.
 */
template <typename TFrom, typename TTo>
struct CasterImpl<std::complex<TFrom>, std::complex<TTo>, ifDifferent<TFrom, TTo>> {
  inline static std::complex<TTo> cast(std::complex<TFrom> value);
};

/**
 * @brief Cast any to number.
 */
template <typename TTo>
struct CasterImpl<boost::any, TTo, ifScalar<TTo>> {
  inline static TTo cast(boost::any value);
};

/**
 * @brief Cast any to complex.
 */
template <typename TTo>
struct CasterImpl<boost::any, std::complex<TTo>, ifScalar<TTo>> {
  inline static TTo cast(boost::any value);
};

/**
 * @brief Cast any to string.
 */
template <>
struct CasterImpl<boost::any, std::string, void> {
  inline static std::string cast(boost::any value);
};

/**
 * @brief Cast all to any.
 */
template <typename TFrom>
struct CasterImpl<TFrom, boost::any, ifDifferent<TFrom, boost::any>> {
  inline static boost::any cast(TFrom value);
};

template <typename TFrom, typename TTo, class TValid>
TTo CasterImpl<TFrom, TTo, TValid>::cast(TFrom value) {
  return static_cast<TTo>(value);
}

template <typename TFrom>
TFrom CasterImpl<TFrom, TFrom, void>::cast(TFrom value) {
  return value;
}

template <typename TFrom, typename TTo>
std::complex<TTo>
CasterImpl<std::complex<TFrom>, std::complex<TTo>, ifDifferent<TFrom, TTo>>::cast(std::complex<TFrom> value) {
  return { CasterImpl<TFrom, TTo>::cast(value.im()), CasterImpl<TFrom, TTo>::cast(value.re()) };
}

template <typename TTo>
TTo CasterImpl<boost::any, TTo, ifScalar<TTo>>::cast(boost::any value) {
  const auto &id = value.type();
  if (id == typeid(TTo)) {
    return boost::any_cast<TTo>(value);
  } else if (id == typeid(bool)) {
    return CasterImpl<bool, TTo>::cast(boost::any_cast<bool>(value));
  } else if (id == typeid(char)) {
    return CasterImpl<char, TTo>::cast(boost::any_cast<char>(value));
  } else if (id == typeid(short)) {
    return CasterImpl<short, TTo>::cast(boost::any_cast<short>(value));
  } else if (id == typeid(int)) {
    return CasterImpl<int, TTo>::cast(boost::any_cast<int>(value));
  } else if (id == typeid(long)) {
    return CasterImpl<long, TTo>::cast(boost::any_cast<long>(value));
  } else if (id == typeid(long long)) {
    return CasterImpl<long long, TTo>::cast(boost::any_cast<long long>(value));
  } else if (id == typeid(float)) {
    return CasterImpl<float, TTo>::cast(boost::any_cast<float>(value));
  } else if (id == typeid(double)) {
    return CasterImpl<double, TTo>::cast(boost::any_cast<double>(value));
  } else if (id == typeid(unsigned char)) {
    return CasterImpl<unsigned char, TTo>::cast(boost::any_cast<unsigned char>(value));
  } else if (id == typeid(unsigned short)) {
    return CasterImpl<unsigned short, TTo>::cast(boost::any_cast<unsigned short>(value));
  } else if (id == typeid(unsigned int)) {
    return CasterImpl<unsigned int, TTo>::cast(boost::any_cast<unsigned int>(value));
  } else if (id == typeid(unsigned long)) {
    return CasterImpl<unsigned long, TTo>::cast(boost::any_cast<unsigned long>(value));
  } else if (id == typeid(unsigned long long)) {
    return CasterImpl<unsigned long long, TTo>::cast(boost::any_cast<unsigned long long>(value));
  } else {
    throw boost::bad_any_cast();
  }
}

template <typename TTo>
TTo CasterImpl<boost::any, std::complex<TTo>, ifScalar<TTo>>::cast(boost::any value) {
  const auto &id = value.type();
  if (id == typeid(TTo)) {
    return boost::any_cast<TTo>(value);
  } else if (id == typeid(std::complex<float>)) {
    return CasterImpl<std::complex<float>, TTo>::cast(boost::any_cast<std::complex<float>>(value));
  } else if (id == typeid(std::complex<double>)) {
    return CasterImpl<std::complex<double>, TTo>::cast(boost::any_cast<std::complex<double>>(value));
  } else {
    throw boost::bad_any_cast();
  }
}

std::string CasterImpl<boost::any, std::string, void>::cast(boost::any value) {
  return boost::any_cast<std::string>(value);
}

template <typename TFrom>
boost::any CasterImpl<TFrom, boost::any, ifDifferent<TFrom, boost::any>>::cast(TFrom value) {
  return boost::any(value);
}

} // namespace Internal

/////////////////////
// IMPLEMENTATION //
///////////////////

template <typename T>
Record<T>::Record(const std::string &k, T v, const std::string &u, const std::string &c) :
    keyword(k),
    value(v),
    unit(u),
    comment(c) {
}

template <typename T>
template <typename TOther>
Record<T>::Record(const Record<TOther> &other) :
    keyword(other.keyword),
    value(Internal::CasterImpl<TOther, T>::cast(other.value)),
    unit(other.unit),
    comment(other.comment) {
}

template <typename T>
Record<T>::operator T() const {
  return value;
}

} // namespace FitsIO
} // namespace Euclid

#endif
