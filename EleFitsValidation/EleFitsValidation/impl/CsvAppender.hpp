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

#include "EleFitsValidation/CsvAppender.h"

#if defined(_EL_FITSIO_VALIDATION_CSVAPPENDER_IMPL) || defined(CHECK_QUALITY)

namespace Euclid {
namespace FitsIO {
namespace Test {

template <typename T>
CsvAppender& CsvAppender::operator<<(const T& value) {
  m_file << value << m_sep;
  return *this;
}

template <typename... Ts>
CsvAppender& CsvAppender::writeRow(const Ts&... values) {
  // TODO check size
  using mockUnpack = int[];
  (void)mockUnpack { (operator<<(values), 0)... };
  return operator<<(std::endl);
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
