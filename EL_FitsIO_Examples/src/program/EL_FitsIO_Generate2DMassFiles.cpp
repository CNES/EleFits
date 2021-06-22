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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsFile/MefFile.h"
#include "EL_FitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;
using namespace FitsIO;

/**
 * @brief Generate a random scalar column without unit.
 */
template <typename T>
VecColumn<T> randomColumn(const std::string& name, long rows) {
  return VecColumn<T>({ name, "", 1 }, Test::generateRandomVector<T>(rows, T(0), T(1)));
}

/**
 * @brief Append a 2D-MASS-like binary table extension to a file.
 * @details
 * Random columns of type double ('D') and float ('E') are generated and written.
 */
void writeBintable(const std::string& filename, long rows) {
  MefFile f(filename, MefFile::Permission::Overwrite);
  const auto col1 = randomColumn<double>("SHE_LENSMC_UPDATED_RA", rows);
  const auto col2 = randomColumn<double>("SHE_LENSMC_UPDATED_DEC", rows);
  const auto col3 = randomColumn<float>("SHE_LENSMC_G1", rows);
  const auto col4 = randomColumn<float>("SHE_LENSMC_G2", rows);
  const auto col5 = randomColumn<float>("PHZ_MEDIAN", rows);
  const auto col6 = randomColumn<float>("PHZ_LENSMC_CORRECTION", rows);
  const auto col7 = randomColumn<float>("SHE_LENSMC_WEIGHT", rows);
  f.assignBintableExt("", col1, col2, col3, col4, col5, col6, col7); // Unnamed extension
}

/**
 * @brief Write some records to given HDU.
 * @details
 * WCS shows examples of records of different types (int and string), with and without units.
 * We rely on VariantValue, but it would be possible to skip this abstraction and go with raw types
 * using a tuple instead of a vector.
 */
void writeSomeRecords(const RecordHdu& hdu) {
  std::vector<Record<VariantValue>> records = {
    { "WCSAXES", 2, "", "Number of axes in World Coordinate System" },
    { "CRPIX1", "", "", "Pixel coordinate of reference point" },
    { "CRPIX2", "", "", "Pixel coordinate of reference point" },
    { "PC1_1", 0, "", "Coordinate transformation matrix element" },
    { "PC1_2", 0, "", "Coordinate transformation matrix element" },
    { "PC2_1", 0, "", "Coordinate transformation matrix element" },
    { "PC2_2", 0, "", "Coordinate transformation matrix element" },
    { "CDELT1", "", "deg", "Coordinate increment at reference point" },
    { "CDELT2", "", "deg", "Coordinate increment at reference point" },
    { "CUNIT1", "deg", "", "Unit of the first coordinate value" },
    { "CUNIT2", "deg", "", "Unit of the second coordinate value" },
    { "CTYPE1", "RA---TAN", "", "Right ascension, gnomonic projection" },
    { "CTYPE2", "DEC--TAN", "", "Declination, gnomonic projection" },
    { "CRVAL1", 0, "deg", "Coordinate value at reference point" },
    { "CRVAL2", 0, "deg", "Coordinate value at reference point" },
    { "LONPOLE", "", "deg", "Native longitude of celestial pole" },
    { "LATPOLE", "", "deg", "Native latitude of celestial pole" },
    { "RADESYS", "", "", "Equatorial coordinate system" },
    { "EQUINOX", "", "", "Equinox of celestial coordinate system (e.g. 2000)" }
  };
  hdu.writeRecords(records);
}

/**
 * @brief Append a 2D-MASS-like image extension to a file.
 * @details
 * A random 3D raster is generated and written.
 */
void writeImage(const std::string& filename, const Position<3>& shape) {
  MefFile f(filename, MefFile::Permission::Overwrite);
  Test::RandomRaster<float, 3> raster(shape, 0.F, 1.F);
  const auto& ext = f.assignImageExt("KAPPA_PATCH", raster); // Named extension
  writeSomeRecords(ext);
}

class EL_FitsIO_Generate2DMassFiles : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::FitsIO::ProgramOptions options("Generate random 2DMASS-like outputs.");
    options.named("bintable", value<std::string>()->default_value("/tmp/bintable.fits"), "Output binary table file");
    options.named("rows", value<long>()->default_value(10), "Binary table row count");
    options.named("image", value<std::string>()->default_value("/tmp/image.fits"), "Output image file");
    options.named("width", value<long>()->default_value(10), "Image width");
    options.named("height", value<long>()->default_value(10), "Image height");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Generate2DMassFiles");

    const std::string bintable = args["bintable"].as<std::string>();
    const long rows = args["rows"].as<long>();
    const std::string image = args["image"].as<std::string>();
    const Position<3> shape { args["width"].as<long>(), args["height"].as<long>(), 3 };

    logger.info("Writing binary table...");
    writeBintable(bintable, rows);
    logger.info("Done.");

    logger.info("Writing image...");
    writeImage(image, shape);
    logger.info("Done.");

    logger.info("Reading binary table...");
    MefFile b(bintable, MefFile::Permission::Read);
    const auto someColumn = b.access<BintableHdu>(1).readColumn<float>("SHE_LENSMC_G1");
    logger.info() << "First value of SHE_LENSMC_G1 = " << someColumn.vector()[0];

    logger.info("Reading image...");
    MefFile i(image, MefFile::Permission::Read);
    const auto& ext = i.accessFirst<ImageHdu>("KAPPA_PATCH");
    const auto raster = ext.readRaster<float, 3>();
    const Position<3> center { raster.length<0>() / 2, raster.length<1>() / 2, raster.length<2>() / 2 };
    logger.info() << "Central pixel = " << raster[center];

    logger.info("Reading header...");
    const auto records = ext.parseAllRecords<VariantValue>();
    const auto intRecord = records.as<int>("CRVAL1");
    logger.info() << intRecord.comment << " = " << intRecord.value << " " << intRecord.unit;
    const auto strRecord = records.as<std::string>("CUNIT1");
    logger.info() << strRecord.comment << " = " << strRecord.value << " " << strRecord.unit;

    logger.info("The end!");
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_Generate2DMassFiles)
