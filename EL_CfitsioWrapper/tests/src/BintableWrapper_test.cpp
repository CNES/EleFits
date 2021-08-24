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

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_FitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkScalarColumnIsReadBack() {
  using namespace FitsIO::Test;
  RandomScalarColumn<T> input;
  MinimalFile file;
  try {
    Hdu::createBintableExtension(file.fptr, "BINEXT", input);
    const auto index = Bintable::columnIndex(file.fptr, input.info.name);
    BOOST_TEST(index == 1);
    const auto info = Bintable::readColumnInfo<T>(file.fptr, index);
    BOOST_TEST(info.name == input.info.name);
    BOOST_TEST(info.unit == input.info.unit);
    BOOST_TEST(info.repeatCount == input.info.repeatCount);
    const auto output = Bintable::readColumn<T>(file.fptr, input.info.name);
    BOOST_TEST(output.vector() == input.vector());
  } catch (const CfitsioError& e) {
    std::cerr << "Input:" << std::endl;
    for (const auto& v : input.vector()) {
      std::cerr << v << ' ';
    }
    std::cerr << std::endl;
    if (e.status == NUM_OVERFLOW) {
      BOOST_WARN(e.what());
    } else {
      BOOST_FAIL(e.what());
    }
  }
}

#define SCALAR_COLUMN_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_scalar_column_is_read_back_test_test) { \
    checkScalarColumnIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_COLUMN_TYPE(SCALAR_COLUMN_IS_READ_BACK_TEST)

template <typename T>
void checkVectorColumnIsReadBack() {
  using namespace FitsIO::Test;
  constexpr long rowCount = 3;
  constexpr long repeatCount = 2;
  RandomVectorColumn<T> input(repeatCount, rowCount);
  MinimalFile file;
  try {
    Hdu::createBintableExtension(file.fptr, "BINEXT", input);
    const auto output = Bintable::readColumn<T>(file.fptr, input.info.name);
    BOOST_TEST(output.info.repeatCount == repeatCount);
    BOOST_TEST(output.vector() == input.vector());
  } catch (const CfitsioError& e) {
    std::cerr << "Input:" << std::endl;
    for (const auto& v : input.vector()) {
      std::cerr << v << ' ';
    }
    std::cerr << std::endl;
    if (e.status == NUM_OVERFLOW) {
      BOOST_WARN(e.what());
    } else {
      BOOST_FAIL(e.what());
    }
  }
}

template <>
void checkVectorColumnIsReadBack<std::string>();

template <>
void checkVectorColumnIsReadBack<std::string>() {
  // String is tested as a scalar column
}

#define VECTOR_COLUMN_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_vector_column_is_read_back_test_test) { \
    checkVectorColumnIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_COLUMN_TYPE(VECTOR_COLUMN_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(small_table_test, FitsIO::Test::MinimalFile) {
  using namespace FitsIO::Test;
  SmallTable input;
  Hdu::createBintableExtension(this->fptr, "IMGEXT", input.numCol, input.radecCol, input.nameCol, input.distMagCol);
  const auto outputNums = Bintable::readColumn<SmallTable::Num>(this->fptr, input.numCol.info.name);
  BOOST_TEST(outputNums.vector() == input.numCol.vector());
  const auto outputRadecs = Bintable::readColumn<SmallTable::Radec>(this->fptr, input.radecCol.info.name);
  BOOST_TEST(outputRadecs.vector() == input.radecCol.vector());
  const auto outputNames = Bintable::readColumn<SmallTable::Name>(this->fptr, input.nameCol.info.name);
  BOOST_TEST(outputNames.vector() == input.nameCol.vector());
  const auto outputDistsMags = Bintable::readColumn<SmallTable::DistMag>(this->fptr, input.distMagCol.info.name);
  BOOST_TEST(outputDistsMags.vector() == input.distMagCol.vector());
}

BOOST_FIXTURE_TEST_CASE(rowwise_test, FitsIO::Test::MinimalFile) {
  using namespace FitsIO::Test;
  constexpr long rowCount(10000); // Large enough to ensure CFitsIO buffer is full
  RandomScalarColumn<int> i(rowCount);
  i.info.name = "I";
  RandomScalarColumn<float> f(rowCount);
  f.info.name = "F";
  RandomScalarColumn<double> d(rowCount);
  d.info.name = "D";
  Hdu::createBintableExtension(this->fptr, "BINEXT", i, f, d);
  const auto table = Bintable::readColumns<int, float, double>(this->fptr, { "I", "F", "D" });
  int status = 0;
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  BOOST_TEST(chunkRows < rowCount);
  BOOST_TEST(std::get<0>(table).vector() == i.vector());
  BOOST_TEST(std::get<1>(table).vector() == f.vector());
  BOOST_TEST(std::get<2>(table).vector() == d.vector());
}

BOOST_FIXTURE_TEST_CASE(append_columns_test, FitsIO::Test::MinimalFile) {
  using namespace FitsIO::Test;
  SmallTable table;
  Hdu::createBintableExtension(this->fptr, "TABLE", table.nameCol);
  const auto names = Bintable::readColumn<SmallTable::Name>(fptr, table.nameCol.info.name);
  BOOST_TEST(names.vector() == table.names);
  Bintable::appendColumns(fptr, table.distMagCol, table.radecCol);
  const auto distsMags = Bintable::readColumn<SmallTable::DistMag>(fptr, table.distMagCol.info.name);
  BOOST_TEST(distsMags.vector() == table.distsMags);
  const auto radecs = Bintable::readColumn<SmallTable::Radec>(fptr, table.radecCol.info.name);
  BOOST_TEST(radecs.vector() == table.radecs);
}

BOOST_FIXTURE_TEST_CASE(append_rows_test, FitsIO::Test::MinimalFile) {
  using namespace FitsIO::Test;
  const SmallTable table;
  const auto initSize = table.names.size();
  Hdu::createBintableExtension(this->fptr, "TABLE", table.nameCol, table.radecCol);
  BOOST_TEST(Bintable::rowCount(this->fptr) == initSize);
  Bintable::writeColumns(this->fptr, table.nameCol, table.radecCol);
  BOOST_TEST(Bintable::rowCount(this->fptr) == initSize * 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
