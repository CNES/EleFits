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

void writeMeta(MefFile &f, int objIndex) {
  std::string extname = std::to_string(objIndex) + "_META";
  const auto &ext = f.initImageExt<unsigned char, 1>(extname, { 0 });
  ext.writeRecords<int, int, float, float>(
      { "DITH_NUM", 0 }, // TODO
      { "SOURC_ID", objIndex },
      { "RA_OBJ", float(2 * objIndex) },
      { "DEC_OBJ", float(3 * objIndex) });
}

void writeCombinedSignal(MefFile &f, int objIndex, int bins) {
  auto wminData = Test::generateRandomVector<float>(bins);
  auto signalData = Test::generateRandomVector<float>(bins);
  auto qualityData = Test::generateRandomVector<char>(bins);
  auto varData = Test::generateRandomVector<float>(bins);
  const long repeat = 1; // TODO bins?
  VecRefColumn<float> wminCol({ "WMIN", "nm", repeat }, wminData);
  VecRefColumn<float> signalCol({ "SIGNAL", "erg", repeat }, signalData);
  VecRefColumn<char> qualityCol({ "QUALITY", "", repeat }, qualityData);
  VecRefColumn<float> varCol({ "VAR", "erg^2", repeat }, varData);
  std::string extname = std::to_string(objIndex) + "_COMBINED1D_SIGNAL";
  const auto &ext = f.assignBintableExt(extname, wminCol, signalCol);
  ext.appendColumn(qualityCol);
  ext.appendColumn(varCol);
  ext.writeRecords<float, float, int, float>(
      { "WMIN", 0.F },
      { "BINWIDTH", 1.F },
      { "BINCOUNT", bins },
      { "EXPTIME", 3600.F });
}

void writeCombinedCov(MefFile &f, int objIndex, int bins) {
  Test::RandomRaster<float, 2> cov_raster({ bins, bins });
  std::string extname = std::to_string(objIndex) + "_COMBINED1D_COV";
  const auto &ext = f.assignImageExt(extname, cov_raster);
  ext.writeRecords<int, std::string>({ "COV_SIDE", bins }, { "CODEC", "IDENTITY" });
}

void writeCombined(MefFile &f, int objIndex, int bins) {
  writeCombinedSignal(f, objIndex, bins);
  writeCombinedCov(f, objIndex, bins);
}

void writeAstroObj(MefFile &f, int objIndex, int bins) {
  writeMeta(f, objIndex);
  writeCombined(f, objIndex, bins);
}

class EL_FitsIO_GenerateAstroObj : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {

    options_description options {};
    options.add_options()("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file")(
        "nobj",
        value<int>()->default_value(1),
        "AstroObj count")("nbin", value<int>()->default_value(1000), "Wavelength bin count");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_GenerateAstroObj");

    std::string filename = args["output"].as<std::string>();
    int nobj = args["nobj"].as<int>();
    int nbin = args["nbin"].as<int>();

    logger.info() << "Creating Fits file: " << filename;
    MefFile f(filename, MefFile::Permission::Overwrite);
    logger.info() << "Writing metadata";
    const auto &primary = f.accessPrimary<>();
    primary.writeRecord("N_OBJ", nobj);

    for (int i = 0; i < nobj; ++i) {
      logger.info() << "Writing AstroObj " << i;
      writeAstroObj(f, i, nbin);
    }
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_GenerateAstroObj)
