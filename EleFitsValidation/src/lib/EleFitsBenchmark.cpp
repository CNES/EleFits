// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/EleFitsBenchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

EleFitsColwiseBenchmark::EleFitsColwiseBenchmark(const std::string& filename) :
    Benchmark(filename), m_f(filename, FileMode::Overwrite) {
  m_logger.info() << "EleFits benchmark (column-wise, filename: " << filename << ")";
}

void EleFitsColwiseBenchmark::open() {
  m_f.reopen();
}

void EleFitsColwiseBenchmark::close() {
  m_f.close();
}

BChronometer::Unit EleFitsColwiseBenchmark::write_bintable(const BColumns& columns) {
  m_chrono.start();
  const auto& ext = m_f.append_bintable_header(
      "",
      {},
      std::get<0>(columns).info(),
      std::get<1>(columns).info(),
      std::get<2>(columns).info(),
      std::get<3>(columns).info(),
      std::get<4>(columns).info(),
      std::get<5>(columns).info(),
      std::get<6>(columns).info(),
      std::get<7>(columns).info(),
      std::get<8>(columns).info(),
      std::get<9>(columns).info());
  ext.write_column(std::get<0>(columns));
  ext.write_column(std::get<1>(columns));
  ext.write_column(std::get<2>(columns));
  ext.write_column(std::get<3>(columns));
  ext.write_column(std::get<4>(columns));
  ext.write_column(std::get<5>(columns));
  ext.write_column(std::get<6>(columns));
  ext.write_column(std::get<7>(columns));
  ext.write_column(std::get<8>(columns));
  ext.write_column(std::get<9>(columns));
  return m_chrono.stop();
}

BColumns EleFitsColwiseBenchmark::read_bintable(long index) {
  m_chrono.start();
  const auto& ext = m_f.access<BintableHdu>(index);
  const auto columns = std::make_tuple(
      ext.read_column<std::tuple_element<0, BColumns>::type::Value>(0),
      ext.read_column<std::tuple_element<1, BColumns>::type::Value>(1),
      ext.read_column<std::tuple_element<2, BColumns>::type::Value>(2),
      ext.read_column<std::tuple_element<3, BColumns>::type::Value>(3),
      ext.read_column<std::tuple_element<4, BColumns>::type::Value>(4),
      ext.read_column<std::tuple_element<5, BColumns>::type::Value>(5),
      ext.read_column<std::tuple_element<6, BColumns>::type::Value>(6),
      ext.read_column<std::tuple_element<7, BColumns>::type::Value>(7),
      ext.read_column<std::tuple_element<8, BColumns>::type::Value>(8),
      ext.read_column<std::tuple_element<9, BColumns>::type::Value>(9));
  m_chrono.stop();
  return columns;
}

EleFitsBenchmark::EleFitsBenchmark(const std::string& filename) : EleFitsColwiseBenchmark(filename) {
  m_logger.info() << "EleFits benchmark (buffered, filename: " << filename << ")";
}

BChronometer::Unit EleFitsBenchmark::write_image(const BRaster& raster) {
  m_chrono.start();
  m_f.append_image("", {}, raster);
  return m_chrono.stop();
}

BChronometer::Unit EleFitsBenchmark::write_bintable(const BColumns& columns) {
  m_chrono.start();
  m_f.append_bintable("", {}, columns);
  return m_chrono.stop();
}

BRaster EleFitsBenchmark::read_image(long index) {
  m_chrono.start();
  const auto raster = m_f.access<ImageHdu>(index).read_raster<BRaster::Value, BRaster::Dim>();
  m_chrono.stop();
  return raster;
}

BColumns EleFitsBenchmark::read_bintable(long index) {
  m_chrono.start();
  const auto columns = m_f.access<BintableColumns>(index).read_n(
      col_indexed<0>(),
      col_indexed<1>(),
      col_indexed<2>(),
      col_indexed<3>(),
      col_indexed<4>(),
      col_indexed<5>(),
      col_indexed<6>(),
      col_indexed<7>(),
      col_indexed<8>(),
      col_indexed<9>());
  m_chrono.stop();
  return columns;
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid
