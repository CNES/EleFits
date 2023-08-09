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
void check_scalar_column_is_read_back() {
  using namespace Fits::Test;
  RandomScalarColumn<T> input;
  MinimalFile file;
  try {
    HduAccess::assign_bintable(file.fptr, "BINEXT", input);
    const auto index = BintableIo::column_index(file.fptr, input.info().name);
    BOOST_TEST(index == 1);
    const auto info = BintableIo::read_column_info<T>(file.fptr, index);
    BOOST_TEST((info == input.info()));
    const auto output = BintableIo::read_column<T>(file.fptr, input.info().name);
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
    check_scalar_column_is_read_back<type>(); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(SCALAR_COLUMN_IS_READ_BACK_TEST)

template <typename T>
void check_vector_column_is_read_back() {
  using namespace Fits::Test;
  constexpr long row_count = 3;
  constexpr long repeatCount = 2;
  RandomVectorColumn<T> input(repeatCount, row_count);
  MinimalFile file;
  try {
    HduAccess::assign_bintable(file.fptr, "BINEXT", input);
    BOOST_TEST(BintableIo::row_count(file.fptr) == row_count);
    const auto output = BintableIo::read_column<T>(file.fptr, input.info().name);
    BOOST_TEST(output.info().repeatCount() == repeatCount);
    BOOST_TEST(output.rowCount() == row_count);
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
void check_vector_column_is_read_back<std::string>();

template <>
void check_vector_column_is_read_back<std::string>() {
  // String is tested as a scalar column
}

#define VECTOR_COLUMN_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_vector_column_is_read_back_test) { \
    check_vector_column_is_read_back<type>(); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(VECTOR_COLUMN_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(small_table_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallTable input;
  HduAccess::assign_bintable(this->fptr, "IMGEXT", input.num_col, input.radec_col, input.name_col, input.dist_mag_col);
  const auto outputNums = BintableIo::read_column<SmallTable::Num>(this->fptr, input.num_col.info().name);
  BOOST_TEST(outputNums.vector() == input.nums);
  const auto outputRadecs = BintableIo::read_column<SmallTable::Radec>(this->fptr, input.radec_col.info().name);
  BOOST_TEST(outputRadecs.vector() == input.radecs);
  const auto outputNames = BintableIo::read_column<SmallTable::Name>(this->fptr, input.name_col.info().name);
  BOOST_TEST(outputNames.vector() == input.names);
  const auto outputDistsMags = BintableIo::read_column<SmallTable::DistMag>(this->fptr, input.dist_mag_col.info().name);
  BOOST_TEST(outputDistsMags.vector() == input.dists_mags);
}

BOOST_FIXTURE_TEST_CASE(rowwise_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  constexpr long row_count(10000); // Large enough to ensure CFITSIO buffer is full
  RandomScalarColumn<int> i(row_count);
  i.rename("I");
  RandomScalarColumn<float> f(row_count);
  f.rename("F");
  RandomScalarColumn<double> d(row_count);
  d.rename("D");
  HduAccess::assign_bintable(this->fptr, "BINEXT", i, f, d);
  const auto table = BintableIo::read_columns<int, float, double>(this->fptr, {"I", "F", "D"});
  int status = 0;
  long chunk_row_count = 0;
  fits_get_rowsize(fptr, &chunk_row_count, &status);
  BOOST_TEST(chunk_row_count < row_count);
  BOOST_TEST(std::get<0>(table).vector() == i.vector());
  BOOST_TEST(std::get<1>(table).vector() == f.vector());
  BOOST_TEST(std::get<2>(table).vector() == d.vector());
}

BOOST_FIXTURE_TEST_CASE(append_columns_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallTable table;
  HduAccess::assign_bintable(this->fptr, "TABLE", table.name_col);
  const auto names = BintableIo::read_column<SmallTable::Name>(fptr, table.name_col.info().name);
  BOOST_TEST(names.vector() == table.names);
  BintableIo::append_columns(fptr, table.dist_mag_col, table.radec_col);
  const auto dists_mags = BintableIo::read_column<SmallTable::DistMag>(fptr, table.dist_mag_col.info().name);
  BOOST_TEST(dists_mags.vector() == table.dists_mags);
  const auto radecs = BintableIo::read_column<SmallTable::Radec>(fptr, table.radec_col.info().name);
  BOOST_TEST(radecs.vector() == table.radecs);
}

template <long N>
void check_tdim_is_read_back(fitsfile* fptr, const Fits::ColumnInfo<char, N>& info) {
  HduAccess::init_bintable(fptr, "TABLE", info);
  const bool should_have_tdim = (info.shape.size() > 1) || (info.shape[0] != info.repeatCount());
  BOOST_TEST(HeaderIo::has_keyword(fptr, "TDIM1") == should_have_tdim);
  const auto result = BintableIo::read_column_info<char, N>(fptr, 1);
  BOOST_TEST((result == info));
}

BOOST_FIXTURE_TEST_CASE(tdim_is_read_back_test, Fits::Test::MinimalFile) {
  static constexpr long width = 10;
  static constexpr long height = 6;
  check_tdim_is_read_back(this->fptr, Fits::ColumnInfo<char, 1>("SCALAR"));
  check_tdim_is_read_back(this->fptr, Fits::ColumnInfo<char, 1>("VECTOR", "", width));
  check_tdim_is_read_back(this->fptr, Fits::ColumnInfo<char, 2>("MULTI", "", {width, height}));
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
