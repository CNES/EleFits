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

#include <boost/test/unit_test.hpp>

#include "EL_FitsData/TestRaster.h"

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void check_random_3d() {
  using namespace FitsIO::Test;
  RandomRaster<T, 3> input({ 2, 3, 4 });
  MinimalFile file;
  try {
    Hdu::createImageExtension(file.fptr, "IMGEXT", input);
    const auto output = Image::readRaster<T, 3>(file.fptr);
    checkEqualVectors(output.vector(), input.vector());
  } catch (const CfitsioError &e) {
    std::cerr << "Input:" << std::endl;
    for (const auto &v : input.vector()) {
      std::cerr << v << ' ';
    }
    std::cerr << std::endl;
    BOOST_FAIL(e.what());
  }
}

#define TEST_3D(type, name) \
  BOOST_AUTO_TEST_CASE(name##_test) { \
    check_random_3d<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(TEST_3D)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
