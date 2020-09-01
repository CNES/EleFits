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

void writeMeta(MefFile& f, int obj_index) {
  std::string extname = std::to_string(obj_index) + "_META";
  const auto& ext = f.init_image_ext<unsigned char, 1>(extname, {0});
  ext.writeRecords<int, int, float, float>(
      { "DITH_NUM", 0 }, //TODO
      { "SOURC_ID", obj_index },
      { "RA_OBJ", float(2 * obj_index) },
      { "DEC_OBJ", float(3 * obj_index) }
  );
}

void writeCombinedSignal(MefFile& f, int obj_index, int bins) {
  auto wmin_data = Test::generateRandomVector<float>(bins);
  auto signal_data = Test::generateRandomVector<float>(bins);
  auto quality_data = Test::generateRandomVector<char>(bins);
  auto var_data = Test::generateRandomVector<float>(bins);
  const long repeat = 1; //TODO bins?
  VecRefColumn<float> wmin_col( { "WMIN", "nm", repeat }, wmin_data);
  VecRefColumn<float> signal_col( { "SIGNAL", "erg", repeat }, signal_data);
  VecRefColumn<char> quality_col( { "QUALITY", "", repeat }, quality_data);
  VecRefColumn<float> var_col( { "VAR", "erg^2", repeat }, var_data);
  std::string extname = std::to_string(obj_index) + "_COMBINED1D_SIGNAL";
  const auto& ext = f.assign_bintable_ext(extname, wmin_col, signal_col);
  ext.appendColumn(quality_col);
  ext.appendColumn(var_col);
  ext.writeRecords<float, float, int, float>(
      { "WMIN", 0.F },
      { "BINWIDTH", 1.F },
      { "BINCOUNT", bins },
      { "EXPTIME", 3600.F }
  );
}

void writeCombinedCov(MefFile& f, int obj_index, int bins) {
  Test::RandomRaster<float, 2> cov_raster({bins, bins});
  std::string extname = std::to_string(obj_index) + "_COMBINED1D_COV";
  const auto& ext = f.assign_image_ext(extname, cov_raster);
  ext.writeRecords<int, std::string>(
      { "COV_SIDE", bins },
      { "CODEC", "IDENTITY" }
  );
}

void writeCombined(MefFile& f, int obj_index, int bins) {
  writeCombinedSignal(f, obj_index, bins);
  writeCombinedCov(f, obj_index, bins);
}

void writeAstroObj(MefFile& f, int obj_index, int bins) {
  writeMeta(f, obj_index);
  writeCombined(f, obj_index, bins);
}

class EL_FitsIO_GenerateAstroObj : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
  
    options_description options {};
    options.add_options()
        ("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file")
        ("nobj", value<int>()->default_value(1), "AstroObj count")
        ("nbin", value<int>()->default_value(1000), "Wavelength bin count");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_GenerateAstroObj");

    std::string filename = args["output"].as<std::string>();
    int nobj = args["nobj"].as<int>();
    int nbin = args["nbin"].as<int>();

    logger.info() << "Creating Fits file: " << filename;
    MefFile f(filename, MefFile::Permission::OVERWRITE);
    logger.info() << "Writing metadata";
    const auto& primary = f.access_primary<>();
    primary.writeRecord("N_OBJ", nobj);

    for(int i=0; i<nobj; ++i) {
      logger.info() << "Writing AstroObj " << i;
      writeAstroObj(f, i, nbin);
    }
    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_GenerateAstroObj)
