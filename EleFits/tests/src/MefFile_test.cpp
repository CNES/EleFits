// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/ImageWrapper.h"
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
  BOOST_TEST(primary.index() == 0);
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
  const auto& ext = this->appendNullImage<float, 2>("IMG", {}, raster.shape());
  BOOST_TEST(this->hduCount() == 2); // 1 with CFITSIO
  ext.writeRaster(raster);
  BOOST_TEST(this->hduCount() == 2);
}

BOOST_FIXTURE_TEST_CASE(append_test, Test::NewMefFile) {
  Test::SmallRaster raster; // TODO RandomRaster
  const auto& ext1 = this->appendImage("IMG1", {}, raster);
  BOOST_TEST(ext1.index() == 1);
  BOOST_TEST(this->hduCount() == 2);
  this->close();
  // Reopen as read-only
  this->open(this->filename(), FileMode::Edit);
  BOOST_TEST(this->hduCount() == 2);
  const auto& ext2 = this->appendImage("IMG2", {}, raster);
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
  this->appendNullImage<float, 2>("IMG", {}, {});
  const auto& secondlyAccessedPrimary = this->primary();
  BOOST_TEST(firstlyAccessedPrimary.readName() == secondlyAccessedPrimary.readName());
}

BOOST_FIXTURE_TEST_CASE(access_single_named_hdu_test, Test::TemporaryMefFile) {
  const std::string extname = "EXT";
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
  this->appendImageHeader(extname);
  BOOST_CHECK_NO_THROW(this->access<>(extname));
  this->appendImageHeader(extname);
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
}

BOOST_FIXTURE_TEST_CASE(access_data_units_test, Test::TemporaryMefFile) {
  const Position<2> shape {2, 56};
  const ColumnInfo<char, 2> info {"COL", "unit", shape};
  this->appendNullImage<char>("IMAGE", {}, shape);
  this->appendBintableHeader("TABLE", {}, info);
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
bool isNull(T value) {
  return value == T();
}

template <>
bool isNull(float value) {
  return value != value;
}

template <>
bool isNull(double value) {
  return value != value;
}

template <typename T>
void checkAppendZeroImage(MefFile& f) {
  Position<1> shape {10};
  RecordSeq withoutBlank {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.appendNullImage<T>("ZERO", withoutBlank, shape);
  BOOST_TEST(ext.readName() == "ZERO");
  BOOST_TEST(ext.readSize() == shapeSize(shape));
  BOOST_TEST(ext.template readShape<1>() == shape);
  BOOST_TEST(not ext.header().has("BLANK"));
  BOOST_TEST(ext.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto zero = ext.raster().template read<T, 1>();
  BOOST_TEST(zero.shape() == shape);
  for (auto v : zero) {
    BOOST_TEST(isNull(v));
  }
}

template <typename T>
void checkAppendNullImage(MefFile& f) {

  if (std::is_same<T, std::uint64_t>::value) {
    return; // FIXME CFITSIO bug?
  }

  Position<1> shape {10};
  RecordSeq withBlank {{"BLANK", T(1)}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.appendNullImage<T>("NULL", withBlank, shape);
  BOOST_TEST(ext.readName() == "NULL");
  BOOST_TEST(ext.readSize() == shapeSize(shape));
  BOOST_TEST(ext.template readShape<1>() == shape);
  BOOST_TEST(ext.header().template parse<int>("NAXIS").value == 1);
  BOOST_TEST(ext.header().template parse<int>("NAXIS1").value == 10);
  BOOST_TEST(ext.header().template parse<int>("BLANK").value == 1);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto offset = ext.header().template parseOr<T>("BZERO", T());
  const auto blank = ext.raster().template read<T, 1>();
  BOOST_TEST(blank.shape() == shape);
  for (auto v : blank) {
    BOOST_TEST(v == T(1) + offset);
  }
}

template <>
void checkAppendNullImage<float>(MefFile&) {
  // Cannot use BLANK for float images
}

template <>
void checkAppendNullImage<double>(MefFile&) {
  // Cannot use BLANK for double images
}

template <typename T>
void checkAppendImage(MefFile& f) {
  Position<1> shape {10};
  Test::RandomRaster<T, 1> raster(shape);
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.appendImage("ZERO", records, raster);
  BOOST_TEST(ext.readName() == "ZERO");
  BOOST_TEST(ext.readSize() == shapeSize(shape));
  BOOST_TEST(ext.readSize() == shapeSize(shape));
  BOOST_TEST(ext.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto output = ext.raster().template read<T, 1>();
  BOOST_TEST(output.shape() == shape);
  BOOST_TEST(output.container() == raster.container());
}

#define APPEND_IMAGE_TEST(T, name) \
  BOOST_FIXTURE_TEST_CASE(append_zero_##name##_image_test, Test::TemporaryMefFile) { \
    checkAppendZeroImage<T>(*this); \
  } \
  BOOST_FIXTURE_TEST_CASE(append_null_##name##_image_test, Test::TemporaryMefFile) { \
    checkAppendNullImage<T>(*this); \
  } \
  BOOST_FIXTURE_TEST_CASE(append_##name##_image_test, Test::TemporaryMefFile) { \
    checkAppendImage<T>(*this); \
  }
ELEFITS_FOREACH_RASTER_TYPE(APPEND_IMAGE_TEST)

template <typename T>
void checkAppendNullBintable(MefFile& f) {

  if (std::is_same<T, std::uint64_t>::value) {
    return; // FIXME CFITSIO bug?
  }

  ColumnInfo<T> zero("ZERO");
  ColumnInfo<T> blank("BLANK");
  RecordSeq records {{"TNULL2", T(1)}, {"FOO", "BAR"}};
  const auto& ext = f.appendNullBintable("BINTABLE", records, 10, zero, blank);
  const auto offset = ext.header().template parseOr<T>("TZERO2", T());
  const auto rowCount = ext.readRowCount();
  BOOST_TEST(rowCount == 10);
  const auto output = ext.columns().readSeq(as<T>("ZERO"), as<T>("BLANK"));
  for (long i = 0; i < rowCount; ++i) {
    BOOST_TEST(isNull(std::get<0>(output)[i] - offset));
    const auto value1 = std::get<1>(output)[1];
    if (std::is_floating_point<T>::value) {
      BOOST_TEST(value1 != value1);
    } else {
      BOOST_TEST(value1 == T(1) + offset);
    }
  }
}

#define APPEND_BINTABLE_TEST(T, name) \
  BOOST_FIXTURE_TEST_CASE(append_null_##name##_bintable_test, Test::TemporaryMefFile) { \
    checkAppendNullBintable<T>(*this); \
  }
ELEFITS_FOREACH_RASTER_TYPE(APPEND_BINTABLE_TEST)

BOOST_FIXTURE_TEST_CASE(append_copy_test, Test::TemporaryMefFile) { // FIXME split into cases

  Test::TemporaryMefFile fileCopy;
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}}; // for images
  Gzip algo;

  /* Multi-column bintable in source MefFile */
  const ColumnInfo<char> charInfo("CHAR");
  const ColumnInfo<float> floatInfo("FLOAT");
  const auto& bintable = this->appendBintableHeader("BINTABLE", records, charInfo, floatInfo);

  /* Empty Image in source MefFile */
  const auto& emptyImage = this->appendImageHeader("EMPTY", records);
  BOOST_TEST(emptyImage.matches(HduCategory::RawImage));

  /* Random Image in source MefFile */
  Position<1> shape {10};
  Test::RandomRaster<double, 1> raster(shape);
  const auto& image = this->appendImage("IMAGE", records, raster);
  const auto input = image.raster().template read<double, 1>();
  BOOST_TEST(image.matches(HduCategory::RawImage));

  /* Same Image in source but Compressed */
  this->startCompressing(algo);
  const auto& compImage = this->appendImage("ZIMAGE", records, raster);
  BOOST_TEST(compImage.matches(HduCategory::CompressedImageExt));

  /* Copy bintable */
  const auto& bintableCopy = fileCopy.appendCopy(bintable);
  BOOST_TEST(bintableCopy.readName() == bintable.readName());
  BOOST_TEST(bintableCopy.readRowCount() == bintable.readRowCount());
  BOOST_TEST(bintableCopy.readColumnCount() == bintable.readColumnCount());
  BOOST_TEST(bintableCopy.columns().readName(0) == bintable.columns().readName(0));
  BOOST_TEST(bintableCopy.columns().readName(1) == bintable.columns().readName(1));
  BOOST_TEST(bintableCopy.header().parse<int>("FOO").value == bintable.header().parse<int>("FOO").value);
  BOOST_TEST(bintableCopy.header().parse<int>("BAR").value == bintable.header().parse<int>("BAR").value);

  /* Copy empty uncompressed to uncompressed */
  const auto& emptyCopy = fileCopy.appendCopy(emptyImage);
  BOOST_TEST(emptyCopy.readName() == emptyImage.readName());
  BOOST_TEST(emptyCopy.readSize() == emptyImage.readSize());
  BOOST_TEST(emptyCopy.header().parse<int>("FOO").value == emptyImage.header().parse<int>("FOO").value);
  BOOST_TEST(emptyCopy.header().parse<int>("BAR").value == emptyImage.header().parse<int>("BAR").value);
  BOOST_TEST(emptyCopy.matches(HduCategory::RawImage));

  /* Copy uncompressed to uncompressed */
  const auto& imageCopy = fileCopy.appendCopy(image);
  BOOST_TEST(imageCopy.readName() == image.readName());
  BOOST_TEST(imageCopy.readSize() == image.readSize());
  BOOST_TEST(imageCopy.header().parse<int>("FOO").value == image.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy.header().parse<int>("BAR").value == image.header().parse<int>("BAR").value);
  const auto output = imageCopy.raster().template read<double, 1>();
  BOOST_TEST(output.shape() == input.shape());
  BOOST_TEST(output.container() == input.container());
  BOOST_TEST(imageCopy.matches(HduCategory::RawImage));

  /* Copy empty to compressed */
  fileCopy.startCompressing(algo);
  const auto& imageCopy4 = fileCopy.appendCopy(emptyImage);
  BOOST_TEST(imageCopy4.readName() == emptyImage.readName());
  BOOST_TEST(imageCopy4.readSize() == emptyImage.readSize());
  BOOST_TEST(imageCopy4.header().parse<int>("FOO").value == emptyImage.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy4.header().parse<int>("BAR").value == emptyImage.header().parse<int>("BAR").value);
  BOOST_TEST(imageCopy4.matches(HduCategory::RawImage)); // empty images are actually NOT compressed

  /* Copy uncompressed to compressed */
  fileCopy.startCompressing(algo);
  const auto& imageCopy2 = fileCopy.appendCopy(image);
  BOOST_TEST(imageCopy2.readName() == image.readName());
  BOOST_TEST(imageCopy2.readSize() == image.readSize());
  BOOST_TEST(imageCopy2.header().parse<int>("FOO").value == image.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy2.header().parse<int>("BAR").value == image.header().parse<int>("BAR").value);
  const auto output2 = imageCopy2.raster().template read<double, 1>();
  BOOST_TEST(output2.shape() == input.shape());
  BOOST_TEST(output2.container() == input.container());
  BOOST_TEST(imageCopy2.matches(HduCategory::CompressedImageExt)); // the copy should now be compressed

  /* Copy compressed to uncompressed */
  fileCopy.stopCompressing();
  const auto& imageCopy3 = fileCopy.appendCopy(compImage);
  BOOST_TEST(imageCopy3.readName() == compImage.readName());
  BOOST_TEST(imageCopy3.readSize() == compImage.readSize());
  BOOST_TEST(imageCopy3.header().parse<int>("FOO").value == compImage.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy3.header().parse<int>("BAR").value == compImage.header().parse<int>("BAR").value);
  const auto output3 = imageCopy3.raster().template read<double, 1>();
  BOOST_TEST(output3.shape() == input.shape());
  BOOST_TEST(output3.container() == input.container());
  BOOST_TEST(imageCopy3.matches(HduCategory::RawImage)); // the copy should now be uncompressed
}

BOOST_FIXTURE_TEST_CASE(append_copy_with_strategy_test, Test::TemporaryMefFile) { // FIXME split into cases

  Test::TemporaryMefFile fileCopy;
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}}; // for images
  Gzip algo;

  /* Multi-column bintable in source MefFile */
  const ColumnInfo<char> charInfo("CHAR");
  const ColumnInfo<float> floatInfo("FLOAT");
  const auto& bintable = this->appendBintableHeader("BINTABLE", records, charInfo, floatInfo);

  /* Empty Image in source MefFile */
  const auto& emptyImage = this->appendImageHeader("EMPTY", records);
  BOOST_TEST(emptyImage.matches(HduCategory::RawImage));

  /* Random Image in source MefFile */
  Position<2> shape {2000, 3000};
  Test::RandomRaster<double, 2> raster(shape);
  const auto& image = this->appendImage("IMAGE", records, raster);
  const auto input = image.raster().template read<double, 2>();
  BOOST_TEST(image.matches(HduCategory::RawImage));

  /* Same Image in source but Compressed */
  this->strategy(Compress<Gzip>(algo));
  const auto& compImage = this->appendImage("ZIMAGE", records, raster);
  BOOST_TEST(compImage.matches(HduCategory::CompressedImageExt));

  /* Setting up compression strategy */
  fileCopy.strategy(CompressAptly::lossless());

  /* Copy bintable */
  const auto& bintableCopy = fileCopy.appendCopy(bintable);
  BOOST_TEST(bintableCopy.readName() == bintable.readName());
  BOOST_TEST(bintableCopy.readRowCount() == bintable.readRowCount());
  BOOST_TEST(bintableCopy.readColumnCount() == bintable.readColumnCount());
  BOOST_TEST(bintableCopy.columns().readName(0) == bintable.columns().readName(0));
  BOOST_TEST(bintableCopy.columns().readName(1) == bintable.columns().readName(1));
  BOOST_TEST(bintableCopy.header().parse<int>("FOO").value == bintable.header().parse<int>("FOO").value);
  BOOST_TEST(bintableCopy.header().parse<int>("BAR").value == bintable.header().parse<int>("BAR").value);

  /* Copy empty uncompressed */
  const auto& emptyCopy = fileCopy.appendCopy(emptyImage);
  BOOST_TEST(emptyCopy.readName() == emptyImage.readName());
  BOOST_TEST(emptyCopy.readSize() == emptyImage.readSize());
  BOOST_TEST(emptyCopy.header().parse<int>("FOO").value == emptyImage.header().parse<int>("FOO").value);
  BOOST_TEST(emptyCopy.header().parse<int>("BAR").value == emptyImage.header().parse<int>("BAR").value);
  BOOST_TEST(emptyCopy.matches(HduCategory::RawImage));

  /* Copy uncompressed */
  BOOST_TEST(not image.isCompressed());
  const auto& imageCopy = fileCopy.appendCopy(image);
  BOOST_TEST(imageCopy.readName() == image.readName());
  BOOST_TEST(imageCopy.readSize() == image.readSize());
  BOOST_TEST(imageCopy.header().parse<int>("FOO").value == image.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy.header().parse<int>("BAR").value == image.header().parse<int>("BAR").value);
  const auto output = imageCopy.raster().template read<double, 2>();
  BOOST_TEST(output.shape() == input.shape());
  BOOST_TEST(output.container() == input.container());
  BOOST_TEST(imageCopy.matches(HduCategory::CompressedImageExt));

  /* Copy compressed */
  BOOST_TEST(compImage.isCompressed());
  const auto& imageCopy3 = fileCopy.appendCopy(compImage);
  BOOST_TEST(imageCopy3.readName() == compImage.readName());
  BOOST_TEST(imageCopy3.readSize() == compImage.readSize());
  BOOST_TEST(imageCopy3.header().parse<int>("FOO").value == compImage.header().parse<int>("FOO").value);
  BOOST_TEST(imageCopy3.header().parse<int>("BAR").value == compImage.header().parse<int>("BAR").value);
  const auto output3 = imageCopy3.raster().template read<double, 2>();
  BOOST_TEST(output3.shape() == input.shape());
  BOOST_TEST(output3.container() == input.container());
  BOOST_TEST(imageCopy3.matches(HduCategory::CompressedImageExt));
}

// This tests the isCompressedImage function from the ImageWrapper
BOOST_FIXTURE_TEST_CASE(is_compressed_test, Test::TemporaryMefFile) {

  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};

  Position<1> shape {10};
  Test::RandomRaster<double, 1> raster(shape);

  // turning compression on:
  Gzip algo;
  this->startCompressing(algo);

  // existing primary should still be uncompressed
  BOOST_TEST(not this->primary().isCompressed());

  // added ext should be compressed
  const auto& image1 = this->appendImage("SECOND", records, raster);
  BOOST_TEST(image1.isCompressed());

  // turning compression off
  this->stopCompressing();

  // added ext should not be compressed
  const auto& image2 = this->appendImage("THIRD", records, raster);
  BOOST_TEST(not image2.isCompressed());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
