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

#include "EL_FitsData/TestColumn.h"

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"

using namespace Euclid;
using namespace Cfitsio;

template <typename T>
void checkEqualVectors(const std::vector<T> &test, const std::vector<T> &expected) {
  BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableWrapper_test)

//-----------------------------------------------------------------------------

/**
 * Learning test which checks that there is indeed a bug in the way
 * CFitsIO handles unsigned integers with BZERO.
 */
BOOST_AUTO_TEST_CASE(cfitsio_overflow_bug_test) {
  FitsIO::Test::MinimalFile file;
  CStrArray ttype({ "COL" });
  auto tform1 = TypeCode<unsigned>::tform(1);
  char *tforms[1];
  tforms[0] = (char *)malloc(3);
  strcpy(tforms[0], tform1.c_str());
  CStrArray tunit({ "" });
  printf("TTYPE: %s\nTFORM: %s\nTUNIT: %s\n", ttype.data()[0], tforms[0], tunit.data()[0]);
  int status = 0;
  fits_create_tbl(file.fptr, BINARY_TBL, 0, 1, ttype.data(), tforms, tunit.data(), "TBL", &status);
  free(tforms[0]);

  constexpr unsigned small = 0;
  constexpr unsigned medium = std::numeric_limits<int>::max();
  constexpr unsigned large = medium + 1;
  constexpr unsigned max = std::numeric_limits<unsigned>::max();
  BOOST_CHECK_LE(large, std::numeric_limits<unsigned>::max());
  unsigned values[] = { small, medium, large, max };

  printf("Small value: %u\n", small);
  fits_write_col(file.fptr, TUINT, 1, 1, 1, 1, &values[0], &status);
  BOOST_CHECK_EQUAL(status, 0);
  status = 0;
  printf("Medium value: %u\n", medium);
  fits_write_col(file.fptr, TUINT, 1, 1, 1, 1, &values[1], &status);
  BOOST_CHECK_EQUAL(status, 0);
  status = 0;
  printf("Large value: %u\n", large);
  fits_write_col(file.fptr, TUINT, 1, 1, 1, 1, &values[2], &status);
  BOOST_CHECK_EQUAL(status, 0);
  status = 0;
  printf("Max value: %u\n", max);
  fits_write_col(file.fptr, TUINT, 1, 1, 1, 1, &values[3], &status);
  BOOST_CHECK_EQUAL(status, 0);
}

template <typename T>
void checkScalar() {
  FitsIO::Test::RandomScalarColumn<T> input;
  FitsIO::Test::MinimalFile file;
  try {
    Hdu::createBintableExtension(file.fptr, "BINEXT", input);
    const auto output = Bintable::readColumn<T>(file.fptr, input.info.name);
    checkEqualVectors(output.vector(), input.vector());
  } catch (const CfitsioError &e) {
    std::cerr << "Input:" << std::endl;
    for (const auto &v : input.vector()) {
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

#define TEST_SCALAR_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE(name##_test) { \
    checkScalar<type>(); \
  }

#define TEST_SCALAR(type) TEST_SCALAR_ALIAS(type, type)

#define TEST_SCALAR_UNSIGNED(type) TEST_SCALAR_ALIAS(unsigned type, u##type)

// TEST_SCALAR(bool) //TODO won't compile because a vector of bools has no .data()
TEST_SCALAR(char)
TEST_SCALAR(short)
TEST_SCALAR(int)
TEST_SCALAR(long)
TEST_SCALAR_ALIAS(std::int16_t, int16)
TEST_SCALAR_ALIAS(std::int32_t, int32)
TEST_SCALAR_ALIAS(std::int64_t, int64)
TEST_SCALAR(float)
TEST_SCALAR(double)
TEST_SCALAR_ALIAS(std::string, string)
TEST_SCALAR_UNSIGNED(char)
TEST_SCALAR_UNSIGNED(short)
TEST_SCALAR_UNSIGNED(int)
TEST_SCALAR_UNSIGNED(long)
TEST_SCALAR_ALIAS(std::uint16_t, uint16)
TEST_SCALAR_ALIAS(std::uint32_t, uint32)
TEST_SCALAR_ALIAS(std::uint64_t, uint64)

template <typename T>
void checkVector() {
  constexpr long rowCount = 3;
  constexpr long repeat = 2;
  FitsIO::Test::RandomScalarColumn<T> input(rowCount * repeat);
  input.info.repeat = repeat;
  FitsIO::Test::MinimalFile file;
  try {
    Hdu::createBintableExtension(file.fptr, "BINEXT", input);
    const auto output = Bintable::readColumn<T>(file.fptr, input.info.name);
    BOOST_CHECK_EQUAL(output.info.repeat, repeat);
    checkEqualVectors(output.vector(), input.vector());
  } catch (const CfitsioError &e) {
    std::cerr << "Input:" << std::endl;
    for (const auto &v : input.vector()) {
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

#define TEST_VECTOR_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE(vector_##name##_test) { \
    checkVector<type>(); \
  }

#define TEST_VECTOR(type) TEST_VECTOR_ALIAS(type, type)

#define TEST_VECTOR_UNSIGNED(type) TEST_VECTOR_ALIAS(unsigned type, u##type)

// TEST_VECTOR(bool) //TODO won't compile because of vector specialization for bool
TEST_VECTOR(char)
TEST_VECTOR(short)
TEST_VECTOR(int)
TEST_VECTOR(long)
TEST_VECTOR_ALIAS(std::int16_t, int16)
TEST_VECTOR_ALIAS(std::int32_t, int32)
TEST_VECTOR_ALIAS(std::int64_t, int64)
TEST_VECTOR(float)
TEST_VECTOR(double)
TEST_VECTOR_UNSIGNED(char)
TEST_VECTOR_UNSIGNED(short)
TEST_VECTOR_UNSIGNED(int)
TEST_VECTOR_UNSIGNED(long)
TEST_VECTOR_ALIAS(std::uint16_t, uint16)
TEST_VECTOR_ALIAS(std::uint32_t, uint32)
TEST_VECTOR_ALIAS(std::uint64_t, uint64)

BOOST_FIXTURE_TEST_CASE(small_table_test, FitsIO::Test::MinimalFile) {
  using FitsIO::Test::SmallTable;
  SmallTable input;
  Hdu::createBintableExtension(this->fptr, "IMGEXT", input.numCol, input.radecCol, input.nameCol, input.distMagCol);
  const auto outputNums = Bintable::readColumn<SmallTable::Num>(this->fptr, input.numCol.info.name);
  checkEqualVectors(outputNums.vector(), input.numCol.vector());
  const auto outputRadecs = Bintable::readColumn<SmallTable::Radec>(this->fptr, input.radecCol.info.name);
  checkEqualVectors(outputRadecs.vector(), input.radecCol.vector());
  const auto outputNames = Bintable::readColumn<SmallTable::Name>(this->fptr, input.nameCol.info.name);
  checkEqualVectors(outputNames.vector(), input.nameCol.vector());
  const auto outputDistsMags = Bintable::readColumn<SmallTable::DistMag>(this->fptr, input.distMagCol.info.name);
  checkEqualVectors(outputDistsMags.vector(), input.distMagCol.vector());
}

BOOST_FIXTURE_TEST_CASE(rowwise_test, FitsIO::Test::MinimalFile) {
  constexpr long rowCount(10000);
  FitsIO::Test::RandomScalarColumn<int> i(rowCount);
  i.info.name = "I";
  FitsIO::Test::RandomScalarColumn<float> f(rowCount);
  f.info.name = "F";
  FitsIO::Test::RandomScalarColumn<double> d(rowCount);
  d.info.name = "D";
  Hdu::createBintableExtension(this->fptr, "BINEXT", i, f, d);
  const auto table = Bintable::readColumns<int, float, double>(this->fptr, { "I", "F", "D" });
  checkEqualVectors(std::get<0>(table).vector(), i.vector());
  checkEqualVectors(std::get<1>(table).vector(), f.vector());
  checkEqualVectors(std::get<2>(table).vector(), d.vector());
}

BOOST_FIXTURE_TEST_CASE(append_test, FitsIO::Test::MinimalFile) {
  using FitsIO::Test::SmallTable;
  SmallTable table;
  Hdu::createBintableExtension(this->fptr, "TABLE", table.nameCol);
  const auto names = Bintable::readColumn<SmallTable::Name>(fptr, table.nameCol.info.name);
  checkEqualVectors(names.vector(), table.names);
  Bintable::appendColumns(fptr, table.distMagCol, table.radecCol);
  const auto distsMags = Bintable::readColumn<SmallTable::DistMag>(fptr, table.distMagCol.info.name);
  checkEqualVectors(distsMags.vector(), table.distsMags);
  const auto radecs = Bintable::readColumn<SmallTable::Radec>(fptr, table.radecCol.info.name);
  checkEqualVectors(radecs.vector(), table.radecs);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
