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

template<typename T>
void check_vector() {
  constexpr std::size_t rows = 10;
  constexpr std::size_t repeat = 2;
  Test::RandomScalarColumn<T> input(rows * repeat);
  input.info.repeat = repeat;
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::TEMPORARY);
  file.init_bintable_ext("BINEXT", input.info);
  file.access_first<BintableHdu>("BINEXT").write_column(input);
  const auto output = file.access_first<BintableHdu>("BINEXT").read_column<T>(input.info.name);
}

/**
 * We test only one type here to check the flow from the top-level API to CFitsIO.
 * Support for other types is tested in EL_CfitsioWrapper.
 */
BOOST_AUTO_TEST_CASE( float_test ) {
  check_scalar<float>();
  check_vector<float>();
}

BOOST_AUTO_TEST_CASE( empty_column_test ) {
  const std::string filename = Elements::TempFile().path().string();
  VecColumn<float> input({"NAME", "", 1}, std::vector<float>());
  MefFile file(filename, MefFile::Permission::TEMPORARY);
  file.assign_bintable_ext("BINEXT", input);
}

BOOST_AUTO_TEST_CASE( colsize_mismatch_test ) {
  VecColumn<float> input0({"COL0", "", 1}, std::vector<float>());
  Test::RandomScalarColumn<float> input1(1);
  Test::RandomScalarColumn<float> input2(2);
  input1.info.name = "COL1";
  input2.info.name = "COL2";
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::TEMPORARY);
  BOOST_CHECK_NO_THROW(file.assign_bintable_ext("0AND1", input0, input1));
  BOOST_CHECK_NO_THROW(file.assign_bintable_ext("1AND0", input1, input0));
  BOOST_CHECK_NO_THROW(file.assign_bintable_ext("1AND2", input1, input2));
  BOOST_CHECK_NO_THROW(file.assign_bintable_ext("2AND1", input2, input1));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
