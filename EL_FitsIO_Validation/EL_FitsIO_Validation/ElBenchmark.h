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

#ifndef _EL_FITSIO_VALIDATION_ELBENCHMARK_H
#define _EL_FITSIO_VALIDATION_ELBENCHMARK_H

#include "EL_FitsFile/MefFile.h"

#include "EL_FitsIO_Validation/Benchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief EL_FitsIO without column I/O buffering.
 * @details
 * Tests on image HDUs are not supported (see ElBenchmark).
 */
class ElUnbufferedBenchmark : public Benchmark {
public:
  virtual ~ElUnbufferedBenchmark() = default;

  ElUnbufferedBenchmark(const std::string& filename) : Benchmark(), m_f(filename, MefFile::Permission::Overwrite) {
  }

  virtual BChronometer::Unit writeBintable(const BColumns& columns) override {
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

protected:
  MefFile m_f;
};

/**
 * @brief Standard EL_FitsIO.
 */
class ElBenchmark : public ElUnbufferedBenchmark {
public:
  virtual ~ElBenchmark() = default;

  ElBenchmark(const std::string& filename) : ElUnbufferedBenchmark(filename) {
  }

  virtual BChronometer::Unit writeImage(const BRaster& raster) override {
    m_chrono.start();
    m_f.assignImageExt("", raster);
    return m_chrono.stop();
  }

  virtual BChronometer::Unit writeBintable(const BColumns& columns) override {
    m_chrono.start();
    m_f.assignBintableExt("", columns);
    return m_chrono.stop();
  }
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
