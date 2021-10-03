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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsFile/FitsFileFixture.h"
#include "EL_FitsFile/ImageRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageRaster_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkRasterIsReadBack() {
  Test::RandomRaster<T, 3> input({ 16, 9, 3 }); // FIXME more dimensions?
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(input.shape);
  du.write(input);
  const auto output = du.read<T, 3>();
  BOOST_TEST(output.vector() == input.vector());
}

#define RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_raster_is_read_back_test) { \
    checkRasterIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(RASTER_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(const_data_raster_is_read_back_test, Test::TemporarySifFile) {
  const Position<2> shape { 7, 2 };
  const auto cData = Test::generateRandomVector<std::int16_t>(shapeSize(shape));
  const PtrRaster<const std::int16_t> cRaster = makeRaster(shape, cData.data());
  writeRaster(cRaster);
  const auto res = readRaster<std::int16_t>();
  BOOST_TEST(res.vector() == cData);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
