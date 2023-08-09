// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/CfitsioBenchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

CfitsioBenchmark::~CfitsioBenchmark() {
  fits_close_file(m_fptr, &m_status);
}

CfitsioBenchmark::CfitsioBenchmark(const std::string& filename, long chunk_row_count) :
    Benchmark(filename), m_fptr(nullptr), m_status(0), m_chunk_row_count(chunk_row_count) {
  m_logger.info() << "CFITSIO benchmark (chunk_row_count: " << chunk_row_count << ", filename: " << filename << ")";
  fits_create_file(&m_fptr, (std::string("!") + filename).c_str(), &m_status);
  fits_create_img(m_fptr, BYTE_IMG, 0, nullptr, &m_status); // Create empty Primary
  may_throw("Cannot create file");
}

long CfitsioBenchmark::chunk_row_count() const {
  return m_chunk_row_count;
}

void CfitsioBenchmark::open() {
  fits_open_file(&m_fptr, m_filename.c_str(), READWRITE, &m_status);
}

void CfitsioBenchmark::close() {
  fits_close_file(m_fptr, &m_status);
}

BChronometer::Unit CfitsioBenchmark::write_image(const BRaster& raster) {
  m_chrono.start();
  auto nonconst_shape = raster.shape();
  fits_create_img(
      m_fptr,
      Cfitsio::TypeCode<BRaster::Value>::bitpix(),
      raster.shape().size(),
      nonconst_shape.data(),
      &m_status);
  may_throw("Cannot create image HDU");
  std::vector<BRaster::Value> nonconst_data(raster.data(), raster.data() + raster.size());
  fits_write_img(
      m_fptr,
      Cfitsio::TypeCode<BRaster::Value>::forImage(),
      1,
      raster.size(),
      nonconst_data.data(),
      &m_status);
  may_throw("Cannot write image");
  return m_chrono.stop();
}

BRaster CfitsioBenchmark::read_image(long index) {
  m_chrono.start();
  int hdu_type = 0;
  fits_movabs_hdu(m_fptr, index + 1, &hdu_type, &m_status);
  Position<BRaster::Dim> shape;
  fits_get_img_size(m_fptr, BRaster::Dim, shape.data(), &m_status);
  BRaster raster(shape);
  fits_read_img(
      m_fptr,
      Cfitsio::TypeCode<BRaster::Value>::forImage(),
      1,
      raster.size(),
      nullptr,
      raster.data(),
      nullptr,
      &m_status);
  may_throw("Cannot read image");
  m_chrono.stop();
  return raster;
}

BChronometer::Unit CfitsioBenchmark::write_bintable(const BColumns& columns) {
  long row_count = std::get<0>(columns).rowCount();
  std::vector<std::string> names(row_count);
  std::vector<std::string> formats(row_count);
  std::vector<std::string> units(row_count);
  setup_column_info<0>(columns, names, formats, units);
  setup_column_info<1>(columns, names, formats, units);
  setup_column_info<2>(columns, names, formats, units);
  setup_column_info<3>(columns, names, formats, units);
  setup_column_info<4>(columns, names, formats, units);
  setup_column_info<5>(columns, names, formats, units);
  setup_column_info<6>(columns, names, formats, units);
  setup_column_info<7>(columns, names, formats, units);
  setup_column_info<8>(columns, names, formats, units);
  setup_column_info<9>(columns, names, formats, units); // TODO index_sequence
  String::CStrArray name_array(names);
  String::CStrArray format_array(formats);
  String::CStrArray unit_array(units);
  m_chrono.start();
  fits_create_tbl(
      m_fptr,
      BINARY_TBL,
      0,
      ColumnCount,
      name_array.data(),
      format_array.data(),
      unit_array.data(),
      "",
      &m_status);
  may_throw("Cannot create binary table HDU");
  long chunk_row_count = compute_chunk_row_count(row_count);
  for (long first_row = 0; first_row < row_count;) {
    const long past_last_row = std::min(first_row + chunk_row_count, row_count);
    write_column<0>(columns, first_row, past_last_row - first_row);
    write_column<1>(columns, first_row, past_last_row - first_row);
    write_column<2>(columns, first_row, past_last_row - first_row);
    write_column<3>(columns, first_row, past_last_row - first_row);
    write_column<4>(columns, first_row, past_last_row - first_row);
    write_column<5>(columns, first_row, past_last_row - first_row);
    write_column<6>(columns, first_row, past_last_row - first_row);
    write_column<7>(columns, first_row, past_last_row - first_row);
    write_column<8>(columns, first_row, past_last_row - first_row);
    write_column<9>(columns, first_row, past_last_row - first_row); // TODO index_sequence
    first_row = past_last_row;
  }
  return m_chrono.stop();
}

BColumns CfitsioBenchmark::read_bintable(long index) {
  int hdu_type = 0;
  fits_movabs_hdu(m_fptr, index + 1, &hdu_type, &m_status);
  may_throw("Cannot access HDU");
  BColumns columns;
  m_chrono.start();
  long row_count = 0;
  fits_get_num_rows(m_fptr, &row_count, &m_status);
  may_throw("Cannot read number of rows");
  init_column<0>(columns, row_count);
  init_column<1>(columns, row_count);
  init_column<2>(columns, row_count);
  init_column<3>(columns, row_count);
  init_column<4>(columns, row_count);
  init_column<5>(columns, row_count);
  init_column<6>(columns, row_count);
  init_column<7>(columns, row_count);
  init_column<8>(columns, row_count);
  init_column<9>(columns, row_count);
  long chunk_row_count = compute_chunk_row_count(row_count);
  for (long first_row = 0; first_row < row_count;) {
    const long past_last_row = std::min(first_row + chunk_row_count, row_count);
    read_column<0>(columns, first_row, past_last_row - first_row);
    read_column<1>(columns, first_row, past_last_row - first_row);
    read_column<2>(columns, first_row, past_last_row - first_row);
    read_column<3>(columns, first_row, past_last_row - first_row);
    read_column<4>(columns, first_row, past_last_row - first_row);
    read_column<5>(columns, first_row, past_last_row - first_row);
    read_column<6>(columns, first_row, past_last_row - first_row);
    read_column<7>(columns, first_row, past_last_row - first_row);
    read_column<8>(columns, first_row, past_last_row - first_row);
    read_column<9>(columns, first_row, past_last_row - first_row); // TODO index_sequence
    first_row = past_last_row;
  }
  m_chrono.stop();
  return columns;
}

long CfitsioBenchmark::compute_chunk_row_count(long row_count) {
  if (m_chunk_row_count == -1) {
    m_logger.debug() << "Row chunk size: " << row_count;
    return row_count;
  }
  if (m_chunk_row_count == 0) {
    long size = 0;
    fits_get_rowsize(m_fptr, &size, &m_status);
    may_throw("Cannot compute buffer size");
    m_logger.debug() << "Row chunk size: " << size;
    return size;
  }
  m_logger.debug() << "Row chunk size: " << m_chunk_row_count;
  return m_chunk_row_count;
}

void CfitsioBenchmark::may_throw(const std::string& context) const {
  Cfitsio::CfitsioError::may_throw(m_status, m_fptr, context);
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid
