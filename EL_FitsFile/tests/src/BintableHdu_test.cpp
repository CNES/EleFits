/**
 * @file tests/src/BintableHdu_test.cpp
 * @date 08/30/19
 * @author user
 *
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

#include "ElementsKernel/Temporary.h"

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsData/FitsDataFixture.h"

#include "EL_FitsFile/BintableHdu.h"

using namespace Euclid::FitsIO;

template<typename T>
void check_equal_vectors(const std::vector<T>& test, const std::vector<T>& expected) {
  BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BintableHdu_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_scalar() {
  Test::RandomScalarColumn<T> input;
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::TEMPORARY);
  file.assign_bintable_ext("BINEXT", input);
  const auto output = file.access_first<BintableHdu>("BINEXT").read_column<T>(input.info.name);
  check_equal_vectors(output.vector(), input.vector());
}

/**
 * We test only one type here to check the flow from the top-level API to CFitsIO.
 * Support for other types is tested in EL_CfitsioWrapper.
 */
BOOST_AUTO_TEST_CASE( float_test ) {
  check_scalar<float>();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


