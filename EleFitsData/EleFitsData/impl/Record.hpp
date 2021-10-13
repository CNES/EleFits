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

#if defined(_EL_FITSDATA_RECORD_IMPL) || defined(CHECK_QUALITY)

  #include "EleFitsData/Record.h"

  #include <type_traits> // enable_if & co

namespace Euclid {
namespace FitsIO {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Valid only if TFrom and TTo are different.
 */
template <typename TFrom, typename TTo>
using ifDifferent = typename std::enable_if<not std::is_same<TFrom, TTo>::value>::type;

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
  /** @brief Cast. */
  inline static TTo cast(TFrom value);
};

/**
 * @brief Not a cast.
 */
template <typename TFrom>
struct CasterImpl<TFrom, TFrom, void> {
  /** @brief Cast. */
  inline static TFrom cast(TFrom value);
};

/**
 * @brief Cast complex<TFrom> to complex<TTo>.
 */
template <typename TFrom, typename TTo>
struct CasterImpl<std::complex<TFrom>, std::complex<TTo>, ifDifferent<TFrom, TTo>> {
  /** @brief Cast. */
  inline static std::complex<TTo> cast(std::complex<TFrom> value);
};

/**
 * @brief Cast any to number.
 */
template <typename TTo>
struct CasterImpl<VariantValue, TTo, ifScalar<TTo>> {
  /** @brief Cast. */
  inline static TTo cast(VariantValue value);
};

/**
 * @brief Cast any to complex.
 */
template <typename TTo>
struct CasterImpl<VariantValue, std::complex<TTo>, ifScalar<TTo>> {
  /** @brief Cast. */
  inline static std::complex<TTo> cast(VariantValue value);
};

/**
 * @brief Cast any to string.
 */
template <>
struct CasterImpl<VariantValue, std::string, void> {
  /** @brief Cast. */
  inline static std::string cast(VariantValue value);
};

/**
 * @brief Cast all to any.
 */
template <typename TFrom>
struct CasterImpl<TFrom, VariantValue, ifDifferent<TFrom, VariantValue>> {
  /** @brief Cast. */
  inline static VariantValue cast(TFrom value);
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
  return { CasterImpl<TFrom, TTo>::cast(value.real()), CasterImpl<TFrom, TTo>::cast(value.imag()) };
}

/*
 * TODO
 * This function only handles a subset of the supported types (scalar types),
 * therefore macro EL_FITSIO_FOREACH_RECORD_TYPE cannot be used straightforwardly:
 * ifScalar could be used.
 * Another option is to rely on a map<typeid, function>
 */
template <typename TTo>
TTo CasterImpl<VariantValue, TTo, ifScalar<TTo>>::cast(VariantValue value) {
  const auto& id = value.type();
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
std::complex<TTo> CasterImpl<VariantValue, std::complex<TTo>, ifScalar<TTo>>::cast(VariantValue value) {
  const auto& id = value.type();
  if (id == typeid(std::complex<TTo>)) {
    return boost::any_cast<std::complex<TTo>>(value);
  } else if (id == typeid(std::complex<float>)) {
    return CasterImpl<std::complex<float>, std::complex<TTo>>::cast(boost::any_cast<std::complex<float>>(value));
  } else if (id == typeid(std::complex<double>)) {
    return CasterImpl<std::complex<double>, std::complex<TTo>>::cast(boost::any_cast<std::complex<double>>(value));
  } else {
    throw boost::bad_any_cast();
  }
}

std::string CasterImpl<VariantValue, std::string, void>::cast(VariantValue value) {
  return boost::any_cast<std::string>(value);
}

template <typename TFrom>
VariantValue CasterImpl<TFrom, VariantValue, ifDifferent<TFrom, VariantValue>>::cast(TFrom value) {
  return VariantValue(value);
}

} // namespace Internal
/// @endcond

template <typename T>
Record<T>::Record(const std::string& k, T v, const std::string& u, const std::string& c) :
    keyword(k), value(v), unit(u), comment(c) {}

template <typename T>
template <typename TFrom>
Record<T>::Record(const Record<TFrom>& other) :
    keyword(other.keyword), value(Internal::CasterImpl<TFrom, T>::cast(other.value)), unit(other.unit),
    comment(other.comment) {}

template <typename T>
template <typename TFrom>
Record<T>& Record<T>::assign(const std::string& k, TFrom v, const std::string& u, const std::string& c) {
  keyword = k;
  value = Internal::CasterImpl<TFrom, T>::cast(v);
  unit = u;
  comment = c;
  return *this;
}

template <typename T>
template <typename TFrom>
Record<T>& Record<T>::assign(const Record<TFrom>& other) {
  return assign(other.keyword, other.value, other.unit, other.comment);
}

template <typename T>
Record<T>::operator T() const {
  return value;
}

template <typename T>
template <typename TFrom>
T Record<T>::cast(TFrom value) {
  return Internal::CasterImpl<TFrom, T>::cast(value);
}

template <typename T>
std::string Record<T>::rawComment() const {
  if (unit.empty()) {
    return comment;
  }
  return "[" + unit + "] " + comment;
}

template <typename T>
bool Record<T>::hasLongKeyword() const {
  return keyword.length() > 8;
}

template <typename T>
bool Record<T>::hasLongStringValue() const {
  return false;
}

template <>
bool Record<std::string>::hasLongStringValue() const;

template <>
bool Record<const char*>::hasLongStringValue() const;

template <>
bool Record<VariantValue>::hasLongStringValue() const;

  #ifndef DECLARE_RECORD_CLASS
    #define DECLARE_RECORD_CLASS(type, unused) extern template struct Record<type>;
EL_FITSIO_FOREACH_RECORD_TYPE(DECLARE_RECORD_CLASS)
    #undef DECLARE_RECORD_CLASS
  #endif

} // namespace FitsIO
} // namespace Euclid

#endif
