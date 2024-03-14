// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "Linx/Run/ProgramOptions.h"

#include <map>
#include <string>

using namespace Euclid;
using namespace Fits;

/**
 * @brief Generate a random scalar column without unit.
 */
template <typename T>
VecColumn<T> random_column(const std::string& name, Linx::Index rows)
{
  return VecColumn<T>({name, "", 1}, Test::generate_random_vector<T>(rows, T(0), T(1)));
}

/**
 * @brief Append a 2D-MASS-like binary table extension to a file.
 * @details
 * Random columns of type double ('D') and float ('E') are generated and written.
 */
void write_bintable(const std::string& filename, Linx::Index rows)
{
  MefFile f(filename, FileMode::Overwrite);
  const auto col1 = random_column<double>("SHE_LENSMC_UPDATED_RA", rows);
  const auto col2 = random_column<double>("SHE_LENSMC_UPDATED_DEC", rows);
  const auto col3 = random_column<float>("SHE_LENSMC_G1", rows);
  const auto col4 = random_column<float>("SHE_LENSMC_G2", rows);
  const auto col5 = random_column<float>("PHZ_MEDIAN", rows);
  const auto col6 = random_column<float>("PHZ_LENSMC_CORRECTION", rows);
  const auto col7 = random_column<float>("SHE_LENSMC_WEIGHT", rows);
  f.append_bintable("", {}, col1, col2, col3, col4, col5, col6, col7); // Unnamed extension
}

/**
 * @brief Write some records to given HDU.
 * @details
 * WCS shows examples of records of different types (int and string), with and without units.
 * We rely on VariantValue, but it would be possible to skip this abstraction and go with raw types
 * using a tuple instead of a vector.
 */
void write_some_records(const Header& header)
{
  std::vector<Record<VariantValue>> records = {
      {"WCSAXES", 2, "", "Number of axes in World Coordinate System"},
      {"CRPIX1", "", "", "Pixel coordinate of reference point"},
      {"CRPIX2", "", "", "Pixel coordinate of reference point"},
      {"PC1_1", 0, "", "Coordinate transformation matrix element"},
      {"PC1_2", 0, "", "Coordinate transformation matrix element"},
      {"PC2_1", 0, "", "Coordinate transformation matrix element"},
      {"PC2_2", 0, "", "Coordinate transformation matrix element"},
      {"CDELT1", "", "deg", "Coordinate increment at reference point"},
      {"CDELT2", "", "deg", "Coordinate increment at reference point"},
      {"CUNIT1", "deg", "", "Unit of the first coordinate value"},
      {"CUNIT2", "deg", "", "Unit of the second coordinate value"},
      {"CTYPE1", "RA---TAN", "", "Right ascension, gnomonic projection"},
      {"CTYPE2", "DEC--TAN", "", "Declination, gnomonic projection"},
      {"CRVAL1", 0, "deg", "Coordinate value at reference point"},
      {"CRVAL2", 0, "deg", "Coordinate value at reference point"},
      {"LONPOLE", "", "deg", "Native longitude of celestial pole"},
      {"LATPOLE", "", "deg", "Native latitude of celestial pole"},
      {"RADESYS", "", "", "Equatorial coordinate system"},
      {"EQUINOX", "", "", "Equinox of celestial coordinate system (e.g. 2000)"}};
  header.write_n(records);
}

/**
 * @brief Append a 2D-MASS-like image extension to a file.
 * @details
 * A random 3D raster is generated and written.
 */
void write_image(const std::string& filename, const Linx::Position<3>& shape)
{
  MefFile f(filename, FileMode::Overwrite);
  Test::RandomRaster<float, 3> raster(shape, 0.F, 1.F);
  const auto& ext = f.append_image("KAPPA_PATCH", {}, raster); // Named extension
  write_some_records(ext.header());
}

int main(int argc, char const* argv[])
{
  Linx::ProgramOptions options("Generate random 2DMASS-like outputs.");
  options.named<std::string>("bintable", "Output binary table file", "/tmp/bintable.fits");
  options.named<Linx::Index>("rows", "Binary table row count", 10);
  options.named<std::string>("image", "Output image file", "/tmp/image.fits");
  options.named<Linx::Index>("width", "Image width", 10);
  options.named<Linx::Index>("height", "Image height", 10);
  options.parse(argc, argv);

  Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerate2DMassFiles");

  const std::string bintable = options.as<std::string>("bintable");
  const Linx::Index rows = options.as<Linx::Index>("rows");
  const std::string image = options.as<std::string>("image");
  const Linx::Position<3> shape {options.as<Linx::Index>("width"), options.as<Linx::Index>("height"), 3};

  logger.info("Writing binary table...");
  write_bintable(bintable, rows);
  logger.info("Done.");

  logger.info("Writing image...");
  write_image(image, shape);
  logger.info("Done.");

  logger.info("Reading binary table...");
  MefFile b(bintable, FileMode::Read);
  const auto some_column = b.access<BintableHdu>(1).read_column<float>("SHE_LENSMC_G1");
  logger.info() << "First value of SHE_LENSMC_G1 = " << some_column.container()[0];

  logger.info("Reading image...");
  MefFile i(image, FileMode::Read);
  const auto& ext = i.find<ImageHdu>("KAPPA_PATCH");
  const auto raster = ext.read_raster<float, 3>();
  const Linx::Position<3> center {raster.length<0>() / 2, raster.length<1>() / 2, raster.length<2>() / 2};
  logger.info() << "Central pixel = " << raster[center];

  logger.info("Reading header...");
  const auto records = ext.header().parse_all();
  const auto int_record = records.as<int>("CRVAL1");
  logger.info() << int_record.comment << " = " << int_record.value << " " << int_record.unit;
  const auto str_record = records.as<std::string>("CUNIT1");
  logger.info() << str_record.comment << " = " << str_record.value << " " << str_record.unit;

  logger.info("The end!");
  return 0;
}
