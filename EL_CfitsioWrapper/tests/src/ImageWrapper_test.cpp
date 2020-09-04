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

#include "EL_FitsData/FitsDataFixture.h"

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"


using namespace Euclid;
using namespace Cfitsio;

template<typename T>
void checkEqualVectors(const std::vector<T>& test, const std::vector<T>& expected) {
  BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImageWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_random_3d() {
  FitsIO::Test::RandomRaster<T, 3> input({2, 3, 4});
  FitsIO::Test::MinimalFile file;
  try {
    Hdu::createImageExtension(file.fptr, "IMGEXT", input);
    const auto output = Image::readRaster<T, 3>(file.fptr);
    checkEqualVectors(output.vector(), input.vector());
  } catch(const CfitsioError& e) {
    std::cerr << "Input:" << std::endl;
    for (const auto& v : input.vector()) {
      std::cerr << v << ' ';
    }
    std::cerr << std::endl;
    BOOST_FAIL(e.what());
  }
}

#define TEST_3D_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE( name##_test ) { check_random_3d<type>(); }

#define TEST_3D(type) \
  TEST_3D_ALIAS(type, type)

#define TEST_3D_UNSIGNED(type) \
  TEST_3D_ALIAS(unsigned type, u##type)

TEST_3D(char)
TEST_3D(short)
TEST_3D(int)
TEST_3D(long)
TEST_3D_ALIAS(std::int16_t, int16)
TEST_3D_ALIAS(std::int32_t, int32)
TEST_3D_ALIAS(std::int64_t, int64)
TEST_3D(float)
TEST_3D(double)
TEST_3D_UNSIGNED(char)
TEST_3D_UNSIGNED(short)
TEST_3D_UNSIGNED(int)
TEST_3D_UNSIGNED(long)
TEST_3D_ALIAS(std::uint16_t, uint16)
TEST_3D_ALIAS(std::uint32_t, uint32)
TEST_3D_ALIAS(std::uint64_t, uint64)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
