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

BOOST_FIXTURE_TEST_CASE(primary_index_is_consistent_test, Test::TemporaryMefFile)
{
  const auto& primary = this->primary();
  BOOST_TEST(primary.index() == 0);
}

BOOST_FIXTURE_TEST_CASE(primary_resize_test, Test::NewMefFile)
{
  Test::SmallRaster input; // TODO RandomRaster
  const auto& primary = this->primary();
  primary.update_type_shape<float, 2>(input.shape());
  primary.write_raster(input);
  this->close();
  // Reopen as read-only
  MefFile f(this->filename(), FileMode::Read);
  const auto output = f.primary().read_raster<float, 2>();
  std::remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(count_test, Test::TemporaryMefFile)
{
  BOOST_TEST(this->hdu_count() == 1); // 0 with CFITSIO
  Test::SmallRaster raster;
  const auto& primary = this->primary();
  primary.update_type_shape<float, 2>(raster.shape());
  BOOST_TEST(this->hdu_count() == 1);
  const auto& ext = this->append_null_image<float, 2>("IMG", {}, raster.shape());
  BOOST_TEST(this->hdu_count() == 2); // 1 with CFITSIO
  ext.write_raster(raster);
  BOOST_TEST(this->hdu_count() == 2);
}

BOOST_FIXTURE_TEST_CASE(append_test, Test::NewMefFile)
{
  Test::SmallRaster raster; // TODO RandomRaster
  const auto& ext1 = this->append_image("IMG1", {}, raster);
  BOOST_TEST(ext1.index() == 1);
  BOOST_TEST(this->hdu_count() == 2);
  this->close();
  // Reopen as edit
  MefFile f(this->filename(), FileMode::Edit);
  BOOST_TEST(f.hdu_count() == 2);
  const auto& ext2 = f.append_image("IMG2", {}, raster);
  BOOST_TEST(ext2.index() == 2);
  BOOST_TEST(f.hdu_count() == 3);
  std::vector<std::string> input_names {"", "IMG1", "IMG2"};
  const auto output_names = f.read_hdu_names();
  BOOST_TEST(output_names == input_names);
  std::remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(remove_primary_test, Test::TemporaryMefFile)
{
  Test::SmallRaster raster;
  this->append_image("IMAGE", {{"KEY", 1}}, raster);
  this->append_image_header("EXT", {});
  this->remove(0);
  BOOST_TEST(this->hdu_count() == 2);
  BOOST_TEST(this->primary().read_name() == "IMAGE");
  BOOST_TEST(this->primary().header().parse<int>("KEY").value == 1);
  BOOST_TEST(this->primary().raster().read<float>() == raster);
  const auto& ext = this->find("EXT");
  BOOST_TEST(ext.index() == 1);
}

BOOST_FIXTURE_TEST_CASE(reaccess_hdu_and_use_previous_reference_test, Test::TemporaryMefFile)
{
  const auto& firstly_accessed_primary = this->primary();
  BOOST_CHECK_NO_THROW(firstly_accessed_primary.read_name());
  this->append_null_image<float, 2>("IMG", {}, {});
  const auto& secondly_accessed_primary = this->primary();
  BOOST_TEST(firstly_accessed_primary.read_name() == secondly_accessed_primary.read_name());
}

BOOST_FIXTURE_TEST_CASE(access_single_named_hdu_test, Test::TemporaryMefFile)
{
  const std::string extname = "EXT";
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
  this->append_image_header(extname);
  BOOST_CHECK_NO_THROW(this->access<>(extname));
  this->append_image_header(extname);
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
}

BOOST_FIXTURE_TEST_CASE(access_data_units_test, Test::TemporaryMefFile)
{
  const Position<2> shape {2, 56};
  const ColumnInfo<char, 2> info {"COL", "unit", shape};
  this->append_null_image<char>("IMAGE", {}, shape);
  this->append_bintable_header("TABLE", {}, info);
  BOOST_TEST(this->access<Header>(1).has("NAXIS"));
  BOOST_TEST(this->access<Header>("IMAGE").has("NAXIS"));
  BOOST_TEST(this->access<ImageRaster>(1).read_shape() == shape);
  BOOST_TEST(this->access<ImageRaster>("IMAGE").read_shape() == shape);
  BOOST_TEST(this->access<BintableColumns>(2).read_name(0) == info.name);
  BOOST_TEST(this->access<BintableColumns>("TABLE").read_name(0) == info.name);
}

BOOST_FIXTURE_TEST_CASE(append_header_test, Test::TemporaryMefFile)
{
  /* Image */
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& image = this->append_image_header("IMAGE", records);
  BOOST_TEST(image.read_name() == "IMAGE");
  BOOST_TEST(image.read_size() == 0);
  BOOST_TEST(image.header().parse<int>("FOO").value == 3);
  BOOST_TEST(image.header().parse<int>("BAR").value == 41);

  /* No-column bintable */
  const auto& bintable0 = this->append_bintable_header("BINTABLE0", records);
  BOOST_TEST(bintable0.read_name() == "BINTABLE0");
  BOOST_TEST(bintable0.read_row_count() == 0);
  BOOST_TEST(bintable0.read_column_count() == 0);
  BOOST_TEST(bintable0.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable0.header().parse<int>("BAR").value == 41);

  /* Single-column bintable */
  const ColumnInfo<char> char_info("CHAR");
  const auto& bintable1 = this->append_bintable_header("BINTABLE1", records, char_info);
  BOOST_TEST(bintable1.read_name() == "BINTABLE1");
  BOOST_TEST(bintable1.read_row_count() == 0);
  BOOST_TEST(bintable1.read_column_count() == 1);
  BOOST_TEST(bintable1.columns().read_name(0) == "CHAR");
  BOOST_TEST(bintable1.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable1.header().parse<int>("BAR").value == 41);
  // FIXME check with tuple

  /* Multi-column bintable */
  const ColumnInfo<float> float_info("FLOAT");
  const auto& bintable2 = this->append_bintable_header("BINTABLE2", records, char_info, float_info);
  BOOST_TEST(bintable2.read_name() == "BINTABLE2");
  BOOST_TEST(bintable2.read_row_count() == 0);
  BOOST_TEST(bintable2.read_column_count() == 2);
  BOOST_TEST(bintable2.columns().read_name(0) == "CHAR");
  BOOST_TEST(bintable2.columns().read_name(1) == "FLOAT");
  BOOST_TEST(bintable2.header().parse<int>("FOO").value == 3);
  BOOST_TEST(bintable2.header().parse<int>("BAR").value == 41);
  // FIXME check with tuple
}

template <typename T>
bool is_null(T value)
{
  return value == T();
}

template <>
bool is_null(float value)
{
  return value != value;
}

template <>
bool is_null(double value)
{
  return value != value;
}

template <typename T>
void check_append_zero_image(MefFile& f)
{
  Position<1> shape {10};
  RecordSeq without_blank {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.append_null_image<T>("ZERO", without_blank, shape);
  BOOST_TEST(ext.read_name() == "ZERO");
  BOOST_TEST(ext.read_size() == shape_size(shape));
  BOOST_TEST(ext.template read_shape<1>() == shape);
  BOOST_TEST(not ext.header().has("BLANK"));
  BOOST_TEST(ext.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto zero = ext.raster().template read<T, 1>();
  BOOST_TEST(zero.shape() == shape);
  for (auto v : zero) {
    BOOST_TEST(is_null(v));
  }
}

template <typename T>
void check_append_null_image(MefFile& f)
{
  if (std::is_same<T, std::uint64_t>::value) {
    return; // FIXME CFITSIO bug?
  }

  Position<1> shape {10};
  RecordSeq with_blank {{"BLANK", T(1)}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.append_null_image<T>("NULL", with_blank, shape);
  BOOST_TEST(ext.read_name() == "NULL");
  BOOST_TEST(ext.read_size() == shape_size(shape));
  BOOST_TEST(ext.template read_shape<1>() == shape);
  BOOST_TEST(ext.header().template parse<int>("NAXIS").value == 1);
  BOOST_TEST(ext.header().template parse<int>("NAXIS1").value == 10);
  BOOST_TEST(ext.header().template parse<int>("BLANK").value == 1);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto offset = ext.header().template parse_or<T>("BZERO", T());
  const auto blank = ext.raster().template read<T, 1>();
  BOOST_TEST(blank.shape() == shape);
  for (auto v : blank) {
    BOOST_TEST(v == T(1) + offset);
  }
}

template <>
void check_append_null_image<float>(MefFile&)
{
  // Cannot use BLANK for float images
}

template <>
void check_append_null_image<double>(MefFile&)
{
  // Cannot use BLANK for double images
}

template <typename T>
void check_append_image(MefFile& f)
{
  Position<1> shape {10};
  Test::RandomRaster<T, 1> raster(shape);
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}};
  const auto& ext = f.append_image("ZERO", records, raster);
  BOOST_TEST(ext.read_name() == "ZERO");
  BOOST_TEST(ext.read_size() == shape_size(shape));
  BOOST_TEST(ext.read_size() == shape_size(shape));
  BOOST_TEST(ext.header().template parse<int>("FOO").value == 3);
  BOOST_TEST(ext.header().template parse<int>("BAR").value == 41);
  const auto output = ext.raster().template read<T, 1>();
  BOOST_TEST(output.shape() == shape);
  BOOST_TEST(output.container() == raster.container());
}

#define APPEND_IMAGE_TEST(T, name) \
  BOOST_FIXTURE_TEST_CASE(append_zero_##name##_image_test, Test::TemporaryMefFile) \
  { \
    check_append_zero_image<T>(*this); \
  } \
  BOOST_FIXTURE_TEST_CASE(append_null_##name##_image_test, Test::TemporaryMefFile) \
  { \
    check_append_null_image<T>(*this); \
  } \
  BOOST_FIXTURE_TEST_CASE(append_##name##_image_test, Test::TemporaryMefFile) \
  { \
    check_append_image<T>(*this); \
  }
ELEFITS_FOREACH_RASTER_TYPE(APPEND_IMAGE_TEST)

template <typename T>
void check_append_null_bintable(MefFile& f)
{
  if (std::is_same<T, std::uint64_t>::value) {
    return; // FIXME CFITSIO bug?
  }

  ColumnInfo<T> zero("ZERO");
  ColumnInfo<T> blank("BLANK");
  RecordSeq records {{"TNULL2", T(1)}, {"FOO", "BAR"}};
  const auto& ext = f.append_null_bintable("BINTABLE", records, 10, zero, blank);
  const auto offset = ext.header().template parse_or<T>("TZERO2", T());
  const auto row_count = ext.read_row_count();
  BOOST_TEST(row_count == 10);
  const auto output = ext.columns().read_n(as<T>("ZERO"), as<T>("BLANK"));
  for (long i = 0; i < row_count; ++i) {
    BOOST_TEST(is_null(std::get<0>(output)[i] - offset));
    const auto value1 = std::get<1>(output)[1];
    if (std::is_floating_point<T>::value) {
      BOOST_TEST(value1 != value1);
    } else {
      BOOST_TEST(value1 == T(1) + offset);
    }
  }
}

#define APPEND_BINTABLE_TEST(T, name) \
  BOOST_FIXTURE_TEST_CASE(append_null_##name##_bintable_test, Test::TemporaryMefFile) \
  { \
    check_append_null_bintable<T>(*this); \
  }
ELEFITS_FOREACH_RASTER_TYPE(APPEND_BINTABLE_TEST)

BOOST_FIXTURE_TEST_CASE(append_copy_test, Test::TemporaryMefFile)
{ // FIXME split into cases

  Test::TemporaryMefFile file_copy;
  RecordSeq records {{"FOO", 3.14}, {"BAR", 41, "s", "useless"}}; // for images
  Gzip algo;

  /* Multi-column bintable in source MefFile */
  const ColumnInfo<char> char_info("CHAR");
  const ColumnInfo<float> float_info("FLOAT");
  const auto& bintable = this->append_bintable_header("BINTABLE", records, char_info, float_info);

  /* Empty Image in source MefFile */
  const auto& empty_image = this->append_image_header("EMPTY", records);
  BOOST_TEST(empty_image.matches(HduCategory::RawImage));

  /* Random Image in source MefFile */
  Position<1> shape {2881}; // More than a block
  Test::RandomRaster<double, 1> raster(shape);
  const auto& image = this->append_image("IMAGE", records, raster);
  const auto input = image.raster().template read<double, 1>();
  BOOST_TEST(image.matches(HduCategory::RawImage));

  /* Same Image in source but Compressed */
  this->strategy(algo);
  const auto& comp_image = this->append_image("ZIMAGE", records, raster);
  BOOST_TEST(comp_image.matches(HduCategory::CompressedImageExt));

  /* Copy bintable */
  const auto& bintable_copy = file_copy.append(bintable);
  BOOST_TEST(bintable_copy.read_name() == bintable.read_name());
  BOOST_TEST(bintable_copy.read_row_count() == bintable.read_row_count());
  BOOST_TEST(bintable_copy.read_column_count() == bintable.read_column_count());
  BOOST_TEST(bintable_copy.columns().read_name(0) == bintable.columns().read_name(0));
  BOOST_TEST(bintable_copy.columns().read_name(1) == bintable.columns().read_name(1));
  BOOST_TEST(bintable_copy.header().parse<int>("FOO").value == bintable.header().parse<int>("FOO").value);
  BOOST_TEST(bintable_copy.header().parse<int>("BAR").value == bintable.header().parse<int>("BAR").value);

  /* Copy empty uncompressed to uncompressed */
  const auto& empty_copy = file_copy.append(empty_image);
  BOOST_TEST(empty_copy.read_name() == empty_image.read_name());
  BOOST_TEST(empty_copy.read_size() == empty_image.read_size());
  BOOST_TEST(empty_copy.header().parse<int>("FOO").value == empty_image.header().parse<int>("FOO").value);
  BOOST_TEST(empty_copy.header().parse<int>("BAR").value == empty_image.header().parse<int>("BAR").value);
  BOOST_TEST(empty_copy.matches(HduCategory::RawImage));

  /* Copy uncompressed to uncompressed */
  const auto& image_copy = file_copy.append(image);
  BOOST_TEST(image_copy.read_name() == image.read_name());
  BOOST_TEST(image_copy.read_size() == image.read_size());
  BOOST_TEST(image_copy.header().parse<int>("FOO").value == image.header().parse<int>("FOO").value);
  BOOST_TEST(image_copy.header().parse<int>("BAR").value == image.header().parse<int>("BAR").value);
  const auto output = image_copy.raster().template read<double, 1>();
  BOOST_TEST(output.shape() == input.shape());
  BOOST_TEST(output.container() == input.container());
  BOOST_TEST(image_copy.matches(HduCategory::RawImage));

  /* Copy empty to compressed */
  file_copy.strategy().clear();
  file_copy.strategy(algo);
  const auto& image_copy4 = file_copy.append(empty_image);
  BOOST_TEST(image_copy4.read_name() == empty_image.read_name());
  BOOST_TEST(image_copy4.read_size() == empty_image.read_size());
  BOOST_TEST(image_copy4.header().parse<int>("FOO").value == empty_image.header().parse<int>("FOO").value);
  BOOST_TEST(image_copy4.header().parse<int>("BAR").value == empty_image.header().parse<int>("BAR").value);
  BOOST_TEST(image_copy4.matches(HduCategory::RawImage)); // empty images are actually NOT compressed

  /* Copy uncompressed to compressed */
  file_copy.strategy().clear();
  file_copy.strategy(algo);
  const auto& image_copy2 = file_copy.append(image);
  BOOST_TEST(image_copy2.read_name() == image.read_name());
  BOOST_TEST(image_copy2.read_size() == image.read_size());
  BOOST_TEST(image_copy2.header().parse<int>("FOO").value == image.header().parse<int>("FOO").value);
  BOOST_TEST(image_copy2.header().parse<int>("BAR").value == image.header().parse<int>("BAR").value);
  const auto output2 = image_copy2.raster().template read<double, 1>();
  BOOST_TEST(output2.shape() == input.shape());
  BOOST_TEST(output2.container() == input.container());
  BOOST_TEST(image_copy2.matches(HduCategory::CompressedImageExt)); // the copy should now be compressed

  /* Copy compressed to uncompressed */
  file_copy.strategy().clear();
  const auto& image_copy3 = file_copy.append(comp_image);
  BOOST_TEST(image_copy3.read_name() == comp_image.read_name());
  BOOST_TEST(image_copy3.read_size() == comp_image.read_size());
  BOOST_TEST(image_copy3.header().parse<int>("FOO").value == comp_image.header().parse<int>("FOO").value);
  BOOST_TEST(image_copy3.header().parse<int>("BAR").value == comp_image.header().parse<int>("BAR").value);
  const auto output3 = image_copy3.raster().template read<double, 1>();
  BOOST_TEST(output3.shape() == input.shape());
  BOOST_TEST(output3.container() == input.container());
  BOOST_TEST(image_copy3.matches(HduCategory::RawImage)); // the copy should now be uncompressed
}

void check_append_copy(bool zin, bool zout)
{
  VecRaster<float> raster({2881, 1});
  Test::TemporaryMefFile in;
  Test::TemporaryMefFile out;

  if (zin) {
    in.strategy(Gzip());
  }

  const auto& image = in.append_image("", {}, raster);
  BOOST_TEST(image.is_compressed() == zin);
  BOOST_TEST(image.matches(HduCategory::CompressedImageExt) == zin);
  const auto& blank = in.append_null_image<float>("", {}, raster.shape());
  BOOST_TEST(blank.is_compressed() == zin);
  BOOST_TEST(blank.matches(HduCategory::CompressedImageExt) == zin);

  if (zout) {
    out.strategy(ShuffledGzip());
  }

  const auto& image_copy = out.append(image);
  BOOST_TEST(image_copy.is_compressed() == zout);
  BOOST_TEST(image_copy.matches(HduCategory::CompressedImageExt) == zout);
  const auto& blank_copy = out.append(blank);
  BOOST_TEST(blank_copy.is_compressed() == zout);
  BOOST_TEST(blank_copy.matches(HduCategory::CompressedImageExt) == zout);

  BOOST_TEST(image_copy.raster().read<float>() == raster);
  BOOST_TEST(blank_copy.raster().read<float>().shape() == raster.shape());
}

BOOST_AUTO_TEST_CASE(copy_raw_to_raw_test)
{
  check_append_copy(false, false);
}

BOOST_AUTO_TEST_CASE(copy_raw_to_compressed_test)
{
  check_append_copy(false, true);
}

BOOST_AUTO_TEST_CASE(copy_compressed_to_compressed_test)
{
  check_append_copy(true, true);
}

BOOST_AUTO_TEST_CASE(copy_compressed_to_raw_test)
{
  check_append_copy(true, false);
}

// This tests the is_compressed function from the ImageWrapper
BOOST_FIXTURE_TEST_CASE(is_compressed_test, Test::TemporaryMefFile)
{
  Test::RandomRaster<double, 1> raster({2881});

  this->strategy(Gzip());
  BOOST_TEST(not this->primary().is_compressed());

  const auto& image1 = this->append_image("", {}, raster);
  BOOST_TEST(image1.is_compressed());

  this->strategy().clear();
  const auto& image2 = this->append_image("", {}, raster);
  BOOST_TEST(not image2.is_compressed());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
