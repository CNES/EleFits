/**
 * @file src/program/EL_FitsIO_Generate2DMassFiles.cpp
 * @date 09/18/20
 * @author user
 *
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

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_FitsFile/MefFile.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using namespace FitsIO;

/**
 * @brief Generate a random scalar column without unit.
 */
template <typename T>
VecColumn<T> randomColumn(const std::string &name, long rows) {
  return VecColumn<T>({ name, "", 1 }, Test::generateRandomVector<T>(rows));
}

/**
 * @brief Append a 2D-MASS-like bintable extension to a file.
 */
void writeBintable(const std::string &filename, long rows) {
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
 * @brief Append a 2D-MASS-like bintable extension to a file.
 */
void writeImage(const std::string &filename, const Position<3> &shape) {
  MefFile f(filename, MefFile::Permission::Overwrite);
  Test::RandomRaster<float, 3> raster(shape); // Random raster with given shape
  f.assignImageExt("KAPPA_PATCH", raster);
}

class EL_FitsIO_Generate2DMassFiles : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {

    options_description options {};
    auto add = options.add_options();
    add("bintable", value<std::string>()->default_value("/tmp/bintable.fits"), "Output bintable file");
    add("rows", value<long>()->default_value(10), "Bintable row count");
    add("image", value<std::string>()->default_value("/tmp/image.fits"), "Output image file");
    add("width", value<long>()->default_value(10), "Image width");
    add("height", value<long>()->default_value(10), "Image height");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Generate2DMassFiles");

    const std::string bintable = args["bintable"].as<std::string>();
    const long rows = args["rows"].as<long>();
    const std::string image = args["image"].as<std::string>();
    const Position<3> shape { args["width"].as<long>(), args["height"].as<long>(), 3 };

    logger.info("Writing bintable...");
    writeBintable(bintable, rows);
    logger.info("Writing image...");
    writeImage(image, shape);

    logger.info("Done.");

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_Generate2DMassFiles)
