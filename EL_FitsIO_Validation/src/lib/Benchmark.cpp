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

#include "EL_FitsIO_Validation/Benchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

Benchmark::Benchmark() : m_chrono(), m_logger(Elements::Logging::getLogger("Benchmark")) {
}

const BChronometer& Benchmark::writeImages(int count, const BRaster& raster) {
  m_chrono.reset();
  for (int i = 0; i < count; ++i) {
    const auto inc = writeImage(raster);
    m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  return m_chrono;
}

const BChronometer& Benchmark::writeBintables(int count, const BColumns& columns) { // TODO avoid duplication
  m_chrono.reset();
  for (int i = 0; i < count; ++i) {
    const auto inc = writeBintable(columns);
    m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  return m_chrono;
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid