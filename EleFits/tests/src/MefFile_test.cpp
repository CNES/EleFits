// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>
#include <cstdio>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(MefFile_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(primary_index_is_consistent_test, Test::TemporaryMefFile) {
  const auto& primary = this->primary();
  BOOST_TEST(primary.index() == MefFile::primaryIndex);
}

BOOST_FIXTURE_TEST_CASE(primary_resize_test, Test::NewMefFile) {
  Test::SmallRaster input; // TODO RandomRaster
  const auto& primary = this->primary();
  primary.updateShape<float, 2>(input.shape());
  primary.writeRaster(input);
  this->close();
  // Reopen as read-only
  this->open(this->filename(), FileMode::Read);
  const auto output = this->primary().readRaster<float, 2>();
  remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(count_test, Test::TemporaryMefFile) {
  BOOST_TEST(this->hduCount() == 1); // 0 with CFITSIO
  Test::SmallRaster raster;
  const auto& primary = this->primary();
  primary.updateShape<float, 2>(raster.shape());
  BOOST_TEST(this->hduCount() == 1);
  const auto& ext = this->initImageExt<float, 2>("IMG", raster.shape());
  BOOST_TEST(this->hduCount() == 2); // 1 with CFITSIO
  ext.writeRaster(raster);
  BOOST_TEST(this->hduCount() == 2);
}

BOOST_FIXTURE_TEST_CASE(append_test, Test::NewMefFile) {
  Test::SmallRaster raster; // TODO RandomRaster
  const auto& ext1 = this->assignImageExt("IMG1", raster);
  BOOST_TEST(ext1.index() == 1);
  BOOST_TEST(this->hduCount() == 2);
  this->close();
  // Reopen as read-only
  this->open(this->filename(), FileMode::Edit);
  BOOST_TEST(this->hduCount() == 2);
  const auto& ext2 = this->assignImageExt("IMG2", raster);
  BOOST_TEST(ext2.index() == 2);
  BOOST_TEST(this->hduCount() == 3);
  std::vector<std::string> inputNames {"", "IMG1", "IMG2"};
  const auto outputNames = this->readHduNames();
  BOOST_TEST(outputNames == inputNames);
  remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(reaccess_hdu_and_use_previous_reference_test, Test::TemporaryMefFile) {
  const auto& firstlyAccessedPrimary = this->primary();
  BOOST_CHECK_NO_THROW(firstlyAccessedPrimary.readName());
  this->initImageExt<float, 2>("IMG", {});
  const auto& secondlyAccessedPrimary = this->primary();
  BOOST_TEST(firstlyAccessedPrimary.readName() == secondlyAccessedPrimary.readName());
}

BOOST_FIXTURE_TEST_CASE(access_single_named_hdu_test, Test::TemporaryMefFile) {
  const std::string extname = "EXT";
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
  this->initRecordExt(extname);
  BOOST_CHECK_NO_THROW(this->access<>(extname));
  this->initRecordExt(extname);
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
}

BOOST_FIXTURE_TEST_CASE(access_data_units_test, Test::TemporaryMefFile) {
  const Position<2> shape {2, 56};
  const ColumnInfo<char, 2> info {"COL", "unit", shape};
  this->initImageExt<char>("IMAGE", shape);
  this->initBintableExt("TABLE", info);
  BOOST_TEST(this->access<Header>(1).has("NAXIS"));
  BOOST_TEST(this->access<Header>("IMAGE").has("NAXIS"));
  BOOST_TEST(this->access<ImageRaster>(1).readShape() == shape);
  BOOST_TEST(this->access<ImageRaster>("IMAGE").readShape() == shape);
  BOOST_TEST(this->access<BintableColumns>(2).readName(0) == info.name);
  BOOST_TEST(this->access<BintableColumns>("TABLE").readName(0) == info.name);
}

BOOST_FIXTURE_TEST_CASE(append_header_test, Test::TemporaryMefFile) {
  /* Image */
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& image = this->appendImageHeader("IMAGE", records);
  BOOST_TEST(image.readName() == "IMAGE");
  BOOST_TEST(image.readSize() == 0);
  BOOST_TEST(image.header().parse<int>("FOO").value == 3);
  BOOST_TEST(image.header().parse<int>("BAR").value == 41);

  /* No-column bintable */
  const auto& bintable0 = this->appendBintableHeader("BINTABLE0", records);
  BOOST_TEST(bintable0.readName() == "BINTABLE0");
  BOOST_TEST(bintable0.readRowCount() == 0);
  BOOST_TEST(bintable0.readColumnCount() == 0);
  BOOST_TEST(bintable0.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable0.header().parse<int>("BAR").value == 41);

  /* Single-column bintable */
  const ColumnInfo<char> charInfo("CHAR");
  const auto& bintable1 = this->appendBintableHeader("BINTABLE1", records, charInfo);
  BOOST_TEST(bintable1.readName() == "BINTABLE1");
  BOOST_TEST(bintable1.readRowCount() == 0);
  BOOST_TEST(bintable1.readColumnCount() == 1);
  BOOST_TEST(bintable1.columns().readName(0) == "CHAR");
  BOOST_TEST(bintable1.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable1.header().parse<int>("BAR").value == 41);
  // FIXME check with tuple

  /* Multi-column bintable */
  const ColumnInfo<float> floatInfo("FLOAT");
  const auto& bintable2 = this->appendBintableHeader("BINTABLE2", records, charInfo, floatInfo);
  BOOST_TEST(bintable2.readName() == "BINTABLE2");
  BOOST_TEST(bintable2.readRowCount() == 0);
  BOOST_TEST(bintable2.readColumnCount() == 2);
  BOOST_TEST(bintable2.columns().readName(0) == "CHAR");
  BOOST_TEST(bintable2.columns().readName(1) == "FLOAT");
  BOOST_TEST(bintable2.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable2.header().parse<int>("BAR").value == 41);
  // FIXME check with tuple
}

template <typename T>
bool isNull(T value, T blank) {
  return value == blank;
}

template <>
bool isNull(float value, float) {
  return value != value;
}

template <>
bool isNull(double value, double) {
  return value != value;
}

template <typename T>
bool isNull(T value) {
  return isNull(value, T());
}

template <typename T>
void checkAppendNullImage(MefFile& f) {

  /* Without BLANK keyword */
  Position<1> shape {10};
  RecordSeq withoutBlank {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& image0 = f.appendNullImage<T>("ZERO", withoutBlank, shape);
  BOOST_TEST(image0.readName() == "ZERO");
  BOOST_TEST(image0.readSize() == shapeSize(shape));
  BOOST_TEST(image0.template readShape<1>() == shape);
  BOOST_TEST(image0.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(image0.header().template parse<int>("BAR").value == 41);
  const auto zero = image0.raster().template read<T, 1>();
  BOOST_TEST(zero.shape() == shape);
  for (auto v : zero) {
    BOOST_TEST(isNull(v));
  }

  /* With BLANK keyword */
  RecordSeq withBlank {{"BLANK", T(1)}, {"BAR", 41, "s", "useless"}};
  const auto& image1 = f.appendNullImage<T>("ZERO", withBlank, shape);
  BOOST_TEST(image1.readName() == "ZERO");
  BOOST_TEST(image1.readSize() == shapeSize(shape));
  BOOST_TEST(image1.template readShape<1>() == shape);
  BOOST_TEST(image1.header().template parse<int>("NAXIS").value == 1);
  BOOST_TEST(image1.header().template parse<int>("NAXIS1").value == 10);
  BOOST_TEST(image1.header().template parse<int>("BLANK").value == 1);
  BOOST_TEST(image1.header().template parse<int>("BAR").value == 41);
  const auto blank = image1.raster().template read<T, 1>();
  BOOST_TEST(blank.shape() == shape);
  for (auto v : blank) {
    BOOST_TEST(isNull(v, T(1)));
  }
}

template <typename T>
void checkAppendImage(MefFile& f) {

  /* Without BLANK keyword */
  Position<1> shape {10};
  Test::RandomRaster<T, 1> raster(shape);
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& image = f.appendImage("ZERO", records, raster);
  BOOST_TEST(image.readName() == "ZERO");
  BOOST_TEST(image.readSize() == shapeSize(shape));
  BOOST_TEST(image.readSize() == shapeSize(shape));
  BOOST_TEST(image.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(image.header().template parse<int>("BAR").value == 41);
  const auto output = image.raster().template read<T, 1>();
  BOOST_TEST(output.shape() == shape);
  BOOST_TEST(output.container() == raster.container());
}

#define APPEND_IMAGE_TEST(T, name) \
  BOOST_FIXTURE_TEST_CASE(append_null_##name##_image_test, Test::TemporaryMefFile) { \
    checkAppendNullImage<T>(*this); \
  } \
  BOOST_FIXTURE_TEST_CASE(append_##name##_image_test, Test::TemporaryMefFile) { \
    checkAppendImage<T>(*this); \
  }
// ELEFITS_FOREACH_RASTER_TYPE(APPEND_NULL_IMAGE_TEST)
APPEND_IMAGE_TEST(float, float)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
