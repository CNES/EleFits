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
#include "EL_FitsData/TestColumn.h"

#include "EL_FitsFile/BintableHdu.h"

using namespace Euclid::FitsIO;

template <typename T> // TODO move to FitsDataFixture
void checkEqualVectors(const std::vector<T> &test, const std::vector<T> &expected) {
  BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableHdu_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkScalar() {
  Test::RandomScalarColumn<T> input;
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::Temporary);
  file.assignBintableExt("BINEXT", input);
  const auto output = file.accessFirst<BintableHdu>("BINEXT").readColumn<T>(input.info.name);
  checkEqualVectors(output.vector(), input.vector());
}

template <typename T>
void checkVector() {
  constexpr long rowCount = 10;
  constexpr long repeat = 2;
  Test::RandomScalarColumn<T> input(rowCount * repeat);
  input.info.repeat = repeat;
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::Temporary);
  file.initBintableExt("BINEXT", input.info);
  file.accessFirst<BintableHdu>("BINEXT").writeColumn(input);
  const auto output = file.accessFirst<BintableHdu>("BINEXT").readColumn<T>(input.info.name);
}

/**
 * We test only one type here to check the flow from the top-level API to CFitsIO.
 * Support for other types is tested in EL_CfitsioWrapper.
 */
BOOST_AUTO_TEST_CASE(float_test) {
  checkScalar<float>();
  checkVector<float>();
}

BOOST_AUTO_TEST_CASE(empty_column_test) {
  const std::string filename = Elements::TempFile().path().string();
  VecColumn<float> input({ "NAME", "", 1 }, std::vector<float>());
  MefFile file(filename, MefFile::Permission::Temporary);
  file.assignBintableExt("BINEXT", input);
}

BOOST_AUTO_TEST_CASE(colsize_mismatch_test) {
  VecColumn<float> input0({ "COL0", "", 1 }, std::vector<float>());
  Test::RandomScalarColumn<float> input1(1);
  Test::RandomScalarColumn<float> input2(2);
  input1.info.name = "COL1";
  input2.info.name = "COL2";
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, MefFile::Permission::Temporary);
  BOOST_CHECK_NO_THROW(file.assignBintableExt("0AND1", input0, input1));
  // BOOST_CHECK_NO_THROW(file.assignBintableExt("1AND0", input1, input0));
  // no mapping at fault address 0x0, see https://euclid.roe.ac.uk/issues/13572
  BOOST_CHECK_NO_THROW(file.assignBintableExt("1AND2", input1, input2));
  // BOOST_CHECK_NO_THROW(file.assignBintableExt("2AND1", input2, input1));
  // Syscall param write(buf) points to uninitialised byte(s)
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
