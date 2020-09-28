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

#ifndef _EL_FITSDATA_TESTRECORD_H
#define _EL_FITSDATA_TESTRECORD_H

#include <algorithm>
#include <complex>
#include <string>

#include "EL_FitsData/TestUtils.h"
#include "EL_FitsData/Record.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief A random set of Records.
 * @details
 * All compatible types of Records are generated, but `boost::any`.
 */
struct RandomHeader {
  /** @brief bool */
  Record<bool> b { "BOOL", generateRandomValue<bool>(), "b", "bool" };
  /** @brief char */
  Record<char> c { "CHAR", generateRandomValue<char>(), "c", "char" };
  /** @brief short */
  Record<short> s { "SHORT", generateRandomValue<short>(), "s", "short" };
  /** @brief int */
  Record<int> i { "INT", generateRandomValue<int>(), "i", "int" };
  /** @brief long */
  Record<long> l { "LONG", generateRandomValue<long>(), "l", "long" };
  /** @brief long long */
  Record<long long> ll { "LLONG", generateRandomValue<long long>(), "ll", "l long" };
  /** @brief unsigned char */
  Record<unsigned char> uc { "UCHAR", generateRandomValue<unsigned char>(), "uc", "u char" };
  /** @brief unsigned short */
  Record<unsigned short> us { "USHORT", generateRandomValue<unsigned short>(), "us", "u short" };
  /** @brief unsigned int */
  Record<unsigned int> ui { "UINT", generateRandomValue<unsigned int>(), "ui", "u int" };
  /** @brief unsigned long */
  Record<unsigned long> ul { "ULONG", generateRandomValue<unsigned long>(), "ul", "u long" };
  /** @brief unsigned long long */
  Record<unsigned long long> ull { "ULLONG", generateRandomValue<unsigned long long>(), "ull", "ul long" };
  /** @brief std::string */
  Record<std::string> str { "STRING", generateRandomValue<std::string>(), "s", "string" };
  /** @brief const char* */
  Record<const char *> cstr { "CSTR", generateRandomValue<std::string>().c_str(), "cs", "c str" };
  /** @brief float */
  Record<float> f { "FLOAT", generateRandomValue<float>(), "f", "float" };
  /** @brief double */
  Record<double> d { "DOUBLE", generateRandomValue<double>(), "d", "double" };
  /** @brief std::complex<float> */
  Record<std::complex<float>> cf { "CFLOAT", generateRandomValue<std::complex<float>>(), "cf", "c float" };
  /** @brief std::complex<double> */
  Record<std::complex<double>> cd { "CDOUBLE", generateRandomValue<std::complex<double>>(), "cd", "c double" };

  /**
   * Generate a random record.
   * @tparam T The value type
   * @param typeName The type name as a string, e.g. `"unsigned long int"`
   * @details
   * In `"unsigned long int"`, `"unsigned"` and `"long"` are prefixes, and `"int"` is the suffix.
   * The generated record is as follows:
   * - The keyword is made of the first letter of the prefixes and the suffix, capitalized, e.g. `"ULINT"`;
   * - The value is random;
   * - The unit is made of the first letter of the prefixes, e.g. `"ul"`;
   * - The comment is made of the first letter of the prefixes, a space, and the suffix, e.g. `"ul int"`.
   */
  template <typename T>
  static Record<T> generateRecord(std::string typeName);
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
