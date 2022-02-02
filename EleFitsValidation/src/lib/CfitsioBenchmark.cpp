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

CfitsioBenchmark::CfitsioBenchmark(const std::string& filename, long rowChunkSize) :
    Benchmark(filename), m_fptr(nullptr), m_status(0), m_rowChunkSize(rowChunkSize) {
  m_logger.info() << "CFITSIO benchmark (rowChunkSize: " << rowChunkSize << ", filename: " << filename << ")";
  fits_create_file(&m_fptr, (std::string("!") + filename).c_str(), &m_status);
  fits_create_img(m_fptr, BYTE_IMG, 0, nullptr, &m_status); // Create empty Primary
  mayThrow("Cannot create file");
}

long CfitsioBenchmark::rowChunkSize() const {
  return m_rowChunkSize;
}

void CfitsioBenchmark::open() {
  fits_open_file(&m_fptr, m_filename.c_str(), READWRITE, &m_status);
}

void CfitsioBenchmark::close() {
  fits_close_file(m_fptr, &m_status);
}

BChronometer::Unit CfitsioBenchmark::writeImage(const BRaster& raster) {
  m_chrono.start();
  auto nonconstShape = raster.shape();
  fits_create_img(
      m_fptr,
      Cfitsio::TypeCode<BRaster::Value>::bitpix(),
      raster.shape().size(),
      nonconstShape.data(),
      &m_status);
  mayThrow("Cannot create image HDU");
  std::vector<BRaster::Value> nonconstData(raster.data(), raster.data() + raster.size());
  fits_write_img(
      m_fptr,
      Cfitsio::TypeCode<BRaster::Value>::forImage(),
      1,
      raster.size(),
      nonconstData.data(),
      &m_status);
  mayThrow("Cannot write image");
  return m_chrono.stop();
}

BRaster CfitsioBenchmark::readImage(long index) {
  m_chrono.start();
  int hduType = 0;
  fits_movabs_hdu(m_fptr, index + 1, &hduType, &m_status);
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
  mayThrow("Cannot read image");
  m_chrono.stop();
  return raster;
}

BChronometer::Unit CfitsioBenchmark::writeBintable(const BColumns& columns) {
  long rowCount = std::get<0>(columns).rowCount();
  std::vector<std::string> names(rowCount);
  std::vector<std::string> formats(rowCount);
  std::vector<std::string> units(rowCount);
  setupColumnInfo<0>(columns, names, formats, units);
  setupColumnInfo<1>(columns, names, formats, units);
  setupColumnInfo<2>(columns, names, formats, units);
  setupColumnInfo<3>(columns, names, formats, units);
  setupColumnInfo<4>(columns, names, formats, units);
  setupColumnInfo<5>(columns, names, formats, units);
  setupColumnInfo<6>(columns, names, formats, units);
  setupColumnInfo<7>(columns, names, formats, units);
  setupColumnInfo<8>(columns, names, formats, units);
  setupColumnInfo<9>(columns, names, formats, units); // TODO index_sequence
  String::CStrArray nameArray(names);
  String::CStrArray formatArray(formats);
  String::CStrArray unitArray(units);
  m_chrono.start();
  fits_create_tbl(
      m_fptr,
      BINARY_TBL,
      0,
      columnCount,
      nameArray.data(),
      formatArray.data(),
      unitArray.data(),
      "",
      &m_status);
  mayThrow("Cannot create binary table HDU");
  long rowChunkSize = computeRowChunkSize(rowCount);
  for (long firstRow = 0; firstRow < rowCount;) {
    const long pastLastRow = std::min(firstRow + rowChunkSize, rowCount);
    writeColumn<0>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<1>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<2>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<3>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<4>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<5>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<6>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<7>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<8>(columns, firstRow, pastLastRow - firstRow);
    writeColumn<9>(columns, firstRow, pastLastRow - firstRow); // TODO index_sequence
    firstRow = pastLastRow;
  }
  return m_chrono.stop();
}

BColumns CfitsioBenchmark::readBintable(long index) {
  int hduType = 0;
  fits_movabs_hdu(m_fptr, index + 1, &hduType, &m_status);
  mayThrow("Cannot access HDU");
  BColumns columns;
  m_chrono.start();
  long rowCount = 0;
  fits_get_num_rows(m_fptr, &rowCount, &m_status);
  mayThrow("Cannot read number of rows");
  initColumn<0>(columns, rowCount);
  initColumn<1>(columns, rowCount);
  initColumn<2>(columns, rowCount);
  initColumn<3>(columns, rowCount);
  initColumn<4>(columns, rowCount);
  initColumn<5>(columns, rowCount);
  initColumn<6>(columns, rowCount);
  initColumn<7>(columns, rowCount);
  initColumn<8>(columns, rowCount);
  initColumn<9>(columns, rowCount);
  long rowChunkSize = computeRowChunkSize(rowCount);
  for (long firstRow = 0; firstRow < rowCount;) {
    const long pastLastRow = std::min(firstRow + rowChunkSize, rowCount);
    readColumn<0>(columns, firstRow, pastLastRow - firstRow);
    readColumn<1>(columns, firstRow, pastLastRow - firstRow);
    readColumn<2>(columns, firstRow, pastLastRow - firstRow);
    readColumn<3>(columns, firstRow, pastLastRow - firstRow);
    readColumn<4>(columns, firstRow, pastLastRow - firstRow);
    readColumn<5>(columns, firstRow, pastLastRow - firstRow);
    readColumn<6>(columns, firstRow, pastLastRow - firstRow);
    readColumn<7>(columns, firstRow, pastLastRow - firstRow);
    readColumn<8>(columns, firstRow, pastLastRow - firstRow);
    readColumn<9>(columns, firstRow, pastLastRow - firstRow); // TODO index_sequence
    firstRow = pastLastRow;
  }
  m_chrono.stop();
  return columns;
}

long CfitsioBenchmark::computeRowChunkSize(long rowCount) {
  if (m_rowChunkSize == -1) {
    m_logger.debug() << "Row chunk size: " << rowCount;
    return rowCount;
  }
  if (m_rowChunkSize == 0) {
    long size = 0;
    fits_get_rowsize(m_fptr, &size, &m_status);
    mayThrow("Cannot compute buffer size");
    m_logger.debug() << "Row chunk size: " << size;
    return size;
  }
  m_logger.debug() << "Row chunk size: " << m_rowChunkSize;
  return m_rowChunkSize;
}

void CfitsioBenchmark::mayThrow(const std::string& context) const {
  Cfitsio::CfitsioError::mayThrow(m_status, m_fptr, context);
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid
