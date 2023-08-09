// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsData/TestColumn.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkScalarColumnIsReadBack() {
  using namespace Fits::Test;
  RandomScalarColumn<T> input;
  MinimalFile file;
  try {
    HduAccess::assign_bintable(file.fptr, "BINEXT", input);
    const auto index = BintableIo::columnIndex(file.fptr, input.info().name);
    BOOST_TEST(index == 1);
    const auto info = BintableIo::readColumnInfo<T>(file.fptr, index);
    BOOST_TEST((info == input.info()));
    const auto output = BintableIo::readColumn<T>(file.fptr, input.info().name);
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

ELEFITS_FOREACH_COLUMN_TYPE(SCALAR_COLUMN_IS_READ_BACK_TEST)

template <typename T>
void checkVectorColumnIsReadBack() {
  using namespace Fits::Test;
  constexpr long rowCount = 3;
  constexpr long repeatCount = 2;
  RandomVectorColumn<T> input(repeatCount, rowCount);
  MinimalFile file;
  try {
    HduAccess::assign_bintable(file.fptr, "BINEXT", input);
    BOOST_TEST(BintableIo::rowCount(file.fptr) == rowCount);
    const auto output = BintableIo::readColumn<T>(file.fptr, input.info().name);
    BOOST_TEST(output.info().repeatCount() == repeatCount);
    BOOST_TEST(output.rowCount() == rowCount);
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
  BOOST_AUTO_TEST_CASE(name##_vector_column_is_read_back_test) { \
    checkVectorColumnIsReadBack<type>(); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(VECTOR_COLUMN_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(small_table_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallTable input;
  HduAccess::assign_bintable(
      this->fptr,
      "IMGEXT",
      input.numCol,
      input.radecCol,
      input.nameCol,
      input.distMagCol);
  const auto outputNums = BintableIo::readColumn<SmallTable::Num>(this->fptr, input.numCol.info().name);
  BOOST_TEST(outputNums.vector() == input.nums);
  const auto outputRadecs = BintableIo::readColumn<SmallTable::Radec>(this->fptr, input.radecCol.info().name);
  BOOST_TEST(outputRadecs.vector() == input.radecs);
  const auto outputNames = BintableIo::readColumn<SmallTable::Name>(this->fptr, input.nameCol.info().name);
  BOOST_TEST(outputNames.vector() == input.names);
  const auto outputDistsMags = BintableIo::readColumn<SmallTable::DistMag>(this->fptr, input.distMagCol.info().name);
  BOOST_TEST(outputDistsMags.vector() == input.distsMags);
}

BOOST_FIXTURE_TEST_CASE(rowwise_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  constexpr long rowCount(10000); // Large enough to ensure CFITSIO buffer is full
  RandomScalarColumn<int> i(rowCount);
  i.rename("I");
  RandomScalarColumn<float> f(rowCount);
  f.rename("F");
  RandomScalarColumn<double> d(rowCount);
  d.rename("D");
  HduAccess::assign_bintable(this->fptr, "BINEXT", i, f, d);
  const auto table = BintableIo::readColumns<int, float, double>(this->fptr, {"I", "F", "D"});
  int status = 0;
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  BOOST_TEST(chunkRows < rowCount);
  BOOST_TEST(std::get<0>(table).vector() == i.vector());
  BOOST_TEST(std::get<1>(table).vector() == f.vector());
  BOOST_TEST(std::get<2>(table).vector() == d.vector());
}

BOOST_FIXTURE_TEST_CASE(append_columns_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallTable table;
  HduAccess::assign_bintable(this->fptr, "TABLE", table.nameCol);
  const auto names = BintableIo::readColumn<SmallTable::Name>(fptr, table.nameCol.info().name);
  BOOST_TEST(names.vector() == table.names);
  BintableIo::appendColumns(fptr, table.distMagCol, table.radecCol);
  const auto distsMags = BintableIo::readColumn<SmallTable::DistMag>(fptr, table.distMagCol.info().name);
  BOOST_TEST(distsMags.vector() == table.distsMags);
  const auto radecs = BintableIo::readColumn<SmallTable::Radec>(fptr, table.radecCol.info().name);
  BOOST_TEST(radecs.vector() == table.radecs);
}

template <long N>
void checkTdimIsReadBack(fitsfile* fptr, const Fits::ColumnInfo<char, N>& info) {
  HduAccess::init_bintable(fptr, "TABLE", info);
  const bool shouldHaveTdim = (info.shape.size() > 1) || (info.shape[0] != info.repeatCount());
  BOOST_TEST(HeaderIo::hasKeyword(fptr, "TDIM1") == shouldHaveTdim);
  const auto result = BintableIo::readColumnInfo<char, N>(fptr, 1);
  BOOST_TEST((result == info));
}

BOOST_FIXTURE_TEST_CASE(tdim_is_read_back_test, Fits::Test::MinimalFile) {
  static constexpr long width = 10;
  static constexpr long height = 6;
  checkTdimIsReadBack(this->fptr, Fits::ColumnInfo<char, 1>("SCALAR"));
  checkTdimIsReadBack(this->fptr, Fits::ColumnInfo<char, 1>("VECTOR", "", width));
  checkTdimIsReadBack(this->fptr, Fits::ColumnInfo<char, 2>("MULTI", "", {width, height}));
}

BOOST_FIXTURE_TEST_CASE(tdim_for_string_learning_test, Fits::Test::MinimalFile) {
  int status = 0;
  Fits::String::CStrArray ttype({std::string("TEST")});
  Fits::String::CStrArray tform({std::string("10A")});
  int naxis = 0;
  long naxes[] = {0, 0, 0};
  fits_create_tbl(this->fptr, BINARY_TBL, 0, 1, ttype.data(), tform.data(), nullptr, "EXTNAME", &status);
  BOOST_CHECK_EQUAL(status, 0);
  int dummy = 0;
  fits_get_key_strlen(fptr, "TDIM1", &dummy, &status);
  BOOST_CHECK_EQUAL(status, KEY_NO_EXIST);
  status = 0;
  int ncols = 0;
  fits_get_num_cols(this->fptr, &ncols, &status); // Note: if this is not called, fits_read_tdim throws
  BOOST_CHECK_EQUAL(status, 0);
  BOOST_CHECK_EQUAL(ncols, 1);
  fits_read_tdim(this->fptr, 1, 3, &naxis, naxes, &status);
  BOOST_CHECK_EQUAL(status, 0);
  BOOST_CHECK_EQUAL(naxis, 1);
  BOOST_CHECK_NE(naxes[1], 10);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
