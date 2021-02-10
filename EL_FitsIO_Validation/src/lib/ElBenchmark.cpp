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

#include "EL_FitsIO_Validation/ElBenchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

ElUnbufferedBenchmark::ElUnbufferedBenchmark(const std::string& filename) :
    Benchmark(),
    m_f(filename, MefFile::Permission::Overwrite) {
}

BChronometer::Unit ElUnbufferedBenchmark::writeBintable(const BColumns& columns) {
  m_chrono.start();
  const auto& ext = m_f.initBintableExt(
      "",
      std::get<0>(columns).info,
      std::get<1>(columns).info,
      std::get<2>(columns).info,
      std::get<3>(columns).info,
      std::get<4>(columns).info,
      std::get<5>(columns).info,
      std::get<6>(columns).info,
      std::get<7>(columns).info,
      std::get<8>(columns).info,
      std::get<9>(columns).info);
  ext.writeColumn(std::get<0>(columns));
  ext.writeColumn(std::get<1>(columns));
  ext.writeColumn(std::get<2>(columns));
  ext.writeColumn(std::get<3>(columns));
  ext.writeColumn(std::get<4>(columns));
  ext.writeColumn(std::get<5>(columns));
  ext.writeColumn(std::get<6>(columns));
  ext.writeColumn(std::get<7>(columns));
  ext.writeColumn(std::get<8>(columns));
  ext.writeColumn(std::get<9>(columns));
  return m_chrono.stop();
}

ElBenchmark::ElBenchmark(const std::string& filename) : ElUnbufferedBenchmark(filename) {
}

BChronometer::Unit ElBenchmark::writeImage(const BRaster& raster) {
  m_chrono.start();
  m_f.assignImageExt("", raster);
  return m_chrono.stop();
}

BChronometer::Unit ElBenchmark::writeBintable(const BColumns& columns) {
  m_chrono.start();
  m_f.assignBintableExt("", columns);
  return m_chrono.stop();
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid
